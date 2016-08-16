# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file PyUtils.scripts.tc_submit_pkg
# @purpose Submit one or more TAGs to TagCollector.
# @author Sebastien Binet
# @date February 2010

__version__ = "$Revision: 766754 $"
__doc__ = "Submit one or more TAGs to TagCollector."
__author__ = "Sebastien Binet, Frank Winklmeier"


### imports -------------------------------------------------------------------
import readline
import getpass
import os
import os.path as osp
import sys

import PyUtils.acmdlib as acmdlib
try:
   import pyAMI.exception
except ImportError:
   pass #do nothing
import PyCmt.Cmt as cmt

### functions -----------------------------------------------------------------

def query_option(opt_name):
    """query option from user and set proper history files"""
    history_file = None
    _allowed_values = ('project', 'release', 'justification', 'bug',)
    if opt_name not in _allowed_values:
        raise ValueError(
            "'opt_name' must be in %s (got %s)" %
            _allowed_values,
            opt_name
            )
    history_file = osp.expanduser('~/.tc_submit_tag.%s.history' % opt_name)
    if osp.exists(history_file):
        readline.read_history_file(history_file)

    value = raw_input('%s: ' % opt_name)
    if history_file:
        readline.write_history_file(history_file)
    readline.clear_history()

    if value == '':
        return None
    return value
    
def query_project(projects, release, pkg):
    """query the project(s) to submit to"""
    if len(projects)==1:
        return projects[0]

    print "::: Available projects for package [%s] and release [%s]" % (
        pkg,
        release)
    for p in projects:
        print "   %s" % (p,)

    readline.clear_history()
    for r in reversed(projects):
        readline.add_history(r)

    choice = raw_input("Select (comma separated or '*' for all): ")

    if choice=='*':
        return ','.join(projects)
    return choice

def query_release(releases, project):
    """query the release(s) to submit to"""

    if len(releases)==1:
        return releases[0]

    print "::: Available releases for %s:" % (project,)
    for r in releases:
        print "  %s" % (r,)

    readline.clear_history()
    for r in reversed(releases):
        readline.add_history(r)

    choice = raw_input("Select (comma separated or '*' for all): ")

    if choice=='*':
        return ','.join(releases)
    return choice
   
def submit_tag(client, args, pkg, tag, dryrun=None):
    """Submit tag"""

    cmd_args = {}
    cmd_args['-action'] = 'update'
    cmd_args['-fullPackageName'] = '"'+pkg+'"'
    cmd_args['-packageTag'] = '"'+tag+'"'
    cmd_args['-autoDetectChanges'] = 'yes'

    if args.justification: cmd_args['-justification'] = '"'+args.justification+'"'
    if args.bug: cmd_args['-bugReport'] = '"'+args.bug+'"'
    if args.bundle: cmd_args['-bundleName'] = '"'+args.bundle+'"'
    if args.no_mail: cmd_args['-noMail'] = ''
   
    for i,p in enumerate(args.project):
        cmd_args['-groupName'] = '"'+p+'"'
        cmd_args['-releaseName'] = args.release[i]
        ok = client.exec_cmd(cmd='TCSubmitTagApproval', args=cmd_args, dryrun=dryrun)
        if ok:
            print "%s %s submitted to %s %s" % (pkg,tag,p,args.release[i])

@acmdlib.command(name='tc.submit-tag')
@acmdlib.argument(
    '-p', '--project',
    action='store',
    help='(comma separated list of) project(s) to submit tags to')
@acmdlib.argument(
    '-r', '--release',
    action='store',
    help='(comma separated list of) release(s) to submit tags to')
@acmdlib.argument(
    '-j', '-m', '--justification',
    action='store',
    help='justification for tag request')
@acmdlib.argument(
    '-s', '--bug',
    dest='bug',
    action='store',
    metavar='BUG',
    help='Jira issue')
@acmdlib.argument(
    '-b','--bundle',
    action='store',
    help="bundle name (stays incomplete)")
@acmdlib.argument(
    '-n', '--no-mail',
    action='store_true',
    default=False,
    help="do not send confirmation email")
@acmdlib.argument(
    '--dryrun',
    action='store_true',
    default=False,
    help='switch to simulate the commands but not actually send the requests'
    )
@acmdlib.argument(
    'pkgs',
    nargs='+',
    metavar='TAG',
    help="""\
    (list of package) tags to submit or a file containing that list""")

def main(args):
    """submit one or more package tags to TagCollector

    TAG can be one of the following formats:
      Container/Package-00-01-02
      Package-00-01-02
      Package --> will use latest package tag

    All submitted tags need approval via the TagCollector web interface.
    If several TAGs are given they will be submitted to the same release(s)
    with the same justification, etc. Optionally a bundle name can be specified.
    If no release number is given a list of available releases is presented.

    For any required argument that is not specified on the command line,
    an interactive query is presented. Some text fields support history (arrow keys).

    Authentication is handled via pyAMI (see https://atlas-ami.cern.ch/AMI/pyAMI)
    """

    import PyUtils.AmiLib as amilib

    try:
        client = amilib.Client()
    
        # create a list of (pkg,tag) with full package path
        pkgs = []
    
        for pkg in args.pkgs:
            # a file ?
            if osp.exists(pkg):
                fname = pkg
                print "::: taking tags from file [%s]..." % (fname,)
                for l in open(fname, 'r'):
                    l = l.strip()
                    if l:
                        print " - [%s]" % (l,)
                        pkgs.append(l)
            else:
                pkgs.append(pkg)
    
        pkg_list = [client.find_pkg(pkg, check_tag=False) for pkg in pkgs]
    
        # setup history
        readline.set_history_length(10)
    
        # query release if project is known
        if args.project and not args.release:
            for p in args.project.split(','):
                rel = client.get_open_releases(p)
                if len(rel)==0:
                    continue
                if not args.release:
                    args.release = query_release(rel, p)
                else:
                    args.release += (',%s' % query_release(rel, p))
        if args.release and len(args.release.split(',')) == 1:
            _release = args.release.split(',')[0]
            args.release = ','.join([_release]*len(pkg_list))
            # adjust the project list too
            if args.project and len(args.project.split(',')) == 1:
                args.project = ','.join([args.project.split(',')[0]]*len(pkg_list))
                
        # query project if release is known
        if args.release and not args.project:
            _releases = args.release.split(',')
            _projects = []
            rel = _releases[0]
            for pkg in pkg_list:
                proj = client.get_pkg_info(pkg['packageName'], rel, resultKey="groupName")
                if len(proj)==0:
                    _projects.append(None)
                    continue
                v = query_project(proj, rel, pkg['packageName'])
                _projects.append(v)
                pass # pkgs
            if not args.project:
                args.project = ','.join(_projects)
            else:
                args.project += ','+','.join(_projects)
            pass
    
        # Find latest tag if needed
        print '-'*80
        for p in pkg_list:
            if not 'packageTag' in p:
                pkg = (p['packagePath']+p['packageName']).strip('/') # CMTise path
                #p['packageTag'] = cmt.CmtWrapper().get_latest_pkg_tag(pkg)
                from PyUtils.WorkAreaLib import get_latest_pkg_tag
                p['packageTag'] = get_latest_pkg_tag(pkg)
                print 'Using latest tag %s' % (p['packageTag'])
    
        # query for missing options    
        for o in ('project', 'release', 'justification', 'bug',):
            value = getattr(args, o)
            if value:
                print '%s : %s' % (o, value)
            else:
                setattr(args, o, query_option(o))
        print '-'*80
    
        args.project = args.project.split(',')
        args.release = args.release.split(',')
        if len(args.project) != len(args.release):
            raise RuntimeError(
                'Number of projects %s and releases %s do not match' %
                (args.project, args.release)
                )
    
        # If only one tag given, submit this tag to all releases
        if len(pkg_list)==1: pkg_list = pkg_list*len(args.release)
                        
        choice = raw_input("Submit tag? [Y/n] ")      
        ok = len(choice)==0 or choice.upper()=="Y"
    
        releases = args.release[:]
        projects = args.project[:]
    
        exitcode = 0
        if ok:
            # Submit tag request
            for p,rel,proj in zip(pkg_list, releases, projects):
                args.release = [rel]
                args.project = [proj]
                submit_tag(client, args,
                           p['packagePath']+p['packageName'],p['packageTag'], dryrun=args.dryrun)
        else:
            print "Tag submission aborted"
            exitcode = 1
            
        return exitcode

    except amilib.PyUtilsAMIException, e:
        print >>sys.stderr, e
        sys.exit(1)
