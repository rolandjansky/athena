from TRT_Digitization.TRT_DigitizationConf import TRT_SimDriftTimeTool

trt_dt_tool = TRT_SimDriftTimeTool()
ToolSvc += trt_dt_tool

#choose random number service
from Digitization.DigitizationFlags import jobproperties
trt_dt_tool.RndServ = jobproperties.Digitization.rndmSvc()

