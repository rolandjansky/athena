include.block( "MissingETPerformance/MissingETData_jobOptions.py" )

from MissingETPerformance.MissingETPerformanceConf import \
     MissingETData as ConfiguredMissingETData
MissingETData = ConfiguredMissingETData(

    #whether to retrieve the CaloCells or not
    #if True, need to make sure other options below are correct
    getCaloCells               = False,

    #MissingET Keys
    #To turn off the retrieval of a MissingET container, set value to ''
    #for example, METBase0Key                 = '',
    #to ignore all missing containers, set IgnoreMissingContainers = True
    IgnoreMissingContainers = True,

    METCryoKey                  = 'MET_Cryo',
    METDM_AllKey                = 'MET_DM_All',
    METDM_Crack1Key             = 'MET_DM_Crack1',
    METDM_Crack2Key             = 'MET_DM_Crack2',
    METDM_CryoKey               = 'MET_DM_Cryo',
    METFinalKey                 = 'MET_Final',
    METLocHadTopoObjKey         = 'MET_LocHadTopoObj',
    METTopoObjKey               = 'MET_TopoObj',
    METMuonBoy_SpectroKey       = 'MET_MuonBoy_Spectro',
    METMuonBoy_TrackKey         = 'MET_MuonBoy_Track',
    METMuonKey                  = 'MET_Muon',
    METMuonMuonsKey             = 'MET_MuonMuons',
    METBase0Key                 = 'MET_Base0',
    METTopoKey                  = 'MET_Topo',
    METLocHadTopoKey            = 'MET_LocHadTopo',
    METBaseKey                  = 'MET_Base',
    METTruthKey                 = 'MET_Truth',
    METCorrTopoKey              = 'MET_CorrTopo',
    METCalibKey                 = 'MET_Calib',
    METCryoConeKey              = 'MET_CryoCone',
    METRefFinalKey              = 'MET_RefFinal',
    METRefEleKey                = 'MET_RefEle',
    METRefGammaKey              = 'MET_RefGamma',
    METRefJetKey                = 'MET_RefJet',
    METSoftJetsKey              = 'MET_SoftJets',
    METRefMuonKey               = 'MET_RefMuon',
    METRefMuon_TrackKey         = 'MET_RefMuon_Track',
    METRefTauKey                = 'MET_RefTau',
    METMuonBoyKey               = 'MET_MuonBoy',
    METMuIDKey                  = 'MET_Muid',
    METCellOutKey               = 'MET_CellOut',
    METCellOutEFlowKey          = 'MET_CellOut_Eflow',
    METCompositionKey           = 'MET_RefComposition',
    AllCaloKey                  = 'AllCalo',

    #MissingET Trigger Keys
    METL1ROIKey                 = 'LVL1_ROI',
    METL2Key                    = 'HLT_T2MissingET',
    METEF_FEBHeaderKey          = 'HLT_TrigEFMissingET_FEB',
    METEF_noiseSuppKey          = 'HLT_TrigEFMissingET_noiseSupp',
    METEF_defaultKey            = 'HLT_TrigEFMissingET',

    UseCaloNoiseTool            = False,
    NSigmaCut                   = 2.0,
    UseBadChannelMasker         = False,
    UseBadChannelTool           = False,
    )

if MissingETData.getCaloCells == True:
    from AthenaCommon.GlobalFlags import globalflags
    #need to set DetDescrVersion to agree with the version of your file, can use "dumpVersionTags.py filename" to find out the right version
    #globalflags.DetDescrVersion = "ATLAS-GEO-08-00-00"
    globalflags.DetDescrVersion = "ATLAS-GEONTF-05-00-00"
    from AthenaCommon.GlobalFlags import GlobalFlags
    #need this set_data() uncommented for real data, commented for monte carlo
    GlobalFlags.DataSource.set_data()
    include( "RecExCond/AllDet_detDescr.py")

    if MissingETData.UseCaloNoiseTool == True:
        from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
        theCaloNoiseTool=CaloNoiseToolDefault()
        from AthenaCommon.AppMgr import ToolSvc 		        	   
        ToolSvc+=theCaloNoiseTool
        MissingETData.CaloNoiseTool = theCaloNoiseTool
        #theCaloNoiseTool.OutputLevel = 1

    if MissingETData.UseBadChannelTool == True:
        from LArBadChannelTool.LArBadChannelToolConf import LArBadChanTool
        theLArBadChanTool=LArBadChanTool("LArBadChanTool")
        theLArBadChanTool.OutputLevel=DEBUG
        #can put in bad channels by hand in text files by setting this true and specifying names of text files, see LArBadChanTool documentation
        theLArBadChanTool.ReadFromASCII=False
        #have to specify the these names iff ReadFromASCII=True
        #theLArBadChanTool.EMBAfile   = 'EMBA.txt'
        #theLArBadChanTool.EMBCfile   = 'EMBC.txt'
        #theLArBadChanTool.EMECAfile  = 'EMECA.txt'
        #theLArBadChanTool.EMECCfile  = 'EMECC.txt'
        #theLArBadChanTool.HECAfile   = 'HECA.txt'
        #theLArBadChanTool.HECCfile   = 'HECC.txt'
        #theLArBadChanTool.FCALAfile  = 'FCALA.txt'
        #theLArBadChanTool.FCALCfile  = 'FCALC.txt'
        #theLArBadChanTool.FEBfile    = 'FEB.txt'
        ToolSvc+=theLArBadChanTool
        MissingETData.BadChannelTool = theLArBadChanTool
    if MissingETData.UseBadChannelMasker == True:
        from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker 			           
        theLArMasker=LArBadChannelMasker("myLArMasker") 						           
        theLArMasker.DoMasking=True									           
        #set problems to be masked here, badchannelmasker and badchantool are independently applied
        theLArMasker.ProblemsToMask=["deadReadout","deadPhys","almostDead","short",
                                     "lowNoiseHG","highNoiseHG","unstableNoiseHG","lowNoiseMG","highNoiseMG","unstableNoiseMG",
                                     "lowNoiseLG","highNoiseLG","unstableNoiseLG","deadCalib","distorted","peculiarCalibrationLine",
                                     "unstable","sporadicBurstNoise","problematicForUnknownReason","shortProblem","missingFEB"
                                     ]
        ToolSvc+=theLArMasker
        MissingETData.BadChannelMasker = theLArMasker

ToolSvc += MissingETData
print      MissingETData
