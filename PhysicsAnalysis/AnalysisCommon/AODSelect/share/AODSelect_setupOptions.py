
# Only include this file once
include.block ("AODSelect/AODSelect_setupOptions.py")


# =============================================================================
# Common job preparation
# =============================================================================

from AthenaCommon.Logging import logging
logAODSelect_setupOptions = logging.getLogger( 'AODSelect_setupOptions' )
#logAODSelect_setupOptions.setLevel(DEBUG)

# Import the AODSelectFlags to steer the job
from AODSelect.AODSelectFlags import AODSelectFlags

# Get the helper methods
import AODSelect.AODSelectHelpers as ash

# Import all other needed things
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon import CfgMgr



# ====================================================================
# Check if we have Monte Carlo or real data, based on the inputFileSummary
# ====================================================================
from AthenaCommon.GlobalFlags  import globalflags
if not globalflags.DataSource() == 'data':
    logAODSelect_setupOptions.debug( "Detected that the input file is a simulated dataset" )
    AODSelectFlags.isSim = True
    pass


# Check for the presence of GEN_AOD, TruthEvent (ESD), GEN_EVENT, and SpclMC:
if AODSelectFlags.isSim():
    # Check which McEventCollection we have available
    from RecExConfig.ObjKeyStore import objKeyStore
    if objKeyStore.isInInput("McEventCollection", "GEN_AOD") \
           or objKeyStore['transient'].has_item("McEventCollection#GEN_AOD"):
        AODSelectFlags.mcEventCollection = "GEN_AOD"
        pass
    elif objKeyStore.isInInput("McEventCollection", "TruthEvent") \
             or objKeyStore['transient'].has_item("McEventCollection#TruthEvent"):
        AODSelectFlags.mcEventCollection = "TruthEvent"
        pass
    elif objKeyStore.isInInput("McEventCollection", "GEN_EVENT") \
             or objKeyStore['transient'].has_item("McEventCollection#GEN_EVENT"):
        AODSelectFlags.mcEventCollection = "GEN_EVENT"
        pass

    # Check which TruthParticleContainer we have available
    if objKeyStore.isInInput("TruthParticleContainer", "SpclMC") \
           or objKeyStore['transient'].has_item("TruthParticleContainer#SpclMC"):
        AODSelectFlags.truthParticleContainer = "SpclMC"
        pass
    
    # Print a message what MCEventCollection is found
    if AODSelectFlags.mcEventCollection() == "" :
        logAODSelect_setupOptions.warning( "No MCEventCollection found even though the input file is simulation!" )
        pass
    else:
        logAODSelect_setupOptions.debug( "Found the McEventCollection with name: %s" \
                                         % AODSelectFlags.mcEventCollection() )
        pass
    # Print a message what TruthParticleContainer is found
    if AODSelectFlags.truthParticleContainer() == "" :
        logAODSelect_setupOptions.warning( "No TruthParticleContainer found even though the input file is simulation!" )
        pass
    else:
        logAODSelect_setupOptions.debug( "Found the TruthParticleContainer with name: %s" \
                                         % AODSelectFlags.truthParticleContainer() )
        pass
    pass




# =============================================================================
# Create the AODSelect sequence
# =============================================================================
AODSelectSeq = CfgMgr.AthSequencer( "AODSelectSeq" )
topSequence += AODSelectSeq

# Add all requested filtering algorithms to the new AODSelect sequece
for _alg in AODSelectFlags.preFilterSequence():
    try:
        if _alg.getGaudiType() == 'Algorithm':
            logAODSelect_setupOptions.info("Adding algorithm %s to AODSelectSeq sequence" % _alg.getFullJobOptName() )
            AODSelectSeq += _alg
            pass
        else:
            logAODSelect_setupOptions.warning("Could NOT add %s (which is of type %s) to AODSelectSeq sequence"
                                              % (_alg, _alg.getGaudiType()) )
            pass
        pass
    except AttributeError as err:
        logAODSelect_setupOptions.warning("Could NOT add %s (which is of type %s) to AODSelectSeq sequence" % (_alg, type(_alg)) )
        print err
        pass
    pass

