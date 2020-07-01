
haas_mualign2=True
if haas_mualign2:
    #use muon alignments
    printfunc ("Haas: Reading muon alignment constants from DB")

    from AthenaCommon.AlgSequence import AthSequencer

    #from IOVDbSvc.CondDB import conddb
    #conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/BARREL','/MUONALIGN/MDT/BARREL',className='CondAttrListCollection')
    #conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/ENDCAP/SIDEA','/MUONALIGN/MDT/ENDCAP/SIDEA',className='CondAttrListCollection')
    #conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/ENDCAP/SIDEC','/MUONALIGN/MDT/ENDCAP/SIDEC',className='CondAttrListCollection')
    #conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/TGC/SIDEA','/MUONALIGN/TGC/SIDEA',className='CondAttrListCollection')
    #conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/TGC/SIDEC','/MUONALIGN/TGC/SIDEC',className='CondAttrListCollection')

    from AtlasGeoModel.MuonGM import GeoModelSvc
    MuonDetectorTool = GeoModelSvc.DetectorTools[ "MuonDetectorTool" ]
    MuonDetectorTool.UseConditionDb = 1
    MuonDetectorTool.OutputLevel=DEBUG
    
    MuonDetectorTool.EnableFineClashFixing = 1 #this should be on for g4?
    printfunc (MuonDetectorTool)

    #register callbacks for alignments, to get IOVs?
    #GeoModelSvc.AlignCallbacks = True
    
    from MuonCondAlg.MuonCondAlgConf import MuonAlignmentCondAlg
    condSequence = AthSequencer("AthCondSeq")
    if conddb.dbdata != 'COMP200' and conddb.dbmc != 'COMP200' and \
       'HLT' not in globalflags.ConditionsTag() and not conddb.isOnline :

        condSequence+=MuonAlignmentCondAlg('MuonAlignmentCondAlg')
        MuonAlignmentCondAlg.ParlineFolders = ["/MUONALIGN/MDT/BARREL",
                                               "/MUONALIGN/MDT/ENDCAP/SIDEA",
                                               "/MUONALIGN/MDT/ENDCAP/SIDEC",
                                               "/MUONALIGN/TGC/SIDEA",
                                               "/MUONALIGN/TGC/SIDEC"]
        MuonAlignmentCondAlg.ILinesFromCondDB = False
        MuonAlignmentCondAlg.DumpALines = False
        MuonAlignmentCondAlg.DumpBLines = False
        MuonAlignmentCondAlg.DumpILines = False
    
