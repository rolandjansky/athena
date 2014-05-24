#print '\n************************************************************************************************\n'
#print "   ******** Welcome to the Offline BS MuonRawDataMonitoring/CscRawDataMonitoring package. ******** "  
#print "   ******** Documentation may be found at:******** "  
#print "   **** https://twiki.cern.ch/twiki/bin/view/Atlas/CSCOfflineMonitoringPlots ***** "  
#print '\n************************************************************************************************\n'

cscprdBsRawMonMan = AthenaMonManager(name="CscPrdBsRawMonManager",
                                     FileKey             = DQMonFlags.monManFileKey(),
                                     Environment         = DQMonFlags.monManEnvironment(),
                                     OutputLevel         = muonOutputLevel)

cscrdoBsRawMonMan = AthenaMonManager(name="CscRdoBsRawMonManager",
                                     FileKey             = DQMonFlags.monManFileKey(),
                                     Environment         = DQMonFlags.monManEnvironment(),
                                     OutputLevel         = muonOutputLevel)  

csBsRawMonMan = AthenaMonManager(name="CscClusterBsRawMonManager",
                                 FileKey             = DQMonFlags.monManFileKey(),
                                 Environment         = DQMonFlags.monManEnvironment(),
                                 OutputLevel         = muonOutputLevel)

from AthenaCommon.AppMgr import ToolSvc

###############################
# Setup CfgMgr
###############################

from AthenaCommon import CfgMgr
from AthenaCommon.CfgGetter import getPublicTool,getPublicToolClone

CalibCscStripFitter = getPublicTool("CalibCscStripFitter")
ToolSvc += CalibCscStripFitter

#---------------------------------------------------------------
# CSC RDO Monitoring
#--------------------------------------------------------------
from CscRawDataMonitoring.CscRawDataMonitoringConf import CscRdoValAlg
cscRdoBSValAlg = CscRdoValAlg ( name = 'cscRdoBSValAlg',
                              CSCRawDataPath = 'Muon/MuonRawDataMonitoring/CSC/RDO',
                              CSCRawDataKey = 'CSCRDO',
                              NoiseCutADC = 100
                            )
ToolSvc += cscRdoBSValAlg

cscrdoBsRawMonMan.AthenaMonTools += [ cscRdoBSValAlg ]
cscRdoBSValAlg.OutputLevel = INFO

topSequence += cscrdoBsRawMonMan
#print cscrdoBsRawMonMan


#---------------------------------------------------------------
# CSC PRD Monitoring
#--------------------------------------------------------------
from CscRawDataMonitoring.CscRawDataMonitoringConf import CscPrdValAlg
cscPrdBSValAlg = CscPrdValAlg ( name = 'cscPrdBSValAlg',
                              CSCPrepRawDataPath = 'Muon/MuonRawDataMonitoring/CSC/PRD',
                              CSCPrepRawDataKey = 'CSC_Measurements',
                              NoiseCutADC = 100,
                              MapYXandRZ = True,
                              CSCStripFitter = ToolSvc.CalibCscStripFitter
                            )
ToolSvc += cscPrdBSValAlg
cscprdBsRawMonMan.AthenaMonTools += [ cscPrdBSValAlg ]
cscPrdBSValAlg.OutputLevel = INFO

topSequence += cscprdBsRawMonMan
#print cscprdBsRawMonMan

#---------------------------------------------------------------
# CSC Cluster Monitoring
#---------------------------------------------------------------
from CscRawDataMonitoring.CscRawDataMonitoringConf import CscClusterValAlg
cscClusterBSValAlg = CscClusterValAlg( name = 'cscClusterBSValAlg',
                                       CSCClusterPath = 'Muon/MuonRawDataMonitoring/CSC/Clusters',
                                       CSCClusterKey = 'CSC_Clusters',
                                       CSCPrepRawDataKey = 'CSC_Measurements',
                                       CSCQmaxCutADC = 200,
                                       CSCCalibTool = ToolSvc.CscCalibTool,
                                       CSCStripFitter = ToolSvc.CalibCscStripFitter
                                     )

ToolSvc += cscClusterBSValAlg
csBsRawMonMan.AthenaMonTools += [cscClusterBSValAlg]
cscClusterBSValAlg.OutputLevel = INFO

topSequence += csBsRawMonMan
#print csBsRawMonMan

############################################################################################################
