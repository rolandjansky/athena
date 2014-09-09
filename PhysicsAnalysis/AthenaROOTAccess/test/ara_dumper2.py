# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
# $Id: ara_dumper2.py,v 1.9 2008-04-23 21:14:36 ssnyder Exp $
#
# @file AthenaROOTAccess/test/ara_dumper2.py
# @author sss
# @date Jul 2007
# @brief Test for AthenaROOTAccess.
#
# This test uses ARA to dump AOD-1.pool.root and AOD-2.pool.root
# to dump2.out/dump2-rand.out, using a TChain.
#

execfile('ara_dumper_common.py')

ptree = ROOT.AthenaROOTAccess.TChainROOTAccess('ptree')
ptree.Add (make_fin('AOD-1.pool.root') + '/CollectionTree')
ptree.Add (make_fin('AOD-2.pool.root') + '/CollectionTree')
#ptree.Add ('AOD-13.0.30.3/AOD-13.0.30.3-1.pool.root/CollectionTree')
#ptree.Add ('AOD-13.0.30.3/AOD-13.0.30.3-2.pool.root/CollectionTree')
#ptree.Add ('AOD-dev/AOD-dev-1.pool.root/CollectionTree')
#ptree.Add ('AOD-dev/AOD-dev-2.pool.root/CollectionTree')
run (Files (ptree, 'dump2'))

