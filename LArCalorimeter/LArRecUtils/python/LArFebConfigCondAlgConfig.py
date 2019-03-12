# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from IOVDbSvc.IOVDbSvcConfig import addFolders
from LArRecUtils.LArRecUtilsConf import LArFEBConfigCondAlg


def LArFEBConfigCondAlgCfg (configFlags): 

    result=ComponentAccumulator()

    LArFebConfigFolders=[
    "/LAR/Configuration/FEBConfig/Physics/EMBA1",  
    "/LAR/Configuration/FEBConfig/Physics/EMBA2",
    "/LAR/Configuration/FEBConfig/Physics/EMBA3",
    "/LAR/Configuration/FEBConfig/Physics/EMBA4",
    "/LAR/Configuration/FEBConfig/Physics/EMBC1",
    "/LAR/Configuration/FEBConfig/Physics/EMBC2",
    "/LAR/Configuration/FEBConfig/Physics/EMBC3",
    "/LAR/Configuration/FEBConfig/Physics/EMBC4", 
    "/LAR/Configuration/FEBConfig/Physics/EMECA1",
    "/LAR/Configuration/FEBConfig/Physics/EMECA2",
    "/LAR/Configuration/FEBConfig/Physics/EMECA3",
    "/LAR/Configuration/FEBConfig/Physics/EMECC1",
    "/LAR/Configuration/FEBConfig/Physics/EMECC2",
    "/LAR/Configuration/FEBConfig/Physics/EMECC3",
    "/LAR/Configuration/FEBConfig/Physics/FCALA", 
    "/LAR/Configuration/FEBConfig/Physics/FCALC", 
    "/LAR/Configuration/FEBConfig/Physics/HECA", 
    "/LAR/Configuration/FEBConfig/Physics/HECC", 
    ]


    for f in LArFebConfigFolders:
        conddb.addFolder("LAR_ONL",f)
        result.merge(addFolders(configFlags,f,"LAR_ONL",className="CondAttrListCollection"))

    result.addCondAlgo(LArFEBConfigCondAlg(ListOfFolders=LArFebConfigFolders,keyOutput="LArFebConfig"))    

    return tool
