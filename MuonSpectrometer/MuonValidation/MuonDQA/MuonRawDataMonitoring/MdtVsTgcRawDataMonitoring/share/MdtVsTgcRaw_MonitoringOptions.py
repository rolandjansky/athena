print '\n****************************************************************************************\n'
print "   ******** Welcome to the Offline ESD MuonRawDataMonitoring package. ******** "  
print "   ******** Documentation may be found at:******** "  
print "   **** https://twiki.cern.ch/twiki/bin/view/Atlas/MuonOfflineDQA ***** "  
print '\n****************************************************************************************\n'

if not 'MuonDQAFlags' in dir():
    print "MuonDQAFlags.py: MuonDQAFlags not yet imported - I import them now"
    from MuonDQAMonFlags.MuonDQAFlags import MuonDQAFlags as MuonDQAFlags

MdtTgcRawMonMan = AthenaMonManager(name="MdtVsTgcRawMonManager",
                                   FileKey             = DQMonFlags.monManFileKey(),
                                   Environment         = DQMonFlags.monManEnvironment(),
                                   OutputLevel         = muonOutputLevel)

############## MDT vs TGC ##########
from MdtVsTgcRawDataMonitoring.MdtVsTgcRawDataMonitoringConf import MdtVsTgcRawDataValAlg
MdtVsTgcRawDataValAlg = MdtVsTgcRawDataValAlg (name = "MdtVsTgcRawDataValAlg",
                                               MdtAdcCut = 50,
                                               Side=0,
                                               TgcPrepDataContainer="TGC_MeasurementsAllBCs",
                                               OutputCoinCollection="TrigT1CoinDataCollection",
                                               MdtPrepDataContainer="MDT_DriftCircles",
                                               TgcLv1File = False) 
ToolSvc += MdtVsTgcRawDataValAlg
MdtTgcRawMonMan.AthenaMonTools += [ MdtVsTgcRawDataValAlg ]
topSequence += MdtTgcRawMonMan
print MdtTgcRawMonMan
##################################################################
