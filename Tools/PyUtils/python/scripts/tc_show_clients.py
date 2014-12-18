# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file PyUtils.scripts.tc_show_clients
# @purpose show the clients of a package using TC-2
# @author Sebastien Binet
# @date May 2011

__version__ = "$Revision: 636803 $"
__doc__ = "show the clients of a package using TC-2"
__author__ = "Sebastien Binet"


### imports -------------------------------------------------------------------
import os
import PyUtils.acmdlib as acmdlib

@acmdlib.command(name='tc.show-clients')
@acmdlib.argument('pkg',
                  nargs='+',
                  help='(list of) package(s) to show clients of')
@acmdlib.argument('-r', '--release',
                  required=True,
                  help='the release in which to show the clients (e.g: 17.0.1)')
@acmdlib.argument('--co',
                  action='store_true',
                  default=False,
                  help='enable the checkout of these clients')
def main(args):
    """show the clients of a package using TC-2"""

    import PyUtils.AmiLib as amilib
    client = amilib.Client()

    pkgs = args.pkg
    if isinstance(pkgs, basestring):
        pkgs = [pkgs]

    from collections import defaultdict
    all_clients = defaultdict(list)
    
    for pkg in pkgs:
        print
        client.msg.info('showing clients of [%s]...', pkg)
        # find the project for this pkg
        projects = client.get_project_of_pkg(pkg, args.release)
        pkg = client.find_pkg(pkg, check_tag=False)
        fpkg = pkg['packagePath']+pkg['packageName']
        if len(projects) > 1:
            client.msg.info('pkg [%s] exists in more than 1 project: %s ==> will use last one' % (pkg['packageName'], projects))
        elif len(projects) < 1:
            continue
        project = projects[-1]
        clients = client.get_clients(project, args.release, fpkg)
        for full_name,v,rel,grp in clients:
            if full_name.startswith('Projects/'):
                # remove "meta" packages
                continue
            v = v.strip()
            if os.path.basename(full_name) == v:
                # filter out container packages
                continue
            if '-%s-'%project not in v:
                all_clients[full_name].append(v)
        #client.msg.info('        tag= [%s]', tag)

    _all_clients = dict(all_clients)
    all_clients = []
    for full_name in sorted(_all_clients.keys()):
        v = _all_clients[full_name]
        if len(v) > 1:
            versions = client.get_pkg_info(full_name, args.release, resultKey="packageTag")
            if len(versions) != 1:
                client.msg.info('found multiple versions for package [%s]: %r',
                                full_name, versions)
                v = versions
        print '%-40s' % v[0], full_name
        all_clients.append(v[0])
        
    rc = 0
    if args.co:
        print
        client.msg.info(":"*40)
        client.msg.info(":: list of package to checkout:")
        for c in all_clients:
            print c
        cmd = ['pkgco.py',]+all_clients
        import subprocess
        rc = subprocess.call(cmd)
    return rc

