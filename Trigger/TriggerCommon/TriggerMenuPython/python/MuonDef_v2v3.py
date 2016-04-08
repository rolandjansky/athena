# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


########################################################################
#
# SliceDef file for muon chains/signatures
#
#########################################################################
from TriggerJobOpts.TriggerFlags            import TriggerFlags
from TriggerMenuPython.Lvl1Flags            import Lvl1Flags

from TriggerMenuPython.HltConfig import *

from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo
from TrigmuFast.TrigmuFastConfig import *
from TrigmuComb.TrigmuCombConfig import *
from TrigIDSCAN.TrigIDSCAN_Config import TrigIDSCAN_Muon
from TrigIDSCAN.TrigIDSCAN_Config import TrigIDSCAN_Muon_NoTRT
from TrigIDSCAN.TrigIDSCAN_Config import TrigIDSCAN_Cosmics
from TrigIDSCAN.TrigIDSCAN_Config import TrigIDSCAN_muonIso
from TrigSiTrack.TrigSiTrack_Config import TrigSiTrack_Muon
from TrigSiTrack.TrigSiTrack_Config import TrigSiTrack_Muon_NoTRT
from TrigSiTrack.TrigSiTrack_Config import TrigSiTrack_muonIso
from TrigmuIso.TrigmuIsoConfig import *
from TrigL2TRTSegFinder.TrigTRTSegFinder_Config import TrigTRTSegFinder_Muon
from TrigMuonHypo.TrigL2MuonOverlapRemoverConfig import *

from AthenaCommon import CfgGetter

#Hidden Valley
from TrigL2LongLivedParticles.TrigL2LongLivedParticlesConfig import MuonClusterConfig
from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConfig import MuonClusterHypoConfig
from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConfig import MuonClusterAllMSHypoConfig

from TrigIDSCAN.TrigIDSCAN_Config import TrigIDSCAN_Tile
from TrigTileMuId.TrigTileLookForMuAlgConfig import TrigTileLookForMuAlg_L2
from TrigTileMuId.TrigTileMuFexConfig import *

from TrigMuonHypo.TrigMuonHypoConfig import *
from InDetTrigRecExample.EFInDetConfig import *
from TrigMuonEF.TrigMuonEFConfig import *
from TrigMuonEF.TrigMuonEFCosmicConfig import *
from TrigMuGirl.TrigMuGirlConfig import *
from TrigmuRoI.TrigmuRoIConfig import *

theTrigmuFastMuon = TrigmuFastConfig('Muon')
UseMuFastHypo=True

theTrigmuFast900GeV = TrigmuFastConfig('900GeV')
theTrigmuFastOutBCID = TrigmuFastConfig('OutBCID')
theTrigmuFastOutBCIDMuonEcut4 = TrigmuFastConfig('OutBCIDMuonEcut4')
theTrigmuFastCAL = TrigmuFastConfig('Mcal')

theTrigmuFastMuonEcut4   = TrigmuFastConfig('MuonEcut4')
theTrigmuFastMuonEcut4_empty   = TrigmuFastConfig('MuonEcut4Empty')

theTrigMuonEFTrackIsoAnnulus = TrigMuonEFTrackIsolationAnnulusConfig()

# muFast threshold for combiner chains
mufastThr_4GeV='4GeV_v11a'
mufastThr_6GeV='6GeV_v11a'
mufastThr_10GeV='6GeV_v11a'
mufastThr_13GeV='6GeV_v11a'
mufastThr_15GeV='6GeV_v11a'
mufastThr_18GeV='6GeV_v11a'
mufastThr_20GeV='6GeV_v11a'
mufastThr_22GeV='6GeV_v11a'
mufastThr_24GeV='6GeV_v11a'
mufastThr_30GeV='6GeV_v11a'
mufastThr_36GeV='6GeV_v11a'
mufastThr_40GeV='6GeV_v11a'

muFastThrMap = { '4GeV'  : mufastThr_4GeV,
                 '6GeV'  : mufastThr_6GeV,
                 '10GeV' : mufastThr_10GeV,
                 '13GeV' : mufastThr_13GeV,
                 '15GeV' : mufastThr_15GeV,
                 '18GeV' : mufastThr_18GeV,
                 '20GeV' : mufastThr_20GeV,
                 '22GeV' : mufastThr_22GeV,
                 '24GeV' : mufastThr_24GeV,
                 '30GeV' : mufastThr_30GeV,
                 '36GeV' : mufastThr_36GeV,
                 '40GeV' : mufastThr_40GeV }

try:
    from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_MuonA
    theTrigL2SiTrackFinder_MuonA = TrigL2SiTrackFinder_MuonA()
except:
    theTrigL2SiTrackFinder_MuonA = None
try:
    from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_MuonB
    theTrigL2SiTrackFinder_MuonB = TrigL2SiTrackFinder_MuonB()
except:
    theTrigL2SiTrackFinder_MuonB = None
try:
    from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_MuonC
    theTrigL2SiTrackFinder_MuonC = TrigL2SiTrackFinder_MuonC()
except:
    theTrigL2SiTrackFinder_MuonC = None

# ---
try:
    from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_muonIsoA
    theTrigL2SiTrackFinder_muonIsoA = TrigL2SiTrackFinder_muonIsoA()
except:
    theTrigL2SiTrackFinder_muonIsoA = None
try:
    from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_muonIsoB
    theTrigL2SiTrackFinder_muonIsoB = TrigL2SiTrackFinder_muonIsoB()
except:
    theTrigL2SiTrackFinder_muonIsoB = None
try:
    from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_muonIsoC
    theTrigL2SiTrackFinder_muonIsoC = TrigL2SiTrackFinder_muonIsoC()
except:
    theTrigL2SiTrackFinder_muonIsoC = None


theTrigIDSCAN_Muon = TrigIDSCAN_Muon()
theTrigIDSCAN_Muon_NoTRT = TrigIDSCAN_Muon_NoTRT()
theTrigIDSCAN_Cosmics = TrigIDSCAN_Cosmics()
theTrigIDSCAN_muonIso = TrigIDSCAN_muonIso()
theTrigIDSCAN_Tile = TrigIDSCAN_Tile()
theTrigSiTrack_muonIso = TrigSiTrack_muonIso()
#theTrigSiTrack_Tile = TrigSiTrack_Tile()
theTrigTRTSegFinder_Muon = TrigTRTSegFinder_Muon()
#theTrigMuonEFSegmentFinderCosmic = TrigMuonEFSegmentFinderCosmicConfig()
#theTrigMuonEFTrackBuilderCosmic = TrigMuonEFTrackBuilderCosmicConfig()

theTrigMuonEFSegmentFinder = TrigMuonEFSegmentFinderConfig()
theTrigMuonEFSegmentFinder_fs = TrigMuonEFSegmentFinderConfig("TrigMuonEFSegmentFinder_FS")
theTrigMuonEFSegmentFinder_fs.fullScan = True
theTrigMuonEFTrackBuilder = TrigMuonEFTrackBuilderConfig()
theTrigMuonEFTrackBuilder_fs = TrigMuonEFTrackBuilderConfig("TrigMuonEFTrackBuilder_FS")
theTrigMuonEFExtrapolator = TrigMuonEFExtrapolatorConfig()
theTrigMuonEFExtrapolator_rejectFailedExtrap = TrigMuonEFExtrapolatorConfig("TrigMuonEFExtrapolatorConfig_rejectFailedExtrap")
theTrigMuonEFExtrapolator_rejectFailedExtrap.rejectFailedExtrap=True
theTrigMuonEFExtrapolator_fs_hi = TrigMuonEFExtrapolatorConfig("TrigMuonEFExtrapolatorConfig_FS_HI")
theTrigMuonEFExtrapolator_fs_hi.rejectFailedExtrap=True
theTrigMuonEFCombiner = TrigMuonEFCombinerConfig()

theTrigMuonEFCombiner_hi = TrigMuonEFCombinerConfig('TrigMuonEFCombiner_HI')
theTrigMuonEFCombiner_hi.IdTrackParticles='InDetTrigParticleCreation_MuonHI_EFID'
theTrigMuonEFCombiner_hi.IdTracks='InDetTrigTrackSlimmer_MuonHI_EFID'

theMuIsoConfig = muIsoConfig('HighPT','IDSCAN')
theMuIsoConfig.IDalgo="IDSCAN"

theMuIsoConfig_SITRACK = muIsoConfig('HighPT','SITRACK')
theMuIsoConfig_SITRACK.IDalgo="SITRACK" 

theMuIsoConfig_L2StarA = muIsoConfig('HighPT', 'STRATEGY_A')
theMuIsoConfig_L2StarA.IDalgo = "STRATEGY_A" 
theMuIsoConfig_L2StarB = muIsoConfig('HighPT', 'STRATEGY_B')
theMuIsoConfig_L2StarB.IDalgo ="STRATEGY_B"
theMuIsoConfig_L2StarC = muIsoConfig('HighPT', 'STRATEGY_C')
theMuIsoConfig_L2StarC.IDalgo ="STRATEGY_C"

#MSonly IDScan
theMuIsoConfig_MSonly = muIsoConfig('MSonly','IDSCAN')
theMuIsoConfig_MSonly.MuIsoStrategy=2
theMuIsoConfig_MSonly.IDalgo="IDSCAN"

#LowPt
theMuIsoConfig_LowPT = muIsoConfig('LowPT','IDSCAN')
theMuIsoConfig_LowPT.IDalgo="IDSCAN"

#Hidden Valley
theMuonCluster = MuonClusterConfig(IDtracking="SITRACK")
theMuonClusterHypo = MuonClusterHypoConfig()
theMuonClusterAllMSHypo = MuonClusterAllMSHypoConfig()

theTrigEFIDInsideOut_Muon = TrigEFIDInsideOut_Muon()
theTrigEFIDOutsideInTRTOnly_Muon = TrigEFIDOutsideInTRTOnly_Muon()
theTrigMuonEFCombinerTRTOnly = TrigMuonEFCombinerConfigTRTOnly()

theTrigMuonEFTrackIso = TrigMuonEFTrackIsolationConfig() 
theTrigEFIDInsideOut_MuonIso = TrigEFIDSequence("MuonIso","muonIso","InsideOut") 


# put actual creation inside a function, so that it is only created on demand
def getTrigMuSuperEF():
    return CfgGetter.getAlgorithmClone("TrigMuSuperEF","TrigMuSuperEFDefault",
                                       doInsideOut = True, doOutsideIn=True, insideOutFirst = False)

# alternate choices
def getTrigMuSuperEF_MGfirst():
    return CfgGetter.getAlgorithmClone("TrigMuSuperEF_MGfirst","TrigMuSuperEFDefault",
                                       insideOutFirst = True)

def getTrigMuSuperEF_TMEFfirst():
    return CfgGetter.getAlgorithmClone("TrigMuSuperEF_TMEFfirst","TrigMuSuperEFDefault",
                                       insideOutFirst = False)

def getTrigMuSuperEF_MGonly():
    return CfgGetter.getAlgorithmClone("TrigMuSuperEF_MGonly","TrigMuSuperEFDefault",
                                       doInsideOut = True, doOutsideIn=False)

def getTrigMuSuperEF_TMEFonly():
    return CfgGetter.getAlgorithmClone("TrigMuSuperEF_TMEFonly","TrigMuSuperEFDefault",
                                       doInsideOut = False, doOutsideIn=True)

def getTrigMuSuperEF_both():
    return CfgGetter.getAlgorithmClone("TrigMuSuperEF_both","TrigMuSuperEFDefault",
                                       doInsideOut = True, doOutsideIn=True, forceBoth=True)



theTrigMuSuperEFTrackIso = TrigMuonEFTrackIsolationConfig("TrigMuSuperEFTrackIsolation")


# TrigL2MuonSA instance
from TrigL2MuonSA.TrigL2MuonSAConfig import *
theTrigL2MuonSAMuon   = TrigL2MuonSAConfig('Muon')
theTrigL2MuonSA900GeV = TrigL2MuonSAConfig('900GeV')


#for monitoring chain with vtx cut
from TrigMinBias.TrigMinBiasConfig import *

###########################################################################
#Basic chain class, with isolation hypo
###########################################################################
class L2EFChain_mu(L2EFChainDef):
    def __init__(self, sig_id,
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter,
                 l2_inputTEs, config,
                 prescale=1, pass_through=0, rerun=0):
        L2EFChainDef.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter,
                              l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter,
                              l2_inputTEs, config,
                              prescale, pass_through, rerun)
        #self.l2_lower_chain_name = l2_lower_chain_name

    class L2Config:
        def __init__(self, suffix,
                     MufastHypo_1, MucombHypo_1, MuisolHypo_1=None):
            self.suffix = suffix
            self.MufastHypo_1 = MufastHypo_1
            self.MucombHypo_1 = MucombHypo_1
            self.MuisolHypo_1 = MuisolHypo_1
            pass

    class EFConfig:
        def __init__(self, suffix, 
                     TrigMuonHypo_1):
            self.suffix = suffix
            self.TrigMuonHypo_1 = TrigMuonHypo_1
            pass

    class Config:
        def __init__(self, L2Config, EFConfig):
            self.L2Config = L2Config
            self.EFConfig = EFConfig


    def defineSequences(self, config):
        L2conf = config.L2Config
        EFconf = config.EFConfig
        # L2 sequences
        self.addL2Sequence(self.inputTEs[0],
                           [ theTrigmuFastMuon, L2conf.MufastHypo_1],
                           'L2_mu_standalone1')
        self.addL2Sequence('L2_mu_standalone1',
                           [ theTrigIDSCAN_Muon, TrigSiTrack_Muon(),
                             TrigmuCombConfig('Muon', 'IDSCAN'),
                             L2conf.MucombHypo_1],
                           'L2_mu')
        self.addL2Sequence('L2_mu',
                           [ theMuIsoConfig, L2conf.MuisolHypo_1 ],
                           'L2_mui')

        # EF sequences
        self.addEFSequence(['L2_mui'],
                           theTrigEFIDInsideOut_Muon.getSequence(),
                           'EF_efid')
        self.addEFSequence('EF_efid',
                           [theTrigMuonEFSegmentFinder, TrigMuonEFSegmentFinderHypo()],
                           'EF_MS')
        self.addEFSequence('EF_MS',
                           [theTrigMuonEFTrackBuilder, TrigMuonEFTrackBuilderHypo()],
                           'EF_TB')
        self.addEFSequence('EF_TB',
                           [theTrigMuonEFExtrapolator, TrigMuonEFExtrapolatorHypo()],
                           'EF_SA')
        self.addEFSequence('EF_SA',
                           [theTrigMuonEFCombiner, EFconf.TrigMuonHypo_1],
                           'EF_mu')
        pass

    def defineSignatures(self):
        self.addL2Signature(['L2_mu_standalone1'])
        self.addL2Signature(['L2_mu'])
        self.addL2Signature(['L2_mui'])
        self.addEFSignature(['EF_efid'])
        self.addEFSignature(['EF_MS'])
        self.addEFSignature(['EF_TB'])
        self.addEFSignature(['EF_SA'])
        self.addEFSignature(['EF_mu'])
        pass

    def defineStreamGroupTriggerType(self):
        self.physics_streams = ['muons']
        self.calib_streams = []
        if('EMPTY' in self.l2_lower_chain_name):
            self.groups = ['RATE:Cosmic_Muon','BW:Muon']
        else:
            self.groups = ['RATE:SingleMuon','BW:Muon']
        self.trigger_type = ['hello']


        pass

    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        self.TErenamingMap = {
            'L2_mu_standalone1': mergeRemovingOverlap('L2_mu_standalone', L2suf),
            'L2_mu': mergeRemovingOverlap('L2_mu', L2suf),
            'L2_mui': mergeRemovingOverlap('L2_mui', L2suf),
            'EF_efid': mergeRemovingOverlap('EF_efid', EFsuf),
            'EF_MS': mergeRemovingOverlap('EF_MS', EFsuf),
            'EF_TB': mergeRemovingOverlap('EF_TB', EFsuf),
            'EF_SA': mergeRemovingOverlap('EF_SA', EFsuf),
            'EF_mu': mergeRemovingOverlap('EF_mu', EFsuf),
            }
        pass


###########################################################################
#Hidden Valley
###########################################################################
class L2EFChain_HV(L2EFChain_mu):
    
    class L2Config:
        def __init__(self, suffix):
            self.suffix = suffix
            pass

    class EFConfig:
        def __init__(self, suffix): 
            self.suffix = suffix
            pass

    class Config:
        def __init__(self, L2Config, EFConfig):
            self.L2Config = L2Config
            self.EFConfig = EFConfig

    def defineSequences(self, config):
        L2conf = config.L2Config
        EFconf = config.EFConfig
        # L2 sequences
        self.addL2Sequence(self.inputTEs[0],
                           [ theTrigSiTrack_muonIso],
                           'L2_mu_clID')
        self.addL2Sequence(['L2_mu_clID', self.inputTEs[1]],
                           [ theMuonCluster, theMuonClusterHypo ],
                           'L2_mu_cl')

        # EF sequences
        self.addEFSequence(['L2_mu_cl'],
                           [ PESA__DummyUnseededAllTEAlgo() ],
                           'EF_mu_cl')
        pass

    def defineSignatures(self):
        self.addL2Signature(['L2_mu_clID'])
        self.addL2Signature(['L2_mu_cl'])
        self.addEFSignature(['EF_mu_cl'])
        pass

    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        self.TErenamingMap = {
            'L2_mu_clID': mergeRemovingOverlap('L2_tmp', L2suf),
            'L2_mu_cl': mergeRemovingOverlap('L2_', L2suf),
            'EF_mu_cl': mergeRemovingOverlap('EF_', EFsuf),
            }
        pass

    def defineStreamGroupTriggerType(self):
        self.physics_streams = ['muons']
        self.calib_streams = []
        if('EMPTY' in self.l2_lower_chain_name):
            self.groups = ['RATE:Cosmic_Muon','BW:Muon']
        else:
            self.groups = ['RATE:MultiMuon','BW:Muon']
        self.trigger_type = ['hello']


        pass

###########################################################################
# HV with acceptance in |eta|<2.5
###########################################################################
class L2EFChain_HV_AllMS(L2EFChain_mu):
    
    class L2Config:
        def __init__(self, suffix):
            self.suffix = suffix
            pass

    class EFConfig:
        def __init__(self, suffix): 
            self.suffix = suffix
            pass

    class Config:
        def __init__(self, L2Config, EFConfig):
            self.L2Config = L2Config
            self.EFConfig = EFConfig

    def defineSequences(self, config):
        L2conf = config.L2Config
        EFconf = config.EFConfig
        # L2 sequences
        self.addL2Sequence(self.inputTEs[0],
                           [ theTrigSiTrack_muonIso],
                           'L2_mu_clID')
        self.addL2Sequence(['L2_mu_clID', self.inputTEs[1]],
                           [ theMuonCluster, theMuonClusterAllMSHypo ],
                           'L2_mu_cl')

        # EF sequences
        self.addEFSequence(['L2_mu_cl'],
                           [ PESA__DummyUnseededAllTEAlgo() ],
                           'EF_mu_cl')
        pass

    def defineSignatures(self):
        self.addL2Signature(['L2_mu_clID'])
        self.addL2Signature(['L2_mu_cl'])
        self.addEFSignature(['EF_mu_cl'])
        pass

    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        self.TErenamingMap = {
            'L2_mu_clID': mergeRemovingOverlap('L2_tmp', L2suf),
            'L2_mu_cl': mergeRemovingOverlap('L2_', L2suf),
            'EF_mu_cl': mergeRemovingOverlap('EF_', EFsuf),
            }
        pass

    def defineStreamGroupTriggerType(self):
        self.physics_streams = ['muons']
        self.calib_streams = []
        if('EMPTY' in self.l2_lower_chain_name):
            self.groups = ['RATE:Cosmic_Muon','BW:Muon']
        else:
            self.groups = ['RATE:MultiMuon','BW:Muon']
        self.trigger_type = ['hello']


        pass

    
###########################################################################
#Basic chain class, no isolation
###########################################################################
class L2EFChain_mu_noiso(L2EFChain_mu):
    def defineSequences(self, config):
        L2conf = config.L2Config
        EFconf = config.EFConfig
        # L2 sequences
        #-------------------------------------------------------
        # HACK for CAF reprocessing (TK, 08.10.2010)
        #-------------------------------------------------------
        if UseMuFastHypo or 'MU0' in self.inputTEs:
            self.addL2Sequence(self.inputTEs[0],
                               [ theTrigmuFastMuon, L2conf.MufastHypo_1],
                               'L2_mu_standalone1')
            self.addL2Sequence('L2_mu_standalone1',
                               [ theTrigIDSCAN_Muon,
                                 TrigmuCombConfig('Muon', 'IDSCAN'),
                                 L2conf.MucombHypo_1],
                               'L2_mu')
        else:
            self.addL2Sequence(self.inputTEs[0],
                               [ theTrigmuFastMuon, ], 
                               'L2_mu_standalone1')
            self.addL2Sequence('L2_mu_standalone1',
                               [ theTrigIDSCAN_Muon,
                                 TrigmuCombConfig('Muon', 'IDSCAN'),
                                 L2conf.MucombHypo_1],
                               'L2_mu')

        # EF sequences
        self.addEFSequence(['L2_mu'],
                           theTrigEFIDInsideOut_Muon.getSequence(),
                           'EFID_mu')
        self.addEFSequence('EFID_mu',
                           [theTrigMuonEFSegmentFinder, TrigMuonEFSegmentFinderHypo()],
                           'EF_MS')
        self.addEFSequence('EF_MS',
                           [theTrigMuonEFTrackBuilder, TrigMuonEFTrackBuilderHypo()],
                           'EF_TB')
        self.addEFSequence('EF_TB',
                           [theTrigMuonEFExtrapolator, TrigMuonEFExtrapolatorHypo()],
                           'EF_SA')
        self.addEFSequence('EF_SA',
                           [theTrigMuonEFCombiner, EFconf.TrigMuonHypo_1],
                           'EF_mu')
        pass

    def defineSignatures(self):
        self.addL2Signature(['L2_mu_standalone1'])
        self.addL2Signature(['L2_mu'])
        self.addEFSignature(['EFID_mu'])
        self.addEFSignature(['EF_MS'])
        self.addEFSignature(['EF_TB'])
        self.addEFSignature(['EF_SA'])
        self.addEFSignature(['EF_mu'])
        pass

    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        self.TErenamingMap = {
            'L2_mu_standalone1': mergeRemovingOverlap('L2_muon_standalone', L2suf),
            'L2_mu': mergeRemovingOverlap('L2_mu', L2suf),
            'EFID_mu': mergeRemovingOverlap('EF_efid_mu', EFsuf),
            'EF_MS': mergeRemovingOverlap('EF_MS', EFsuf),
            'EF_TB': mergeRemovingOverlap('EF_TB', EFsuf),
            'EF_SA': mergeRemovingOverlap('EF_SA', EFsuf),
            'EF_mu': mergeRemovingOverlap('EF_mu', EFsuf),
            }
        pass


###########################################################################
###########################################################################
class L2EFChain_mu_Seeded_EFFS(L2EFChain_mu_noiso):

    # extend with hypo for FS
    class EFConfig:
        def __init__(self, suffix, TrigMuonHypo_1, TrigMuonHypoFS_1):
            self.suffix = suffix
            self.TrigMuonHypo_1 = TrigMuonHypo_1
            self.TrigMuonHypoFS_1 = TrigMuonHypoFS_1

    def defineSequences(self, config):
        L2conf = config.L2Config
        EFconf = config.EFConfig
        # MS FS instance
        theTrigMuonEFMS_FS = TrigMuonEFSegmentFinderConfig("TrigMuonEFSegmentFinder_SeededFS")
        theTrigMuonEFMS_FS.fullScan = True
        # CB FS instance
        theTrigMuonEFCB_FS = TrigMuonEFCombinerConfig("TrigMuonEFCombiner_SeededFS")
        theTrigMuonEFCB_FS.IdTracks = "InDetTrigTrackSlimmer_FullScan_EFID"

        if UseMuFastHypo or 'MU0' in self.inputTEs:
            self.addL2Sequence(self.inputTEs[0],
                               [ theTrigmuFastMuon, L2conf.MufastHypo_1],
                               'L2_mu_standalone1')
            self.addL2Sequence('L2_mu_standalone1',
                               [ theTrigIDSCAN_Muon,
                                 TrigmuCombConfig('Muon', 'IDSCAN'),
                                 L2conf.MucombHypo_1],
                               'L2_mu')
        else:
            self.addL2Sequence(self.inputTEs[0],
                               [ theTrigmuFastMuon, ], 
                               'L2_mu_standalone1')
            self.addL2Sequence('L2_mu_standalone1',
                               [ theTrigIDSCAN_Muon,
                                 TrigmuCombConfig('Muon', 'IDSCAN'),
                                 L2conf.MucombHypo_1],
                               'L2_mu')

            
        # std EF sequences
        self.addEFSequence(['L2_mu'],
                           theTrigEFIDInsideOut_Muon.getSequence(),
                           'EFID_mu')
        self.addEFSequence('EFID_mu',
                           [theTrigMuonEFSegmentFinder,
                            TrigMuonEFSegmentFinderHypo()],
                           'EF_MS')
        self.addEFSequence('EF_MS',
                           [theTrigMuonEFTrackBuilder,
                            TrigMuonEFTrackBuilderHypo()],
                           'EF_TB')
        self.addEFSequence('EF_TB',
                           [theTrigMuonEFExtrapolator,
                            TrigMuonEFExtrapolatorHypo()],
                           'EF_SA')
        self.addEFSequence('EF_SA',
                           [theTrigMuonEFCombiner,
                            EFconf.TrigMuonHypo_1],
                           'EF_mu')
        # FS EF sequences
        # FS in ID sequence
        self.addEFSequence('',
                           [PESA__DummyUnseededAllTEAlgo("EFDummyAlgo")]+
                           TrigEFIDInsideOut_FullScan().getSequence(),
                           'EF_FStracks')
        ## FS MS
        self.addEFSequence('EF_FStracks',
                           [theTrigMuonEFMS_FS,
                           TrigMuonEFSegmentFinderHypo()],
                          'EF_MS_FS')
        ## FS TB
        self.addEFSequence('EF_MS_FS',
                          [ TrigMuonEFTrackBuilderConfig("TrigMuonEFTrackBuilderConfig_SeededFS"),
                          TrigMuonEFTrackBuilderHypo()],
                          'EF_TB_FS')
        ## FS EXTR
        self.addEFSequence('EF_TB_FS',
                           [TrigMuonEFExtrapolatorConfig("TrigMuonEFExtrapolatorConfig_SeededFS"),
                            TrigMuonEFExtrapolatorHypo()],
                           'EF_SA_FS')
        ## FS combiner sequence
        self.addEFSequence('EF_SA_FS',
                           [theTrigMuonEFCB_FS,
                            EFconf.TrigMuonHypoFS_1],
                           'EF_CB_FS')
        pass

    def defineSignatures(self):
        self.addL2Signature(['L2_mu_standalone1'])
        self.addL2Signature(['L2_mu'])
        self.addEFSignature(['EFID_mu'])
        self.addEFSignature(['EF_MS'])
        self.addEFSignature(['EF_TB'])
        self.addEFSignature(['EF_SA'])
        self.addEFSignature(['EF_mu'])
        #FS
        self.addEFSignature(['EF_FStracks'])
        self.addEFSignature(['EF_MS_FS'])
        self.addEFSignature(['EF_TB_FS'])
        self.addEFSignature(['EF_SA_FS'])
        self.addEFSignature(['EF_CB_FS'])
        pass

    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        self.TErenamingMap = {
            'L2_mu_standalone1': mergeRemovingOverlap('L2_muon_standalone', L2suf),
            'L2_mu': mergeRemovingOverlap('L2_mu', L2suf),
            'EFID_mu': mergeRemovingOverlap('EF_efid_mu', EFsuf),
            'EF_MS': mergeRemovingOverlap('EF_MS', EFsuf),
            'EF_TB': mergeRemovingOverlap('EF_TB', EFsuf),
            'EF_SA': mergeRemovingOverlap('EF_SA', EFsuf),
            'EF_mu': mergeRemovingOverlap('EF_mu', EFsuf),
#            'EF_FStracks': mergeRemovingOverlap('EF_FStracks', EFsuf),
#            'EF_MS_FS': mergeRemovingOverlap('EF_MS_FS', EFsuf),
#            'EF_TB_FS': mergeRemovingOverlap('EF_TB_FS', EFsuf),
#            'EF_SA_FS': mergeRemovingOverlap('EF_SA_FS', EFsuf),
            'EF_CB_FS': mergeRemovingOverlap('EF_CB_FS', EFsuf),
            }
        pass

## HI
class L2EFChain_mu_Seeded_EFFS_HI(L2EFChain_mu):

    # extend with hypo for FS
    class EFConfig:
        def __init__(self, suffix, TrigMuonHypo_1, TrigMuonHypoFS_1):
            self.suffix = suffix
            self.TrigMuonHypo_1 = TrigMuonHypo_1
            self.TrigMuonHypoFS_1 = TrigMuonHypoFS_1

    def defineSequences(self, config):
        L2conf = config.L2Config
        EFconf = config.EFConfig

        if UseMuFastHypo or 'MU0' in self.inputTEs:
            self.addL2Sequence(self.inputTEs[0],
                               [ theTrigmuFast900GeV, L2conf.MufastHypo_1],
                               'L2_mu_standalone1')
        else:
            self.addL2Sequence(self.inputTEs[0],
                               [ theTrigmuFastMuon, L2conf.MufastHypo_1], 
                               'L2_mu_standalone1')
        # std EF sequences
        self.addEFSequence('L2_mu_standalone1',
                           [theTrigMuonEFSegmentFinder,
                            TrigMuonEFSegmentFinderHypo()],
                           'EF_MS')
        self.addEFSequence('EF_MS',
                           [theTrigMuonEFTrackBuilder,
                            TrigMuonEFTrackBuilderHypo()],
                           'EF_TB')
        self.addEFSequence('EF_TB',
                           [theTrigMuonEFExtrapolator,
                            EFconf.TrigMuonHypo_1],
                           'EF_SA')
        # FS EF sequences
        ## FS MS
        self.addEFSequence('',
                           [PESA__DummyUnseededAllTEAlgo("EFDummyAlgo")]+
                           [theTrigMuonEFSegmentFinder_fs,
                            TrigMuonEFSegmentFinderHypo()],
                           'EF_MS_FS')
        ## FS TB
        self.addEFSequence('EF_MS_FS',
                           [ theTrigMuonEFTrackBuilder_fs,
                             TrigMuonEFTrackBuilderHypo()],
                           'EF_TB_FS')
        ## FS EXTR
        self.addEFSequence('EF_TB_FS',
                           [theTrigMuonEFExtrapolator_fs_hi,
                            #TrigMuonEFExtrapolatorConfig("TrigMuonEFExtrapolatorConfig_SeededFS"),
                            EFconf.TrigMuonHypoFS_1],
                           'EF_SA_FS')
        pass

    def defineSignatures(self):
        self.addL2Signature(['L2_mu_standalone1'])
        self.addEFSignature(['EF_MS'])
        self.addEFSignature(['EF_TB'])
        self.addEFSignature(['EF_SA'])
        #FS
        self.addEFSignature(['EF_MS_FS'])
        self.addEFSignature(['EF_TB_FS'])
        self.addEFSignature(['EF_SA_FS'])
        pass

    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        self.TErenamingMap = {
            'L2_mu_standalone1': mergeRemovingOverlap('L2_muon_standalone', L2suf),
            'EF_MS': mergeRemovingOverlap('EF_MS', EFsuf),
            'EF_TB': mergeRemovingOverlap('EF_TB', EFsuf),
            'EF_SA': mergeRemovingOverlap('EF_SA', EFsuf),
            }
        pass

    def defineStreamGroupTriggerType(self):
        self.physics_streams = ['muons']
        self.calib_streams = []
        if('EMPTY' in self.l2_lower_chain_name):
            self.groups = ['RATE:Cosmic_Muon','BW:Muon']
        else:
            if ('2mu' in self.ef_chain_name):
                self.groups = ['RATE:MultiMuon','BW:Muon']
            else:
                self.groups = ['RATE:SingleMuon','BW:Muon']
        self.trigger_type = ['hello']


class L2EFChain_mu_NoTRT_noiso(L2EFChain_mu_noiso):
    def defineSequences(self, config):
        # L2 sequences
        L2conf = config.L2Config
        EFconf = config.EFConfig
        if UseMuFastHypo or 'MU0' in self.inputTEs:
            self.addL2Sequence(self.inputTEs[0],
                               [ theTrigmuFastMuon, L2conf.MufastHypo_1],
                               'L2_mu_standalone1')
            self.addL2Sequence('L2_mu_standalone1',
                               [ theTrigIDSCAN_Muon_NoTRT,
                                 TrigmuCombConfig('Muon', 'IDSCAN'),
                                 L2conf.MucombHypo_1],
                               'L2_mu')
        else:
            self.addL2Sequence(self.inputTEs[0],
                               [ theTrigmuFastMuon, ],
                               'L2_mu_standalone1')
            self.addL2Sequence('L2_mu_standalone1',
                               [ theTrigIDSCAN_Muon_NoTRT,
                                 TrigmuCombConfig('Muon', 'IDSCAN'),
                                 L2conf.MucombHypo_1],
                               'L2_mu')


        # EF sequences
        self.addEFSequence(['L2_mu'],
                           TrigEFIDSequence("MuonHI","heavyIon","InsideOutNoTRT").getSequence(),
                           'EFID_mu')
        self.addEFSequence('EFID_mu',
                           [theTrigMuonEFSegmentFinder, TrigMuonEFSegmentFinderHypo()],
                           'EF_MS')
        self.addEFSequence('EF_MS',
                           [theTrigMuonEFTrackBuilder, TrigMuonEFTrackBuilderHypo()],
                           'EF_TB')
        self.addEFSequence('EF_TB',
                           [theTrigMuonEFExtrapolator, TrigMuonEFExtrapolatorHypo()],
                           'EF_SA')
        self.addEFSequence('EF_SA',
                           [theTrigMuonEFCombiner_hi, EFconf.TrigMuonHypo_1],
                           'EF_mu')
        pass


# ---------------------------------------------------------
class L2EFChain_mu_NoTRT_NoL2Trk_noiso(L2EFChain_mu_noiso):
    def defineSequences(self, config):
        # L2 sequences
        L2conf = config.L2Config
        EFconf = config.EFConfig
        if UseMuFastHypo or 'MU0' in self.inputTEs:
            self.addL2Sequence(self.inputTEs[0],
                               [ theTrigmuFastMuon, L2conf.MufastHypo_1],
                               'L2_mu_standalone1')
        else:
            self.addL2Sequence(self.inputTEs[0],
                               [ theTrigmuFastMuon, ],
                               'L2_mu_standalone1')

        # EF sequences
        self.addEFSequence(['L2_mu_standalone1'],
                           TrigEFIDSequence("MuonHI","heavyIon","InsideOutNoTRT").getSequence(),
                           'EFID_mu')
        self.addEFSequence('EFID_mu',
                           [theTrigMuonEFSegmentFinder, TrigMuonEFSegmentFinderHypo()],
                           'EF_MS')
        self.addEFSequence('EF_MS',
                           [theTrigMuonEFTrackBuilder, TrigMuonEFTrackBuilderHypo()],
                           'EF_TB')
        self.addEFSequence('EF_TB',
                           [theTrigMuonEFExtrapolator, TrigMuonEFExtrapolatorHypo()],
                           'EF_SA')
        self.addEFSequence('EF_SA',
                           [theTrigMuonEFCombiner_hi, EFconf.TrigMuonHypo_1],
                           'EF_mu')
        pass
    def defineSignatures(self):
        self.addL2Signature(['L2_mu_standalone1'])
        self.addEFSignature(['EFID_mu'])
        self.addEFSignature(['EF_MS'])
        self.addEFSignature(['EF_TB'])
        self.addEFSignature(['EF_SA'])
        self.addEFSignature(['EF_mu'])
        pass
    
    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        self.TErenamingMap = {
            'L2_mu_standalone1': mergeRemovingOverlap('L2_mu_standalone1', L2suf),
            'EFID_mu': mergeRemovingOverlap('EF_efid_mu', EFsuf),
            'EF_MS': mergeRemovingOverlap('EF_MS', EFsuf),
            'EF_TB': mergeRemovingOverlap('EF_TB', EFsuf),
            'EF_SA': mergeRemovingOverlap('EF_SA', EFsuf),
            'EF_mu': mergeRemovingOverlap('EF_mu', EFsuf),
            }
        pass

class L2EFChain_mu_900_NoTRT_NoL2Trk(L2EFChain_mu_NoTRT_NoL2Trk_noiso):
    def defineSequences(self, config):
        L2EFChain_mu_NoTRT_NoL2Trk_noiso.defineSequences(self, config)
        self.l2chain.sequences[0].algos[0] = theTrigmuFast900GeV
        #self.l2chain.sequences[1].algos[1] = TrigmuCombConfig('900GeV', 'IDSCAN')

class L2EFChain_mu_900_NoTRT(L2EFChain_mu_NoTRT_noiso):
    def defineSequences(self, config):
        L2EFChain_mu_NoTRT_noiso.defineSequences(self, config)
        self.l2chain.sequences[0].algos[0] = theTrigmuFast900GeV
        self.l2chain.sequences[1].algos[1] = TrigmuCombConfig('900GeV', 'IDSCAN')

# ---------------------------------------------------------
class L2EFChain_mu_900(L2EFChain_mu_noiso):
    def defineSequences(self, config):
        L2EFChain_mu_noiso.defineSequences(self, config)
        self.l2chain.sequences[0].algos[0] = theTrigmuFast900GeV
        self.l2chain.sequences[1].algos[1] = TrigmuCombConfig('900GeV', 'IDSCAN')

# ---------------------------------------------------------
class L2EFChain_mu_noiso_Ecut123(L2EFChain_mu_noiso):
    def defineSequences(self, config):
        L2EFChain_mu_noiso.defineSequences(self, config)
        self.l2chain.sequences[0].algos[0] = theTrigmuFastMuonEcut123

# ---------------------------------------------------------
class L2EFChain_mu_noiso_Ecut12(L2EFChain_mu_noiso):
    def defineSequences(self, config):
        L2EFChain_mu_noiso.defineSequences(self, config)
        self.l2chain.sequences[0].algos[0] = theTrigmuFastMuonEcut12

# ---------------------------------------------------------
class L2EFChain_mu_noiso_Ecut13(L2EFChain_mu_noiso):
    def defineSequences(self, config):
        L2EFChain_mu_noiso.defineSequences(self, config)
        self.l2chain.sequences[0].algos[0] = theTrigmuFastMuonEcut13

# ---------------------------------------------------------
class L2EFChain_mu_noiso_Ecut2(L2EFChain_mu_noiso):
    def defineSequences(self, config):
        L2EFChain_mu_noiso.defineSequences(self, config)
        self.l2chain.sequences[0].algos[0] = theTrigmuFastMuonEcut2

# ---------------------------------------------------------
class L2EFChain_mu_noiso_Ecut3(L2EFChain_mu_noiso):
    def defineSequences(self, config):
        L2EFChain_mu_noiso.defineSequences(self, config)
        self.l2chain.sequences[0].algos[0] = theTrigmuFastMuonEcut3

# ---------------------------------------------------------
# ---------------------------------------------------------


###########################################################################
###########################################################################
class L2EFChain_mu_EFFSonly(L2EFChain_mu_noiso):

    class L2Config:
        def __init__(self, suffix):
            self.suffix = suffix
            
    class EFConfig:
        def __init__(self, suffix, TrigMuonHypoFS_1):
            self.suffix = suffix
            self.TrigMuonHypoFS_1 = TrigMuonHypoFS_1
            
    def defineSequences(self, config):
        L2conf = config.L2Config
        EFconf = config.EFConfig
# MS FS instance
        theTrigMuonEFMS_FS = TrigMuonEFSegmentFinderConfig("TrigMuonEFSegmentFinder_SeededFS")
        theTrigMuonEFMS_FS.fullScan = True
# CB FS instance
        theTrigMuonEFCB_FS = TrigMuonEFCombinerConfig("TrigMuonEFCombiner_SeededFS")
        theTrigMuonEFCB_FS.IdTracks = "InDetTrigTrackSlimmer_FullScan_EFID"
        
# FS EF sequences
# FS in ID sequence
        self.addEFSequence('',
                           [PESA__DummyUnseededAllTEAlgo("EFDummyAlgo")]+
                           TrigEFIDInsideOut_FullScan().getSequence(),
                           'EF_FStracks')
## FS MS
        self.addEFSequence('EF_FStracks',
                           [theTrigMuonEFMS_FS,
                            TrigMuonEFSegmentFinderHypo()],
                           'EF_MS_FS')
## FS TB
        self.addEFSequence('EF_MS_FS',
                           [ TrigMuonEFTrackBuilderConfig("TrigMuonEFTrackBuilderConfig_SeededFS"),
                             TrigMuonEFTrackBuilderHypo()],
                           'EF_TB_FS')
## FS EXTR
        self.addEFSequence('EF_TB_FS',
                           [TrigMuonEFExtrapolatorConfig("TrigMuonEFExtrapolatorConfig_SeededFS"),
                            TrigMuonEFExtrapolatorHypo()],
                           'EF_SA_FS')
## FS combiner sequence
        self.addEFSequence('EF_SA_FS',
                           [theTrigMuonEFCB_FS,
                            EFconf.TrigMuonHypoFS_1],
                           'EF_CB_FS')
        pass
     
    def defineSignatures(self):
        self.addEFSignature(['EF_FStracks'])
        self.addEFSignature(['EF_MS_FS'])
        self.addEFSignature(['EF_TB_FS'])
        self.addEFSignature(['EF_SA_FS'])
        self.addEFSignature(['EF_CB_FS'])
        pass
    
    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        self.TErenamingMap = {
            #            'EF_FStracks': mergeRemovingOverlap('EF_FStracks', EFsuf),
            #            'EF_MS_FS': mergeRemovingOverlap('EF_MS_FS', EFsuf),
            #            'EF_TB_FS': mergeRemovingOverlap('EF_TB_FS', EFsuf),
            #            'EF_SA_FS': mergeRemovingOverlap('EF_SA_FS', EFsuf),
            'EF_CB_FS': mergeRemovingOverlap('EF_CB_FS', EFsuf),
            }
        pass
     

# muCombTag_noEF 
class L2EFChain_mu_muCombTag_NoEF(L2EFChain_mu): 
    def defineSequences(self, config): 
        L2conf = config.L2Config 
        # L2 sequences 
        self.addL2Sequence(self.inputTEs[0], 
                           [ theTrigIDSCAN_Muon, TrigmuCombConfig('Muon', 'IDSCAN'), L2conf.MucombHypo_1], 
                           'L2_mu')
        
    def defineSignatures(self): 
        self.addL2Signature(['L2_mu'])
        
    def defineTErenaming(self): 
        L2suf = self.config.L2Config.suffix 
        self.TErenamingMap = {
            'L2_mu': mergeRemovingOverlap('L2_mu', L2suf),
            } 
                

                
###########################################################################
# muCombTag_peb
###########################################################################
class L2EFChain_mu_muCombTag_peb(L2EFChain_mu):
    def defineSequences(self, config):
        L2conf = config.L2Config
        EFconf = config.EFConfig
        # L2 sequences
        self.addL2Sequence(self.inputTEs[0],
                           [ theTrigIDSCAN_Muon, TrigmuCombConfig('Muon', 'IDSCAN'), L2conf.MucombHypo_1],
                           'L2_mu')

        # EF sequences
        self.addEFSequence(['L2_mu'],
                           theTrigEFIDInsideOut_Muon.getSequence(),
                           'EFID_mu')
        self.addEFSequence('EFID_mu',
                           [theTrigMuonEFSegmentFinder, TrigMuonEFSegmentFinderHypo()],
                           'EF_MS')
        self.addEFSequence('EF_MS',
                           [theTrigMuonEFTrackBuilder, TrigMuonEFTrackBuilderHypo()],
                           'EF_TB')
        self.addEFSequence('EF_TB',
                           [theTrigMuonEFExtrapolator, TrigMuonEFExtrapolatorHypo()],
                           'EF_SA')
        self.addEFSequence('EF_SA',
                           [theTrigMuonEFCombiner, EFconf.TrigMuonHypo_1],
                           'EF_mu')
        self.addEFSequence('EF_mu',
                           [theTrigMuonSubDetAlgEF],
                           'EF_peb')

    def defineSignatures(self):
        self.addL2Signature(['L2_mu'])
        self.addEFSignature(['EFID_mu'])
        self.addEFSignature(['EF_MS'])
        self.addEFSignature(['EF_TB'])
        self.addEFSignature(['EF_SA'])
        self.addEFSignature(['EF_mu'])
        self.addEFSignature(['EF_peb'])

    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        self.TErenamingMap = {
            'L2_mu':   mergeRemovingOverlap('L2_mu',      L2suf),
            'EFID_mu': mergeRemovingOverlap('EF_efid_mu', EFsuf),
            'EF_MS':   mergeRemovingOverlap('EF_MS',      EFsuf),
            'EF_TB':   mergeRemovingOverlap('EF_TB',      EFsuf),
            'EF_SA':   mergeRemovingOverlap('EF_SA',      EFsuf),
            'EF_mu':   mergeRemovingOverlap('EF_mu',      EFsuf),
            'EF_peb':  mergeRemovingOverlap('EF_peb',     EFsuf),
            }

    def defineStreamGroupTriggerType(self):
        self.physics_streams = []
        self.calib_streams   = ['EFMuonComm']
        self.groups          = ['RATE:SingleMuon','BW:Muon', 'Calibration']



# ---------------------------------------------------------
class L2EFChain_mu_900_muCombTag_peb(L2EFChain_mu_muCombTag_peb):
    def defineSequences(self, config):
        L2EFChain_mu_muCombTag_peb.defineSequences(self, config)
        self.l2chain.sequences[0].algos[1] = TrigmuCombConfig('900GeV', 'IDSCAN')

# ---------------------------------------------------------
# ---------------------------------------------------------




###########################################################################
###########################################################################
class L2EFChain_mu_NoIDTrkCut(L2EFChain_mu_noiso):

    def defineSequences(self, config):
        
        self.addL2Sequence(self.inputTEs[0],
                           [ theTrigmuFastMuon, config.L2Config.MufastHypo_1],
                           'L2_mu_standalone1')
        self.addL2Sequence('L2_mu_standalone1',
                           [ theTrigIDSCAN_Muon, TrigSiTrack_Muon(),
                             theTrigTRTSegFinder_Muon,
                             theTrigL2SiTrackFinder_MuonA,
                             theTrigL2SiTrackFinder_MuonB,
                             theTrigL2SiTrackFinder_MuonC,
                             TrigmuCombConfig('Muon', 'IDSCAN'),
                             config.L2Config.MucombHypo_1],
                           'L2_mu')
        
        # EF sequences
        # if many vx requirement do something different!
        self.efchain.sequences=[]
        if('ManyVx' in self.config.EFConfig.suffix):
            #--- full scan
            self.addEFSequence(['L2_mu'],
                               TrigEFIDInsideOut_FullScan().getSequence(),
                               'EF_FStracks')
            ##--- then the vtx requirement
            self.addEFSequence('EF_FStracks',
                               [EFMbVxFex, config.EFConfig.TrigMuonHypo_1],
                               'EF_mu')
            ##--- then roi driven
            self.addEFSequence(['L2_mu'],
                               theTrigEFIDInsideOut_Muon.getSequence(),
                               'EFID_mu')
            self.addEFSequence(['EFID_mu'],
                               theTrigEFIDOutsideInTRTOnly_Muon.getSequence(),
                               'EF_trt')        
            pass
        
        else:
            self.addEFSequence(['L2_mu'],
                               theTrigEFIDInsideOut_Muon.getSequence(),
                               'EFID_mu')
            self.addEFSequence(['EFID_mu'],
                               theTrigEFIDOutsideInTRTOnly_Muon.getSequence(),
                               'EF_trt')        
            self.addEFSequence('EF_trt',
                               [theTrigMuonEFSegmentFinder, TrigMuonEFSegmentFinderHypo()],
                               'EF_MS')
            self.addEFSequence('EF_MS',
                               [theTrigMuonEFTrackBuilder, TrigMuonEFTrackBuilderHypo()],
                               'EF_TB')
            self.addEFSequence('EF_TB',
                               [theTrigMuonEFExtrapolator, config.EFConfig.TrigMuonHypo_1],
                               'EF_SA')
            self.addEFSequence('EF_SA',
                               [theTrigMuonEFCombiner, TrigMuonEFCombinerHypoConfig('Muon','passthrough')],
                               'EF_mu')
            pass
        

    def defineSignatures(self):
        self.addL2Signature(['L2_mu_standalone1'])
        self.addL2Signature(['L2_mu'])
        if('ManyVx' in self.config.EFConfig.suffix):
            self.addEFSignature(['EF_FStracks'])
            self.addEFSignature(['EF_mu'])
            self.addEFSignature(['EFID_mu'])
            self.addEFSignature(['EF_trt'])
        else:
            self.addEFSignature(['EFID_mu'])
            self.addEFSignature(['EF_trt'])
            self.addEFSignature(['EF_MS'])
            self.addEFSignature(['EF_TB'])
            self.addEFSignature(['EF_SA'])
            self.addEFSignature(['EF_mu'])
        pass

    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        if('ManyVx' in self.config.EFConfig.suffix):
            self.TErenamingMap = {
                'L2_mu_standalone1': mergeRemovingOverlap('L2_muon_standalone', L2suf),
                'L2_mu': mergeRemovingOverlap('L2_mu', L2suf),
                'EF_FStracks': mergeRemovingOverlap('EF_FStracks', EFsuf),
                'EF_mu': mergeRemovingOverlap('EF_mu', EFsuf),
                'EFID_mu': mergeRemovingOverlap('EF_EFID_mu', EFsuf),
                'EF_trt': mergeRemovingOverlap('EF_trt', EFsuf),
                }
        else:
            self.TErenamingMap = {
                'L2_mu_standalone1': mergeRemovingOverlap('L2_muon_standalone', L2suf),
                'L2_mu': mergeRemovingOverlap('L2_mu', L2suf),
                'EFID_mu': mergeRemovingOverlap('EF_EFID_mu', EFsuf),
                'EF_trt': mergeRemovingOverlap('EF_trt', EFsuf),
                'EF_MS': mergeRemovingOverlap('EF_MS', EFsuf),
                'EF_TB': mergeRemovingOverlap('EF_TB', EFsuf),
                'EF_SA': mergeRemovingOverlap('EF_SA', EFsuf),
                'EF_mu': mergeRemovingOverlap('EF_mu', EFsuf),
                }
        pass

class L2EFChain_mu_900_NoIDTrkCut(L2EFChain_mu_NoIDTrkCut):
    def defineSequences(self, config):
        L2EFChain_mu_NoIDTrkCut.defineSequences(self, config)
        self.l2chain.sequences[0].algos[0] = theTrigmuFast900GeV
        self.l2chain.sequences[1].algos[6] = TrigmuCombConfig('900GeV', 'IDSCAN')
        pass


class L2EFChain_mu_NoTRT_NoIDTrkCut(L2EFChain_mu_NoIDTrkCut):

    def defineSequences(self, config):
        
        self.addL2Sequence(self.inputTEs[0],
                           [ theTrigmuFastMuon, config.L2Config.MufastHypo_1],
                           'L2_mu_standalone1')
        self.addL2Sequence('L2_mu_standalone1',
                           [ theTrigIDSCAN_Muon_NoTRT, TrigSiTrack_Muon_NoTRT(),
                             TrigmuCombConfig('Muon', 'IDSCAN'),
                             config.L2Config.MucombHypo_1],
                           'L2_mu')
        
        # EF sequences
        self.efchain.sequences=[]
        self.addEFSequence(['L2_mu'],
                           TrigEFIDSequence("MuonHI","heavyIon","InsideOutNoTRT").getSequence(),
                           'EFID_mu')
        self.addEFSequence('EFID_mu',
                           [theTrigMuonEFSegmentFinder, TrigMuonEFSegmentFinderHypo()],
                           'EF_MS')
        self.addEFSequence('EF_MS',
                           [theTrigMuonEFTrackBuilder, TrigMuonEFTrackBuilderHypo()],
                           'EF_TB')
        self.addEFSequence('EF_TB',
                           [theTrigMuonEFExtrapolator, config.EFConfig.TrigMuonHypo_1],
                           'EF_SA')
        self.addEFSequence('EF_SA',
                           [theTrigMuonEFCombiner_hi, TrigMuonEFCombinerHypoConfig('Muon','passthrough')],
                           'EF_mu')
        pass

    def defineSignatures(self):
        self.addL2Signature(['L2_mu_standalone1'])
        self.addL2Signature(['L2_mu'])
        self.addEFSignature(['EFID_mu'])
        self.addEFSignature(['EF_MS'])
        self.addEFSignature(['EF_TB'])
        self.addEFSignature(['EF_SA'])
        self.addEFSignature(['EF_mu'])
        pass

    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        self.TErenamingMap = {
            'L2_mu_standalone1': mergeRemovingOverlap('L2_muon_standalone', L2suf),
            'L2_mu': mergeRemovingOverlap('L2_mu', L2suf),
            'EFID_mu': mergeRemovingOverlap('EF_EFID_mu', EFsuf),
            'EF_MS': mergeRemovingOverlap('EF_MS', EFsuf),
            'EF_TB': mergeRemovingOverlap('EF_TB', EFsuf),
            'EF_SA': mergeRemovingOverlap('EF_SA', EFsuf),
            'EF_mu': mergeRemovingOverlap('EF_mu', EFsuf),
            }
        pass


class L2EFChain_mu_900_NoTRT_NoIDTrkCut(L2EFChain_mu_NoTRT_NoIDTrkCut):
    def defineSequences(self, config):
        L2EFChain_mu_NoTRT_NoIDTrkCut.defineSequences(self, config)
        self.l2chain.sequences[0].algos[0] = theTrigmuFast900GeV
        self.l2chain.sequences[1].algos[3] = TrigmuCombConfig('900GeV', 'IDSCAN')
        pass

# ---------------------------------------------------------
# ---------------------------------------------------------
# NoCut class needed for RTT testing
class L2EFChain_mu_NoCut(L2EFChain_mu_noiso):
    
    def defineSequences(self, config):
        
        self.addL2Sequence(self.inputTEs[0],
                           [ theTrigmuFastMuon],
                           'L2_mu_standalone1')
        self.addL2Sequence('L2_mu_standalone1',
                           [ theTrigIDSCAN_Muon, TrigSiTrack_Muon(),
                             theTrigTRTSegFinder_Muon,
                             TrigmuCombConfig('Muon', 'IDSCAN')],
                           'L2_mu')

        # EF sequences
        # if many vx requirement do something different!
        self.efchain.sequences=[]
        self.addEFSequence(['L2_mu'],
                           theTrigEFIDInsideOut_Muon.getSequence(),
                           'EFID_mu')
        self.addEFSequence('EFID_mu',
                           [theTrigMuonEFSegmentFinder],
                           'EF_MS')
        self.addEFSequence('EF_MS',
                           [theTrigMuonEFTrackBuilder],
                           'EF_TB')
        self.addEFSequence('EF_TB',
                           [theTrigMuonEFExtrapolator],
                           'EF_SA')
        self.addEFSequence('EF_SA',
                           [theTrigMuonEFCombiner, TrigMuonEFCombinerHypoConfig('Muon','passthrough')],
                           'EF_mu')
        pass
    
    
    def defineSignatures(self):
        self.addL2Signature(['L2_mu_standalone1'])
        self.addL2Signature(['L2_mu'])
        if('ManyVx' in self.config.EFConfig.suffix):
            self.addEFSignature(['EF_FStracks'])
            self.addEFSignature(['EF_mu'])
            self.addEFSignature(['EFID_mu'])
        else:
            self.addEFSignature(['EFID_mu'])
            self.addEFSignature(['EF_MS'])
            self.addEFSignature(['EF_TB'])
            self.addEFSignature(['EF_SA'])
            self.addEFSignature(['EF_mu'])
            pass

    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        if('ManyVx' in self.config.EFConfig.suffix):
            self.TErenamingMap = {
                'L2_mu_standalone1': mergeRemovingOverlap('L2_muon_standalone', L2suf),
                'L2_mu': mergeRemovingOverlap('L2_mu', L2suf),
                'EF_FStracks': mergeRemovingOverlap('EF_FStracks', EFsuf),
                'EF_mu': mergeRemovingOverlap('EF_mu', EFsuf),
                'EFID_mu': mergeRemovingOverlap('EF_EFID_mu', EFsuf),
                'EF_trt': mergeRemovingOverlap('EF_trt', EFsuf),
                }
        else:
            self.TErenamingMap = {
                'L2_mu_standalone1': mergeRemovingOverlap('L2_muon_standalone', L2suf),
                'L2_mu': mergeRemovingOverlap('L2_mu', L2suf),
                'EFID_mu': mergeRemovingOverlap('EF_EFID_mu', EFsuf),
                'EF_trt': mergeRemovingOverlap('EF_trt', EFsuf),
                'EF_MS': mergeRemovingOverlap('EF_MS', EFsuf),
                'EF_TB': mergeRemovingOverlap('EF_TB', EFsuf),
                'EF_SA': mergeRemovingOverlap('EF_SA', EFsuf),
                'EF_mu': mergeRemovingOverlap('EF_mu', EFsuf),
                }
        pass

class L2EFChain_mu_900_NoCut(L2EFChain_mu_NoCut):
    def defineSequences(self, config):
        L2EFChain_mu_NoCut.defineSequences(self, config)
        self.l2chain.sequences[0].algos[0] = theTrigmuFast900GeV
        self.l2chain.sequences[1].algos[3] = TrigmuCombConfig('900GeV', 'IDSCAN')
        pass
    


###########################################################################
# L2Star code for 17 now
###########################################################################
class L2EFChain_mu_L2Star_NoCut(L2EFChain_mu_noiso):

    def defineSequences(self, config):
        
        self.addL2Sequence(self.inputTEs[0],
                           [theTrigmuFastMuon ],
                           'L2_mu_standalone1')
        self.addL2Sequence('L2_mu_standalone1',
                           [ theTrigL2SiTrackFinder_MuonA,
                             theTrigL2SiTrackFinder_MuonB,
                             theTrigL2SiTrackFinder_MuonC,
                             TrigmuCombConfig('Muon', 'IDSCAN')],
                           'L2_mu')
        
        # EF sequences
        # if many vx requirement do something different!
        self.efchain.sequences=[]
        self.addEFSequence(['L2_mu'],
                           theTrigEFIDInsideOut_Muon.getSequence(),
                           'EFID_mu')
        self.addEFSequence('EFID_mu',
                           [theTrigMuonEFSegmentFinder],
                           'EF_MS')
        self.addEFSequence('EF_MS',
                           [theTrigMuonEFTrackBuilder],
                           'EF_TB')
        self.addEFSequence('EF_TB',
                           [theTrigMuonEFExtrapolator],
                          'EF_SA')
        self.addEFSequence('EF_SA',
                           [theTrigMuonEFCombiner, TrigMuonEFCombinerHypoConfig('Muon','passthrough')],
                           'EF_mu')
        pass
        

    def defineSignatures(self):
        self.addL2Signature(['L2_mu_standalone1'])
        self.addL2Signature(['L2_mu'])
        if('ManyVx' in self.config.EFConfig.suffix):
            self.addEFSignature(['EF_FStracks'])
            self.addEFSignature(['EF_mu'])
            self.addEFSignature(['EFID_mu'])
        else:
            self.addEFSignature(['EFID_mu'])
            self.addEFSignature(['EF_MS'])
            self.addEFSignature(['EF_TB'])
            self.addEFSignature(['EF_SA'])
            self.addEFSignature(['EF_mu'])
        pass

    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        if('ManyVx' in self.config.EFConfig.suffix):
            self.TErenamingMap = {
                'L2_mu_standalone1': mergeRemovingOverlap('L2_muon_standalone', L2suf),
                'L2_mu': mergeRemovingOverlap('L2_mu', L2suf),
                'EF_FStracks': mergeRemovingOverlap('EF_FStracks', EFsuf),
                'EF_mu': mergeRemovingOverlap('EF_mu', EFsuf),
                'EFID_mu': mergeRemovingOverlap('EF_EFID_mu', EFsuf),
                'EF_trt': mergeRemovingOverlap('EF_trt', EFsuf),
                }
        else:
            self.TErenamingMap = {
                'L2_mu_standalone1': mergeRemovingOverlap('L2_muon_standalone', L2suf),
                'L2_mu': mergeRemovingOverlap('L2_mu', L2suf),
                'EFID_mu': mergeRemovingOverlap('EF_EFID_mu', EFsuf),
                'EF_trt': mergeRemovingOverlap('EF_trt', EFsuf),
                'EF_MS': mergeRemovingOverlap('EF_MS', EFsuf),
                'EF_TB': mergeRemovingOverlap('EF_TB', EFsuf),
                'EF_SA': mergeRemovingOverlap('EF_SA', EFsuf),
                'EF_mu': mergeRemovingOverlap('EF_mu', EFsuf),
                }
        pass

class L2EFChain_mu_900_L2Star_NoCut(L2EFChain_mu_L2Star_NoCut):
    def defineSequences(self, config):
        L2EFChain_mu_L2Star_NoCut.defineSequences(self, config)
        self.l2chain.sequences[0].algos[0] = theTrigmuFast900GeV
        self.l2chain.sequences[1].algos[3] = TrigmuCombConfig('900GeV', 'IDSCAN')
        pass

class L2EFChain_L2Star(L2EFChain_mu):
    def defineSequences(self, config):
        L2conf = config.L2Config
        EFconf = config.EFConfig
        self.addL2Sequence(self.inputTEs[0],
                           [ theTrigmuFastMuon, L2conf.MufastHypo_1],
                           'L2_mu_standalone1')
        self.addL2Sequence('L2_mu_standalone1',
                           [ theTrigL2SiTrackFinder_MuonA,
                             TrigmuCombConfig('Muon', 'STRATEGY_A'), L2conf.MucombHypo_1],
                           'L2_mu')

        # EF sequences
        self.addEFSequence(['L2_mu'],
                           theTrigEFIDInsideOut_Muon.getSequence(),
                           'EFID_mu')
        self.addEFSequence('EFID_mu',
                           [theTrigMuonEFSegmentFinder, TrigMuonEFSegmentFinderHypo()],
                           'EF_MS')
        self.addEFSequence('EF_MS',
                           [theTrigMuonEFTrackBuilder, TrigMuonEFTrackBuilderHypo()],
                           'EF_TB')
        self.addEFSequence('EF_TB',
                           [theTrigMuonEFExtrapolator, TrigMuonEFExtrapolatorHypo()],
                           'EF_SA')
        self.addEFSequence('EF_SA',
                           [theTrigMuonEFCombiner, EFconf.TrigMuonHypo_1],
                           'EF_mu')
        pass
    
    def defineSignatures(self):
        self.addL2Signature(['L2_mu_standalone1'])
        self.addL2Signature(['L2_mu'])
        if('ManyVx' in self.config.EFConfig.suffix):
            self.addEFSignature(['EF_FStracks'])
            self.addEFSignature(['EF_mu'])
            self.addEFSignature(['EFID_mu'])
        else:
            self.addEFSignature(['EFID_mu'])
            self.addEFSignature(['EF_MS'])
            self.addEFSignature(['EF_TB'])
            self.addEFSignature(['EF_SA'])
            self.addEFSignature(['EF_mu'])
        pass
                                                                                                                           
                                                                                                                           
    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        if('ManyVx' in self.config.EFConfig.suffix):
            self.TErenamingMap = {
                'L2_mu_standalone1': mergeRemovingOverlap('L2_muon_standalone', L2suf),
                'L2_mu': mergeRemovingOverlap('L2_mu', L2suf),
                'EF_FStracks': mergeRemovingOverlap('EF_FStracks', EFsuf),
                'EF_mu': mergeRemovingOverlap('EF_mu', EFsuf),
                'EFID_mu': mergeRemovingOverlap('EF_EFID_mu', EFsuf),
                'EF_trt': mergeRemovingOverlap('EF_trt', EFsuf),
                }
        else:
            self.TErenamingMap = {
                'L2_mu_standalone1': mergeRemovingOverlap('L2_muon_standalone', L2suf),
                'L2_mu': mergeRemovingOverlap('L2_mu', L2suf),
                'EFID_mu': mergeRemovingOverlap('EF_EFID_mu', EFsuf),
                'EF_trt': mergeRemovingOverlap('EF_trt', EFsuf),
                'EF_MS': mergeRemovingOverlap('EF_MS', EFsuf),
                'EF_TB': mergeRemovingOverlap('EF_TB', EFsuf),
                'EF_SA': mergeRemovingOverlap('EF_SA', EFsuf),
                'EF_mu': mergeRemovingOverlap('EF_mu', EFsuf),
                }
        pass
                                                           
#----------------------------
class L2EFChain_L2StarB(L2EFChain_L2Star):
    def defineSequences(self, config):
        L2EFChain_L2Star.defineSequences(self, config)
        self.l2chain.sequences[1].algos[0] = theTrigL2SiTrackFinder_MuonB
        self.l2chain.sequences[1].algos[1] = TrigmuCombConfig('Muon', 'STRATEGY_B')
        pass

class L2EFChain_L2StarC(L2EFChain_L2Star):
    def defineSequences(self, config):
        L2EFChain_L2Star.defineSequences(self, config)
        self.l2chain.sequences[1].algos[0] = theTrigL2SiTrackFinder_MuonC
        self.l2chain.sequences[1].algos[1] = TrigmuCombConfig('Muon', 'STRATEGY_C')
        pass
                                    

class L2EFChain_900_L2Star(L2EFChain_L2Star):
    def defineSequences(self, config):
        L2EFChain_L2Star.defineSequences(self, config)
        self.l2chain.sequences[0].algos[0] = theTrigmuFast900GeV
        self.l2chain.sequences[1].algos[1] = TrigmuCombConfig('900GeV', 'STRATEGY_A')
        pass
    
class L2EFChain_900_L2StarB(L2EFChain_L2StarB):
    def defineSequences(self, config):
        L2EFChain_L2StarB.defineSequences(self, config)
        self.l2chain.sequences[0].algos[0] = theTrigmuFast900GeV
        self.l2chain.sequences[1].algos[1] = TrigmuCombConfig('900GeV', 'STRATEGY_B')
        pass
    
class L2EFChain_900_L2StarC(L2EFChain_L2StarC):
    def defineSequences(self, config):
        L2EFChain_L2StarC.defineSequences(self, config)
        self.l2chain.sequences[0].algos[0] = theTrigmuFast900GeV
        self.l2chain.sequences[1].algos[1] = TrigmuCombConfig('900GeV', 'STRATEGY_C')
        pass
                                                                                                            
                                                                                                            
class L2EFChain_mu_Unseeded_EFFS(L2EFChain_mu):

    class L2Config:
        def __init__(self, suffix):
            self.suffix = suffix
            # dummy AllTE to create a common execution point for caching
            self.dummyRoI = PESA__DummyUnseededAllTEAlgo("DummyRoI_Unseeded_EFFS")
            self.dummyRoI.createRoIDescriptors = True
            self.dummyRoI.NumberOfOutputTEs=1


    class EFConfig:
        def __init__(self, suffix, TrigMuonHypo_1):
            self.suffix = suffix
            self.TrigMuonHypo_1 = TrigMuonHypo_1

    class Config:
        def __init__(self, L2Config, EFConfig):
            self.L2Config = L2Config
            self.EFConfig = EFConfig

    def defineSequences(self, config):

        # EF sequences
        self.addEFSequence('',
                           [PESA__DummyUnseededAllTEAlgo("EFDummyAlgo")]+
                           [theTrigMuonEFSegmentFinder_fs, TrigMuonEFSegmentFinderHypo()],
                           'EF_FSMuonTracks')
        self.addEFSequence('EF_FSMuonTracks',
                           [theTrigMuonEFTrackBuilder_fs, TrigMuonEFTrackBuilderHypo()],
                           'EF_TB')
        self.addEFSequence('EF_TB', 
                           [theTrigMuonEFExtrapolator_fs_hi, config.EFConfig.TrigMuonHypo_1], 
                           'EF_SA') 
        #self.addEFSequence(['EF_SA']+['EF_FStracks'],  #this part will not work
        #                   [theTrigMuonEFCombiner, config.EFConfig.TrigMuonHypo_1],
        #                   'EF_mu')
        
        pass
    
    def defineSignatures(self):
        self.addEFSignature(['EF_FSMuonTracks'])
        self.addEFSignature(['EF_TB'])
        self.addEFSignature(['EF_SA'])
        #self.addEFSignature(['EF_mu'])
        pass
    
    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        self.TErenamingMap = {
            'EF_TB': mergeRemovingOverlap('EF_TB', EFsuf),
            'EF_SA': mergeRemovingOverlap('EF_SA', EFsuf),
            #'EF_mu': mergeRemovingOverlap('EF_mu', EFsuf),
            }
        pass
    
    def defineStreamGroupTriggerType(self):
        self.physics_streams = ['muons']
        self.calib_streams = []
        if('EMPTY' in self.l2_lower_chain_name):
            self.groups = ['RATE:Cosmic_Muon','BW:Muon']
        else:
            if ('2mu' in self.l2_chain_name or 'mu4_mu2' in self.l2_chain_name):
                self.groups = ['RATE:MultiMuon','BW:Muon']
            else:
                self.groups = ['RATE:SingleMuon','BW:Muon']
        self.trigger_type = ['hello']


###########################################################################
# Basic chain class, isolation fex and hypo
###########################################################################
class L2EFChain_mu_iso(L2EFChain_mu):
    def defineSequences(self, config):
        L2conf = config.L2Config
        EFconf = config.EFConfig
        # L2 sequences
        L2conf = config.L2Config
        if UseMuFastHypo or 'MU0' in self.inputTEs:
            self.addL2Sequence(self.inputTEs[0],
                               [ theTrigmuFastMuon, L2conf.MufastHypo_1],
                               'L2_muon_standalone1')
        else:
            self.addL2Sequence(self.inputTEs[0],
                               [ theTrigmuFastMuon, ],
                               'L2_muon_standalone1')
        self.addL2Sequence('L2_muon_standalone1',
                           [ theTrigIDSCAN_muonIso, 
                             TrigmuCombConfig('Muon', 'IDSCAN'),
                             L2conf.MucombHypo_1],
                           'L2_muon_combined')
        self.addL2Sequence('L2_muon_combined',
                           [ theMuIsoConfig, L2conf.MuisolHypo_1 ],
                           'L2_mu')
        
        # EF sequences
        self.addEFSequence(['L2_mu'],
                           theTrigEFIDInsideOut_Muon.getSequence(),
                           'EFID_mu')
        self.addEFSequence('EFID_mu',
                           [theTrigMuonEFSegmentFinder, TrigMuonEFSegmentFinderHypo()],
                           'EF_MS')
        self.addEFSequence('EF_MS',
                           [theTrigMuonEFTrackBuilder, TrigMuonEFTrackBuilderHypo()],
                           'EF_TB')
        self.addEFSequence('EF_TB',
                           [theTrigMuonEFExtrapolator, TrigMuonEFExtrapolatorHypo()],
                           'EF_SA')
        self.addEFSequence('EF_SA',
                           [theTrigMuonEFCombiner, EFconf.TrigMuonHypo_1],
                           'EF_mu')
        pass

    def defineSignatures(self):
        self.addL2Signature(['L2_muon_standalone1'])
        self.addL2Signature(['L2_muon_combined'])
        self.addL2Signature(['L2_mu'])
        self.addEFSignature(['EFID_mu'])
        self.addEFSignature(['EF_MS'])
        self.addEFSignature(['EF_TB'])
        self.addEFSignature(['EF_SA'])
        self.addEFSignature(['EF_mu'])
        pass

    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        self.TErenamingMap = {
            'L2_muon_standalone1': mergeRemovingOverlap('L2_muon_standalone', L2suf),
            'L2_muon_combined': mergeRemovingOverlap('L2_muon_combined', L2suf),
            'L2_mu': mergeRemovingOverlap('L2_mu', L2suf),
            'EFID_mu': mergeRemovingOverlap('EF_efid_mu', EFsuf),
            'EF_MS': mergeRemovingOverlap('EF_MS', EFsuf),
            'EF_TB': mergeRemovingOverlap('EF_TB', EFsuf),
            'EF_SA': mergeRemovingOverlap('EF_SA', EFsuf),
            'EF_mu': mergeRemovingOverlap('EF_mu', EFsuf),
            }
        pass

###########################################################################
# Basic chain class, EFonly isolation
###########################################################################
class L2EFChain_mu_iso_efonly(L2EFChain_mu):

    # extend with hypo for EFIso
    class EFConfig:
        def __init__(self, suffix, TrigMuonHypo_1, TrkIsolHypo_1):
            self.suffix = suffix
            self.TrigMuonHypo_1 = TrigMuonHypo_1
            self.TrkIsolHypo_1  = TrkIsolHypo_1

    def defineSequences(self, config):
        L2conf = config.L2Config
        EFconf = config.EFConfig
        # L2 sequences
        if UseMuFastHypo or 'MU0' in self.inputTEs:
            self.addL2Sequence(self.inputTEs[0],
                               [ theTrigmuFastMuon, L2conf.MufastHypo_1],
                               'L2_muon_standalone1')
        else:
            self.addL2Sequence(self.inputTEs[0],
                               [ theTrigmuFastMuon, ],
                               'L2_muon_standalone1')
        self.addL2Sequence('L2_mu_standalone1',
                           [ theTrigIDSCAN_Muon,
                             TrigmuCombConfig('Muon', 'IDSCAN'),
                             L2conf.MucombHypo_1],
                           'L2_mu')

        # EF sequences
        self.addEFSequence(['L2_mu'], 
                           theTrigEFIDInsideOut_Muon.getSequence(), 
                           'EFID_mu') 
        self.addEFSequence('EFID_mu', 
                           [theTrigMuonEFSegmentFinder, TrigMuonEFSegmentFinderHypo()], 
                           'EF_MS') 
        self.addEFSequence('EF_MS', 
                           [theTrigMuonEFTrackBuilder, TrigMuonEFTrackBuilderHypo()], 
                           'EF_TB') 
        self.addEFSequence('EF_TB', 
                           [theTrigMuonEFExtrapolator, TrigMuonEFExtrapolatorHypo()], 
                           'EF_SA') 
        self.addEFSequence('EF_SA', 
                           [theTrigMuonEFCombiner, EFconf.TrigMuonHypo_1], 
                           'EF_mu_combined') 
        self.addEFSequence('EF_mu_combined', 
                           theTrigEFIDInsideOut_MuonIso.getSequence(), 
                           'EF_muI_efid') 
        self.addEFSequence('EF_muI_efid', 
                           [theTrigMuonEFTrackIso, EFconf.TrkIsolHypo_1], 
                           'EF_mu') 
        pass

    def defineSignatures(self):
        self.addL2Signature(['L2_muon_standalone1'])
        self.addL2Signature(['L2_mu'])
        
        self.addEFSignature(['EFID_mu']) 
        self.addEFSignature(['EF_MS']) 
        self.addEFSignature(['EF_TB']) 
        self.addEFSignature(['EF_SA']) 
        self.addEFSignature(['EF_mu_combined']) 
        self.addEFSignature(['EF_muI_efid']) 
        self.addEFSignature(['EF_mu']) 
        pass

    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        self.TErenamingMap = {
            'L2_muon_standalone1': mergeRemovingOverlap('L2_muon_standalone', L2suf),
            'L2_muon_combined': mergeRemovingOverlap('L2_muon_combined', L2suf),
            'L2_mu': mergeRemovingOverlap('L2_mu', L2suf),

            'EFID_mu': mergeRemovingOverlap('EF_efid_mu', EFsuf), 
            'EF_MS': mergeRemovingOverlap('EF_MS', EFsuf), 
            'EF_TB': mergeRemovingOverlap('EF_TB', EFsuf), 
            'EF_SA': mergeRemovingOverlap('EF_SA', EFsuf), 
            'EF_mu_combined': mergeRemovingOverlap('EF_mu_combined', EFsuf), 
            'EF_muI_efid': mergeRemovingOverlap('EF_muI_efid', EFsuf), 
            'EF_mu': mergeRemovingOverlap('EF_mu', EFsuf), 
            }
        pass

###########################################################################
# Basic chain class, L2+EF isolation
###########################################################################
class L2EFChain_mu_iso_l2ef(L2EFChain_mu):

    # extend with hypo for EFIso
    class EFConfig:
        def __init__(self, suffix, TrigMuonHypo_1, TrkIsolHypo_1):
            self.suffix = suffix
            self.TrigMuonHypo_1 = TrigMuonHypo_1
            self.TrkIsolHypo_1  = TrkIsolHypo_1

    def defineSequences(self, config):
        L2conf = config.L2Config
        EFconf = config.EFConfig
        # L2 sequences
        if UseMuFastHypo or 'MU0' in self.inputTEs:
            self.addL2Sequence(self.inputTEs[0],
                               [ theTrigmuFastMuon, L2conf.MufastHypo_1],
                               'L2_muon_standalone1')
        else:
            self.addL2Sequence(self.inputTEs[0],
                               [ theTrigmuFastMuon, ],
                               'L2_muon_standalone1')
        self.addL2Sequence('L2_muon_standalone1',
                           [ theTrigIDSCAN_muonIso, 
                             TrigmuCombConfig('Muon', 'IDSCAN'),
                             L2conf.MucombHypo_1],
                           'L2_muon_combined')
        self.addL2Sequence('L2_muon_combined',
                           [ theMuIsoConfig, L2conf.MuisolHypo_1 ],
                           'L2_mu')
        # EF sequences
        self.addEFSequence(['L2_mu'], 
                           theTrigEFIDInsideOut_Muon.getSequence(), 
                           'EFID_mu') 
        self.addEFSequence('EFID_mu', 
                           [theTrigMuonEFSegmentFinder, TrigMuonEFSegmentFinderHypo()], 
                           'EF_MS') 
        self.addEFSequence('EF_MS', 
                           [theTrigMuonEFTrackBuilder, TrigMuonEFTrackBuilderHypo()], 
                           'EF_TB') 
        self.addEFSequence('EF_TB', 
                           [theTrigMuonEFExtrapolator, TrigMuonEFExtrapolatorHypo()], 
                           'EF_SA') 
        self.addEFSequence('EF_SA', 
                           [theTrigMuonEFCombiner, EFconf.TrigMuonHypo_1], 
                           'EF_mu_combined') 
        self.addEFSequence('EF_mu_combined', 
                           theTrigEFIDInsideOut_MuonIso.getSequence(), 
                           'EF_muI_efid') 
        self.addEFSequence('EF_muI_efid', 
                           [theTrigMuonEFTrackIso, EFconf.TrkIsolHypo_1],
                           'EF_mu') 
        pass

    def defineSignatures(self):
        self.addL2Signature(['L2_muon_standalone1'])
        self.addL2Signature(['L2_muon_combined'])
        self.addL2Signature(['L2_mu'])
        
        self.addEFSignature(['EFID_mu']) 
        self.addEFSignature(['EF_MS']) 
        self.addEFSignature(['EF_TB']) 
        self.addEFSignature(['EF_SA']) 
        self.addEFSignature(['EF_mu_combined']) 
        self.addEFSignature(['EF_muI_efid']) 
        self.addEFSignature(['EF_mu']) 
        pass

    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        self.TErenamingMap = {
            'L2_muon_standalone1': mergeRemovingOverlap('L2_muon_standalone', L2suf),
            'L2_muon_combined': mergeRemovingOverlap('L2_muon_combined', L2suf),
            'L2_mu': mergeRemovingOverlap('L2_mu', L2suf),

            'EFID_mu': mergeRemovingOverlap('EF_efid_mu', EFsuf), 
            'EF_MS': mergeRemovingOverlap('EF_MS', EFsuf), 
            'EF_TB': mergeRemovingOverlap('EF_TB', EFsuf), 
            'EF_SA': mergeRemovingOverlap('EF_SA', EFsuf), 
            'EF_mu_combined': mergeRemovingOverlap('EF_mu_combined', EFsuf), 
            'EF_muI_efid': mergeRemovingOverlap('EF_muI_efid', EFsuf), 
            'EF_mu': mergeRemovingOverlap('EF_mu', EFsuf), 
            }
        pass

###########################################################################
#L2 Star version
###########################################################################
class L2EFChain_mu_iso_l2ef_L2StarA(L2EFChain_mu_iso_l2ef):
    def defineSequences(self, config):
        L2EFChain_mu_iso_l2ef.defineSequences(self, config)
        self.l2chain.sequences[1].algos[0] = theTrigL2SiTrackFinder_muonIsoA
        self.l2chain.sequences[1].algos[1] = TrigmuCombConfig('Muon', 'STRATEGY_A')
        self.l2chain.sequences[2].algos[0] = theMuIsoConfig_L2StarA
        pass

class L2EFChain_mu_iso_l2ef_L2StarB(L2EFChain_mu_iso_l2ef):
    def defineSequences(self, config):
        L2EFChain_mu_iso_l2ef.defineSequences(self, config)
        self.l2chain.sequences[1].algos[0] = theTrigL2SiTrackFinder_muonIsoB
        self.l2chain.sequences[1].algos[1] = TrigmuCombConfig('Muon', 'STRATEGY_B')
        self.l2chain.sequences[2].algos[0] = theMuIsoConfig_L2StarB
        pass

class L2EFChain_mu_iso_l2ef_L2StarC(L2EFChain_mu_iso_l2ef):
    def defineSequences(self, config):
        L2EFChain_mu_iso_l2ef.defineSequences(self, config)
        self.l2chain.sequences[1].algos[0] = theTrigL2SiTrackFinder_muonIsoC
        self.l2chain.sequences[1].algos[1] = TrigmuCombConfig('Muon', 'STRATEGY_C')
        self.l2chain.sequences[2].algos[0] = theMuIsoConfig_L2StarC
        pass
                                                                                                                                    
###########################################################################
# Basic chain class, EF based isolation fex and hypo #83402
###########################################################################
class L2EFChain_mu_iso_debug(L2EFChain_mu_iso_l2ef):
    def defineSequences(self, config):
        L2EFChain_mu_iso_l2ef.defineSequences(self, config)
        self.l2chain.sequences[2].algos[1] = None
        self.efchain.sequences[6].algos[1] = None
        pass

###########################################################################
###########################################################################
class L2EFChain_mu_iso_lowpt(L2EFChain_mu_iso):
    def defineSequences(self, config):
        L2EFChain_mu_iso.defineSequences(self, config)
        self.l2chain.sequences[2].algos[0] = theMuIsoConfig_LowPT
        pass

class L2EFChain_mu_iso_900_lowpt(L2EFChain_mu_iso_lowpt):
    def defineSequences(self, config):
        L2EFChain_mu_iso_lowpt.defineSequences(self, config)
        self.l2chain.sequences[0].algos[0] = theTrigmuFast900GeV
        self.l2chain.sequences[1].algos[1] = TrigmuCombConfig('900GeV', 'IDSCAN')
        pass




###########################################################################
# MSonly
###########################################################################
class L2EFChain_mu_MSonly(L2EFChain_mu):
    def defineSequences(self, config):
        L2conf = config.L2Config
        EFconf = config.EFConfig
        self.addL2Sequence(self.inputTEs[0],
                           [ theTrigmuFastMuon, L2conf.MufastHypo_1],
                           'L2_mu_standalone1')
        # EF sequences
        self.addEFSequence('L2_mu_standalone1',
                           [theTrigMuonEFSegmentFinder, TrigMuonEFSegmentFinderHypo()],
                           'EF_MS'),
        self.addEFSequence('EF_MS',
                           [theTrigMuonEFTrackBuilder, TrigMuonEFTrackBuilderHypo()],
                           'EF_TB'),
        self.addEFSequence('EF_TB',
                           [theTrigMuonEFExtrapolator, EFconf.TrigMuonHypo_1],
                           'EF_SA')
        pass
    
    def defineSignatures(self):
        self.addL2Signature(['L2_mu_standalone1'])
        self.addEFSignature(['EF_MS'])
        self.addEFSignature(['EF_TB'])
        self.addEFSignature(['EF_SA'])
        pass
    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        self.TErenamingMap = {
            'L2_mu_standalone1': mergeRemovingOverlap('L2_mu_standalone', L2suf),
            'EF_MS': mergeRemovingOverlap('EF_MS', EFsuf),
            'EF_TB': mergeRemovingOverlap('EF_muTB', EFsuf),
            'EF_SA': mergeRemovingOverlap('EF_muSA', EFsuf),
            }
        pass

    
# ---------------------------------------------------------
class L2EFChain_mu_MSonly_Ecut4(L2EFChain_mu_MSonly): 
    def defineSequences(self, config): 
        L2EFChain_mu_MSonly.defineSequences(self, config) 
        self.l2chain.sequences[0].algos[0] = theTrigmuFastMuonEcut4 
        pass

# ---------------------------------------------------------
class L2EFChain_mu_MSonly_rejectFailedExtrap(L2EFChain_mu_MSonly): 
    def defineSequences(self, config): 
        L2EFChain_mu_MSonly.defineSequences(self, config) 
        self.efchain.sequences[2].algos[0] = theTrigMuonEFExtrapolator_rejectFailedExtrap
        pass 

# ---------------------------------------------------------
# variation on above, for 900GeV config (used in mu4_MSonly)
# ---------------------------------------------------------
class L2EFChain_mu_900_MSonly(L2EFChain_mu_MSonly):
    def defineSequences(self, config):
        L2EFChain_mu_MSonly.defineSequences(self, config)
        self.l2chain.sequences[0].algos[0] = theTrigmuFast900GeV
        pass

class L2EFChain_mu_900_MSonly_EFFS(L2EFChain_mu_900_MSonly):
    def defineSequences(self, config):
        L2EFChain_mu_900_MSonly.defineSequences(self, config)
        self.efchain.sequences[0].algos[0] = theTrigMuonEFSegmentFinder_fs
        pass


###########################################################################
# mu_sitrack
###########################################################################
class L2EFChain_mu_sitrack(L2EFChain_mu):
    def defineSequences(self, config):
        L2conf = config.L2Config
        EFconf = config.EFConfig
        if UseMuFastHypo or 'MU0' in self.inputTEs:
            self.addL2Sequence(self.inputTEs[0],
                               [ theTrigmuFastMuon, L2conf.MufastHypo_1],
                               'L2_mu_standalone1')
        else:
            self.addL2Sequence(self.inputTEs[0],
                               [ theTrigmuFastMuon, ],
                               'L2_mu_standalone1')
        self.addL2Sequence('L2_mu_standalone1',
                           [ TrigSiTrack_Muon(),
                             TrigmuCombConfig('Muon', 'SITRACK'),
                             L2conf.MucombHypo_1],
                           'L2_mu')
        # EF sequences
        self.addEFSequence(['L2_mu'],
                           theTrigEFIDInsideOut_Muon.getSequence(),
                           'EF_efid')
        self.addEFSequence('EF_efid',
                           [theTrigMuonEFSegmentFinder, TrigMuonEFSegmentFinderHypo()],
                           'EF_MS'),
        self.addEFSequence('EF_MS',
                           [theTrigMuonEFTrackBuilder, TrigMuonEFTrackBuilderHypo()],
                           'EF_TB'),
        self.addEFSequence('EF_TB',
                           [theTrigMuonEFExtrapolator, TrigMuonEFExtrapolatorHypo()],
                           'EF_SA'),
        self.addEFSequence('EF_SA',
                           [theTrigMuonEFCombiner, EFconf.TrigMuonHypo_1],
                           'EF_mu'),
        pass

    def defineSignatures(self):
        self.addL2Signature(['L2_mu'])
        self.addEFSignature(['EF_efid'])
        self.addEFSignature(['EF_MS'])
        self.addEFSignature(['EF_TB'])
        self.addEFSignature(['EF_SA'])
        self.addEFSignature(['EF_mu'])
        pass


# ---------------------------------------------------------
class L2EFChain_mu_sitrack_loose(L2EFChain_mu_sitrack):
    def defineSequences(self, config):
        L2EFChain_mu_sitrack.defineSequences(self, config)
        self.l2chain.sequences[0].algos[0] = theTrigmuFast900GeV
#        self.l2chain.sequences[1].algos[0] = TrigmuCombConfig('900GeV', 'SITRACK')
        self.l2chain.sequences[1].algos[1] = TrigmuCombConfig('900GeV', 'SITRACK')
        pass
# ---------------------------------------------------------
# ---------------------------------------------------------



###########################################################################
# mu_noiso_wOvlpRm
###########################################################################
class L2EFChain_mu_noiso_wOvlpRm(L2EFChain_mu):
    def defineSequences(self, config):
        L2conf = config.L2Config
        EFconf = config.EFConfig
        # L2 sequences
        #-------------------------------------------------------
        # HACK for CAF reprocessing (TK, 08.10.2010)
        #-------------------------------------------------------
        if UseMuFastHypo or 'MU0' in self.inputTEs:
            self.addL2Sequence(self.inputTEs[0],
                               [ theTrigmuFastMuon, L2conf.MufastHypo_1],
                               'L2_mu_standalone1')
            self.addL2Sequence(['L2_mu_standalone1'],
                               [ TrigL2MuonOverlapRemoverConfig('Mufast','nominal') ],
                               'L2_mu_standalone1_wOvlpRm')
            self.addL2Sequence('L2_mu_standalone1_wOvlpRm',
                               [ theTrigIDSCAN_Muon,
                                 TrigmuCombConfig('Muon', 'IDSCAN'),
                                 L2conf.MucombHypo_1],
                               'L2_mu_combined1')
            self.addL2Sequence(['L2_mu_combined1'],
                               [ TrigL2MuonOverlapRemoverConfig('Mucomb','nominal') ],
                               'L2_mu_combined1_wOvlpRm')
        else:
            self.addL2Sequence(self.inputTEs[0],
                               [ theTrigmuFastMuon, ], 
                               'L2_mu_standalone1')
            self.addL2Sequence(['L2_mu_standalone1'],
                               [ TrigL2MuonOverlapRemoverConfig('Mufast','nominal') ],
                               'L2_mu_standalone1_wOvlpRm')
            self.addL2Sequence('L2_mu_standalone1_wOvlpRm',
                               [ theTrigIDSCAN_Muon,
                                 TrigmuCombConfig('Muon', 'IDSCAN'),
                                 L2conf.MucombHypo_1],
                               'L2_mu_combined1')
            self.addL2Sequence(['L2_mu_combined1'],
                               [ TrigL2MuonOverlapRemoverConfig('Mucomb','nominal') ],
                               'L2_mu_combined1_wOvlpRm')

        # EF sequences
        self.addEFSequence(['L2_mu_combined1_wOvlpRm'],
                           theTrigEFIDInsideOut_Muon.getSequence(),
                           'EFID_mu')
        self.addEFSequence('EFID_mu',
                           [theTrigMuonEFSegmentFinder, TrigMuonEFSegmentFinderHypo()],
                           'EF_MS')
        self.addEFSequence('EF_MS',
                           [theTrigMuonEFTrackBuilder, TrigMuonEFTrackBuilderHypo()],
                           'EF_TB')
        self.addEFSequence('EF_TB',
                           [theTrigMuonEFExtrapolator, TrigMuonEFExtrapolatorHypo()],
                           'EF_SA')
        self.addEFSequence('EF_SA',
                           [theTrigMuonEFCombiner, EFconf.TrigMuonHypo_1],
                           'EF_mu')
        pass

    def defineSignatures(self):
        self.addL2Signature(['L2_mu_standalone1'])
        self.addL2Signature(['L2_mu_standalone1_wOvlpRm'])
        self.addL2Signature(['L2_mu_combined1'])
        self.addL2Signature(['L2_mu_combined1_wOvlpRm'])
        self.addEFSignature(['EFID_mu'])
        self.addEFSignature(['EF_MS'])
        self.addEFSignature(['EF_TB'])
        self.addEFSignature(['EF_SA'])
        self.addEFSignature(['EF_mu'])
        pass

    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        self.TErenamingMap = {
            'L2_mu_standalone1'        : mergeRemovingOverlap('L2_muon_standalone',         L2suf),
            'L2_mu_standalone1_wOvlpRm': mergeRemovingOverlap('L2_muon_standalone_wOvlpRm', L2suf),
            'L2_mu_combined1'          : mergeRemovingOverlap('L2_muon_combined',           L2suf),
            'L2_mu_combined1_wOvlpRm'  : mergeRemovingOverlap('L2_muon_combined_wOvlpRm',   L2suf),
            'EFID_mu': mergeRemovingOverlap('EF_efid_mu', EFsuf),
            'EF_MS': mergeRemovingOverlap('EF_MS', EFsuf),
            'EF_TB': mergeRemovingOverlap('EF_TB', EFsuf),
            'EF_SA': mergeRemovingOverlap('EF_SA', EFsuf),
            'EF_mu': mergeRemovingOverlap('EF_mu', EFsuf),
            }
        pass

#--------------L2 Star with overlap removal-----------------#
class L2EFChain_L2StarA_wOvlpRm(L2EFChain_mu_noiso_wOvlpRm):
    def defineSequences(self, config):
        L2EFChain_mu_noiso_wOvlpRm.defineSequences(self, config)
        self.l2chain.sequences[2].algos[0] = theTrigL2SiTrackFinder_MuonA
        self.l2chain.sequences[2].algos[1] = TrigmuCombConfig('Muon', 'STRATEGY_A')
        pass

class L2EFChain_L2StarB_wOvlpRm(L2EFChain_mu_noiso_wOvlpRm):
    def defineSequences(self, config):
        L2EFChain_mu_noiso_wOvlpRm.defineSequences(self, config)
        self.l2chain.sequences[2].algos[0] = theTrigL2SiTrackFinder_MuonB
        self.l2chain.sequences[2].algos[1] = TrigmuCombConfig('Muon', 'STRATEGY_B')
        pass
    
class L2EFChain_L2StarC_wOvlpRm(L2EFChain_mu_noiso_wOvlpRm):
    def defineSequences(self, config):
        L2EFChain_mu_noiso_wOvlpRm.defineSequences(self, config)
        self.l2chain.sequences[2].algos[0] = theTrigL2SiTrackFinder_MuonC
        self.l2chain.sequences[2].algos[1] = TrigmuCombConfig('Muon', 'STRATEGY_C')
        pass
    

# ---------------------------------------------------------
class L2EFChain_mu_noiso_wOvlpRm_900(L2EFChain_mu_noiso_wOvlpRm):
    def defineSequences(self, config):
        L2EFChain_mu_noiso_wOvlpRm.defineSequences(self, config)
        self.l2chain.sequences[0].algos[0] = theTrigmuFast900GeV
        self.l2chain.sequences[2].algos[1] = TrigmuCombConfig('900GeV', 'IDSCAN') 
        pass
# ---------------------------------------------------------
# ---------------------------------------------------------



###########################################################################
# MSonly_wOvlpRm
###########################################################################
class L2EFChain_mu_MSonly_wOvlpRm(L2EFChain_mu):
    def defineSequences(self, config):
        L2conf = config.L2Config
        EFconf = config.EFConfig
        self.addL2Sequence(self.inputTEs[0],
                           [ theTrigmuFastMuon, L2conf.MufastHypo_1],
                           'L2_mu_standalone1')
        self.addL2Sequence(['L2_mu_standalone1'],
                           [ TrigL2MuonOverlapRemoverConfig('Mufast','nominal') ],
                           'L2_mu_standalone1_wOvlpRm')

        # EF sequences
        self.addEFSequence('L2_mu_standalone1_wOvlpRm',
                           [theTrigMuonEFSegmentFinder, TrigMuonEFSegmentFinderHypo()],
                           'EF_MS'),
        self.addEFSequence('EF_MS',
                           [theTrigMuonEFTrackBuilder, TrigMuonEFTrackBuilderHypo()],
                           'EF_TB'),
        self.addEFSequence('EF_TB',
                           [theTrigMuonEFExtrapolator, EFconf.TrigMuonHypo_1],
                           'EF_SA')
        pass
    
    def defineSignatures(self):
        self.addL2Signature(['L2_mu_standalone1'])
        self.addL2Signature(['L2_mu_standalone1_wOvlpRm'])
        self.addEFSignature(['EF_MS'])
        self.addEFSignature(['EF_TB'])
        self.addEFSignature(['EF_SA'])
        pass
    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        self.TErenamingMap = {
            'L2_mu_standalone1'        : mergeRemovingOverlap('L2_mu_standalone', L2suf),
            'L2_mu_standalone1_wOvlpRm': mergeRemovingOverlap('L2_muon_standalone_wOvlpRm', L2suf),
            'EF_MS': mergeRemovingOverlap('EF_MS', EFsuf),
            'EF_TB': mergeRemovingOverlap('EF_muTB', EFsuf),
            'EF_SA': mergeRemovingOverlap('EF_muSA', EFsuf),
            }
        pass


# ------------------------------------------------------------------------------------
class L2EFChain_mu_900_MSonly_wOvlpRm(L2EFChain_mu_MSonly_wOvlpRm):
    def defineSequences(self, config):
        L2EFChain_mu_MSonly_wOvlpRm.defineSequences(self, config)
        self.l2chain.sequences[0].algos[0] = theTrigmuFast900GeV
        pass
# ------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------

class L2EFChain_mu_tile_v1(L2EFChainDef):
    class L2Config:
        def __init__(self, suffix, TileMuHypo_1):
            self.suffix       = suffix
            self.TileMuHypo_1 = TileMuHypo_1
            pass

    class EFConfig:
        def __init__(self, suffix, TrigMuonHypo_1):
            self.suffix = suffix
            self.TrigMuonHypo_1 = TrigMuonHypo_1
            pass

    class Config:
        def __init__(self, L2Config, EFConfig):
            self.L2Config = L2Config
            self.EFConfig = EFConfig

    def __init__(self, sig_id,
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter,
                 l2_inputTEs, config,
                 prescale=1, pass_through=0):
        L2EFChainDef.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter,
                              l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter,
                              l2_inputTEs, config,
                              prescale, pass_through)
        #self.l2_lower_chain_name = l2_lower_chain_name
        
    def defineSequences(self, config):
        L2conf = config.L2Config
        EFconf = config.EFConfig
         # L2 sequences
        self.addL2Sequence(self.inputTEs[0],
                           [ TrigTileLookForMuAlg_L2(),
                             theTrigIDSCAN_Tile, TrigTileMuFexConfig('900GeV'),
                             L2conf.TileMuHypo_1 ],
                           'L2_mu_tile1'),

        # EF sequences
        EFconf = config.EFConfig
        self.addEFSequence(['L2_mu_tile1'],
                           theTrigEFIDInsideOut_Muon.getSequence(),
                           'EFID_mu')
        self.addEFSequence('EFID_mu',
                           [theTrigMuonEFSegmentFinder, TrigMuonEFSegmentFinderHypo()],
                           'EF_MS'),
        self.addEFSequence('EF_MS',
                           [theTrigMuonEFTrackBuilder, TrigMuonEFTrackBuilderHypo()],
                           'EF_TB'),
        self.addEFSequence('EF_TB',
                           [theTrigMuonEFExtrapolator, TrigMuonEFExtrapolatorHypo()],
                           'EF_SA'),
        self.addEFSequence('EF_SA',
                           [theTrigMuonEFCombiner, EFconf.TrigMuonHypo_1],
                           'EF_mu'),
        pass

    def defineSignatures(self):
        self.addL2Signature(['L2_mu_tile1'])
        self.addEFSignature(['EFID_mu'])
        self.addEFSignature(['EF_MS'])
        self.addEFSignature(['EF_TB'])
        self.addEFSignature(['EF_SA'])
        self.addEFSignature(['EF_mu'])
        pass

    def defineStreamGroupTriggerType(self):
        self.physics_streams = ['muons']
        self.calib_streams = []
        if('EMPTY' in self.l2_lower_chain_name):
            self.groups = ['RATE:Cosmic_Muon','BW:Muon']
        else:
            self.groups = ['RATE:SingleMuon','BW:Muon']


        self.trigger_type = ['hello']
        pass
    
    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        self.TErenamingMap = {
            'L2_mu_tile1': mergeRemovingOverlap('L2_mu', L2suf),
            'EFID_mu': mergeRemovingOverlap('EF_efid_mu', EFsuf),
            'EF_MS': mergeRemovingOverlap('EF_MS', EFsuf),
            'EF_TB': mergeRemovingOverlap('EF_TB', EFsuf),
            'EF_SA': mergeRemovingOverlap('EF_SA', EFsuf),
            'EF_mu': mergeRemovingOverlap('EF_mu', EFsuf),
            }
        pass
 

###########################################################################
#mugirl chain class
###########################################################################
class L2EFChain_mugirl(L2EFChainDef):
    class L2Config:
        def __init__(self, suffix,
                     MufastHypo_1, MucombHypo_1):
            self.suffix = suffix
            self.MufastHypo_1 = MufastHypo_1
            self.MucombHypo_1 = MucombHypo_1
            pass

    class EFConfig:
        def __init__(self, suffix, 
                     TrigMuGirlHypo_1):
            self.suffix = suffix
            self.TrigMuGirlHypo_1 = TrigMuGirlHypo_1
            pass

    class Config:
        def __init__(self, L2Config, EFConfig):
            self.L2Config = L2Config
            self.EFConfig = EFConfig

    def __init__(self, sig_id,
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter,
                 l2_inputTEs, config,
                 prescale=1, pass_through=0):
        L2EFChainDef.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter,
                              l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter,
                              l2_inputTEs, config,
                              prescale, pass_through)
        #self.l2_lower_chain_name = l2_lower_chain_name
        
    def defineSequences(self, config):
        L2conf = config.L2Config
        EFconf = config.EFConfig
        # L2 sequences
        if UseMuFastHypo or 'MU0' in self.inputTEs:
            self.addL2Sequence(self.inputTEs[0],
                               [ theTrigmuFastMuon, L2conf.MufastHypo_1],
                               'L2_mu_standalone1')
            self.addL2Sequence('L2_mu_standalone1',
                               [ theTrigIDSCAN_Muon,
                                 TrigmuCombConfig('Muon', 'IDSCAN'),
                                 L2conf.MucombHypo_1],
                               'L2_mu')
        else:
            self.addL2Sequence(self.inputTEs[0],
                               [ theTrigmuFastMuon, ],
                               'L2_mu_standalone1')
            self.addL2Sequence('L2_mu_standalone1',
                               [ theTrigIDSCAN_Muon,
                                 TrigmuCombConfig('Muon', 'IDSCAN'),
                                 L2conf.MucombHypo_1],
                               'L2_mu')
        # EF sequences
        self.addEFSequence(['L2_mu'],
                           theTrigEFIDInsideOut_Muon.getSequence(),
                           'EFID_mu')
        self.addEFSequence('EFID_mu',
                           [TrigMuGirlConfig('test'),
                            EFconf.TrigMuGirlHypo_1],
                           'EF_mu')
        pass

    def defineSignatures(self):
        self.addL2Signature(['L2_mu_standalone1'])
        self.addL2Signature(['L2_mu'])
        self.addEFSignature(['EFID_mu'])
        self.addEFSignature(['EF_mu'])
        pass

    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        self.TErenamingMap = {
            'L2_mu_standalone1': mergeRemovingOverlap('L2_muon_standalone', L2suf),
            'L2_mu': mergeRemovingOverlap('L2_mu', L2suf),
            'EFID_mu': mergeRemovingOverlap('EF_efid_mu', EFsuf),
            'EF_mu': mergeRemovingOverlap('EF_mu', EFsuf),
            }
        pass

    def defineStreamGroupTriggerType(self):
        self.physics_streams = ['muons']
        self.calib_streams = []
        if('EMPTY' in self.l2_lower_chain_name):
            self.groups = ['RATE:Cosmic_Muon','BW:Muon']
        else:
            self.groups = ['RATE:SingleMuon','BW:Muon']


    
        self.trigger_type = ['hello']
        pass



# ------------------------------------------------------------------------------------
class L2EFChain_900_mugirl(L2EFChain_mugirl):
    def defineSequences(self, config):
        L2EFChain_mugirl.defineSequences(self, config)
        self.l2chain.sequences[0].algos[0] = theTrigmuFast900GeV
        self.l2chain.sequences[1].algos[1] = TrigmuCombConfig('900GeV', 'IDSCAN')
# ------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------


###########################################################################
#
###########################################################################
class L2EFChain_mugirlslow(L2EFChainDef):
    class L2Config:
        def __init__(self, suffix,
                     MufastStauHypo_1, MucombStauHypo_1):
            self.suffix = suffix
            self.MufastStauHypo_1 = MufastStauHypo_1
            self.MucombStauHypo_1 = MucombStauHypo_1
            pass

    class EFConfig:
        def __init__(self, suffix, 
                     TrigMuGirlStauHypo_1):
            self.suffix = suffix
            self.TrigMuGirlStauHypo_1 = TrigMuGirlStauHypo_1
            pass

    class Config:
        def __init__(self, L2Config, EFConfig):
            self.L2Config = L2Config
            self.EFConfig = EFConfig

    def __init__(self, sig_id,
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter,
                 l2_inputTEs, config,
                 prescale=1, pass_through=0):
        L2EFChainDef.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter,
                              l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter,
                              l2_inputTEs, config,
                              prescale, pass_through)
        #self.l2_lower_chain_name = l2_lower_chain_name
        
    def defineSequences(self, config):
        L2conf = config.L2Config
        EFconf = config.EFConfig
        # L2 sequences
        self.addL2Sequence(self.inputTEs[0],
                           [ theTrigmuFastMuonEcut4, L2conf.MufastStauHypo_1],
                           'L2_mu_standalone2')
        self.addL2Sequence('L2_mu_standalone2',
                           [ theTrigIDSCAN_Muon,
                             TrigmuCombConfig('Muon', 'IDSCAN'),
                             L2conf.MucombStauHypo_1],
                           'L2_mu')
        # EF sequences
        self.addEFSequence(['L2_mu'],
                           theTrigEFIDInsideOut_Muon.getSequence(),
                           'EFID_mu')
        self.addEFSequence('EFID_mu',
                           [TrigMuGirlConfig('Slow'),
                            EFconf.TrigMuGirlStauHypo_1],
                           'EF_mu')
        pass

    def defineSignatures(self):
        self.addL2Signature(['L2_mu_standalone2'])
        self.addL2Signature(['L2_mu'])
        self.addEFSignature(['EFID_mu'])
        self.addEFSignature(['EF_mu'])
        pass

    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        self.TErenamingMap = {
            'L2_mu_standalone2': mergeRemovingOverlap('L2_muon_standalone', L2suf),
            'L2_mu': mergeRemovingOverlap('L2_mu', L2suf),
            'EFID_mu': mergeRemovingOverlap('EF_efid_mu', EFsuf),
            'EF_mu': mergeRemovingOverlap('EF_mu', EFsuf),
            }
        pass


    def defineStreamGroupTriggerType(self):
        self.physics_streams = ['muons']
        self.calib_streams = []
        if('EMPTY' in self.l2_lower_chain_name):
            self.groups = ['RATE:Cosmic_Muon','BW:Muon']
        else:
            self.groups = ['RATE:SingleMuon','BW:Muon']

        self.trigger_type = ['hello']
        pass


# ------------------------------------------------------------------------------------
class L2EFChain_mugirlslow_empty(L2EFChain_mugirlslow):
    def defineSequences(self, config):
        L2EFChain_mugirlslow.defineSequences(self, config)
        self.l2chain.sequences[0].algos[0] = theTrigmuFastMuonEcut4_empty
# ------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------



###########################################################################
#
###########################################################################
class L2EFChain_mugirlslow_out(L2EFChainDef):
    class L2Config:
        def __init__(self, suffix,
                     MufastStauHypo_1, MucombStauHypo_1):
            self.suffix = suffix
            self.MufastStauHypo_1 = MufastStauHypo_1
            self.MucombStauHypo_1 = MucombStauHypo_1
            pass

    class EFConfig:
        def __init__(self, suffix, 
                     TrigMuGirlStauHypo_1):
            self.suffix = suffix
            self.TrigMuGirlStauHypo_1 = TrigMuGirlStauHypo_1
            pass

    class Config:
        def __init__(self, L2Config, EFConfig):
            self.L2Config = L2Config
            self.EFConfig = EFConfig

    def __init__(self, sig_id,
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter,
                 l2_inputTEs, config,
                 prescale=1, pass_through=0):
        L2EFChainDef.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter,
                              l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter,
                              l2_inputTEs, config,
                              prescale, pass_through)
        #self.l2_lower_chain_name = l2_lower_chain_name


    def defineSequences(self, config):
        L2conf = config.L2Config
        EFconf = config.EFConfig
        # L2 sequences

        self.addL2Sequence('',
                           [ TrigmuRoIConfig()],
                           'L2_mu_out_of_time')

        self.addL2Sequence('L2_mu_out_of_time',
                           [ theTrigmuFastOutBCIDMuonEcut4, L2conf.MufastStauHypo_1],
                           'L2_mu_standalone2')
        self.addL2Sequence('L2_mu_standalone2',
                           [ theTrigIDSCAN_Muon,
                             TrigmuCombConfig('Muon', 'IDSCAN'),
                             L2conf.MucombStauHypo_1],
                           'L2_mu')
        # EF sequences
        self.addEFSequence(['L2_mu'],
                           theTrigEFIDInsideOut_Muon.getSequence(),
                           'EFID_mu')
        self.addEFSequence('EFID_mu',
                           [TrigMuGirlConfig('Slow'),
                            EFconf.TrigMuGirlStauHypo_1],
                           'EF_mu')
        pass

    def defineSignatures(self):
        self.addL2Signature(['L2_mu_standalone2'])
        self.addL2Signature(['L2_mu'])
        self.addEFSignature(['EFID_mu'])
        self.addEFSignature(['EF_mu'])
        pass

    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        self.TErenamingMap = {
            'L2_mu_standalone2': mergeRemovingOverlap('L2_muon_standalone', L2suf),
            'L2_mu': mergeRemovingOverlap('L2_mu', L2suf),
            'EFID_mu': mergeRemovingOverlap('EF_efid_mu', EFsuf),
            'EF_mu': mergeRemovingOverlap('EF_mu', EFsuf),
            }
        pass


    def defineStreamGroupTriggerType(self):
        self.physics_streams = ['muons']
        self.calib_streams = [] 
        if('EMPTY' in self.l2_lower_chain_name):
            self.groups = ['RATE:Cosmic_Muon','BW:Muon']
        else:
            self.groups = ['RATE:SingleMuon','BW:Muon']

        self.trigger_type = ['hello']
        pass


###########################################################################
#
###########################################################################
class L2EFChain_mu_cal(L2EFChain_mu):
    def __init__(self, sig_id,
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter,
                 l2_inputTEs, config):
        L2EFChain_mu.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter,
                              l2_lower_chain_name,
                              '', -1 ,
                              l2_inputTEs, config)
        
    def defineSequences(self, config):
        L2conf = config.L2Config
        EFconf = None
        self.addL2Sequence(self.inputTEs[0],
                           [ theTrigmuFastCAL, MufastCALHypoConfig('MuonCal','Reject') ],
                           'L2_mu_cal')
        pass
    
    def defineSignatures(self):
        self.addL2Signature(['L2_mu_cal'])
        pass
    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        self.TErenamingMap = {
            'L2_mu_cal': mergeRemovingOverlap('L2_mu_cal', L2suf),
            }
        pass
    def defineStreamGroupTriggerType(self):
        self.physics_streams = []
        self.calib_streams = ['Muon_Calibration']
        if('EMPTY' in self.l2_lower_chain_name):
            self.groups = ['RATE:Cosmic_Muon','BW:Muon']
        else:
            self.groups = ['RATE:SingleMuon','BW:Muon']
        self.trigger_type = ['hello']
        pass
        
#The one mugirl chain:
Config   = L2EFChain_mugirl.Config
L2Config = L2EFChain_mugirl.L2Config
EFConfig = L2EFChain_mugirl.EFConfig



###########################################################################
# RoI based
###########################################################################
class L2EFChain_mu_noroi(L2EFChainDef):
    def __init__(self, sig_id,
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter,
                 l2_inputTEs, config,
                 prescale=1, pass_through=0):
        L2EFChainDef.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter,
                              l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter,
                              l2_inputTEs, config,
                              prescale, pass_through)

    class L2Config:
        def __init__(self,suffix):
            self.suffix = suffix
            from TrigGenericAlgs.TrigGenericAlgsConf import AcceptWhenInputMissing
            self.L2_Dummy = AcceptWhenInputMissing("L2Dummy%s" % suffix)
            pass

    class EFConfig:
        def __init__(self,suffix):
            self.suffix = suffix
            from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
            self.EF_Dummy = DummyFEX("EFDummy%s" % suffix)
            pass
    
    class Config:
        def __init__(self, L2Config, EFConfig):
            self.L2Config = L2Config
            self.EFConfig = EFConfig
            pass

    def defineSequences(self, config):
        L2conf = config.L2Config
        self.addL2Sequence(self.inputTEs, [ config.L2Config.L2_Dummy ],'L2_mu_noroi')
        self.addEFSequence('L2_mu_noroi', [ config.EFConfig.EF_Dummy ],'EF_mu_noroi')

    def defineSignatures(self):
        self.addL2Signature(['L2_mu_noroi'])
        self.addEFSignature(['EF_mu_noroi'])
        
    def defineStreamGroupTriggerType(self):
        self.physics_streams = ['muons']
        self.calib_streams = []
        if('EMPTY' in self.l2_lower_chain_name):
            self.groups = ['RATE:Cosmic_Muon','BW:Muon']
        else:
            self.groups = ['RATE:SingleMuon','BW:Muon']

        self.trigger_type = ['hello']

    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        self.TErenamingMap = {
            'L2_mu_noroi': mergeRemovingOverlap('L2_mu_noroi', L2suf),
            'EF_mu_noroi': mergeRemovingOverlap('EF_mu_noroi', EFsuf),
            }
        pass



###########################################################################
# mu_NoAlg
###########################################################################
class L2EFChain_mu_NoAlg(L2EFChainDef):
    def __init__(self, sig_id,
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter,
                 l2_inputTEs, config,
                 prescale=1, pass_through=0):
        L2EFChainDef.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter,
                              l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter,
                              l2_inputTEs, config,
                              prescale, pass_through)
        
    def defineStreamGroupTriggerType(self):
        self.physics_streams = ['muons']
        self.calib_streams = []
        self.groups = ['RATE:SingleMuon','BW:Muon']


        self.trigger_type = ['hello']

class L2EFChain_out_of_time(L2EFChainDef):
    def __init__(self, sig_id,
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter,
                 l2_inputTEs, config,
                 prescale=1, pass_through=0):
        L2EFChainDef.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter,
                              l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter,
                              l2_inputTEs, config,
                              prescale, pass_through)

    class L2Config:
        def __init__(self, suffix,
                     MufastHypo_1):
            self.suffix = suffix
            self.MufastHypo_1 = MufastHypo_1
            pass

    class EFConfig:
        def __init__(self, suffix):
            self.suffix = suffix
            pass

    class Config:
        def __init__(self, L2Config, EFConfig):
            self.L2Config = L2Config
            self.EFConfig = EFConfig

    def defineSequences(self, config):
        L2conf = config.L2Config
        EFconf = config.EFConfig
        # L2 sequences
        self.addL2Sequence('', 
                           [ TrigmuRoIConfig()], 
                           'L2_mu_out_of_time') 

        self.addL2Sequence('L2_mu_out_of_time', 
                           [ theTrigmuFastOutBCID, L2conf.MufastHypo_1], 
                           'L2_mu_standalone1') 

    def defineSignatures(self):
        self.addL2Signature(['L2_mu_standalone1'])

    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        self.TErenamingMap = {
            'L2_mu_standalone1': mergeRemovingOverlap('L2_mu_standalone', L2suf) }

    def defineStreamGroupTriggerType(self):
        self.physics_streams = ['muons']
        self.calib_streams = []
        self.groups = ['RATE:SingleMuon','BW:Muon']

        self.trigger_type = ['hello']



#####################################################################
#
#####################################################################
class L2EFChain_mu_L2Star_NoIDTrkCut(L2EFChain_mu_noiso):
 
    def defineSequences(self, config):
        
        self.addL2Sequence(self.inputTEs[0],
                           [ theTrigmuFastMuon, config.L2Config.MufastHypo_1],
                           'L2_mu_standalone1')
        self.addL2Sequence('L2_mu_standalone1',
                           [ theTrigL2SiTrackFinder_MuonA,
                             theTrigL2SiTrackFinder_MuonB,
                             theTrigL2SiTrackFinder_MuonC,
                             TrigmuCombConfig('Muon', 'IDSCAN'),
                             config.L2Config.MucombHypo_1],
                           'L2_mu')
        
        # EF sequences
        self.efchain.sequences=[]
        self.addEFSequence(['L2_mu'],
                           theTrigEFIDInsideOut_Muon.getSequence(),
                           'EFID_mu')
        self.addEFSequence(['EFID_mu'],
                           theTrigEFIDOutsideInTRTOnly_Muon.getSequence(),
                           'EF_trt')        
        self.addEFSequence('EF_trt',
                           [theTrigMuonEFSegmentFinder, TrigMuonEFSegmentFinderHypo()],
                           'EF_MS')
        self.addEFSequence('EF_MS',
                           [theTrigMuonEFTrackBuilder, TrigMuonEFTrackBuilderHypo()],
                           'EF_TB')
        self.addEFSequence('EF_TB',
                           [theTrigMuonEFExtrapolator, config.EFConfig.TrigMuonHypo_1],
                           'EF_SA')
        self.addEFSequence('EF_SA',
                           [theTrigMuonEFCombiner, TrigMuonEFCombinerHypoConfig('Muon','passthrough')],
                           'EF_mu')
        pass

    def defineSignatures(self):
        self.addL2Signature(['L2_mu_standalone1'])
        self.addL2Signature(['L2_mu'])
        self.addEFSignature(['EFID_mu'])
        self.addEFSignature(['EF_trt'])
        self.addEFSignature(['EF_MS'])
        self.addEFSignature(['EF_TB'])
        self.addEFSignature(['EF_SA'])
        self.addEFSignature(['EF_mu'])
        pass

    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        self.TErenamingMap = {
            'L2_mu_standalone1': mergeRemovingOverlap('L2_muon_standalone', L2suf),
            'L2_mu': mergeRemovingOverlap('L2_mu', L2suf),
            'EFID_mu': mergeRemovingOverlap('EF_EFID_mu', EFsuf),
            'EF_trt': mergeRemovingOverlap('EF_trt', EFsuf),
            'EF_MS': mergeRemovingOverlap('EF_MS', EFsuf),
            'EF_TB': mergeRemovingOverlap('EF_TB', EFsuf),
            'EF_SA': mergeRemovingOverlap('EF_SA', EFsuf),
            'EF_mu': mergeRemovingOverlap('EF_mu', EFsuf),
            }
        pass

#####################################################################

#####################################################################
class L2EFChain_mu_EFmerged(L2EFChain_mu):
    def __init__(self, sig_id, chain_counter,
                 l2_lower_chain_name, l2_inputTEs,
                 muThr, EFiso=None, L2iso=None, EFalg=None,
                 prescale=1, pass_through=0, rerun=0):

        global muFastThrMap

        # expand input for subclass __init__
        l2_chain_counter = chain_counter
        ef_chain_counter = chain_counter
        l2_chain_name = "L2_" + sig_id
        ef_chain_name = "EF_" + sig_id
        if not isinstance(l2_inputTEs, (list,tuple)):
            l2_inputTEs = [ l2_inputTEs ]

        # chose default EF algorithm
        if EFalg is None: EFalg = getTrigMuSuperEF()
        # determine hypo name (actually only part of name)
        EFalgName = EFalg.getName()
        underscore = EFalgName.find('_')
        if underscore == -1:
            hypoName = 'OR_Muon'
        else:
            hypoName = 'OR%s_Muon' % EFalgName[underscore:]

        suffix = "_" + sig_id        
        muFastThr=muFastThrMap[muThr]
        l2_config = self.__class__.L2Config(suffix,hypoName,muFastThr,muThr,L2iso)
        ef_config = self.__class__.EFConfig(suffix,hypoName,EFalg,muThr,EFiso)
        config = self.__class__.Config(l2_config,ef_config)

        L2EFChain_mu.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter,
                              l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter,
                              l2_inputTEs, config,
                              prescale, pass_through, rerun)


    class L2Config:
        def __init__(self, suffix, hypoName, muFastThr, muCombThr, iso):
            self.suffix = suffix
            self.muFastThr = muFastThr
            self.muCombThr = muCombThr
            self.iso = iso
            self.hypoName = hypoName
            

    class EFConfig:
        def __init__(self, suffix, hypoName, EFalg, muThr, iso):
            self.suffix = suffix
            self.EFalg  = EFalg
            self.muThr  = muThr
            self.iso    = iso
            self.hypoName = hypoName

    
    def defineSequences(self, config):
        self.defineL2Sequence(config.L2Config)
        self.defineEFSequence(config.EFConfig)



    def defineL2Sequence(self,L2conf):
        # L2 sequences
        #-------------------------------------------------------
        # HACK for CAF reprocessing (TK, 08.10.2010)
        #-------------------------------------------------------
        if UseMuFastHypo or 'MU0' in self.inputTEs:
            self.addL2Sequence(self.inputTEs[0],
                               [ theTrigmuFastMuon, MufastHypoConfig('Muon', L2conf.muFastThr) ],
                               'L2_mu_standalone1')
        else:
            self.addL2Sequence(self.inputTEs[0],
                               [ theTrigmuFastMuon, ],
                               'L2_mu_standalone1')

        if L2conf.iso is not None: # with L2 isolation
            theTrigIDSCAN = theTrigIDSCAN_muonIso
            L2outputTE = 'L2_muon_combined'
        else: # no L2 isolation
            theTrigIDSCAN = theTrigIDSCAN_Muon
            L2outputTE = 'L2_mu'
            
        self.addL2Sequence('L2_mu_standalone1',
                           [ theTrigIDSCAN,
                             TrigmuCombConfig('Muon', 'IDSCAN'),
                             MucombHypoConfig('Muon', L2conf.muCombThr) ],
                           L2outputTE)


        if L2conf.iso is not None: # with L2 isolation
            self.addL2Sequence(L2outputTE,
                               [ theMuIsoConfig, MuisoHypoConfig(L2conf.hypoName, L2conf.iso) ],
                               'L2_mu')

    # end of defineL2Sequence
        

    def defineEFSequence(self,EFconf):
        self.addEFSequence(['L2_mu'],
                           theTrigEFIDInsideOut_Muon.getSequence(),
                           'EFID_mu')

        if EFconf.iso is None:
            EFoutputTE = 'EF_mu'
        else:
            EFoutputTE = 'EF_mu_combined'
            
        self.addEFSequence('EFID_mu',
                           [ EFconf.EFalg, TrigMuonEFCombinerHypoConfig(EFconf.hypoName,EFconf.muThr) ],
                           EFoutputTE)

        if EFconf.iso is not None:
            self.addEFSequence(EFoutputTE, 
                               theTrigEFIDInsideOut_MuonIso.getSequence(), 
                               'EF_muI_efid') 
            self.addEFSequence('EF_muI_efid', 
                               [theTrigMuSuperEFTrackIso,
                                TrigMuonEFTrackIsolationHypoConfig(EFconf.hypoName,EFconf.iso)],
                               'EF_mu') 

    # end of defineEFSequence


    def defineSignatures(self):
        # L2 signatures
        self.addL2Signature(['L2_mu_standalone1'])
        if self.config.L2Config.iso is not None:
            self.addL2Signature(['L2_muon_combined'])
        self.addL2Signature(['L2_mu'])
        # EF signatures
        self.addEFSignature(['EFID_mu'])
        if self.config.EFConfig.iso is not None:
            self.addEFSignature(['EF_mu_combined']) 
            self.addEFSignature(['EF_muI_efid']) 
        self.addEFSignature(['EF_mu'])


    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        self.TErenamingMap = {
            'L2_mu_standalone1': mergeRemovingOverlap('L2_muon_standalone', L2suf),
            'L2_muon_combined' : mergeRemovingOverlap('L2_muon_combined', L2suf), 
            'L2_mu'            : mergeRemovingOverlap('L2_mu', L2suf),
            'EFID_mu'          : mergeRemovingOverlap('EF_efid_mu', EFsuf),
            'EF_mu_combined'   : mergeRemovingOverlap('EF_mu_combined', EFsuf), 
            'EF_muI_efid'      : mergeRemovingOverlap('EF_muI_efid', EFsuf), 
            'EF_mu'            : mergeRemovingOverlap('EF_mu', EFsuf),
            }






class L2EFChain_mu_EFmerged_TMEFfirst(L2EFChain_mu_EFmerged):
    def __init__(self, sig_id, chain_counter,
                 l2_lower_chain_name, l2_inputTEs,
                 muThr, EFiso=None, L2iso=None, EFalg=None,
                 prescale=1, pass_through=0, rerun=0):
                 
        # chose default EF algorithm
        if EFalg is None: EFalg = getTrigMuSuperEF_TMEFfirst()
        # call the base class constructor
        L2EFChain_mu_EFmerged.__init__(self,sig_id, chain_counter,
                                       l2_lower_chain_name, l2_inputTEs,
                                       muThr, EFiso=EFiso, L2iso=L2iso, EFalg=EFalg,
                                       prescale=prescale,
                                       pass_through=pass_through,
                                       rerun=rerun)
    # end of __init__



class L2EFChain_mu_EFmerged_MGfirst(L2EFChain_mu_EFmerged):
    def __init__(self, sig_id, chain_counter,
                 l2_lower_chain_name, l2_inputTEs,
                 muThr, EFiso=None, L2iso=None, EFalg=None,
                 prescale=1, pass_through=0, rerun=0):
                 
        # chose default EF algorithm
        if EFalg is None: EFalg = getTrigMuSuperEF_MGfirst()
        # call the base class constructor
        L2EFChain_mu_EFmerged.__init__(self,sig_id, chain_counter,
                                       l2_lower_chain_name, l2_inputTEs,
                                       muThr, EFiso=EFiso, L2iso=L2iso, EFalg=EFalg,
                                       prescale=prescale,
                                       pass_through=pass_through,
                                       rerun=rerun)
    # end of __init__

class L2EFChain_mu_EFmerged_TMEFonly(L2EFChain_mu_EFmerged):
    def __init__(self, sig_id, chain_counter,
                 l2_lower_chain_name, l2_inputTEs,
                 muThr, EFiso=None, L2iso=None, EFalg=None,
                 prescale=1, pass_through=0, rerun=0):
                 
        # chose default EF algorithm
        if EFalg is None: EFalg = getTrigMuSuperEF_TMEFonly()
        # call the base class constructor
        L2EFChain_mu_EFmerged.__init__(self,sig_id, chain_counter,
                                       l2_lower_chain_name, l2_inputTEs,
                                       muThr, EFiso=EFiso, L2iso=L2iso, EFalg=EFalg,
                                       prescale=prescale,
                                       pass_through=pass_through,
                                       rerun=rerun)
    # end of __init__
            

class L2EFChain_mu_EFmerged_MGonly(L2EFChain_mu_EFmerged):
    def __init__(self, sig_id, chain_counter,
                 l2_lower_chain_name, l2_inputTEs,
                 muThr, EFiso=None, L2iso=None, EFalg=None,
                 prescale=1, pass_through=0, rerun=0):
                 
        # chose default EF algorithm
        if EFalg is None: EFalg = getTrigMuSuperEF_MGonly()
        # call the base class constructor
        L2EFChain_mu_EFmerged.__init__(self,sig_id, chain_counter,
                                       l2_lower_chain_name, l2_inputTEs,
                                       muThr, EFiso=EFiso, L2iso=L2iso, EFalg=EFalg,
                                       prescale=prescale,
                                       pass_through=pass_through,
                                       rerun=rerun)
    # end of __init__
            

class L2EFChain_mu_EFmerged_both(L2EFChain_mu_EFmerged):
    def __init__(self, sig_id, chain_counter,
                 l2_lower_chain_name, l2_inputTEs,
                 muThr, EFiso=None, L2iso=None, EFalg=None,
                 prescale=1, pass_through=0, rerun=0):
                 
        # chose default EF algorithm
        if EFalg is None: EFalg = getTrigMuSuperEF_both()
        # call the base class constructor
        L2EFChain_mu_EFmerged.__init__(self,sig_id, chain_counter,
                                       l2_lower_chain_name, l2_inputTEs,
                                       muThr, EFiso=EFiso, L2iso=L2iso, EFalg=EFalg,
                                       prescale=prescale,
                                       pass_through=pass_through,
                                       rerun=rerun)
    # end of __init__


#####################################################################

class L2EFChain_mu_iso_annulus(L2EFChain_mu):

    # extend with hypo for EFIso
    class EFConfig:
        def __init__(self, suffix, TrigMuonHypo_1, TrkIsolHypo_1):
            self.suffix = suffix
            self.TrigMuonHypo_1 = TrigMuonHypo_1
            self.TrkIsolHypo_1  = TrkIsolHypo_1

    def defineSequences(self, config):
        L2conf = config.L2Config
        EFconf = config.EFConfig
        # L2 sequences
        if UseMuFastHypo or 'MU0' in self.inputTEs:
            self.addL2Sequence(self.inputTEs[0],
                               [ theTrigmuFastMuon, L2conf.MufastHypo_1],
                               'L2_muon_standalone1')
        else:
            self.addL2Sequence(self.inputTEs[0],
                               [ theTrigmuFastMuon, ],
                               'L2_muon_standalone1')
        self.addL2Sequence('L2_mu_standalone1',
                           [ theTrigIDSCAN_Muon,
                             TrigmuCombConfig('Muon', 'IDSCAN'),
                             L2conf.MucombHypo_1],
                           'L2_mu')

        # EF sequences
        self.addEFSequence(['L2_mu'],
                           theTrigEFIDInsideOut_Muon.getSequence(),
                           'EFID_mu')
        self.addEFSequence('EFID_mu',
                           [theTrigMuonEFSegmentFinder, TrigMuonEFSegmentFinderHypo()],
                           'EF_MS')
        self.addEFSequence('EF_MS',
                           [theTrigMuonEFTrackBuilder, TrigMuonEFTrackBuilderHypo()],
                           'EF_TB')
        self.addEFSequence('EF_TB',
                           [theTrigMuonEFExtrapolator, TrigMuonEFExtrapolatorHypo()],
                           'EF_SA')
        self.addEFSequence('EF_SA',
                           [theTrigMuonEFCombiner, EFconf.TrigMuonHypo_1],
                           'EF_mu_combined')
        self.addEFSequence('EF_mu_combined',
                           theTrigEFIDInsideOut_MuonIso.getSequence(),
                           'EF_muI_efid')
        self.addEFSequence('EF_muI_efid',
                           [theTrigMuonEFTrackIsoAnnulus, EFconf.TrkIsolHypo_1],
                           'EF_mu')
        pass

    def defineSignatures(self):
        self.addL2Signature(['L2_muon_standalone1'])
        self.addL2Signature(['L2_mu'])
        self.addEFSignature(['EFID_mu'])
        self.addEFSignature(['EF_MS'])
        self.addEFSignature(['EF_TB'])
        self.addEFSignature(['EF_SA'])
        self.addEFSignature(['EF_mu_combined'])
        self.addEFSignature(['EF_muI_efid'])
        self.addEFSignature(['EF_mu'])
        pass

    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        self.TErenamingMap = {
            'L2_muon_standalone1': mergeRemovingOverlap('L2_muon_standalone', L2suf),
            'L2_muon_combined': mergeRemovingOverlap('L2_muon_combined', L2suf),
            'L2_mu': mergeRemovingOverlap('L2_mu', L2suf),
            'EFID_mu': mergeRemovingOverlap('EF_efid_mu', EFsuf),
            'EF_MS': mergeRemovingOverlap('EF_MS', EFsuf),
            'EF_TB': mergeRemovingOverlap('EF_TB', EFsuf),
            'EF_SA': mergeRemovingOverlap('EF_SA', EFsuf),
            'EF_mu_combined': mergeRemovingOverlap('EF_mu_combined', EFsuf),
            'EF_muI_efid': mergeRemovingOverlap('EF_muI_efid', EFsuf),
            'EF_mu': mergeRemovingOverlap('EF_mu', EFsuf),
            }
        pass


class L2EFChain_mu_MSonly_wOvlpRm_iso_annulus(L2EFChain_mu):
    class EFConfig:
        def __init__(self, suffix, TrigMuonHypo_1, TrkIsolHypo_1):
            self.suffix = suffix
            self.TrigMuonHypo_1 = TrigMuonHypo_1
            self.TrkIsolHypo_1  = TrkIsolHypo_1

    def defineSequences(self, config):
        L2conf = config.L2Config
        EFconf = config.EFConfig
        self.addL2Sequence(self.inputTEs[0],
                           [ theTrigmuFastMuon, L2conf.MufastHypo_1],
                           'L2_mu_standalone1')
        self.addL2Sequence(['L2_mu_standalone1'],
                           [ TrigL2MuonOverlapRemoverConfig('Mufast','nominal') ],
                           'L2_mu_standalone1_wOvlpRm')

        # EF sequences
        self.addEFSequence('L2_mu_standalone1_wOvlpRm',
                           [theTrigMuonEFSegmentFinder, TrigMuonEFSegmentFinderHypo()],
                           'EF_MS'),
        self.addEFSequence('EF_MS',
                           [theTrigMuonEFTrackBuilder, TrigMuonEFTrackBuilderHypo()],
                           'EF_TB'),
        self.addEFSequence('EF_TB',
                           [theTrigMuonEFExtrapolator, EFconf.TrigMuonHypo_1],
                           'EF_SA')
        self.addEFSequence('EF_SA',
                           theTrigEFIDInsideOut_MuonIso.getSequence(),
                           'EF_muI_efid')
        self.addEFSequence('EF_muI_efid',
                           [theTrigMuonEFTrackIsoAnnulus, EFconf.TrkIsolHypo_1],
                           'EF_mu')
        pass
    
    def defineSignatures(self):
        self.addL2Signature(['L2_mu_standalone1'])
        self.addL2Signature(['L2_mu_standalone1_wOvlpRm'])
        self.addEFSignature(['EF_MS'])
        self.addEFSignature(['EF_TB'])
        self.addEFSignature(['EF_SA'])
        self.addEFSignature(['EF_muI_efid'])
        self.addEFSignature(['EF_mu'])
        pass
    
    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        self.TErenamingMap = {
            'L2_mu_standalone1'        : mergeRemovingOverlap('L2_mu_standalone', L2suf),
            'L2_mu_standalone1_wOvlpRm': mergeRemovingOverlap('L2_muon_standalone_wOvlpRm', L2suf),
            'EF_MS': mergeRemovingOverlap('EF_MS', EFsuf),
            'EF_TB': mergeRemovingOverlap('EF_TB', EFsuf),
            'EF_SA': mergeRemovingOverlap('EF_SA', EFsuf),
            'EF_muI_efid': mergeRemovingOverlap('EF_muI_efid', EFsuf),
            'EF_mu': mergeRemovingOverlap('EF_mu', EFsuf),
            }
        pass
    
#####################################################################
# TrigL2MuonSA classes
#####################################################################

class L2EFChain_mu_trigL2MuonSA(L2EFChain_mu_noiso):
    def defineSequences(self, config):
        L2EFChain_mu_noiso.defineSequences(self, config)
        self.l2chain.sequences[0].algos[0] = theTrigL2MuonSAMuon

class L2EFChain_mu_trigL2MuonSA_wOvlpRm(L2EFChain_mu_noiso_wOvlpRm):
    def defineSequences(self, config):
        L2EFChain_mu_noiso_wOvlpRm.defineSequences(self, config)
        self.l2chain.sequences[0].algos[0] = theTrigL2MuonSAMuon

class L2EFChain_mu_trigL2MuonSA_900GeV(L2EFChain_mu_trigL2MuonSA):
    def defineSequences(self, config):
        L2EFChain_mu_trigL2MuonSA.defineSequences(self, config)
        self.l2chain.sequences[0].algos[0] = theTrigL2MuonSA900GeV
        self.l2chain.sequences[1].algos[1] = TrigmuCombConfig('900GeV', 'IDSCAN')

class L2EFChain_mu_trigL2MuonSA_900GeV_wOvlpRm(L2EFChain_mu_trigL2MuonSA_wOvlpRm):
    def defineSequences(self, config):
        L2EFChain_mu_trigL2MuonSA_wOvlpRm.defineSequences(self, config)
        self.l2chain.sequences[0].algos[0] = theTrigL2MuonSA900GeV
        self.l2chain.sequences[1].algos[1] = TrigmuCombConfig('900GeV', 'IDSCAN')

#####################################################################
#####################################################################
    
if __name__ == '__main__':
    triggerPythonConfig = TriggerPythonConfig('hlt.xml', None)
    for m in Muons:
        m.generateMenu(triggerPythonConfig)
    triggerPythonConfig.writeConfigFiles()
    
