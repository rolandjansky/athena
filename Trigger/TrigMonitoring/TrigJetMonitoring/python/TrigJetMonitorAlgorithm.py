#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''@file MTMonitoring.py
@authors P-A. Delsart, Jona Bossio
@date    03/04/2020
@brief   Python configuration for the Run III Trigger Jet Monitoring
'''

import sys,argparse

#####################################
# Offline jet collections to monitor
#####################################

OfflineJetCollections = [
  'AntiKt4EMTopoJets',
  'AntiKt4EMPFlowJets',
]

###########################################
# L1 jet collections and chains to monitor
###########################################

L1JetCollections = ['LVL1JetRoIs']
Chain2L1JetCollDict = { # set L1 jet collection name for L1 jet chains
  'L1_J15'  : 'LVL1JetRoIs',
  'L1_J20'  : 'LVL1JetRoIs',
  'L1_J100' : 'LVL1JetRoIs',
}


############################################
# HLT jet collections and chains to monitor
############################################

Chain2JetCollDict  = dict() # set HLT jet collection for AT and legacy master HLT jet chains
JetCollections     = dict() # List of HLT jet collections for AT and legacy master
TurnOnCurves       = dict() # List reference chains and offline jet collections to be used for producing turn-on curves
JetColls2Match     = dict()

# AthenaMT
JetCollections['MT'] = [
  'HLT_AntiKt4EMTopoJets_subjesIS',                   # default small-R EM
  'HLT_AntiKt10JetRCJets_subjesIS',                   # a10r
  'HLT_AntiKt10LCTopoJets_subjes',                    # a10
  'HLT_AntiKt10LCTopoTrimmedPtFrac4SmallR20Jets_jes', # a10t
  'HLT_AntiKt4EMPFlowJets_subjesIS_ftf',              # pflow w/o calo+track GSC
  'HLT_AntiKt4EMPFlowJets_subjesgscIS_ftf',           # pflow w/ calo+track GSC
  'HLT_AntiKt4EMPFlowJets_subresjesgscIS_ftf',        # pflow w/ residual + calo+track GSC
  'HLT_AntiKt4EMPFlowJets_nojcalib_ftf',              # pflow nojcalib
  'HLT_AntiKt4EMPFlowCSSKJets_nojcalib_ftf',          # pflow cssk nojcalib
]
Chain2JetCollDict['MT'] = {
  'HLT_j420_L1J100'                        : 'HLT_AntiKt4EMTopoJets_subjesIS',
  'HLT_j260_320eta490_L1J75_31ETA49'       : 'HLT_AntiKt4EMTopoJets_subjesIS',
  'HLT_5j70_0eta240_L14J20'                : 'HLT_AntiKt4EMTopoJets_subjesIS',
  'HLT_3j200_L1J100'                       : 'HLT_AntiKt4EMTopoJets_subjesIS',
  'HLT_j460_a10r_L1J100'                   : 'HLT_AntiKt10JetRCJets_subjesIS',
  'HLT_j460_a10_lcw_subjes_L1J100'         : 'HLT_AntiKt10LCTopoJets_subjes',
  'HLT_j460_a10t_lcw_jes_L1J100'           : 'HLT_AntiKt10LCTopoTrimmedPtFrac4SmallR20Jets_jes',
  'HLT_2j330_a10t_lcw_jes_35smcINF_L1J100' : 'HLT_AntiKt10LCTopoTrimmedPtFrac4SmallR20Jets_jes',
  'HLT_j45_pf_ftf_L1J20'                   : 'HLT_AntiKt4EMPFlowJets_subjesIS_ftf',
  'HLT_j45_pf_subjesgscIS_ftf_L1J20'       : 'HLT_AntiKt4EMPFlowJets_subjesgscIS_ftf',
  'HLT_j85_pf_ftf_L1J20'                   : 'HLT_AntiKt4EMPFlowJets_subjesIS_ftf',
  'HLT_j45_pf_nojcalib_ftf_L1J20'          : 'HLT_AntiKt4EMPFlowJets_nojcalib_ftf',
  'HLT_j45_csskpf_nojcalib_ftf_L1J20'      : 'HLT_AntiKt4EMPFlowCSSKJets_nojcalib_ftf',
}
TurnOnCurves['MT'] = { # ref chain, offline jet coll
  'HLT_j420_L1J100'                        : ['HLT_j80_L1J15','AntiKt4EMTopoJets'],
  'HLT_3j200_L1J100'                       : ['HLT_j80_L1J15','AntiKt4EMTopoJets'],
  'HLT_j460_a10r_L1J100'                   : ['HLT_j80_L1J15','AntiKt4EMTopoJets'],
  'HLT_j460_a10_lcw_subjes_L1J100'         : ['HLT_j80_L1J15','AntiKt4EMTopoJets'],
  'HLT_j460_a10t_lcw_jes_L1J100'           : ['HLT_j80_L1J15','AntiKt4EMTopoJets'],
  'HLT_2j330_a10t_lcw_jes_35smcINF_L1J100' : ['HLT_j80_L1J15','AntiKt4EMTopoJets'],
  'HLT_j85_pf_ftf_L1J20'                   : ['HLT_j45_pf_ftf_L1J20','AntiKt4EMPFlowJets'],
}
JetColls2Match['MT'] = {
  'HLT_AntiKt4EMTopoJets_subjesIS'            : 'AntiKt4EMPFlowJets',
  'HLT_AntiKt4EMPFlowJets_subjesIS_ftf'       : 'AntiKt4EMPFlowJets',
  'HLT_AntiKt4EMPFlowJets_subjesgscIS_ftf'    : 'AntiKt4EMPFlowJets',
  'HLT_AntiKt4EMPFlowJets_subresjesgscIS_ftf' : 'AntiKt4EMPFlowJets',
}

# Legacy
JetCollections['Legacy'] = [
  'HLT_xAOD__JetContainer_a4tcemsubjesISFS',    # default small-R
  'HLT_xAOD__JetContainer_a10r_tcemsubjesISFS', # a10r
  'HLT_xAOD__JetContainer_a10tclcwsubjesFS',    # a10
  'HLT_xAOD__JetContainer_a10ttclcwjesFS',      # a10t
]
Chain2JetCollDict['Legacy'] = {
  'HLT_j420'                               : 'HLT_xAOD__JetContainer_a4tcemsubjesISFS',
  'HLT_j260_320eta490'                     : 'HLT_xAOD__JetContainer_a4tcemsubjesISFS',
  'HLT_j460_a10r_L1J100'                   : 'HLT_xAOD__JetContainer_a10r_tcemsubjesISFS',
  'HLT_j460_a10_lcw_subjes_L1J100'         : 'HLT_xAOD__JetContainer_a10tclcwsubjesFS',
  'HLT_j460_a10t_lcw_jes_L1J100'           : 'HLT_xAOD__JetContainer_a10ttclcwjesFS',
  'HLT_2j330_a10t_lcw_jes_35smcINF_L1J100' : 'HLT_xAOD__JetContainer_a10ttclcwjesFS',
  'HLT_5j70_0eta240'                       : 'HLT_xAOD__JetContainer_a4tcemsubjesISFS',
  'HLT_3j200'                              : 'HLT_xAOD__JetContainer_a4tcemsubjesISFS',
}
TurnOnCurves['Legacy'] = { # ref chain, offline jet coll
  'HLT_j420'                               : ['HLT_j175','AntiKt4EMTopoJets'],
  'HLT_j260_320eta490'                     : ['HLT_j45_320eta490','AntiKt4EMTopoJets'],
  'HLT_j460_a10r_L1J100'                   : ['HLT_j175','AntiKt4EMTopoJets'],
  'HLT_j460_a10_lcw_subjes_L1J100'         : ['HLT_j175','AntiKt4EMTopoJets'],
  'HLT_j460_a10t_lcw_jes_L1J100'           : ['HLT_j175','AntiKt4EMTopoJets'],
  'HLT_2j330_a10t_lcw_jes_35smcINF_L1J100' : ['HLT_j175','AntiKt4EMTopoJets'],
  'HLT_3j200'                              : ['HLT_j175','AntiKt4EMTopoJets'],
}

#########################################################
# Helpful functions
#########################################################

def getEtaRange(chain):
  if 'eta' in chain:
    etaParts    = chain.split('eta')
    etaMinTemp  = etaParts[0].split('_')
    etaMin      = etaMinTemp[len(etaMinTemp)-1]
    etaMin      = int(etaMin)/10
    etaMax      = etaParts[1].split('_')[0]
    etaMax      = int(etaMax)/10
    return etaMin,etaMax
  else: # by default central
    return 0,2.5

#########################################################
# Schedule more histograms for dedicated jet collections
#########################################################
from JetMonitoring.JetMonitoringConfig import JetMonAlgSpec, HistoSpec, EventHistoSpec, SelectSpec, ToolSpec #VarSpec can be added to define specific/custom variables

# All offline jet collections
ExtraOfflineHists = [
  "EMFrac",
  "HECFrac",
  "Jvt",
  "JVFCorr",
  "JvtRpt",
  "NumTrkPt1000[0]",
  "TrackWidthPt1000[0]",
  "SumPtTrkPt500[0]",
  SelectSpec( 'LooseBadFailedJets', 'LooseBad', InverseJetSel=True, FillerTools = ["pt","phi","eta"]),
]

# All online small-R jet collections
ExtraSmallROnlineHists = [
  HistoSpec('et:GeV;eta',  (100,0,750, 50,-5,5) , title='#eta vs E_{T};E_{T} [GeV];#eta;Entries'),
  "EMFrac",
  "HECFrac",
  "DetectorEta",
  "ActiveArea", 
  "EM3Frac",
  "Tile0Frac",
]

# All online large-R jet collections
ExtraLargeROnlineHists = [
]

# Kinematics at different scales for offline and small-R online jet collections
OfflineScaleMomenta = [ "ConstitScale", "EMScale", "PileupScale", "EtaJESScale"]
OnlineScaleMomenta  = [ "ConstitScale" ]
for var in [ "pt", "eta", "m" ]:
  for offlinescale in OfflineScaleMomenta:
    ExtraOfflineHists.append("Jet"+offlinescale+"Momentum_"+var)
  for onlinescale in OnlineScaleMomenta:
    ExtraSmallROnlineHists.append("Jet"+onlinescale+"Momentum_"+var)


from AthenaConfiguration.AllConfigFlags import ConfigFlags

def TrigJetMonConfig(inputFlags):

  # This is the right place to get the info, but the autoconfig of the flag
  # is not yet implemented
  AthenaMT = ConfigFlags.Trigger.EDMDecodingVersion==3

  # AthenaMT or Legacy
  InputType = 'MT' if AthenaMT else 'Legacy'

  from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
  cfg = ComponentAccumulator()

  # Match HLT to offline jets
  for j1,j2 in JetColls2Match[InputType].items():
    from AthenaConfiguration.ComponentFactory import CompFactory
    name = 'Matching_{}_{}'.format(j1,j2)
    alg = CompFactory.JetMatcherAlg(name, JetContainerName1=j1,JetContainerName2=j2)
    cfg.addEventAlgo(alg)

  # The following class will make a sequence, configure algorithms, and link
  # them to GenericMonitoringTools
  from AthenaMonitoring import AthMonitorCfgHelper
  helper = AthMonitorCfgHelper(inputFlags,'TrigJetMonitorAlgorithm')

  # Loop over L1 jet collectoins
  for jetcoll in L1JetCollections:
    l1jetconf = l1JetMonitoringConfig(ConfigFlags,jetcoll)
    l1jetconf.toAlg(helper)

  # Loop over L1 jet chains
  for chain,jetcoll in Chain2L1JetCollDict.items():
    l1chainconf = l1JetMonitoringConfig(ConfigFlags,jetcoll,chain)
    l1chainconf.toAlg(helper)

  # Loop over offline jet collections
  for jetcoll in OfflineJetCollections:
    offlineMonitorConf = jetMonitoringConfig(inputFlags,jetcoll,AthenaMT)
    offlineMonitorConf.toAlg(helper)

  # Loop over HLT jet collections
  for jetcoll in JetCollections[InputType]:
    monitorConf = jetMonitoringConfig(inputFlags,jetcoll,AthenaMT)
    # then we turn the full specification into properly configured algorithm and tools.
    # we use the method 'toAlg()' defined for the specialized dictionnary 'JetMonAlgSpec'
    monitorConf.toAlg(helper)

  # Loop over HLT jet chains
  for chain,jetcoll in Chain2JetCollDict[InputType].items():
    # kinematic plots
    if AthenaMT:
      chainMonitorConfT = jetChainMonitoringConfig(inputFlags,jetcoll,chain,AthenaMT,True)
      chainMonitorConfT.toAlg(helper)
    chainMonitorConfF = jetChainMonitoringConfig(inputFlags,jetcoll,chain,AthenaMT,False)
    chainMonitorConfF.toAlg(helper)
    # efficiency plots
    refChain       = 'NONE'
    offlineJetColl = 'NONE'
    if chain in TurnOnCurves[InputType]:
      refChain       = TurnOnCurves[InputType][chain][0]
      offlineJetColl = TurnOnCurves[InputType][chain][1]
    if offlineJetColl != 'NONE' and refChain != 'NONE':
      effMonitorConf = jetEfficiencyMonitoringConfig(inputFlags,jetcoll,offlineJetColl,chain,refChain,AthenaMT)
      effMonitorConf.toAlg(helper)

  cfg.merge(helper.result())
  return cfg


# Basic selection of histograms common for online and offline jets
def basicJetMonAlgSpec(jetcoll,isOnline,athenaMT):
  # we use a specialized dictionnary (JetMonAlgSpec) which will be translated into the final C++ tool
  path = 'NoTriggerSelection' if isOnline else 'standardHistos/'

  TopLevelDir  = 'HLT/JetMon/'
  TopLevelDir += 'Online/' if isOnline else 'Offline/'

  # Remap online Run 2 jet collections
  from TrigJetMonitoring import JetCollRemapping
  jetcollFolder = jetcoll
  if jetcoll in JetCollRemapping.JetCollRun2ToRun3 and not athenaMT:
    jetcollFolder = JetCollRemapping.JetCollRun2ToRun3[jetcoll]
  Conf = JetMonAlgSpec(jetcoll+"Mon",JetContainerName = jetcoll, defaultPath = path, topLevelDir=TopLevelDir, bottomLevelDir=jetcollFolder, failureOnMissingContainer=False)

  # Now start filling the histo spec list    
  Conf.appendHistos(
    # See knownHistos in JetStandardHistoSpecs.py for the list of standard specification.
    "pt",  
    "m",
    "eta",
    "phi",
    "e",
    "et",
    # or we can directly add our custom histo specification in the form of a HistoSpec:
    # the basic call is : HistoSpec( variable, histobins, title='histotile;xtitle,ytitle')
    
    # Say we want a 2nd 'pt' plot but with a different binning than in the standard spec.
    # WARNING : we can not re-use the same spec name in a given JetMonitoringAlg !!!
    # so we give a new name AND we specify the actual variable with the argument 'xvar'
    #   (the ':GeV' means the variable is to be set at GeV scale)
    #HistoSpec( 'lowpt',  (100,0,150) , title='p_{T};p_{T} [GeV];', xvar='pt:GeV'),            
    # An equivalent solution would have been to clone the existing spec like in :
    # knownHistos.pt.clone('lowpt',bins= (100,0,200) ),

    # 2D histos are usually refered to by concatenating vars with a ';' as in 'varx;vary' 
    # if the 'vax;vary' alias doesn't exist in knownHistos but 'varx' and 'vary'
    # do exist, then a spec fot 'vax;vary' will be automatically generated.
    "pt;m",    # mass vs pt
    "eta;phi", # phi vs eta
    "eta;e",   # energy vs eta
    "phi;e",   # energy vs phi

    SelectSpec( 'central', '|eta|<3.2', path, FillerTools = ["pt","et","m"] ),
    SelectSpec( 'forward', '3.2<|eta|', path, FillerTools = ["pt","et","m"] ),
    SelectSpec( 'lowmu', 'avgMu<30', path, isEventVariable=True, FillerTools = ["pt","et","m","phi","eta"]),
    SelectSpec( 'highmu', '30<avgMu', path, isEventVariable=True, FillerTools = ["pt","et","m","phi","eta"]),

    EventHistoSpec('njets', (25,0,25), title='NJets;NJets;Entries' ),
    EventHistoSpec('njetsPt20', (25,0,25), title='NJetsPt20;NJetsPt20;Entries' ),
    EventHistoSpec('njetsPt50', (25,0,25), title='NJetsPt50;NJetsPt50;Entries' ),
    # Jet multiplicity histograms can be added by using an EventHistoSpec
    # Their specifications (pT cut, ET cut, eta cuts) must be defined in the knownEventVar dictionary within JetStandardHistoSpecs.py
    # The following line is an example for a jet multiplicity histogram with ET>40 GeV, 1.0<|eta|<2.0, and binning of (10,0,10):
    # EventHistoSpec('njetsEt40Eta1_2', (10,0,10), title='NJetsEt40Eta1_2;NJetsEt40Eta1_2;Entries' ),

    # TProfile2D : just use 3 variables. For now the sytem will automatically
    #  interpret it as a TProfile2D (the 3rd variable being profiled)
    #"phi;eta;e", # --> Average Energy vs pt and eta
     
    # another possible selections : only sub-leading jets and highJVF
    #SelectSpec( 'subleading',
    #            '', # no selection on variables
    #            SelectedIndex=1, # force 2nd (sub-leading) jet (we would set 0 for leading jets)
    #            path='standardHistos', # force the path where the histos are saved in the final ROOT file
    #            FillerTools = [
    #                "pt",
    #                "m",
    #            ] ),
    #SelectSpec( 'highJVF',
    #            '0.3<JVF[0]', # JVF is a vector<float> for each jets. Here we cut on the 0th entry of this vector
    #            FillerTools = [
    #                "pt",
    #            ] ),
  )

  return Conf


def jetMonitoringConfig(inputFlags,jetcoll,athenaMT):
   '''Function to configures some algorithms in the monitoring system.'''

   isOnline  = True if 'HLT' in jetcoll else False
   InputType = 'Legacy' if not athenaMT else 'MT'
   conf      = basicJetMonAlgSpec(jetcoll,isOnline,athenaMT)

   def defineHistoForJetMatch(conf, parentAlg, monhelper , path):
       # create a monitoring group with the histo path starting from the parentAlg
       print('toplevel = '+str(conf.topLevelDir))
       print('bottomlevel = '+str(conf.bottomLevelDir))
       group = monhelper.addGroup(parentAlg, conf.Group, conf.topLevelDir+'/'+conf.bottomLevelDir+'/NoTriggerSelection/')
       # define the histogram
       group.defineHistogram('ptdiff',title='title', type="TH1F", path='MatchedJets_{}'.format(JetColls2Match[InputType][jetcoll]), xbins=100 , xmin=-100000, xmax=100000. ,)

   # Declare a configuration dictionnary for a JetContainer
   if isOnline:
     if 'AntiKt4' in jetcoll or 'a4tcem' in jetcoll:
       for hist in ExtraSmallROnlineHists: conf.appendHistos(hist)
       if 'ftf' in jetcoll: # dedicated histograms for FTF chains
         conf.appendHistos("JVFCorr")
         conf.appendHistos("JvtRpt")
         conf.appendHistos("SumPtTrkPt500[0]")
         conf.appendHistos("NumTrkPt1000[0]")
         conf.appendHistos("TrackWidthPt1000[0]")
         if 'PF' in jetcoll: # dedicated histograms for online PFlow jets
           conf.appendHistos("SumPtChargedPFOPt500[0]")
           conf.appendHistos("fCharged")
     else:
       for hist in ExtraLargeROnlineHists: conf.appendHistos(hist)
     # Add matched jets plots
     if jetcoll in JetColls2Match[InputType]:
       matchedJetColl = JetColls2Match[InputType][jetcoll]
       name           = 'jetMatched_{}_{}'.format(jetcoll,matchedJetColl)
       jetmatchKey    = '{}.matched_{}'.format(jetcoll,matchedJetColl)
       jetptdiffKey   = '{}.ptdiff_{}'.format(jetcoll,matchedJetColl)
       conf.appendHistos(ToolSpec('JetHistoMatchedFiller',name,JetMatchedKey=jetmatchKey,JetPtDiffKey=jetptdiffKey,defineHistoFunc=defineHistoForJetMatch,Group='matchedJets_'+jetcoll))
   else: # offline
     for hist in ExtraOfflineHists: conf.appendHistos(hist)
     if 'PF' in jetcoll: # dedicated histograms for offline PFlow jets
       conf.appendHistos("SumPtChargedPFOPt500[0]")
       conf.appendHistos("fCharged")

   return conf

def l1JetMonitoringConfig(inputFlags,jetcoll,chain=''):
  from TrigJetMonitoring.L1JetMonitoringConfig import L1JetMonAlg
  name = jetcoll if chain=='' else jetcoll+'_'+chain
  conf = L1JetMonAlg(name,jetcoll,chain)
  return conf

def jetChainMonitoringConfig(inputFlags,jetcoll,chain,athenaMT,onlyUsePassingJets=True):
   '''Function to configures some algorithms in the monitoring system.'''

   # Remap online Run 2 jet collections
   from TrigJetMonitoring import JetCollRemapping
   jetcollFolder = jetcoll
   if jetcoll in JetCollRemapping.JetCollRun2ToRun3 and not athenaMT:
     jetcollFolder = JetCollRemapping.JetCollRun2ToRun3[jetcoll]

   # Remap Run 2 jet chain name to Run 3 jet chain
   from TrigJetMonitoring import JetChainRemapping
   if chain in JetChainRemapping.JetChainRun2ToRun3:
     chainFolder = JetChainRemapping.JetChainRun2ToRun3[chain]
   else:
     chainFolder = chain

   if not athenaMT:
     onlyUsePassingJets = False #does not work for legacy samples yet
   jetMonAlgSpecName = chain+"TrigMon"
   if not onlyUsePassingJets:
     chainFolder = chainFolder + "/ExpertHistos"
     jetMonAlgSpecName = jetMonAlgSpecName + "_ExpertHistos"

   # Define helper functions to automatize ET & eta selection strings for NJet histograms of chains
   def getThreshold(parts):
     return parts[1].split('_')[0]

   def getEtaRangeString(chain):
     if 'eta' in chain:
       etaParts    = chain.split('eta')
       etaMinTemp  = etaParts[0].split('_')
       etaMin      = etaMinTemp[len(etaMinTemp)-1]
       if int(etaMin) > 0 : etaMin = str(int(int(etaMin)/10))
       etaMax      = etaParts[1].split('_')[0]
       if int(etaMax) > 0 : etaMax = str(int(int(etaMax)/10))
       return 'Eta{}_{}'.format(etaMin,etaMax)
     else: return 'Eta0_32'

   def getNjetHistName(chain):
     parts         = chain.split('j')
     # check if it is a multi-threshold multijet chain or a single-threshold multijet chain
     multiplicity  = parts[0].split('_')[1] # for single-threshold multijet chains
     if (chain.count('_j')-chain.count('_jes')) > 1  or multiplicity != '':
       return 'njetsEt{}{}'.format(getThreshold(parts),getEtaRangeString(chain))
     else: return 'NONE'

   trigConf = JetMonAlgSpec( # the usual JetMonAlgSpec 
       jetMonAlgSpecName,
       JetContainerName = jetcoll,
       TriggerChain = chain,
       defaultPath = chainFolder,
       topLevelDir="HLT/JetMon/Online/",
       bottomLevelDir=jetcollFolder,
       failureOnMissingContainer=True,
       onlyPassingJets=onlyUsePassingJets,
       )
   trigConf.appendHistos(
           "pt",
           "m",
           "eta",
           "et",
           "phi",
           EventHistoSpec('njets', (25,0,25), title='njets;njets;Entries' ),
           EventHistoSpec('njetsEt20Eta0_32', (25,0,25), title='njetsEt20Eta0_32;njetsEt20Eta0_32;Entries' ),
           EventHistoSpec('njetsEt50Eta0_32', (25,0,25), title='njetsEt50Eta0_32;njetsEt50Eta0_32;Entries' ),
   )
   NjetHistName = getNjetHistName(chain)
   from JetMonitoring.JetStandardHistoSpecs import knownEventVar
   if knownEventVar.get(NjetHistName,None) is not None:
     trigConf.appendHistos(
       EventHistoSpec(NjetHistName, (25,0,25), title=NjetHistName+';'+NjetHistName+';Entries' ),
     )

   return trigConf

def jetEfficiencyMonitoringConfig(inputFlags,onlinejetcoll,offlinejetcoll,chain,refChain,athenaMT):
   '''Function to configures some algorithms in the monitoring system.'''

   # Remap online Run 2 jet collections
   from TrigJetMonitoring import JetCollRemapping
   jetcollFolder = onlinejetcoll
   if onlinejetcoll in JetCollRemapping.JetCollRun2ToRun3 and not athenaMT:
     jetcollFolder = JetCollRemapping.JetCollRun2ToRun3[onlinejetcoll]

   # Remap Run 2 jet chain name to Run 3 jet chain
   from TrigJetMonitoring import JetChainRemapping
   if chain in JetChainRemapping.JetChainRun2ToRun3:
     chainFolder = JetChainRemapping.JetChainRun2ToRun3[chain]
   else:
     chainFolder = chain

   # We schedule a new JetAlg which will be acting only when a TriggerChain fired (using the TriggerChain from the base classes).
   # We'll plot 1 histo build by a dedicated JetHistoTriggEfficiency tool.
   # So we'll have to explicitely give a specification via the generic dicionnary 'ToolSpec'
   # This implies defining a little function which declares to the monitoring framework which variables to histogram and how.
   #  this is done here.
   def defineHistoForJetTrigg(conf, parentAlg, monhelper , path):
       # create a monitoring group with the histo path starting from the parentAlg
       group = monhelper.addGroup(parentAlg, conf.Group, conf.topLevelDir+jetcollFolder+'/')
       # define the histogram
       group.defineHistogram('trigPassed,jetVar',title='titletrig', type="TEfficiency", path=chainFolder, xbins=100 , xmin=0, xmax=500000. ,)

   # Get jet index and eta selection for offline jets
   parts        = chain.split('j')
   multiplicity = parts[0].split('_')[1]
   if multiplicity != '': index = int(multiplicity) - 1 # single-threhold multijet chains
   else: index = 0 # single-jet chain
   etaMin,etaMax = getEtaRange(chain)

   from JetMonitoring.JetMonitoringConfig import retrieveVarToolConf
   trigConf = JetMonAlgSpec( # the usual JetMonAlgSpec 
       chain+"TrigEffMon",
       JetContainerName          = offlinejetcoll,
       TriggerChain              = refChain, # reference chain
       defaultPath               = chainFolder,
       topLevelDir               = "HLT/JetMon/Online/",
       bottomLevelDir            = jetcollFolder,
       failureOnMissingContainer = True,
       onlyPassingJets           = False,
       )
   trigConf.appendHistos(
       SelectSpec( 'eff', '{}<|eta|<{}'.format(etaMin,etaMax), chainFolder, SelectedIndex=index, FillerTools = [
           # we pass directly the ToolSpec
           ToolSpec('JetHistoTriggEfficiency', chain,
                    # below we pass the Properties of this JetHistoTriggEfficiency tool :
                    Group='jetTrigGroup_'+chain,
                    Var=retrieveVarToolConf("pt"), # In this context we can not just pass a str alias to describe a histo variable
                                                   # so we use retrieveVarToolConf("pt") which returns a full specification for the "pt" histo variable.
                    ProbeTrigChain=chain,defineHistoFunc=defineHistoForJetTrigg),
       ] ),
   )

   if 'smc' in chain:
     trigConf.appendHistos(
             SelectSpec( 'm50', '50<m', chainFolder, FillerTools = [
               ToolSpec('JetHistoTriggEfficiency', chain+'_m50',
                 Group='jetTrigGroup_'+chain+'_m50',
                 Var=retrieveVarToolConf("pt"), # In this context we can not just pass a str alias to describe a histo variable
                 ProbeTrigChain=chain,defineHistoFunc=defineHistoForJetTrigg
               ),
             ] ),
             SelectSpec( 'et500', '500<et', chainFolder, FillerTools = [
               ToolSpec('JetHistoTriggEfficiency', chain+'_et500',
                 Group='jetTrigGroup_'+chain+'_et500',
                 Var=retrieveVarToolConf("m"), # In this context we can not just pass a str alias to describe a histo variable
                 ProbeTrigChain=chain,defineHistoFunc=defineHistoForJetTrigg
               ),
             ] ),
     )

   return trigConf

if __name__=='__main__':

  # Read arguments
  parser = argparse.ArgumentParser()
  parser.add_argument('--athenaMT', action='store_true', dest='athenaMT', default=False)
  parser.add_argument('--legacy',   action='store_true', dest='legacy',   default=False)
  parser.add_argument('--input',    action='store',      dest='inputFile')
  args     = parser.parse_args()
  AthenaMT = args.athenaMT
  Legacy   = args.legacy
  # Protections
  if AthenaMT and Legacy:
    print('ERROR: Choose AthenaMT or Legacy, exiting')
    sys.exit(0)
  elif not AthenaMT and not Legacy:
    print('ERROR: Choose AthenaMT or Legacy, exiting')
    sys.exit(0)

  # Input file
  if args.inputFile is not None: inputFile = args.inputFile
  else:
    print('ERROR: No input file provided, exiting')
    sys.exit(0)

  # Setup the Run III behavior
  from AthenaCommon.Configurable import Configurable
  Configurable.configurableRun3Behavior = 1

  # Setup logs
  from AthenaCommon.Logging import log
  from AthenaCommon.Constants import INFO #,DEBUG 
  log.setLevel(INFO)

  # Set the Athena configuration flags
  ConfigFlags.Input.Files = [inputFile]
  ConfigFlags.Input.isMC = True
  ConfigFlags.Output.HISTFileName = 'AthenaMTMonitorOutput.root' if AthenaMT else 'LegacyMonitoringOutput.root'
  ConfigFlags.lock()

  # Initialize configuration object, add accumulator, merge, and run.
  from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
  from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
  cfg = MainServicesCfg(ConfigFlags)
  cfg.merge(PoolReadCfg(ConfigFlags))

  # The following class will make a sequence, configure algorithms, and link
  # them to GenericMonitoringTools
  from AthenaMonitoring import AthMonitorCfgHelper
  helper = AthMonitorCfgHelper(ConfigFlags,'TrigJetMonitorAlgorithm')

  # AthenaMT or Legacy
  InputType = 'MT' if AthenaMT else 'Legacy'

  # Loop over L1 jet collectoins
  for jetcoll in L1JetCollections:
    l1jetconf = l1JetMonitoringConfig(ConfigFlags,jetcoll)
    l1jetconf.toAlg(helper)

  # Loop over L1 jet chains
  for chain,jetcoll in Chain2L1JetCollDict.items():
    l1chainconf = l1JetMonitoringConfig(ConfigFlags,jetcoll,chain)
    l1chainconf.toAlg(helper)

  # Loop over offline jet collections
  for jetcoll in OfflineJetCollections:
    offlineMonitorConf = jetMonitoringConfig(ConfigFlags,jetcoll,AthenaMT)
    offlineMonitorConf.toAlg(helper)

  # Loop over HLT jet collections
  for jetcoll in JetCollections[InputType]:
    monitorConf = jetMonitoringConfig(ConfigFlags,jetcoll,AthenaMT)
    # then we turn the full specification into properly configured algorithm and tools.
    # we use the method 'toAlg()' defined for the specialized dictionnary 'JetMonAlgSpec'
    monitorConf.toAlg(helper)

  # Loop over HLT jet chains
  for chain,jetcoll in Chain2JetCollDict[InputType].items():
    if AthenaMT:
      chainMonitorConfT = jetChainMonitoringConfig(ConfigFlags,jetcoll,chain,AthenaMT,True)
      chainMonitorConfT.toAlg(helper)
    chainMonitorConfF = jetChainMonitoringConfig(ConfigFlags,jetcoll,chain,AthenaMT,False)
    chainMonitorConfF.toAlg(helper)

    # Produce efficiency plots
    refChain       = 'NONE'
    offlineJetColl = 'NONE'
    if chain in TurnOnCurves[InputType]:
      refChain       = TurnOnCurves[InputType][chain][0]
      offlineJetColl = TurnOnCurves[InputType][chain][1]
    if offlineJetColl != 'NONE' and refChain != 'NONE':
      effMonitorConf = jetEfficiencyMonitoringConfig(ConfigFlags,jetcoll,offlineJetColl,chain,refChain,AthenaMT)
      effMonitorConf.toAlg(helper)

  cfg.merge(helper.result())
  
  cfg.run()
