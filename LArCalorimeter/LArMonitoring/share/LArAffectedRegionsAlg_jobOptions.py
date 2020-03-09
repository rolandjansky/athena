
#AthMonitorCfgHelperOld must be imported in upper level JO    


#define the group names here, as you'll use them multiple times
affectedRegGroupName="LArAffectedRegionsMonGroup"


#here are the helpers
helper_affectedReg = AthMonitorCfgHelperOld(DQMonFlags, affectedRegGroupName)

#then the algorithms
#from AthenaMonitoring.AthenaMonitoringConf import ExampleMonitorAlgorithm
from LArMonitoring.LArMonitoringConf import LArAffectedRegionsAlg
larAffectedRegAlg = helper_affectedReg.addAlgorithm(LArAffectedRegionsAlg,'larAffectedRegAlg')
larAffectedRegAlg.AffectedRegionsGroupName=affectedRegGroupName
larAffectedRegAlg.IsOnline = isOnline

#if you have to add a tool to the algorithm, put it here


#now the groups and histograms
from LArMonitoring import GlobalVariables #to define the ranges

larAffReg_hist_path='AffectedRegions/' #histogram path

#EMBPS
group_name_ending="EMBPS" 
larAffectedRegAlg.EMBPSname=group_name_ending
affectedRegGroupEMBPS = helper_affectedReg.addGroup(
    larAffectedRegAlg,
    affectedRegGroupName+group_name_ending,
    '/LAr/'
)

affectedRegGroupEMBPS.defineHistogram('etaPOS,phi;LArAffectedRegionsEMBAPS',
                                      title='HV Affected Regions - EMBA - Presampler;#eta;#phi',
                                      type='TH2F',
                                      path=larAffReg_hist_path,
                                      weight='problem',
                                      xbins=GlobalVariables.HVeta_EMB["EMBAPS"][0],xmin=GlobalVariables.HVeta_EMB["EMBAPS"][1],xmax=GlobalVariables.HVeta_EMB["EMBAPS"][2],
                                      ybins=GlobalVariables.HVphi_EMB["EMBAPS"][0],ymin=GlobalVariables.HVphi_EMB["EMBAPS"][1],ymax=GlobalVariables.HVphi_EMB["EMBAPS"][2]
)
affectedRegGroupEMBPS.defineHistogram('etaNEG,phi;LArAffectedRegionsEMBCPS',
                                      title='HV Affected Regions - EMBC - Presampler;#eta;#phi',
                                      type='TH2F',
                                      path=larAffReg_hist_path,
                                      weight='problem',
                                      xbins=GlobalVariables.HVeta_EMB["EMBCPS"][0],xmin=GlobalVariables.HVeta_EMB["EMBCPS"][1],xmax=GlobalVariables.HVeta_EMB["EMBCPS"][2],
                                      ybins=GlobalVariables.HVphi_EMB["EMBCPS"][0],ymin=GlobalVariables.HVphi_EMB["EMBCPS"][1],ymax=GlobalVariables.HVphi_EMB["EMBCPS"][2]
)


#EMB
group_name_ending="EMB"
larAffectedRegAlg.EMBname=group_name_ending
affectedRegGroupEMB = helper_affectedReg.addGroup(
    larAffectedRegAlg,
    affectedRegGroupName+group_name_ending,
    '/LAr/'
)

affectedRegGroupEMB.defineHistogram('etaPOS,phi;LArAffectedRegionsEMBA',
                                    title='HV Affected Regions - EMBA - Samplings 1-3;#eta;#phi',
                                    type='TH2F',
                                    path=larAffReg_hist_path,
                                    weight='problem',
                                    xbins=GlobalVariables.HVeta_EMB["EMBA"][0],xmin=GlobalVariables.HVeta_EMB["EMBA"][1],xmax=GlobalVariables.HVeta_EMB["EMBA"][2],
                                    ybins=GlobalVariables.HVphi_EMB["EMBA"][0],ymin=GlobalVariables.HVphi_EMB["EMBA"][1],ymax=GlobalVariables.HVphi_EMB["EMBA"][2]
)
affectedRegGroupEMB.defineHistogram('etaNEG,phi;LArAffectedRegionsEMBC',
                                    title='HV Affected Regions - EMBC - Samplings 1-3;#eta;#phi',
                                    type='TH2F',
                                    path=larAffReg_hist_path,
                                    weight='problem',
                                    xbins=GlobalVariables.HVeta_EMB["EMBC"][0],xmin=GlobalVariables.HVeta_EMB["EMBC"][1],xmax=GlobalVariables.HVeta_EMB["EMBC"][2],
                                    ybins=GlobalVariables.HVphi_EMB["EMBC"][0],ymin=GlobalVariables.HVphi_EMB["EMBC"][1],ymax=GlobalVariables.HVphi_EMB["EMBC"][2]
)


#EMECPS
group_name_ending="EMECPS"
larAffectedRegAlg.EMECPSname=group_name_ending
affectedRegGroupEMECPS = helper_affectedReg.addGroup(
    larAffectedRegAlg,
    affectedRegGroupName+group_name_ending,
    '/LAr/'
)

affectedRegGroupEMECPS.defineHistogram('etaPOS,phi;LArAffectedRegionsEMECAPS',
                                       title='HV Affected Regions - EMECA - Presampler;#eta;#phi',
                                       type='TH2F',
                                       path=larAffReg_hist_path,
                                       weight='problem',
                                       xbins=GlobalVariables.HVeta_EMEC["EMECAPS"],
                                       ybins=GlobalVariables.HVphi_EMEC["EMECAPS"]
)
affectedRegGroupEMECPS.defineHistogram('etaNEG,phi;LArAffectedRegionsEMECCPS',
                                       title='HV Affected Regions - EMECC - Presampler;#eta;#phi',
                                       type='TH2F',
                                       path=larAffReg_hist_path,
                                       weight='problem',
                                       xbins=GlobalVariables.HVeta_EMEC["EMECCPS"],
                                       ybins=GlobalVariables.HVphi_EMEC["EMECCPS"]
)


#EMEC
group_name_ending="EMEC"
larAffectedRegAlg.EMECname=group_name_ending 
affectedRegGroupEMEC = helper_affectedReg.addGroup(
    larAffectedRegAlg,
    affectedRegGroupName+group_name_ending,
    '/LAr/'
)

affectedRegGroupEMEC.defineHistogram('etaPOS,phi;LArAffectedRegionsEMECA',
                                     title='HV Affected Regions - EMECA - Samplings 1-3;#eta;#phi',
                                     type='TH2F',
                                     path=larAffReg_hist_path,
                                     weight='problem',
                                     xbins=GlobalVariables.HVeta_EMEC["EMECA"],
                                     ybins=GlobalVariables.HVphi_EMEC["EMECA"]
)
affectedRegGroupEMEC.defineHistogram('etaNEG,phi;LArAffectedRegionsEMECC',
                                     title='HV Affected Regions - EMECC - Samplings 1-3;#eta;#phi',
                                     type='TH2F',
                                     path=larAffReg_hist_path,
                                     weight='problem',
                                     xbins=GlobalVariables.HVeta_EMEC["EMECC"],
                                     ybins=GlobalVariables.HVphi_EMEC["EMECC"]
)

#HEC0
group_name_ending="HEC0"
larAffectedRegAlg.HEC0name=group_name_ending
affectedRegGroupHEC0 = helper_affectedReg.addGroup(
    larAffectedRegAlg,
    affectedRegGroupName+group_name_ending,
    '/LAr/'
)

affectedRegGroupHEC0.defineHistogram('etaPOS,phi;LArAffectedRegionsHECA0',
                                     title='HV Affected Regions - HECA - Layer 1;#eta;#phi',
                                     type='TH2F',
                                     path=larAffReg_hist_path,
                                     weight='problem',
                                     xbins=GlobalVariables.HVeta_HECFcal["HECA"][0],xmin=GlobalVariables.HVeta_HECFcal["HECA"][1],xmax=GlobalVariables.HVeta_HECFcal["HECA"][2],
                                     ybins=GlobalVariables.HVphi_HECFcal["HECA"][0],ymin=GlobalVariables.HVphi_HECFcal["HECA"][1],ymax=GlobalVariables.HVphi_HECFcal["HECA"][2]
)
affectedRegGroupHEC0.defineHistogram('etaNEG,phi;LArAffectedRegionsHECC0',
                                     title='HV Affected Regions - HECC - Layer 1;#eta;#phi',
                                     type='TH2F',
                                     path=larAffReg_hist_path,
                                     weight='problem',
                                     xbins=GlobalVariables.HVeta_HECFcal["HECC"][0],xmin=GlobalVariables.HVeta_HECFcal["HECC"][1],xmax=GlobalVariables.HVeta_HECFcal["HECC"][2],
                                     ybins=GlobalVariables.HVphi_HECFcal["HECC"][0],ymin=GlobalVariables.HVphi_HECFcal["HECC"][1],ymax=GlobalVariables.HVphi_HECFcal["HECC"][2]
)

#HEC1
group_name_ending="HEC1"
larAffectedRegAlg.HEC1name=group_name_ending
affectedRegGroupHEC1 = helper_affectedReg.addGroup(
    larAffectedRegAlg,
    affectedRegGroupName+group_name_ending,
    '/LAr/'
)
    
affectedRegGroupHEC1.defineHistogram('etaPOS,phi;LArAffectedRegionsHECA1',
                                     title='HV Affected Regions - HECA - Layer 2;#eta;#phi',
                                     type='TH2F',
                                     path=larAffReg_hist_path,
                                     weight='problem',
                                     xbins=GlobalVariables.HVeta_HECFcal["HECA"][0],xmin=GlobalVariables.HVeta_HECFcal["HECA"][1],xmax=GlobalVariables.HVeta_HECFcal["HECA"][2],
                                     ybins=GlobalVariables.HVphi_HECFcal["HECA"][0],ymin=GlobalVariables.HVphi_HECFcal["HECA"][1],ymax=GlobalVariables.HVphi_HECFcal["HECA"][2]
)
affectedRegGroupHEC1.defineHistogram('etaNEG,phi;LArAffectedRegionsHECC1',
                                     title='HV Affected Regions - HECC - Layer 2;#eta;#phi',
                                     type='TH2F',
                                     path=larAffReg_hist_path,
                                     weight='problem',
                                     xbins=GlobalVariables.HVeta_HECFcal["HECC"][0],xmin=GlobalVariables.HVeta_HECFcal["HECC"][1],xmax=GlobalVariables.HVeta_HECFcal["HECC"][2],
                                     ybins=GlobalVariables.HVphi_HECFcal["HECC"][0],ymin=GlobalVariables.HVphi_HECFcal["HECC"][1],ymax=GlobalVariables.HVphi_HECFcal["HECC"][2]
)
    
#HEC2
group_name_ending="HEC2"
larAffectedRegAlg.HEC2name=group_name_ending
affectedRegGroupHEC2 = helper_affectedReg.addGroup(
    larAffectedRegAlg,
    affectedRegGroupName+group_name_ending,
    '/LAr/'
)
    
affectedRegGroupHEC2.defineHistogram('etaPOS,phi;LArAffectedRegionsHECA2',
                                     title='HV Affected Regions - HECA - Layer 3;#eta;#phi',
                                     type='TH2F',
                                     path=larAffReg_hist_path,
                                     weight='problem',
                                     xbins=GlobalVariables.HVeta_HECFcal["HECA"][0],xmin=GlobalVariables.HVeta_HECFcal["HECA"][1],xmax=GlobalVariables.HVeta_HECFcal["HECA"][2],
                                     ybins=GlobalVariables.HVphi_HECFcal["HECA"][0],ymin=GlobalVariables.HVphi_HECFcal["HECA"][1],ymax=GlobalVariables.HVphi_HECFcal["HECA"][2]
)
affectedRegGroupHEC2.defineHistogram('etaNEG,phi;LArAffectedRegionsHECC2',
                                     title='HV Affected Regions - HECC - Layer 3;#eta;#phi',
                                     type='TH2F',
                                     path=larAffReg_hist_path,
                                     weight='problem',
                                     xbins=GlobalVariables.HVeta_HECFcal["HECC"][0],xmin=GlobalVariables.HVeta_HECFcal["HECC"][1],xmax=GlobalVariables.HVeta_HECFcal["HECC"][2],
                                     ybins=GlobalVariables.HVphi_HECFcal["HECC"][0],ymin=GlobalVariables.HVphi_HECFcal["HECC"][1],ymax=GlobalVariables.HVphi_HECFcal["HECC"][2]
)
        
#HEC3
group_name_ending="HEC3"
larAffectedRegAlg.HEC3name=group_name_ending
affectedRegGroupHEC3 = helper_affectedReg.addGroup(
    larAffectedRegAlg,
    affectedRegGroupName+group_name_ending,
    '/LAr/'
)
    
affectedRegGroupHEC3.defineHistogram('etaPOS,phi;LArAffectedRegionsHECA3',
                                     title='HV Affected Regions - HECA - Layer 4;#eta;#phi',
                                     type='TH2F',
                                     path=larAffReg_hist_path,
                                     weight='problem',
                                     xbins=GlobalVariables.HVeta_HECFcal["HECA"][0],xmin=GlobalVariables.HVeta_HECFcal["HECA"][1],xmax=GlobalVariables.HVeta_HECFcal["HECA"][2],
                                     ybins=GlobalVariables.HVphi_HECFcal["HECA"][0],ymin=GlobalVariables.HVphi_HECFcal["HECA"][1],ymax=GlobalVariables.HVphi_HECFcal["HECA"][2]
)
affectedRegGroupHEC3.defineHistogram('etaNEG,phi;LArAffectedRegionsHECC3',
                                     title='HV Affected Regions - HECC - Layer 4;#eta;#phi',
                                     type='TH2F',
                                     path=larAffReg_hist_path,
                                     weight='problem',
                                     xbins=GlobalVariables.HVeta_HECFcal["HECC"][0],xmin=GlobalVariables.HVeta_HECFcal["HECC"][1],xmax=GlobalVariables.HVeta_HECFcal["HECC"][2],
                                     ybins=GlobalVariables.HVphi_HECFcal["HECC"][0],ymin=GlobalVariables.HVphi_HECFcal["HECC"][1],ymax=GlobalVariables.HVphi_HECFcal["HECC"][2]
)
    
#FCAL0
group_name_ending="FCAL0"
larAffectedRegAlg.FCAL0name=group_name_ending
affectedRegGroupFCAL0 = helper_affectedReg.addGroup(
    larAffectedRegAlg,
    affectedRegGroupName+group_name_ending,
    '/LAr/'
)
    
affectedRegGroupFCAL0.defineHistogram('etaPOS,phi;LArAffectedRegionsFCALA0',
                                      title='HV Affected Regions - FCALA - Layer 1;#eta;#phi',
                                      type='TH2F',
                                      path=larAffReg_hist_path,
                                      weight='problem',
                                      xbins=GlobalVariables.HVeta_HECFcal["FCalA"][0],xmin=GlobalVariables.HVeta_HECFcal["FCalA"][1],xmax=GlobalVariables.HVeta_HECFcal["FCalA"][2],
                                      ybins=GlobalVariables.HVphi_HECFcal["FCalA"][0],ymin=GlobalVariables.HVphi_HECFcal["FCalA"][1],ymax=GlobalVariables.HVphi_HECFcal["FCalA"][2]
)
affectedRegGroupFCAL0.defineHistogram('etaNEG,phi;LArAffectedRegionsFCALC0',
                                      title='HV Affected Regions - FCALC - Layer 1;#eta;#phi',
                                      type='TH2F',
                                      path=larAffReg_hist_path,
                                      weight='problem',
                                      xbins=GlobalVariables.HVeta_HECFcal["FCalC"][0],xmin=GlobalVariables.HVeta_HECFcal["FCalC"][1],xmax=GlobalVariables.HVeta_HECFcal["FCalC"][2],
                                      ybins=GlobalVariables.HVphi_HECFcal["FCalC"][0],ymin=GlobalVariables.HVphi_HECFcal["FCalC"][1],ymax=GlobalVariables.HVphi_HECFcal["FCalC"][2]
)

#FCAL1
group_name_ending="FCAL1"
larAffectedRegAlg.FCAL1name=group_name_ending
affectedRegGroupFCAL1 = helper_affectedReg.addGroup(
    larAffectedRegAlg,
    affectedRegGroupName+group_name_ending,
    '/LAr/'
)
    
affectedRegGroupFCAL1.defineHistogram('etaPOS,phi;LArAffectedRegionsFCALA1',
                                      title='HV Affected Regions - FCALA - Layer 2;#eta;#phi',
                                      type='TH2F',
                                      path=larAffReg_hist_path,
                                      weight='problem',
                                      xbins=GlobalVariables.HVeta_HECFcal["FCalA"][0],xmin=GlobalVariables.HVeta_HECFcal["FCalA"][1],xmax=GlobalVariables.HVeta_HECFcal["FCalA"][2],
                                      ybins=GlobalVariables.HVphi_HECFcal["FCalA"][0],ymin=GlobalVariables.HVphi_HECFcal["FCalA"][1],ymax=GlobalVariables.HVphi_HECFcal["FCalA"][2]
)
affectedRegGroupFCAL1.defineHistogram('etaNEG,phi;LArAffectedRegionsFCALC1',
                                      title='HV Affected Regions - FCALC - Layer 2;#eta;#phi',
                                      type='TH2F',
                                      path=larAffReg_hist_path,
                                      weight='problem',
                                      xbins=GlobalVariables.HVeta_HECFcal["FCalC"][0],xmin=GlobalVariables.HVeta_HECFcal["FCalC"][1],xmax=GlobalVariables.HVeta_HECFcal["FCalC"][2],
                                      ybins=GlobalVariables.HVphi_HECFcal["FCalC"][0],ymin=GlobalVariables.HVphi_HECFcal["FCalC"][1],ymax=GlobalVariables.HVphi_HECFcal["FCalC"][2]
)

#FCAL2
group_name_ending="FCAL2"
larAffectedRegAlg.FCAL2name=group_name_ending
affectedRegGroupFCAL2 = helper_affectedReg.addGroup(
    larAffectedRegAlg,
    affectedRegGroupName+group_name_ending,
    '/LAr/'
)

affectedRegGroupFCAL2.defineHistogram('etaPOS,phi;LArAffectedRegionsFCALA2',
                                      title='HV Affected Regions - FCALA - Layer 3;#eta;#phi',
                                      type='TH2F',
                                      path=larAffReg_hist_path,
                                      weight='problem',
                                      xbins=GlobalVariables.HVeta_HECFcal["FCalA"][0],xmin=GlobalVariables.HVeta_HECFcal["FCalA"][1],xmax=GlobalVariables.HVeta_HECFcal["FCalA"][2],
                                      ybins=GlobalVariables.HVphi_HECFcal["FCalA"][0],ymin=GlobalVariables.HVphi_HECFcal["FCalA"][1],ymax=GlobalVariables.HVphi_HECFcal["FCalA"][2]
)
affectedRegGroupFCAL2.defineHistogram('etaNEG,phi;LArAffectedRegionsFCALC2',
                                      title='HV Affected Regions - FCALC - Layer 3;#eta;#phi',
                                      type='TH2F',
                                      path=larAffReg_hist_path,
                                      weight='problem',
                                      xbins=GlobalVariables.HVeta_HECFcal["FCalC"][0],xmin=GlobalVariables.HVeta_HECFcal["FCalC"][1],xmax=GlobalVariables.HVeta_HECFcal["FCalC"][2],
                                      ybins=GlobalVariables.HVphi_HECFcal["FCalC"][0],ymin=GlobalVariables.HVphi_HECFcal["FCalC"][1],ymax=GlobalVariables.HVphi_HECFcal["FCalC"][2]
)

#and finally add all to topSequence
topSequence += helper_affectedReg.result()
