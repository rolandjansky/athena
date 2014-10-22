#print '\n****************************************************************************************\n'
#print "   ******** Welcome to the Offline ESD MuonRawDataMonitoring/CscRawDataMonitoring package. ******** "  
#print "   ******** Documentation may be found at:******** "  
#print "   **** https://twiki.cern.ch/twiki/bin/view/Atlas/CSCOfflineMonitoringPlots ***** "  
#print '\n************************************************************************************************\n'

cscprdRawMonMan = AthenaMonManager(name="CscPrdRawMonManager",
                                   FileKey             = DQMonFlags.monManFileKey(),
                                   Environment         = DQMonFlags.monManEnvironment(),
                                   OutputLevel         = muonOutputLevel)

#cscrdoRawMonMan = AthenaMonManager(name="CscRdoRawMonManager",
#                                   FileKey             = DQMonFlags.monManFileKey(),
#                                   Environment         = DQMonFlags.monManEnvironment(),
#                                   OutputLevel         = muonOutputLevel)  

cscesdRawMonMan = AthenaMonManager(name="CscClusterEsdRawMonManager",
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
#from CscRawDataMonitoring.CscRawDataMonitoringConf import CscRdoValAlg
#cscRdoESDValAlg = CscRdoValAlg ( name = 'cscRdoESDValAlg',
#                              CSCRawDataPath = 'Muon/MuonRawDataMonitoring/CSC/RDO',
#                              CSCRawDataKey = 'CSCRDO',
#                              NoiseCutADC = 100
#                            )
#ToolSvc += cscRdoESDValAlg
#cscrdoRawMonMan.AthenaMonTools += [ cscRdoESDValAlg ]

#topSequence += cscrdoRawMonMan
#print cscrdoRawMonMan

#---------------------------------------------------------------
# CSC PRD Monitoring
#--------------------------------------------------------------
from CscRawDataMonitoring.CscRawDataMonitoringConf import CscPrdValAlg
cscPrdESDValAlg = CscPrdValAlg ( name = 'cscPrdESDValAlg',
                              CSCPrepRawDataPath = 'Muon/MuonRawDataMonitoring/CSC/PRD',
                              CSCPrepRawDataKey = 'CSC_Measurements',
                              NoiseCutADC = 100,
                              CSCStripFitter = ToolSvc.CalibCscStripFitter
                            )
ToolSvc += cscPrdESDValAlg
cscprdRawMonMan.AthenaMonTools += [ cscPrdESDValAlg ]

topSequence += cscprdRawMonMan
#print cscprdRawMonMan

#---------------------------------------------------------------
# CSC Cluster Monitoring
#---------------------------------------------------------------

## trigger-aware monitoring: sample seletion triggers
csc_evtSelectionTriggers = [ "L1_MU10", "L1_MU15", "EF_mu20_muCombTag_NoEF", "EF_mu15", "EF_mu15_mu10_EFFS", "EF_2mu10", "EF_2mu10_loose" ]

## retrieve TDT (this should be done already?)
if(DQMonFlags.useTrigger() and hasattr(ToolSvc, DQMonFlags.nameTrigDecTool())):
  from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
  ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
  ToolSvc.TrigDecisionTool.OutputLevel=ERROR
  ToolSvc.TrigDecisionTool.Navigation.OutputLevel = ERROR

from CscRawDataMonitoring.CscRawDataMonitoringConf import CscClusterValAlg
cscClusterESDValAlg = CscClusterValAlg( name = 'cscClusterESDValAlg',
                                       CSCClusterPath = 'Muon/MuonRawDataMonitoring/CSC/Clusters',
                                       CSCClusterKey = 'CSC_Clusters',
                                       CSCPrepRawDataKey = 'CSC_Measurements',
                                       CSCQmaxCutADC = 200,
                                       CSCCalibTool = ToolSvc.CscCalibTool,
                                       CSCStripFitter = ToolSvc.CalibCscStripFitter,
                                       #CSCTrigDecisionTool = ToolSvc.TrigDecisionTool,
                                       CSCDoEventSelection = False,
                                       CSCEventSelTriggers = csc_evtSelectionTriggers                                    
                                     )

ToolSvc += cscClusterESDValAlg
cscesdRawMonMan.AthenaMonTools += [cscClusterESDValAlg]

topSequence += cscesdRawMonMan
#print cscesdRawMonMan

############################################################################################################

