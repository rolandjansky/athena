#==============================================================
# Job options file for the ALFA_CLinkAlg package
#==============================================================

from ALFA_CLinkAlg.ALFA_CLinkAlgConf import ALFA_CLinkAlg
from AthenaCommon.AlgSequence        import AlgSequence

TopLocRecSeq  = AlgSequence()
TopLocRecSeq += ALFA_CLinkAlg("ALFA_CLinkAlg")

alfaCLink = TopLocRecSeq.ALFA_CLinkAlg

from AthenaCommon.GlobalFlags import globalflags
if globalflags.DataSource()=='data':
	alfaCLink.DataType = 1
	IOVDbSvc = Service( "IOVDbSvc" )
	from IOVDbSvc.CondDB import conddb

	if not conddb.folderRequested('/RPO/DCS/BLM'):
		conddb.addFolder("DCS_OFL","/RPO/DCS/BLM")
	if not conddb.folderRequested('/RPO/DCS/FECONFIGURATION'):
		conddb.addFolder("DCS_OFL","/RPO/DCS/FECONFIGURATION")
	if not conddb.folderRequested('/RPO/DCS/HVCHANNEL'):
		conddb.addFolder("DCS_OFL","/RPO/DCS/HVCHANNEL")
	if not conddb.folderRequested('/RPO/DCS/LOCALMONITORING'):
		conddb.addFolder("DCS_OFL","/RPO/DCS/LOCALMONITORING")
	if not conddb.folderRequested('/RPO/DCS/MOVEMENT'):
		conddb.addFolder("DCS_OFL","/RPO/DCS/MOVEMENT")
	if not conddb.folderRequested('/RPO/DCS/RADMON'):
		conddb.addFolder("DCS_OFL","/RPO/DCS/RADMON")
	if not conddb.folderRequested('/RPO/DCS/TRIGGERRATES'):
		conddb.addFolder("DCS_OFL","/RPO/DCS/TRIGGERRATES")
	if not conddb.folderRequested('/RPO/DCS/TRIGGERSETTINGS'):
		conddb.addFolder("DCS_OFL","/RPO/DCS/TRIGGERSETTINGS")

	####for other possible servers see dbreplica.config in Athena installation directory
	#uncomment following line for test purposes only
	#IOVDbSvc.dbConnection="oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=COMP200"
	#IOVDbSvc.Folders += ["/RPO/DCS/BLM"]
	#IOVDbSvc.Folders += ["/RPO/DCS/FECONFIGURATION"]
	#IOVDbSvc.Folders += ["/RPO/DCS/HVCHANNEL"]
	#IOVDbSvc.Folders += ["/RPO/DCS/LOCALMONITORING"]
	#IOVDbSvc.Folders += ["/RPO/DCS/MOVEMENT"]
	#IOVDbSvc.Folders += ["/RPO/DCS/RADMON"]
	#IOVDbSvc.Folders += ["/RPO/DCS/TRIGGERRATES"]
	#IOVDbSvc.Folders += ["/RPO/DCS/TRIGGERSETTINGS"]
else:
	alfaCLink.DataType = 0
