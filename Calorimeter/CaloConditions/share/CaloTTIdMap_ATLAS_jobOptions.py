include.block ( "CaloConditions/CaloTTIdMap_ATLAS_jobOptions.py" )

###################################################################
#
#  Job options file for initializing CaloTriggerTowerService's
#  This jo is called by CaloConditions_jobOptions.py which is the
#  main jo to be used to initialize the CaloTriggerTowerService
#
#  Created 2007-APR-25 by D. Prieur
#==================================================================

#------------------------------------------------------------------
# Add the TTOnOffIdMap & TTOnAttrIdMap Maps COOL folders to IOVDbSvc
#------------------------------------------------------------------du

from IOVDbSvc.CondDB import conddb
from AthenaCommon.GlobalFlags import globalflags
if globalflags.DataSource()=='data':
  CaloDB = "CALO"
  FolderPrefix="/CALO/"
else:
  CaloDB = "CALO_OFL"
  FolderPrefix="/CALO/Ofl/"

# add the DB folders to be opened
TTIdMapFolderNameList  = [ FolderPrefix+"Identifier/CaloTTOnOffIdMapAtlas" ]
TTIdMapFolderNameList += [ FolderPrefix+"Identifier/CaloTTOnAttrIdMapAtlas" ]
TTIdMapFolderNameList += [ FolderPrefix+"Identifier/CaloTTPpmRxIdMapAtlas" ]


#------------------------------------------------------------------

for FolderName in TTIdMapFolderNameList:
	conddb.addFolder(CaloDB,FolderName)
