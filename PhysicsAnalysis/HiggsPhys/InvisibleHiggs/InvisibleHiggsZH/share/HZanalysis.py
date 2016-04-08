## setup ####################################
# include EventView environment, always necessary to use EventView tools
include("EventViewConfiguration/EventViewFullInclude_jobOptions.py")
include( "TrigEventAthenaPool/TrigEventAthenaPool_joboptions.py" )
include( "TrigSteeringTestAthenaPool/TrigSteeringTestAthenaPool_joboptions.py" )

# import modules to use, necessary since we'll use python modules in these packages
from EventViewConfiguration import *
from TopView import *

# Load TopView library, necessary because we'll use tools in TopView library
theApp.Dlls += ["TopView"]
theApp.Dlls+= ['TrackIsolationTools'];
#theApp.Dlls += ["HighPtView"]
theApp.EvtMax = -1

# you can skip events by using the following
# EventSelector.SkipEvents = 10

# the top sequencer which manages algorithms
theJob = AlgSequence()

# load library from EventViewTrigger and prepare trigger objects
include("HighPtView/TriggerPrep_jobOptions.py")

# define EventView looper and schedule to theJob
# top EventView looper which manages EV toools
RecoLooper = EVMultipleOutputToolLooper("RecoLooper")

# add EV looper to theJob 
theJob += RecoLooper

# name the EventView container created by this looper, useful when matching
RecoLooper.EventViewCollectionOutputName="RecoView"

## Insertion of Objects ############################
## from HighPtView import *
## RecoLooper += HighPtSingleInserters("Inserters")

# insert Muon
RecoLooper += anEVTool("EVMuonInserter/MuonInserter") 
RecoLooper.MuonInserter.setProperties(
##  ContainerKey = "MuidMuonCollection",
  ContainerKey = "StacoMuonCollection",
  etCut =  10*GeV, ## default was 20
  requireCombinedMuon=True,
  onlyHighPt=False,                 ## Ignore low-pt algorithm
  useIsolation=False,               ## switch for isolation
  useChi2=False,                    ## chi2
  useMatchChi2=False,               ## chi2
  requireBestMatch=True,           ## require best match between spectrometer and ID  
#  DoPreselection=False,
InsertedLabels=["Muon", "Lepton"]
)

# insert Electron
RecoLooper += anEVTool("EVElectronInserter/ElectronInserter") 
RecoLooper.ElectronInserter.setProperties(
  ContainerKey = "ElectronCollection",
  etCut = 15*GeV,  ## default was 20
  onlyEgamma = True,
  authorCut = False,
  useTRT = False,
  useNN = False,
  useIsolation = False,
  useIsEM = False,
##  isEMMasks=[0x3FF],  ## medium cuts
  isEMMasks=[0x7],      ## loose cuts
  InsertedLabels=["Electron", "Lepton"]
)

# insert photons
RecoLooper += anEVTool("EVPhotonInserter/PhotonInserter") 
RecoLooper.PhotonInserter.setProperties(
    ContainerKey="PhotonCollection",
    etCut=10*GeV,
    useIsEM = False,
    isEMMasks= [0xFF],
    useIsolation=False
    )

## insert tau
RecoLooper += anEVTool("EVTauJetInserter/TauJetInserter") 
RecoLooper.TauJetInserter.setProperties(
    ContainerKey="TauJetCollection",
    etCut=20*GeV,
    LikelihoodCut3p=2.,
    LikelihoodCut1p=2.,
    HadronicEnergyFraction=0.0,
    MultiTrackMassCut=999*GeV,
    RequireChargeToBeOne=False,
    InsertedLabels=["Tau"]
)

# insert ParticleJet - use cone04 jets 
RecoLooper += anEVTool("EVParticleJetInserter/PJetInserter")
RecoLooper["PJetInserter"].setProperties(
  ContainerKey="ConeTowerParticleJets",
  etCut=20*GeV,
  deltaRCut=.4, # cone size
  InsertedLabels=["Jet"]
)

# label JetTag
RecoLooper += anEVTool("EVParticleJetInserter/BJetSelector") # tool name / instance name
# This inserter doesn't "insert" rather, uses selector mode to label b-tagged objects
RecoLooper.BJetSelector.setProperties(       
  etCut=20*GeV,
  useWeight=True,
  weightCut=4.5,
  SelectorMode=True,
  SelectedLabels=["BTagged"],
  SelectorLabel="Jet",
)

# insert truth for electrons
RecoLooper += anEVTool("EVTruthParticleInserter/TrElectronInserter") 
RecoLooper.TrElectronInserter.setProperties(
  ContainerKey = "SpclMC",
  noOverlapCheck = True,
  pdgCode=[11],
  etCut = 5*GeV,  ## 
  InsertedLabels=["TrEl"]
)

# insert truth for Muon
RecoLooper += anEVTool("EVTruthParticleInserter/TrMuonInserter") 
RecoLooper.TrMuonInserter.setProperties(
  ContainerKey = "SpclMC",
  noOverlapCheck = True,
  pdgCode=[13],
  etCut = 5*GeV,  ## 
  InsertedLabels=["TrMu"]
)

# insert missing et to UserData
RecoLooper += anEVTool("EVMissingEtUserData/MissEt"),
RecoLooper.MissEt.MissingETKeys = ["MET_RefFinal"]
RecoLooper.MissEt.MissingETTruthKeys = ["MET_Truth"]

## combinatorics #################################
RecoLooper+=anEVTool("EVSimpleCombo/ZeeMaker")
RecoLooper.ZeeMaker.setProperty( {
  "Labels":["Electron" ],  # select objects by labels
  "OutputLabel" : "Zee",  # label for the reconstructed Z
  "DaughterLabel" : "ZDaughter",  # label for the daughters
  "Charge" : 0,   # required charge of the Z
  "PDGID"  : 23,
  "LowMass" : 66.19*GeV,   # mass window lower limit
  "HighMass" : 116.19*GeV,  # mass window higher limit
  "NDaughters":2,  # number of objects to combine
  "PassOnNoCombo" : True, # pass on EV even if there's no Z?
})

RecoLooper+=anEVTool("EVSimpleCombo/ZmmMaker")
RecoLooper.ZmmMaker.setProperty( {
  "Labels":["Muon" ],  # select objects by labels
  "OutputLabel" : "Zmm",  # label for the reconstructed Z
  "DaughterLabel" : "ZDaughter",  # label for the daughters
  "Charge" : 0,   # required charge of the Z
  "PDGID"  : 23,
  "LowMass" : 66.19*GeV,   # mass window lower limit
  "HighMass" : 116.19*GeV,  # mass window higher limit
  "NDaughters":2,  # number of objects to combine
  "PassOnNoCombo" : True, # pass on EV even if there's no Z?
})

# Setup trigger
## TriggerLevels=[ "L1", "L2", "EF",
##               "Muon", "E/Gamma", "Electron", "Photon", "Tau",
##               "Jet", "MissingEt" ]
## TriggerLevels=[ "EF",
##                "Muon", "E/Gamma", "Electron", "Jet", "MissingEt" ]

TriggerLevels=[ "EF",
               "Muon", "Electron"]

## theApp.Dlls += ["EventViewTrigger"]
## RecoLooper.EventViewTools += ["EVTriggerDecisionUserData","EVEFTriggerUserData"]
## RecoLooper.EVTriggerDecisionUserData.TriggerDecisionKey="MyTriggerDecision"
## RecoLooper.EVTriggerDecisionUserData.TriggerNames=["EF_e25i","EF_e15iEF_e15i","EF_mu6","EF_mu20i","EF_met10"]
## RecoLooper.EVTriggerDecisionUserData.CopyAllSignatures=True
## EVUserDataDumpInfAll(RecoLooper,Prefix="Trigger_",Labels=[],SelectLabels=["Trigger"])

## TriggerPrepLooper= EVMultipleOutputToolLooper("TriggerPrepLooper")
## TriggerPrepLooper.EventViewCollectionOutputName="DummyTriggerView"
## theJob += TriggerPrepLooper
## TriggerPrepLooper += L1TriggerPreparators("L1TrigPrep",TriggerLevels)
## TriggerPrepLooper += L2TriggerPreparators("L2TrigPrep",TriggerLevels)
## TriggerPrepLooper += EFTriggerPreparators("EFTrigPrep",TriggerLevels)

## #
# Object info Calculators ###########################
# for electrons
RecoLooper+=anEVTool("EVUDFinalStateLooper/ElectronLooper")
RecoLooper.ElectronLooper.setProperties(
  Prefix="El_",
  Labels=["Electron"] #specify objects to loop over by label
)
RecoLooper += ElectronUDInfo("El_Info",
                           Level=["FullStandardAOD","TriggerMatch","TruthMatch"])
#RecoLooper.ElectronLooper += anEVTool("EVUDKinCalc/ElecKin")
RecoLooper.ElectronLooper += anEVTool("EVUDTrackBasedEtCone/ElTrackEtCone")
RecoLooper.ElectronLooper.ElTrackEtCone.setProperties(
      DeltaRVec=[0.01,0.1,0.2,0.3,0.4,0.5,0.6]
)

# for Muons
RecoLooper+=anEVTool("EVUDFinalStateLooper/MuonLooper")
RecoLooper.MuonLooper.setProperties(
  Prefix="Mu_",
  Labels=["Muon"] #specify objects to loop over by label
)
RecoLooper += MuonUDInfo("Mu_Info",
                           Level=["FullStandardAOD","TriggerMatch","TruthMatch"])
#RecoLooper.MuonLooper += anEVTool("EVUDKinCalc/ElecKin")
RecoLooper.MuonLooper += anEVTool("EVUDTrackBasedEtCone/MuTrackEtCone")
RecoLooper.MuonLooper.MuTrackEtCone.setProperties(
      DeltaRVec=[0.01,0.1,0.2,0.3,0.4,0.5,0.6]
)


# for TauJet
RecoLooper+=anEVTool("EVUDFinalStateLooper/TauJetLooper")
RecoLooper.TauJetLooper.setProperties(
  Prefix="Tau_",
  Labels=["Tau"] #specify objects to loop over by label
)
RecoLooper.TauJetLooper += anEVTool("EVUDKinCalc/ElecKin")

# for particle jets
RecoLooper+=anEVTool("EVUDFinalStateLooper/PJetLooper")
RecoLooper.PJetLooper.setProperties(
  Prefix="Jet_",
  Labels=["Jet"]
)

RecoLooper.PJetLooper += [ anEVTool("EVUDKinCalc/PJetKin"), 
                           anEVTool("EVUDParticleJetAll"), 
                           anEVTool("EVUDLabelCalc/BTagLabel") ]
RecoLooper.PJetLooper.BTagLabel.Labels=["BTagged"]

# for true electrons
RecoLooper+=anEVTool("EVUDFinalStateLooper/TrueElectronLooper")
RecoLooper.TrueElectronLooper.setProperties(
  Prefix="TEl_",
  Labels=["TrEl"]
)

RecoLooper.TrueElectronLooper += anEVTool("EVUDKinCalc/TElKinCalc")
RecoLooper.TrueElectronLooper += anEVTool("EVUDTruthParticleAll/TPAlle")

RecoLooper.TrueElectronLooper.TPAlle.setProperties(
  TreeInfo = True,
)    


# for true muons
RecoLooper+=anEVTool("EVUDFinalStateLooper/TrueMuonLooper")
RecoLooper.TrueMuonLooper.setProperties(
  Prefix="TMu_",
  Labels=["TrMu"]
)
RecoLooper.TrueMuonLooper += anEVTool("EVUDKinCalc/TMuKinCalc")
RecoLooper.TrueMuonLooper += anEVTool("EVUDTruthParticleAll/TPAllm")
RecoLooper.TrueMuonLooper.TPAllm.setProperties(
  TreeInfo = True,
)    

RecoLooper += [anEVTool("EVTriggerDecisionUserData/EVTriggerDecision")]

RecoLooper.EVTriggerDecision.setProperties(TriggerDecisionKey="MyTriggerDecision",
                                           CopyAllSignatures=True,
                                           #TriggerNames=["E25i"]
                                           )

RecoLooper += [anEVTool( "ManT::EVL1MissingEtUserData/L1MissingEtUserData" )]

RecoLooper += [anEVTool( "ManT::EVHLTMissingEtUserData/HLTMissingEtUserData" )]
        
RecoLooper.HLTMissingEtUserData.setProperties(ObjectKey="TrigEFMissingET",
                                              Level="EF")

# for Z's
RecoLooper+=anEVTool("EVUDInferredObjectLooper/ZLooper") #Inferred object looper for Z
RecoLooper.ZLooper.setProperties(
  Prefix="Z_",
  Labels=["Zee","Zmm"]
)
RecoLooper.ZLooper += anEVTool("EVUDKinCalc/ZKin")
RecoLooper.ZLooper += anEVTool("EVUDLabelCalc/ZLabel")
RecoLooper.ZLooper.ZLabel.Labels=["Zee","Zmm"]

#RecoLooper+=EVScreenDump("RecoScreenDumper", printUD=True, printUDVals=True)
#RecoLooper+=EVScreenDump("RecoScreenDumper", printUD=False, printUDVals=False)
#--------------------------------------------------------------------
# UserData to AANtuple
#--------------------------------------------------------------------
if not 'mode' in dir():
    mode='test'

filename = mode
filename+="_ntuple.root"
#--------------------------------------------------------------------
RecoLooper += AANtupleFromUserData("RecoAADumper", filename, sequencer=theJob, EventTree=True, CandTree=True, Prefix="EV")
print theJob

## to submit the job:
## athena -c "mode='HZ'" HZanalysis.py HZanalysis-dataset.py 2>&1 | tee HZ-test.log   to append
## athena -c "mode='test'" HZanalysis.py HZanalysis-dataset.py >&! test.log
## athena -c "mode='HZ'" HZanalysis.py HZanalysis-dataset.py >&! HZ-test.log
## athena -c "mode='ZZ4l'" HZanalysis.py HZanalysis-dataset.py 2>&1 | tee ZZ4l.log
## athena -c "mode='tt6811'" HZanalysis.py HZanalysis-dataset.py 2>&1 | tee tt6811.log
## athena -c "mode='WpZ'" HZanalysis.py HZanalysis-dataset.py 2>&1 | tee WpZ.log
## athena -c "mode='ZZtautaununu'" HZanalysis.py HZanalysis-dataset.py 2>&1 | tee ZZtautaununu.log
## athena -c "mode='WmZ'" HZanalysis.py HZanalysis-dataset.py 2>&1 | tee WmZ.log
## athena -c "mode='ZZlltautau'" HZanalysis.py HZanalysis-dataset.py 2>&1 | tee ZZlltautau.log
## athena -c "mode='HZ140'" HZanalysis.py HZanalysis-dataset.py 2>&1 | tee HZ140.log
## athena -c "mode='ChL'" HZanalysis.py HZanalysis-dataset.py 2>&1 | tee ChL.log
## athena -c "mode='Zee-ljets'" HZanalysis.py HZanalysis-dataset.py 2>&1 | tee Zee-ljets.log
## athena -c "mode='Zee-bjets'" HZanalysis.py HZanalysis-dataset.py 2>&1 | tee Zee-bjets.log
## athena -c "mode='Zmm-ljets'" HZanalysis.py HZanalysis-dataset.py 2>&1 | tee Zmm-ljets.log
## athena -c "mode='Zmm-bjets'" HZanalysis.py HZanalysis-dataset.py 2>&1 | tee Zmm-bjets.log
## athena -c "mode='Zee-bjets-2'" HZanalysis.py HZanalysis-dataset.py 2>&1 | tee Zee-bjets-2.log
## athena -c "mode='Zmm-bjets-2'" HZanalysis.py HZanalysis-dataset.py 2>&1 | tee Zmm-bjets-2.log
## athena -c "mode='mh110'" HZanalysis.py HZanalysis-dataset.py 2>&1 | tee mh110.log
## athena -c "mode='mh150'" HZanalysis.py HZanalysis-dataset.py 2>&1 | tee mh150.log
## athena -c "mode='mh200'" HZanalysis.py HZanalysis-dataset.py 2>&1 | tee mh200.log
## athena -c "mode='mh250'" HZanalysis.py HZanalysis-dataset.py 2>&1 | tee mh250.log
