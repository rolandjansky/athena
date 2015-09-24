##=============================================================================
## Name: SlimTruth.py   
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
## 
## Based on: DPDUtils
##
## Description: Remove information from the Monte Carlo Truth.
##              Remove: PartonShowers, low pt photons,
##
##=============================================================================

# Import the logging module
from AthenaCommon.Logging import logging
primaryDPD_SlimTruth_msg = logging.getLogger( 'TruthSlimming' )

primaryDPD_SlimTruth_msg.info( 'Doing TruthSlimming...' )

# Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon import CfgMgr
from McParticleAlgs.McAodFlags import jobproperties as jp
jp.McAodFlags.doTruthEtIsolations = True

# Deal with truth:
#
from McParticleAlgs.JobOptCfg import McAodBuilder
# from McParticleTools.McParticleToolsConf import NoopFilterTool
from McParticleTools.McParticleToolsConf import OldSpclMcFilterTool
from McParticleTools.McParticleToolsConf import TruthParticleCnvTool



###############################################################
# Add this if you want the McAodBuilder to work in 13.0.40
# This is the only difference wrt TruthSlimming.py in DPDUtils
# (from S. Binet, February 2008)
if jp.McAodFlags.doTruthEtIsolations() :
    print 'Doing truthEtIsolation...'
    if not hasattr(ToolSvc, 'TruthIsolationTool') :
        print 'ToolSvc has no attribute TruthIsolationTool...'
        ToolSvc += CfgMgr.TruthIsolationTool()
        pass
    pass
#
###############################################################

# Configure the actual truth slimming
topSequence += McAodBuilder(
    "McAodBuilder",
    OutputLevel = INFO,
    FilterTool = OldSpclMcFilterTool(
    McEvents             = "GEN_EVENT",
    #McEvents             = "GEN_AOD",
    McEventsOutput       = "GEN_AOD",
    #McEventsOutput       = "GEN_DPD",
    DoEtIsolations       = jp.McAodFlags.doTruthEtIsolations(),
    IncludeSimul         = False,
    IncludePartonShowers = False,
    ptGammaMin           = 1.*Units.GeV
    ),
    CnvTool = TruthParticleCnvTool(
    #McEvents             = "GEN_DPD",
    McEvents             = "GEN_AOD",
    TruthParticlesOutput = "SpclMC",
    DoEtIsolations       = jp.McAodFlags.doTruthEtIsolations()
    )
    )
ToolSvc.TruthIsolationTool.McEventsOutput = "GEN_AOD"
#ToolSvc.TruthIsolationTool.McEventsOutput = "GEN_DPD"

primaryDPD_SlimTruth_msg.info( '...done TruthSlimming!' )


