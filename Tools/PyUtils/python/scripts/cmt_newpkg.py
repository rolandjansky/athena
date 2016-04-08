# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file PyUtils.scripts.cmt_newpkg
# @purpose streamline and ease the creation of new cmt packages
# @author Sebastien Binet
# @date February 2010

from __future__ import with_statement

__version__ = "$Revision: 734631 $"
__author__ = "Sebastien Binet"
__doc__ = "streamline and ease the creation of new cmt packages"

### imports -------------------------------------------------------------------
import os
import textwrap
import commands
import PyUtils.acmdlib as acmdlib

### functions -----------------------------------------------------------------
@acmdlib.command(
    name='cmt.new-pkg'
    )
@acmdlib.argument(
    'pkgname',
    help="(fully qualified) name of the new package"
    )
@acmdlib.argument(
    '--author',
    default='${USER}',
    help='name of the author of this new package'
    )
@acmdlib.argument(
    '-r', '--runtime',
    action='store_true',
    default=False,
    help='switch to make a package a runtime package',
    )
def main(args):
    """create a new cmt package with sensible atlas-oriented defaults

    ex:
     $ acmd cmt new-pkg Control/MyContainer/NewPackage
    """
    sc = 0
    
    full_pkg_name = args.pkgname
    if full_pkg_name[0] == '/':
        full_pkg_name = full_pkg_name[1:]
        
    pkg_path = os.path.dirname(full_pkg_name)
    pkg_name = os.path.basename(full_pkg_name)
    pkg_vers = '%s-00-00-00' % pkg_name
    author = os.path.expanduser(os.path.expandvars(args.author))

    if os.path.exists(full_pkg_name):
        print "ERROR: %s package already exists" % full_pkg_name
        return 1
        
    print textwrap.dedent("""\
    ::: creating package [%(full_pkg_name)s]...
    :::   - pkg name:    %(pkg_name)s
    :::   - pkg version: %(pkg_vers)s
    :::   - pkg path:    %(pkg_path)s
    :::   - author:      %(author)s""" % locals())
    cmd = 'cmt create %(pkg_name)s %(pkg_vers)s %(pkg_path)s' % locals()
    
    sc, out = commands.getstatusoutput(cmd)
    if sc != 0:
        print "ERROR while running [%s]" % (cmd,)
        print out
        return sc

    with open(os.path.join(full_pkg_name,'cmt','requirements'), 'w') as req:
        print >> req, textwrap.dedent("""\
        ## automatically generated CMT requirements file
        package %(pkg_name)s
        author  %(author)s

        ## for athena policies: this has to be the first use statement
        use AtlasPolicy \tAtlasPolicy-*

        ## for gaudi tools, services and objects
        use GaudiInterface \tGaudiInterface-* \tExternal

        ## put here your package dependencies...

        ##

        branches src src/components doc python share

        ## default is to make component library. See: https://twiki.cern.ch/twiki/bin/view/Main/LearningAthena#Libraries_in_CMT for alternatives
        
        
        apply_pattern declare_joboptions files="*.py"
        apply_pattern declare_python_modules files="*.py"

        
        """%locals())
    print "::: creating package [%(full_pkg_name)s]... [done]" % locals()

    cwd = os.getcwd()
    try:
        os.chdir(os.path.join(full_pkg_name,'cmt'))
        _ = commands.getstatusoutput('cmt config')
    finally:
        os.chdir(cwd)

    ## if sc != 0:
    ##     print ":"*80
    ##     print out
    ##     return sc
    
    return sc

