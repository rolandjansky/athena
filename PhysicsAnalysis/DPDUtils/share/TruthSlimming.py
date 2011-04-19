##########################
# Truth Slimming
########################


from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from AthenaCommon.AppMgr import ToolSvc
from McParticleAlgs.McAodFlags import jobproperties as jp
jp.McAodFlags.doTruthEtIsolations = True

# Deal with truth:
#
from McParticleAlgs.JobOptCfg import McAodBuilder
# from McParticleTools.McParticleToolsConf import NoopFilterTool
from McParticleTools.McParticleToolsConf import OldSpclMcFilterTool
from McParticleTools.McParticleToolsConf import TruthParticleCnvTool
topSequence += McAodBuilder(
    "McAodBuilder",
    OutputLevel = INFO,
    FilterTool = OldSpclMcFilterTool(
                   McEvents = "GEN_AOD",
                   McEventsOutput = "GEN_DPD",
                   DoEtIsolations = jp.McAodFlags.doTruthEtIsolations(),
                   IncludeSimul=False,
                   IncludePartonShowers=False,
                   ptGammaMin = 5000.
                   ),
    CnvTool = TruthParticleCnvTool(
                   McEvents = "GEN_DPD",
                   TruthParticlesOutput = "SpclMC",
                   DoEtIsolations = jp.McAodFlags.doTruthEtIsolations()
                   )
    )
ToolSvc.TruthIsolationTool.McEventsOutput="GEN_DPD"

