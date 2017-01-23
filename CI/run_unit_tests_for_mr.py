import argparse, logging, requests, sys
from gitlab_mr_helpers import *

def run_unit_tests(args):
    # work around to get change set as long as we can't use the python API wrapper
    r = requests.get("https://gitlab.cern.ch/api/v3/projects/13363/merge_requests/%d/changes" % (args.mr_id),headers={"PRIVATE-TOKEN" : args.token})
    changes = r.json()['changes']
    changed_files = set([c[p] for c in changes for p in ['old_path','new_path']])
    logging.debug("changed files:\n" + print_collection(changed_files))
    affected_packages = sorted(set([map_filename_to_package(f) for f in changed_files]))

    # assemble ctest command
    ctest_cmd = "ctest "
    for p in affected_packages:
        # label is package name and not full package path
        ctest_cmd += "-L " + os.path.basename(p) + " "

    # execute
    logging.debug("ctest command = '%s'" % ctest_cmd)
    os.system(ctest_cmd)

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
                        level=eval("logging." + args.verbose))

    # delegate
    run_unit_tests(args)

if __name__ == "__main__":
    main()
