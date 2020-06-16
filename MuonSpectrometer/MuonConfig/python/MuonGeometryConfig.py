# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AtlasGeoModel.GeoModelConfig import GeoModelCfg
MuonDetectorTool=CompFactory.MuonDetectorTool
Muon__MuonIdHelperSvc=CompFactory.Muon.MuonIdHelperSvc
AGDDtoGeoSvc=CompFactory.AGDDtoGeoSvc
MuonAGDDTool, NSWAGDDTool=CompFactory.getComps("MuonAGDDTool","NSWAGDDTool",)

def MuonGeoModelCfg(flags):
    acc = ComponentAccumulator()

    # get the GeoModelSvc and add MuonDetectorTool
    gmsAcc=GeoModelCfg(flags )
    gms=gmsAcc.getPrimary()
    acc.merge(gmsAcc)

    detTool = MuonDetectorTool(
        HasCSC=flags.Detector.GeometryCSC,
        HasSTgc=flags.Detector.GeometrysTGC,
        HasMM=flags.Detector.GeometryMM
        )
    detTool.UseConditionDb = 1
    detTool.UseIlinesFromGM = 1

    enableAlignment = flags.Common.Project != 'AthSimulation' and not flags.Detector.SimulateMuon and not flags.Detector.OverlayMuon
    if enableAlignment:
        # This is all migrated from MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonAlignConfig.py

        from IOVDbSvc.IOVDbSvcConfig import addFolders
        MuonAlignmentCondAlg=CompFactory.MuonAlignmentCondAlg
        if (flags.Common.isOnline and not flags.Input.isMC):                
            acc.merge(addFolders( flags, ['/MUONALIGN/Onl/MDT/BARREL'], 'MUONALIGN', className='CondAttrListCollection'))
            acc.merge(addFolders( flags, ['/MUONALIGN/Onl/MDT/ENDCAP/SIDEA'], 'MUONALIGN', className='CondAttrListCollection'))
            acc.merge(addFolders( flags, ['/MUONALIGN/Onl/MDT/ENDCAP/SIDEC'], 'MUONALIGN', className='CondAttrListCollection'))
            acc.merge(addFolders( flags, ['/MUONALIGN/Onl/TGC/SIDEA'], 'MUONALIGN', className='CondAttrListCollection'))
            acc.merge(addFolders( flags, ['/MUONALIGN/Onl/TGC/SIDEC'], 'MUONALIGN', className='CondAttrListCollection'))
        else:
            acc.merge(addFolders( flags, ['/MUONALIGN/MDT/BARREL'], 'MUONALIGN_OFL', className='CondAttrListCollection'))
            acc.merge(addFolders( flags, ['/MUONALIGN/MDT/ENDCAP/SIDEA'], 'MUONALIGN_OFL', className='CondAttrListCollection'))
            acc.merge(addFolders( flags, ['/MUONALIGN/MDT/ENDCAP/SIDEC'], 'MUONALIGN_OFL', className='CondAttrListCollection'))
            acc.merge(addFolders( flags, ['/MUONALIGN/TGC/SIDEA'], 'MUONALIGN_OFL', className='CondAttrListCollection'))
            acc.merge(addFolders( flags, ['/MUONALIGN/TGC/SIDEC'], 'MUONALIGN_OFL', className='CondAttrListCollection'))

        MuonAlign = MuonAlignmentCondAlg()
        MuonAlign.ParlineFolders = ["/MUONALIGN/MDT/BARREL",
                                    "/MUONALIGN/MDT/ENDCAP/SIDEA",
                                    "/MUONALIGN/MDT/ENDCAP/SIDEC",
                                    "/MUONALIGN/TGC/SIDEA",
                                    "/MUONALIGN/TGC/SIDEC"]

        acc.addCondAlgo(MuonAlign)

        # Condition DB is needed only if A-lines or B-lines are requested
        if not (not flags.Muon.Align.UseALines and flags.Muon.Align.UseBLines=='none'):
            detTool.UseConditionDb = 1
        # here define to what extent B-lines are enabled
        if flags.Muon.Align.UseBLines=='none':
            detTool.EnableMdtDeformations = 0
        elif flags.Muon.Align.UseBLines=='all':
            detTool.EnableMdtDeformations = 1
        elif flags.Muon.Align.UseBLines=='barrel':
            detTool.EnableMdtDeformations = 2
        elif flags.Muon.Align.UseBLines=='endcaps':
            detTool.EnableMdtDeformations = 3


        # here define if I-lines (CSC internal alignment) are enabled
        if flags.Muon.Align.UseILines: 
            if 'HLT' in flags.IOVDb.GlobalTag:
                #logMuon.info("Reading CSC I-Lines from layout - special configuration for COMP200 in HLT setup.")
                MuonAlign.ILinesFromCondDB = False
                detTool.UseIlinesFromGM = True
                detTool.EnableCscInternalAlignment = False
            else :
                #logMuon.info("Reading CSC I-Lines from conditions database.")
                if (flags.Common.isOnline and not flags.Input.isMC):                
                    acc.merge(addFolders( flags, ['/MUONALIGN/Onl/CSC/ILINES'], 'MUONALIGN', className='CondAttrListCollection'))
                    detTool.EnableCscInternalAlignment = True
                else:
                    acc.merge(addFolders( flags, ['/MUONALIGN/CSC/ILINES'], 'MUONALIGN_OFL', className='CondAttrListCollection'))                
                    
                    MuonAlign.ParlineFolders += ["/MUONALIGN/CSC/ILINES"]
                    MuonAlign.ILinesFromCondDB = True
                    detTool.UseIlinesFromGM = False
                    detTool.EnableCscInternalAlignment = True

        # here define if As-Built (MDT chamber alignment) are enabled
        if flags.Muon.Align.UseAsBuilt:
            if flags.IOVDb.DatabaseInstance == 'COMP200' or \
                    'HLT' in flags.IOVDb.GlobalTag or flags.Common.isOnline :
                #logMuon.info("No MDT As-Built parameters applied.")
                detTool.EnableMdtAsBuiltParameters = 0
            else :
                #logMuon.info("Reading As-Built parameters from conditions database")
                detTool.EnableMdtAsBuiltParameters = 1
                acc.merge(addFolders( flags, '/MUONALIGN/MDT/ASBUILTPARAMS', 'MUONALIGN_OFL', className='CondAttrListCollection'))
                MuonAlign.ParlineFolders += ["/MUONALIGN/MDT/ASBUILTPARAMS"]
                pass
    #####

    else:
        detTool.UseConditionDb = 0
        detTool.UseAsciiConditionData = 0
        if flags.Detector.SimulateMuon:
            detTool.FillCacheInitTime = 0

    ## Additional material in the muon system
    AGDD2Geo = AGDDtoGeoSvc()
    muonAGDDTool = MuonAGDDTool("MuonSpectrometer", BuildNSW=False)
    AGDD2Geo.Builders += [ muonAGDDTool ]
    if (flags.Detector.GeometrysTGC and flags.Detector.GeometryMM):
        nswAGDDTool = NSWAGDDTool("NewSmallWheel", Locked=False)
        nswAGDDTool.Volumes = ["NewSmallWheel"]
        nswAGDDTool.DefaultDetector = "Muon"
        AGDD2Geo.Builders += [ nswAGDDTool ]
    #create=True is needed for the service to be initialised in the new style
    acc.addService(AGDD2Geo, create=True)
    
    # call fill cache of MuonDetectorTool such that all MdtReadoutElement caches are filled
    # already during initialize() -> this will increase memory -> needs to be measured
    detTool.FillCacheInitTime = 1
    # turn on/off caching of MdtReadoutElement surfaces
    detTool.CachingFlag = 1

    if enableAlignment:
        MuonDetectorCondAlg = CompFactory.MuonDetectorCondAlg
        MuonDetectorManagerCond = MuonDetectorCondAlg()
        MuonDetectorManagerCond.MuonDetectorTool = detTool
        acc.addCondAlgo(MuonDetectorManagerCond)

    gms.DetectorTools += [ detTool ]
    
    acc.addService( Muon__MuonIdHelperSvc("MuonIdHelperSvc",
        HasCSC=flags.Detector.GeometryCSC,
        HasSTgc=flags.Detector.GeometrysTGC,
        HasMM=flags.Detector.GeometryMM
        ) )

    return acc
