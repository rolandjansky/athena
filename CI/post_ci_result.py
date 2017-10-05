import argparse, gitlab, logging, sys
from gitlab.exceptions import GitlabGetError, GitlabCreateError

def compose_result_text(status):
    """
    generate comment line describing global result of CI job

    status ... Jenkins exit code as string
    """
    text = ":question: **CI Result UNSET**"
    if status == "SUCCESS":
        text = ":white_check_mark: **CI Result SUCCESS**"
    elif status == "FAILURE":
        text = ":negative_squared_cross_mark: **CI Result FAILURE**"
    elif status == "ABORT":
        text = ":point_up: **CI Result ABORTED**"

    return text

def compose_stage_text(stage,result):
    """
    generate comment line describing result of CI job stage

    result ... Jenkin exit code as string
    """
    if result == "SUCCESS":
        text = ":white_check_mark: "
    elif result == "FAILURE":
        text = ":o: "
    # everything else means that this stage was not run
    else:
        text = ":white_circle: "
        
    return text + stage
        
def main():
    parser = argparse.ArgumentParser(description="GitLab merge request commentator",formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("--externals",required=True,type=str,help="result of external building step")
    parser.add_argument("--cmake",required=True,type=str,help="result of cmake configuration step")
    parser.add_argument("--make",required=True,type=str,help="result of build step")
    parser.add_argument("--test",required=True,type=str,help="result of test step")
    parser.add_argument("--status",required=True,choices=["SUCCESS","FAILURE","ABORT"],help="final result of CI job")
    parser.add_argument("-i","--ci-job-id",dest="ci_id",required=True,type=int,help="ID of CI job")    
    parser.add_argument("-m","--merge-request-id",dest="mr_id",required=True,type=int,help="(unique) ID of merge request (not the project specific IID)")
    parser.add_argument("-n","--nicos-project-relname",dest="nicos_name",required=True,type=str,help="NICOS project release name for this build")
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

    # prepare individual parts of the CI comment
    comment = compose_result_text(args.status)
    comment += "  \n"
    comment += compose_stage_text("externals",args.externals) + "  \n"
    comment += compose_stage_text("cmake",args.cmake) + "  \n"
    comment += compose_stage_text("make",args.make) + "  \n"
    comment += compose_stage_text("test",args.test) + "  \n"
    comment += "  \n"
    comment += "Full details available at [NICOS {0}](http://atlas-nightlies-browser.cern.ch/~platinum/nightlies/info?tp=g&nightly=MR-CI-builds&rel={0}&ar=*)  \n".format(args.nicos_name)
    comment += "For experts only: Jenkins output [[CI-MERGE-REQUEST {0}]](http://aibuild080.cern.ch:8080/job/CI-MERGE-REQUEST/{0}/) (for [remote](http://localhost:9999/job/CI-MERGE-REQUEST/{0}/) access see the [FAQ](https://atlassoftwaredocs.web.cern.ch/guides/MR_reviewers/#faq))".format(args.ci_id)

    logging.debug(comment)
    try:
        mr.notes.create({'body':comment})
    except GitlabCreateError as e:
        logging.critical("error creating MR comment '%s'" % (e.error_message))
        sys.exit(1)

if __name__ == "__main__":
    main()
