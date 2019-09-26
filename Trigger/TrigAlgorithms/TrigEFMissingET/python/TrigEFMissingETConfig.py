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
default_do_FTK = "FTK" in METChainParts_Default["addInfo"]

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
    def __new__(cls, name = Configurable.DefaultName, do_FTK=False):
        if name == Configurable.DefaultName:
            name = "EFMissingET_Fex_{0}TrackAndJets".format("FTK" if do_FTK else "")
        return super(TrkMHTFex, cls).__new__(cls, name, do_FTK)

    def __init__(self, name, do_FTK):
        key_suffix = "FTK" if do_FTK else ""
        super(TrkMHTFex, self).__init__(name)
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
        return ["jets", "fs_tracks", "muons"]

class TrkTCFex(HLT__MET__TrkTCFex):
    __slots__ = []
    def __new__(cls, name = Configurable.DefaultName, do_FTK=False):
        if name == Configurable.DefaultName:
            name = "EFMissingET_Fex_{0}TrackAndClusters".format("FTK" if do_FTK else "")
        return super(TrkTCFex, cls).__new__(cls, name, do_FTK)

    def __init__(self, name, do_FTK):
        key_suffix = "FTK" if do_FTK else ""
        super(TrkTCFex, self).__init__(name)

        self.MissingETOutputKey = "TrigEFMissingET_trktc{0}".format(key_suffix)

        self.CVFTool.TrackClusterLinkName = "MatchedExtrapolatedTrackLinks"
        # These are set up as private tools so the names don't matter.
        self.ClusterModTools += [
                VoronoiWeightTool("VoronoiWeightTool", doSpread=True),
                SoftKillerWeightTool("SKWeightTool")
        ]

        for target in ["Validation", "Online", "Cosmic"]:
            add_monitor(self, target, ["standard"])

    def request_inputs(self):
        return ["clusters", "fs_tracks", "muons"]

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
        do_FTK = default_do_FTK):
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
    elif reco_alg == "tc":
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
        return TrkMHTFex(do_FTK = do_FTK)
    elif reco_alg == "trktc":
        return TrkTCFex(do_FTK = do_FTK)
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
            chainDict["EFrecoAlg"],
            chainDict["calib"],
            chainDict["jetCalib"],
            "FTK" in chainDict["addInfo"])
