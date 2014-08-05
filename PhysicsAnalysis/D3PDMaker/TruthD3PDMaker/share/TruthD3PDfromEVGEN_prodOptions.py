# Build truth missing Et
include( "ParticleBuilderOptions/MissingEtTruth_jobOptions.py" )
METPartTruth.TruthCollectionName = "GEN_EVENT"
topSequence.METAlg += METPartTruth

###################################################################
# Make the D3PD.
#

#--------------------------------------------------------------------------
# Configuration
#--------------------------------------------------------------------------
from AthenaCommon.SystemOfUnits import GeV
from D3PDMakerConfig.D3PDMakerFlags import jobproperties

# compatibility with jets
#from RecExConfig.RecConfFlags import jobproperties
#jobproperties.RecConfFlags.AllowBackNavigation = True

from JetRec.JetRecFlags import jobproperties as jobpropjet
jobpropjet.JetRecFlags.inputFileType = "GEN"

# Build list of particles stored in D3PD
from TruthD3PDAnalysis.truthParticleConfig import truthParticleConfig
truthParticleConfig (topSequence)

# Build list of particles used to jet building
from TruthD3PDMaker.TruthJetFilterConfig import TruthJetFilterConfig
TruthJetFilterConfig (topSequence, writePartons = False, writeHadrons = True,
                      excludeWZdecays = True)
# Build list of particles used for WZ jet building
TruthJetFilterConfig (topSequence, sgkey = 'FilteredWZD3PDTruth', writePartons = False, writeHadrons = True,
                      excludeWZdecays = True, photonCone=0.1, excludeLeptonsFromTau=True)

# Build truth particle (hadron-level) jets
# Flags for AOD
from ParticleBuilderOptions.AODFlags import AODFlags
AODFlags.MissingEtTruth = True
AODFlags.TruthParticleJet = True
AODFlags.McEventKey="GEN_EVENT"

# Reconstruct standard ATLAS truth jets
from JetRec.JetGetters import *
antikt4truthAlgStd = make_StandardJetGetter('AntiKt',0.4,'Truth',disable=False).jetAlgorithmHandle()
antikt6truthAlgStd = make_StandardJetGetter('AntiKt',0.6,'Truth',disable=False).jetAlgorithmHandle()

# Add truth jet collection that includes all final state particles (including muons and neutrinos)
antikt4truthAlgWZ = make_StandardJetGetter('AntiKt',0.4,'Truth',disable=False,
                                           inputCollectionNames=['FilteredWZD3PDTruth'],
                                           outputCollectionName='AntiKt4TruthJets_WZ',
                                           useInteractingOnly=False,
                                           includeMuons=True
                                           ).jetAlgorithmHandle()
antikt6truthAlgWZ = make_StandardJetGetter('AntiKt',0.6,'Truth',disable=False,
                                           inputCollectionNames=['FilteredWZD3PDTruth'],
                                           outputCollectionName='AntiKt6TruthJets_WZ',
                                           useInteractingOnly=False,
                                           includeMuons=True
                                           ).jetAlgorithmHandle()

if jobproperties.D3PDMakerFlags.TruthWriteExtraJets():
    antikt6truthAlgStd = make_StandardJetGetter('AntiKt',0.6,'Truth',disable=False).jetAlgorithmHandle()
    antikt4truthAlgWZ = make_StandardJetGetter('AntiKt',0.4,'Truth',disable=False,
                                               outputCollectionName='AntiKt4TruthJets_ALL',
                                               useInteractingOnly=False,
                                               includeMuons=True
                                               ).jetAlgorithmHandle()
    antikt6truthAlgWZ = make_StandardJetGetter('AntiKt',0.6,'Truth',disable=False,
                                               outputCollectionName='AntiKt6TruthJets_ALL',
                                               useInteractingOnly=False,
                                               includeMuons=True
                                               ).jetAlgorithmHandle()

    # Make trimmed fat jet:
    from SUSYD3PDMaker.SUSYJSjets import setupTruthJets
    GroomedDictsTrimming = [ { 'Type' : 'Trimming',
                               'args' : { 'SmallR' : 0.3, 'PtFrac' : 0.05 , 'SaveSubjets' : True } } ]
    jetsToWrite = setupTruthJets('AntiKt', 1.0, GroomedDictsTrimming, topSequence)


#--------------------------------------------------------------------------
# Make the D3PD
#--------------------------------------------------------------------------
from TruthD3PDMaker.TruthD3PDMakerFlags import TruthD3PDFlags
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
alg = MSMgr.NewRootStream( "StreamNTUP_TRUTH", TruthD3PDFlags.TruthD3PDOutputFileName(), "truth" )


#--------------------------------------------------------------------------
# Configure the heavy flavour overlap removal(Hfor) tool
#--------------------------------------------------------------------------

# Is there a preExec variable telling us not to use Hfor?
if "useHfor" not in dir() or useHfor:

    # If steering file is not defined in runArgs then use default file
    if "hforSteeringFile" not in dir():
        hforSteeringFile = "/cvmfs/atlas.cern.ch/repo/sw/Generators/MC12JobOptions/latest/hfor/hforconfig.txt"

    # Get the sample type from the steering file (i.e. isBB, isCC etc.)
    import TruthD3PDMaker.HforConfig as HFC
    hforConfTool = HFC.HforConfig()
    hforSampleType = hforConfTool.getSampleType(runArgs, hforSteeringFile)

    # Check that we found a valid configuration type
    if hforSampleType != "fail":
        # Set up the HFOR D3PD object
        from TruthD3PDMaker.HforD3PDObject import HforD3PDObject
        alg += HforD3PDObject(0,'HforInfo')

        # Configure the Hfor tool
        from AthenaCommon.AppMgr import ToolSvc
        from HforTool.HforToolConf import HforTool
        ToolSvc += HforTool("HforTool")
        ToolSvc.HforTool.RemovalSchema = "jetbased"
        ToolSvc.HforTool.SampleType = hforSampleType


#--------------------------------------------------------------------------
# Copy EventInfo into D3PD
#--------------------------------------------------------------------------
from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject
alg += EventInfoD3PDObject (0,'EventInfo')


#--------------------------------------------------------------------------
# Copy (a subset of) the HepMC truth event into D3PD
#--------------------------------------------------------------------------
from TruthD3PDMaker.GenEventD3PDObject import GenEventD3PDObject
alg += GenEventD3PDObject (0, 'GenEvent')

if TruthD3PDFlags.WriteTruthVertices():
    from TruthD3PDMaker.GenVertexD3PDObject import GenVertexD3PDObject
    alg += GenVertexD3PDObject(1, 'GenVertex', sgkey=jobproperties.D3PDMakerFlags.TruthSGKey(),prefix='mcVx_',label='mcVx_')

from JetD3PDMaker.JetD3PDObject import JetD3PDObject as myJetD3PDObject
import TruthD3PDMaker

# Add parton flavor information to jets
myJetD3PDObject.defineBlock(0, 'JetFullTruthTag_Info',TruthD3PDMaker.JetFullTruthTag,prefix='flavor_',MinPartonPt=5*GeV,MinHadronPt=5*GeV)

alg += myJetD3PDObject (2, 'Jet_Truth',     sgkey='AntiKt4TruthJets',    prefix='jet_AntiKt4TruthJets_',    include=['TrueFlavorComponents','JetLabel'],exclude=['L1Kinematics','L2Kinematics','EFKinematics','El02Match','Mu02Match','L1Info','L2Info','EFInfo','OriginCorrection','DQMoments','JetSamplingsFrac','JetQual','EMFraction','JES','JESMoments','EMScale','Layer','Samplings','ConstituentScale','JetLArHVMoment','JetClusterMoment'])
alg += myJetD3PDObject (2, 'Jet_Truth_WZ',  sgkey='AntiKt4TruthJets_WZ', prefix='jet_AntiKt4TruthJets_WZ_', include=['TrueFlavorComponents','JetLabel'],exclude=['L1Kinematics','L2Kinematics','EFKinematics','El02Match','Mu02Match','L1Info','L2Info','EFInfo','OriginCorrection','DQMoments','JetSamplingsFrac','JetQual','EMFraction','JES','JESMoments','EMScale','Layer','Samplings','ConstituentScale','JetLArHVMoment','JetClusterMoment'])
alg += myJetD3PDObject (2, 'Jet_Truth6_WZ', sgkey='AntiKt6TruthJets_WZ', prefix='jet_AntiKt6TruthJets_WZ_', include=['TrueFlavorComponents','JetLabel'],exclude=['L1Kinematics','L2Kinematics','EFKinematics','El02Match','Mu02Match','L1Info','L2Info','EFInfo','OriginCorrection','DQMoments','JetSamplingsFrac','JetQual','EMFraction','JES','JESMoments','EMScale','Layer','Samplings','ConstituentScale','JetLArHVMoment','JetClusterMoment'])

if jobproperties.D3PDMakerFlags.TruthWriteExtraJets():
    alg += myJetD3PDObject (2, 'Jet_Truth6',     sgkey='AntiKt6TruthJets',    prefix='jet_AntiKt6TruthJets_',    include=['TrueFlavorComponents','JetLabel'],exclude=['L1Kinematics','L2Kinematics','EFKinematics','El02Match','Mu02Match','L1Info','L2Info','EFInfo','OriginCorrection','DQMoments','JetSamplingsFrac','JetQual','EMFraction','JES','JESMoments','EMScale','Layer','Samplings','ConstituentScale','JetLArHVMoment','JetClusterMoment'])
    alg += myJetD3PDObject (2, 'Jet_Truth_ALL',  sgkey='AntiKt4TruthJets_ALL', prefix='jet_AntiKt4TruthJets_ALL_', include=['TrueFlavorComponents','JetLabel'],exclude=['L1Kinematics','L2Kinematics','EFKinematics','El02Match','Mu02Match','L1Info','L2Info','EFInfo','OriginCorrection','DQMoments','JetSamplingsFrac','JetQual','EMFraction','JES','JESMoments','EMScale','Layer','Samplings','ConstituentScale','JetLArHVMoment','JetClusterMoment'])
    alg += myJetD3PDObject (2, 'Jet_Truth6_ALL', sgkey='AntiKt6TruthJets_ALL', prefix='jet_AntiKt6TruthJets_ALL_', include=['TrueFlavorComponents','JetLabel'],exclude=['L1Kinematics','L2Kinematics','EFKinematics','El02Match','Mu02Match','L1Info','L2Info','EFInfo','OriginCorrection','DQMoments','JetSamplingsFrac','JetQual','EMFraction','JES','JESMoments','EMScale','Layer','Samplings','ConstituentScale','JetLArHVMoment','JetClusterMoment'])

    alg += myJetD3PDObject (2, 'Jet_Truth10', sgkey='AntiKt10TruthJets', prefix='jet_AntiKt10TruthJets_', include=['TrueFlavorComponents','JetLabel'],exclude=['L1Kinematics','L2Kinematics','EFKinematics','El02Match','Mu02Match','L1Info','L2Info','EFInfo','OriginCorrection','DQMoments','JetSamplingsFrac','JetQual','EMFraction','JES','JESMoments','EMScale','Layer','Samplings','ConstituentScale','JetLArHVMoment','JetClusterMoment'])
    alg += myJetD3PDObject(2, 'Jet_Truth10Trim', prefix='jet_AntiKt10TruthTrimmedPtFrac5SmallR30_', sgkey='AntiKt10TruthTrimmedPtFrac5SmallR30Jets', include=['TrueFlavorComponents','JetLabel'],exclude=['L1Kinematics','L2Kinematics','EFKinematics','El02Match','Mu02Match','L1Info','L2Info','EFInfo','OriginCorrection','DQMoments','JetSamplingsFrac','JetQual','EMFraction','JES','JESMoments','EMScale','Layer','Samplings','ConstituentScale','JetLArHVMoment','JetClusterMoment'])

from TruthD3PDMaker.TruthParticleD3PDObject import TruthParticleD3PDObject
alg += TruthParticleD3PDObject (1, 'TruthParticle')
alg.TruthParticle.BlockFillers[1].WriteE=True # Turn on the writing of the Energy branch

from TruthD3PDMaker.Atlfast1MissingETD3PDObject      import TruthMETD3PDObject
alg += TruthMETD3PDObject (level=10)

# Add the necessary input containers for the objects
from TruthD3PDMaker.TruthParticleFakerObject import simpleParticleConfig
simpleParticleConfig(topSequence,sgkey='SimpleElectronContainer',pdg_id=11,min_pt=1*GeV)
simpleParticleConfig(topSequence,sgkey='SimpleMuonContainer',pdg_id=13,min_pt=1*GeV)
simpleParticleConfig(topSequence,sgkey='SimpleTauContainer',pdg_id=15,min_pt=1*GeV)
simpleParticleConfig(topSequence,sgkey='SimplePhotonContainer',pdg_id=22,min_pt=3*GeV)

# Set up and add the objects to the alg sequence
from TruthD3PDMaker.TruthParticleFakerObject import simpleTruthParticleD3PDObject
elTruth  = simpleTruthParticleD3PDObject( 'SimpleElectronContainer' , 'el_' )
muTruth  = simpleTruthParticleD3PDObject( 'SimpleMuonContainer' , 'mu_' )
tauTruth = simpleTruthParticleD3PDObject( 'SimpleTauContainer' , 'tau_' , skipDressing=True )

# Add lepton parent association to leptons
from TruthD3PDMaker.TruthLeptonParentAssociation import TruthLeptonParentAssociation
for lepTruth in [elTruth,muTruth,tauTruth]:
    ChildAssoc = TruthLeptonParentAssociation(
                            parent = lepTruth,
                            prefix = 'parent_',
                            target = 'mc_',
                            level = 0 )
def _TruthElParentAssocHook (c, prefix, *args, **kw):
    assoc = getattr(c, c.name() + '_child_ElParentDecayAssociation', None)
    if assoc:
        indexer = getattr(assoc, assoc.name() + 'Index')
        indexer.Target = prefix
    return
elTruth.defineHook(_TruthElParentAssocHook)

def _TruthMuParentAssocHook (c, prefix, *args, **kw):
    assoc = getattr(c, c.name() + '_child_MuParentDecayAssociation', None)
    if assoc:
        indexer = getattr(assoc, assoc.name() + 'Index')
        indexer.Target = prefix
    return
muTruth.defineHook(_TruthMuParentAssocHook)

def _TruthTauParentAssocHook (c, prefix, *args, **kw):
    assoc = getattr(c, c.name() + '_child_TauParentDecayAssociation', None)
    if assoc:
        indexer = getattr(assoc, assoc.name() + 'Index')
        indexer.Target = prefix
    return
tauTruth.defineHook(_TruthTauParentAssocHook)

# Add hadronic daughter association to the taus!
from TruthD3PDMaker.TruthTauDecayAssociation import TruthTauDecayAssociation
ChildAssoc = TruthTauDecayAssociation(
                        parent = tauTruth,
                        prefix = 'decay_',
                        target = 'mc_',
                        level = 0 )
def _TruthTauDecayAssocHook (c, prefix, *args, **kw):
    assoc = getattr(c, c.name() + '_child_TruthTauDecayAssociation', None)
    if assoc:
        indexer = getattr(assoc, assoc.name() + 'Index')
        indexer.Target = prefix
    return
tauTruth.defineHook(_TruthTauDecayAssocHook)

phTruth  = simpleTruthParticleD3PDObject( 'SimplePhotonContainer' , 'ph_' , skipDressing=True )
alg += elTruth (0,'ElTruthParticle',sgkey='SimpleElectronContainer')
alg += muTruth (0,'MuTruthParticle',sgkey='SimpleMuonContainer')
alg += tauTruth(0,'TauTruthParticle',sgkey='SimpleTauContainer')
alg += phTruth (0,'PhotonTruthParticle',sgkey='SimplePhotonContainer')

# To explicitly list variables to keep or exclude in final D3PD
from AthenaCommon.AppMgr import ServiceMgr
from D3PDMakerRoot.D3PDMakerRootConf import D3PD__RootD3PDSvc
ServiceMgr += D3PD__RootD3PDSvc( "MyD3PDVetoSvc")
ServiceMgr.MyD3PDVetoSvc.VetoedNames = [ 'timestamp','timestamp_ns','lbn','bcid','detmask0','detmask1','actualIntPerXing','averageIntPerXing',
                                         'el_pdgId','mu_pdgId','tau_pdgId','ph_pdgId','ph_charge'] # These are just stupid
alg.D3PDSvc = ServiceMgr.MyD3PDVetoSvc
