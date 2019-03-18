# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Include import Include, IncludeError, include
from IOVDbSvc.CondDB import conddb
from AthenaCommon.AlgSequence import AthSequencer
from LArRecUtils.LArRecUtilsConf import LArFEBConfigCondAlg


def LArFebConfigCondAlgDefault (): 

    condSeq = AthSequencer("AthCondSeq")
    if hasattr (condSeq,"LArFEBConfigCondAlg"):
        return getattr(condSeq,"LArFEBConfigCondAlg")

    theLArFEBConfigCondAlg = LArFEBConfigCondAlg()

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
        conddb.addFolder("LAR_ONL",f,className="CondAttrListCollection")

    theLArFEBConfigCondAlg.ListOfFolders=LArFebConfigFolders
    theLArFEBConfigCondAlg.keyOutput="LArFebConfig"    

    condSeq+=theLArFEBConfigCondAlg
    return theLArFEBConfigCondAlg
