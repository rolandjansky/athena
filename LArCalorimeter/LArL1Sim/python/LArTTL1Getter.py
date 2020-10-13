# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Author: F. Ledroit (ledroit@lpsc.in2p3.fr)
# LArTTL1 creation from LArHits with LArTTL1Maker algorithm

from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
import traceback

class LArTTL1Getter ( Configured )  :

        
    def configure(self):
        mlog = logging.getLogger( 'LArTTL1Getter::configure:' )
        mlog.info ('entering')        

        # get handle to upstream object
        try:
            from LArL1Sim.LArTTL1Getter import LArTTL1Getter
            theLArTTL1Getter=LArTTL1Getter()
        except Exception:
            mlog.error("could not get handle to LArTTL1Getter Quit")
            traceback.print_exc()
            return False

        if not theLArTTL1Getter.usable():
            if not self.ignoreConfigError():
                mlog.error("LArTTL1Getter unusable. Quit.")
                return False
            else:
                mlog.error("LArTTL1Getter unusable. Continue nevertheless")
                
        # Instantiation of the C++ algorithm
        try:        
            from LArL1Sim.LArL1SimConf import LArTTL1Maker                
        except Exception:
            mlog.error("could not import LArL1Sim.LArTTL1Maker")
            traceback.print_exc()
            return False

        theLArTTL1Maker=LArTTL1Maker()
        self._LArTTL1Maker = theLArTTL1Maker

        # Configure LArTTL1Maker here
        #theLArTTL1Maker.SubDetectors="LAr_All"
        #theLArTTL1Maker.EmBarrelHitContainerName="LArHitEMB"
        #theLArTTL1Maker.EmEndCapHitContainerName="LArHitEMEC"
        #theLArTTL1Maker.HecHitContainerName="LArHitHEC"
        #theLArTTL1Maker.ForWardHitContainerName="LArHitFCAL"
        
        #theLArTTL1Maker.EmTTL1ContainerName="LArTTL1EM"
        #theLArTTL1Maker.HadTTL1ContainerName="LArTTL1HAD"
        
        #theLArTTL1Maker.NoiseOnOff=true
        
        #theLArTTL1Maker.PileUp=false
        #theLArTTL1Maker.UseTriggerTime=false
        #theLArTTL1Maker.TriggerTimeToolName="CosmicTriggerTimeTool"
        
        #theLArTTL1Maker.EmBarrelCalibrationCoeffs=[1.03,  1.024, 1.019, 1.02,  1.02,  1.024, 1.03,  1.046, 1.06, 1.053, 1.057, 1.062, 1.063, 1.076, 1.176]
        #theLArTTL1Maker.EmEndCapCalibrationCoeffs=[1.176, 1.061, 1.087, 1.015, 1.019, 1.014, 1.014, 1.009, 1.01, 1.003, 1.016, 1.003, 0.993, 1.005, 0.963]
        #theLArTTL1Maker.HECCalibrationCoeffs=[1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1.]
        #theLArTTL1Maker.EmFcalCalibrationCoeffs=[1., 1., 1., 1.]
        #theLArTTL1Maker.HadFcalCalibrationCoeffs=[1., 1., 1., 1.]
        
        #theLArTTL1Maker.NoEmCalibrationMode=false
        #theLArTTL1Maker.NoHadCalibrationMode=false
        #theLArTTL1Maker.ChronoTest=false
        #theLArTTL1Maker.DebugThreshold=5000.

        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()

        # check if LArdigitization is run before. If yes, uses hit map from detector store produces from lardigitization
        from AthenaCommon.DetFlags import DetFlags
        if DetFlags.digitize.LAr_on():
            mlog.info("Using hit map from LArDigitMaker algoritm")
        else:
            mlog.info("digitmaker1 not found in topSequence, using own map in LArTTL1Maker")
            theLArTTL1Maker.useMapFromStore = False
 
        
        
        # now add algorithm to topSequence
        # this should always come at the end

        mlog.info(" now adding to topSequence")        
        topSequence += theLArTTL1Maker

        
        return True

    def LArTTL1Maker(self):
        return self._LArTTL1Maker
   


