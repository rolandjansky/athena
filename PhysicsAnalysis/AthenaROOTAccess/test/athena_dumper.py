# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
# $Id: athena_dumper.py,v 1.33 2009-02-11 02:18:49 ssnyder Exp $
#
# @file AthenaROOTAccess/test/athena_dumper.py
# @author sss
# @date Jul 2007
# @brief Test for AthenaROOTAccess.
#
# This test uses Athena to dump AOD-full.pool.root to dump.out/dump-rand.out.
#

vers = globals().get('vers')
if vers:
    input_file = "AOD-%s/AOD-%s-full.pool.root" % (vers,vers)
else:
    input_file = "AOD-full.pool.root"

include ("AthenaROOTAccess/athena_dump.py")
