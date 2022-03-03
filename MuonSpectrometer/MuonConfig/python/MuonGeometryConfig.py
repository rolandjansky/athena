# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import ProductionStep
from AtlasGeoModel.GeoModelConfig import GeoModelCfg

def MuonIdHelperSvcCfg(flags):
    acc = ComponentAccumulator()
    acc.addService( CompFactory.Muon.MuonIdHelperSvc("MuonIdHelperSvc",
        HasCSC=flags.Detector.GeometryCSC,
        HasSTgc=flags.Detector.GeometrysTGC,
        HasMM=flags.Detector.GeometryMM
        ) )
    return acc


def MuonDetectorToolCfg(flags):
    acc = ComponentAccumulator()
    detTool = CompFactory.MuonDetectorTool(
        HasCSC=flags.Detector.GeometryCSC,
        HasSTgc=flags.Detector.GeometrysTGC,
        HasMM=flags.Detector.GeometryMM
        )
    detTool.UseConditionDb = 1
    detTool.UseIlinesFromGM = 1

    # temporary way to pass MM correction for edge passivation
    from MuonGeoModel.MMPassivationFlag import MMPassivationFlag
    detTool.passivationWidthMM = MMPassivationFlag.correction

    if flags.Muon.enableAlignment:
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
        if flags.Muon.Align.UseILines and flags.Detector.GeometryCSC:
            if 'HLT' in flags.IOVDb.GlobalTag:
                #logMuon.info("Reading CSC I-Lines from layout - special configuration for COMP200 in HLT setup.")
                detTool.UseIlinesFromGM = True
                detTool.EnableCscInternalAlignment = False
            else :
                #logMuon.info("Reading CSC I-Lines from conditions database.")
                if (flags.Common.isOnline and not flags.Input.isMC):
                    detTool.EnableCscInternalAlignment = True
                else:
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
                pass

    else:
        detTool.UseConditionDb = 0
        detTool.UseAsciiConditionData = 0
        if flags.Common.ProductionStep == ProductionStep.Simulation:
            detTool.FillCacheInitTime = 0

    ## Additional material in the muon system
    AGDD2Geo = CompFactory.AGDDtoGeoSvc()
    muonAGDDTool = CompFactory.MuonAGDDTool("MuonSpectrometer", BuildNSW=False)
    AGDD2Geo.Builders += [ muonAGDDTool ]
    if (flags.Detector.GeometrysTGC and flags.Detector.GeometryMM):
        nswAGDDTool = CompFactory.NSWAGDDTool("NewSmallWheel", Locked=False)
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

    acc.setPrivateTools(detTool)
    return acc


def MuonAlignmentCondAlgCfg(flags):
    acc = MuonGeoModelToolCfg(flags)
    acc.merge(MuonIdHelperSvcCfg(flags))

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

    # here define if I-lines (CSC internal alignment) are enabled
    if flags.Muon.Align.UseILines:
        if 'HLT' in flags.IOVDb.GlobalTag:
            #logMuon.info("Reading CSC I-Lines from layout - special configuration for COMP200 in HLT setup.")
            MuonAlign.ILinesFromCondDB = False
        else :
            #logMuon.info("Reading CSC I-Lines from conditions database.")
            if (flags.Common.isOnline and not flags.Input.isMC):
                acc.merge(addFolders( flags, ['/MUONALIGN/Onl/CSC/ILINES'], 'MUONALIGN', className='CondAttrListCollection'))
            else:
                acc.merge(addFolders( flags, ['/MUONALIGN/CSC/ILINES'], 'MUONALIGN_OFL', className='CondAttrListCollection'))
                MuonAlign.ParlineFolders += ["/MUONALIGN/CSC/ILINES"]
                MuonAlign.ILinesFromCondDB = True

    # here define if As-Built (MDT chamber alignment) are enabled
    if flags.Muon.Align.UseAsBuilt:
        if flags.IOVDb.DatabaseInstance == 'COMP200' or \
                'HLT' in flags.IOVDb.GlobalTag or flags.Common.isOnline :
            #logMuon.info("No MDT As-Built parameters applied.")
            pass
        else :
            #logMuon.info("Reading As-Built parameters from conditions database")
            acc.merge(addFolders( flags, '/MUONALIGN/MDT/ASBUILTPARAMS', 'MUONALIGN_OFL', className='CondAttrListCollection'))
            MuonAlign.ParlineFolders += ["/MUONALIGN/MDT/ASBUILTPARAMS"]
            pass

    MuonAlign.DoCSCs = flags.Detector.GeometryCSC
    acc.addCondAlgo(MuonAlign)

    if flags.IOVDb.DatabaseInstance != 'COMP200' and \
                'HLT' not in flags.IOVDb.GlobalTag and not flags.Common.isOnline:
        acc.merge(addFolders( flags, '/MUONALIGN/ERRS', 'MUONALIGN_OFL', className='CondAttrListCollection'))
        acc.addCondAlgo(CompFactory.MuonAlignmentErrorDbAlg("MuonAlignmentErrorDbAlg"))
    
    return acc

def MuonDetectorCondAlgCfg(flags):
    acc = MuonAlignmentCondAlgCfg(flags)
    MuonDetectorCondAlg = CompFactory.MuonDetectorCondAlg
    MuonDetectorManagerCond = MuonDetectorCondAlg()
    detTool = acc.popToolsAndMerge(MuonDetectorToolCfg(flags))
    MuonDetectorManagerCond.MuonDetectorTool = detTool
    acc.addCondAlgo(MuonDetectorManagerCond)
    return acc


def MuonGeoModelToolCfg(flags):
    acc = GeoModelCfg(flags)
    gms = acc.getPrimary()
    detTool = acc.popToolsAndMerge(MuonDetectorToolCfg(flags))
    detTool.FillCacheInitTime = 0 # We do not need to fill cache for the MuonGeoModel MuonDetectorTool, just for the condAlg
    gms.DetectorTools += [ detTool ]
    return acc


def MuonGeoModelCfg(flags, forceDisableAlignment=False):
    acc = MuonGeoModelToolCfg(flags)

    if flags.Muon.enableAlignment and not forceDisableAlignment:
        acc.merge(MuonDetectorCondAlgCfg(flags))

    acc.merge(MuonIdHelperSvcCfg(flags)) # This line can be removed once the configuration methods for all 258 components which directly use this service are updated!!
    return acc
