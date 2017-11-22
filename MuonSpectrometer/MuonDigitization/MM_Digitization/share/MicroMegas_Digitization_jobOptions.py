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
                                        OutputLevel = DEBUG
                                        )
