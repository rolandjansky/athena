# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigEFMissingET.TrigEFMissingETConf import (
        HLT__MET__TCFex, HLT__MET__MHTFex, HLT__MET__TrkMHTFex,
        HLT__MET__TrkTCFex, HLT__MET__CellFex, HLT__MET__TCPufitFex,
        HLT__MET__TCPufitTrkFex, HLT__MET__CellPufitFex,
        HLT__MET__TCPuetaFex, HLT__MET__MuonFex)
from TrigEFMissingET.TrigEFMissingETMonitoring import add_monitor

from JetRecTools.JetRecToolsConf import (
        SoftKillerWeightTool, VoronoiWeightTool)

from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
theCaloNoiseTool=CaloNoiseToolDefault()
from AthenaCommon.AppMgr import ToolSvc
ToolSvc+=theCaloNoiseTool

from AthenaCommon.Constants import VERBOSE,DEBUG,INFO
from AthenaCommon.SystemOfUnits import GeV
from AthenaCommon.SystemOfUnits import nanosecond
from AthenaCommon.Configurable import Configurable

from TriggerMenu.menu.SignatureDicts import METChainParts_Default
default_cluster_calib = METChainParts_Default["calib"]
default_jet_calib = METChainParts_Default["jetCalib"]
default_do_TST = "tst" in METChainParts_Default["addInfo"]
default_do_FTK = "FTK" in METChainParts_Default["addInfo"]
default_tst_ceiling = "tstceil" in METChainParts_Default["addInfo"]
default_do_CVF = "cvf" in METChainParts_Default["clusmod"]

from TriggerJobOpts.TriggerFlags import TriggerFlags
import logging
log = logging.getLogger(__name__)

# A note on default names.
# There are few cases here where we want to delegate the responsibility of each
# FEX config to set the default name (e.g. if it depends on some calibration
# level). This is a little bit tricky with the way that athena assigns
# configurables. Two configurables with the same name are the *same* thing so
# the __new__ function will just return the same instance (without calling
# __init__ on it) if it sees the same name. This means that we need to override
# __new__ in these cases.
# Otherwise the default name is worked out from the default in the __init__
# call. If this is not set then it uses the getType function. This doesn't play
# nicely with the monitoring classes that try to use their parent's name as a
# folder name (ROOT folders don't like to have colons in them).

class EFMissingET_Fex_2sidednoiseSupp(HLT__MET__CellFex):
    __slots__ = []
    components = ["PreSamplerB", "EMB1", "EMB2", "EMB3",    # LAr barrel
                  "PreSamplerE", "EME1", "EME2", "EME3",    # LAr EM endcap 
                  "HEC0",        "HEC1", "HEC2", "HEC3",    # Hadronic end cap cal.
                  "TileBar0",    "TileBar1", "TileBar2",    # Tile barrel
                  "TileGap1",    "TileGap2", "TileGap3",    # Tile gap (ITC & scint)
                  "TileExt0",    "TileExt1", "TileExt2",    # Tile extended barrel
                  "FCAL0",       "FCAL1",    "FCAL2",       # Forward cal endcap
                 ]
    def __init__(self, name = "EFMissingET_Fex_2sidednoiseSupp"):
        super(EFMissingET_Fex_2sidednoiseSupp, self).__init__(name)
        self.MissingETOutputKey = "TrigEFMissingET"
        self.CaloNoiseTool = theCaloNoiseTool
        for target in ["Validation", "Online", "Cosmic"]:
            add_monitor(self, target, ["standard"], self.components)

    def request_inputs(self):
        return []

class EFMissingET_Fex_2sidednoiseSuppPUC(HLT__MET__CellPufitFex):
    __slots__ = []
    def __init__(self, name = "EFMissingET_Fex_2sidednoiseSuppPUC"):
        super(EFMissingET_Fex_2sidednoiseSuppPUC, self).__init__(name)

        self.MissingETOutputKey = "TrigEFMissingET_cell_PUC"
        self.CaloNoiseTool = theCaloNoiseTool

        for target in ["Validation", "Online", "Cosmic"]:
            add_monitor(self, target, ["standard"])

    def request_inputs(self):
        return []


class EFMissingET_Fex_topoClusters(HLT__MET__TCFex):
    __slots__ = []
    def __new__(cls, name = Configurable.DefaultName, cluster_calib="lcw"):
        if name == Configurable.DefaultName:
            name = "EFMissingET_Fex_topoClusters"
            if cluster_calib != "lcw":
                name += "_" + cluster_calib
        return super(EFMissingET_Fex_topoClusters, cls).__new__(cls, name, cluster_calib)


    def __init__(self, name, cluster_calib):
        super(EFMissingET_Fex_topoClusters, self).__init__(name)

        self.MissingETOutputKey = "TrigEFMissingET_topocl"
        if cluster_calib == "lcw":
            self.UseUncalibrated = False
        elif cluster_calib == "em":
            self.UseUncalibrated = True
            self.MissingETOutputKey += "_em"
        else:
            log.error("Unexpected cluster calibration {0}".format(cluster_calib) )


        for target in ["Validation", "Online", "Cosmic"]:
            add_monitor(self, target, ["standard"])

    def request_inputs(self):
        return ["clusters"]

class EFMissingET_Fex_topoClustersPS(HLT__MET__TCPuetaFex):
    __slots__ = []
    def __init__(self, name = "EFMissingET_Fex_topoClustersPS"):
        super(EFMissingET_Fex_topoClustersPS, self).__init__(name)

        self.MissingETOutputKey = "TrigEFMissingET_topocl_PS"
        for target in ["Validation", "Online", "Cosmic"]:
            add_monitor(self, target, ["standard"])

    def request_inputs(self):
        return ["clusters"]

class EFMissingET_Fex_topoClustersTracksPUC(HLT__MET__TCPufitTrkFex):
    __slots__ = []
    def __new__(cls, name=Configurable.DefaultName, key_suffix=""):
        if name == Configurable.DefaultName:
            name = "EFMissingET_Fex_topoClustersTracksPUC" + key_suffix
        return super(EFMissingET_Fex_topoClustersTracksPUC, cls).__new__(cls, name, key_suffix)

    def __init__(self, name, key_suffix):
        super(EFMissingET_Fex_topoClustersTracksPUC, self).__init__(name)
        # name of TrigMissingET object
        self.MissingETOutputKey = "TrigEFMissingET_topocltrk_PUC{0}".format(key_suffix)

        # TODO I'm pretty sure that we should be setting some properties on the
        # track selection tool here...

        for target in ["Validation", "Online", "Cosmic"]:
            add_monitor(self, target, ["standard"])

    def request_inputs(self):
        return ["clusters", "jets", "roi_tracks"]


##### Use topo. clusters for noise suppression #####
class EFMissingET_Fex_topoClustersPUC(HLT__MET__TCPufitFex):
    __slots__ = []
    def __init__(self, name = "EFMissingET_Fex_topoClustersPUC"):
        super(EFMissingET_Fex_topoClustersPUC, self).__init__(name)

        self.MissingETOutputKey = "TrigEFMissingET_topocl_PUC"
        for target in ["Validation", "Online", "Cosmic"]:
            add_monitor(self, target, ["standard"])

    def request_inputs(self):
        return ["clusters"]


##### loop over jets #####
class EFMissingET_Fex_Jets (HLT__MET__MHTFex):
    __slots__ = []
    def __new__(cls, name = Configurable.DefaultName, key_suffix=""):
        if name == Configurable.DefaultName:
            name = "EFMissingET_Fex_Jets"+key_suffix
        return super(EFMissingET_Fex_Jets, cls).__new__(cls, name, key_suffix)
    def __init__ (self, name, key_suffix):
        super(EFMissingET_Fex_Jets, self).__init__(name)

        # name of TrigMissingET object
        self.MissingETOutputKey = "TrigEFMissingET_mht{0}".format(key_suffix)
                
        ## Configuration of jet fex
        self.EtaCut = 2.2
        self.CentralPtCut = 0.0
        self.ForwardPtCut = 0.0
        self.ApplyTG3Correction = TriggerFlags.run2Config() != '2016' # Do not apply the TileGap3 correction for 2015+2016 data
        
        for target in ["Validation", "Online", "Cosmic"]:
            add_monitor(self, target, ["standard"])

    def request_inputs(self):
        return ["jets"]
        

class TrkMHTFex(HLT__MET__TrkMHTFex):
    __slots__ = []
    def __new__(cls, name = Configurable.DefaultName, do_FTK=False, tst_ceiling=False):
        if name == Configurable.DefaultName:
            name = "EFMissingET_Fex_{0}TrackAndJets{1}".format(
                    "FTK" if do_FTK else "",
                    "_tstceil" if tst_ceiling else "")
        return super(TrkMHTFex, cls).__new__(cls, name, do_FTK, tst_ceiling)

    def __init__(self, name, do_FTK, tst_ceiling):
        super(TrkMHTFex, self).__init__(name)
        key_suffix = "FTK" if do_FTK else ""
        if tst_ceiling:
            self.TrackSoftTermPtCeiling = 20 * GeV
            key_suffix += "_tstceil"
            self.DoMuonOR = False
        else:
            self.DoMuonOR = True
        self.MissingETOutputKey = "TrigEFMissingET_trkmht{0}".format(key_suffix)


        # Configuration of Fex
        self.EtaCut = 2.2
        self.CentralPtCut = 25 * GeV
        self.ForwardPtCut = 0
        self.TrackPtCut = 1 * GeV
        self.JVTCut = 0.9
        self.MinJetJVTPt = 20 * GeV
        self.MaxJetJVTPt = 50 * GeV

        self.TrackSelectionTool.CutLevel = "Loose"
        self.TrackSelectionTool.maxZ0SinTheta = 1.5
        self.TrackSelectionTool.maxD0overSigmaD0 = 3

        for target in ["Validation", "Online", "Cosmic"]:
            add_monitor(self, target, ["standard"])

    def request_inputs(self):
        inputs = ["jets", "fs_tracks"]
        if self.DoMuonOR:
            inputs.append("muons")
        return inputs

class TrkTCFex(HLT__MET__TrkTCFex):
    __slots__ = []
    def __new__(cls, name = Configurable.DefaultName, do_FTK=False, do_VS = True, do_SK = True, do_CVF = True, do_TST = True, tst_ceiling=False):
        if name == Configurable.DefaultName:
            #Keep the default (VSSK + CVF + TST) name as trktc only
            #VSSK does not use tracks, keep name accordingly:
            if not do_CVF and not do_TST:
                name = "EFMissingET_Fex_{0}{1}topoClusters".format(
                    "VS" if do_VS else "",
                    "SK" if do_SK else "")
            elif not (do_VS and do_SK and do_CVF and do_TST):
                name = "EFMissingET_Fex_{0}{1}{2}{3}{4}TrackAndClusters{5}".format(
                    "FTK" if do_FTK else "",
                    "VS" if do_VS else "",
                    "SK" if do_SK else "",
                    "CVF" if do_CVF else "",
                    "TST" if do_TST else "",
                    "_tstceil" if (do_TST and tst_ceiling) else "")
            else:
                name = "EFMissingET_Fex_{0}TrackAndClusters{1}".format(
                    "FTK" if do_FTK else "",
                    "_tstceil" if tst_ceiling else "")
        return super(TrkTCFex, cls).__new__(cls, name, do_FTK, do_VS, do_SK, do_CVF, do_TST, tst_ceiling)

    def __init__(self, name, do_FTK, do_VS, do_SK, do_CVF, do_TST, tst_ceiling):
        super(TrkTCFex, self).__init__(name)
        key_suffix = "FTK" if do_FTK else ""
        if not (do_VS and do_SK and do_CVF and do_TST):
        #    key_suffix += "VS" if do_VS else ""
        #    key_suffix += "SK" if do_SK else ""
            key_suffix += "_cvf" if do_CVF else ""
            key_suffix += "_tst" if do_TST else ""
            key_suffix += "ceil" if do_TST and tst_ceiling else""

        #super(TrkTCFex, self).__init__(name)
        #key_suffix = "FTK" if do_FTK else ""
        if do_TST and tst_ceiling:
            if not "tstceil" in key_suffix:
                key_suffix += "_tstceil"
            self.TrackSoftTermPtCeiling = 20 * GeV
            self.DoMuonOR = False
        else:
            self.DoMuonOR = True


        #modify OutputKey according to algorithm name:
        if do_VS and do_SK and not do_TST:
            self.MissingETOutputKey = "TrigEFMissingET_sktc{0}".format(key_suffix)
        elif not do_VS and not do_SK and do_CVF:
            self.MissingETOutputKey = "TrigEFMissingET_tc{0}".format(key_suffix)
        elif do_VS and do_SK and do_CVF and do_TST:
            self.MissingETOutputKey = "TrigEFMissingET_trktc{0}".format(key_suffix)

        if do_CVF:
            self.CVFTool.TrackClusterLinkName = "MatchedExtrapolatedTrackLinks"
        # These are set up as private tools so the names don't matter.
        if do_VS:
            self.ClusterModTools += [ VoronoiWeightTool("VoronoiWeightTool", doSpread=True) ]
        if do_SK:
            self.ClusterModTools += [ SoftKillerWeightTool("SKWeightTool") ]
        #passing flags to Algorithm:
        self.doVS = do_VS
        self.doSK = do_SK
        self.doCVF = do_CVF
        self.doTST = do_TST

        for target in ["Validation", "Online", "Cosmic"]:
            add_monitor(self, target, ["standard"])

    def request_inputs(self):
        inputlist = ["clusters"]
        if self.doCVF or self.doTST:
            inputlist += ["fs_tracks"]
        if self.doTST and self.DoMuonOR:
            inputlist += ["muons"]
        return inputlist

class MuonFex(HLT__MET__MuonFex):
    __slots__ = []
    def __init__(self, name = "HLT_MET_MuonFex"):
        super(MuonFex, self).__init__(name)
        self.MissingETOutputKey = "TrigEFMissingET_muons"

        for target in ["Validation", "Online", "Cosmic"]:
            add_monitor(self, target, ["standard"])

    def request_inputs(self):
        return ["muons"]


def getFEX(
        reco_alg,
        cluster_calib = default_cluster_calib,
        jet_calib = default_jet_calib,
        do_FTK = default_do_FTK,
        tst_ceiling = default_tst_ceiling,
        do_CVF = default_do_CVF,
        do_TST = default_do_TST):
    """
        Get the correct FEX algorithm for this configuration
    """
    jet_calib_string = ""
    if cluster_calib != default_cluster_calib:
        jet_calib_string += "_{0}".format(cluster_calib)
    if jet_calib != default_jet_calib:
        jet_calib_string += "_{0}".format(jet_calib)
    if reco_alg == "cell":
        return EFMissingET_Fex_2sidednoiseSupp()
    elif reco_alg == "cellpufit":
        return EFMissingET_Fex_2sidednoiseSuppPUC()
    elif reco_alg == "tc" and not do_CVF:
        return EFMissingET_Fex_topoClusters(cluster_calib=cluster_calib)
    elif reco_alg == "pueta":
        return EFMissingET_Fex_topoClustersPS()
    elif reco_alg == "pufittrack":
        return EFMissingET_Fex_topoClustersTracksPUC(key_suffix=jet_calib_string)
    elif reco_alg == "pufit":
        return EFMissingET_Fex_topoClustersPUC()
    elif reco_alg == "mht":
        return EFMissingET_Fex_Jets(key_suffix=jet_calib_string)
    elif reco_alg == "trkmht":
        return TrkMHTFex(do_FTK = do_FTK, tst_ceiling=tst_ceiling)
    elif reco_alg == "trktc":
        return TrkTCFex(do_FTK = do_FTK, tst_ceiling=tst_ceiling)
    elif reco_alg == "sktc":
        return TrkTCFex(do_FTK = False, do_VS = True, do_SK = True, do_CVF = do_CVF, do_TST = False, tst_ceiling=False)
    elif reco_alg == "tc":
        return TrkTCFex(do_FTK = do_FTK, do_VS = False, do_SK = False, do_CVF = True, do_TST = do_TST, tst_ceiling=tst_ceiling)    
    else:
        log.error("Unknown reco algo {0} requested".format(reco_alg) )
        return None
        


def getFEXFromDict(chainDict):
    """
        Get the correct FEX algorithm for this configuration

        Other elements of the chain dict may be used to add additional
        configuration information.
    """
    return getFEX(
            reco_alg = chainDict["EFrecoAlg"],
            cluster_calib = chainDict["calib"],
            jet_calib = chainDict["jetCalib"],
            do_FTK = "FTK" in chainDict["addInfo"],
            tst_ceiling = 'tstceil' in chainDict["addInfo"],
            do_CVF = 'cvf' in chainDict["clusmod"],
            do_TST = 'tst' in chainDict["addInfo"])
