# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AtlasGeoModel.GeoModelConfig import GeoModelCfg
from MuonGeoModel.MuonGeoModelConf import MuonDetectorTool
from MuonIdHelpers.MuonIdHelpersConf import Muon__MuonIdHelperSvc
from AGDD2GeoSvc.AGDD2GeoSvcConf import AGDDtoGeoSvc
from MuonAGDD.MuonAGDDConf import MuonAGDDTool, NSWAGDDTool
from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags

def MuonGeoModelCfg(flags):
    acc = ComponentAccumulator()

    # get the GeoModelSvc and add MuonDetectorTool
    gmsAcc=GeoModelCfg(flags )
    gms=gmsAcc.getPrimary()
    acc.merge(gmsAcc)

    detTool = MuonDetectorTool(
        HasCSC=MuonGeometryFlags.hasCSC(),
        HasSTgc=MuonGeometryFlags.hasSTGC(),
        HasMM=MuonGeometryFlags.hasMM()
        )
    detTool.UseConditionDb = 1
    detTool.UseIlinesFromGM = 1
    detTool.BuildFromNova = 0
    if ( ( not flags.Detector.SimulateMuon or flags.Detector.OverlayMuon ) and flags.Common.Project != "AthSimulation" ):
        # This is all migrated from MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonAlignConfig.py

        from IOVDbSvc.IOVDbSvcConfig import addFolders
        from MuonCondAlg.MuonCondAlgConf import MuonAlignmentCondAlg
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
            detTool.EnableCscInternalAlignment = True
            if 'HLT' in flags.IOVDb.GlobalTag:
                #logMuon.info("Reading CSC I-Lines from layout - special configuration for COMP200 in HLT setup.")
                MuonAlign.ILinesFromCondDB = False
                detTool.UseIlinesFromGM = True
            else :
                #logMuon.info("Reading CSC I-Lines from conditions database.")
                if (flags.Common.isOnline and not flags.Input.isMC):                
                    acc.merge(addFolders( flags, ['/MUONALIGN/Onl/CSC/ILINES'], 'MUONALIGN', className='CondAttrListCollection'))
                else:
                    acc.merge(addFolders( flags, ['/MUONALIGN/CSC/ILINES'], 'MUONALIGN_OFL', className='CondAttrListCollection'))                
                    
                    MuonAlign.ParlineFolders += ["/MUONALIGN/CSC/ILINES"]
                    MuonAlign.ILinesFromCondDB = True
                    detTool.UseIlinesFromGM = False

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
            if (MuonGeometryFlags.hasSTGC() and MuonGeometryFlags.hasMM()):
                nswAGDDTool = NSWAGDDTool("NewSmallWheel", Locked=False)
                nswAGDDTool.Volumes = ["NewSmallWheel"]
                nswAGDDTool.DefaultDetector = "Muon"
                AGDD2Geo.Builders += [ nswAGDDTool ]
            acc.addService(AGDD2Geo)
    # call fill cache of MuonDetectorTool such that all MdtReadoutElement caches are filled
    # already during initialize() -> this will increase memory -> needs to be measured
    detTool.FillCacheInitTime = 1
    # turn on/off caching of MdtReadoutElement surfaces
    detTool.CachingFlag = 1

    gms.DetectorTools += [ detTool ]

    acc.addService( Muon__MuonIdHelperSvc("MuonIdHelperSvc",
        HasCSC=MuonGeometryFlags.hasCSC(),
        HasSTgc=MuonGeometryFlags.hasSTGC(),
        HasMM=MuonGeometryFlags.hasMM()
        ) )

    return acc
