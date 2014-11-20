include.block ("sTGC_Digitization/sTGC_Digitization_jobOptions.py")
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence() 

from AthenaCommon import CfgGetter
from Digitization.DigitizationFlags import jobproperties

job += CfgGetter.getAlgorithm("sTGC_Digitizer/sTGC_Digitizer", tryDefaultConfigurable=True) 
from sTGC_Digitization.sTGC_DigitizationConf import sTgcDigitizationTool
sTgcDigitizationTool = sTgcDigitizationTool("sTgcDigitizationTool", 
                                            RndmSvc = jobproperties.Digitization.rndmSvc.get_Value(),
                                            RndmEngine = "sTGC_Digitization",
                                            InputObjectName = "sTGCSensitiveDetector", 
                                            OutputObjectName = "sTGC_DIGITS", 
                                            OutputSDOName = "sTGC_SDO",
					    OutputLevel = VERBOSE)


#from AthenaCommon import CfgGetter
#from Digitization.DigitizationFlags import jobproperties
#if jobproperties.Digitization.doXingByXingPileUp(): # PileUpTool approach
#    job.PileUpToolsAlg.PileUpTools += [ CfgGetter.getPrivateTool("sTgcDigitizationTool", checkType=True) ]
#else : # Algorithm approach
#    job += CfgGetter.getAlgorithm("sTGC_Digitizer/sTGC_Digitizer", tryDefaultConfigurable=True)
