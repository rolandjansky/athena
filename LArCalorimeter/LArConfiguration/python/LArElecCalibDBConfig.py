# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator,ConfigurationError
from IOVDbSvc.IOVDbSvcConfig import IOVDbSvcCfg,addFolderList


#Import LArSymConditionsAlgs: Templated on the payload-type they handle
#These algs are mostly needed for MC processing
LArDAC2uASymAlg         =  CompFactory.getComp("LArSymConditionsAlg<LArDAC2uAMC, LArDAC2uASym>")                     
LArMinBiasAverageSymAlg =  CompFactory.getComp("LArSymConditionsAlg<LArMinBiasAverageMC, LArMinBiasAverageSym>")     
LArMinBiasSymAlg        =  CompFactory.getComp("LArSymConditionsAlg<LArMinBiasMC, LArMinBiasSym>")                   
LArNoiseSymAlg          =  CompFactory.getComp("LArSymConditionsAlg<LArNoiseMC, LArNoiseSym>")                       
LArRampSymAlg           =  CompFactory.getComp("LArSymConditionsAlg<LArRampMC, LArRampSym>")                         
LArfSamplSymAlg         =  CompFactory.getComp("LArSymConditionsAlg<LArfSamplMC, LArfSamplSym>")                     
LArAutoCorrSymAlg       =  CompFactory.getComp("LArSymConditionsAlg<LArAutoCorrMC, LArAutoCorrSym>")                 
LAruA2MeVSymAlg         =  CompFactory.getComp("LArSymConditionsAlg<LAruA2MeVMC, LAruA2MeVSym>")                     
LArShapeSymAlg          =  CompFactory.getComp("LArSymConditionsAlg<LArShape32MC, LArShape32Sym>")                   
LArMPhysOverMcalSymAlg  =  CompFactory.getComp("LArSymConditionsAlg<LArMphysOverMcalMC, LArMphysOverMcalSym>")       


LArHVScaleCorrCondFlatAlg  =  CompFactory.getComp("LArFlatConditionsAlg<LArHVScaleCorrFlat>")
LAruA2MeVCondAlg           =  CompFactory.getComp("LArFlatConditionsAlg<LAruA2MeVFlat>")
LArDAC2uACondAlg           =  CompFactory.getComp("LArFlatConditionsAlg<LArDAC2uAFlat>")
LArPedestalCondAlg         =  CompFactory.getComp("LArFlatConditionsAlg<LArPedestalFlat>")
LArRampCondAlg             =  CompFactory.getComp("LArFlatConditionsAlg<LArRampFlat>")
LArMphysOverMcalCondAlg    =  CompFactory.getComp("LArFlatConditionsAlg<LArMphysOverMcalFlat>")
LArOFCCondAlg              =  CompFactory.getComp("LArFlatConditionsAlg<LArOFCFlat>")
LArShapeCondAlg            =  CompFactory.getComp("LArFlatConditionsAlg<LArShapeFlat>")

LArDAC2uASCCondAlg = CompFactory.getComp("LArFlatConditionsAlg<LArDAC2uASC>")
LArRampSCCondAlg = CompFactory.getComp("LArFlatConditionsAlg<LArRampSC>")
LAruA2MeVSCCondAlg = CompFactory.getComp("LArFlatConditionsAlg<LAruA2MeVSC>")
LArfSamplSCCondAlg     =  CompFactory.getComp("LArFlatConditionsAlg<LArfSamplSC>")
LArShapeSCCondAlg      =  CompFactory.getComp("LArFlatConditionsAlg<LArShapeSC>")
LArPedestalSCCondAlg   =  CompFactory.getComp("LArFlatConditionsAlg<LArPedestalSC>")
LArNoiseSCCondAlg       =  CompFactory.getComp("LArFlatConditionsAlg<LArNoiseSC>")
LArMinBiasSCCondAlg       =  CompFactory.getComp("LArFlatConditionsAlg<LArMinBiasSC>")
LArAutoCorrSCCondAlg      =  CompFactory.getComp("LArFlatConditionsAlg<LArAutoCorrSC>")
LArPileupAverageSCCondAlg = CompFactory.getComp("LArFlatConditionsAlg<LArMinBiasAverageSC>")


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
        if condData == "fSampl" and ConfigFlags.Overlay.DataOverlay:
            LArMCSymCondAlg = CompFactory.LArMCSymCondAlg
            result.addCondAlgo(LArMCSymCondAlg(ReadKey="LArOnOffIdMap"))
            from IOVDbSvc.IOVDbSvcConfig import addFolders
            # TODO: does this need to be configurable?
            result.merge(addFolders(ConfigFlags, "/LAR/ElecCalibMC/fSampl", "LAR_OFL", className="LArfSamplMC", tag="LARElecCalibMCfSampl-G496-19213-FTFP_BERT_BIRK", db="OFLP200"))
            result.addCondAlgo(LArfSamplSymAlg(ReadKey="LArfSampl", WriteKey="LArfSamplSym"))
            continue

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
            calg = None
            if ConfigFlags.Overlay.DataOverlay and ConfigFlags.LAr.OFCShapeFolder == "4samples1phase":
                dbString+="<tag>LARElecCalibOflOFCPhysWaveRTM4samples1phase-RUN2-UPD4-00</tag>"
        if len(ConfigFlags.LAr.OFCShapeFolder)>0 and condData=="Shape":
            fldr="/LAR/ElecCalibOfl/Shape/RTM/"+ConfigFlags.LAr.OFCShapeFolder
            dbString="<db>COOLOFL_LAR/CONDBR2</db>"
            persClass="LArShapeComplete"
            calg = None
            if ConfigFlags.Overlay.DataOverlay and ConfigFlags.LAr.OFCShapeFolder == "4samples1phase":
                dbString+="<tag>LARElecCalibOflShapeRTM4samples1phase-RUN2-UPD4-00</tag>"

        iovDbSvc.Folders.append(fldr+dbString)# (addFolder(ConfigFlags,fldr,"LAR_ONL",'CondAttrListCollection'))
        condLoader.Load.append((persClass,fldr))
        if calg is not None:
            result.addCondAlgo(calg (ReadKey=fldr, WriteKey=outputKey))

    return result




def LArElecCalibDBRun1Cfg(ConfigFlags,condObjs):

    _larCondDBFoldersDataR1 = {"Ramp":("/LAR/ElecCalibOnl/Ramp","LAR_ONL","LArRampComplete",None),
                               "DAC2uA":("/LAR/ElecCalibOnl/DAC2uA","LAR_ONL","LArDAC2uAMC",LArDAC2uASymAlg),
                               "Pedestal":("/LAR/ElecCalibOnl/Pedestal<key>LArPedestal</key>","LAR_ONL","LArPedestalComplete",None),
                               "uA2MeV":("/LAR/ElecCalibOfl/uA2MeV/Symmetry","LAR_OFL", "LAruA2MeVMC",LAruA2MeVSymAlg),
                               "MphysOverMcal":("/LAR/ElecCalibOfl/MphysOverMcal/RTM","LAR_OFL","LArMphysOverMcalComplete",None),
                               "HVScaleCorr":("/LAR/ElecCalibOnl/HVScaleCorr","LAR_ONL","LArHVScaleCorrComplete",None),
                               "OFC":("/LAR/ElecCalibOfl/OFC/PhysWave/RTM/"+ ConfigFlags.LAr.OFCShapeFolder if len(ConfigFlags.LAr.OFCShapeFolder)>0 else "5samples1phase","LAR_OFL","LArOFCComplete",None),
                               "Shape":("/LAR/ElecCalibOfl/Shape/RTM/"+ ConfigFlags.LAr.OFCShapeFolder if len(ConfigFlags.LAr.OFCShapeFolder)>0 else "5samples1phase","LAR_OFL","LArShapeComplete",None),
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
            if obj.endswith ('MC'):
                obj = obj[:-2]
            result.addCondAlgo(calg(ReadKey=obj,WriteKey=obj+"Sym"))
    result.merge(addFolderList(ConfigFlags,folderlist))
                     
    return result


def LArElecCalibDBMCCfg(ConfigFlags,folders):
    _larCondDBFoldersMC = {
                           "Ramp":("LArRampMC","/LAR/ElecCalibMC/Ramp","LArRamp", LArRampSymAlg ),
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
    LArMCSymCondAlg=CompFactory.LArMCSymCondAlg
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


def LArElecCalibDBMCSCCfg(ConfigFlags,folders):
    _larCondDBFoldersMC = {
                           "RampSC":('CondAttrListCollection',"/LAR/ElecCalibMCSC/Ramp","LArRampSC", LArRampSCCondAlg ),
                           "DAC2uASC":('CondAttrListCollection',"/LAR/ElecCalibMCSC/DAC2uA","LArDAC2uASC",LArDAC2uASCCondAlg),
                           "uA2MeVSC":('CondAttrListCollection',"/LAR/ElecCalibMCSC/uA2MeV","LAruA2MeVSC",LAruA2MeVSCCondAlg),
                           "fSamplSC":('CondAttrListCollection','/LAR/ElecCalibMCSC/fSampl',"LArfSamplSC",LArfSamplSCCondAlg),
                           "ShapeSC":('CondAttrListCollection','/LAR/ElecCalibMCSC/Shape',"LArShapeSC",LArShapeSCCondAlg),
                           "PileupAverageSC":('CondAttrListCollection','/LAR/ElecCalibMCSC/LArPileupAverage',"LArPileupAverageSC",LArPileupAverageSCCondAlg),
                           "PedestalSC":('CondAttrListCollection','/LAR/ElecCalibMCSC/Pedestal',"LArPedestalSC",LArPedestalSCCondAlg),
                           "NoiseSC":('CondAttrListCollection','/LAR/ElecCalibMCSC/Noise',"LArNoiseSC",LArNoiseSCCondAlg),
                           "MinBiasSC":('CondAttrListCollection','/LAR/ElecCalibMCSC/MinBias',"LArMinBiasSC",LArMinBiasSCCondAlg),
                           "AutoCorrSC":('CondAttrListCollection','/LAR/ElecCalibMCSC/AutoCorr',"LArAutoCorrSC",LArAutoCorrSCCondAlg)
                       }

    result=ComponentAccumulator()
    folderlist=[]
    for folder in folders:
        try:
            classname,fldr,key,calg=_larCondDBFoldersMC[folder]
        except KeyError:
            raise ConfigurationError("No conditions data %s found for Monte Carlo" % folder)

        folderlist+=[(fldr,"LAR_OFL",classname),]
        if calg is not None:
            result.addCondAlgo(calg(ReadKey=fldr,WriteKey=key))

    result.merge(addFolderList(ConfigFlags,folderlist))
    return result


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.loadAllDynamicFlags()

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
    
