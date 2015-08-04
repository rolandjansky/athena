# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# SetupCustomTrackingGeometry
class SetupCustomTrackingGeometry():
    # the constructor
    def __init__(self, 
                 name             = 'CustomTrackingGeometry',
                 geometryName     = 'CustomGeometry',
                 prefix           = '',
                 customPixel      = False,
                 customSCT        = False,
                 customTRT        = False,
                 writeXML         = True,
                 layerAssociation = True,
                 TRTstrawLayers   = False ) :
    
        # get the custom geometry builder 
        from ISF_FatrasDetDescrTools.CustomInDetTrackingGeometryBuilder import CustomInDetTrackingGeometryBuilder
        self.__ctGeometryBuilder__ = CustomInDetTrackingGeometryBuilder(name                = geometryName+'Builder',
                                                                        namePrefix          = prefix,
                                                                        setLayerAssociation = layerAssociation,
                                                                        buildTrtStrawLayers = TRTstrawLayers,
                                                                        buildCustomPixel    = customPixel,
                                                                        buildCustomSCT      = customSCT,
                                                                        buildCustomTRT      = customTRT,
                                                                        createXML           = writeXML,
                                                                        dictionaryFileName  = geometryName+'.xml')
        
        
        # now attach the file to GeoModel
        from AthenaCommon.AppMgr import theApp
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        
        import AthenaCommon.AtlasUnixStandardJob
        
        # Create DetDescrCnvSvc and add to SvcMgr
        from DetDescrCnvSvc.DetDescrCnvSvcConf import DetDescrCnvSvc
        DetDescrCnvSvc = DetDescrCnvSvc()
        DetDescrCnvSvc.IdDictFromRDB   = False
        DetDescrCnvSvc.InDetIDFileName = geometryName+'.xml'
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
