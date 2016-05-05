#print '\n************************************************************************************************\n'
#print "   ******** Welcome to the Offline BS MuonRawDataMonitoring/CscRawDataMonitoring package. ******** "  
#print "   ******** Documentation may be found at:******** "  
#print "   **** https://twiki.cern.ch/twiki/bin/view/Atlas/CSCOfflineMonitoringPlots ***** "  
#print '\n************************************************************************************************\n'

cscrdoBsRawMonMan = AthenaMonManager(name="CscRdoBsRawMonManager",
                                     FileKey             = DQMonFlags.monManFileKey(),
                                     Environment         = DQMonFlags.monManEnvironment(),
                                     OutputLevel         = muonOutputLevel)  

from AthenaCommon.AppMgr import ToolSvc

#---------------------------------------------------------------
# CSC RDO Monitoring
#--------------------------------------------------------------
from CscRawDataMonitoring.CscRawDataMonitoringConf import CscRdoValAlg
cscRdoBSValAlg = CscRdoValAlg (name = 'cscRdoBSValAlg',
                               CSCRawDataPath = 'Muon/MuonRawDataMonitoring/CSC/RDO',
                               CSCRawDataKey = 'CSCRDO',
                               NoiseCutADC = 100
                               )
ToolSvc += cscRdoBSValAlg

cscrdoBsRawMonMan.AthenaMonTools += [ cscRdoBSValAlg ]
cscRdoBSValAlg.OutputLevel = INFO

topSequence += cscrdoBsRawMonMan
#print cscrdoBsRawMonMan

