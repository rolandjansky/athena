#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 
def TrigMuonTruthMonConfig(helper):
    
    from AthenaConfiguration.ComponentFactory import CompFactory
 
    GroupName = 'TruthMon'

    from MuonSelectorTools.MuonSelectorToolsConfig import MuonSelectionToolCfg
    monAlg = helper.addAlgorithm(CompFactory.TrigMuonTruthMon,'TrigMuonTruthMon',
                                 MuonSelectionTool = helper.result().popToolsAndMerge(MuonSelectionToolCfg(helper.inputFlags, MuQuality=1)))

    ### monitorig groups
    from TrigConfigSvc.TriggerConfigAccess import getHLTMonitoringAccess
    moniAccess = getHLTMonitoringAccess(helper.inputFlags)
    monAlg.MonitoredChains = moniAccess.monitoredChains(signatures="muonMon",monLevels=["shifter","t0","val"])
  
    # if mon groups not found fall back to hard-coded trigger monitoring list
    if len(monAlg.MonitoredChains) == 0:
        monAlg.MonitoredChains = ['HLT_mu6_L1MU5VF', 'HLT_mu24_ivarmedium_L1MU14FCH', 'HLT_mu50_L1MU14FCH', 'HLT_mu60_0eta105_msonly_L1MU14FCH', 'HLT_2mu14_L12MU8F']

    monAlg.Group = GroupName
    
    for chain in monAlg.MonitoredChains:
        histGroupChain = helper.addGroup(monAlg, GroupName + '_' + chain, 'HLT/MuonMon/' + GroupName + '/' + chain)

        # Truth variables
        histGroupChain.defineHistogram(chain+'_truthEta', 
                                       title = 'Truth eta ' + chain,
                                       type = 'TH1F', path='', xbins=50, xmin=-2.7, xmax=2.7)
        
        histGroupChain.defineHistogram(chain+'_truthPhi', 
                                       title = 'Truth phi ' + chain,
                                       type = 'TH1F', path='', xbins=50, xmin=-3.14, xmax=3.14)

        histGroupChain.defineHistogram(chain+'_truthPt', 
                                       title = 'Truth pt ' + chain,
                                       type = 'TH1F', path='', xbins=[0.0,20.0,25.0,30.0,35.0,40.0,45.0,50.0,55.0,60.0,65.0,70.0,80.0,90.0,100.0,120.0,200.0], xmin=0.0, xmax=200.0)

        histGroupChain.defineHistogram(chain+'_truthEndcapPt', 
                                       title = 'Truth pt ' + chain,
                                       type = 'TH1F', path='', xbins=[0.0,20.0,25.0,30.0,35.0,40.0,45.0,50.0,55.0,60.0,65.0,70.0,80.0,90.0,100.0,120.0,200.0], xmin=0.0, xmax=200.0)

        histGroupChain.defineHistogram(chain+'_truthBarrelPt', 
                                       title = 'Truth pt ' + chain,
                                       type = 'TH1F', path='', xbins=[0.0,20.0,25.0,30.0,35.0,40.0,45.0,50.0,55.0,60.0,65.0,70.0,80.0,90.0,100.0,120.0,200.0], xmin=0.0, xmax=200.0)

        histGroupChain.defineHistogram(chain+'_truthIntPerBC', 
                                       title = 'Average interactions per bunch crossing ' + chain,
                                       type = 'TH1F', path='', xbins=60, xmin=30, xmax=90)
        
        
        # Matched EFCB to truth variables
        histGroupChain.defineHistogram(chain+'_MatchedEFCBtruthEta', 
                                       title = 'Matched truth eta ' + chain,
                                       type = 'TH1F', path='', xbins=50, xmin=-2.7, xmax=2.7)
        
        histGroupChain.defineHistogram(chain+'_MatchedEFCBtruthPhi', 
                                       title = 'Matched truth phi ' + chain,
                                       type = 'TH1F', path='', xbins=50, xmin=-3.14, xmax=3.14)
                            
        histGroupChain.defineHistogram(chain+'_MatchedEFCBtruthPt', 
                                        title = 'Matched EFCB muon truth pt ' + chain,
                                        type = 'TH1F', path='', xbins=[0.0,20.0,25.0,30.0,35.0,40.0,45.0,50.0,55.0,60.0,65.0,70.0,80.0,90.0,100.0,120.0,200.0], xmin=0.0, xmax=200.0)

        histGroupChain.defineHistogram(chain+'_MatchedEFCBtruthEndcapPt', 
                                        title = 'Matched EFCB muon truth pt ' + chain,
                                        type = 'TH1F', path='', xbins=[0.0,20.0,25.0,30.0,35.0,40.0,45.0,50.0,55.0,60.0,65.0,70.0,80.0,90.0,100.0,120.0,200.0], xmin=0.0, xmax=200.0)

        histGroupChain.defineHistogram(chain+'_MatchedEFCBtruthBarrelPt', 
                                        title = 'Matched EFCB muon truth pt ' + chain,
                                        type = 'TH1F', path='', xbins=[0.0,20.0,25.0,30.0,35.0,40.0,45.0,50.0,55.0,60.0,65.0,70.0,80.0,90.0,100.0,120.0,200.0], xmin=0.0, xmax=200.0)

        histGroupChain.defineHistogram(chain+'_MatchedEFCBtruthIntPerBC', 
                                       title = 'Matched EFCB muon average interactions ber bunch crossing ' + chain,
                                       type = 'TH1F', path='', xbins=60, xmin=30, xmax=90)

       # Matched L1 to truth variables
        histGroupChain.defineHistogram(chain+'_MatchedL1truthEta', 
                                       title = 'Matched L1 muon truth eta ' + chain,
                                       type = 'TH1F', path='', xbins=50, xmin=-2.7, xmax=2.7)
        
        histGroupChain.defineHistogram(chain+'_MatchedL1truthPhi', 
                                       title = 'Matched L1 muon truth phi ' + chain,
                                       type = 'TH1F', path='', xbins=50, xmin=-3.14, xmax=3.14)
                            
        histGroupChain.defineHistogram(chain+'_MatchedL1truthPt', 
                                        title = 'Matched L1 muon truth pt ' + chain,
                                        type = 'TH1F', path='', xbins=[0.0,20.0,25.0,30.0,35.0,40.0,45.0,50.0,55.0,60.0,65.0,70.0,80.0,90.0,100.0,120.0,200.0], xmin=0.0, xmax=200.0)

        histGroupChain.defineHistogram(chain+'_MatchedL1truthEndcapPt', 
                                        title = 'Matched L1 muon truth pt ' + chain,
                                        type = 'TH1F', path='', xbins=[0.0,20.0,25.0,30.0,35.0,40.0,45.0,50.0,55.0,60.0,65.0,70.0,80.0,90.0,100.0,120.0,200.0], xmin=0.0, xmax=200.0)

        histGroupChain.defineHistogram(chain+'_MatchedL1truthBarrelPt', 
                                        title = 'Matched L1 muon truth pt ' + chain,
                                        type = 'TH1F', path='', xbins=[0.0,20.0,25.0,30.0,35.0,40.0,45.0,50.0,55.0,60.0,65.0,70.0,80.0,90.0,100.0,120.0,200.0], xmin=0.0, xmax=200.0)

        histGroupChain.defineHistogram(chain+'_MatchedL1truthIntPerBC', 
                                       title = 'Matched L1 muon average interactions ber bunch crossing ' + chain,
                                       type = 'TH1F', path='', xbins=60, xmin=30, xmax=90)

