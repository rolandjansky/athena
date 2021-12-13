###############################################################
#
# Job options file to configure:
#
#        TrigConf
#        Digitization
#        LVL1 Simulation
#        ByteStream conversion
#
# Author: Attila Krasznahorkay (Attila.Krasznahorkay@cern.ch)
# Author: Wolfgang Ehrenfeld (wolfgang.ehrenfeld@desy.de)
#
#==============================================================
#
# LVL1 Simulation
#
#==============================================================
from AthenaCommon.Resilience import protectedInclude
from AthenaCommon.DetFlags import DetFlags
from Digitization.DigitizationFlags import digitizationFlags
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.Logging import logging  # loads logger
log = logging.getLogger( "LVL1Digitization" )

#--------------------------------------------------------------
# set up simulation of LVL1 Trigger inputs
# (a prerequisite for LVL1 simulation)
#--------------------------------------------------------------
if DetFlags.simulateLVL1.LAr_on():
    protectedInclude( "LArL1Sim/LArL1Sim_G4_jobOptions.py" )
    # Noise
    if not digitizationFlags.doCaloNoise.get_Value():
        topSequence.LArTTL1Maker.NoiseOnOff= False #(default:True)
    # PileUp
    if DetFlags.pileup.LAr_on(): #DetFlags.pileup.LVL1_on():
        topSequence.LArTTL1Maker.PileUp = True
    else:
        topSequence.LArTTL1Maker.PileUp = False
    from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as commonGeoFlags
    if commonGeoFlags.Run()=="RUN3":
        from LArL1Sim.LArSCL1Getter import *
        theLArSCL1Getter = LArSCL1Getter()
        if not digitizationFlags.PileUpPresampling:
            from LArROD.LArSCellGetter import LArSCellGetter
            theLArSCellGetter = LArSCellGetter()

if DetFlags.simulateLVL1.Tile_on():
    protectedInclude( "TileSimAlgs/TileTTL1_jobOptions.py" )
    protectedInclude( "TileSimAlgs/TileMuonReceiver_jobOptions.py" )

if DetFlags.digitize.LVL1_on():
    # fwinkl, Oct 2021
    # Remove all code to configure the LVL1 simulation during digitization.
    # If this is a required use-case, check the git history of this file (commit d9432939)
    # and update it for the Run-3 trigger.
    log.error("Running the LVL1 simulation during digitization is currently not supported.")
