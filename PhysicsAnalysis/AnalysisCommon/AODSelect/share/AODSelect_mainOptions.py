
# Only include this file once
include.block ("AODSelect/AODSelect_mainOptions.py")


# =============================================================================
# Common job preparation
# =============================================================================

from AthenaCommon.Logging import logging
logAODSelect_mainOptions = logging.getLogger( 'AODSelect_mainOptions' )
#logAODSelect_mainOptions.setLevel(DEBUG)

# Import the AODSelectFlags to steer the job
from AODSelect.AODSelectFlags import AODSelectFlags


# =============================================================================
# Determine if we even need to bother running this
# =============================================================================
if AODSelectFlags.doAll() or AODSelectFlags.printAODSelect():
    AODSelectFlags.doAODSelect = True
    pass

# If we found that we don't need to run AODSelect, we can simply exit this python file
if AODSelectFlags.doAODSelect():
    logAODSelect_mainOptions.debug( "Found that we actually need to run AODSelect since we found at least one request to run it" )
    
    # =============================================================================
    # Import the remaining stuff
    # =============================================================================
    # Get the helper methods
    import AODSelect.AODSelectHelpers as ash

    # Import all other needed things
    from AthenaCommon.AppMgr import theApp
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr

    # =============================================================================
    # Schedule the creation of all sensible stuff
    # =============================================================================
    if AODSelectFlags.doAll():
        AODSelectFlags.doAODSelect = True
        include("AODSelect/AODSelect_doAllOptions.py")
        pass

    # =============================================================================
    # Create an instance of the AODSelectConfiguration helper class
    # =============================================================================
    logAODSelect_mainOptions.debug( "Creating the configurationHelper" )
    configurationHelper = ash.AODSelectConfiguration()
    logAODSelect_mainOptions.debug( "Done creating the configurationHelper" )


    # =============================================================================
    # Include all the domain-specific job options
    # =============================================================================
    include ("AODSelect/AODSelect_commonOptions.py")
    include ("AODSelect/AODSelect_muonOptions.py")
    include ("AODSelect/AODSelect_egammaOptions.py")
    include ("AODSelect/AODSelect_jetOptions.py")
    include ("AODSelect/AODSelect_matchingOptions.py")


    # =============================================================================
    # Print the final configuration
    # =============================================================================
    if logAODSelect_mainOptions.isEnabledFor(logging.VERBOSE): configurationHelper.doPrint()

    # =============================================================================
    # Now, actually create the algorithms and assign the requested tools 
    # =============================================================================
    retVal = configurationHelper.scheduleAllAlgs( AODSelectSeq )
    if not retVal:
        logAODSelect_mainOptions.error( "Could NOT set up and schedule all AODSelect algorithms! Something went wrong..." )
        pass

    # =============================================================================
    # Do a printout, if wanted
    # =============================================================================
    if AODSelectFlags.printAODSelect():
        ash.printAODSelect()
        if AODSelectFlags.printAODSelect.ExitAfterPrintout :
            exit(0)
            pass
        pass

    
    pass # End: if AODSelectFlags.doAODSelect()
