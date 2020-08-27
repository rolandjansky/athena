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

Chain2JetCollDict = dict() # set HLT jet collection for AT and legacy master HLT jet chains
JetCollections    = dict() # List of HLT jet collections for AT and legacy master

# AthenaMT
JetCollections['MT'] = [
  'HLT_AntiKt4EMTopoJets_subjesIS',                   # default small-R EM
  'HLT_AntiKt10JetRCJets_subjesIS',                   # a10r
  'HLT_AntiKt10LCTopoJets_subjes',                    # a10
  'HLT_AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets_jes', # a10t
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
  'HLT_j460_a10t_lcw_jes_L1J100'           : 'HLT_AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets_jes',
  'HLT_2j330_a10t_lcw_jes_35smcINF_L1J100' : 'HLT_AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets_jes',
  'HLT_j45_ftf_pf_L1J20'                   : 'HLT_AntiKt4EMPFlowJets_subjesIS_ftf',
  'HLT_j45_ftf_subjesgscIS_pf_L1J20'       : 'HLT_AntiKt4EMPFlowJets_subjesgscIS_ftf',
  'HLT_j45_ftf_subresjesgscIS_pf_L1J20'    : 'HLT_AntiKt4EMPFlowJets_subresjesgscIS_ftf',
  'HLT_j85_ftf_pf_L1J20'                   : 'HLT_AntiKt4EMPFlowJets_subjesIS_ftf',
  'HLT_j45_ftf_pf_nojcalib_L1J20'          : 'HLT_AntiKt4EMPFlowJets_nojcalib_ftf',
  'HLT_j45_ftf_csskpf_nojcalib_L1J20'      : 'HLT_AntiKt4EMPFlowCSSKJets_nojcalib_ftf',
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

#########################################################
# Schedule more histograms for dedicated jet collections
#########################################################
from JetMonitoring.JetMonitoringConfig import JetMonAlgSpec, HistoSpec, EventHistoSpec, SelectSpec, ToolSpec #VarSpec can be added to define specific/custom variables

# All offline jet collections
ExtraOfflineHists = [
  HistoSpec('HECFrac', (50,0,1), title="HECFrac;HEC fraction;Entries" ),
  HistoSpec('EMFrac', (50,0,1), title="EMFrac;EM fraction;Entries" ),
  HistoSpec('Jvt', (50,-0.1,1), title="JVT;JVT;Entries" ),
  "JVFCorr",
  "JvtRpt",
  "NumTrkPt1000[0]",
  "TrackWidthPt1000[0]",
  "SumPtTrkPt500[0]",
  SelectSpec( 'LooseBadFailedJets', 'LooseBad', InverseJetSel=True, FillerTools = ["pt","phi","eta"]),
]

# All online small-R jet collections
ExtraSmallROnlineHists = [
  HistoSpec('HECFrac', (50,0,1), title="HECFrac;HEC fraction;Entries" ),
  HistoSpec('EMFrac', (50,0,1), title="EMFrac;EM fraction;Entries" ),
  HistoSpec('DetectorEta', (100,-5,5), title="DetectorEta;Detector #eta;Entries" ), 
  HistoSpec('ActiveArea', (80,0,0.8), title="ActiveArea;Active Area;Entries" ), 
  HistoSpec('et:GeV;eta',  (100,0,750, 50,-5,5) , title='#eta vs E_{T};E_{T} [GeV];#eta;Entries'),
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

  # The following class will make a sequence, configure algorithms, and link
  # them to GenericMonitoringTools
  from AthenaMonitoring import AthMonitorCfgHelper
  helper = AthMonitorCfgHelper(inputFlags,'TrigJetMonitorAlgorithm')

  # This is the right place to get the info, but the autoconfig of the flag
  # is not yet implemented
  AthenaMT = ConfigFlags.Trigger.EDMDecodingVersion==3

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
    chainMonitorConf = jetChainMonitoringConfig(inputFlags,jetcoll,chain,AthenaMT)
    chainMonitorConf.toAlg(helper)

  # the AthMonitorCfgHelper returns an accumulator to be used by the general configuration system.
  return helper.result()


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

    EventHistoSpec('njetsPt20', (20,0,20), title='NJetsPt20;NJetsPt20;Entries' ),
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

   # Declare a configuration dictionnary for a JetContainer
   isOnline = True if 'HLT' in jetcoll else False
   conf     = basicJetMonAlgSpec(jetcoll,isOnline,athenaMT)
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

def jetChainMonitoringConfig(inputFlags,jetcoll,chain,athenaMT):
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

   from JetMonitoring.JetMonitoringConfig import retrieveVarToolConf
   trigConf = JetMonAlgSpec( # the usual JetMonAlgSpec 
       chain+"TrigMon",
       JetContainerName = jetcoll,
       TriggerChain = chain,
       defaultPath = chainFolder,
       topLevelDir="HLT/JetMon/Online/",
       bottomLevelDir=jetcollFolder,
       failureOnMissingContainer=True,
       )
   trigConf.appendHistos(
           "pt",
           "m",
           "eta",
           "et",
           "phi",
           # we pass directly the ToolSpec
           ToolSpec('JetHistoTriggEfficiency', chain,
                    # below we pass the Properties of this JetHistoTriggEfficiency tool :
                    Group='jetTrigGroup_'+chain,
                    Var=retrieveVarToolConf("pt"), # In this context we can not just pass a str alias to describe a histo variable
                                                   # so we use retrieveVarToolConf("pt") which returns a full specification for the "pt" histo variable.
                    ProbeTrigChain=chain,defineHistoFunc=defineHistoForJetTrigg),
   )

   if 'smc' in chain:
     trigConf.appendHistos(
             SelectSpec( 'm50', '50<m', chainFolder, FillerTools = [
               ToolSpec('JetHistoTriggEfficiency', chain,
                 Group='jetTrigGroup_'+chain+'_m50',
                 Var=retrieveVarToolConf("pt"), # In this context we can not just pass a str alias to describe a histo variable
                 ProbeTrigChain=chain,defineHistoFunc=defineHistoForJetTrigg
               ),
             ] ),
             SelectSpec( 'et500', '500<et', chainFolder, FillerTools = [
               ToolSpec('JetHistoTriggEfficiency', chain,
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
    chainMonitorConf = jetChainMonitoringConfig(ConfigFlags,jetcoll,chain,AthenaMT)
    chainMonitorConf.toAlg(helper)

  cfg.merge(helper.result())
  
  cfg.run()
