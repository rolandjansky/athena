# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AlgSequence import AthSequencer
from IOVDbSvc.CondDB import conddb
from LArRecUtils.LArRecUtilsConf import LArOnOffMappingAlg, LArFebRodMappingAlg, LArCalibLineMappingAlg, LArLATOMEMappingAlg

from LArConditionsCommon.LArCondFlags import larCondFlags


#FIXME, the folder are also set up in LArConditionsCommon/share/LArIdMap_*_jobOptions.py
 
def LArOnOffIdMapping():
    condSequence = AthSequencer("AthCondSeq")
    if hasattr(condSequence,"LArOnOffMappingAlg"):
        return #Already there....

    if conddb.isMC:
        dbname="LAR_OFL"
        larCondFlags.config_idmap_MC()
    else:
        dbname="LAR"

    
    folder="/LAR/Identifier/OnOffIdMap"
    conddb.addFolder(dbname,folder,className="AthenaAttributeList")
    larCondFlags.addTag(folder,conddb)
    condSequence+=LArOnOffMappingAlg(ReadKey=folder)
    return

def LArOnOffIdMappingSC():
    condSequence = AthSequencer("AthCondSeq")
    folder="/LAR/IdentifierOfl/OnOffIdMap_SC"
    if hasattr(condSequence,"LArOnOffMappingAlgSC") :
        return #Already there....

    #for the moment SC mapping is only in MC database
    #and with one tag
    #conddb.addFolder(dbname,folder,className="AthenaAttributeList",forceMC=True)
    conddb.addFolder("","<db>COOLOFL_LAR/OFLP200</db>"+folder,className="AthenaAttributeList",forceMC=True)
    conddb.addOverride(folder,"LARIdentifierOflOnOffIdMap_SC-000")
    condSequence+=LArOnOffMappingAlg("LArOnOffMappingAlgSC",ReadKey=folder, WriteKey="LArOnOffIdMapSC", isSuperCell=True)
    return



def LArFebRodMapping():
    condSequence = AthSequencer("AthCondSeq")
    if hasattr(condSequence,"LArFebRodMappingAlg"):
        return #Already there....

    if conddb.isMC:
        dbname="LAR_OFL"
        larCondFlags.config_idmap_MC()
    else:
        dbname="LAR"

    folder="/LAR/Identifier/FebRodMap"
    conddb.addFolder(dbname,folder,className="AthenaAttributeList")
    larCondFlags.addTag(folder,conddb)
    condSequence+=LArFebRodMappingAlg(ReadKey=folder)
    return


def LArCalibIdMapping():
    condSequence = AthSequencer("AthCondSeq")
    if hasattr(condSequence,"LArCalibLineMappingAlg"):
        return #Already there....

    if conddb.isMC:
        dbname="LAR_OFL"
        larCondFlags.config_idmap_MC()
    else:
        dbname="LAR"

    folder="/LAR/Identifier/CalibIdMap"
    conddb.addFolder(dbname,folder,className="AthenaAttributeList")
    larCondFlags.addTag(folder,conddb)
    condSequence+=LArCalibLineMappingAlg(ReadKey=folder)
    return

def LArCalibIdMappingSC():
    condSequence = AthSequencer("AthCondSeq")
    folder="/LAR/Identifier/CalibIdMap_SC"
    if hasattr(condSequence,"LArCalibLineMappingAlgSC") and condSequence.LArCalibLineMappingAlg.ReadKey==folder:
        return #Already there....

    if conddb.isMC:
        return #No calib mapping in the MC (will be in LAR_OFL ?)
    else:
        dbname="LAR_ONL"
    conddb.addFolder(dbname,folder,className="AthenaAttributeList")
    condSequence+=LArCalibLineMappingAlg("LArCalibLineMappingAlgSC",ReadKey=folder, WriteKey="LArCalibIdMapSC",isSuperCell=True,MaxCL=16)
    return

def LArLATOMEMappingSC():
    folder="/LAR/Identifier/LatomeMapping"
    condSequence = AthSequencer("AthCondSeq")
    if hasattr(condSequence,"LArLATOMEMappingAlg") and condSequence.LArLATOMEMappingAlg.ReadKey==folder:
        return #Already there....

    if conddb.isMC:
        dbname="LAR_OFL"
        folder=""
        from AthenaCommon.Logging import logging
        mlog = logging.getLogger( 'LArCablingAccess' )
        mlog.warning("There is no LATOME mapping in the MC jobs yet")
    else:
        dbname="LAR_ONL"
    conddb.addFolder(dbname,folder,className="AthenaAttributeList")
    condSequence+=LArLATOMEMappingAlg("LArLATOMEMappingAlgSC",ReadKey=folder, WriteKey="LArLATOMEMap")

    return
