## @file WriteAthenaPool_jobOptions.py
## @brief AthenaPool job options file for writing event objects.
## @author Peter van Gemmeren <gemmeren@bnl.gov>
## $Id: WriteAthenaPool_jobOptions.py,v 1.12 2008-10-01 15:45:57 gemmeren Exp $
###############################################################
# Avoid double inclusion:
include.block( "AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py" )

# Configure Athena for writing out POOL files
import AthenaPoolCnvSvc.WriteAthenaPool
