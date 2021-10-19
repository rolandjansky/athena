# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

class FatrasTrackingGeometryCondAlg:
  def __init__(self):
    from AthenaCommon.AppMgr import ToolSvc

    ##################################################################################
    # The FatrasTrackingGeometry Svc fragment
    #
    # usage: 
    #   include('TrkDetDescrSvc/FatrasTrackingGeometryCondAlg.py')
    #
    #    + for the navigator, set the TrackingGeometryName to:
    #      FatrasTrackingGeometryCondAlg.trackingGeometryName()
    #
    ##################################################################################
    # import the DetFlags for the setting
    from AthenaCommon.DetFlags import DetFlags

    #################################################################################
    # Material for the Geometry
    #
    # if only ID is switched on different COOLDB Folders
    #    and different Tag is used
    # 
    #################################################################################

    from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
    
    if TrkDetFlags.ConfigurationOutputLevel() < 3 :
      print ('[ Configuration : start ] *** FatrasTrackingGeometry ********************************')
      print ('[ TrackingGeometrySvc ]')

    # check whether the material retrieval is ment to be from COOL
    if TrkDetFlags.MaterialFromCool() :
        # the tag names
        CoolDataBaseFolder = TrkDetFlags.MaterialStoreGateKey()
        AtlasMaterialTag = TrkDetFlags.MaterialTagBase()+str(TrkDetFlags.MaterialVersion())+'_'

        if TrkDetFlags.ConfigurationOutputLevel() < 3 :
           print ('[ TrackingGeometrySvc ] Associating DB folder : ',CoolDataBaseFolder)
           print ('[ TrackingGeometrySvc ]     base material tag : ',AtlasMaterialTag)

        # we need the conditions interface
        from IOVDbSvc.CondDB import conddb
        # use a local database
        if TrkDetFlags.MaterialDatabaseLocal():
            # specify the local database
            DataBasePath  = TrkDetFlags.MaterialDatabaseLocalPath() 
            DataBaseName  = TrkDetFlags.MaterialDatabaseLocalName()
            MagicTag      = TrkDetFlags.MaterialMagicTag()
            DataBaseConnection = '<dbConnection>sqlite://X;schema='+DataBasePath+DataBaseName+';dbname=OFLP200</dbConnection>'
            conddb.blockFolder('/GLOBAL/TrackingGeo/LayerMaterial')
            conddb.addFolderWithTag('',DataBaseConnection+CoolDataBaseFolder,AtlasMaterialTag+MagicTag,force=True)
            if TrkDetFlags.ConfigurationOutputLevel() < 3 :
                print ('[ TrackingGeometrySvc ] Using Local Database: '+DataBaseConnection        )
            # make sure that the pool files are in the catalog
            #from PoolSvc.PoolSvcConf import PoolSvc
            #PoolSvc.ReadCatalog += [ DataBasePath+'PoolFileCatalog.xml' ]
        else :
            # load the right folders (preparation for calo inclusion)
            cfolder = CoolDataBaseFolder +'<tag>TagInfoMajor/'+AtlasMaterialTag+'/GeoAtlas</tag>'
            conddb.addFolderSplitMC('GLOBAL',cfolder,cfolder)

    #################################################################################
    # The Geometry Builder
    #################################################################################

    # the name to register the Geometry
    FatrasTrackingGeometryName = 'FatrasTrackingGeometry'

    # the geometry builder alg tool
    from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__GeometryBuilder
    FatrasGeometryBuilder = Trk__GeometryBuilder(name = 'FatrasGeometryBuilder')
    # switch on the Detectors
    FatrasGeometryBuilder.InDetTrackingGeometry  = DetFlags.ID_on()
    FatrasGeometryBuilder.CaloTrackingGeometry   = DetFlags.Calo_on()
    FatrasGeometryBuilder.MuonTrackingGeometry   = DetFlags.Muon_on() # avoid double building of MTG
    # register it to tool svc
    ToolSvc += FatrasGeometryBuilder

    # (ID) 
    if DetFlags.ID_on() :
      # get hand on the ID Tracking Geometry Builder
      from InDetTrackingGeometry.ConfiguredInDetTrackingGeometryBuilder import ConfiguredInDetTrackingGeometryBuilder as IDGeometryBuilder

      self.__InDetTrackingGeometryBuilder__ = IDGeometryBuilder(name ='InDetTrackingGeometryBuilder',
                                                                namePrefix = 'Fatras',
                                                                setLayerAssociation = False,
                                                                buildTrtStrawLayers = True)
      self.__InDetTrackingGeometryBuilder__.VolumeEnclosureOuterR = 1148.

      # make a public tool out of it
      ToolSvc += self.__InDetTrackingGeometryBuilder__
      # and give it to the Geometry Builder
      FatrasGeometryBuilder.InDetTrackingGeometryBuilder = self.__InDetTrackingGeometryBuilder__
      # 
    # (Calo)
    if DetFlags.Calo_on():
       from CaloTrackingGeometry.ConfiguredCaloTrackingGeometryBuilder import ConfiguredCaloTrackingGeometryBuilder as ConfiguredCaloGeo
       CaloTrackingGeometryBuilder = ConfiguredCaloGeo(name = 'FatrasCaloTrackingGeometryBuilder')
       CaloTrackingGeometryBuilder.RecordLayerIndexCaloSampleMap = True
       # make a publid tool
       ToolSvc += CaloTrackingGeometryBuilder
       # and give it to the Geometry Builder
       FatrasGeometryBuilder.CaloTrackingGeometryBuilder = CaloTrackingGeometryBuilder

    # (Muon)  
    if DetFlags.Muon_on():
       from MuonTrackingGeometry.ConfiguredMuonTrackingGeometry import MuonTrackingGeometryBuilder
       FatrasMuonTrackingGeometryBuilder = MuonTrackingGeometryBuilder(name = 'FatrasMuonTrackingGeometryBuilder')
       FatrasMuonTrackingGeometryBuilder.BarrelZ = 6783.  
       FatrasMuonTrackingGeometryBuilder.InnerBarrelRadius = 4255.  
       ToolSvc += FatrasMuonTrackingGeometryBuilder
       # and give it to the Geometry Builder
       FatrasGeometryBuilder.MuonTrackingGeometryBuilder = FatrasMuonTrackingGeometryBuilder      
  
    from FatrasExample.FatrasTuning import FatrasTuningFlags
    from InDetCondFolders import InDetAlignFolders_FATRAS     # noqa: F401
    from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlg import ConfiguredTrackingGeometryCondAlg as GeometryCondAlg
    FatrasTrackingGeometryCondAlg = GeometryCondAlg(name = 'FatrasTrackingGeometryCondAlg')

    # register the Builder 
    FatrasTrackingGeometryCondAlg.GeometryBuilder = FatrasGeometryBuilder
    # and register the name under which the geometry is registered
    FatrasTrackingGeometryCondAlg.TrackingGeometryWriteKey = FatrasTrackingGeometryName
    # global scaling of the material if necessary
    FatrasTrackingGeometryCondAlg.GlobalScaleFactor = FatrasTuningFlags.MaterialScalor()
    # some flags - only works when ID is on
    FatrasTrackingGeometryCondAlg.BuildGeometryFromTagInfo = True
    # switch to the rigth CoolDB folder 
    FatrasTrackingGeometryCondAlg.InputLayerMaterialSetName = CoolDataBaseFolder

    from AthenaCommon.AlgSequence import AthSequencer
    condSeq = AthSequencer("AthCondSeq")
    condSeq+= FatrasTrackingGeometryCondAlg

    print ('* [ Configuration : start ] *** FatrasTrackingGeometry ********************************')
    print ('* [ FatrasTrackingGeometryCondAlg   ]')
    print (FatrasTrackingGeometryCondAlg)
    print ('* [ GeometryBuilder       ]')
    print (FatrasGeometryBuilder)
    print ('* [ Configuration : end   ] *** FatrasTrackingGeometry ********************************')
