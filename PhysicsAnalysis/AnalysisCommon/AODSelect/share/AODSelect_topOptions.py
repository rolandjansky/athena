
# Only include this file once
include.block ("AODSelect/AODSelect_topOptions.py")


############################
## Common job preparation ##
############################

from AthenaCommon.Logging import logging
logAODSelect_topOptions = logging.getLogger( 'AODSelect_topOptions' )
#logAODSelect_topOptions.setLevel(DEBUG)

# Import the AODSelectFlags to steer the job
from AODSelect.AODSelectFlags import AODSelectFlags

# Get the helper methods
import AODSelect.AODSelectHelpers as ash


# Import all other needed things
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon import CfgMgr




############################
## Do a printout, if wanted
############################
if AODSelectFlags.printAODSelect():
    ash.printAODSelect()
    if AODSelectFlags.printAODSelect.ExitAfterPrintout :
        exit(0)
        pass
    pass
