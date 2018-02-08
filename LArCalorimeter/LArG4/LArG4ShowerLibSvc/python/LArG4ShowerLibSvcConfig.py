# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getLArG4ShowerLibSvc(name="LArG4ShowerLibSvc", **kwargs):
    fileNameList = []
    #TODO make this configurable based on simFlags?
    # FCAL1 frozen shower libraries
    fileNameList += [ "LArG4ShowerLibData/MC16_v1/LArG4ShowerLib.FCAL1.11.root",
                      "LArG4ShowerLibData/MC16_v1/LArG4ShowerLib.FCAL1.22.root",
                      "LArG4ShowerLibData/MC16_v1/LArG4ShowerLib.FCAL1.2112.root",
                      "LArG4ShowerLibData/MC16_v1/LArG4ShowerLib.FCAL1.211.root" ]
    # FCAL2 frozen shower libraries
    fileNameList += [ "LArG4ShowerLibData/MC16_v1/LArG4ShowerLib.FCAL2.11.root",
                      "LArG4ShowerLibData/MC16_v1/LArG4ShowerLib.FCAL2.22.root",
                      "LArG4ShowerLibData/MC16_v1/LArG4ShowerLib.FCAL2.2112.root",
                      "LArG4ShowerLibData/MC16_v1/LArG4ShowerLib.FCAL2.211.root" ]
    kwargs.setdefault("FileNameList", fileNameList)
    return CfgMgr.LArG4ShowerLibSvc(name, **kwargs)
