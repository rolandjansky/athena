#print '\n*************************************************************************************************\n'
#print "   ******** Welcome to the Offline ESD MuonRawDataMonitoring/MdtRawDataMonitoring package. ******** "  
#print "   ******** Documentation may be found at:******** "  
#print "   **** https://twiki.cern.ch/twiki/bin/view/Atlas/MDTOfflineMonitoringPlots ***** "  

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
isTier0Flag = not athenaCommonFlags.isOnline()

#
#if not isTier0Flag :
#    print "   **** Running Online ****"
#else :
#    print "   **** Running at Tier0 ****"
#
#print '\n*************************************************************************************************\n'


if not 'MuonDQAFlags' in dir():
    print "MuonDQAFlags.py: MuonDQAFlags not yet imported - I import them now"
    from MuonDQAMonFlags.MuonDQAFlags import MuonDQAFlags as MuonDQAFlags

mdtRawMonMan = AthenaMonManager(name="MdtRawMonManager",
                                FileKey             = DQMonFlags.monManFileKey(),
                                Environment         = DQMonFlags.monManEnvironment(),
                                OutputLevel         = muonOutputLevel)

############# MDTRawDataValAlg #############
from MdtRawDataMonitoring.MdtRawDataMonitoringConf import MdtRawDataValAlg

#This it the main tool w/ detailed histograms
mdtRawDataValAlg_main = MdtRawDataValAlg(name='mdtRawDataValAlg_main',
                                         Title='MDT',
                                         OutputLevel = INFO,
                                         DoMdtEsd = True,
                                         #Use minimal tool for online monitoring
                                         DoOnlineMonitoring = not isTier0Flag,
                                         DoChamberHist = isTier0Flag,
                                         maskNoisyTubes = True,
                                         DoChamber2DHist = True, 
                                         ADCCut = 80.,
                                         ADCCutForBackground = 80.,
                                         #MaxTDCCutForBackground = 0.,
                                         #TGCTrigKey = 136,                                    
                                         #RPCTrigKey = 144,
                                         AtlasFilterTool = [ monAtlasReadyFilterTool ],
                                         do_mdtChamberHits = isTier0Flag,
                                         do_mdttdccut_sector = isTier0Flag,  
                                         do_mdtchamberstatphislice = isTier0Flag,
                                         do_mdttdc = True,
                                         do_mdttdccut_ML1 = True,
                                         do_mdttdccut_ML2 = True,
                                         do_mdttdccut_RPCtrig_ML1 = True,
                                         do_mdttdccut_TGCtrig_ML1 = True,
                                         do_mdttdccut_RPCtrig_ML2 = True,
                                         do_mdttdccut_TGCtrig_ML2 = True,
                                         do_mdtadc = True,
                                         do_mdttdcadc = True,
                                         do_mdtmultil = True,
                                         do_mdtlayer = True,
                                         do_mdttube = True,
                                         do_mdttube_bkgrd = True,
                                         do_mdttube_fornoise = True,
                                         do_mdttube_masked = True,
                                         do_mdtmezz = True,
                                         do_mdt_effEntries = True,
                                         do_mdt_effCounts = True,
                                         do_mdt_effPerTube = False,
                                         do_mdt_DRvsDT = False,
                                         do_mdt_DRvsDRerr = False,
                                         do_mdt_DRvsSegD = False,
					 nHits_NoiseThreshold = 10000
                                         )

ToolSvc += mdtRawDataValAlg_main
mdtRawMonMan.AthenaMonTools += [ mdtRawDataValAlg_main ]

if( isTier0Flag ) :
    #No need for this online
    #This tool is meant for express stream monitoring
    #We invoke the trig-decision tool to trigger on muon triggers
    mdtRawDataValAlg_express = MdtRawDataValAlg(name='mdtRawDataValAlg_express',
                                                Title='MDT_express',
                                                OutputLevel = INFO,
                                                DoMdtEsd = True,
                                                DoChamberHist = isTier0Flag,
                                                DoOnlineMonitoring = False,
                                                maskNoisyTubes = True,
                                                DoChamber2DHist = True, 
                                                ADCCut = 80.,
                                                ADCCutForBackground = 80.,
                                                #MaxTDCCutForBackground = 0.,
                                                #TGCTrigKey = 136,       
                                                #RPCTrigKey = 144,
                                                AtlasFilterTool = [ monAtlasReadyFilterTool ],
                                                do_mdtChamberHits = False,
                                                do_mdttdccut_sector = False,                                         
                                                do_mdtchamberstatphislice = False,
                                                do_mdttdc = False,
                                                do_mdttdccut_ML1 = False,
                                                do_mdttdccut_ML2 = False,
                                                do_mdttdccut_RPCtrig_ML1 = False,
                                                do_mdttdccut_TGCtrig_ML1 = False,
                                                do_mdttdccut_RPCtrig_ML2 = False,
                                                do_mdttdccut_TGCtrig_ML2 = False,
                                                do_mdtadc = False,
                                                do_mdttdcadc = False,
                                                do_mdtmultil = False,
                                                do_mdtlayer = False,
                                                do_mdttube = True,
                                                do_mdttube_bkgrd = False,
                                                do_mdttube_fornoise = False,
                                                do_mdttube_masked = False,
                                                do_mdtmezz = False,
                                                do_mdt_effEntries = True,
                                                do_mdt_effCounts = True,
                                                do_mdt_effPerTube = False,
                                                do_mdt_DRvsDT = False,
                                                do_mdt_DRvsDRerr = False,
                                                do_mdt_DRvsSegD = False,
					 	nHits_NoiseThreshold = 10000
                                                )
    if DQMonFlags.useTrigger():
            mdtRawDataValAlg_express.TrigDecisionTool = monTrigDecTool #tool should already be in namespace
            mdtRawDataValAlg_express.TriggerChain = "L1_MU0, L1_MU6, L1_MU10, L1_MU15, L1_MU20"
    #end if useTrigger
    
    ToolSvc += mdtRawDataValAlg_express
    mdtRawMonMan.AthenaMonTools += [ mdtRawDataValAlg_express ]
#end if( isTier0Flag )

topSequence += mdtRawMonMan
print mdtRawMonMan
##################################################################
