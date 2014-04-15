## @file CheckEDM.py
## @brief TopOptions: Check whether simulation output from DC2
## is still readable.
## @author Jack Cranshaw (Jack.Cranshaw@cern.ch
## $Id: CheckEDM.py,v 1.6 2005-08-17 21:11:07 cranshaw Exp $
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
#import glob, os, re
In=['/afs/cern.ch/atlas/offline/data/testfile/dc2.002885.pyt_z_ee.g4dig805._0001.pool.root']
Out="CheckEDM_out.root"
include('AthenaPoolMultiTest/EventCheck.py')
#==============================================================
#
# End of job options file
#
###############################################################
