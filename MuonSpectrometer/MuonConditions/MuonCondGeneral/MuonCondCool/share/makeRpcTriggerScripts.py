# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import sys,os
from os import popen

inputfolder="/afs/cern.ch/atlas/testbeam/muontbh8/scratch03/Monica2/Trigger/"
etasubfolder="Eta/"
phisubfolder="Phi/"

EtaSXX = popen('ls %s ' %(inputfolder+etasubfolder)).readlines()
PhiSXX = popen('ls %s ' %(inputfolder+phisubfolder)).readlines()

EtaChannel=0
PhiChannel=0

currentDir=os.getcwd()
if not os.path.isdir(currentDir+'/scripts'):
    os.mkdir(currentDir+'/scripts')

#content of the Phi job option sctript
#------------------------------------------
def makeTriggerPhiWriteJobOption(path,CM_Name,channel):
    PhiFileName = 'TriggerWritePhi_'+ CM_Name + '.py'
    jo=r"""
import AthenaCommon.AtlasUnixStandardJob
import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr

from AthenaCommon.AlgSequence import AlgSequence
job=AlgSequence()

dbConn="oracle://devdb10;schema=ATLAS_RPC;dbname=RPC_DQA;user=ATLAS_RPC"

#specific algos/tools

phifolder="/RPC/TRIGGER/CM_THR_PHI"
etafolder="/RPC/TRIGGER/CM_THR_ETA"
from MuonCondSvc.MuonCondSvcConf import MuonCalib__TriggerCoolSvc
TriggerCoolSvc=MuonCalib__TriggerCoolSvc("TriggerCoolSvc")
TriggerCoolSvc.OutputLevel=VERBOSE
TriggerCoolSvc.EtaFolder=etafolder
TriggerCoolSvc.PhiFolder=phifolder
# specify the COOL datatbase connection string

from IOVDbSvc.CondDB import conddb
include("RegistrationServices/IOVRegistrationSvc_jobOptions.py")

ServiceMgr.IOVDbSvc.dbConnection=dbConn

ServiceMgr += TriggerCoolSvc

from MuonCondCool.MuonCondCoolConf import MuonCalib__TriggerCool

TriggerCool=MuonCalib__TriggerCool()

TriggerCool.OutputLevel=VERBOSE
TriggerCool.WritePhi=True
TriggerCool.Read=False
TriggerCool.WriteEta=False

            
TriggerCool.Chan_Phi=%s
TriggerCool.FileName_CM_Phi="%s"
TriggerCool.FileName_Th0_Phi="%s"
TriggerCool.InfoPhi=" "


# data is written to conditions database using OutputConditionsAlg

if(TriggerCool.WritePhi or TriggerCool.Read ):
    
    from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
    OutCondAlg = OutputConditionsAlg("OutCondAlg","dummy.root")
    prefix = "CondAttrListCollection#"
    OutCondAlg.ObjectList=[prefix+phifolder]

    OutCondAlg.IOVTagList = ["TestPhi"]
    OutCondAlg.WriteIOV=True
    OutCondAlg.Run1=0
    #OutputConditionsAlg.Event1=0
    #OutCondAlg.Run2=999999999
    #OutputConditionsAlg.Event2=9999
    OutCondAlg.OutputLevel=VERBOSE
else:
    ServiceMgr.IOVDbSvc.Folders+=[phifolder+" <tag>TestPhi</tag> <dbConnection>"+dbConn+"</dbConnection>"] 

job+=TriggerCool
#ServiceMgr.EventSelector.RunNumber  = 1189
theApp.EvtMax=1

#theApp.initialize()
MessageSvc.OutputLevel = VERBOSE
""" % (channel,CM_Name,path)
    PhiMainScript = file(currentDir+'/scripts/'+PhiFileName,'w')
    PhiMainScript.write(jo)
    return PhiFileName


#content of the Eta job option sctript
#------------------------------------------
def makeTriggerEtaWriteJobOption(pathTh0,pathTh1,pathTh2,CM_Name,channel,sequence):
    EtaFileName = 'TriggerWriteEta_'+ CM_Name + '.py'
    jo=r"""
import AthenaCommon.AtlasUnixStandardJob
import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr

from AthenaCommon.AlgSequence import AlgSequence
job=AlgSequence()

dbConn="oracle://devdb10;schema=ATLAS_RPC;dbname=RPC_DQA;user=ATLAS_RPC"

#specific algos/tools

phifolder="/RPC/TRIGGER/CM_THR_PHI"
etafolder="/RPC/TRIGGER/CM_THR_ETA"
from MuonCondSvc.MuonCondSvcConf import MuonCalib__TriggerCoolSvc
TriggerCoolSvc=MuonCalib__TriggerCoolSvc("TriggerCoolSvc")
TriggerCoolSvc.OutputLevel=VERBOSE
TriggerCoolSvc.EtaFolder=etafolder
TriggerCoolSvc.PhiFolder=phifolder
# specify the COOL datatbase connection string

from IOVDbSvc.CondDB import conddb
include("RegistrationServices/IOVRegistrationSvc_jobOptions.py")

ServiceMgr.IOVDbSvc.dbConnection=dbConn

ServiceMgr += TriggerCoolSvc

from MuonCondCool.MuonCondCoolConf import MuonCalib__TriggerCool

TriggerCool=MuonCalib__TriggerCool()

TriggerCool.OutputLevel=VERBOSE
TriggerCool.WritePhi=False
TriggerCool.Read=False
TriggerCool.WriteEta=True


TriggerCool.Chan_Eta=%s
TriggerCool.FileName_CM="%s"
TriggerCool.FileName_Th0="%s"
TriggerCool.FileName_Th1="%s"
TriggerCool.FileName_Th2="%s"
TriggerCool.Sequence="%s"
TriggerCool.InfoEta=" "

# data is written to conditions database using OutputConditionsAlg

if(TriggerCool.WriteEta ):
    
    from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
    OutCondAlg = OutputConditionsAlg("OutCondAlg","dummy.root")
    prefix = "CondAttrListCollection#"
    OutCondAlg.ObjectList=[prefix+etafolder]

    OutCondAlg.IOVTagList = ["TestEta"]
    OutCondAlg.WriteIOV=True
    OutCondAlg.Run1=0
    #OutputConditionsAlg.Event1=0
    #OutCondAlg.Run2=999999999
    #OutputConditionsAlg.Event2=9999
    OutCondAlg.OutputLevel=VERBOSE
else:
    ServiceMgr.IOVDbSvc.Folders+=[etafolder+" <tag>TestEta</tag> <dbConnection>"+dbConn+"</dbConnection>"]      

job+=TriggerCool
#ServiceMgr.EventSelector.RunNumber  = 1189
theApp.EvtMax=1

#theApp.initialize()
MessageSvc.OutputLevel = VERBOSE
""" % (channel,CM_Name,pathTh0,pathTh1,pathTh2,sequence)
    EtaMainScript = file(currentDir+'/scripts/'+EtaFileName,'w')
    EtaMainScript.write(jo)
    return EtaFileName



# hier starts the main program
#--------------------------------------------
    
# big loop over all directories    
for i in range(len(EtaSXX)):
    # take care of eta
    #------------------------------
    EtaSXXi= EtaSXX[i].strip('\n')
    EtaPath= inputfolder+etasubfolder+EtaSXXi
    EtaSXXmu6=  popen('ls %s | grep .txt' %(EtaPath+"/mu6/")).readlines()
    EtaSXXmu10=  popen('ls %s | grep .txt' %(EtaPath+"/mu10/")).readlines()
    EtaSXXmu20=  popen('ls %s | grep .txt' %(EtaPath+"/mu20/")).readlines()
    #printfunc ("EtaSXXmu6= ",EtaSXXmu6)
    
    #just a safety check for eta
    if len(EtaSXXmu6)!= len(EtaSXXmu10) or len(EtaSXXmu6)!=len(EtaSXXmu20) or len(EtaSXXmu10)!=len(EtaSXXmu20):
        printfunc ("EtaSXXmu6, EtaSXXmu10, EtaSXXmu20 have different number of files")
        printfunc ("===>leaving loop")
        if EtaSXXi=="s56":
            printfunc ("it's s56")
            #continue
        else:
            break
    
    #take care of phi
    #-------------------------------
    PhiSXXi= PhiSXX[i].strip('\n')
    PhiPath=inputfolder+phisubfolder+PhiSXXi
    PhiSXXmu=  popen('ls %s | grep .txt' %(PhiPath)).readlines()
    #printfunc ("PhiSXX= ",PhiSXXmu )

    #just a safety check for phi      
    if len(PhiSXXmu) % 2 !=0:
        printfunc ("Nfiles(PhiSXX)= neparne")
        break
   
    #loop over eta files
    for j in range(len(EtaSXXmu6)):
        EtaChannel+= 1
        printfunc ("low pt EtaChannel= ", EtaChannel)

        EtaSXXmu6[j]=EtaSXXmu6[j].strip('\n')
        fileSuffix= EtaSXXmu6[j].split('_')[4]
        EtaName_CM6= EtaSXXmu6[j].replace('_'+fileSuffix,'')

        EtaSXXmu10[j]=EtaSXXmu10[j].strip('\n')
        fileSuffix= EtaSXXmu10[j].split('_')[4]
        EtaName_CM10= EtaSXXmu10[j].replace('_'+fileSuffix,'')

        printfunc ("EtaName_CM6= ", EtaName_CM6,"    |name= ",EtaSXXmu6[j])
        printfunc ("EtaName_CM10= ", EtaName_CM10,"    |name= ",EtaSXXmu10[j])
        genfile_name= makeTriggerEtaWriteJobOption(" ",EtaPath+"/mu6/"+EtaSXXmu6[j],EtaPath+"/mu10/"+EtaSXXmu10[j],EtaName_CM6,EtaChannel,"0,6,10")
        printfunc ("generated file ",genfile_name)

        EtaChannel+= 1
        printfunc ("high pt EtaChannel= ", EtaChannel)
        EtaSXXmu20[j]=EtaSXXmu20[j].strip('\n')
        fileSuffix= EtaSXXmu20[j].split('_')[4]
        EtaName_CM20= EtaSXXmu20[j].replace('_'+fileSuffix,'')

        printfunc ("EtaName_CM20= ", EtaName_CM20,"    |name= ",EtaSXXmu20[j])
        printfunc ("path20= ", EtaPath+"/mu20/"+EtaSXXmu20[j])
        genfile_name= makeTriggerEtaWriteJobOption(" "," ",EtaPath+"/mu20/"+EtaSXXmu20[j],EtaName_CM20,EtaChannel,"0,0,20")
        printfunc ("generated file ",genfile_name)
   
    #loop over phi files
    for j in range(len(PhiSXXmu)):
        PhiChannel+= 1
        printfunc ("PhiChannel= ", PhiChannel)
        PhiSXXmu[j]=PhiSXXmu[j].strip('\n')
        fileSuffix= PhiSXXmu[j].split('_')[4]
        PhiName_CM= PhiSXXmu[j].replace('_'+fileSuffix,'')

        printfunc ("PhiName_CM= ", PhiName_CM,"    |name= ",PhiSXXmu[j])
        printfunc ("path= ", PhiPath+"/"+PhiSXXmu[j])
        genfile_name= makeTriggerPhiWriteJobOption(PhiPath+"/"+PhiSXXmu[j],PhiName_CM,PhiChannel)
        printfunc ("generated file ",genfile_name)
