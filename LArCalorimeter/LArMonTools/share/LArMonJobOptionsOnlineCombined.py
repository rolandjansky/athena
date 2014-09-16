Oimport os
import os.path

online = True 
ReadCOOL=True
Type='Pedestal'

#
## What is needed to start
#
include("LArMonTools/LArMonCommonHeader_jobOptions.py")
include("LArMonTools/LArMonManager.py") 

svcMgr.IOVDbSvc.GlobalTag="COMCOND-ES1S-000-00"
#
## The tools 
#
include("LArMonTools/LArFEBMon_jobOptions.py")
include("LArMonTools/LArDigitNoiseMonTool_jobOptions.py")
include("LArMonTools/LArOddCellsMonTool_jobOptions.py")

svcMgr.PoolSvc.ReadCatalog.remove("xmlcatalog_file:/det/lar/project/databases/poolcond/PoolCat_oflcond.xml")

include("LArMonTools/LArMyJob.py")

#
## To finalise things 
#
include("LArMonTools/LArMonCommonTrailer_jobOptions.py")
