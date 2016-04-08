# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigJetHypo.TrigJetHypoConf import TrigL2JetHypo
from TrigJetHypo.TrigJetHypoConf import TrigL2JetHypoNoise
from TrigJetHypo.TrigJetHypoConf import TrigL2MultiJetHypo
from TrigJetHypo.TrigJetHypoConf import TrigL2MultiJetAllTE
from TrigJetHypo.TrigJetHypoConf import TrigL2HTFullScanHypo 
from TrigJetHypo.TrigJetHypoConf import TrigL2HTAllTE 
from TrigJetHypo.TrigJetHypoConf import TrigEFJetHypo
#from TrigJetHypo.TrigJetHypoConf import TrigEFJetHypo_RF
from TrigJetHypo.TrigJetHypoConf import TrigEFJetHypoNoise
from LArCellRec.LArCellRecConf import LArNoisyROTool

from AthenaCommon.SystemOfUnits import GeV

JetHypos = {}

# FullScan hypos
class L2MultiJetHypoBase (TrigL2MultiJetHypo):
    __slots__ = []
    def __init__(self, name):
        super( L2MultiJetHypoBase, self )._init__( name )
        
        from TrigJetHypo.TrigJetHypoMonitoring import TrigL2MultiJetHypoValidationMonitoring, TrigL2MultiJetHypoOnlineMonitoring, TrigL2MultiJetHypoCosmicMonitoring
        validation = TrigL2MultiJetHypoValidationMonitoring()
        online = TrigL2MultiJetHypoOnlineMonitoring()
        cosmic = TrigL2MultiJetHypoCosmicMonitoring()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("L2MultiJetHypo_Time")
        
        self.AthenaMonTools = [ time, validation, online, cosmic ]
        
        
class L2FullScanMultiJetHypo (L2MultiJetHypoBase):
    __slots__ = []
    def __init__(self, name = "L2FullScanMultiJetHypo",l2_thr=20*GeV,multiplicity=2, l2_thrs=[], etaMin=0., etaMax=3.2):
        super( L2FullScanMultiJetHypo, self ).__init__( name )
        self.Etcut_L2 = l2_thr
        self.Multiplicity_L2 = multiplicity
        self.EtThresholds=l2_thrs
        self.etaMinCut = etaMin
        self.etaMaxCut = etaMax
        

class L2FullScanMultiJetNoCutHypo (L2MultiJetHypoBase):
    __slots__ = []
    def __init__(self, name = "L2FullScanMultiJetNoCutHypo"):
        super( L2FullScanMultiJetNoCutHypo, self ).__init__( name )
        self.Etcut_L2 = 0
        self.Multiplicity_L2 = 1
        self.EtThresholds=[]
        self.AcceptAll=True
        

class L2MultiJetAllTEBase (TrigL2MultiJetAllTE):
    __slots__ = []
    def __init__(self, name):
        super( L2MultiJetAllTEBase, self ).__init__( name )
        
        from TrigJetHypo.TrigJetHypoMonitoring import TrigL2MultiJetAllTEValidationMonitoring, TrigL2MultiJetAllTEOnlineMonitoring, TrigL2MultiJetAllTECosmicMonitoring
        validation = TrigL2MultiJetAllTEValidationMonitoring()
        online = TrigL2MultiJetAllTEOnlineMonitoring()
        cosmic = TrigL2MultiJetAllTECosmicMonitoring()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("L2MultiJetAllTE_Time")
        
        self.AthenaMonTools = [ time, validation, online, cosmic ]
        
        
class L2FullScanMultiJetAllTE (L2MultiJetAllTEBase):
    __slots__ = []
    def __init__(self, name = "L2FullScanMultiJetAllTE",l2_thr=20*GeV,multiplicity=2, l2_thrs=[], etaMin=0., etaMax=3.2):
        super( L2FullScanMultiJetAllTE, self ).__init__( name )
        self.Etcut_L2 = l2_thr
        self.Multiplicity_L2 = multiplicity
        self.EtThresholds=l2_thrs
        self.etaMinCut = etaMin
        self.etaMaxCut = etaMax
        
class L2HTHypoBase (TrigL2HTAllTE):
    __slots__ = []
    def __init__(self, name):
        super( L2HTHypoBase, self ).__init__( name )
        
        from TrigJetHypo.TrigJetHypoMonitoring import TrigL2HTFullScanHypoValidationMonitoring, TrigL2HTFullScanHypoOnlineMonitoring, TrigL2HTFullScanHypoCosmicMonitoring
        validation = TrigL2HTFullScanHypoValidationMonitoring()
        online = TrigL2HTFullScanHypoOnlineMonitoring()
        cosmic = TrigL2HTFullScanHypoCosmicMonitoring()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("L2HTHypo_Time")
        
        self.AthenaMonTools = [ time, validation, online, cosmic ]
        
class FullScanL2HTHypo (L2HTHypoBase):
    __slots__ = []
    def __init__(self, name = "L2FullScanL2HTHypo",ht_thr=350*GeV,l2_thr=20*GeV,etaMin=0,etaMax=3.2,l2_thrs=[]):
        super( FullScanL2HTHypo, self ).__init__( name )
        self.HTcut = ht_thr
        self.Etcut = l2_thr			
        self.etaMincut = etaMin			
        self.etaMaxcut = etaMax			
        			

class FullScanL2HTNoCutHypo (L2HTHypoBase):
    __slots__ = []
    def __init__(self, name = "L2FullScanL2HTNoCutHypo"):
        super( FullScanL2HTNoCutHypo, self ).__init__( name )
        self.AcceptAll=True

class L2JetHypoBase (TrigL2JetHypo):
    __slots__ = []
    def __init__(self, name):
        super( L2JetHypoBase, self ).__init__( name )
        
        from TrigJetHypo.TrigJetHypoMonitoring import TrigL2JetHypoValidationMonitoring, TrigL2JetHypoOnlineMonitoring, TrigL2JetHypoCosmicMonitoring
        validation = TrigL2JetHypoValidationMonitoring()
        online = TrigL2JetHypoOnlineMonitoring()
        cosmic = TrigL2JetHypoCosmicMonitoring()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("L2JetHypo_Time")
        
        self.AthenaMonTools = [ time, validation, online, cosmic ]
        
        # switch on basic cleaning by default for all chains
        #self.doBasicCleaning = True
        self.n90CleaningThreshold = 2
        self.presamplerCleaningThreshold = 0.9

class EFJetHypoBase (TrigEFJetHypo):
    __slots__ = []
    def __init__(self, name):
        super( EFJetHypoBase, self ).__init__( name )

        from TrigJetHypo.TrigJetHypoMonitoring import TrigEFJetHypoValidationMonitoring, TrigEFJetHypoOnlineMonitoring, TrigEFJetHypoCosmicMonitoring
        validation = TrigEFJetHypoValidationMonitoring()
        online = TrigEFJetHypoOnlineMonitoring()
        cosmic = TrigEFJetHypoCosmicMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("EFJetHypo_Time")

        self.AthenaMonTools = [ time, validation, online, cosmic ]
        
        # switch on basic cleaning by default for all chains
        #self.doBasicCleaning = True
        self.n90CleaningThreshold = 2
        self.presamplerCleaningThreshold = 0.9
        self.negativeECleaningThreshold = -60e3    # 60 GeV
        self.qmeanCleaningThreshold = 0.8
        self.HECQCleaningThreshold = 0.5
        self.HECfCleaningThreshold = 0.5
        self.LArQCleaningThreshold = 0.8
        self.EMfCleaningThreshold = 0.95

        
#class EFJetHypoBase_RF (TrigEFJetHypo_RF):
#    __slots__ = []
#    def __init__(self, name):
#        super( EFJetHypoBase, self ).__init__( name )
# 
#        from TrigJetHypo.TrigJetHypoMonitoring import TrigEFJetHypoValidationMonitoring, TrigEFJetHypoOnlineMonitoring, TrigEFJetHypoCosmicMonitoring
#        validation = TrigEFJetHypoValidationMonitoring()
#        online = TrigEFJetHypoOnlineMonitoring()
#        cosmic = TrigEFJetHypoCosmicMonitoring()
# 
#        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
#        time = TrigTimeHistToolConfig("EFJetHypo_Time")
# 
#        self.AthenaMonTools = [ time, validation, online, cosmic ]
#        
#        # switch on basic cleaning by default for all chains
#        #self.doBasicCleaning = True
#        self.n90CleaningThreshold = 2
#        self.presamplerCleaningThreshold = 0.9
#        self.negativeECleaningThreshold = -60e3    # 60 GeV
#        self.qmeanCleaningThreshold = 0.8
#        self.HECQCleaningThreshold = 0.5
#        self.HECfCleaningThreshold = 0.5
#        self.LArQCleaningThreshold = 0.8
#        self.EMfCleaningThreshold = 0.95

        
        
class L2JetHypo (L2JetHypoBase):
    __slots__ = []
    def __init__(self, name = "L2JetHypo",l2_thr=20*GeV):
        super( L2JetHypo, self ).__init__( name )

        self.Etcut_L2 = l2_thr
        self.doHECfnLeadingCellsCleaning = False
        self.doEMfJetQualityCleaning = False
        self.doHECfJetQualityCleaning = False
        self.doTimeQualityCleaning = False
        self.doEMfCleaningHighEta = False
        self.doEMfCleaningLowEta = False

class L2JetHypoNoiseBase (TrigL2JetHypoNoise):
    __slots__ = []
    def __init__(self, name):
        super( L2JetHypoNoiseBase, self ).__init__( name )

#        from TrigJetHypo.TrigJetHypoMonitoring import TrigL2JetHypoValidationMonitoring, TrigL2JetHypoOnlineMonitoring, TrigL2JetHypoCosmicMonitoring
#        validation = TrigL2JetHypoValidationMonitoring()
#        online = TrigL2JetHypoOnlineMonitoring()
#        cosmic = TrigL2JetHypoCosmicMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("L2JetHypoNoise_Time")

        #self.AthenaMonTools = [ time, validation, online, cosmic ]
        self.AthenaMonTools = [ time ]

class L2JetHypoNoiseTight (L2JetHypoNoiseBase):
    __slots__ = []
    def __init__(self, name = "L2JetHypoNoiseTight",l2_thr=20*GeV):
        super( L2JetHypoNoiseTight, self ).__init__( name )

        self.Etcut_L2 = l2_thr
        self.doNLeadingCellsUncleaning = True
        self.doJetQualityUncleaning = True
        self.leadingCellsThr = 6
        self.jetQualityThr = 0.7

class L2JetHypoNoise (L2JetHypoNoiseBase):
    __slots__ = []
    def __init__(self, name = "L2JetHypoNoise",l2_thr=20*GeV):
        super( L2JetHypoNoise, self ).__init__( name )

        self.Etcut_L2 = l2_thr
        self.doNLeadingCellsUncleaning = True
        self.doJetQualityUncleaning = True
        self.leadingCellsThr = 4
        self.jetQualityThr = 0.5

class EFJetHypo (EFJetHypoBase):
    __slots__ = []
    def __init__(self, name = "EFJetHypo",ef_thr=20*GeV,etaMin=0,etaMax=10):
        super( EFJetHypo, self ).__init__( name )

        self.Etcut = ef_thr
        self.etaMincut = etaMin
        self.etaMaxcut = etaMax

class EFJetHypoNoiseConfig (TrigEFJetHypoNoise):
    __slots__ = []
    def __init__(self, name = "EFJetHypoNoiseConfig",ef_thr=20*GeV,etaMin=0,etaMax=10):
        super( EFJetHypoNoiseConfig, self ).__init__( name )

        self.Etcut = ef_thr
        self.BadFEBCut=3
        theLArNoisyROTool=LArNoisyROTool()
        self.NoiseTool = theLArNoisyROTool


class EFCentJetHypo (EFJetHypoBase):
    __slots__ = []
    def __init__(self, name = "EFCentJetHypo",ef_thr=20*GeV,etaMin=0,etaMax=3.2):
        super( EFCentJetHypo, self ).__init__( name )

        self.Etcut = ef_thr
        self.etaMincut = etaMin
        self.etaMaxcut = etaMax


class EFFwdJetHypo (EFJetHypoBase):
    __slots__ = []
    def __init__(self, name = "EFFwdJetHypo",ef_thr=20*GeV,etaMin=3.2,etaMax=10):
        super( EFFwdJetHypo, self ).__init__( name )

        self.Etcut = ef_thr
        self.etaMincut = etaMin
        self.etaMaxcut = etaMax



class L2JetHypo_noJetCut (L2JetHypo):
    __slots__ = []
    def __init__(self, name = "L2JetHypo_noJetCut",l2_thr=20*GeV):
        super( L2JetHypo_noJetCut, self ).__init__( name )

        self.Etcut_L2 = l2_thr
        self.AcceptAll = True

class EFJetHypo_noJetCut (EFJetHypo):
    __slots__ = []
    def __init__(self, name = "EFJetHypo_noJetCut",ef_thr=20*GeV,etaMin=0,etaMax=10):
        super( EFJetHypo_noJetCut, self ).__init__( name )

        self.Etcut = ef_thr
        self.etaMincut = etaMin
        self.etaMaxcut = etaMax
        self.AcceptAll = True
        self.forceAccept = True

# FullScan dijet hypos

class EFFullScanMultiJetHypo (EFJetHypoBase):
    __slots__ = []
    def __init__(self, name = "EFFullScanMultiJetHypo",ef_thr=20*GeV,multiplicity=2, ef_thrs=[],etaMin=0,etaMax=10):
        super( EFFullScanMultiJetHypo, self ).__init__( name )

        self.Etcut = ef_thr
        self.etaMincut = etaMin
        self.etaMaxcut = etaMax
        self.Multiplicitycut = multiplicity
        self.EtThresholds=ef_thrs

class EFFullScanMultiJetHypo (EFJetHypoBase):
    __slots__ = []
    def __init__(self, name = "EFFullScanMultiJetHypo",ef_thr=20*GeV,multiplicity=2, ef_thrs=[],etaMin=0,etaMax=10):
        super( EFFullScanMultiJetHypo, self ).__init__( name )

        self.Etcut = ef_thr
        self.etaMincut = etaMin
        self.etaMaxcut = etaMax
        self.Multiplicitycut = multiplicity
        self.EtThresholds=ef_thrs

class EFFwdFullScanMultiJetHypo (EFJetHypoBase):
    __slots__ = []
    def __init__(self, name = "EFFwdFullScanMultiJetHypo",ef_thr=20*GeV,multiplicity=2, ef_thrs=[],etaMin=3.2,etaMax=10):
        super( EFFwdFullScanMultiJetHypo, self ).__init__( name )

        self.Etcut = ef_thr
        self.etaMincut = etaMin
        self.etaMaxcut = etaMax
        self.Multiplicitycut = multiplicity
        self.EtThresholds=ef_thrs

class EFCentFullScanMultiJetHypo (EFJetHypoBase):
    __slots__ = []
    def __init__(self, name = "EFCentFullScanMultiJetHypo",ef_thr=20*GeV,multiplicity=2, ef_thrs=[],etaMin=0,etaMax=3.2):
        super( EFCentFullScanMultiJetHypo, self ).__init__( name )

        self.Etcut = ef_thr
        self.etaMincut = etaMin
        self.etaMaxcut = etaMax
        self.Multiplicitycut = multiplicity
        self.EtThresholds=ef_thrs

#class EFJetHypo_RF (EFJetHypoBase_RF):
#    __slots__ = []
#    def __init__(self, name = "EFCentFullScanMultiJetHypo",ef_thr=20*GeV,multiplicity=2, ef_thrs=[],etaMin=0,etaMax=3.2):
#        super( EFJetHypoBase_RF, self ).__init__( name )
# 
#        self.Etcut = ef_thr
#        self.etaMincut = etaMin
#        self.etaMaxcut = etaMax
#        self.Multiplicitycut = multiplicity
#        self.EtThresholds=ef_thrs
        

class EFFullScanMultiJetHypo_noJetCut (EFJetHypo):
    __slots__ = []
    def __init__(self, name = "EFFullScanMultiJetHypo_noJetCut",ef_thr=20*GeV, multiplicity=2, ef_thrs=[],etaMin=0,etaMax=10):
        super( EFFullScanMultiJetHypo_noJetCut, self ).__init__( name )

        self.Etcut = ef_thr
        self.EtThresholds=ef_thrs
        self.etaMincut = etaMin
        self.etaMaxcut = etaMax
        self.AcceptAll = True
        self.forceAccept = True
        self.Multiplicitycut = multiplicity



        
# create concrete instances of standard dijet hypos,
# for use in the improved menu generation
EFFullScanDijet20Hypo       = EFFullScanMultiJetHypo("EFjetHypo_2j20fs", multiplicity=2)
EFFullScanDijet20Hypo_NoCut = EFFullScanMultiJetHypo_noJetCut("EFJetHypo_2j20fs_jetNoCut", multiplicity=2)

JetHypos["EFjetHypo_2j20fs"]          = EFFullScanDijet20Hypo
JetHypos["EFjetHypo_2j20fs_jetNoCut"] = EFFullScanDijet20Hypo_NoCut

# print JetHypos
# print EFFullScanDijet20Hypo
# print EFFullScanDijet20Hypo_NoCut


# hypo applying conservative L2 jet cleaning
class L2JetHypo_doCleaning_conservative (L2JetHypo):
    __slots__ = []
    def __init__(self, name = "L2JetHypo_doCleaning_conservative",l2_thr=20*GeV):
        super( L2JetHypo_doCleaning_conservative, self ).__init__( name )

        self.Etcut_L2 = l2_thr
        self.doHECfnLeadingCellsCleaning = True
        self.doEMfJetQualityCleaning = True
        self.doHECfJetQualityCleaning = True
        self.doTimeQualityCleaning = True
        self.doEMfCleaningHighEta = True
        self.doEMfCleaningLowEta = True
        self.applyCleaningToHighEtJets = True
        self.applyCleaningToLowEtJets = True
        self.cleaningHighEtThreshold = 1000000.
        self.cleaningLowEtThreshold = 20000.
        self.leadingCellsThr = 1
        self.hecfThrN = 0.9
        self.hecfThrQ = 0.8
        self.jetQualityThrH = 0.9
        self.jetQualityThrE = 0.95
        self.emfThrQ = 0.99
        self.etaThrEmfQ = 2.7
        self.emfThrEmfHighEta = 0.02
        self.etaThrEmfHighEta = 2.1
        self.emfThrEmfLowEta = 0.01
        self.etaThrEmfLowEta = 2.1
        self.timeThrTimeQuality = 30
        self.qualityThrTimeQuality = 0.25


# hypo applying L2 jet cleaning aiming to reduce hec spikes & presampler noise
class L2JetHypo_doCleaning_HECPS (L2JetHypo):
    __slots__ = []
    def __init__(self, name = "L2JetHypo_doCleaning_HECPS",l2_thr=20*GeV):
        super( L2JetHypo_doCleaning_HECPS, self ).__init__( name )

        self.Etcut_L2 = l2_thr
        self.doHECfnLeadingCellsCleaning = False
        self.doEMfJetQualityCleaning = True
        self.doHECfJetQualityCleaning = True
        self.doTimeQualityCleaning = False
        self.doEMfCleaningHighEta = False
        self.doEMfCleaningLowEta = False
        self.applyCleaningToHighEtJets = True
        self.applyCleaningToLowEtJets = True
        self.cleaningHighEtThreshold = 1000000.
        self.cleaningLowEtThreshold = 20000.
        self.leadingCellsThr = 1
        self.hecfThrN = 0.9
        self.hecfThrQ = 0.8
        self.jetQualityThrH = 0.9
        self.jetQualityThrE = 0.95
        self.emfThrQ = 0.99
        self.etaThrEmfQ = 1.8
        self.emfThrEmfHighEta = 0.02
        self.etaThrEmfHighEta = 2.1
        self.emfThrEmfLowEta = 0.01
        self.etaThrEmfLowEta = 2.1
        self.timeThrTimeQuality = 30
        self.qualityThrTimeQuality = 0.25


# hypo applying EF jet cleaning
class EFJetHypo_doCleaning (EFJetHypoBase):
    __slots__ = []
    def __init__(self, name = "EFJetHypo_doCleaning",ef_thr=20*GeV,etaMin=0,etaMax=3.2):
        super( EFJetHypo_doCleaning, self ).__init__( name )
        
        self.Etcut = ef_thr
        self.etaMincut = etaMin
        self.etaMaxcut = etaMax
        
        self.doBasicCleaning = True
        self.n90CleaningThreshold = 2
        self.presamplerCleaningThreshold = 0.9
        self.negativeECleaningThreshold = -60e3    # 60 GeV
        self.qmeanCleaningThreshold = 0.8
        self.HECQCleaningThreshold = 0.5
        self.HECfCleaningThreshold = 0.5
        self.LArQCleaningThreshold = 0.8
        self.EMfCleaningThreshold = 0.95


# hypo applying reverse EF jet cleaning; trigger only on bad jets
class EFJetHypo_doCleaningInverse (EFJetHypoBase):
    __slots__ = []
    def __init__(self, name = "EFJetHypo_doCleaningInverse",ef_thr=20*GeV,etaMin=0,etaMax=3.2):
        super( EFJetHypo_doCleaningInverse, self ).__init__( name )
        
        self.Etcut = ef_thr
        self.etaMincut = etaMin
        self.etaMaxcut = etaMax

        self.doBasicCleaning = True
        self.triggerOnBad = True
        self.n90CleaningThreshold = 2
        self.presamplerCleaningThreshold = 0.9
        self.negativeECleaningThreshold = -60e3    # 60 GeV
        self.qmeanCleaningThreshold = 0.8
        self.HECQCleaningThreshold = 0.5
        self.HECfCleaningThreshold = 0.5
        self.LArQCleaningThreshold = 0.8
        self.EMfCleaningThreshold = 0.95

###################
#
# next 3 hypos are default L1.5 L2 EF hypos with basic cleaning switched on
# can be removed after testing is done
#
###################

# default EF hypo applying basic jet cleaning
class EFJetHypo_doBasicCleaning (EFJetHypoBase):
    __slots__ = []
    def __init__(self, name = "EFJetHypo_doBasicCleaning",ef_thr=20*GeV,etaMin=0,etaMax=3.2):
        super( EFJetHypo_doBasicCleaning, self ).__init__( name )
        
        self.Etcut = ef_thr
        self.etaMincut = etaMin
        self.etaMaxcut = etaMax
        
        self.doBasicCleaning = True
        self.n90CleaningThreshold = 2
        self.presamplerCleaningThreshold = 0.9
        self.negativeECleaningThreshold = -60e3    # 60 GeV


# default EF forward hypo applying basic jet cleaning
class EFFwdJetHypo_doBasicCleaning (EFJetHypoBase):
    __slots__ = []
    def __init__(self, name = "EFFwdJetHypo_doBasicCleaning",ef_thr=20*GeV,etaMin=3.2,etaMax=10):
        super( EFFwdJetHypo_doBasicCleaning, self ).__init__( name )
        
        self.Etcut = ef_thr
        self.etaMincut = etaMin
        self.etaMaxcut = etaMax
        
        self.doBasicCleaning = True
        self.n90CleaningThreshold = 2
        self.presamplerCleaningThreshold = 0.9
        self.negativeECleaningThreshold = -60e3    # 60 GeV


# default L2 base hypo with n90 cleaning cleaning enabled
class L2JetHypo_doBasicCleaning (L2JetHypo):
    __slots__ = []
    def __init__(self, name = "L2JetHypo_doBasicCleaning",l2_thr=20*GeV):
        super( L2JetHypo_doBasicCleaning, self ).__init__( name )
        
        self.Etcut_L2 = l2_thr
        self.doBasicCleaning = True
        self.n90CleaningThreshold = 2


# default L1.5 base hypo with n90 cleaning cleaning enabled
class L2FullScanMultiJetAllTE_doBasicCleaning (L2MultiJetAllTEBase):
    __slots__ = []
    def __init__(self, name = "L2FullScanMultiJetAllTE_doBasicCleaning",l2_thr=20*GeV,multiplicity=2, l2_thrs=[], etaMin=0., etaMax=3.2):
        super( L2FullScanMultiJetAllTE_doBasicCleaning, self ).__init__( name )
        self.Etcut_L2 = l2_thr
        self.Multiplicity_L2 = multiplicity
        self.EtThresholds=l2_thrs
        self.etaMinCut = etaMin
        self.etaMaxCut = etaMax
        
        self.n90Cut = 2
        self.etaMaxN90Cut = 2.5

