# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator,ConfigurationError
from IOVDbSvc.IOVDbSvcConfig import IOVDbSvcCfg,addFolderList

from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArDAC2uAMC_LArDAC2uASym_ as LArDAC2uASymAlg
from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArMinBiasAverageMC_LArMinBiasAverageSym_ as LArMinBiasAverageSymAlg
from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArMinBiasMC_LArMinBiasSym_ as LArMinBiasSymAlg
from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArNoiseMC_LArNoiseSym_ as LArNoiseSymAlg
from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArRampMC_LArRampSym_ as LArRampSymAlg
from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArfSamplMC_LArfSamplSym_ as LArfSamplSymAlg
from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LAruA2MeVMC_LAruA2MeVSym_ as LAruA2MeVSymAlg
from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArAutoCorrMC_LArAutoCorrSym_ as LArAutoCorrSymAlg
from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArShape32MC_LArShape32Sym_ as LArShapeSymAlg
from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArMphysOverMcalMC_LArMphysOverMcalSym_ as LArMPhysOverMcalSymAlg


from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArHVScaleCorrFlat_ as LArHVScaleCorrCondFlatAlg
from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LAruA2MeVFlat_ as LAruA2MeVCondAlg 
from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArDAC2uAFlat_ as LArDAC2uACondAlg 
from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArPedestalFlat_ as LArPedestalCondAlg 
from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArRampFlat_ as LArRampCondAlg 
from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArMphysOverMcalFlat_ as LArMphysOverMcalCondAlg 
from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArOFCFlat_ as LArOFCCondAlg 
from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArShapeFlat_ as LArShapeCondAlg



def LArElecCalibDbCfg(ConfigFlags,condObjs):

    #Check MC case
    if ConfigFlags.Input.isMC:
        return LArElecCalibDBMCCfg(ConfigFlags,condObjs)
    
    #Check run 1 case:
    if "COMP200" in ConfigFlags.IOVDb.DatabaseInstance:
        return LArElecCalibDBRun1Cfg(ConfigFlags,condObjs)

    #Everything else, eg run 2 (and 3?) data
    return LArElecCalibDBRun2Cfg(ConfigFlags,condObjs)
    

    

def LArElecCalibDBRun2Cfg(ConfigFlags,condObjs):

    _larCondDBFoldersDataR2 = {"Ramp":("LArRamp","/LAR/ElecCalibFlat/Ramp", LArRampCondAlg ),
                               "DAC2uA":("LArDAC2uA","/LAR/ElecCalibFlat/DAC2uA",LArDAC2uACondAlg),
                               "Pedestal":("LArPedestal","/LAR/ElecCalibFlat/Pedestal",LArPedestalCondAlg),
                               "uA2MeV":("LAruA2MeV","/LAR/ElecCalibFlat/uA2MeV", LAruA2MeVCondAlg),
                               "MphysOverMcal":("LArMphysOverMcal","/LAR/ElecCalibFlat/MphysOverMcal",LArMphysOverMcalCondAlg),
                               "OFC":("LArOFC","/LAR/ElecCalibFlat/OFC",LArOFCCondAlg),
                               "Shape":("LArShape","/LAR/ElecCalibFlat/Shape",LArShapeCondAlg),
                               "HVScaleCorr":("LArHVScaleCorr","/LAR/ElecCalibFlat/HVScaleCorr",LArHVScaleCorrCondFlatAlg),
                           }

    result=IOVDbSvcCfg(ConfigFlags)
    iovDbSvc=result.getService("IOVDbSvc")
    condLoader=result.getCondAlgo("CondInputLoader")


    for condData in condObjs:
        try:
            outputKey,fldr,calg=_larCondDBFoldersDataR2[condData]
        except KeyError:
            raise ConfigurationError("No conditions data %s found for Run-2 data" % condData)
            
        dbString="<db>COOLONL_LAR/CONDBR2</db>"
        persClass="CondAttrListCollection"
        #Potential special treatment for OFC/Shape: Load them from offline DB
        if len(ConfigFlags.LAr.OFCShapeFolder)>0 and condData=="OFC":
            fldr="/LAR/ElecCalibOfl/OFC/PhysWave/RTM/"+ConfigFlags.LAr.OFCShapeFolder
            dbString="<db>COOLOFL_LAR/CONDBR2</db>"
            persClass="LArOFCComplete"
        if len(ConfigFlags.LAr.OFCShapeFolder)>0 and condData=="Shape":
            fldr="/LAR/ElecCalibOfl/Shape/RTM/"+ConfigFlags.LAr.OFCShapeFolder
            dbString="<db>COOLOFL_LAR/CONDBR2</db>"
            persClass="LArShapeComplete"
        

        iovDbSvc.Folders.append(fldr+dbString)# (addFolder(ConfigFlags,fldr,"LAR_ONL",'CondAttrListCollection'))
        condLoader.Load.append((persClass,fldr))
        result.addCondAlgo(calg (ReadKey=fldr, WriteKey=outputKey))

    return result




def LArElecCalibDBRun1Cfg(ConfigFlags,condObjs):

    _larCondDBFoldersDataR1 = {"Ramp":("/LAR/ElecCalibOnl/Ramp","LAR_ONL","LArRampComplete",None),
                               "DAC2uA":("/LAR/ElecCalibOfl/DAC2uA","LAr_ONL","LArDAC2uAMC",LArDAC2uASymAlg),
                               "Pedestal":("/LAR/ElecCalibOnl/Pedestal<key>LArPedestal</key>","LAR_ONL","LArPedestalComplete",None),
                               "uA2MeV":("/LAR/ElecCalibOfl/uA2MeV/Symmetry","LAr_OFL", "LAruA2MeVMC",LAruA2MeVSymAlg),
                               "MphysOverMcal":("/LAR/ElecCalibOfl/MphysOverMcal/RTM","LAr_OFL","LArMphysOverMcalComplete",None),
                               "HVScale":("/LAR/ElecCalibOnl/HVScaleCorr","LAR_ONL","LArHVScaleCorrComplete",None),
                               "OFC":("/LAR/ElecCalibOfl/OFC/PhysWave/RTM/"+ ConfigFlags.LAr.OFCShapeFolder if len(ConfigFlags.LAr.OFCShapeFolder)>0 else "5samples1phase","LAr_OFL","LArOFCComplete",None),
                               "Shape":("/LAR/ElecCalibOfl/Shape/RTM/"+ ConfigFlags.LAr.OFCShapeFolder if len(ConfigFlags.LAr.OFCShapeFolder)>0 else "5samples1phase","LAr_OFL","LArShapeComplete",None),
                           }



    result=ComponentAccumulator()
    folderlist=[]
    for condData in condObjs:
        try:
            folder,db,obj,calg=_larCondDBFoldersDataR1[condData]
        except KeyError:
            raise ConfigurationError("No conditions data %s found for Run-1 data" % condData)
        folderlist.append((folder,db,obj))
        if (calg):
            result.addCondAlgo(calg(ReadKey="LAr"+obj,WriteKey="LAr"+obj+"Sym"))
    result.merge(addFolderList(ConfigFlags,folderlist))
                     
    return result


def LArElecCalibDBMCCfg(ConfigFlags,folders):
    _larCondDBFoldersMC = {"Ramp":("LArRampMC","/LAR/ElecCalibMC/Ramp","LArRamp", LArRampSymAlg ),
                           "AutoCorr":("LArAutoCorrMC","/LAR/ElecCalibMC/AutoCorr","LArAutoCorr", LArAutoCorrSymAlg),
                           "DAC2uA":("LArDAC2uAMC","/LAR/ElecCalibMC/DAC2uA","LArDAC2uA",LArDAC2uASymAlg),
                           "Pedestal":("LArPedestalMC","/LAR/ElecCalibMC/Pedestal","LArPedestal",None),
                           "Noise":("LArNoiseMC","/LAR/ElecCalibMC/Noise","LArNoise",LArNoiseSymAlg),
                           "fSampl":("LArfSamplMC","/LAR/ElecCalibMC/fSampl","LArfSampl",LArfSamplSymAlg),
                           "uA2MeV":("LAruA2MeVMC","/LAR/ElecCalibMC/uA2MeV","LAruA2MeV", LAruA2MeVSymAlg),
                           "MinBias":("LArMinBiasMC","/LAR/ElecCalibMC/MinBias","LArMinBias",LArMinBiasSymAlg),
                           "Shape":("LArShape32MC","/LAR/ElecCalibMC/Shape","LArShape",LArShapeSymAlg),
                           "MinBiasAvc":("LArMinBiasAverageMC","/LAR/ElecCalibMC/MinBiasAverage","LArMinBiasAverage",LArMinBiasAverageSymAlg),
                           "MphysOverMcal":("LArMphysOverMcalMC","/LAR/ElecCalibMC/MphysOverMcal","LArMphysOverMcal",LArMPhysOverMcalSymAlg),
                           "HVScaleCorr" : ("LArHVScaleCorrComplete", '/LAR/ElecCalibMC/HVScaleCorr',"LArHVScaleCorr",None) 
                       }


    result=ComponentAccumulator()
    #Add cabling
    from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg
    result.merge(LArOnOffIdMappingCfg(ConfigFlags))
    from LArRecUtils.LArRecUtilsConf import LArMCSymCondAlg
    result.addCondAlgo(LArMCSymCondAlg(ReadKey="LArOnOffIdMap"))
    folderlist=[]
    for folder in folders:
        try:
            classname,fldr,key,calg=_larCondDBFoldersMC[folder]
        except KeyError:
            raise ConfigurationError("No conditions data %s found for Monte Carlo" % folder)

        folderlist+=[(fldr,"LAR_OFL",classname),]
        if calg is not None:
            result.addCondAlgo(calg(ReadKey=key,WriteKey=key+"Sym"))

        result.merge(addFolderList(ConfigFlags,folderlist))
    return result
        


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.loadAllDynamicFlags (quiet = True)

    print ('--- run2')
    flags1 = ConfigFlags.clone()
    flags1.Input.Files = defaultTestFiles.RAW
    flags1.lock()
    acc1 = LArElecCalibDbCfg (flags1, ['Ramp', 'Pedestal'])
    acc1.printCondAlgs(summariseProps=True)
    print ('IOVDbSvc:', acc1.getService('IOVDbSvc').Folders)
    acc1.wasMerged()

    print ('--- run1')
    flags2 = ConfigFlags.clone()
    flags2.Input.Files = defaultTestFiles.RAW
    flags2.Input.ProjectName = 'data12_8TeV'
    flags2.lock()
    acc2 = LArElecCalibDbCfg (flags2, ['Ramp', 'Pedestal'])
    acc2.printCondAlgs(summariseProps=True)
    print ('IOVDbSvc:', acc2.getService('IOVDbSvc').Folders)
    acc2.wasMerged()

    print ('--- mc')
    flags3 = ConfigFlags.clone()
    flags3.Input.Files = defaultTestFiles.ESD
    flags3.lock()
    acc3 = LArElecCalibDbCfg (flags3, ['Ramp', 'Pedestal'])
    acc3.printCondAlgs(summariseProps=True)
    print ('IOVDbSvc:', acc3.getService('IOVDbSvc').Folders)
    acc3.wasMerged()
    
