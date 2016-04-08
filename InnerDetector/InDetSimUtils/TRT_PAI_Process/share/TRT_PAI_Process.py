from TRT_PAI_Process.TRT_PAI_ProcessConf import TRT_PAI_Process

trt_pai_Xe = TRT_PAI_Process(name="TRT_PAI_Process_Xe")
ToolSvc += trt_pai_Xe

trt_pai_Ar = TRT_PAI_Process(name="TRT_PAI_Process_Ar")
trt_pai_Ar.GasType = "Argon"
ToolSvc += trt_pai_Ar

trt_pai_Kr = TRT_PAI_Process(name="TRT_PAI_Process_Kr")
trt_pai_Kr.GasType = "Krypton"
ToolSvc += trt_pai_Kr

#choose random number service
from Digitization.DigitizationFlags import jobproperties
trt_pai_Xe.RndServ = jobproperties.Digitization.rndmSvc()
trt_pai_Ar.RndServ = jobproperties.Digitization.rndmSvc()
trt_pai_Kr.RndServ = jobproperties.Digitization.rndmSvc()
