# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

class L1JetMonAlg():
  def __init__(self,name,jetcoll,triggerChain='',matched=False,matchedOfflineJets='',matchedHLTJets=''):
    self.name           = name
    self.L1JetContainer = jetcoll
    self.TriggerChain   = triggerChain
    self.Matched        = matched
    self.MatchedOJ      = matchedOfflineJets
    self.MatchedHLTJ    = matchedHLTJets

  def __str__(self):
    text = ' '.join(['%s: %s' % (str(k), str(v)) for k, v in self.__dict__.items()])
    return self.__class__.__name__ + ':: ' + text


  def toAlg(self,monhelper):
    from AthenaConfiguration.ComponentFactory import CompFactory

    jFex = self.L1JetContainer == 'L1_jFexSRJetRoI'

    jetcontainer       = self.L1JetContainer
    triggerChain       = self.TriggerChain
    ismatched          = self.Matched

    if jFex:
      alg = monhelper.addAlgorithm(CompFactory.TrigL1JFexSRJetMonitorAlgorithm, self.name)
    else:
      alg = monhelper.addAlgorithm(CompFactory.TrigL1JetMonitorAlgorithm, self.name)

    # aliases to save typing
    jetcontainer       = self.L1JetContainer
    triggerChain       = self.TriggerChain
    ismatched          = self.Matched
    
    alg.L1JetContainer = jetcontainer
    alg.TriggerChain   = triggerChain
    alg.IsMatched      = ismatched
    
    # Add a generic monitoring tool (a "group" in old language). The returned 
    # object here is the standard GenericMonitoringTool

    Path  = jetcontainer+'/'
    Path += 'NoTriggerSelection/' if triggerChain == '' else triggerChain+'/'
    if jFex:
      myGroup = monhelper.addGroup(alg,'TrigL1JFexSRJetMonitor','HLT/JetMon/L1/')
      myGroup.defineHistogram('et',title='et',path=Path,xbins=400,xmin=1.0,xmax=400.0)

    else:
      myGroup = monhelper.addGroup(alg,'TrigL1JetMonitor','HLT/JetMon/L1/')
      myGroup.defineHistogram('et8x8',title='et8x8',path=Path,xbins=400,xmin=0.0,xmax=400.0)
      
    myGroup.defineHistogram('eta',title='eta',path=Path,xbins=50,xmin=-5,xmax=5)
    myGroup.defineHistogram('phi',title='phi',path=Path,xbins=50,xmin=-3.3,xmax=3.3)

    # Add histograms for L1 jets matched to offline/online jets
    if ismatched:
      matchedOJ   = self.MatchedOJ
      matchedHLTJ = self.MatchedHLTJ

      alg.MatchedOfflineJets = matchedOJ
      alg.MatchedHLTJets     = matchedHLTJ
        
      for matchcoll,tag in [ [matchedOJ, 'off'], [matchedHLTJ, 'hlt'] ]:
        Path = jetcontainer + '/NoTriggerSelection/MatchedJets_' + matchcoll + '/'
        for histname in [ 'ptdiff', 'energydiff' ]: #defines which variable difference will be plotted, mass difference makes no sense for L1 as m=0
          myGroup.defineHistogram(tag+histname+';'+histname,title=histname, type="TH1F", path=Path, xbins=140 , xmin=-120000., xmax=20000. ,)
        for histname in [ 'ptresp', 'energyresp' ]:
          myGroup.defineHistogram(tag+histname+';'+histname,title=histname, type="TH1F", path=Path, xbins=100 , xmin=-1., xmax=1. ,)
        myGroup.defineHistogram(tag+'ptresp,'+tag+'ptref;ptresp_vs_ptRef',title='ptresponse vs ptRef', type="TH2F", path=Path, xbins=10 , xmin=-1., xmax=1., ybins=10, ymin=0., ymax=500000.,)
        myGroup.defineHistogram(tag+'ptresp,'+tag+'etaref;ptresp_vs_etaRef',title='ptresponse vs etaRef', type="TH2F", path=Path, xbins=10 , xmin=-1., xmax=1., ybins=10, ymin=-5., ymax=5.,)


    return alg
