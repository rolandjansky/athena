#!/usr/bin/env python
#
#Script to obtain the run number in the input data file for partition test,
#in order to use a consistent set of conditions wrt MT/PT test.

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

import optparse

parser = optparse.OptionParser(description="Get run number from first event in file")

parser.add_option("--L2Input",
                  action = "store",
                  help = "BS input file for athenaMT")
# Now parse the options
(opts, args) = parser.parse_args()
run=""
file="" 
if not opts.L2Input:
    parser.error("need option --L2Input <BSfile>")

file = "%s" % opts.L2Input

import eformat
f = eformat.istream(file)
run = f[0].run_no()

#print 'run number : ' , run
print run
