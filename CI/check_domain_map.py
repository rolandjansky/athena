import argparse, logging, re, subprocess
#from gitlab.exceptions import GitlabGetError
from gitlab_mr_helpers import map_filename_to_package
from domain_map import DOMAIN_MAP

def main():
    parser = argparse.ArgumentParser(description="GitLab merge request handler",formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("-p","--path_to_repo",dest="repo_path",default="/home/jchapman/cleanup/athena",help="path to local clone of repository")
    parser.add_argument("-d","--domains_to_print",default=DOMAIN_MAP.keys(),help="Domains to print out")
    parser.add_argument("-v","--verbose",default="INFO",choices=["DEBUG","INFO","WARNING","ERROR","CRITICAL"],help="verbosity level")

    # get command line arguments
    args = parser.parse_args()

    # configure log output
    logging.basicConfig(format='%(asctime)s %(levelname)-10s %(message)s',
                        datefmt='%H:%M:%S',
                        level=logging.getLevelName(args.verbose))

    logging.debug("parsed arguments:\n" + repr(args))
    #TODO Should check that args.repo_path is a valid directory first for security
    result = subprocess.Popen("find %s -type d" % args.repo_path, shell=True, stdout=subprocess.PIPE).stdout.read()
    dirlist = result.split('\n')
    affected_packages = filter(None,sorted(set([map_filename_to_package(f) for f in dirlist])))
    affected_packages = [package.replace(args.repo_path, "") for package in affected_packages]

    #here we want to list which packages match each domain.
    packages_per_domain = dict()
    for domain,pattern_list in DOMAIN_MAP.items():
        domain_packages=[]
        for pkg in affected_packages:
            for pattern in pattern_list:
                if re.search(pattern,pkg,re.I):
                    domain_packages+=[pkg]
                    break
        packages_per_domain[domain] = domain_packages

    for domain in args.domains_to_print:
        logging.info("--------------------------------")
        logging.info("Domain: " + domain)
        logging.info("--------------------------------")
        for pkg in packages_per_domain[domain]:
            logging.info(pkg)
            pass
        pass

if __name__ == "__main__":
    main()
