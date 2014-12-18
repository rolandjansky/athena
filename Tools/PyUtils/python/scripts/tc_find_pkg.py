# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file PyUtils.scripts.tc_find_pkg
# @purpose find a package using TC-2
# @author Sebastien Binet, Graeme Stewart
# @version $Id$

import sys
import PyUtils.acmdlib as acmdlib

@acmdlib.command(name='tc.find-pkg')
@acmdlib.argument('pkg',
                  nargs='+',
                  help='(list of) package(s) to find in TagCollector')
def main(args):
    """find a package using TagCollector-2"""

    import PyUtils.AmiLib as amilib

    try:
        client = amilib.Client()
        pkgs = args.pkg
        if isinstance(pkgs, basestring):
            pkgs = [pkgs]
    
        for pkg in pkgs:
            client.msg.info('looking for [%s]...', pkg)
            pkg = client.find_pkg(pkg, check_tag=False)
            client.msg.info(' found: pkg= [%s]', pkg['packagePath']+pkg['packageName'])
    
        return 0
    except amilib.PyUtilsAMIException, e:
        print >>sys.stderr, e
        sys.exit(1)
