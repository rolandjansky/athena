"""ComponentAccumulator configuration utilities for LArRecUtils

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.Logging import logging
from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg
from LArRecUtils.LArRecUtilsConf import LArMCSymCondAlg
from DetDescrCnvSvc.DetDescrCnvSvcConfig import DetDescrCnvSvcCfg
from LArConfiguration.LArElecCalibDBConfig import LArElecCalibDbCfg

def LArADC2MeVCondAlgCfg(flags):
    from LArRecUtils.LArRecUtilsConf import LArADC2MeVCondAlg 
    
    result=ComponentAccumulator()
    #This CondAlgo needs identifier helpers and the cable map
    result.merge(LArOnOffIdMappingCfg(flags))
    result.merge(DetDescrCnvSvcCfg(flags))
    theADC2MeVCondAlg=LArADC2MeVCondAlg()

    isMC=flags.Input.isMC
    
    if isMC:
        requiredConditons=["Ramp","DAC2uA","uA2MeV","MphysOverMcal","HVScaleCorr"]
        theADC2MeVCondAlg.LAruA2MeVKey="LAruA2MeVSym"
        theADC2MeVCondAlg.LArDAC2uAKey="LArDAC2uASym"
        theADC2MeVCondAlg.LArRampKey="LArRampSym"
        theADC2MeVCondAlg.LArMphysOverMcalKey="LArMphysOverMcalSym"
        theADC2MeVCondAlg.LArHVScaleCorrKey="LArHVScaleCorr"
        theADC2MeVCondAlg.UseFEBGainTresholds=False
    else: # not MC:
        requiredConditons=["Ramp","DAC2uA","uA2MeV","MphysOverMcal","HVScaleCorr"]
        from LArRecUtils.LArFebConfigCondAlgConfig import LArFebConfigCondAlgCfg
        if 'COMP200' in flags.IOVDb.DatabaseInstance: # Run1 case
            theADC2MeVCondAlg.LAruA2MeVKey="LAruA2MeVSym"
            theADC2MeVCondAlg.LArDAC2uAKey="LArDAC2uASym"
        result.merge(LArFebConfigCondAlgCfg(flags))

    result.merge(LArElecCalibDbCfg(flags,requiredConditons))
    result.addCondAlgo(theADC2MeVCondAlg,primary=True)
    return result 

def LArMCSymCondAlgCfg(flags, name="LArMCSymCondAlg", **kwargs):
    """Return ComponentAccumulator with configured LArMCSymCondAlg"""
    acc = ComponentAccumulator()
    kwargs.setdefault("ReadKey", "LArOnOffIdMap")
    acc.addCondAlgo(LArMCSymCondAlg(name, **kwargs))
    return acc

def LArAutoCorrNoiseCondAlgCfg(flags, name="LArAutoCorrNoiseCondAlg", **kwargs):
    """Return ComponentAccumulator with configured LArAutoCorrNoiseCondAlg"""

    from LArRecUtils.LArRecUtilsConf import LArAutoCorrNoiseCondAlg
    # The LArAutoCorrNoiseCondAlgCfg needs the cabling, the sym-object and the AutoCorr
    acc = LArOnOffIdMappingCfg(flags)
    acc.merge(LArMCSymCondAlgCfg(flags))
    acc.merge(LArElecCalibDbCfg(flags,["AutoCorr",]))
    kwargs.setdefault("nSampl", flags.LAr.ROD.nSamples)
    acc.addCondAlgo(LArAutoCorrNoiseCondAlg(name, **kwargs))
    return acc


def LArOFCCondAlgCfg (flags, name = 'LArOFCCondAlg', **kwargs):
    from AthenaCommon.SystemOfUnits import ns
    deltaBunch = int(flags.Beam.BunchSpacing/( 25.*ns)+0.5)

    mlog = logging.getLogger ('LArOFCCondAlgCfg')

    kwargs.setdefault ('isMC', True)
    kwargs.setdefault ('firstSample', flags.LAr.ROD.FirstSample)
    kwargs.setdefault ('useHighestGainAutoCorr', flags.LAr.ROD.UseHighestGainAutoCorr)
      
    if flags.LAr.ROD.DoOFCMixedOptimization: # kept for backward compatibility
        kwargs.setdefault ('UseDelta', 1) # only EMECIW/HEC/FCAL
        kwargs.setdefault ('DeltaBunch', deltaBunch)
        mlog.info("  OFC *MIXED* optimization")
    else:
        kwargs.setdefault ('UseDelta', flags.LAr.ROD.UseDelta)
        
        if flags.LAr.ROD.UseDelta == 0:
            mlog.info("  Standard OFC optimization computation")

        elif flags.LAr.ROD.UseDelta == 1:
            mlog.info("  OFC optimization asking for no average shift as extra constraint only in EMECIW/HEC/FCAL")
            kwargs.setdefault ('DeltaBunch', deltaBunch)

        elif flags.LAr.ROD.UseDelta == 2:
            mlog.info("  OFC optimization asking for no average shift as extra constraint everywhere")
            kwargs.setdefault ('DeltaBunch', deltaBunch)

        elif flags.LAr.ROD.UseDelta == 3:
            mlog.info("  OFC optimization asking for no average shift as extra constraint only in EMECIW/HEC/FCAL1+high eta FCAL2-3")
            kwargs.setdefault ('DeltaBunch', deltaBunch)
            
        else:
            kwargs.setdefault ('UseDelta', 0)

    #The LArPileUpTool needs: Calbling, Shape, Noise, Pedestal and the (total) AutoCorr
    acc = LArOnOffIdMappingCfg(flags)
    requiredConditons=["Shape","Noise","Pedestal"]
    acc.merge(LArElecCalibDbCfg(flags,requiredConditons))
    acc.merge(LArAutoCorrTotalCondAlgCfg(flags))
    from LArRecUtils.LArRecUtilsConf import LArOFCCondAlg
    acc.addCondAlgo (LArOFCCondAlg (name, **kwargs))
    return acc


def LArAutoCorrTotalCondAlgCfg (flags, name = 'LArAutoCorrTotalCondAlg', **kwargs):
    kwargs.setdefault("Nsamples", flags.LAr.ROD.nSamples)

    #The LArAutoCorrTotalAlg needs cabling and
    #Shape, AutoCorr, Noise, Pedestal, fSampl and MinBias 
    acc = LArOnOffIdMappingCfg(flags)
    requiredConditons=["Shape","AutoCorr","Noise","Pedestal","fSampl","MinBias"]
    acc.merge(LArElecCalibDbCfg(flags,requiredConditons))
    from LArRecUtils.LArRecUtilsConf import LArAutoCorrTotalCondAlg
    acc.addCondAlgo (LArAutoCorrTotalCondAlg (name, **kwargs))
    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.loadAllDynamicFlags()

    print ('--- LArOFCCondAlg 1')
    flags1 = ConfigFlags.clone()
    flags1.Input.Files = defaultTestFiles.RDO
    acc1 = LArOFCCondAlgCfg (flags1)
    acc1.printCondAlgs(summariseProps=True)
    acc1.wasMerged()

    print ('--- LArOFCCondAlg 2')
    flags2 = ConfigFlags.clone()
    flags2.Input.Files = defaultTestFiles.RDO
    flags2.LAr.ROD.UseDelta = 2
    acc2 = LArOFCCondAlgCfg (flags2)
    acc2.printCondAlgs(summariseProps=True)
    acc2.wasMerged()

    print ('--- LArOFCCondAlg 3')
    flags3 = ConfigFlags.clone()
    flags3.Input.Files = defaultTestFiles.RDO
    flags3.LAr.ROD.DoOFCMixedOptimization = True
    acc3 = LArOFCCondAlgCfg (flags3)
    acc3.printCondAlgs(summariseProps=True)
    acc3.wasMerged()

    print ('--- LArAutoCorrTotalCondAlg')
    flags4 = ConfigFlags.clone()
    flags4.Input.Files = defaultTestFiles.RDO
    flags4.LAr.ROD.nSamples = 32
    flags4.LAr.ROD.DoOFCMixedOptimization = True
    acc4 = LArAutoCorrTotalCondAlgCfg (flags4)
    acc4.printCondAlgs(summariseProps=True)
    acc4.wasMerged()
