import argparse, gitlab, logging, os, subprocess, sys
from gitlab.exceptions import GitlabGetError
from gitlab_mr_helpers import print_collection, map_filename_to_package

def run_unit_tests(args):
    # get GitLab API handler
    gl = gitlab.Gitlab(args.url,args.token)
    try:
        # get Gitlab project object
        project = gl.projects.get(args.project_name)
        logging.debug("retrieved Gitlab project handle")
        # get Gitlab merge request object
        mr = project.mergerequests.get(args.mr_id)
        logging.debug("retrieved Gitlab merge request handle")
    except GitlabGetError as e:
        logging.critical("error communication with Gitlab API '%s'" % (e.error_message))
        sys.exit(1)

    changes = mr.changes()['changes']
    changed_files = set([c[p] for c in changes for p in ['old_path','new_path']])
    logging.debug("changed files:\n" + print_collection(changed_files))
    affected_packages = sorted(set([map_filename_to_package(f) for f in changed_files]))

    # assemble ctest command
    ctest_cmd = "ctest --output-on-failure "
    for p in affected_packages:
        # label is package name and not full package path
        ctest_cmd += "-L ^" + os.path.basename(p) + " "

    # execute
    logging.debug("ctest command = '%s'" % ctest_cmd)
    status = subprocess.call(ctest_cmd,shell=True)
    return status

def main():
    parser = argparse.ArgumentParser(description="ATLAS unit test runner",
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("-m","--merge-request-id",dest="mr_id",required=True,type=int,help="(unique) ID of merge request (not the project specific IID)")
    parser.add_argument("-p","--project-name",dest="project_name",required=True,help="GitLab project with namespace (e.g. user/my-project)")
    parser.add_argument("-t","--token",required=True,help="private GitLab user token")
    parser.add_argument("-u","--url",default="https://gitlab.cern.ch",help="URL of GitLab instance")
    parser.add_argument("-v","--verbose",default="INFO",choices=["DEBUG","INFO","WARNING","ERROR","CRITICAL"],help="verbosity level")

    # get command line arguments
    args = parser.parse_args()

    # configure log output
    logging.basicConfig(format='%(asctime)s %(levelname)-10s %(message)s',
                        datefmt='%H:%M:%S',
                        level=logging.getLevelName(args.verbose))

    # delegate
    status = run_unit_tests(args)
    sys.exit(status)

if __name__ == "__main__":
    main()
