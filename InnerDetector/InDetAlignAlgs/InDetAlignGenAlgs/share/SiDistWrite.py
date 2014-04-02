# job options for SiDistWriteAlg
# read pixel distortion data from a text file and put it in TDS and then condDB

key='/Indet/PixelDist'
from InDetAlignGenAlgs.InDetAlignGenAlgsConf import SiDistWriteAlg
SiDistWrite=SiDistWriteAlg(ReadFile="PixelDistortionsParameter.txt",
                           KeyName=key,ArraySize=3,
                           Print=True)
topSequence+=SiDistWrite

# register in conditions database
include("RegistrationServices/OutputConditionsAlg_jobOptions.py")
ToolSvc.ConditionsAlgStream.OutputFile="distobj.root"
OutputConditionsAlg.ObjectList=["DetCondCFloat#"+key]
OutputConditionsAlg.WriteIOV=True
OutputConditionsAlg.IOVTagList=["InDetPixelDist-000-00"]
