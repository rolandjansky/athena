from TRT_PAI_Process.TRT_PAI_ProcessConf import TRT_PAI_Process

trt_pai = TRT_PAI_Process(name="TRT_PAI_Process")
ToolSvc += trt_pai

trt_pai_optional = TRT_PAI_Process(name="TRT_PAI_Process_optional")
trt_pai_optional.GasType = "Argon"
ToolSvc += trt_pai_optional

#choose random number service
from Digitization.DigitizationFlags import jobproperties
trt_pai.RndServ = jobproperties.Digitization.rndmSvc()
trt_pai_optional.RndServ = jobproperties.Digitization.rndmSvc()
