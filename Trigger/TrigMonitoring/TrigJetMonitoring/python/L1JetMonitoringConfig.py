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

    jFexSR = self.L1JetContainer == 'L1_jFexSRJetRoI'
    jFexLR = self.L1JetContainer == 'L1_jFexLRJetRoI'
    gFexSR = self.L1JetContainer == 'L1_gFexSRJetRoI'
    gFexLR = self.L1JetContainer == 'L1_gFexSRJetRoI'

    L1Fex = jFexSR or jFexLR or gFexSR or gFexLR
    
    jetcontainer       = self.L1JetContainer
    triggerChain       = self.TriggerChain
    ismatched          = self.Matched

    if L1Fex:
      alg = monhelper.addAlgorithm(CompFactory.TrigL1FexJetMonitorAlgorithm,
                                   self.name)
      if jFexSR:
        toolClass = getattr(CompFactory,
                            'TrigL1FexJetMonitorTool<DataVector<xAOD::jFexSRJetRoI_v1, DataModel_detail::NoBase> >')
        tool = toolClass("jFexSRDataRetriever")
        tool.do_matching = False   # PS under development
        tool.l1container = self.L1JetContainer
        alg.fillers = [tool]
        
      elif jFexLR:
        toolClass = getattr(CompFactory,
                            'TrigL1FexJetMonitorTool<DataVector<xAOD::jFexLRJetRoI_v1, DataModel_detail::NoBase> >')
        tool = toolClass("jFexLRDataRetriever")
        tool.do_matching = False   # PS under development
        tool.l1container = self.L1JetContainer
        alg.fillers = [tool]
      elif gFexSR:
        toolClass = getattr(CompFactory,
                            'TrigL1FexJetMonitorTool<DataVector<xAOD::gFexJetRoI_v1, DataModel_detail::NoBase> >')
        tool = toolClass("gFexDataRetriever")
        tool.do_matching = False   # PS under development
        tool.l1container = self.L1JetContainer
        alg.fillers = [tool]
      elif gFexLR:
        toolClass = getattr(CompFactory,
                            'TrigL1FexJetMonitorTool<DataVector<xAOD::gFexJetRoI_v1, DataModel_detail::NoBase> >')
        tool = toolClass("gFexDataRetriever")
        tool.do_matching = False   # PS under development
        tool.l1container = self.L1JetContainer
        alg.fillers = [tool]
      else:
        raise RuntimeError("L1Fex flag is set, but no corresponding container flag is set " + self.L1JetContainer)
      
    else:
      alg = monhelper.addAlgorithm(CompFactory.TrigL1JetMonitorAlgorithm, self.name)
      alg.IsMatched      = ismatched
      alg.L1JetContainer = jetcontainer

    # aliases to save typing
    jetcontainer       = self.L1JetContainer
    triggerChain       = self.TriggerChain
    
    alg.TriggerChain   = triggerChain
    
    # Add a generic monitoring tool (a "group" in old language). The returned 
    # object here is the standard GenericMonitoringTool

    Path  = jetcontainer+'/'
    Path += 'NoTriggerSelection/' if triggerChain == '' else triggerChain+'/'
    if jFexSR:
      myGroup = monhelper.addGroup(alg,'TrigL1JFexSRJetMonitor','HLT/JetMon/L1/')
    elif jFexLR:
      myGroup = monhelper.addGroup(alg,'TrigL1JFexLRJetMonitor','HLT/JetMon/L1/')
    elif gFexSR:
      myGroup = monhelper.addGroup(alg,'TrigL1GFexSRJetMonitor','HLT/JetMon/L1/')
    elif gFexLR:
      myGroup = monhelper.addGroup(alg,'TrigL1GFexLRJetMonitor','HLT/JetMon/L1/')
    else:
      myGroup = monhelper.addGroup(alg,'TrigL1JetMonitor','HLT/JetMon/L1/')
                                  

    if L1Fex:
      myGroup.defineHistogram('et',title='et',path=Path,xbins=400,xmin=0.0,xmax=400.0)
    else:
      myGroup.defineHistogram('et8x8',title='et8x8',path=Path,xbins=400,xmin=0.0,xmax=400.0)

      
    myGroup.defineHistogram('eta',title='eta',path=Path,xbins=50,xmin=-5,xmax=5)
    myGroup.defineHistogram('phi',title='phi',path=Path,xbins=50,xmin=-3.3,xmax=3.3)

    # Add histograms for L1 jets matched to offline/online jets
    if ismatched and not L1Fex:  # PS under development
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
