include.block( "MissingETPerformance/ContainerComparatorTool_jobOptions.py" )

#should match suffix names used in naming new storegate keys
suffix_list = ['_antikt', '_antikt_inversion','_antikt6', '_antikt6_inversion', '_cone4_inversion', '_cone7', '_cone7_inversion']

from MissingETPerformance.MissingETPerformanceConf import \
     ContainerComparatorTool as ConfiguredContainerComparatorTool
ContainerComparatorTool = ConfiguredContainerComparatorTool(

#list pairs of containers by storegate names to make comparison plots
    containers_to_compare = [
                  ["MET_MuonBoy"],
                  ["MET_RefFinal"],
                  ["MET_RefMuon"],
                  ["MET_LocHadTopoObj"],
                  ["MET_TopoObj"],
                  ["MET_MuonBoy_Spectro"],
                  ["MET_MuonBoy_Track"],
                  ["MET_Muon"],
                  ["MET_Base"],
                  ["MET_Topo"],
                  ["MET_LocHadTopo"],
                  ["MET_Base0"],
                  ["MET_CorrTopo"],
                  ["MET_Calib"],
                  ["MET_CryoCone"],
                  ["MET_RefEle"],
                  ["MET_RefGamma"],
                  ["MET_RefJet"],
                  ["MET_RefMuon_Track"],
                  ["MET_RefTau"],
                  ["MET_CellOut"],
                            ]
    )

containers_to_compare_loop = ContainerComparatorTool.containers_to_compare

containers_to_compare_new = []
for item in containers_to_compare_loop:
    newentry = []
    newentry += item
    for item2 in suffix_list:
        newentry += [item[0]+item2]
    containers_to_compare_new += [newentry]

ContainerComparatorTool.containers_to_compare = containers_to_compare_new


from MissingETPerformance.MissingETPerformanceConf import MissingETData as ConfiguredMissingETData

#############################################################
##### cone7 ######
#############################################################
MissingETData_cone7 = ConfiguredMissingETData("MissingETData_cone7");

MissingETData_cone7.getCaloCells               = False
MissingETData_cone7.IgnoreMissingContainers = True

suffix_cone7 = '_cone7'

MissingETData_cone7.METCryoKey                  = 'MET_Cryo' + suffix_cone7
MissingETData_cone7.METDM_AllKey                = ''
MissingETData_cone7.METDM_Crack1Key             = ''
MissingETData_cone7.METDM_Crack2Key             = ''
MissingETData_cone7.METDM_CryoKey               = ''
MissingETData_cone7.METFinalKey                 = 'MET_Final' + suffix_cone7
MissingETData_cone7.METLocHadTopoObjKey         = 'MET_LocHadTopoObj' + suffix_cone7
MissingETData_cone7.METTopoObjKey               = 'MET_TopoObj' + suffix_cone7
MissingETData_cone7.METMuonBoy_SpectroKey       = 'MET_MuonBoy_Spectro' + suffix_cone7
MissingETData_cone7.METMuonBoy_TrackKey         = 'MET_MuonBoy_Track' + suffix_cone7
MissingETData_cone7.METMuonKey                  = 'MET_Muon' + suffix_cone7
MissingETData_cone7.METBase0Key                 = 'MET_Base0' + suffix_cone7
MissingETData_cone7.METTopoKey                  = 'MET_Topo' + suffix_cone7
MissingETData_cone7.METLocHadTopoKey            = 'MET_LocHadTopo' + suffix_cone7
MissingETData_cone7.METBaseKey                  = 'MET_Base' + suffix_cone7
MissingETData_cone7.METTruthKey                 = 'MET_Truth'
MissingETData_cone7.METCorrTopoKey              = 'MET_CorrTopo' + suffix_cone7
MissingETData_cone7.METCalibKey                 = 'MET_Calib' + suffix_cone7
MissingETData_cone7.METCryoConeKey              = 'MET_CryoCone' + suffix_cone7
MissingETData_cone7.METRefFinalKey              = 'MET_RefFinal' + suffix_cone7
MissingETData_cone7.METRefEleKey                = 'MET_RefEle' + suffix_cone7
MissingETData_cone7.METRefGammaKey              = 'MET_RefGamma' + suffix_cone7
MissingETData_cone7.METRefJetKey                = 'MET_RefJet' + suffix_cone7
MissingETData_cone7.METRefMuonKey               = 'MET_RefMuon' + suffix_cone7
MissingETData_cone7.METRefMuon_TrackKey         = 'MET_RefMuon_Track' + suffix_cone7
MissingETData_cone7.METRefTauKey                = 'MET_RefTau' + suffix_cone7
MissingETData_cone7.METMuonBoyKey               = 'MET_MuonBoy' + suffix_cone7
MissingETData_cone7.METCellOutKey               = 'MET_CellOut' + suffix_cone7

MissingETData_cone7.AllCaloKey                  = 'AllCalo'
MissingETData_cone7.METL1ROIKey                 = 'LVL1_ROI'
MissingETData_cone7.METL2Key                    = 'HLT_T2MissingET'
MissingETData_cone7.METEF_FEBHeaderKey          = 'HLT_TrigEFMissingET_FEB'
MissingETData_cone7.METEF_noiseSuppKey          = 'HLT_TrigEFMissingET_noiseSupp'
MissingETData_cone7.METEF_defaultKey            = 'HLT_TrigEFMissingET'
MissingETData_cone7.UseCaloNoiseTool            = False
MissingETData_cone7.NSigmaCut                   = 2.0
MissingETData_cone7.UseBadChannelMasker         = False
MissingETData_cone7.UseBadChannelTool           = False

ToolSvc += MissingETData_cone7
print      MissingETData_cone7


#############################################################
##### cone7_inversion ######
#############################################################
MissingETData_cone7_inversion = ConfiguredMissingETData("MissingETData_cone7_inversion");

MissingETData_cone7_inversion.getCaloCells               = False
MissingETData_cone7_inversion.IgnoreMissingContainers = True

suffix_cone7_inversion = '_cone7_inversion'

MissingETData_cone7_inversion.METCryoKey                  = 'MET_Cryo' + suffix_cone7_inversion
MissingETData_cone7_inversion.METDM_AllKey                = ''
MissingETData_cone7_inversion.METDM_Crack1Key             = ''
MissingETData_cone7_inversion.METDM_Crack2Key             = ''
MissingETData_cone7_inversion.METDM_CryoKey               = ''
MissingETData_cone7_inversion.METFinalKey                 = 'MET_Final' + suffix_cone7_inversion
MissingETData_cone7_inversion.METLocHadTopoObjKey         = 'MET_LocHadTopoObj' + suffix_cone7_inversion
MissingETData_cone7_inversion.METTopoObjKey               = 'MET_TopoObj' + suffix_cone7_inversion
MissingETData_cone7_inversion.METMuonBoy_SpectroKey       = 'MET_MuonBoy_Spectro' + suffix_cone7_inversion
MissingETData_cone7_inversion.METMuonBoy_TrackKey         = 'MET_MuonBoy_Track' + suffix_cone7_inversion
MissingETData_cone7_inversion.METMuonKey                  = 'MET_Muon' + suffix_cone7_inversion
MissingETData_cone7_inversion.METBase0Key                 = 'MET_Base0' + suffix_cone7_inversion
MissingETData_cone7_inversion.METTopoKey                  = 'MET_Topo' + suffix_cone7_inversion
MissingETData_cone7_inversion.METLocHadTopoKey            = 'MET_LocHadTopo' + suffix_cone7_inversion
MissingETData_cone7_inversion.METBaseKey                  = 'MET_Base' + suffix_cone7_inversion
MissingETData_cone7_inversion.METTruthKey                 = 'MET_Truth'
MissingETData_cone7_inversion.METCorrTopoKey              = 'MET_CorrTopo' + suffix_cone7_inversion
MissingETData_cone7_inversion.METCalibKey                 = 'MET_Calib' + suffix_cone7_inversion
MissingETData_cone7_inversion.METCryoConeKey              = 'MET_CryoCone' + suffix_cone7_inversion
MissingETData_cone7_inversion.METRefFinalKey              = 'MET_RefFinal' + suffix_cone7_inversion
MissingETData_cone7_inversion.METRefEleKey                = 'MET_RefEle' + suffix_cone7_inversion
MissingETData_cone7_inversion.METRefGammaKey              = 'MET_RefGamma' + suffix_cone7_inversion
MissingETData_cone7_inversion.METRefJetKey                = 'MET_RefJet' + suffix_cone7_inversion
MissingETData_cone7_inversion.METRefMuonKey               = 'MET_RefMuon' + suffix_cone7_inversion
MissingETData_cone7_inversion.METRefMuon_TrackKey         = 'MET_RefMuon_Track' + suffix_cone7_inversion
MissingETData_cone7_inversion.METRefTauKey                = 'MET_RefTau' + suffix_cone7_inversion
MissingETData_cone7_inversion.METMuonBoyKey               = 'MET_MuonBoy' + suffix_cone7_inversion
MissingETData_cone7_inversion.METCellOutKey               = 'MET_CellOut' + suffix_cone7_inversion

MissingETData_cone7_inversion.AllCaloKey                  = 'AllCalo'
MissingETData_cone7_inversion.METL1ROIKey                 = 'LVL1_ROI'
MissingETData_cone7_inversion.METL2Key                    = 'HLT_T2MissingET'
MissingETData_cone7_inversion.METEF_FEBHeaderKey          = 'HLT_TrigEFMissingET_FEB'
MissingETData_cone7_inversion.METEF_noiseSuppKey          = 'HLT_TrigEFMissingET_noiseSupp'
MissingETData_cone7_inversion.METEF_defaultKey            = 'HLT_TrigEFMissingET'
MissingETData_cone7_inversion.UseCaloNoiseTool            = False
MissingETData_cone7_inversion.NSigmaCut                   = 2.0
MissingETData_cone7_inversion.UseBadChannelMasker         = False
MissingETData_cone7_inversion.UseBadChannelTool           = False

ToolSvc += MissingETData_cone7_inversion
print      MissingETData_cone7_inversion

#############################################################
##### cone4_inversion ######
#############################################################
MissingETData_cone4_inversion = ConfiguredMissingETData("MissingETData_cone4_inversion");

MissingETData_cone4_inversion.getCaloCells               = False
MissingETData_cone4_inversion.IgnoreMissingContainers = True

suffix_cone4_inversion = '_cone4_inversion'

MissingETData_cone4_inversion.METCryoKey                  = 'MET_Cryo' + suffix_cone4_inversion
MissingETData_cone4_inversion.METDM_AllKey                = ''
MissingETData_cone4_inversion.METDM_Crack1Key             = ''
MissingETData_cone4_inversion.METDM_Crack2Key             = ''
MissingETData_cone4_inversion.METDM_CryoKey               = ''
MissingETData_cone4_inversion.METFinalKey                 = 'MET_Final' + suffix_cone4_inversion
MissingETData_cone4_inversion.METLocHadTopoObjKey         = 'MET_LocHadTopoObj' + suffix_cone4_inversion
MissingETData_cone4_inversion.METTopoObjKey               = 'MET_TopoObj' + suffix_cone4_inversion
MissingETData_cone4_inversion.METMuonBoy_SpectroKey       = 'MET_MuonBoy_Spectro' + suffix_cone4_inversion
MissingETData_cone4_inversion.METMuonBoy_TrackKey         = 'MET_MuonBoy_Track' + suffix_cone4_inversion
MissingETData_cone4_inversion.METMuonKey                  = 'MET_Muon' + suffix_cone4_inversion
MissingETData_cone4_inversion.METBase0Key                 = 'MET_Base0' + suffix_cone4_inversion
MissingETData_cone4_inversion.METTopoKey                  = 'MET_Topo' + suffix_cone4_inversion
MissingETData_cone4_inversion.METLocHadTopoKey            = 'MET_LocHadTopo' + suffix_cone4_inversion
MissingETData_cone4_inversion.METBaseKey                  = 'MET_Base' + suffix_cone4_inversion
MissingETData_cone4_inversion.METTruthKey                 = 'MET_Truth'
MissingETData_cone4_inversion.METCorrTopoKey              = 'MET_CorrTopo' + suffix_cone4_inversion
MissingETData_cone4_inversion.METCalibKey                 = 'MET_Calib' + suffix_cone4_inversion
MissingETData_cone4_inversion.METCryoConeKey              = 'MET_CryoCone' + suffix_cone4_inversion
MissingETData_cone4_inversion.METRefFinalKey              = 'MET_RefFinal' + suffix_cone4_inversion
MissingETData_cone4_inversion.METRefEleKey                = 'MET_RefEle' + suffix_cone4_inversion
MissingETData_cone4_inversion.METRefGammaKey              = 'MET_RefGamma' + suffix_cone4_inversion
MissingETData_cone4_inversion.METRefJetKey                = 'MET_RefJet' + suffix_cone4_inversion
MissingETData_cone4_inversion.METRefMuonKey               = 'MET_RefMuon' + suffix_cone4_inversion
MissingETData_cone4_inversion.METRefMuon_TrackKey         = 'MET_RefMuon_Track' + suffix_cone4_inversion
MissingETData_cone4_inversion.METRefTauKey                = 'MET_RefTau' + suffix_cone4_inversion
MissingETData_cone4_inversion.METMuonBoyKey               = 'MET_MuonBoy' + suffix_cone4_inversion
MissingETData_cone4_inversion.METCellOutKey               = 'MET_CellOut' + suffix_cone4_inversion

MissingETData_cone4_inversion.AllCaloKey                  = 'AllCalo'
MissingETData_cone4_inversion.METL1ROIKey                 = 'LVL1_ROI'
MissingETData_cone4_inversion.METL2Key                    = 'HLT_T2MissingET'
MissingETData_cone4_inversion.METEF_FEBHeaderKey          = 'HLT_TrigEFMissingET_FEB'
MissingETData_cone4_inversion.METEF_noiseSuppKey          = 'HLT_TrigEFMissingET_noiseSupp'
MissingETData_cone4_inversion.METEF_defaultKey            = 'HLT_TrigEFMissingET'
MissingETData_cone4_inversion.UseCaloNoiseTool            = False
MissingETData_cone4_inversion.NSigmaCut                   = 2.0
MissingETData_cone4_inversion.UseBadChannelMasker         = False
MissingETData_cone4_inversion.UseBadChannelTool           = False

ToolSvc += MissingETData_cone4_inversion
print      MissingETData_cone4_inversion

#############################################################
##### antikt ######
#############################################################
MissingETData_antikt = ConfiguredMissingETData("MissingETData_antikt");

MissingETData_antikt.getCaloCells               = False
MissingETData_antikt.IgnoreMissingContainers = True

suffix_antikt = '_antikt'

MissingETData_antikt.METCryoKey                  = 'MET_Cryo' + suffix_antikt
MissingETData_antikt.METDM_AllKey                = ''
MissingETData_antikt.METDM_Crack1Key             = ''
MissingETData_antikt.METDM_Crack2Key             = ''
MissingETData_antikt.METDM_CryoKey               = ''
MissingETData_antikt.METFinalKey                 = 'MET_Final' + suffix_antikt
MissingETData_antikt.METLocHadTopoObjKey         = 'MET_LocHadTopoObj' + suffix_antikt
MissingETData_antikt.METTopoObjKey               = 'MET_TopoObj' + suffix_antikt
MissingETData_antikt.METMuonBoy_SpectroKey       = 'MET_MuonBoy_Spectro' + suffix_antikt
MissingETData_antikt.METMuonBoy_TrackKey         = 'MET_MuonBoy_Track' + suffix_antikt
MissingETData_antikt.METMuonKey                  = 'MET_Muon' + suffix_antikt
MissingETData_antikt.METBase0Key                 = 'MET_Base0' + suffix_antikt
MissingETData_antikt.METTopoKey                  = 'MET_Topo' + suffix_antikt
MissingETData_antikt.METLocHadTopoKey            = 'MET_LocHadTopo' + suffix_antikt
MissingETData_antikt.METBaseKey                  = 'MET_Base' + suffix_antikt
MissingETData_antikt.METTruthKey                 = 'MET_Truth'
MissingETData_antikt.METCorrTopoKey              = 'MET_CorrTopo' + suffix_antikt
MissingETData_antikt.METCalibKey                 = 'MET_Calib' + suffix_antikt
MissingETData_antikt.METCryoConeKey              = 'MET_CryoCone' + suffix_antikt
MissingETData_antikt.METRefFinalKey              = 'MET_RefFinal' + suffix_antikt
MissingETData_antikt.METRefEleKey                = 'MET_RefEle' + suffix_antikt
MissingETData_antikt.METRefGammaKey              = 'MET_RefGamma' + suffix_antikt
MissingETData_antikt.METRefJetKey                = 'MET_RefJet' + suffix_antikt
MissingETData_antikt.METRefMuonKey               = 'MET_RefMuon' + suffix_antikt
MissingETData_antikt.METRefMuon_TrackKey         = 'MET_RefMuon_Track' + suffix_antikt
MissingETData_antikt.METRefTauKey                = 'MET_RefTau' + suffix_antikt
MissingETData_antikt.METMuonBoyKey               = 'MET_MuonBoy' + suffix_antikt
MissingETData_antikt.METCellOutKey               = 'MET_CellOut' + suffix_antikt

MissingETData_antikt.AllCaloKey                  = 'AllCalo'
MissingETData_antikt.METL1ROIKey                 = 'LVL1_ROI'
MissingETData_antikt.METL2Key                    = 'HLT_T2MissingET'
MissingETData_antikt.METEF_FEBHeaderKey          = 'HLT_TrigEFMissingET_FEB'
MissingETData_antikt.METEF_noiseSuppKey          = 'HLT_TrigEFMissingET_noiseSupp'
MissingETData_antikt.METEF_defaultKey            = 'HLT_TrigEFMissingET'
MissingETData_antikt.UseCaloNoiseTool            = False
MissingETData_antikt.NSigmaCut                   = 2.0
MissingETData_antikt.UseBadChannelMasker         = False
MissingETData_antikt.UseBadChannelTool           = False

ToolSvc += MissingETData_antikt
print      MissingETData_antikt


#############################################################
##### antikt_inversion ######
#############################################################
MissingETData_antikt_inversion = ConfiguredMissingETData("MissingETData_antikt_inversion");

MissingETData_antikt_inversion.getCaloCells               = False
MissingETData_antikt_inversion.IgnoreMissingContainers = True

suffix_antikt_inversion = '_antikt_inversion'

MissingETData_antikt_inversion.METCryoKey                  = 'MET_Cryo' + suffix_antikt_inversion
MissingETData_antikt_inversion.METDM_AllKey                = ''
MissingETData_antikt_inversion.METDM_Crack1Key             = ''
MissingETData_antikt_inversion.METDM_Crack2Key             = ''
MissingETData_antikt_inversion.METDM_CryoKey               = ''
MissingETData_antikt_inversion.METFinalKey                 = 'MET_Final' + suffix_antikt_inversion
MissingETData_antikt_inversion.METLocHadTopoObjKey         = 'MET_LocHadTopoObj' + suffix_antikt_inversion
MissingETData_antikt_inversion.METTopoObjKey               = 'MET_TopoObj' + suffix_antikt_inversion
MissingETData_antikt_inversion.METMuonBoy_SpectroKey       = 'MET_MuonBoy_Spectro' + suffix_antikt_inversion
MissingETData_antikt_inversion.METMuonBoy_TrackKey         = 'MET_MuonBoy_Track' + suffix_antikt_inversion
MissingETData_antikt_inversion.METMuonKey                  = 'MET_Muon' + suffix_antikt_inversion
MissingETData_antikt_inversion.METBase0Key                 = 'MET_Base0' + suffix_antikt_inversion
MissingETData_antikt_inversion.METTopoKey                  = 'MET_Topo' + suffix_antikt_inversion
MissingETData_antikt_inversion.METLocHadTopoKey            = 'MET_LocHadTopo' + suffix_antikt_inversion
MissingETData_antikt_inversion.METBaseKey                  = 'MET_Base' + suffix_antikt_inversion
MissingETData_antikt_inversion.METTruthKey                 = 'MET_Truth'
MissingETData_antikt_inversion.METCorrTopoKey              = 'MET_CorrTopo' + suffix_antikt_inversion
MissingETData_antikt_inversion.METCalibKey                 = 'MET_Calib' + suffix_antikt_inversion
MissingETData_antikt_inversion.METCryoConeKey              = 'MET_CryoCone' + suffix_antikt_inversion
MissingETData_antikt_inversion.METRefFinalKey              = 'MET_RefFinal' + suffix_antikt_inversion
MissingETData_antikt_inversion.METRefEleKey                = 'MET_RefEle' + suffix_antikt_inversion
MissingETData_antikt_inversion.METRefGammaKey              = 'MET_RefGamma' + suffix_antikt_inversion
MissingETData_antikt_inversion.METRefJetKey                = 'MET_RefJet' + suffix_antikt_inversion
MissingETData_antikt_inversion.METRefMuonKey               = 'MET_RefMuon' + suffix_antikt_inversion
MissingETData_antikt_inversion.METRefMuon_TrackKey         = 'MET_RefMuon_Track' + suffix_antikt_inversion
MissingETData_antikt_inversion.METRefTauKey                = 'MET_RefTau' + suffix_antikt_inversion
MissingETData_antikt_inversion.METMuonBoyKey               = 'MET_MuonBoy' + suffix_antikt_inversion
MissingETData_antikt_inversion.METCellOutKey               = 'MET_CellOut' + suffix_antikt_inversion

MissingETData_antikt_inversion.AllCaloKey                  = 'AllCalo'
MissingETData_antikt_inversion.METL1ROIKey                 = 'LVL1_ROI'
MissingETData_antikt_inversion.METL2Key                    = 'HLT_T2MissingET'
MissingETData_antikt_inversion.METEF_FEBHeaderKey          = 'HLT_TrigEFMissingET_FEB'
MissingETData_antikt_inversion.METEF_noiseSuppKey          = 'HLT_TrigEFMissingET_noiseSupp'
MissingETData_antikt_inversion.METEF_defaultKey            = 'HLT_TrigEFMissingET'
MissingETData_antikt_inversion.UseCaloNoiseTool            = False
MissingETData_antikt_inversion.NSigmaCut                   = 2.0
MissingETData_antikt_inversion.UseBadChannelMasker         = False
MissingETData_antikt_inversion.UseBadChannelTool           = False

ToolSvc += MissingETData_antikt_inversion
print      MissingETData_antikt_inversion

#############################################################
##### antikt6 ######
#############################################################
MissingETData_antikt6 = ConfiguredMissingETData("MissingETData_antikt6");

MissingETData_antikt6.getCaloCells               = False
MissingETData_antikt6.IgnoreMissingContainers = True

suffix_antikt6 = '_antikt6'

MissingETData_antikt6.METCryoKey                  = 'MET_Cryo' + suffix_antikt6
MissingETData_antikt6.METDM_AllKey                = ''
MissingETData_antikt6.METDM_Crack1Key             = ''
MissingETData_antikt6.METDM_Crack2Key             = ''
MissingETData_antikt6.METDM_CryoKey               = ''
MissingETData_antikt6.METFinalKey                 = 'MET_Final' + suffix_antikt6
MissingETData_antikt6.METLocHadTopoObjKey         = 'MET_LocHadTopoObj' + suffix_antikt6
MissingETData_antikt6.METTopoObjKey               = 'MET_TopoObj' + suffix_antikt6
MissingETData_antikt6.METMuonBoy_SpectroKey       = 'MET_MuonBoy_Spectro' + suffix_antikt6
MissingETData_antikt6.METMuonBoy_TrackKey         = 'MET_MuonBoy_Track' + suffix_antikt6
MissingETData_antikt6.METMuonKey                  = 'MET_Muon' + suffix_antikt6
MissingETData_antikt6.METBase0Key                 = 'MET_Base0' + suffix_antikt6
MissingETData_antikt6.METTopoKey                  = 'MET_Topo' + suffix_antikt6
MissingETData_antikt6.METLocHadTopoKey            = 'MET_LocHadTopo' + suffix_antikt6
MissingETData_antikt6.METBaseKey                  = 'MET_Base' + suffix_antikt6
MissingETData_antikt6.METTruthKey                 = 'MET_Truth'
MissingETData_antikt6.METCorrTopoKey              = 'MET_CorrTopo' + suffix_antikt6
MissingETData_antikt6.METCalibKey                 = 'MET_Calib' + suffix_antikt6
MissingETData_antikt6.METCryoConeKey              = 'MET_CryoCone' + suffix_antikt6
MissingETData_antikt6.METRefFinalKey              = 'MET_RefFinal' + suffix_antikt6
MissingETData_antikt6.METRefEleKey                = 'MET_RefEle' + suffix_antikt6
MissingETData_antikt6.METRefGammaKey              = 'MET_RefGamma' + suffix_antikt6
MissingETData_antikt6.METRefJetKey                = 'MET_RefJet' + suffix_antikt6
MissingETData_antikt6.METRefMuonKey               = 'MET_RefMuon' + suffix_antikt6
MissingETData_antikt6.METRefMuon_TrackKey         = 'MET_RefMuon_Track' + suffix_antikt6
MissingETData_antikt6.METRefTauKey                = 'MET_RefTau' + suffix_antikt6
MissingETData_antikt6.METMuonBoyKey               = 'MET_MuonBoy' + suffix_antikt6
MissingETData_antikt6.METCellOutKey               = 'MET_CellOut' + suffix_antikt6

MissingETData_antikt6.AllCaloKey                  = 'AllCalo'
MissingETData_antikt6.METL1ROIKey                 = 'LVL1_ROI'
MissingETData_antikt6.METL2Key                    = 'HLT_T2MissingET'
MissingETData_antikt6.METEF_FEBHeaderKey          = 'HLT_TrigEFMissingET_FEB'
MissingETData_antikt6.METEF_noiseSuppKey          = 'HLT_TrigEFMissingET_noiseSupp'
MissingETData_antikt6.METEF_defaultKey            = 'HLT_TrigEFMissingET'
MissingETData_antikt6.UseCaloNoiseTool            = False
MissingETData_antikt6.NSigmaCut                   = 2.0
MissingETData_antikt6.UseBadChannelMasker         = False
MissingETData_antikt6.UseBadChannelTool           = False

ToolSvc += MissingETData_antikt6
print      MissingETData_antikt6


#############################################################
##### antikt6_inversion ######
#############################################################
MissingETData_antikt6_inversion = ConfiguredMissingETData("MissingETData_antikt6_inversion");

MissingETData_antikt6_inversion.getCaloCells               = False
MissingETData_antikt6_inversion.IgnoreMissingContainers = True

suffix_antikt6_inversion = '_antikt6_inversion'

MissingETData_antikt6_inversion.METCryoKey                  = 'MET_Cryo' + suffix_antikt6_inversion
MissingETData_antikt6_inversion.METDM_AllKey                = ''
MissingETData_antikt6_inversion.METDM_Crack1Key             = ''
MissingETData_antikt6_inversion.METDM_Crack2Key             = ''
MissingETData_antikt6_inversion.METDM_CryoKey               = ''
MissingETData_antikt6_inversion.METFinalKey                 = 'MET_Final' + suffix_antikt6_inversion
MissingETData_antikt6_inversion.METLocHadTopoObjKey         = 'MET_LocHadTopoObj' + suffix_antikt6_inversion
MissingETData_antikt6_inversion.METTopoObjKey               = 'MET_TopoObj' + suffix_antikt6_inversion
MissingETData_antikt6_inversion.METMuonBoy_SpectroKey       = 'MET_MuonBoy_Spectro' + suffix_antikt6_inversion
MissingETData_antikt6_inversion.METMuonBoy_TrackKey         = 'MET_MuonBoy_Track' + suffix_antikt6_inversion
MissingETData_antikt6_inversion.METMuonKey                  = 'MET_Muon' + suffix_antikt6_inversion
MissingETData_antikt6_inversion.METBase0Key                 = 'MET_Base0' + suffix_antikt6_inversion
MissingETData_antikt6_inversion.METTopoKey                  = 'MET_Topo' + suffix_antikt6_inversion
MissingETData_antikt6_inversion.METLocHadTopoKey            = 'MET_LocHadTopo' + suffix_antikt6_inversion
MissingETData_antikt6_inversion.METBaseKey                  = 'MET_Base' + suffix_antikt6_inversion
MissingETData_antikt6_inversion.METTruthKey                 = 'MET_Truth'
MissingETData_antikt6_inversion.METCorrTopoKey              = 'MET_CorrTopo' + suffix_antikt6_inversion
MissingETData_antikt6_inversion.METCalibKey                 = 'MET_Calib' + suffix_antikt6_inversion
MissingETData_antikt6_inversion.METCryoConeKey              = 'MET_CryoCone' + suffix_antikt6_inversion
MissingETData_antikt6_inversion.METRefFinalKey              = 'MET_RefFinal' + suffix_antikt6_inversion
MissingETData_antikt6_inversion.METRefEleKey                = 'MET_RefEle' + suffix_antikt6_inversion
MissingETData_antikt6_inversion.METRefGammaKey              = 'MET_RefGamma' + suffix_antikt6_inversion
MissingETData_antikt6_inversion.METRefJetKey                = 'MET_RefJet' + suffix_antikt6_inversion
MissingETData_antikt6_inversion.METRefMuonKey               = 'MET_RefMuon' + suffix_antikt6_inversion
MissingETData_antikt6_inversion.METRefMuon_TrackKey         = 'MET_RefMuon_Track' + suffix_antikt6_inversion
MissingETData_antikt6_inversion.METRefTauKey                = 'MET_RefTau' + suffix_antikt6_inversion
MissingETData_antikt6_inversion.METMuonBoyKey               = 'MET_MuonBoy' + suffix_antikt6_inversion
MissingETData_antikt6_inversion.METCellOutKey               = 'MET_CellOut' + suffix_antikt6_inversion

MissingETData_antikt6_inversion.AllCaloKey                  = 'AllCalo'
MissingETData_antikt6_inversion.METL1ROIKey                 = 'LVL1_ROI'
MissingETData_antikt6_inversion.METL2Key                    = 'HLT_T2MissingET'
MissingETData_antikt6_inversion.METEF_FEBHeaderKey          = 'HLT_TrigEFMissingET_FEB'
MissingETData_antikt6_inversion.METEF_noiseSuppKey          = 'HLT_TrigEFMissingET_noiseSupp'
MissingETData_antikt6_inversion.METEF_defaultKey            = 'HLT_TrigEFMissingET'
MissingETData_antikt6_inversion.UseCaloNoiseTool            = False
MissingETData_antikt6_inversion.NSigmaCut                   = 2.0
MissingETData_antikt6_inversion.UseBadChannelMasker         = False
MissingETData_antikt6_inversion.UseBadChannelTool           = False

ToolSvc += MissingETData_antikt6_inversion
print      MissingETData_antikt6_inversion

ContainerComparatorTool.MissingETData_array += [ MissingETData.getFullName() ]
ContainerComparatorTool.MissingETData_array += [ MissingETData_antikt.getFullName() ]
ContainerComparatorTool.MissingETData_array += [ MissingETData_antikt_inversion.getFullName() ]
ContainerComparatorTool.MissingETData_array += [ MissingETData_antikt6.getFullName() ]
ContainerComparatorTool.MissingETData_array += [ MissingETData_antikt6_inversion.getFullName() ]
ContainerComparatorTool.MissingETData_array += [ MissingETData_cone4_inversion.getFullName() ]
ContainerComparatorTool.MissingETData_array += [ MissingETData_cone7.getFullName() ]
ContainerComparatorTool.MissingETData_array += [ MissingETData_cone7_inversion.getFullName() ]


#############################################################
#############################################################
#############################################################

from MissingETPerformance.MissingETPerformanceConf import BasicPlotsTool as ConfiguredBasicPlotsTool
#############################################################
#create a basic plots tool
#############################################################
BasicPlotsTool = ConfiguredBasicPlotsTool("BasicPlotsTool");

BasicPlotsTool.plotJetVariables = False
BasicPlotsTool.iset             = 6020
BasicPlotsTool.nbinp            = 250
BasicPlotsTool.suptmi           = 500.
BasicPlotsTool.nbinE            = 200
BasicPlotsTool.lowET            = -5.
BasicPlotsTool.nbinpxy          = 500
BasicPlotsTool.suptmixy         = 250.
BasicPlotsTool.nbinphi          = 63
BasicPlotsTool.binphi           = 3.15
BasicPlotsTool.nbinsig          = 500
BasicPlotsTool.binsig           = 50.
BasicPlotsTool.nbineta          = 50
BasicPlotsTool.bineta           = 5.0
#need the trailing '/'
BasicPlotsTool.FolderName       = '/'

ToolSvc += BasicPlotsTool
print      BasicPlotsTool

#############################################################
#create a basic plots tool
#############################################################
BasicPlotsTool_cone4_inversion = ConfiguredBasicPlotsTool("BasicPlotsTool_cone4_inversion");

BasicPlotsTool_cone4_inversion.plotJetVariables = False
BasicPlotsTool_cone4_inversion.iset             = 6020
BasicPlotsTool_cone4_inversion.nbinp            = 250
BasicPlotsTool_cone4_inversion.suptmi           = 500.
BasicPlotsTool_cone4_inversion.nbinE            = 200
BasicPlotsTool_cone4_inversion.lowET            = -5.
BasicPlotsTool_cone4_inversion.nbinpxy          = 500
BasicPlotsTool_cone4_inversion.suptmixy         = 250.
BasicPlotsTool_cone4_inversion.nbinphi          = 63
BasicPlotsTool_cone4_inversion.binphi           = 3.15
BasicPlotsTool_cone4_inversion.nbinsig          = 500
BasicPlotsTool_cone4_inversion.binsig           = 50.
BasicPlotsTool_cone4_inversion.nbineta          = 50
BasicPlotsTool_cone4_inversion.bineta           = 5.0
#need the trailing '/'
BasicPlotsTool_cone4_inversion.FolderName       = 'cone4_inversion/'

ToolSvc += BasicPlotsTool_cone4_inversion
print      BasicPlotsTool_cone4_inversion

#############################################################
#create a basic plots tool
#############################################################
BasicPlotsTool_cone7_inversion = ConfiguredBasicPlotsTool("BasicPlotsTool_cone7_inversion");

BasicPlotsTool_cone7_inversion.plotJetVariables = False
BasicPlotsTool_cone7_inversion.iset             = 6020
BasicPlotsTool_cone7_inversion.nbinp            = 250
BasicPlotsTool_cone7_inversion.suptmi           = 500.
BasicPlotsTool_cone7_inversion.nbinE            = 200
BasicPlotsTool_cone7_inversion.lowET            = -5.
BasicPlotsTool_cone7_inversion.nbinpxy          = 500
BasicPlotsTool_cone7_inversion.suptmixy         = 250.
BasicPlotsTool_cone7_inversion.nbinphi          = 63
BasicPlotsTool_cone7_inversion.binphi           = 3.15
BasicPlotsTool_cone7_inversion.nbinsig          = 500
BasicPlotsTool_cone7_inversion.binsig           = 50.
BasicPlotsTool_cone7_inversion.nbineta          = 50
BasicPlotsTool_cone7_inversion.bineta           = 5.0
#need the trailing '/'
BasicPlotsTool_cone7_inversion.FolderName       = 'cone7_inversion/'

ToolSvc += BasicPlotsTool_cone7_inversion
print      BasicPlotsTool_cone7_inversion

#############################################################
#create a basic plots tool
#############################################################
BasicPlotsTool_cone7 = ConfiguredBasicPlotsTool("BasicPlotsTool_cone7");

BasicPlotsTool_cone7.plotJetVariables = False
BasicPlotsTool_cone7.iset             = 6020
BasicPlotsTool_cone7.nbinp            = 250
BasicPlotsTool_cone7.suptmi           = 500.
BasicPlotsTool_cone7.nbinE            = 200
BasicPlotsTool_cone7.lowET            = -5.
BasicPlotsTool_cone7.nbinpxy          = 500
BasicPlotsTool_cone7.suptmixy         = 250.
BasicPlotsTool_cone7.nbinphi          = 63
BasicPlotsTool_cone7.binphi           = 3.15
BasicPlotsTool_cone7.nbinsig          = 500
BasicPlotsTool_cone7.binsig           = 50.
BasicPlotsTool_cone7.nbineta          = 50
BasicPlotsTool_cone7.bineta           = 5.0
#need the trailing '/'
BasicPlotsTool_cone7.FolderName       = 'cone7/'

ToolSvc += BasicPlotsTool_cone7
print      BasicPlotsTool_cone7

#############################################################
#create a basic plots tool
#############################################################
BasicPlotsTool_antikt = ConfiguredBasicPlotsTool("BasicPlotsTool_antikt");

BasicPlotsTool_antikt.plotJetVariables = False
BasicPlotsTool_antikt.iset             = 6020
BasicPlotsTool_antikt.nbinp            = 250
BasicPlotsTool_antikt.suptmi           = 500.
BasicPlotsTool_antikt.nbinE            = 200
BasicPlotsTool_antikt.lowET            = -5.
BasicPlotsTool_antikt.nbinpxy          = 500
BasicPlotsTool_antikt.suptmixy         = 250.
BasicPlotsTool_antikt.nbinphi          = 63
BasicPlotsTool_antikt.binphi           = 3.15
BasicPlotsTool_antikt.nbinsig          = 500
BasicPlotsTool_antikt.binsig           = 50.
BasicPlotsTool_antikt.nbineta          = 50
BasicPlotsTool_antikt.bineta           = 5.0
#need the trailing '/'
BasicPlotsTool_antikt.FolderName       = 'antikt/'

ToolSvc += BasicPlotsTool_antikt
print      BasicPlotsTool_antikt

#############################################################
#create a basic plots tool
#############################################################
BasicPlotsTool_antikt_inversion = ConfiguredBasicPlotsTool("BasicPlotsTool_antikt_inversion");

BasicPlotsTool_antikt_inversion.plotJetVariables = False
BasicPlotsTool_antikt_inversion.iset             = 6020
BasicPlotsTool_antikt_inversion.nbinp            = 250
BasicPlotsTool_antikt_inversion.suptmi           = 500.
BasicPlotsTool_antikt_inversion.nbinE            = 200
BasicPlotsTool_antikt_inversion.lowET            = -5.
BasicPlotsTool_antikt_inversion.nbinpxy          = 500
BasicPlotsTool_antikt_inversion.suptmixy         = 250.
BasicPlotsTool_antikt_inversion.nbinphi          = 63
BasicPlotsTool_antikt_inversion.binphi           = 3.15
BasicPlotsTool_antikt_inversion.nbinsig          = 500
BasicPlotsTool_antikt_inversion.binsig           = 50.
BasicPlotsTool_antikt_inversion.nbineta          = 50
BasicPlotsTool_antikt_inversion.bineta           = 5.0
#need the trailing '/'
BasicPlotsTool_antikt_inversion.FolderName       = 'antikt_inversion/'

ToolSvc += BasicPlotsTool_antikt_inversion
print      BasicPlotsTool_antikt_inversion

#############################################################
#create a basic plots tool
#############################################################
BasicPlotsTool_antikt6 = ConfiguredBasicPlotsTool("BasicPlotsTool_antikt6");

BasicPlotsTool_antikt6.plotJetVariables = False
BasicPlotsTool_antikt6.iset             = 6020
BasicPlotsTool_antikt6.nbinp            = 250
BasicPlotsTool_antikt6.suptmi           = 500.
BasicPlotsTool_antikt6.nbinE            = 200
BasicPlotsTool_antikt6.lowET            = -5.
BasicPlotsTool_antikt6.nbinpxy          = 500
BasicPlotsTool_antikt6.suptmixy         = 250.
BasicPlotsTool_antikt6.nbinphi          = 63
BasicPlotsTool_antikt6.binphi           = 3.15
BasicPlotsTool_antikt6.nbinsig          = 500
BasicPlotsTool_antikt6.binsig           = 50.
BasicPlotsTool_antikt6.nbineta          = 50
BasicPlotsTool_antikt6.bineta           = 5.0
#need the trailing '/'
BasicPlotsTool_antikt6.FolderName       = 'antikt6/'

ToolSvc += BasicPlotsTool_antikt6
print      BasicPlotsTool_antikt6

#############################################################
#create a basic plots tool
#############################################################
BasicPlotsTool_antikt6_inversion = ConfiguredBasicPlotsTool("BasicPlotsTool_antikt6_inversion");

BasicPlotsTool_antikt6_inversion.plotJetVariables = False
BasicPlotsTool_antikt6_inversion.iset             = 6020
BasicPlotsTool_antikt6_inversion.nbinp            = 250
BasicPlotsTool_antikt6_inversion.suptmi           = 500.
BasicPlotsTool_antikt6_inversion.nbinE            = 200
BasicPlotsTool_antikt6_inversion.lowET            = -5.
BasicPlotsTool_antikt6_inversion.nbinpxy          = 500
BasicPlotsTool_antikt6_inversion.suptmixy         = 250.
BasicPlotsTool_antikt6_inversion.nbinphi          = 63
BasicPlotsTool_antikt6_inversion.binphi           = 3.15
BasicPlotsTool_antikt6_inversion.nbinsig          = 500
BasicPlotsTool_antikt6_inversion.binsig           = 50.
BasicPlotsTool_antikt6_inversion.nbineta          = 50
BasicPlotsTool_antikt6_inversion.bineta           = 5.0
#need the trailing '/'
BasicPlotsTool_antikt6_inversion.FolderName       = 'antikt6_inversion/'

ToolSvc += BasicPlotsTool_antikt6_inversion
print      BasicPlotsTool_antikt6_inversion

ContainerComparatorTool.BasicPlotsTools += [ BasicPlotsTool.getFullName() ]
ContainerComparatorTool.BasicPlotsTools += [ BasicPlotsTool_antikt.getFullName() ]
ContainerComparatorTool.BasicPlotsTools += [ BasicPlotsTool_antikt_inversion.getFullName() ]
ContainerComparatorTool.BasicPlotsTools += [ BasicPlotsTool_antikt6.getFullName() ]
ContainerComparatorTool.BasicPlotsTools += [ BasicPlotsTool_antikt6_inversion.getFullName() ]
ContainerComparatorTool.BasicPlotsTools += [ BasicPlotsTool_cone4_inversion.getFullName() ]
ContainerComparatorTool.BasicPlotsTools += [ BasicPlotsTool_cone7.getFullName() ]
ContainerComparatorTool.BasicPlotsTools += [ BasicPlotsTool_cone7_inversion.getFullName() ]

#############################################################
#############################################################
#############################################################

ContainerComparatorTool.Data_BasicTool_pairs = [
    ["MissingETData","BasicPlotsTool"],
    ["MissingETData_antikt","BasicPlotsTool_antikt"],
    ["MissingETData_antikt_inversion","BasicPlotsTool_antikt_inversion"],
    ["MissingETData_antikt6","BasicPlotsTool_antikt6"],
    ["MissingETData_antikt6_inversion","BasicPlotsTool_antikt6_inversion"],
    ["MissingETData_cone4_inversion","BasicPlotsTool_cone4_inversion"],
    ["MissingETData_cone7","BasicPlotsTool_cone7"],
    ["MissingETData_cone7_inversion","BasicPlotsTool_cone7_inversion"],
]


from MissingETPerformance.MissingETPerformanceConf import ResolutionTool as ConfiguredResolutionTool

#############################################################
#create a resolution tool
#############################################################
ResolutionTool = ConfiguredResolutionTool("ResolutionTool");

ResolutionTool.numptt             =   40
ResolutionTool.startpt            =   5.
ResolutionTool.binpt              =   20.     
ResolutionTool.scares             =   100.     
ResolutionTool.curvasres          =   30.  
ResolutionTool.fitrange           =   3     
ResolutionTool.ns                 =   2.          
ResolutionTool.percerr            =   0.15
ResolutionTool.entries            =   100    

ResolutionTool.iset               =   5013
ResolutionTool.nbinres1           =   100
ResolutionTool.scares1            =   50.
ResolutionTool.nbinres2           =   100
ResolutionTool.scares2            =   500.
ResolutionTool.nbinres3           =   200
ResolutionTool.scares3            =   100.
ResolutionTool.nbinphires1        =   315

ResolutionTool.lowET              = -5.
ResolutionTool.suET               = 795.

ResolutionTool.useTruth           =   0
ResolutionTool.usesumet           =   5
#need the trailing '/'
ResolutionTool.FolderName       = '/'

ToolSvc += ResolutionTool
print      ResolutionTool

#############################################################
#create a resolution tool
#############################################################
ResolutionTool_cone7 = ConfiguredResolutionTool("ResolutionTool_cone7");

ResolutionTool_cone7.numptt             =   40
ResolutionTool_cone7.startpt            =   5.
ResolutionTool_cone7.binpt              =   20.     
ResolutionTool_cone7.scares             =   100.     
ResolutionTool_cone7.curvasres          =   30.  
ResolutionTool_cone7.fitrange           =   3     
ResolutionTool_cone7.ns                 =   2.          
ResolutionTool_cone7.percerr            =   0.15
ResolutionTool_cone7.entries            =   100    

ResolutionTool_cone7.iset               =   5013
ResolutionTool_cone7.nbinres1           =   100
ResolutionTool_cone7.scares1            =   50.
ResolutionTool_cone7.nbinres2           =   100
ResolutionTool_cone7.scares2            =   500.
ResolutionTool_cone7.nbinres3           =   200
ResolutionTool_cone7.scares3            =   100.
ResolutionTool_cone7.nbinphires1        =   315

ResolutionTool_cone7.lowET              = -5.
ResolutionTool_cone7.suET               = 795.

ResolutionTool_cone7.useTruth           =   0
ResolutionTool_cone7.usesumet           =   5
#need the trailing '/'
ResolutionTool_cone7.FolderName       = 'cone7/'

ToolSvc += ResolutionTool_cone7
print      ResolutionTool_cone7

#############################################################
#create a resolution tool
#############################################################
ResolutionTool_cone7_inversion = ConfiguredResolutionTool("ResolutionTool_cone7_inversion");

ResolutionTool_cone7_inversion.numptt             =   40
ResolutionTool_cone7_inversion.startpt            =   5.
ResolutionTool_cone7_inversion.binpt              =   20.     
ResolutionTool_cone7_inversion.scares             =   100.     
ResolutionTool_cone7_inversion.curvasres          =   30.  
ResolutionTool_cone7_inversion.fitrange           =   3     
ResolutionTool_cone7_inversion.ns                 =   2.          
ResolutionTool_cone7_inversion.percerr            =   0.15
ResolutionTool_cone7_inversion.entries            =   100    

ResolutionTool_cone7_inversion.iset               =   5013
ResolutionTool_cone7_inversion.nbinres1           =   100
ResolutionTool_cone7_inversion.scares1            =   50.
ResolutionTool_cone7_inversion.nbinres2           =   100
ResolutionTool_cone7_inversion.scares2            =   500.
ResolutionTool_cone7_inversion.nbinres3           =   200
ResolutionTool_cone7_inversion.scares3            =   100.
ResolutionTool_cone7_inversion.nbinphires1        =   315

ResolutionTool_cone7_inversion.lowET              = -5.
ResolutionTool_cone7_inversion.suET               = 795.

ResolutionTool_cone7_inversion.useTruth           =   0
ResolutionTool_cone7_inversion.usesumet           =   5
#need the trailing '/'
ResolutionTool_cone7_inversion.FolderName       = 'cone7_inversion/'

ToolSvc += ResolutionTool_cone7_inversion
print      ResolutionTool_cone7_inversion

#############################################################
#create a resolution tool
#############################################################
ResolutionTool_cone4_inversion = ConfiguredResolutionTool("ResolutionTool_cone4_inversion");

ResolutionTool_cone4_inversion.numptt             =   40
ResolutionTool_cone4_inversion.startpt            =   5.
ResolutionTool_cone4_inversion.binpt              =   20.     
ResolutionTool_cone4_inversion.scares             =   100.     
ResolutionTool_cone4_inversion.curvasres          =   30.  
ResolutionTool_cone4_inversion.fitrange           =   3     
ResolutionTool_cone4_inversion.ns                 =   2.          
ResolutionTool_cone4_inversion.percerr            =   0.15
ResolutionTool_cone4_inversion.entries            =   100    

ResolutionTool_cone4_inversion.iset               =   5013
ResolutionTool_cone4_inversion.nbinres1           =   100
ResolutionTool_cone4_inversion.scares1            =   50.
ResolutionTool_cone4_inversion.nbinres2           =   100
ResolutionTool_cone4_inversion.scares2            =   500.
ResolutionTool_cone4_inversion.nbinres3           =   200
ResolutionTool_cone4_inversion.scares3            =   100.
ResolutionTool_cone4_inversion.nbinphires1        =   315

ResolutionTool_cone4_inversion.lowET              = -5.
ResolutionTool_cone4_inversion.suET               = 795.

ResolutionTool_cone4_inversion.useTruth           =   0
ResolutionTool_cone4_inversion.usesumet           =   5
#need the trailing '/'
ResolutionTool_cone4_inversion.FolderName       = 'cone4_inversion/'

ToolSvc += ResolutionTool_cone4_inversion
print      ResolutionTool_cone4_inversion

#############################################################
#create a resolution tool
#############################################################
ResolutionTool_antikt = ConfiguredResolutionTool("ResolutionTool_antikt");

ResolutionTool_antikt.numptt             =   40
ResolutionTool_antikt.startpt            =   5.
ResolutionTool_antikt.binpt              =   20.     
ResolutionTool_antikt.scares             =   100.     
ResolutionTool_antikt.curvasres          =   30.  
ResolutionTool_antikt.fitrange           =   3     
ResolutionTool_antikt.ns                 =   2.          
ResolutionTool_antikt.percerr            =   0.15
ResolutionTool_antikt.entries            =   100    

ResolutionTool_antikt.iset               =   5013
ResolutionTool_antikt.nbinres1           =   100
ResolutionTool_antikt.scares1            =   50.
ResolutionTool_antikt.nbinres2           =   100
ResolutionTool_antikt.scares2            =   500.
ResolutionTool_antikt.nbinres3           =   200
ResolutionTool_antikt.scares3            =   100.
ResolutionTool_antikt.nbinphires1        =   315

ResolutionTool_antikt.lowET              = -5.
ResolutionTool_antikt.suET               = 795.

ResolutionTool_antikt.useTruth           =   0
ResolutionTool_antikt.usesumet           =   5
#need the trailing '/'
ResolutionTool_antikt.FolderName       = 'antikt/'

ToolSvc += ResolutionTool_antikt
print      ResolutionTool_antikt

#############################################################
#create a resolution tool
#############################################################
ResolutionTool_antikt_inversion = ConfiguredResolutionTool("ResolutionTool_antikt_inversion");

ResolutionTool_antikt_inversion.numptt             =   40
ResolutionTool_antikt_inversion.startpt            =   5.
ResolutionTool_antikt_inversion.binpt              =   20.     
ResolutionTool_antikt_inversion.scares             =   100.     
ResolutionTool_antikt_inversion.curvasres          =   30.  
ResolutionTool_antikt_inversion.fitrange           =   3     
ResolutionTool_antikt_inversion.ns                 =   2.          
ResolutionTool_antikt_inversion.percerr            =   0.15
ResolutionTool_antikt_inversion.entries            =   100    

ResolutionTool_antikt_inversion.iset               =   5013
ResolutionTool_antikt_inversion.nbinres1           =   100
ResolutionTool_antikt_inversion.scares1            =   50.
ResolutionTool_antikt_inversion.nbinres2           =   100
ResolutionTool_antikt_inversion.scares2            =   500.
ResolutionTool_antikt_inversion.nbinres3           =   200
ResolutionTool_antikt_inversion.scares3            =   100.
ResolutionTool_antikt_inversion.nbinphires1        =   315

ResolutionTool_antikt_inversion.lowET              = -5.
ResolutionTool_antikt_inversion.suET               = 795.

ResolutionTool_antikt_inversion.useTruth           =   0
ResolutionTool_antikt_inversion.usesumet           =   5
#need the trailing '/'
ResolutionTool_antikt_inversion.FolderName       = 'antikt_inversion/'

ToolSvc += ResolutionTool_antikt_inversion
print      ResolutionTool_antikt_inversion

#############################################################
#create a resolution tool
#############################################################
ResolutionTool_antikt6 = ConfiguredResolutionTool("ResolutionTool_antikt6");

ResolutionTool_antikt6.numptt             =   40
ResolutionTool_antikt6.startpt            =   5.
ResolutionTool_antikt6.binpt              =   20.     
ResolutionTool_antikt6.scares             =   100.     
ResolutionTool_antikt6.curvasres          =   30.  
ResolutionTool_antikt6.fitrange           =   3     
ResolutionTool_antikt6.ns                 =   2.          
ResolutionTool_antikt6.percerr            =   0.15
ResolutionTool_antikt6.entries            =   100    

ResolutionTool_antikt6.iset               =   5013
ResolutionTool_antikt6.nbinres1           =   100
ResolutionTool_antikt6.scares1            =   50.
ResolutionTool_antikt6.nbinres2           =   100
ResolutionTool_antikt6.scares2            =   500.
ResolutionTool_antikt6.nbinres3           =   200
ResolutionTool_antikt6.scares3            =   100.
ResolutionTool_antikt6.nbinphires1        =   315

ResolutionTool_antikt6.lowET              = -5.
ResolutionTool_antikt6.suET               = 795.

ResolutionTool_antikt6.useTruth           =   0
ResolutionTool_antikt6.usesumet           =   5
#need the trailing '/'
ResolutionTool_antikt6.FolderName       = 'antikt6/'

ToolSvc += ResolutionTool_antikt6
print      ResolutionTool_antikt6

#############################################################
#create a resolution tool
#############################################################
ResolutionTool_antikt6_inversion = ConfiguredResolutionTool("ResolutionTool_antikt6_inversion");

ResolutionTool_antikt6_inversion.numptt             =   40
ResolutionTool_antikt6_inversion.startpt            =   5.
ResolutionTool_antikt6_inversion.binpt              =   20.     
ResolutionTool_antikt6_inversion.scares             =   100.     
ResolutionTool_antikt6_inversion.curvasres          =   30.  
ResolutionTool_antikt6_inversion.fitrange           =   3     
ResolutionTool_antikt6_inversion.ns                 =   2.          
ResolutionTool_antikt6_inversion.percerr            =   0.15
ResolutionTool_antikt6_inversion.entries            =   100    

ResolutionTool_antikt6_inversion.iset               =   5013
ResolutionTool_antikt6_inversion.nbinres1           =   100
ResolutionTool_antikt6_inversion.scares1            =   50.
ResolutionTool_antikt6_inversion.nbinres2           =   100
ResolutionTool_antikt6_inversion.scares2            =   500.
ResolutionTool_antikt6_inversion.nbinres3           =   200
ResolutionTool_antikt6_inversion.scares3            =   100.
ResolutionTool_antikt6_inversion.nbinphires1        =   315

ResolutionTool_antikt6_inversion.lowET              = -5.
ResolutionTool_antikt6_inversion.suET               = 795.

ResolutionTool_antikt6_inversion.useTruth           =   0
ResolutionTool_antikt6_inversion.usesumet           =   5
#need the trailing '/'
ResolutionTool_antikt6_inversion.FolderName       = 'antikt6_inversion/'

ToolSvc += ResolutionTool_antikt6_inversion
print      ResolutionTool_antikt6_inversion

ContainerComparatorTool.ResolutionTools += [ ResolutionTool.getFullName() ]
ContainerComparatorTool.ResolutionTools += [ ResolutionTool_antikt.getFullName() ]
ContainerComparatorTool.ResolutionTools += [ ResolutionTool_antikt_inversion.getFullName() ]
ContainerComparatorTool.ResolutionTools += [ ResolutionTool_antikt6.getFullName() ]
ContainerComparatorTool.ResolutionTools += [ ResolutionTool_antikt6_inversion.getFullName() ]
ContainerComparatorTool.ResolutionTools += [ ResolutionTool_cone4_inversion.getFullName() ]
ContainerComparatorTool.ResolutionTools += [ ResolutionTool_cone7.getFullName() ]
ContainerComparatorTool.ResolutionTools += [ ResolutionTool_cone7_inversion.getFullName() ]

ContainerComparatorTool.Data_Resolution_pairs = [
    ["MissingETData","ResolutionTool"],
    ["MissingETData_antikt","ResolutionTool_antikt"],
    ["MissingETData_antikt_inversion","ResolutionTool_antikt_inversion"],
    ["MissingETData_antikt6","ResolutionTool_antikt6"],
    ["MissingETData_antikt6_inversion","ResolutionTool_antikt6_inversion"],
    ["MissingETData_cone4_inversion","ResolutionTool_cone4_inversion"],
    ["MissingETData_cone7","ResolutionTool_cone7"],
    ["MissingETData_cone7_inversion","ResolutionTool_cone7_inversion"],
]

from MissingETPerformance.MissingETPerformanceConf import LinearityTool as ConfiguredLinearityTool
#############################################################
#create a linearity tool
#############################################################
LinearityTool = ConfiguredLinearityTool("LinearityTool");

LinearityTool.lnumptt        = 20
LinearityTool.lstartpt       = 0 
LinearityTool.lbinpt         = 5.
LinearityTool.fitrangel      = 2  
LinearityTool.lnbini         = 50    
LinearityTool.percerr        = 0.15 
LinearityTool.entries        = 50
LinearityTool.curvaslin      = 0.3 
LinearityTool.fitr           = 2.5
LinearityTool.linplot_x = 0.72
LinearityTool.linplot_y = 0.85
LinearityTool.linplot_text = 'W#rightarrow e#nu'
LinearityTool.linorm       = 0
LinearityTool.bin_rp3    = 1.
LinearityTool.suptmi=100.
LinearityTool.etmi_threshold=50.
#need the trailing '/'
LinearityTool.FolderName       = '/'

ToolSvc += LinearityTool
print      LinearityTool

#############################################################
#create a linearity tool
#############################################################
LinearityTool_cone7 = ConfiguredLinearityTool("LinearityTool_cone7");

LinearityTool_cone7.lnumptt        = 20
LinearityTool_cone7.lstartpt       = 0 
LinearityTool_cone7.lbinpt         = 5.
LinearityTool_cone7.fitrangel      = 2  
LinearityTool_cone7.lnbini         = 50    
LinearityTool_cone7.percerr        = 0.15 
LinearityTool_cone7.entries        = 50
LinearityTool_cone7.curvaslin      = 0.3 
LinearityTool_cone7.fitr           = 2.5
LinearityTool_cone7.linplot_x = 0.72
LinearityTool_cone7.linplot_y = 0.85
LinearityTool_cone7.linplot_text = 'W#rightarrow e#nu'
LinearityTool_cone7.linorm       = 0
LinearityTool_cone7.bin_rp3    = 1.
LinearityTool_cone7.suptmi=100.
LinearityTool_cone7.etmi_threshold=50.
#need the trailing '/'
LinearityTool_cone7.FolderName       = 'cone7/'

ToolSvc += LinearityTool_cone7
print      LinearityTool_cone7

#############################################################
#create a linearity tool
#############################################################
LinearityTool_cone7_inversion = ConfiguredLinearityTool("LinearityTool_cone7_inversion");

LinearityTool_cone7_inversion.lnumptt        = 20
LinearityTool_cone7_inversion.lstartpt       = 0 
LinearityTool_cone7_inversion.lbinpt         = 5.
LinearityTool_cone7_inversion.fitrangel      = 2  
LinearityTool_cone7_inversion.lnbini         = 50    
LinearityTool_cone7_inversion.percerr        = 0.15 
LinearityTool_cone7_inversion.entries        = 50
LinearityTool_cone7_inversion.curvaslin      = 0.3 
LinearityTool_cone7_inversion.fitr           = 2.5
LinearityTool_cone7_inversion.linplot_x = 0.72
LinearityTool_cone7_inversion.linplot_y = 0.85
LinearityTool_cone7_inversion.linplot_text = 'W#rightarrow e#nu'
LinearityTool_cone7_inversion.linorm       = 0
LinearityTool_cone7_inversion.bin_rp3    = 1.
LinearityTool_cone7_inversion.suptmi=100.
LinearityTool_cone7_inversion.etmi_threshold=50.
#need the trailing '/'
LinearityTool_cone7_inversion.FolderName       = 'cone7_inversion/'

ToolSvc += LinearityTool_cone7_inversion
print      LinearityTool_cone7_inversion

#############################################################
#create a linearity tool
#############################################################
LinearityTool_cone4_inversion = ConfiguredLinearityTool("LinearityTool_cone4_inversion");

LinearityTool_cone4_inversion.lnumptt        = 20
LinearityTool_cone4_inversion.lstartpt       = 0 
LinearityTool_cone4_inversion.lbinpt         = 5.
LinearityTool_cone4_inversion.fitrangel      = 2  
LinearityTool_cone4_inversion.lnbini         = 50    
LinearityTool_cone4_inversion.percerr        = 0.15 
LinearityTool_cone4_inversion.entries        = 50
LinearityTool_cone4_inversion.curvaslin      = 0.3 
LinearityTool_cone4_inversion.fitr           = 2.5
LinearityTool_cone4_inversion.linplot_x = 0.72
LinearityTool_cone4_inversion.linplot_y = 0.85
LinearityTool_cone4_inversion.linplot_text = 'W#rightarrow e#nu'
LinearityTool_cone4_inversion.linorm       = 0
LinearityTool_cone4_inversion.bin_rp3    = 1.
LinearityTool_cone4_inversion.suptmi=100.
LinearityTool_cone4_inversion.etmi_threshold=50.
#need the trailing '/'
LinearityTool_cone4_inversion.FolderName       = 'cone4_inversion/'

ToolSvc += LinearityTool_cone4_inversion
print      LinearityTool_cone4_inversion

#############################################################
#create a linearity tool
#############################################################
LinearityTool_antikt = ConfiguredLinearityTool("LinearityTool_antikt");

LinearityTool_antikt.lnumptt        = 20
LinearityTool_antikt.lstartpt       = 0 
LinearityTool_antikt.lbinpt         = 5.
LinearityTool_antikt.fitrangel      = 2  
LinearityTool_antikt.lnbini         = 50    
LinearityTool_antikt.percerr        = 0.15 
LinearityTool_antikt.entries        = 50
LinearityTool_antikt.curvaslin      = 0.3 
LinearityTool_antikt.fitr           = 2.5
LinearityTool_antikt.linplot_x = 0.72
LinearityTool_antikt.linplot_y = 0.85
LinearityTool_antikt.linplot_text = 'W#rightarrow e#nu'
LinearityTool_antikt.linorm       = 0
LinearityTool_antikt.bin_rp3    = 1.
LinearityTool_antikt.suptmi=100.
LinearityTool_antikt.etmi_threshold=50.
#need the trailing '/'
LinearityTool_antikt.FolderName       = 'antikt/'

ToolSvc += LinearityTool_antikt
print      LinearityTool_antikt

#############################################################
#create a linearity tool
#############################################################
LinearityTool_antikt_inversion = ConfiguredLinearityTool("LinearityTool_antikt_inversion");

LinearityTool_antikt_inversion.lnumptt        = 20
LinearityTool_antikt_inversion.lstartpt       = 0 
LinearityTool_antikt_inversion.lbinpt         = 5.
LinearityTool_antikt_inversion.fitrangel      = 2  
LinearityTool_antikt_inversion.lnbini         = 50    
LinearityTool_antikt_inversion.percerr        = 0.15 
LinearityTool_antikt_inversion.entries        = 50
LinearityTool_antikt_inversion.curvaslin      = 0.3 
LinearityTool_antikt_inversion.fitr           = 2.5
LinearityTool_antikt_inversion.linplot_x = 0.72
LinearityTool_antikt_inversion.linplot_y = 0.85
LinearityTool_antikt_inversion.linplot_text = 'W#rightarrow e#nu'
LinearityTool_antikt_inversion.linorm       = 0
LinearityTool_antikt_inversion.bin_rp3    = 1.
LinearityTool_antikt_inversion.suptmi=100.
LinearityTool_antikt_inversion.etmi_threshold=50.
#need the trailing '/'
LinearityTool_antikt_inversion.FolderName       = 'antikt_inversion/'

ToolSvc += LinearityTool_antikt_inversion
print      LinearityTool_antikt_inversion

#############################################################
#create a linearity tool
#############################################################
LinearityTool_antikt6 = ConfiguredLinearityTool("LinearityTool_antikt6");

LinearityTool_antikt6.lnumptt        = 20
LinearityTool_antikt6.lstartpt       = 0 
LinearityTool_antikt6.lbinpt         = 5.
LinearityTool_antikt6.fitrangel      = 2  
LinearityTool_antikt6.lnbini         = 50    
LinearityTool_antikt6.percerr        = 0.15 
LinearityTool_antikt6.entries        = 50
LinearityTool_antikt6.curvaslin      = 0.3 
LinearityTool_antikt6.fitr           = 2.5
LinearityTool_antikt6.linplot_x = 0.72
LinearityTool_antikt6.linplot_y = 0.85
LinearityTool_antikt6.linplot_text = 'W#rightarrow e#nu'
LinearityTool_antikt6.linorm       = 0
LinearityTool_antikt6.bin_rp3    = 1.
LinearityTool_antikt6.suptmi=100.
LinearityTool_antikt6.etmi_threshold=50.
#need the trailing '/'
LinearityTool_antikt6.FolderName       = 'antikt6/'

ToolSvc += LinearityTool_antikt6
print      LinearityTool_antikt6

#############################################################
#create a linearity tool
#############################################################
LinearityTool_antikt6_inversion = ConfiguredLinearityTool("LinearityTool_antikt6_inversion");

LinearityTool_antikt6_inversion.lnumptt        = 20
LinearityTool_antikt6_inversion.lstartpt       = 0 
LinearityTool_antikt6_inversion.lbinpt         = 5.
LinearityTool_antikt6_inversion.fitrangel      = 2  
LinearityTool_antikt6_inversion.lnbini         = 50    
LinearityTool_antikt6_inversion.percerr        = 0.15 
LinearityTool_antikt6_inversion.entries        = 50
LinearityTool_antikt6_inversion.curvaslin      = 0.3 
LinearityTool_antikt6_inversion.fitr           = 2.5
LinearityTool_antikt6_inversion.linplot_x = 0.72
LinearityTool_antikt6_inversion.linplot_y = 0.85
LinearityTool_antikt6_inversion.linplot_text = 'W#rightarrow e#nu'
LinearityTool_antikt6_inversion.linorm       = 0
LinearityTool_antikt6_inversion.bin_rp3    = 1.
LinearityTool_antikt6_inversion.suptmi=100.
LinearityTool_antikt6_inversion.etmi_threshold=50.
#need the trailing '/'
LinearityTool_antikt6_inversion.FolderName       = 'antikt6_inversion/'

ToolSvc += LinearityTool_antikt6_inversion
print      LinearityTool_antikt6_inversion

ContainerComparatorTool.LinearityTools += [ LinearityTool.getFullName() ]
ContainerComparatorTool.LinearityTools += [ LinearityTool_antikt.getFullName() ]
ContainerComparatorTool.LinearityTools += [ LinearityTool_antikt_inversion.getFullName() ]
ContainerComparatorTool.LinearityTools += [ LinearityTool_antikt6.getFullName() ]
ContainerComparatorTool.LinearityTools += [ LinearityTool_antikt6_inversion.getFullName() ]
ContainerComparatorTool.LinearityTools += [ LinearityTool_cone4_inversion.getFullName() ]
ContainerComparatorTool.LinearityTools += [ LinearityTool_cone7.getFullName() ]
ContainerComparatorTool.LinearityTools += [ LinearityTool_cone7_inversion.getFullName() ]

ContainerComparatorTool.Data_Linearity_pairs = [
    ["MissingETData","LinearityTool"],
    ["MissingETData_antikt","LinearityTool_antikt"],
    ["MissingETData_antikt_inversion","LinearityTool_antikt_inversion"],
    ["MissingETData_antikt6","LinearityTool_antikt6"],
    ["MissingETData_antikt6_inversion","LinearityTool_antikt6_inversion"],
    ["MissingETData_cone4_inversion","LinearityTool_cone4_inversion"],
    ["MissingETData_cone7","LinearityTool_cone7"],
    ["MissingETData_cone7_inversion","LinearityTool_cone7_inversion"],
]

ToolSvc += ContainerComparatorTool
print      ContainerComparatorTool
