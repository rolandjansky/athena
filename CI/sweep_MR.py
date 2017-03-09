import argparse, gitlab, logging, os, re, subprocess, sys, yaml
from gitlab.exceptions import GitlabGetError, GitlabCreateError

def execute_command_with_retry(cmd,max_attempts=3):
    logging.debug("running command '%s' with max attempts %d",cmd,max_attempts)
    attempt = 0
    while attempt < max_attempts:
        attempt += 1
        logging.debug('running attempt %d',attempt)
        process = subprocess.Popen(cmd,stdout=subprocess.PIPE,stderr=subprocess.PIPE,shell=True)
        out,err = process.communicate()
        status  = process.returncode
        out = out.strip()
        err = err.strip()
        logging.debug('command returned %d',status)
        if out:
            logging.debug('stdout:\n%s',out)
        if err:
            logging.debug('stderr:\n%s',err)

        # break loop if execution was successfull
        if status == 0:
            break
        
    return status,out,err
    
def get_list_of_merge_commits(branch,since):
    logging.info("looking for merge commits on '%s' since '%s'",branch,since)
    git_cmd = 'git log --merges --first-parent --oneline --since="{0}" {1}'.format(since,branch)
    status,out,_ = execute_command_with_retry(git_cmd)

    # bail out in case of errors
    if status != 0:
        logging.critical("failed to retrieve merge commits")
        return None
    
    # extract hashes of merge commits
    hash_list = set()
    for line in out.split('\n'):
        # skip empty lines
        if not line:
            continue
        match = re.match(r"[0-9a-f]{6,}",line)
        if not match:
            logging.critical("could not extract merge commit hash from '%s'",line)
            continue
        hash_list.add(match.group())
    logging.info("found %d merge commits",len(hash_list))
    logging.debug("hashes : %s",repr(hash_list))
    
    return hash_list

def get_sweep_target_branches(src_branch):
    git_cmd = "git show {0}:CI/config.yaml".format(src_branch)
    status,out,_ = execute_command_with_retry(git_cmd)
    
    # bail out in case of errors
    if status != 0:
        logging.critical("failed to retrieve CI configuration")
        return None

    try:
        CI_config = yaml.load(out)
    except:
        logging.critical("failed to interpret the following text as YAML:\n%s",out)
        return None

    if not 'sweep-targets' in CI_config or not CI_config['sweep-targets']:
        logging.info("no sweep targets for branch '%s' configured",src_branch)
        return None

    target_branches = CI_config['sweep-targets']
    logging.info("MRs to '%s' are swept to %d branches",src_branch,len(target_branches))
    logging.debug("sweep target branches: %s",repr(target_branches))
    
    return target_branches

def cherry_pick_mr(merge_commit,source_branch,target_branches,remote_name,project):
    # keep track of successful and failed cherry-picks
    good_branches = set()
    failed_branches = set()

    # remember current git reference
    status,HEAD,_ = execute_command_with_retry("git rev-parse HEAD")
    logging.debug("current HEAD is at '%s'",HEAD)

    # bail out in case of errors
    if status != 0:
        logging.critical("failed to parse current git reference")
        return

    # get merge request IID from commit message
    _,out,_ = execute_command_with_retry("git show {0}".format(merge_commit))
    match = re.search("See merge request !(\d+)",out)
    if match:
        MR_IID = int(match.group(1))
        logging.debug("merge commit '%s' corresponds to MR IID %d",merge_commit,MR_IID)

        # retrieve gitlab API object
        try:
            mr_handle = project.mergerequests.list(iid=MR_IID)[0]
        except:
            logging.critical("failed to retrieve Gitlab merge request handle")
            return
        if not mr_handle:
            logging.critical("failed to retrieve Gitlab merge request handle")
            return
        else:
            logging.debug("retrieved Gitlab merge request handle")
    else:
        logging.critical("failed to determine MR IID")
        return

    # add sweep label
    labels = set(mr_handle.labels)
    if "sweep:done" in labels:
        logging.info("merge commit '%s' was already swept -> skipping",merge_commit)
        return
    if "sweep:ignore" in labels:
        logging.info("merge commit '%s' is marked as ignore -> skipping",merge_commit)
        return

    labels.add("sweep:done")
    mr_handle.labels = ",".join(labels)
    mr_handle.save()

    # get initial MR commit title and description
    _,mr_title,_ = execute_command_with_retry('git show {0} --pretty=format:"%s"'.format(merge_commit))
    _,mr_desc,_ = execute_command_with_retry('git show {0} --pretty=format:"%b"'.format(merge_commit))
    
    # perform cherry-pick to all target branches
    for branch in target_branches:
        failed = False
        remote_branch = os.path.join(remote_name,branch)
        cherry_pick_branch = "cherry-pick-{0}-{1}".format(merge_commit,branch)
        logging.debug("cherry-pick '%s' into '%s'",merge_commit,remote_branch)
        git_cmds = ["git checkout -f -b {0} {1} --no-track".format(cherry_pick_branch,remote_branch),
                    "git cherry-pick --allow-empty -m 1 {0}".format(merge_commit),
                    "git push {0} {1}".format(remote_name,cherry_pick_branch)]

        # perform the actual cherry-pick
        for cmd in git_cmds:
            status,_,_ = execute_command_with_retry(cmd)
            if status != 0:
                failed = True
                break
            
        # clean up local cherry-pick branch
        # if it fails, do not panic
        execute_command_with_retry("git checkout -f {0} && git branch -D {1}".format(HEAD,cherry_pick_branch))
        
        if failed:
            logging.critical("failed to cherry-pick '%s' into '%s'",merge_commit,branch)
            failed_branches.add(branch)
        else:
            logging.info("cherry-picked '%s' into '%s'",merge_commit,branch)
            good_branches.add(branch)

            # create merge request
            mr_data = {}
            mr_data['source_branch'] = cherry_pick_branch
            mr_data['target_branch'] = branch
            mr_data['title'] = mr_title
            mr_data['description'] = mr_desc
            mr_data['labels'] = "sweep:from {0}".format(os.path.basename(source_branch))
            project.mergerequests.create(mr_data)

    # compile comment about sweep results
    comment = "**Sweep summary**  \n"
    if good_branches:
        comment += "successful: " + ", ".join(sorted(good_branches)) + "  \n"
    if failed_branches:
        comment += "failed: " + ", ".join(sorted(failed_branches))
    mr_handle.notes.create({'body': comment})
    
def main():
    parser = argparse.ArgumentParser(description="GitLab merge request commentator",formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("-b","--branch",required=True,help="remote branch whose merge commits should be swept (e.g. origin/master)")
    parser.add_argument("-p","--project-name",dest="project_name",required=True,help="GitLab project with namespace (e.g. user/my-project)")
    parser.add_argument("-s","--since",default="1 day",help="time interval for sweeping MR (e.g. 1 week)")
    parser.add_argument("-t","--token",required=True,help="private GitLab user token")
    parser.add_argument("-u","--url",default="https://gitlab.cern.ch",help="URL of GitLab instance")
    parser.add_argument("-v","--verbose",default="INFO",choices=["DEBUG","INFO","WARNING","ERROR","CRITICAL"],help="verbosity level")
    parser.add_argument("--repository-root",dest="root",default="",help="path to root directory of git repository")

    # get command line arguments
    args = parser.parse_args()

    # configure log output
    logging.basicConfig(format='%(asctime)s %(levelname)-10s %(message)s',
                        datefmt='%H:%M:%S',
                        level=logging.getLevelName(args.verbose))

    logging.debug("parsed arguments:\n" + repr(args))

    # we only support porting merge commits from remote branches since we expect
    # them to be created through the Gitlab web interface
    # -> branch must contain the name of the remote repository (e.g. upstream/master)
    # -> infer it
    tokens = args.branch.split('/')
    if len(tokens) < 2:
        logging.critical("expect branch to specify a remote branch (e.g. 'upstream/master')")
        logging.critical("received branch '%s' which does not look like a remote branch",args.branch)
        logging.critical("--> aborting")
        sys.exit(1)

    # set name of remote repository
    args.remote_name = tokens[0]
    
    # get GitLab API handler
    gl = gitlab.Gitlab(args.url,args.token)
    try:
        # get Gitlab project object
        project = gl.projects.get(args.project_name)
        logging.debug("retrieved Gitlab project handle")
    except GitlabGetError as e:
        logging.critical("error communication with Gitlab API '%s'" % (e.error_message))
        sys.exit(1)

    # get top-level directory of git repository (specific to current directory structure)
    if not args.root:
        workdir = os.path.dirname(os.path.abspath(os.path.join(os.path.realpath(__file__),'../')))
    else:
        workdir = os.path.abspath(args.root)
        
    logging.info("changing to root directory of git repository '%s'",workdir)
    current_dir = os.getcwd()
    os.chdir(workdir)

    # fetch latest changes
    status,_,_ = execute_command_with_retry("git fetch --prune {0}".format(args.remote_name))
    if status != 0:
        logging.critical("failed to fetch from '%s'",args.remote_name)
        return None

    # get list of branches MRs should be forwarded to
    target_branches = get_sweep_target_branches(args.branch)
    if not target_branches:
        logging.info("no sweep targets for branch '%s' found",args.branch)
        sys.exit(0)

    # get list of MRs in relevant period
    MR_list = get_list_of_merge_commits(args.branch,args.since)
    if not MR_list:
        logging.info("no MRs to '%s' found during last %s",args.branch,args.since)
        sys.exit(0)

    # do the actual cherry-picking
    for mr in MR_list:
        cherry_pick_mr(mr,args.branch,target_branches,args.remote_name,project)
    # change back to initial directory
    os.chdir(current_dir)

if __name__ == "__main__":
    main()
