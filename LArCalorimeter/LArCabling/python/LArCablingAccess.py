# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AlgSequence import AthSequencer
from IOVDbSvc.CondDB import conddb
from LArRecUtils.LArRecUtilsConf import LArOnOffMappingAlg, LArFebRodMappingAlg, LArCalibLineMappingAlg

#FIXME, the folder are also set up in LArConditionsCommon/share/LArIdMap_*_jobOptions.py
 
def LArOnOffIdMapping():
    condSequence = AthSequencer("AthCondSeq")
    if hasattr(condSequence,"LArOnOffMappingAlg"):
        return #Already there....

    if conddb.isMC:
        dbname="LAR_OFL"
    else:
        dbname="LAR"

    folder="/LAR/Identifier/OnOffIdMap"
    conddb.addFolder(dbname,folder,className="AthenaAttributeList")
    condSequence+=LArOnOffMappingAlg(ReadKey=folder)
    return

def LArOnOffIdMappingSC():
    condSequence = AthSequencer("AthCondSeq")
    if hasattr(condSequence,"LArOnOffMappingAlg"):
        return #Already there....

    if conddb.isMC:
        dbname="LAR_OFL"
    else:
        dbname="LAR"

    folder="/LAR/IdentifierOfl/OnOffIdMap_SC"
    conddb.addFolder(dbname,folder,className="AthenaAttributeList")
    condSequence+=LArOnOffMappingAlg(ReadKey=folder, WriteKey="LArOnOffIdMapSC", isSuperCell=True)
    return



def LArFebRodMapping():
    condSequence = AthSequencer("AthCondSeq")
    if hasattr(condSequence,"LArFebRodMappingAlg"):
        return #Already there....

    if conddb.isMC:
        dbname="LAR_OFL"
    else:
        dbname="LAR"

    folder="/LAR/Identifier/FebRodMap"
    conddb.addFolder(dbname,folder,className="AthenaAttributeList")
    condSequence+=LArFebRodMappingAlg(ReadKey=folder)
    return


def LArCalibIdMapping():
    condSequence = AthSequencer("AthCondSeq")
    if hasattr(condSequence,"LArCalibLineMappingAlg"):
        return #Already there....

    if conddb.isMC:
        dbname="LAR_OFL"
    else:
        dbname="LAR"

    folder="/LAR/Identifier/CalibIdMap"
    conddb.addFolder(dbname,folder,className="AthenaAttributeList")
    condSequence+=LArCalibLineMappingAlg(ReadKey=folder)
    return
