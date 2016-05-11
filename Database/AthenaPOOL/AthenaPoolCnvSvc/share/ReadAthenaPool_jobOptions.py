## @file ReadAthenaPool_jobOptions.py
## @brief AthenaPool job options file for reading event objects.
## @author Peter van Gemmeren <gemmeren@bnl.gov>
## $Id: ReadAthenaPool_jobOptions.py,v 1.16 2008-10-01 15:45:57 gemmeren Exp $
###############################################################
# Avoid double inclusion:
include.block( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

# Configure Athena for reading out POOL files
import AthenaPoolCnvSvc.ReadAthenaPool
