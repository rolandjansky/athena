# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from IOVDbSvc.IOVDbSvcConfig import addFolders
from AthenaConfiguration.ComponentFactory import CompFactory

def LArFebConfigCondAlgCfg(flags, name="LArFEBConfigCondAlg", **kwargs): 
    """Return ComponentAccumulator with configured LArFEBConfigCondAlg"""
    defaultFolders = [
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
    LArFebConfigFolders = kwargs.setdefault("ListOfFolders", defaultFolders)
    kwargs.setdefault("keyOutput", "LArFebConfig")
    acc = addFolders(flags, LArFebConfigFolders, "LAR_ONL", "CondAttrListCollection")
    acc.addCondAlgo(CompFactory.LArFEBConfigCondAlg(name, **kwargs))
    return acc

