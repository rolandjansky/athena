#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file:    get-tag-diff.py
# @purpose: Get the list of tag differences between 2 releases (CERN centric)
# @author:  Sebastien Binet <binet@cern.ch>
# @date:    October 2009
#
# @example:
#
# get-tag-diff --ref=rel_3,12.0.X --chk=12.0.3
#

__version__ = "$Revision: 273188 $"

from optparse import OptionParser

import sys
import os

def main():
    
    parser = OptionParser( usage = "usage: %prog --ref some_rel --chk other_rel")
    parser.add_option("--ref",
                      dest = "ref",
                      help = "The description string of the reference release (eg: 12.0.X,rel_3,AtlasOffline)" )

    parser.add_option("--chk",
                      dest = "chk",
                      help = "The description string of the to-be-compared release (eg: 12.0.X,rel_3 or 12.0.3)" )

    options,args = parser.parse_args()

    if options.ref is None or options.chk is None:
        parser.print_help()
        return 1
    
    ref = options.ref
    chk = options.chk

    import PyCmt.Cmt as Cmt
    diffs = Cmt.get_tag_diff(ref=ref, chk=chk, verbose=True)
    if len(diffs) > 0:
        return 1
    return 0

if __name__ == "__main__":
    import sys
    sys.exit(main())
    
