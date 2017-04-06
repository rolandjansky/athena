import argparse, gitlab, logging, re, sys
from gitlab.exceptions import GitlabGetError
from gitlab_mr_helpers import print_collection, list_changed_packages, list_affected_domains
from watch_list import WATCH_LIST

def comment_affected_packages(packages):
    """
    prepare initial comment to merge request listing affected packages

    packages ... list of affected packages

    return: comment text
    """
    n_packages = len(packages)
    if n_packages == 0:
        comment = "This merge request affects no known packages. Consult an expert!"
    elif n_packages == 1:
        comment = "This merge request affects 1 package:  \n- " + list(packages)[0]
    elif n_packages <= 20:
        comment = "This merge request affects %d packages:  \n- " % n_packages
        comment += "  \n- ".join(sorted(packages))
    else:
        comment = "This merge request affects %d packages. Since this is a long list, I will not print it here." % n_packages

    return comment

def warn_unmapped_packages(packages):
    """
    prepare warning for packages which could not be mapped to domains

    packages ... set of packages which could not be mapped

    return: comment text
    """
    if not packages:
        return ""

    comment = "  \n  \nCould not map the following packages to a domain:  \n  \n"
    for p in packages:
        comment += "- %s  \n" % p
    return comment

def add_watchers(packages):
    """
    prepare comment to add watchers for the modified packages

    packages ... set of affected packages

    return: comment text
    """
    watchers = set()
    for pkg in packages:
        for pattern,users in WATCH_LIST.items():
            if re.search(pattern,pkg):
                logging.debug("package '%s' matches watch list pattern '%s'" % (pkg,pattern))
                logging.debug("subscribing users:\n" + print_collection(users))
                watchers.update(users)

    logging.debug("list of watchers for this MR:\n" + print_collection(watchers))

    if watchers:
        watchers = ['@' + w for w in watchers]
        comment = "  \n  \nAdding "
        comment += " ,".join(watchers)
        comment += " as watcher%s" % ('' if len(watchers) == 1 else 's')
    else:
        comment = ""

    return comment

def add_labels(mr,domains):
    """
    add review and domain labels to merge request

    mr      ... Gitlab merge request object
    domains ... set of affected domains

    return: updated list of labels
    """
    # labels assigned during MR creation
    labels = set(mr.labels)
    logging.debug("labels assigned during MR creation:\n" + print_collection(labels))
    # assign labels for all affected domains
    for d in domains:
        labels.add(d)
    # remove possible complete review labels
    for label in list(labels):
        if re.match("review-",label):
            labels.discard(label)
    # add review-pending flag
    labels.add("review-pending-level-1")
    # add label for target branch
    labels.add(mr.target_branch)
    logging.debug("updated labels:\n" + print_collection(labels))
    mr.labels = list(labels)
    mr.save()

    return labels

def handle_new_merge_request(args):
    """
    handle new merge request in Gitlab

    This involves the following steps:
    - get a list of packages affected by these changes
    - post a comment about the affected packages
    - determine domain experts and mention those in a comment
    - assign labels for sign-off process by domain experts
    """
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

    handled_mr_states = ["opened","reopened","merged"]
    if not mr.state in handled_mr_states:
        logging.debug("ignore merge request in '%s' state" % mr.state)
        sys.exit(0)

    # get list of affected packages
    affected_packages = filter(None,list_changed_packages(mr))
    # get list of domains
    affected_domains,unmapped_pkgs = list_affected_domains(affected_packages)
    # add CI as fallback check if no responsible domain was found
    if not affected_domains:
        logging.debug("no responsible domain found -> please check that this is expected")
        affected_domains = set(['no-domain'])
    # assemble atlasbot comment
    comment = ""
    # add comment listing affected packages
    comment += comment_affected_packages(affected_packages)
    # warn about unmapped packages
    comment += warn_unmapped_packages(unmapped_pkgs)
    # add watchers for affected packages
    comment += add_watchers(affected_packages)

    # publish comment
    logging.debug("add comment to merge request:\n\n" + comment + "\n")
    mr.notes.create({'body':comment})

    # add labels
    add_labels(mr,affected_domains)

def main():
    parser = argparse.ArgumentParser(description="GitLab merge request handler",formatter_class=argparse.ArgumentDefaultsHelpFormatter)
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

    # delegate
    handle_new_merge_request(args)

if __name__ == "__main__":
    main()
