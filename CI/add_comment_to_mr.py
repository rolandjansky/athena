import argparse, gitlab, logging, sys
from gitlab.exceptions import GitlabGetError, GitlabCreateError

def main():
    parser = argparse.ArgumentParser(description="GitLab merge request commentator",formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("comment",type=str,help="comment text to be added")
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

    logging.debug("parsed arguments:\n" + repr(args))

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

    try:
        mr.notes.create({'body':args.comment})
    except GitlabCreateError as e:
        logging.critical("error creating MR comment '%s'" % (e.error_message))
        sys.exit(1)

if __name__ == "__main__":
    main()
