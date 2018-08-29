# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# python fragment to enable HV database access
from IOVDbSvc.CondDB import conddb

if not conddb.isMC and not conddb.isOnline:
    conddb.addFolder("DCS_OFL","/LAR/DCS/HV/BARREl/I16",className="CondAttrListCollection")
    conddb.addFolder("DCS_OFL","/LAR/DCS/HV/BARREL/I8",className="CondAttrListCollection")
    conddb.addFolder("LAR_OFL","/LAR/IdentifierOfl/HVLineToElectrodeMap",className="AthenaAttributeList")
    conddb.addFolder("LAR_OFL","/LAR/HVPathologiesOfl/Pathologies",className="AthenaAttributeList")

    from AthenaCommon.AlgSequence import AthSequencer
    from Gaudi.Configuration import DEBUG
    condseq = AthSequencer("AthCondSeq")

    from LArRecUtils.LArRecUtilsConf import LArHVIdMappingAlg
    hvmapalg = LArHVIdMappingAlg(ReadKey="/LAR/IdentifierOfl/HVLineToElectrodeMap",WriteKey="LArHVIdMap")
    hvmapalg.OutputLevel=DEBUG
    condseq += hvmapalg

    from LArRecUtils.LArRecUtilsConf import LArHVPathologyDbCondAlg
    hvpath = LArHVPathologyDbCondAlg(PathologyFolder="/LAR/HVPathologiesOfl/Pathologies",HVMappingKey="LArHVIdMap", HVPAthologyKey="LArHVPathology")
    hvpath.OutputLevel=DEBUG
    condseq += hvpath

    from LArRecUtils.LArRecUtilsConf import LArHVCondAlg
    hvcond = LArHVCondAlg(HVPathologies="LArHVPathology")
    hvcond.OutputLevel=DEBUG
    condseq += hvcond

if conddb.isMC:
    conddb.addFolderWithTag("LAR_OFL","/LAR/Identifier/HVLineToElectrodeMap","LARHVLineToElectrodeMap-001",className="AthenaAttributeList")
