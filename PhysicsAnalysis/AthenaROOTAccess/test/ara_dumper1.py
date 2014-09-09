# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
# $Id: ara_dumper1.py,v 1.7 2008-04-23 21:14:36 ssnyder Exp $
#
# @file AthenaROOTAccess/test/ara_dumper1.py
# @author sss
# @date Jul 2007
# @brief Test for AthenaROOTAccess.
#
# This test uses ARA to dump AOD-full.pool.root to dump1.out/dump1-rand.out.
#

import resource
import time
t0=resource.getrusage(resource.RUSAGE_SELF)[0]
tw0=time.time()

execfile('ara_dumper_common.py')

f = ROOT.TFile (make_fin ('AOD-full.pool.root'))
#f = ROOT.TFile ('AOD-13.0.30.3/AOD-13.0.30.3-full.pool.root')
#f = ROOT.TFile ('AOD-dev/AOD-dev-full.pool.root')
ff = Files (f, 'dump1')

t1=resource.getrusage(resource.RUSAGE_SELF)[0]
tw1=time.time()
print "***** init cpu", t1-t0, "wall", tw1-tw0

run (ff)

t2=resource.getrusage(resource.RUSAGE_SELF)[0]
tw2=time.time()
print "***** run cpu", t2-t1, "wall", tw2-tw1
