# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# python fragment to enable HV database access
from IOVDbSvc.CondDB import conddb

if not conddb.isMC and not conddb.isOnline:
    conddb.addFolder("DCS_OFL","/LAR/DCS/HV/BARREl/I16",className="CondAttrListCollection")
    conddb.addFolder("DCS_OFL","/LAR/DCS/HV/BARREL/I8",className="CondAttrListCollection")
    conddb.addFolder("LAR_OFL","/LAR/IdentifierOfl/HVLineToElectrodeMap",className="AthenaAttributeList")
    conddb.addFolder("LAR_OFL","/LAR/HVPathologiesOfl/Pathologies",className="AthenaAttributeList")
    if conddb.dbdata != 'COMP200': 
       conddb.addFolder("LAR_OFL","/LAR/HVPathologiesOfl/Rvalues",className="AthenaAttributeList")

    from AthenaCommon.AlgSequence import AthSequencer
    condseq = AthSequencer("AthCondSeq")

    from LArRecUtils.LArRecUtilsConf import LArHVIdMappingAlg
    hvmapalg = LArHVIdMappingAlg(ReadKey="/LAR/IdentifierOfl/HVLineToElectrodeMap",WriteKey="LArHVIdMap")
    condseq += hvmapalg

    from LArRecUtils.LArRecUtilsConf import LArHVPathologyDbCondAlg
    hvpath = LArHVPathologyDbCondAlg(PathologyFolder="/LAR/HVPathologiesOfl/Pathologies",HVMappingKey="LArHVIdMap", HVPAthologyKey="LArHVPathology")
    condseq += hvpath

    from LArRecUtils.LArRecUtilsConf import LArHVCondAlg
    if conddb.dbdata != 'COMP200': 
      hvcond = LArHVCondAlg(HVPathologies="LArHVPathology",keyOutputCorr="LArHVScaleCorrRecomputed")
    else:  
      hvcond = LArHVCondAlg(HVPathologies="LArHVPathology",keyOutputCorr="LArHVScaleCorrRecomputed",doR=False)

    hvcond.UndoOnlineHVCorr=True
    condseq += hvcond

if conddb.isMC:
    conddb.addFolderWithTag("LAR_OFL","/LAR/Identifier/HVLineToElectrodeMap","LARHVLineToElectrodeMap-001",className="AthenaAttributeList")
