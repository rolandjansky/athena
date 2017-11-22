include.block ("MM_Digitization/MicroMegas_Digitization_jobOptions.py")
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence() 

from AthenaCommon import CfgGetter
from Digitization.DigitizationFlags import jobproperties

job += CfgGetter.getAlgorithm("MM_Digitizer/MM_Digitizer", tryDefaultConfigurable=True) 
from MM_Digitization.MM_DigitizationConf import MmDigitizationTool
MmDigitizationTool = MmDigitizationTool("MmDigitizationTool", 
                                        RndmSvc = jobproperties.Digitization.rndmSvc.getValue(),
                                        RndmEngine = "MM_Digitization",
                                        InputObjectName = "MicromegasSensitiveDetector", 
                                        OutputObjectName = "MM_DIGITS", 
                                        OutputSDOName = "MM_SDO", 
                                        # CheckSimHits = True,
                                        ## Strip response
                                        # qThreshold = 0.00000001,
                                        # DiffusSigma = 0.036,
                                        # LogitundinalDiffusSigma = 0.019,
                                        # DriftVelocity = 0.047,
                                        # crossTalk1 = 0.0,
                                        # crossTalk2 = 0.0,
                                        ## Chip/electronics response
                                        # peakTime = 50.,
                                        # SaveInternalHistos = True,
                                        # qThresholdForTrigger = 2,
                                        # ValidationSetup = False,
                                        # EnergyThreshold = 50,
                                        OutputLevel = DEBUG
                                        )
