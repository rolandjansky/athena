# $Id: Data09MuCTPIFix_jobOfragment.py 275499 2010-01-27 18:25:00Z krasznaa $
#
# JobOptions fragment fixing the LVL1_ROI object in the ESD files for the
# problem in the MuCTPI. (Present for all 2009 collision data.)
#

# Create a new logger, saving the old one if it already exists with this name:
if "logger" in dir(): orig_logger = logger
from AthenaCommon.Logging import logging
logger = logging.getLogger( "Data09MuCTPIFix" )

# Set up the trigger configuration:
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
TriggerConfigGetter( "ReadPool" )
logger.info( "Set up the trigger configuration service" )

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

#
# Fix the RoIBResult object based on the MuCTPI_RDO:
#
from AnalysisTriggerAlgs.AnalysisTriggerAlgsConf import MuCTPI_RDOToRoIBResult
theJob += MuCTPI_RDOToRoIBResult()
theJob.MuCTPI_RDOToRoIBResult.RoIBOutputKey = "CorrectRoIBResult"
logger.info( "Added the RoIBResult object fixer algorithm" )

#
# Create the LVL1_ROI object with the correct muon content.
# Warning: The Calo RoIs should be taken from the original LVL1_ROI object!
#
from AnalysisTriggerAlgs.AnalysisTriggerAlgsConfig import RoIBResultToAOD
theJob += RoIBResultToAOD()
theJob.RoIBResultToAOD.DoCalo = False
theJob.RoIBResultToAOD.RoIBResultInputKey = "CorrectRoIBResult"
theJob.RoIBResultToAOD.LVL1_ROIOutputKey = "LVL1_ROI"

# Restore the original logger if necessary:
if "orig_logger" in dir(): logger = orig_logger
