from AthenaCommon.Logging import logging

log = logging.getLogger("MuonCalibAlgs")

log.warning("""include(\"MuonCalibAlgs/MuonCalibAlgs.py\") is deprecated.
use instead: from MuonCalibAlgs import MuonCalib
This is temporarily done for you now, and should be backwards compatible""" )

from MuonCalibAlgs.MuonCalib import MuonSegmentToCalibSegment,MuonCalibAlg

# MuonCalibExtraTreeAlg may or may not be there, depending on the settings
try:
    from MuonCalibAlgs.MuonCalib import MuonCalibExtraTreeAlg
except ImportError:
    pass
