from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning, DerivationFramework__MuonTrackParticleThinning, DerivationFramework__EgammaTrackParticleThinning, DerivationFramework__TauTrackParticleThinning
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from AthenaCommon.SystemOfUnits import GeV
from AthenaCommon.AppMgr import ToolSvc
from DerivationFrameworkJetEtMiss.PFlowCommon import applyPFOAugmentation

class METTriggerDerivationContentManager(object):
    """ Helper class to assemble MET trigger derivation content """
    def __init__(
            self, stream_name, stream, common_prefix="DFMETTrig",
            jet_algorithms = ["AntiKt4EMTopo", "AntiKt4EMPFlow"],
            track_threshold = 10*GeV, track_sel=None, all_vars=None):
        """ Create the manager

        Parameters
        ------------
        stream_name: The name of the stream (e.g JETM10)
        stream: The stream object returned by MSMgr.NewPoolRootStream
        common_prefix: The prefix used by shared tools and decorations to prevent duplication
        jet_algorithms: The different jet algorithms (+ associated MET containers) to write
        track_threshold: Write out all tracks with pT above this threshold (in MeV)
        track_sel: The track selection string
        all_vars: Extra containers to add to 'AllVariables'
        """
        self.stream_name = stream_name
        self._common_prefix = common_prefix
        self.track_threshold = track_threshold
        track_sel = track_sel
        self.track_sel = track_sel
        self.all_vars = [] if all_vars is None else all_vars
        # Create the tools
        if any(x in self.all_vars for x in ("JetETMissNeutralParticleFlowObjects", "JetETMissChargedParticleFlowObjects") ):
            applyPFOAugmentation()
        self._make_slimming_tools(stream, jet_algorithms)
        self._make_augmentation_tools(stream)
        self._make_thinning_tools(stream)

    @classmethod
    def make_loose_manager(cls, stream_name, stream):
        return cls(
                stream_name, stream, track_threshold=1*GeV, track_sel="Loose",
                all_vars = [
                    "HLT_xAOD__MuonContainer_MuonEFInfo",
                    "CaloCalTopoClusters",
                    "JetETMissChargedParticleFlowObjects",
                    "JetETMissNeutralParticleFlowObjects",
                    "Kt4EMPFlowEventShape"])

    @classmethod
    def make_tight_manager(cls, stream_name, stream):
        return cls(stream_name, stream, track_threshold=10*GeV)

    @property
    def common_prefix(self):
        return self._common_prefix

    def make_kernel(self, *skimming_tools):
        return CfgMgr.DerivationFramework__DerivationKernel(
                self.stream_name + "Kernel",
                AugmentationTools = self.augmentation_tools,
                SkimmingTools = list(skimming_tools),
                ThinningTools = self.thinning_tools)


    def _mk_common_tool(self, cls, name, **kwargs):
        """ Create a common tool, if it isn't already in the ToolSvc """
        global ToolSvc
        # Resolve the full name
        if not name.startswith(self.common_prefix):
            name = self.common_prefix + name
        try:
            return getattr(ToolSvc, name)
        except AttributeError:
            ToolSvc += cls(name, **kwargs)
            return getattr(ToolSvc, name)

    def _make_slimming_tools(self, stream, jet_algorithms):
        self.slimming_helper = SlimmingHelper(self.stream_name + "SlimmingHelper")
        smart_collections = ["Electrons", "Muons", "Photons", "TauJets", "PrimaryVertices", "InDetTrackParticles"]
        smart_collections += ["{0}Jets".format(a) for a in jet_algorithms]
        smart_collections += ["MET_Reference_{0}".format(a) for a in jet_algorithms]
        if "AntiKt4EMTopo" in jet_algorithms:
            smart_collections += [
                    "AntiKt4EMTopoJets_BTagging201810", "BTagging_AntiKt4EMTopo_201810"]
        if "AntiKt4EMPFlow" in jet_algorithms:
            smart_collections += [
                    "AntiKt4EMPFlowJets_BTagging201810", "BTagging_AntiKt4EMPFlow_201810",
                    "AntiKt4EMPFlowJets_BTagging201903", "BTagging_AntiKt4EMPFlow_201903"]
        self.slimming_helper.SmartCollections = smart_collections
        self.slimming_helper.ExtraVariables = [
                "{0}Jets.Timing".format(a) for a in jet_algorithms]
        self.slimming_helper.AllVariables = [
                "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET",
                "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_mht",
                "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl_PS",
                "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl_PUC",
                "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl",
                "LVL1EnergySumRoI",
                "LVL1JetRoIs",
                "LVL1JetEtRoI"] \
                        + ["MET_Core_{0}".format(a) for a in jet_algorithms] \
                        + ["METAssoc_{0}".format(a) for a in jet_algorithms] \
                        + self.all_vars

    def _make_augmentation_tools(self, stream):
        """ Create any common augmentation tools """
        self.augmentation_tools = []
        if self.track_sel is not None:
            tool = self._mk_common_tool(
                    CfgMgr.DerivationFramework__InDetTrackSelectionToolWrapper,
                    "{0.track_sel}TrackSelWrapper".format(self),
                    ContainerName = "InDetTrackParticles",
                    DecorationName = self.common_prefix + self.track_sel)
            tool.TrackSelectionTool.CutLevel = self.track_sel
            self.augmentation_tools.append(tool)
            self.slimming_helper.ExtraVariables.append(
                    "InDetTrackParticles." + tool.DecorationName)
        tva_tool = self._mk_common_tool(
                CfgMgr.DerivationFramework__TVAAugmentationTool,
                "NominalTVAAugmentationTool",
                LinkName = self.common_prefix+"NominalTVA",
                TrackName = "InDetTrackParticles",
                TVATool = self._mk_common_tool(
                    CfgMgr.CP__TrackVertexAssociationTool,
                    "NominalTVATool",
                    WorkingPoint="Nominal") )
        self.augmentation_tools.append(tva_tool)
        self.slimming_helper.ExtraVariables.append(
            "{0.TrackName}.{0.LinkName}".format(tva_tool) )

    def _make_thinning_tools(self, stream):
        stream_name = self.stream_name
        self.thinning_helper = ThinningHelper(stream_name+"ThinningHelper")
        self.thinning_helper.AppendToStream(stream)
        self.thinning_tools = []
        tool = DerivationFramework__TrackParticleThinning(
                stream_name + "TPThinningTool",
                StreamName = stream_name, 
                SelectionString = "InDetTrackParticles.pt > {0}*GeV".format(
                    self.track_threshold) )
        if self.track_sel is not None:
            tool.SelectionString += " || InDetTrackParticles.{0.common_prefix}{0.track_sel}".format(self)
        self.thinning_tools.append(tool)
        self.thinning_tools += [
                DerivationFramework__MuonTrackParticleThinning(
                    stream_name + "MuonTPThinningTool",
                    StreamName             = stream_name,
                    MuonKey                = "Muons",
                    InDetTrackParticlesKey = "InDetTrackParticles"),
                DerivationFramework__EgammaTrackParticleThinning(
                    stream_name + "ElectronTPThinningTool",
                    StreamName             = stream_name,
                    SGKey                  = "Electrons",
                    InDetTrackParticlesKey = "InDetTrackParticles"),
                DerivationFramework__EgammaTrackParticleThinning(
                    stream_name + "PhotonTPThinningTool",
                    StreamName             = stream_name,
                    SGKey                  = "Photons",
                    InDetTrackParticlesKey = "InDetTrackParticles"),
                DerivationFramework__TauTrackParticleThinning(
                    stream_name + "TauTPThinningTool",
                    StreamName             = stream_name,
                    TauKey                 = "TauJets",
                    InDetTrackParticlesKey = "InDetTrackParticles")
                ]
        global ToolSvc
        ToolSvc += self.thinning_tools
