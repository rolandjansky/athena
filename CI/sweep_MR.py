#!/usr/bin/env python
import argparse, gitlab, logging, os, re, subprocess, sys, yaml
from gitlab.exceptions import GitlabGetError, GitlabCreateError, GitlabCherryPickError
from gitlab_mr_helpers import list_changed_packages

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
    
def get_list_of_merge_commits(branch,since,until):
    logging.info("looking for merge commits on '%s' since '%s'",branch,since)
    git_cmd = 'git log --merges --first-parent --oneline --since="{0}" --until="{1}" {2}'.format(since,until,branch)
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

def get_sweep_target_branch_rules(src_branch):
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

    # get branch name without remote repository name
    branch_wo_remote = src_branch.split('/')[1]
    # make sure we have a valid sweeping configuration
    has_sweep_targets = 'sweep-targets' in CI_config
    has_targets_for_branch = has_sweep_targets and branch_wo_remote in CI_config['sweep-targets']
    has_target_rules = has_targets_for_branch and CI_config['sweep-targets'][branch_wo_remote]
    if not has_target_rules:
        return None

    target_branch_rules = CI_config['sweep-targets'][branch_wo_remote]
    logging.info("read %d sweeping rules for MRs to '%s",len(target_branch_rules),src_branch)
    logging.debug("sweeping rules: %r",target_branch_rules)
    
    return target_branch_rules

def find_original_mr_author(mr_iid,project):
    logging.debug("find_original_mr_author( %d )",mr_iid)
    try:
        mr_handle = project.mergerequests.list(iid=mr_iid)[0]
    except:
        logging.critical("failed to retrieve Gitlab merge request handle for iid = %d",mr_iid)
        return
    mr_author = mr_handle.author.username
    logging.debug("!%d author: %s",mr_iid,mr_author)
    if mr_author != "atnight":
        logging.debug("!%d: returning %s != atnight",mr_iid,mr_author)
        return mr_author
    logging.debug("Looking for !%d's parent MR",mr_iid)
    mr_title = mr_handle.title
    logging.debug("mr_title: %s",mr_title)
    parent_MR_IID = -1
    _s_ = mr_title.split('\n')
    for _subs_ in _s_:
        match = re.search('^Sweeping !(\d+)',_subs_)
        if match:
            parent_MR_IID = int(match.group(1))
            logging.debug("Parent MR: !%d",parent_MR_IID)
            break
    if parent_MR_IID != -1:
        mr_author = find_original_mr_author(parent_MR_IID,project)
    else:
        logging.warning("can't find !%d authors except atnight",mr_iid)
    return mr_author
        


def cherry_pick_mr(merge_commit,source_branch,target_branch_rules,project,dry_run=False):
    # keep track of successful and failed cherry-picks
    good_branches = set()
    failed_branches = set()

    # get merge commit object
    try:
        commit = project.commits.get(merge_commit)
    except GitlabGetError as e:
        logging.critical("failed to get merge commit '%s' with\n%s",merge_commit,e.error_message)
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
        else:
            logging.debug("retrieved Gitlab merge request handle")
    else:
        logging.critical("failed to determine MR IID")
        return

    # save original author so that we can add as watcher
    original_mr_author = mr_handle.author.username

    # handle sweep labels
    labels = set(mr_handle.labels)

    if "sweep:done" in labels:
        logging.info("merge commit '%s' was already swept -> skipping ......\n",merge_commit)
        return
    if "sweep:ignore" in labels:
        logging.info("merge commit '%s' is marked as ignore -> skipping .......\n",merge_commit)
        return

    target_branches = set()
    
    logging.debug("Looking through MR labels .... \n")

    for l in labels:
        logging.debug("label: %s",l)
        if re.match('^sweptFrom:',l):
            logging.info("merge commit '%s' contains %s label -> skipping to prevent sweeping it twice .......\n",merge_commit,l)
            return
        if re.match('^alsoTargeting:',l):
            _s_ = l.split(':')
            if len(_s_) == 2: # convert from unicode to have the same ascii string type as in target_branch_rules below
                    _ss_ = [ _s_[1].encode('ascii','replace') ] 
                    logging.info("merge commit '%s' also targets the following branches: %s -> add to target list",merge_commit,_ss_)
                    target_branches.update(_ss_)
            else:
                logging.warning("merge commit '%s' has empty 'alsoTargeting:' label -> ignore")


    # get list of affected packages for this MR
    affected_packages = list_changed_packages(mr_handle)
    logging.debug("MR %d affects the following packages: %r",MR_IID,affected_packages)

    # determine set of target branches from rules and affected packages
    for rule,branches in target_branch_rules.items():
        # get pattern expression for affected packages
        pkg_pattern = re.compile(rule)
        # only add target branches if ALL affected packages match the given pattern
        matches = [pkg_pattern.match(pkg_name) for pkg_name in affected_packages]
        if all(matches):
            logging.debug("add branches for rule '%s': %s",rule,branches)
            target_branches.update(branches)
        else:
            logging.debug("skip branches for rule '%s': %s",rule,branches)

    logging.info("MR %d [%s] is swept to %d branches: %r",MR_IID,merge_commit,len(target_branches),list(target_branches))

    if len(target_branches) == 0:
        labels.add("sweep:ignore")
        logging.debug("\n\nZero target branches found, adding sweep:ignore label to the MR handle\n\n")
    else:
        labels.add("sweep:done")

    if dry_run:
        logging.debug("\n\n----- This is a test run, stop with this MR here ----\n\n ")
        return

    mr_handle.labels = list(labels)
    mr_handle.save()

    # get initial MR commit title and description
    _,mr_title,_ = execute_command_with_retry('git show {0} --pretty=format:"%s"'.format(merge_commit))
    _,mr_desc,_ = execute_command_with_retry('git show {0} --pretty=format:"%b"'.format(merge_commit))

    _s_ = source_branch.split('/')
    if len(_s_) == 2:
        source_branch_strip = _s_[1]
    elif len(_s_) == 1:
        source_branch_strip = _s_[0]
    else:
        source_branch_strip = 'undefined branch'

    _s_ = mr_desc.split('\n')
    mr_desc_strip = ''
    for _subs_ in _s_:
        if not re.match('^See merge request ',_subs_):
            mr_desc_strip += _subs_

    if original_mr_author == "atnight":
        original_mr_author = find_original_mr_author(MR_IID, project)
        logging.debug("Taking MR author from the parent MR: %s",original_mr_author)

    _comment_1_ = 'Sweeping !' + str(MR_IID) + ' from ' + source_branch_strip + ' to '
    _comment_2_ = '.\n' + mr_desc_strip

    for _t_ in target_branches:
        logging.debug("MR title for target branch %s: '%s'", _t_, _comment_1_ + _t_ + _comment_2_)

    # perform cherry-pick to all target branches
    for tbranch in target_branches:
        failed = False

        # create remote branch for containing the cherry-pick commit
        cherry_pick_branch = "cherry-pick-{0}-{1}".format(merge_commit,tbranch)
        try:
            project.branches.create({'branch_name': cherry_pick_branch, 'ref': tbranch})
        except GitlabCreateError as e:
            logging.critical("failed to create remote branch '%s' with\n%s",cherry_pick_branch,e.error_message)
            failed = True
        else:
            # perform cherry-pick
            try:
                commit.cherry_pick(cherry_pick_branch)
            except GitlabCherryPickError as e:
                logging.critical("failed to cherry pick merge commit '%s' with\n%s",merge_commit,e.error_message)
                failed = True

        # only create MR if cherry-pick succeeded
        if failed:
            logging.critical("failed to cherry-pick '%s' into '%s'",merge_commit,tbranch)
            failed_branches.add(tbranch)
        else:
            logging.info("cherry-picked '%s' into '%s'",merge_commit,tbranch)

            _title_ = _comment_1_ + tbranch + _comment_2_
            # create merge request
            mr_data = {}
            mr_data['source_branch'] = cherry_pick_branch
            mr_data['target_branch'] = tbranch
            mr_data['title'] = _title_
            mr_data['description'] = mr_desc
            mr_data['labels'] = "sweep:from {0}".format(os.path.basename(source_branch))
            mr_data['remove_source_branch'] = True
            logging.debug("Sweeping MR %d to %s with a title: '%s'",MR_IID,tbranch,_title_)
            try:
                new_mr = project.mergerequests.create(mr_data)
            except GitlabCreateError as e:
                logging.critical("failed to create merge request for '%s' into '%s' with\n%s",cherry_pick_branch,tbranch,e.error_message)
                failed_branches.add(tbranch)
            else:
                good_branches.add(tbranch)
                # adding original author as watcher
                notification_text = "Adding original author @{0:s} as watcher.".format(original_mr_author)
                new_mr.notes.create({'body': notification_text})

    # compile comment about sweep results
    if len(target_branches) > 0:
        comment = "**Sweep summary**  \n"
        if good_branches:
            comment += "successful:  \n* " + "\n* ".join(sorted(good_branches)) + "  \n  \n"
        if failed_branches:
            comment += "failed:  \n* " + "\n* ".join(sorted(failed_branches))
            # add label to original MR indicating cherry-pick problem
            mr_handle.labels = list(set(mr_handle.labels) | {"sweep:failed"})
            mr_handle.save()

        # add sweep summary to MR in Gitlab
        try:
            mr_handle.notes.create({'body': comment})
        except GitlabCreateError as e:
            logging.critical("failed to add comment with sweep summary with\n{0:s}".format(e.error_message))

def main():
    parser = argparse.ArgumentParser(description="GitLab merge request commentator",formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("-b","--branch",required=True,help="remote branch whose merge commits should be swept (e.g. origin/master)")
    parser.add_argument("-d","--dry-run",dest="dry_run",action='store_true',help="only perform a test run without actually modifying anything")
    parser.add_argument("-p","--project-name",dest="project_name",required=True,help="GitLab project with namespace (e.g. user/my-project)")
    parser.add_argument("-s","--since",default="1 day ago",help="start of time interval for sweeping MR (e.g. 1 week ago)")
    parser.add_argument("-t","--token",required=True,help="private GitLab user token")
    parser.add_argument("-u","--until",default="now",help="end of time interval for sweeping MR (e.g. 1 hour ago)")
    parser.add_argument("--url",default="https://gitlab.cern.ch",help="URL of GitLab instance")
    parser.add_argument("-v","--verbose",default="INFO",choices=["DEBUG","INFO","WARNING","ERROR","CRITICAL"],help="verbosity level")
    parser.add_argument("--repository-root",dest="root",default=os.path.dirname(os.path.abspath(os.path.join(os.path.realpath(__file__),'../'))),help="path to root directory of git repository")

    # get command line arguments
    args = parser.parse_args()

    # configure log output
    logging.basicConfig(format='%(asctime)s %(levelname)-10s %(message)s',
                        datefmt='%H:%M:%S',
                        level=logging.getLevelName(args.verbose))

    logging.debug("parsed arguments:\n" + repr(args))

    if args.dry_run:
        logging.info("running in TEST mode")

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
    target_branch_rules = get_sweep_target_branch_rules(args.branch)
    if not target_branch_rules:
        logging.info("no sweeping rules for branch '%s' found",args.branch)
        sys.exit(0)

    # get list of MRs in relevant period
    MR_list = get_list_of_merge_commits(args.branch,args.since,args.until)
    if not MR_list:
        logging.info("no MRs to '%s' found in period from %s until %s",args.branch,args.since,args.until)
        sys.exit(0)

    # do the actual cherry-picking
    for mr in MR_list:
        logging.debug("\n\n ===== Next MR: %s ====== \n\n",mr)
        cherry_pick_mr(mr,args.branch,target_branch_rules,project,args.dry_run)

    # change back to initial directory
    os.chdir(current_dir)

if __name__ == "__main__":
    main()
