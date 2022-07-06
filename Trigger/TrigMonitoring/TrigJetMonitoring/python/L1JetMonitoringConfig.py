# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

class L1JetMonAlg():
  def __init__(self,
               name,
               jetcoll,
               triggerChain='',
               matched=False,
               matchedOfflineJets='',
               matchedHLTJets=''):
    
    self.name           = name
    self.L1JetContainer = jetcoll
    self.triggerChain   = triggerChain
    self.matched        = matched
    # self.matched        = False
    self.matchedOJ      = matchedOfflineJets
    self.matchedHLTJ    = matchedHLTJets

  def __str__(self):
    text = ' '.join(['%s: %s' % (str(k), str(v)) for k, v in self.__dict__.items()])
    return self.__class__.__name__ + ':: ' + text


  def toAlg(self,monhelper):
    from AthenaConfiguration.ComponentFactory import CompFactory

    jFexSR = self.L1JetContainer == 'L1_jFexSRJetRoI'
    jFexLR = self.L1JetContainer == 'L1_jFexLRJetRoI'
    gFexSR = self.L1JetContainer == 'L1_gFexSRJetRoI'
    gFexLR = self.L1JetContainer == 'L1_gFexLRJetRoI'

    noFex = self.L1JetContainer == 'LVL1JetRoIs'

    L1Fex = jFexSR or jFexLR or gFexSR or gFexLR

    assert L1Fex or noFex, 'unsupported L1Container %s' % self.L1JetContainer
    
    def container2tool_class_name():

      # Note: there are two gFex names, both correspond to the
      # same container type.

      # The strangeness of the class names in the following dict are
      # due to them arising from template instantiations.
      # They can be found in the TrigJetMonitoringConf.py module
      # which is constructed at build time.
      return  {
        'L1_jFexSRJetRoI': 'TrigL1FexJetMonitorTool_JTM_jFexSRJetRoIContainer_',
        'L1_jFexLRJetRoI': 'TrigL1FexJetMonitorTool_JTM_jFexLRJetRoIContainer_',
        'L1_gFexSRJetRoI': 'TrigL1FexJetMonitorTool_JTM_gFexJetRoIContainer_',
        'L1_gFexLRJetRoI': 'TrigL1FexJetMonitorTool_JTM_gFexJetRoIContainer_',
        'LVL1JetRoIs': 'TrigL1FexJetMonitorTool_JTM_JetRoIContainer_',
      }[self.L1JetContainer]
    
    def container2tool_inst_name():

      return  {'L1_jFexSRJetRoI': 'jFexSRDataRetriever',
               'L1_jFexLRJetRoI': 'jFexLRDataRetriever',
               'L1_gFexSRJetRoI': 'gFexSRDataRetriever',
               'L1_gFexLRJetRoI': 'gFexLRDataRetriever',
               'LVL1JetRoIs': 'JetRoIDataRetriever',
               }[self.L1JetContainer]
      

    def container2_monitorgroup():

      return  {'L1_jFexSRJetRoI': 'TrigL1JFexSRJetMonitor',
               'L1_jFexLRJetRoI': 'TrigL1JFexLRJetMonitor',
               'L1_gFexSRJetRoI': 'TrigL1GFexSRJetMonitor',
               'L1_gFexLRJetRoI': 'TrigL1GFexLRJetMonitor',
               'LVL1JetRoIs': 'TrigL1JetMonitor'}[self.L1JetContainer]
    
          
    alg = monhelper.addAlgorithm(CompFactory.TrigL1FexJetMonitorAlgorithm,
                                   self.name)
    toolClass = getattr(CompFactory, container2tool_class_name())

    tool = toolClass(container2tool_inst_name())
    tool.do_matching = self.matched
    tool.offlineJetsToMatch = self.matchedOJ
    tool.HLTJetsToMatch = self.matchedHLTJ
    tool.l1container = self.L1JetContainer
    alg.group_name = container2_monitorgroup()
    alg.filler = tool
    alg.TriggerChain = self.triggerChain
    
    # Add a generic monitoring tool (a "group" in old language). The returned 
    # object here is the standard GenericMonitoringTool

    Path  = self.L1JetContainer+'/'
    Path += 'NoTriggerSelection/' if self.triggerChain == '' else self.triggerChain+'/'
    myGroup = monhelper.addGroup(alg, container2_monitorgroup(),'HLT/JetMon/L1/')
 
    if L1Fex:
      myGroup.defineHistogram('et',title='et',path=Path,xbins=400,xmin=0.0,xmax=400.0)
    else:
      myGroup.defineHistogram('et8x8',title='et8x8',path=Path,xbins=400,xmin=0.0,xmax=400.0)

      
    myGroup.defineHistogram('eta',title='eta',path=Path,xbins=50,xmin=-5,xmax=5)
    myGroup.defineHistogram('phi',title='phi',path=Path,xbins=50,xmin=-3.3,xmax=3.3)

    if self.matched:
      for matchcoll,tag in [ [self.matchedOJ, 'off'], [self.matchedHLTJ, 'hlt'] ]:
        Path = self.L1JetContainer + '/NoTriggerSelection/MatchedJets_' + matchcoll + '/'

        # define which variable difference will be plotted,
        # mass difference makes no sense for L1 as m=0
        for histname in [ 'ptdiff', 'energydiff' ]: 
          myGroup.defineHistogram(tag+histname+';'+histname,
                                  title=histname, type="TH1F", path=Path,
                                  xbins=140 , xmin=-120000., xmax=80000. ,)
        
        for histname in [ 'ptresp', 'energyresp' ]:
          myGroup.defineHistogram(tag+histname+';'+histname,
                                  title=histname,
                                  type="TH1F", path=Path,
                                  xbins=100 , xmin=-1., xmax=2. ,)
          
        myGroup.defineHistogram(tag+'ptresp,'+tag+'ptref;ptresp_vs_ptRef',
                                title='ptresponse vs ptRef', type="TH2F", path=Path,
                                xbins=10 , xmin=-1., xmax=2.,
                                ybins=10, ymin=0., ymax=500000.,)
        
        myGroup.defineHistogram(tag+'ptresp,'+tag+'etaref;ptresp_vs_etaRef',
                                title='ptresponse vs etaRef', type="TH2F", path=Path,
                                xbins=10, xmin=-1., xmax=2.,
                                ybins=10, ymin=-5., ymax=5.,)


    return alg
