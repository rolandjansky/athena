# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# python fragment to enable HV database access
from IOVDbSvc.CondDB import conddb

if not conddb.isMC and not conddb.isOnline:
    conddb.addFolder("DCS_OFL","/LAR/DCS/HV/BARREl/I16")
    conddb.addFolder("DCS_OFL","/LAR/DCS/HV/BARREL/I8")
    conddb.addFolder("LAR_OFL","/LAR/IdentifierOfl/HVLineToElectrodeMap")
    conddb.addFolder("LAR_OFL","/LAR/HVPathologiesOfl/Pathologies")
