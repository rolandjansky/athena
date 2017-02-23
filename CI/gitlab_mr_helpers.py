import logging, os, re
from domain_map import DOMAIN_MAP

DOMAINS = DOMAIN_MAP.keys()

def print_collection(c):
    return "                    - " + "\n                    - ".join(c)

def map_filename_to_package(fname):
    """
    fname ... full path of filename

    note: only works if this file resides in <ATHENA_ROOT>/CI

    return: package path for given full file path
    """
    # get Athena root directory (specific to current layout) which is one level up
    athena_root = os.path.dirname(os.path.abspath(os.path.join(os.path.realpath(__file__),'../')))
    logging.debug("found Athena root directory '%s'",athena_root)

    # start from directory name
    pkg_name = os.path.dirname(fname)

    # recursively move up in directory hierarchy and look for CMakeLists.txt
    while pkg_name and pkg_name != '/':
        if os.path.isfile(os.path.join(athena_root,pkg_name,'CMakeLists.txt')):
            break
        pkg_name = os.path.dirname(pkg_name)

    logging.debug("mapped file '%s' to package '%s'" % (fname,pkg_name))
    return pkg_name

def map_package_domain(pkg):
    """
    map the package name to a domain

    pkg ... package path

    return: domain(s) of package
    """
    domains = set()
    for domain,pattern_list in DOMAIN_MAP.items():
        for pattern in pattern_list:
            if re.search(pattern,pkg,re.I):
                domains.add(domain)
                break

    return domains

def list_changed_packages(mr):
    """
    mr ... Gitlab merge request object

    return: sorted set of package names affected by this merge request
    """
    changed_files = set([c[p] for c in mr.changes()['changes'] for p in ['old_path','new_path']])
    logging.debug("changed files:\n" + print_collection(changed_files))

    return sorted(set([map_filename_to_package(f) for f in changed_files]))

def list_affected_domains(packages):
    """
    packages ... set of Athena packages

    return: sorted set of domains responsible for the given packages
            and a set of all packages which could not be mapped
    """
    domains = set()
    unmapped_pkgs = set()
    for pkg in packages:
        this_domain = map_package_domain(pkg)
        logging.debug("mapped package '%s' to domain '%s'" % (pkg,this_domain))
        if not this_domain:
            unmapped_pkgs.add(pkg)
        else:
            domains.update(this_domain)
    logging.debug("affected domains:\n" + print_collection(domains))
    logging.debug("unmapped packages:\n" + print_collection(unmapped_pkgs))

    return domains,unmapped_pkgs
