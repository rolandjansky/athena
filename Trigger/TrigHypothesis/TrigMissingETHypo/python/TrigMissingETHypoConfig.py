# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from TrigMissingETHypo.TrigMissingETHypoConf import TrigEFMissingETHypo

from AthenaCommon.SystemOfUnits import GeV

class EFMissingETHypoBase (TrigEFMissingETHypo):
    __slots__ = []
    def __init__(self, name):
        super( EFMissingETHypoBase, self ).__init__( name )

        from TrigMissingETHypo.TrigMissingETHypoMonitoring import TrigEFMissingETHypoValidationMonitoring, TrigEFMissingETHypoCosmicMonitoring
        validation = TrigEFMissingETHypoValidationMonitoring()
        cosmic = TrigEFMissingETHypoCosmicMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("METHypo_Time")
        time.TimerHistLimits = [0, 5]

        self.AthenaMonTools = [ time, validation, cosmic ]

    def onlineMonitoring(self, full=False):
        if "TrigEFMissingETHypoOnlineMonitoring" not in [t.name() for t in self.AthenaMonTools]:
            from TrigMissingETHypo.TrigMissingETHypoMonitoring import TrigEFMissingETHypoOnlineMonitoring, TrigEFMissingETHypoFullOnlineMonitoring
            online = TrigEFMissingETHypoOnlineMonitoring()
            self.AthenaMonTools += [ online ]

        if full == True and "TrigEFMissingETHypoFullOnlineMonitoring" not in [t.name() for t in self.AthenaMonTools]:
            from TrigMissingETHypo.TrigMissingETHypoMonitoring import TrigEFMissingETHypoFullOnlineMonitoring
            onlineFull = TrigEFMissingETHypoFullOnlineMonitoring()
            self.AthenaMonTools += [ onlineFull ]


class EFJetEtSumHypo (EFMissingETHypoBase):
    __slots__ = []
    def __init__(self, name = "EFJetEtSumHypo_1000",ef_thr=1000*GeV):
        super( EFJetEtSumHypo, self ).__init__( name )

        self.SumETCut=ef_thr
        self.CutType=-2.0
        self.MissingETCut=100000000.*GeV
        self.METLabel='EFJetEtSum'
        self.doMuonCorrection = False

class L2JetEtSumHypo (EFMissingETHypoBase):
    __slots__ = []
    def __init__(self, name = "L2JetEtSumHypo_1000",l2_thr=1000*GeV):
        super( L2JetEtSumHypo, self ).__init__( name )

        self.SumETCut=l2_thr
        self.CutType=-2.0
        self.MissingETCut=100000000.*GeV
        self.METLabel='L2JetEtSum'
        self.doMuonCorrection = False

class L2MetHypoXE (EFMissingETHypoBase):
    __slots__ = []
    def __init__(self, name = "L2MetHypo_xe1000",l2_thr=1000*GeV):
        super( L2MetHypoXE, self ).__init__( name )

        self.SumETCut=l2_thr
        self.MissingETCut=l2_thr
        self.CutType=-2.0
        self.METLabel='T2MissingET'
        self.doMuonCorrection = False
        self.SumETCut = 100000000*GeV
        self.forceAccept=False
        self.onlineMonitoring(False)
        if 'wMu' in name:
            self.doMuonCorrection = True
        if 'Mon' in name:
            self.onlineMonitoring(True)
        if 'noL2' in name:
            self.MissingETCut=-100*GeV
            self.SumETCut=-100*GeV
        if 'xe30' in name:
            self.onlineMonitoring(True)

class L2MetHypoFEBXE (EFMissingETHypoBase):
    __slots__ = []
    def __init__(self, name = "L2MetHypo_FEB_xe1000",l2_thr=1000*GeV):
        super( L2MetHypoFEBXE, self ).__init__( name )

        self.SumETCut=l2_thr
        self.MissingETCut=l2_thr
        self.CutType=-2.0
        self.METLabel='L2MissingET_FEB'
        self.doMuonCorrection = False
        self.SumETCut = 100000000*GeV
        self.forceAccept=False
        self.onlineMonitoring(False)
        if 'wMu' in name:
            self.doMuonCorrection = True
        if 'Mon' in name:
            self.onlineMonitoring(True)
        if 'noL2' in name:
            self.MissingETCut=-100*GeV
            self.SumETCut=-100*GeV
        if 'xe30' in name:
            self.onlineMonitoring(True)

class L2MetHypoFEBXE_L1check (EFMissingETHypoBase):
    __slots__ = []
    def __init__(self, name = "L2MetHypo_FEB_xe1000",l2_thr=1000*GeV):
        super( L2MetHypoFEBXE_L1check, self ).__init__( name )

        self.SumETCut=l2_thr
        self.MissingETCut=l2_thr
        self.CutType=-2.0
        self.METLabel='L2MissingET_FEB'
        self.doMuonCorrection = False
        self.SumETCut = 100000000*GeV
        self.forceAccept=False
        self.onlineMonitoring(False)
        self.doL1L2FEBTest=True
        self.L1L2FEBTolerance=80.0*GeV
        if 'wMu' in name:
            self.doMuonCorrection = True
        if 'Mon' in name:
            self.onlineMonitoring(True)
        if 'noL2' in name:
            self.MissingETCut=-100*GeV
            self.SumETCut=-100*GeV
        if 'xe30' in name:
            self.onlineMonitoring(True)

class EFMetHypoXE (EFMissingETHypoBase):
    __slots__ = []
    def __init__(self, name = "EFMetHypo_xe1000",ef_thr=1000*GeV):
        super( EFMetHypoXE, self ).__init__( name )

        self.SumETCut=ef_thr
        self.MissingETCut=ef_thr
        self.CutType=-2.0
        self.METLabel='TrigEFMissingET'
        self.doMuonCorrection = False
        self.SumETCut = 100000000*GeV
        self.forceAccept=False
        self.onlineMonitoring(False)
        self.doLArH11off=False
        self.doLArH12off=False
        self.doMETphicut=False
        if 'LArH11off' in name:
            self.doLArH11off = True
        if 'LArH12off' in name:
            self.doLArH12off = True
        if 'METphi' in name:
            self.doMETphicut = True
        if 'wMu' in name:
            self.doMuonCorrection = True
        if 'Mon' in name:
            self.onlineMonitoring(True)
        if 'noEF' in name:
            self.MissingETCut=-100*GeV
            self.SumETCut=-100*GeV
        if 'xe30' in name:
            self.onlineMonitoring(True)

class EFMetHypoFEBXE (EFMissingETHypoBase):
    __slots__ = []
    def __init__(self, name = "EFMetHypoFEB_xe1000",ef_thr=1000*GeV):
        super( EFMetHypoFEBXE, self ).__init__( name )

        self.SumETCut=ef_thr
        self.MissingETCut=ef_thr
        self.CutType=-2.0
        self.METLabel='TrigEFMissingET_FEB'
        self.doMuonCorrection = False
        self.SumETCut = 100000000*GeV
        self.forceAccept=False
        self.onlineMonitoring(False)
        if 'wMu' in name:
            self.doMuonCorrection = True
        if 'Mon' in name:
            self.onlineMonitoring(True)
        if 'noEF' in name:
            self.MissingETCut=-100*GeV
            self.SumETCut=-100*GeV
        if 'xe30' in name:
            self.onlineMonitoring(True)

class EFMetHypoTCXE (EFMissingETHypoBase):
    __slots__ = []
    def __init__(self, name = "EFMetHypo_tcxe1000",ef_thr=1000*GeV):
        super( EFMetHypoTCXE, self ).__init__( name )

        self.SumETCut=ef_thr
        self.MissingETCut=ef_thr
        self.CutType=-2.0
        self.METLabel='TrigEFMissingET_topocl'
        self.doMuonCorrection = False
        self.SumETCut = 100000000*GeV
        self.forceAccept=False
        self.onlineMonitoring(False)
        self.doEMScaleTC=True
        if 'tchad' in name:
            self.doEMScaleTC = False
        if 'tclcw' in name:
            self.doEMScaleTC = False
        if 'tcem' in name:
            self.doEMScaleTC = True
        if 'noFW' in name:
            self.doOnlyCalcCentralMET = True
        if 'wMu' in name:
            self.doMuonCorrection = True
        if 'noEF' in name:
            self.MissingETCut=-100*GeV
            self.SumETCut=-100*GeV
        if 'xe30' in name:
            self.onlineMonitoring(True)

class EFMetHypoTCPSXE (EFMissingETHypoBase):
    __slots__ = []
    def __init__(self, name = "EFMetHypo_tcpsxe1000",ef_thr=1000*GeV):
        super( EFMetHypoTCPSXE, self ).__init__( name )

        self.SumETCut=ef_thr
        self.MissingETCut=ef_thr
        self.CutType=-2.0
        self.METLabel='TrigEFMissingET_topocl_PS'
        self.doMuonCorrection = False
        self.SumETCut = 100000000*GeV
        self.forceAccept=False
        self.onlineMonitoring(False)
        self.doEMScaleTC=True
        if 'tchad' in name:
            self.doEMScaleTC = False
        if 'tclcw' in name:
            self.doEMScaleTC = False
        if 'tcem' in name:
            self.doEMScaleTC = True
        if 'wMu' in name:
            self.doMuonCorrection = True
        if 'noEF' in name:
            self.MissingETCut=-100*GeV
            self.SumETCut=-100*GeV
        if 'xe30' in name:
            self.onlineMonitoring(True)

class EFMetHypoJetsXE (EFMissingETHypoBase):
    __slots__ = []
    def __init__(self, name = "EFMetHypo_mhtxe1000",ef_thr=1000*GeV, extraCalib = ""):
        super( EFMetHypoJetsXE, self ).__init__( name )

        self.SumETCut=ef_thr
        self.MissingETCut=ef_thr
        self.CutType=-2.0
        self.METLabel='TrigEFMissingET_mht{0}'.format(extraCalib)
        self.doMuonCorrection = False
        self.SumETCut = 100000000*GeV
        self.forceAccept=False
        self.onlineMonitoring(False)
        self.doEMScaleTC=False
        if 'noFW' in name:
            self.doOnlyCalcCentralMET = True
        if 'wMu' in name:
            self.doMuonCorrection = True
        if 'noEF' in name:
            self.MissingETCut=-100*GeV
            self.SumETCut=-100*GeV
        if 'xe30' in name:
            self.onlineMonitoring(True)

class EFMetHypoTrackAndJetsXE (EFMissingETHypoBase):
    __slots__ = []
    def __init__(self, name = "EFMetHypo_trkmhtxe1000",ef_thr=1000*GeV):
        super( EFMetHypoTrackAndJetsXE, self ).__init__( name )

        self.SumETCut=ef_thr
        self.MissingETCut=ef_thr
        self.CutType=-2.0
        self.METLabel='TrigEFMissingET_trkmht'
        self.doMuonCorrection = False
        self.SumETCut = 100000000*GeV
        self.forceAccept=False
        self.onlineMonitoring(False)
        self.doEMScaleTC=False
        if 'noFW' in name:
            self.doOnlyCalcCentralMET = True
        if 'wMu' in name:
            self.doMuonCorrection = True
        if 'noEF' in name:
            self.MissingETCut=-100*GeV
            self.SumETCut=-100*GeV
        if 'xe30' in name:
            self.onlineMonitoring(True)

class EFMetHypoFTKTrackAndJetsXE (EFMissingETHypoBase):
    __slots__ = []
    def __init__(self, name = "EFMetHypo_trkmhtxe1000",ef_thr=1000*GeV):
        super( EFMetHypoFTKTrackAndJetsXE, self ).__init__( name )

        self.SumETCut=ef_thr
        self.MissingETCut=ef_thr
        self.CutType=-2.0
        self.METLabel='TrigEFMissingET_trkmhtFTK'
        self.doMuonCorrection = False
        self.SumETCut = 100000000*GeV
        self.forceAccept=False
        self.onlineMonitoring(False)
        self.doEMScaleTC=False
        if 'noFW' in name:
            self.doOnlyCalcCentralMET = True
        if 'wMu' in name:
            self.doMuonCorrection = True
        if 'noEF' in name:
            self.MissingETCut=-100*GeV
            self.SumETCut=-100*GeV
        if 'xe30' in name:
            self.onlineMonitoring(True)

class EFMetHypoTrackAndClustersXE (EFMissingETHypoBase):
    __slots__ = []
    def __init__(self, name = "EFMetHypo_trktcxe1000",ef_thr=1000*GeV):
        super( EFMetHypoTrackAndClustersXE, self ).__init__( name )

        self.SumETCut=ef_thr
        self.MissingETCut=ef_thr
        self.CutType=-2.0
        self.METLabel='TrigEFMissingET_trktc'
        self.doMuonCorrection = False
        self.SumETCut = 100000000*GeV
        self.forceAccept=False
        self.onlineMonitoring(False)
        self.doEMScaleTC=False
        if 'noFW' in name:
            self.doOnlyCalcCentralMET = True
        if 'tclcw' in name:
            self.doEMScaleTC = False
        if 'wMu' in name:
            self.doMuonCorrection = True
        if 'noEF' in name:
            self.MissingETCut=-100*GeV
            self.SumETCut=-100*GeV
        if 'xe30' in name:
            self.onlineMonitoring(True)


class EFMetHypoFTKTrackAndClustersXE (EFMissingETHypoBase):
    __slots__ = []
    def __init__(self, name = "EFMetHypo_trktcxe1000",ef_thr=1000*GeV):
        super( EFMetHypoFTKTrackAndClustersXE, self ).__init__( name )

        self.SumETCut=ef_thr
        self.MissingETCut=ef_thr
        self.CutType=-2.0
        self.METLabel='TrigEFMissingET_trktcFTK'
        self.doMuonCorrection = False
        self.SumETCut = 100000000*GeV
        self.forceAccept=False
        self.onlineMonitoring(False)
        self.doEMScaleTC=False
        if 'noFW' in name:
            self.doOnlyCalcCentralMET = True
        if 'tclcw' in name:
            self.doEMScaleTC = False
        if 'wMu' in name:
            self.doMuonCorrection = True
        if 'noEF' in name:
            self.MissingETCut=-100*GeV
            self.SumETCut=-100*GeV
        if 'xe30' in name:
            self.onlineMonitoring(True)


class EFMetHypoTCPUCXE (EFMissingETHypoBase):
    __slots__ = []
    def __init__(self, name = "EFMetHypo_tcpucxe1000",ef_thr=1000*GeV,labelMET = ""):
        super( EFMetHypoTCPUCXE, self ).__init__( name )

        self.SumETCut=ef_thr
        self.MissingETCut=ef_thr
        self.CutType=-2.0
        self.METLabel='TrigEFMissingET_topocl_PUC' if 'LAr' not in labelMET else 'TrigEFMissingET_EFMissingET_Fex_topoClustersPUC'+labelMET
        self.doMuonCorrection = False
        self.SumETCut = 100000000*GeV
        self.forceAccept=False
        self.onlineMonitoring(False)
        self.doEMScaleTC=True
        self.doLArH11off=False
        self.doLArH12off=False
        self.doMETphicut=False
        if 'LArH11off' in name:
            self.doLArH11off = True
        if 'LArH12off' in name:
            self.doLArH12off = True
        if 'METphi' in name:
            self.doMETphicut = True
        if 'tchad' in name:
            self.doEMScaleTC = False
        if 'tclcw' in name:
            self.doEMScaleTC = False
        if 'tcem' in name:
            self.doEMScaleTC = True
        if 'wMu' in name:
            self.doMuonCorrection = True
        if 'noEF' in name:
            self.MissingETCut=-100*GeV
            self.SumETCut=-100*GeV
        if 'xe30' in name:
            self.onlineMonitoring(True)

class EFMetHypoTCTrkPUCXE (EFMissingETHypoBase):
    __slots__ = []
    def __init__(self, name = "EFMetHypo_tctrkpucxe1000",ef_thr=1000*GeV, extraCalib = ""):
        super( EFMetHypoTCTrkPUCXE, self ).__init__( name )

        self.SumETCut=ef_thr
        self.MissingETCut=ef_thr
        self.CutType=-2.0
        self.METLabel='TrigEFMissingET_topocltrk_PUC{0}'.format(extraCalib)
        self.doMuonCorrection = False
        self.SumETCut = 100000000*GeV
        self.forceAccept=False
        self.onlineMonitoring(False)
        self.doEMScaleTC=True
        if 'tchad' in name:
            self.doEMScaleTC = False
        if 'tc' in name and 'lcw' in name:
            self.doEMScaleTC = False
        if 'tc' in name and 'em' in name:
            self.doEMScaleTC = True
        if 'wMu' in name:
            self.doMuonCorrection = True
        if 'noEF' in name:
            self.MissingETCut=-100*GeV
            self.SumETCut=-100*GeV
        if 'xe30' in name:
            self.onlineMonitoring(True)


class L2MetHypoTE (EFMissingETHypoBase):
    __slots__ = []
    def __init__(self, name = "L2MetHypo_te1000",l2_thr=1000*GeV):
        super( L2MetHypoTE, self ).__init__( name )

        self.SumETCut=l2_thr
        self.MissingETCut=100000000*GeV
        self.CutType=-2.0
        self.METLabel='T2MissingET'
        self.doMuonCorrection = False
        self.forceAccept=False
        self.onlineMonitoring(False)

class L2MetHypoFEBTE (EFMissingETHypoBase):
    __slots__ = []
    def __init__(self, name = "L2MetHypo_FEB_te1000",l2_thr=1000*GeV):
        super( L2MetHypoFEBTE, self ).__init__( name )

        self.SumETCut=l2_thr
        self.MissingETCut=100000000*GeV
        self.CutType=-2.0
        self.METLabel='L2MissingET_FEB'
        self.doMuonCorrection = False
        self.forceAccept=False
        self.onlineMonitoring(False)

class L2MetHypoFEBTE_L1check (EFMissingETHypoBase):
    __slots__ = []
    def __init__(self, name = "L2MetHypo_FEB_te1000",l2_thr=1000*GeV):
        super( L2MetHypoFEBTE_L1check, self ).__init__( name )

        self.SumETCut=l2_thr
        self.MissingETCut=100000000*GeV
        self.CutType=-2.0
        self.METLabel='L2MissingET_FEB'
        self.doMuonCorrection = False
        self.forceAccept=False
        self.onlineMonitoring(False)
        self.doL1L2FEBTest=True
        self.L1L2FEBTolerance=80.0*GeV

class EFMetHypoTE (EFMissingETHypoBase):
    __slots__ = []
    def __init__(self, name = "EFMetHypo_te1000",ef_thr=1000*GeV):
        super( EFMetHypoTE, self ).__init__( name )

        self.SumETCut=ef_thr
        self.MissingETCut=100000000*GeV
        self.CutType=-2.0
        self.METLabel='TrigEFMissingET'
        self.doMuonCorrection = False
        self.forceAccept=False
        self.onlineMonitoring(False)

class L2MetHypoXS_2012 (EFMissingETHypoBase):
    __slots__ = []
    def __init__(self, name = "L2MetHypo_xs100",l2_thr=10):
        super( L2MetHypoXS_2012, self ).__init__( name )

        self.SigCut=l2_thr
        self.CutType=1
        self.METLabel='T2MissingET'
        self.doMuonCorrection = False
        self.forceAccept=False
        self.significanceOffset = -1.886 # Period A: -1.9385
        self.significanceSlope  = 1.15   # Period A: 1.1305
        self.xsMETok = 80000
        if 'xs30' in name:
            self.onlineMonitoring(True)

class L2MetHypoXS_2011 (EFMissingETHypoBase):
    __slots__ = []
    def __init__(self, name = "L2MetHypo_xs100",l2_thr=10):
        super( L2MetHypoXS_2011, self ).__init__( name )

        self.SigCut=l2_thr
        self.CutType=1
        self.METLabel='T2MissingET'
        self.doMuonCorrection = False
        self.forceAccept=False
        self.significanceOffset = -1.4336
        self.significanceSlope  = 1.12
        self.xsMETok = 80000
        if 'xs30' in name:
            self.onlineMonitoring(True)

class L2MetHypoFEBXS (EFMissingETHypoBase):
    __slots__ = []
    def __init__(self, name = "L2MetHypo_FEB_xs100",l2_thr=10):
        super( L2MetHypoFEBXS, self ).__init__( name )

        self.SigCut=l2_thr
        self.CutType=1
        self.METLabel='L2MissingET_FEB'
        self.doMuonCorrection = False
        self.forceAccept=False
        self.significanceOffset = -0.898
        self.significanceSlope  = 0.57315
        self.xsMETok = 80000
        if 'xs30' in name:
            self.onlineMonitoring(True)

class L2MetHypoFEBXS_L1check (EFMissingETHypoBase):
    __slots__ = []
    def __init__(self, name = "L2MetHypo_FEB_xs100",l2_thr=10):
        super( L2MetHypoFEBXS_L1check, self ).__init__( name )

        self.SigCut=l2_thr
        self.CutType=1
        self.METLabel='L2MissingET_FEB'
        self.doMuonCorrection = False
        self.forceAccept=False
        self.significanceOffset = -0.898
        self.significanceSlope  = 0.57315
        self.xsMETok = 80000
        self.doL1L2FEBTest=True
        self.L1L2FEBTolerance=80.0*GeV
        if 'xs30' in name:
            self.onlineMonitoring(True)

class EFMetHypoXS_2sided (EFMissingETHypoBase):
    __slots__ = []
    def __init__(self, name = "EFMetHypo_xs100",ef_thr=10):
        super( EFMetHypoXS_2sided, self ).__init__( name )

        self.SigCut=ef_thr
        self.CutType=1
        self.METLabel='TrigEFMissingET'
        self.doMuonCorrection = False
        self.forceAccept=False
        self.significanceOffset = 4.93182
        self.significanceSlope = 0.442864
        self.xsMETok = 150000
        if 'xs30' in name:
            self.onlineMonitoring(True)


class EFMetHypoXS_2sided_2012 (EFMissingETHypoBase):
    __slots__ = []
    def __init__(self, name = "EFMetHypo_xs100",ef_thr=10):
        super( EFMetHypoXS_2sided, self ).__init__( name )

        self.SigCut=ef_thr
        self.CutType=1
        self.METLabel='TrigEFMissingET'
        self.doMuonCorrection = False
        self.forceAccept=False
        self.significanceOffset = 4.265
        self.significanceSlope  = 0.2966
        self.xsMETok = 95000
        if 'xs30' in name:
            self.onlineMonitoring(True)

class EFMetHypoXS_1sided (EFMissingETHypoBase):
    __slots__ = []
    def __init__(self, name = "EFMetHypo_xs100",ef_thr=10):
        super( EFMetHypoXS_1sided, self ).__init__( name )

        self.SigCut=ef_thr
        self.CutType=1
        self.METLabel='TrigEFMissingET'
        self.doMuonCorrection = False
        self.forceAccept=False
        self.significanceOffset = -0.23
        self.significanceSlope  = 0.46
        self.xsMETok = 95000
        if 'xs30' in name:
            self.onlineMonitoring(True)

class EFMetHypoTCXS (EFMissingETHypoBase):
    __slots__ = []
    def __init__(self, name = "EFMetHypo_tcxs100",ef_thr=10):
        super( EFMetHypoTCXS, self ).__init__( name )

        self.SigCut=ef_thr
        self.CutType=1
        self.METLabel='TrigEFMissingET_topocl'
        self.doMuonCorrection = False
        self.forceAccept=False
        self.onlineMonitoring(False)
        self.significanceOffset = 1.0915
        self.significanceSlope  = 0.837
        self.xsMETok = 95000
        self.doEMScaleTC=True
        if 'tchad' in name:
            self.doEMScaleTC = False
        if 'tclcw' in name:
            self.doEMScaleTC = False
        if 'tcem' in name:
            self.doEMScaleTC = True
            self.significanceOffset = 1.7035
            self.significanceSlope = 0.60855
        if 'wMu' in name:
            self.doMuonCorrection = True
        if 'noEF' in name:
            self.MissingETCut=-100*GeV
            self.SumETCut=-100*GeV
        if 'xs30' in name:
            self.onlineMonitoring(True)

class EFMetHypoTCPSXS (EFMissingETHypoBase):
    __slots__ = []
    def __init__(self, name = "EFMetHypo_tcpsxs100",ef_thr=10):
        super( EFMetHypoTCPSXS, self ).__init__( name )

        self.SigCut=ef_thr
        self.CutType=1
        self.METLabel='TrigEFMissingET_topocl_PS'
        self.doMuonCorrection = False
        self.forceAccept=False
        self.onlineMonitoring(False)
        self.significanceOffset = 1.0915
        self.significanceSlope  = 0.837
        self.xsMETok = 95000
        self.doEMScaleTC=True
        if 'tchad' in name:
            self.doEMScaleTC = False
        if 'tclcw' in name:
            self.doEMScaleTC = False
        if 'tcem' in name:
            self.doEMScaleTC = True
            self.significanceOffset = 1.7035
            self.significanceSlope = 0.60855
        if 'wMu' in name:
            self.doMuonCorrection = True
        if 'noEF' in name:
            self.MissingETCut=-100*GeV
            self.SumETCut=-100*GeV
        if 'xs30' in name:
            self.onlineMonitoring(True)




