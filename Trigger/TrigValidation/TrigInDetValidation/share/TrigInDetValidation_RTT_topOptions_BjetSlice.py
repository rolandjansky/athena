
#####################################################################################################
#
# top level jobOptions to run b-jet chains in the RTT or standalone
# sets some global variables that adjust the execution of TrigInDetValidation_RTT_Common.py
#
# Jiri.Masik@manchester.ac.uk
#
#####################################################################################################

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

#set athenaCommonFlags.FilesInput to be able to use this job options standalone without RTT
#secondSet of files can be activated by the if statement below 

#if athenaCommonFlags.FilesInput()==[]:
#  athenaCommonFlags.FilesInput=[
#    "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.recon.RDO.e3698_s2608_s2183_r7193/RDO.06752771._000001.pool.root.1",
#    ]

include("TrigInDetValidation/TrigInDetValidation_RTT_Chains.py")

rMC = False
if 'runMergedChain' in dir() and runMergedChain==True:
  rMC = True
rID=False
if 'doIDNewTracking' in dir() and doIDNewTracking==True:
  rID = True
rBperf=False
if 'doBperf' in dir() and doBperf==True:
  rBperf = True
rFTK=False
if 'doFTK' in dir() and doFTK==True:
  from TriggerJobOpts.TriggerFlags import TriggerFlags
  TriggerFlags.doFTK=True
  rFTK=True

(idtrigChainlist, tidaAnalysischains) = bjetChains(rMC,rID,rFTK,rBperf)


def resetSigs():
  #TriggerFlags.doMuon=False         #dependency of L1Topo
  TriggerFlags.doHypo=False
  TriggerFlags.Slices_all_setOff()
  TriggerFlags.BjetSlice.setAll();
  TriggerFlags.BjetSlice.signatures = idtrigChainlist


# tags for bjet vertex optimisation testing
#####################################################
## Primary vertex finding optimisations
#####################################################

# if 'bjetEtaHalfWidth' in dir() or 
#   'bjetPhiHalfWidth' in dir() or 
#   'bjetJetMinEt' in dir() or 
#   'bjetNJetsMax' in dir() or 
#   'bjetDynamicMinJetEt' in dir() or 
#   'bjetDynamicNjetsMax' in dir() or
#   'bjetDynamicEtFactor' in dir():
  
from TrigBjetHypo.TrigSuperRoiBuilderAllTEConfig import getSuperRoiBuilderAllTEInstance
theSuperRoi=getSuperRoiBuilderAllTEInstance()

# Vary size of SuperRoi constituents.  Suggest Eta/PhiHalfWidth = 0.1, 0.15, 0.2
if 'bjetEtaHalfWidth' in dir():
    theSuperRoi.EtaHalfWidth = bjetEtaHalfWidth

if 'bjetPhiHalfWidth' in dir():
    theSuperRoi.PhiHalfWidth = bjetPhiHalfWidth

# Raise the pT threshold of jets used to construct the super ROI. Suggest 30, 35, 40
if 'bjetJetMinEt' in dir():
    theSuperRoi.JetMinEt = bjetJetMinEt

# Limit number of jets used to construct the super ROI.  Suggest 5, 10, 15, 20
if 'bjetNJetsMax' in dir():
    theSuperRoi.NJetsMax = NJetsMax

# Dynamically scale the min jet Et threshold as a function of the number of jets.
# Suggest (X, Y) values of (5, 2), (5, 5), (5, 10)
if 'bjetDynamicMinJetEt' in dir():
    theSuperRoi.DynamicMinJetEt = bjetDynamicMinJetEt

if 'bjetDynamicNJetsMax' in dir():
    theSuperRoi.DynamicNJetsMax = bjetDynamicNJetsMax

if 'bjetDynamicEtFactor' in dir():
    theSuperRoi.DynamicEtFactor = bjetDynamicEtFactor

#####################################################
## b-tagging optimisations
#####################################################

print theSuperRoi

if 'splitZHalfWidth' in dir() :
  from TrigBjetHypo.TrigJetSplitterAllTEConfig import getJetSplitterAllTEInstance, getJetSplitterFTKAllTEInstance
  theJetSplit=getJetSplitterAllTEInstance()

# Vary z half-width of RoIs used for b-tagging.  Suggest zHalfWidth = 20, 15, 10, 9, 8, 7, 6, 5 mm
  theJetSplit.ZHalfWidth = splitZHalfWidth

  print theJetSplit

include("TrigInDetValidation/TrigInDetValidation_RTT_Common.py")

# minimum track pT for the bjet vertex tracking 
if 'minVtxTrackpT' in dir() :  
  topSequence.TrigSteer_HLT.TrigFastTrackFinder_BjetVtx.pTmin = minVtxTrackpT 

# print "*********************************"
# print "*********************************"
# print " ***         WARNING          ***"
# print "    Temporarily removing MDT"
# print "     (Savannah bug #104846) "
# print "*********************************"
# print "*********************************"
# disable the muons due to MDT error
RegSelSvcDefault.enableMDT=False

print "*********************************"
print " ***         WARNING          ***"
print "      NO LONGER removing MDT"
print "      (Savannah bug #104846) "
print "*********************************"


