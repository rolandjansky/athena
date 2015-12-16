# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# SetupCustomTrackingGeometry
class SetupCustomTrackingGeometry():
    # the constructor
    def __init__(self, 
                 name                = 'CustomTrackingGeometry',
                 geometryName        = 'CustomGeometry',
                 prefix              = '',
                 PixelType           = 'ATLAS',
                 PixelEndcapType     = '',
                 SCT_Type            = 'ATLAS',
                 doSplitBarrelLayers = False,
                 doRingLayout        = False,
                 doPixel             = True,
                 do_SCT              = True,
                 dictionaryName      = "InDetIdDictFiles/IdDictInnerDetector_SLHC-24-24-dev4.xml",
                 writeXML            = True):
        
        # init ISF_Fatras geometry builder
        from ISF_FatrasDetDescrAlpine.CustomStagedInDetTrackingGeometryBuilder import CustomStagedInDetTrackingGeometryBuilder# get the custom geometry builder 
        self.__ctGeometryBuilder__ = CustomStagedInDetTrackingGeometryBuilder( name                = geometryName+'Builder',
                                                                               namePrefix          = prefix,
                                                                               PixelLayout         = PixelType,
                                                                               PixelEndcapLayout   = PixelEndcapType,
                                                                               SCTLayout           = SCT_Type,
                                                                               splitBarrelLayers   = doSplitBarrelLayers,
                                                                               isRingLayout        = doRingLayout,
                                                                               doPix               = doPixel,
                                                                               doSCT               = do_SCT,
                                                                               dictionaryFileName  = dictionaryName,
                                                                               createXML           = writeXML)

        # now attach the file to GeoModel
        from AthenaCommon.AppMgr import theApp
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        
        import AthenaCommon.AtlasUnixStandardJob
        
        # Create DetDescrCnvSvc and add to SvcMgr
        from DetDescrCnvSvc.DetDescrCnvSvcConf import DetDescrCnvSvc
        DetDescrCnvSvc = DetDescrCnvSvc()
        DetDescrCnvSvc.IdDictFromRDB   = False
        DetDescrCnvSvc.InDetIDFileName = dictionaryName
        DetDescrCnvSvc.IdDictName      = "IdDictParser/ATLAS_IDS.xml"
        DetDescrCnvSvc.DoInitNeighbours = False
        
        svcMgr += DetDescrCnvSvc
        svcMgr.EventPersistencySvc.CnvServices += [ "DetDescrCnvSvc" ]
        theApp.Dlls += [ "IdDictDetDescrCnv" ]
        
        from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
        TrkDetFlags.ISF_FatrasCustomGeometry = True
        TrkDetFlags.ISF_FatrasCustomGeometryBuilderName = geometryName+'Builder'

    # return function of the Geometry Builder
    def geometryBuilder(self):
        return self.__ctGeometryBuilder__       
