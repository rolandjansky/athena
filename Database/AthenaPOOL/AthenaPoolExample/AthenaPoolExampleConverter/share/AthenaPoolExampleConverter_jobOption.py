## @file AthenaPoolExampleConverter_jobOption.py
## @brief Example job options file to illustrate how to load a AthenaPool Converter.
## @author Peter van Gemmeren <gemmeren@anl.gov>
## $Id: AthenaPoolExampleConverter_jobOption.py,v 1.5 2005-12-08 17:27:03 gemmeren Exp $
###############################################################
#
# This Job option:
# ----------------
# 1. Writes/Reads Example event objects to/from AthenaPool.
#
#==============================================================

theApp.Dlls   += [ "AthenaPoolExampleConverterPoolCnv" ]
#==============================================================
#
# End of job options file
#
###############################################################
