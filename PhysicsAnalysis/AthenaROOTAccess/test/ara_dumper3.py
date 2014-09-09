# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
# $Id: ara_dumper3.py,v 1.6 2008-04-23 21:14:37 ssnyder Exp $
#
# @file AthenaROOTAccess/test/ara_dumper3.py
# @author sss
# @date Aug 2007
# @brief Test for AthenaROOTAccess.
#
# This test uses ARA to dump AOD-1.pool.root and AOD-2.pool.root
# to dump3.out/dump3-rand.out, using separate trees.
#

execfile('ara_dumper_common.py')

f1 = Files (ROOT.TFile (make_fin('AOD-1.pool.root')), 'dump3')
f2 = Files (ROOT.TFile (make_fin('AOD-2.pool.root')), f1)
#f1 = Files (ROOT.TFile ('AOD-13.0.30.3/AOD-13.0.30.3-1.pool.root'), 'dump3')
#f2 = Files (ROOT.TFile ('AOD-13.0.30.3/AOD-13.0.30.3-2.pool.root'), f1)
#f1 = Files (ROOT.TFile ('AOD-dev/AOD-dev-1.pool.root'), 'dump3')
#f2 = Files (ROOT.TFile ('AOD-dev/AOD-dev-2.pool.root'), f1)

run (f1)
run (f2)
