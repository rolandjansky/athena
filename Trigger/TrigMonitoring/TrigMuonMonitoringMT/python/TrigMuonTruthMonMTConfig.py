#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 
def TrigMuonTruthMonMTConfig(helper):
    
    from AthenaConfiguration.ComponentFactory import CompFactory
 
    GroupName = 'TruthMon'

    monAlg = helper.addAlgorithm(CompFactory.TrigMuonTruthMonMT,'TrigMuonTruthMonMT')
    monAlg.MonitoredChains = ['HLT_mu6_L1MU5VF', 'HLT_mu26_ivarmedium_L1MU14FCH', 'HLT_mu50_L1MU14FCH', 'HLT_mu60_0eta105_msonly_L1MU14FCH', 'HLT_2mu14_L12MU8F']
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
                                       type = 'TH1F', path='', xbins=30, xmin=0.0, xmax=200.0)
        
        # Matched EFCB to truth variables
        histGroupChain.defineHistogram(chain+'_MatchedEFCBtruthEta', 
                                       title = 'Matched truth eta ' + chain,
                                       type = 'TH1F', path='', xbins=50, xmin=-2.7, xmax=2.7)
        
        histGroupChain.defineHistogram(chain+'_MatchedEFCBtruthPhi', 
                                       title = 'Matched truth phi ' + chain,
                                       type = 'TH1F', path='', xbins=50, xmin=-3.14, xmax=3.14)
                            
        histGroupChain.defineHistogram(chain+'_MatchedEFCBtruthPt', 
                                        title = 'Matched truth pt ' + chain,
                                        type = 'TH1F', path='', xbins=30, xmin=0.0, xmax=200.0)

        # Unmatched EFCB to truth yields
        histGroupChain.defineHistogram(chain+'_failHypothesis', 
                                       title = 'Failed hypothesis ' + chain,
                                       type = 'TH1F', path='', xbins=1, xmin=1.0, xmax=2.0)

        histGroupChain.defineHistogram(chain+'_failMatch', 
                                       title = 'Failed truth match ' + chain,
                                       type = 'TH1F', path='', xbins=1, xmin=1.0, xmax=2.0)

