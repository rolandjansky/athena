# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from IOVDbSvc.CondDB import conddb

def addLArFlatFolder (db, obj, calg, folder_base='/LAR/ElecCalibFlat/',qual=''):
    from AthenaCommon.AlgSequence import AthSequencer
    condSequence = AthSequencer("AthCondSeq")

    folder = folder_base + obj
    if not conddb.folderRequested(folder):
      conddb.addFolder(db, folder + qual,
                     className = 'CondAttrListCollection')
      condSequence += calg (ReadKey=folder, WriteKey='LAr'+obj+'SC')
    return

def LArSuperCellBCIDAlgDefault():

   from LArCabling.LArCablingAccess import LArOnOffIdMappingSC
   LArOnOffIdMappingSC()
   from CaloRec.CaloBCIDAvgAlgSCDefault import CaloBCIDAvgAlgSCDefault
   CaloBCIDAvgAlgSCDefault()
   from LArROD.LArRODConf import LArSuperCellBCIDAlg
   theSelf = LArSuperCellBCIDAlg("LArSuperCellBCIDAlgDefault",SCellContainerIn="SCellnoBCID",SCellContainerOut="SCell")
   return theSelf

def LArSuperCellBuilderAlgDefault():

   from LArCabling.LArCablingAccess import LArOnOffIdMappingSC
   from LArRecUtils.LArADC2MeVSCCondAlgDefault import LArADC2MeVSCCondAlgDefault
   from LArRecUtils.LArOFCSCCondAlgDefault import LArOFCSCCondAlgDefault
   from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArPedestalSC_ as LArPedestalSCFlatCondAlg
   from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArShapeSC_ as LArShapeSCCondAlg
   LArOnOffIdMappingSC()
   addLArFlatFolder ('LAR_OFL', 'Shape', LArShapeSCCondAlg,'/LAR/ElecCalibMCSC/')
   addLArFlatFolder ('LAR_OFL', 'Pedestal', LArPedestalSCFlatCondAlg,'/LAR/ElecCalibMCSC/')
   
   LArADC2MeVSCCondAlgDefault()
   LArOFCSCCondAlgDefault()
   from LArROD.LArRODConf import LArRawChannelBuilderAlg
   theSelf = LArRawChannelBuilderAlg("LArSuperCellBuilderAlg")
   theSelf.CablingKey='LArOnOffIdMapSC'
   theSelf.ADC2MeVKey='LArADC2MeVSC'
   theSelf.ShapeKey='LArShapeSC'
   theSelf.PedestalKey='LArPedestalSC'
   theSelf.OFCKey='LArOFCSC'
   theSelf.CaloCellKey='SCellnoBCID'
   theSelf.LArDigitKey='LArDigitSCL2'
   theSelf.useDB=False
   theSelf.IsSuperCell=True
   theSelf.ECutFortQ = -1024
   return theSelf
