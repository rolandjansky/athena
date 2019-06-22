# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AtlasGeoModel.GeoModelConfig import GeoModelCfg
from MuonGeoModel.MuonGeoModelConf import MuonDetectorTool
from MuonIdHelpers.MuonIdHelpersConf import Muon__MuonIdHelperTool
from AGDD2GeoSvc.AGDD2GeoSvcConf import AGDDtoGeoSvc
from MuonAGDD.MuonAGDDConf import MuonAGDDTool, NSWAGDDTool
from StoreGate.StoreGateConf import StoreGateSvc

def MuonGeoModelCfg(flags):
    acc = ComponentAccumulator()

    # get the GeoModelSvc and add MuonDetectorTool
    gmsAcc=GeoModelCfg(flags )
    gms=gmsAcc.getPrimary()
    acc.merge(gmsAcc)

    detTool = MuonDetectorTool()
    detTool.UseConditionDb = 1
    detTool.UseIlinesFromGM = 1
    detTool.BuildFromNova = 0
    if ( ( not flags.Detector.SimulateMuon or flags.Detector.OverlayMuon ) and flags.Common.Project is not "AthSimulation" ):
        # This is all migrated from MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonAlignConfig.py

        from IOVDbSvc.IOVDbSvcConfig import addFolders
        from MuonCondTool.MuonCondToolConf import MuonAlignmentDbTool
        MuonAlignmentDbTool = MuonAlignmentDbTool("MGM_AlignmentDbTool")
        MuonAlignmentDbTool.ParlineFolders = ["/MUONALIGN/MDT/BARREL",
                                              "/MUONALIGN/MDT/ENDCAP/SIDEA",
                                              "/MUONALIGN/MDT/ENDCAP/SIDEC",
                                              "/MUONALIGN/TGC/SIDEA",
                                              "/MUONALIGN/TGC/SIDEC"]
        acc.addPublicTool(MuonAlignmentDbTool)

        # Condition DB is needed only if A-lines or B-lines are requested
        if not (not flags.Muon.Align.UseALines and flags.Muon.Align.UseBLines=='none'):
            detTool.UseConditionDb = 1
            detTool.TheMuonAlignmentTool = MuonAlignmentDbTool
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
        if flags.IOVDb.DatabaseInstance == 'COMP200' and 'HLT' in flags.IOVDb.GlobalTag:
            #logMuon.info("Reading CSC I-Lines from layout - special configuration for COMP200 in HLT setup.")
            detTool.UseIlinesFromGM = True
            MuonAlignmentDbTool.ILinesFromCondDB = False
        else :
            #logMuon.info("Reading CSC I-Lines from conditions database.")
            if (flags.Common.isOnline and not flags.Input.isMC):                
                acc.merge(addFolders( flags, ['/MUONALIGN/Onl/CSC/ILINES'], 'MUONALIGN'))
            else:
                acc.merge(addFolders( flags, ['/MUONALIGN/CSC/ILINES'], 'MUONALIGN_OFL'))                

            MuonAlignmentDbTool.ParlineFolders += ["/MUONALIGN/CSC/ILINES"]
            detTool.UseIlinesFromGM = False
            MuonAlignmentDbTool.ILinesFromCondDB = True

        # here define if As-Built (MDT chamber alignment) are enabled
        if flags.Muon.Align.UseAsBuilt:
            if flags.IOVDb.DatabaseInstance == 'COMP200' or \
                    'HLT' in flags.IOVDb.GlobalTag or flags.Common.isOnline :
                #logMuon.info("No MDT As-Built parameters applied.")
                detTool.EnableMdtAsBuiltParameters = 0
            else :
                #logMuon.info("Reading As-Built parameters from conditions database")
                detTool.EnableMdtAsBuiltParameters = 1
                acc.merge(addFolders( flags, '/MUONALIGN/MDT/ASBUILTPARAMS', 'MUONALIGN_OFL'))
                MuonAlignmentDbTool.ParlineFolders += ["/MUONALIGN/MDT/ASBUILTPARAMS"]
                pass
    else:
        detTool.TheMuonAlignmentTool = ""
        detTool.UseConditionDb = 0
        detTool.UseAsciiConditionData = 0
        if flags.Detector.SimulateMuon:
            detTool.FillCacheInitTime = 0
            if flags.GeoModel.Run=="RUN3" or flags.GeoModel.Run=="RUN4":
                detTool.StationSelection  = 2
                detTool.SelectedStations  = [ "EIL1", "EIL2", "EIL6", "EIL7",
                                                       "EIS*", "EIL10", "EIL11", "EIL12",
                                                       "EIL17", "CSS*", "CSL*", "T4E*",
                                                       "T4F*" ]
            ## Additional material in the muon system
            AGDD2Geo = AGDDtoGeoSvc()
            muonAGDDTool = MuonAGDDTool("MuonSpectrometer", BuildNSW=False)
            AGDD2Geo.Builders += [ muonAGDDTool ]
            if flags.GeoModel.Run=="RUN3" or flags.GeoModel.Run=="RUN4":
                nswAGDDTool = NSWAGDDTool("NewSmallWheel", Locked=False)
                nswAGDDTool.Volumes = ["NewSmallWheel"]
                nswAGDDTool.DefaultDetector = "Muon"
                AGDD2Geo.Builders += [ nswAGDDTool ]
            acc.addService(AGDD2Geo)

    gms.DetectorTools += [ detTool ]

    # Temporary, until we move to services/private tools
    acc.addPublicTool( Muon__MuonIdHelperTool() )

    return acc
