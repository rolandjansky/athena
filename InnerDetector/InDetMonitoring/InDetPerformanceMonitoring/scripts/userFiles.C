#include <iostream>
#include <string>
#include <list>
#include "TROOT.h"
#include "TStyle.h"
std::list <std::string> userFiles() {
  
  std::list <std::string > s_fileNames;
  
  bool debug = true;
  bool JpsiDataSamples2015 = false;
  bool JpsiDataSamples2016 = false;
  bool JpsimumuMC2015 = false;
  bool ZmumuMC2015 = false;
  bool UpsilonmumuMC2015 = false;
  bool DataSamplespreTSI2016 = false;

  //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/Zmaps_20.7.7.9/20.7.7.9/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/share/ZmumuValidationUserSel.root");
  // s_fileNames.push_back("../dynamicAlignment2016/initial_00302872/ZmumuValidationUserSel_Merged_NoF-292-initial-.root");

  //s_fileNames.push_back("/afs/cern.ch/work/s/sthenkel/public/forSalva/filesForZMassBias/preTSI_dynamicAlignment_2016/initial/ZmumuValidationUserSel_Merged_NoF-851-preInitial-.root");
  //s_fileNames.push_back("/afs/cern.ch/work/s/sthenkel/public/forSalva/filesForZMassBias/preTSI_dynamicAlignment_2016/initial/ZmumuValidationUserSel_Merged_NoF-851-preInitial-.root");
  //s_fileNames.push_back("/afs/cern.ch/work/s/sthenkel/public/forSalva/filesForZMassBias/preTSI_dynamicAlignment_2016/alignment_iterations/After_L3Iter0_merged.root");
   //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/user.oestrada.test7recommendations.00302829.physics_Main.merge.DESDM_MCP.f711_m1624_ZmumuValidationUserSel/user.oestrada.9707515.ZmumuValidationUserSel._000001.root");

   //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/IPStudies_20.7.7.9/IPstudies/compare/Run300655.root");
   //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/IPStudies_20.7.7.9/IPstudies/compare/Run300600.root"); 
   //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/IPStudies_20.7.7.9/IPstudies/compare/Run300655_DESDZMUMU.root");
   //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/IPStudies_20.7.7.9/IPstudies/compare/Run300600_DESDMZMUMU.root"); 
   //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/IPNtuples/user.oestrada.RUNFINALconfig..00302265.merge.DESDM_MCP_ZmumuValidationUserSel/MergedRunFinalConfig.root");
   //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/IPNtuples/group.perf-idtracking.RUNFINALtests..00302265.merge.DESDM_MCP_ZmumuValidationUserSel/MergedRunFinalConfig.root");
   //
   //
   //
   //
   
  //***********************Big periods files for Recommendations
  //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_recommendations/preTSI/MergedPreTSI.root");
  //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_recommendations/pred0fix/MergedPreD0fix.root");
  //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_recommendations/pred0fix/MergedPreD0fix_v2.root.root");
  //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_recommendations/postd0fix/MergedPostD0fixv2.root.root");
  //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_recommendations/postd0fix_AOD/MergedPostD0fix_AOD.root");

   //************Periods Studies for Recommendations
  //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_recommendations/PeriodA/MergedPeriodA.root");
  //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_recommendations/PeriodB/MergedPeriodB.root");
  //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_recommendations/PeriodC/MergedPeriodC.root");
  //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_recommendations/PeriodD/MergedPeriodD.root");
  //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_recommendations/PeriodE/MergedPeriodE.root");
  //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_recommendations/PeriodF/MergedPeriodF.root");
  //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_recommendations/PeriodF2/MergedPeriodF2.root");
  //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_recommendations/PeriodG/MergedPeriodGv2.root.root");
  //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_recommendations/PeriodI/MergedPeriodIv2.root.root");
  //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_recommendations/PeriodK/MergedPeriodKv2.root.root");
  //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_recommendations/PeriodL/MergedPeriodLv2.root.root");

  //*****Rel21 tuples
  //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/Rel21Samples/group.perf-idtracking.AlBaseValidationMapsAllEvtsV2..00302393.recon.DESDM_ZMUMU_ZmumuValidationUserSel/MergedRun302393.root"); 
  //****NewAlBase*****
  //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/Rel21Samples/NewAlBase/group.perf-idtracking.NewAlignBaseV2..00300687.recon.DAOD_ZMUMU_ZmumuValidationUserSel/Run300687_DAOD.root");
  //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/Rel21Samples/NewAlBase/group.perf-idtracking.NewAlignBaseV2..00300800.recon.DAOD_ZMUMU_ZmumuValidationUserSel/Run300800_DAOD.root");
  //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/Rel21Samples/NewAlBase/group.perf-idtracking.NewAlignBaseV2..00302393.recon.DAOD_ZMUMU_ZmumuValidationUserSel/Run302393_DAOD.root");
  //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/Rel21Samples/NewAlBase/group.perf-idtracking.NewAlignBaseV2..00300800.recon.DESDM_ZMUMU_ZmumuValidationUserSel/Run300800DESDM.root");
  //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/Rel21Samples/NewAlBase/group.perf-idtracking.NewAlignBaseV2..00302393.recon.DESDM_ZMUMU_ZmumuValidationUserSel/Run302393_DESDM.root");
  //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/IPStudies_20.7.7.9/IPstudies/compare/Run300655.root");
  //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/IPStudies_20.7.7.9/IPstudies/compare/Run300600.root");


   //
   //
   ///////////////****IP Ntuples*****///////////////////////////////
   //
   //Data (pre-TSI, pre-d0 fix, post-d0 fix)
   //
   //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/IPNtuples/MergedPostd0fixZMUMU_IPStudies.root");
   //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/IPNtuples/MergedPred0fix_IPStudies.root");
   //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/IPNtuples/MergedPreTSI_IPStudies.root");
   //
   //MC
   //
   //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/IPNtuples/user.oestrada.MCTESTFINAL..361107.merge.AOD/MergedZmumuMC.root");
   //s_fileNames.push_back("/afs/cern.ch/user/m/martis/mywork/ZmumuNtuples/MarinaNtuples/ZmumuMC_ntuple.root");

   
    //****************************PAPER NTUPLES*********************************************
    //20.7 Runs
    //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/PaperNtuples/Run284285_20_7_AOD.root");
    //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/PaperNtuples/Run304008_20_7_AOD.root");
    //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/PaperNtuples/Run306310_20_7_AOD.root");
    //
    //MC
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/PaperNtuples/MergedJpsimumuMC.root");
    //
    //
    //EOS files
    //s_fileNames.push_back("/afs/cern.ch/user/o/oestrada/eos/atlas/user/o/oestrada/PaperNtuples/Run284285_20_7_AOD.root");
    //s_fileNames.push_back("/afs/cern.ch/user/o/oestrada/eos/atlas/user/o/oestrada/PaperNtuples/Run304008_20_7_AOD.root");
    //s_fileNames.push_back("/afs/cern.ch/user/o/oestrada/eos/atlas/user/o/oestrada/PaperNtuples/Run306310_20_7_AOD.root");
    //s_fileNames.push_back("/afs/cern.ch/user/o/oestrada/eos/atlas/user/o/oestrada/PaperNtuples/Run284285_21_AOD.root");
    //s_fileNames.push_back("/afs/cern.ch/user/o/oestrada/eos/atlas/user/o/oestrada/PaperNtuples/Run304008_21_AOD.root");
    //s_fileNames.push_back("/afs/cern.ch/user/o/oestrada/eos/atlas/user/o/oestrada/PaperNtuples/Run306310_21_AOD.root");
    //s_fileNames.push_back("/afs/cern.ch/user/o/oestrada/eos/atlas/user/o/oestrada/PaperNtuples/Run304008_20_7_BPHYS.root");
    //s_fileNames.push_back("/afs/cern.ch/user/o/oestrada/eos/atlas/user/o/oestrada/PaperNtuples/Run306310_20_7_BPHYS.root");
    //
    //MC
    //
    //s_fileNames.push_back("/afs/cern.ch/user/o/oestrada/eos/atlas/user/o/oestrada/JpsimumuMC_Paper/user.oestrada.papermcv9..424100.merge.AOD_ZmumuValidationUserSel/MergedJpsimumuMC.root");

    if (JpsiDataSamples2015) {
    if (debug) std::cout << " ** userFiles ** reading JpsiDataSamples2015 sample " << std::endl;
    // Period E
    s_fileNames.push_back("/afs/cern.ch/work/m/martis/public/athena/JPsiNtuples/data15_13TeV/Run278748_Zmumu_r7562.root");
    s_fileNames.push_back("/afs/cern.ch/work/m/martis/public/athena/JPsiNtuples/data15_13TeV/Run278880_Zmumu_r7562.root");
    s_fileNames.push_back("/afs/cern.ch/work/m/martis/public/athena/JPsiNtuples/data15_13TeV/Run278912_Zmumu_r7562.root");
    s_fileNames.push_back("/afs/cern.ch/work/m/martis/public/athena/JPsiNtuples/data15_13TeV/Run278968_Zmumu_r7562.root");
    s_fileNames.push_back("/afs/cern.ch/work/m/martis/public/athena/JPsiNtuples/data15_13TeV/Run278970_Zmumu_r7562.root");
    s_fileNames.push_back("/afs/cern.ch/work/m/martis/public/athena/JPsiNtuples/data15_13TeV/Run279169_Zmumu_r7562.root");
    s_fileNames.push_back("/afs/cern.ch/work/m/martis/public/athena/JPsiNtuples/data15_13TeV/Run279259_Zmumu_r7562.root");
    s_fileNames.push_back("/afs/cern.ch/work/m/martis/public/athena/JPsiNtuples/data15_13TeV/Run279279_Zmumu_r7562.root");
    s_fileNames.push_back("/afs/cern.ch/work/m/martis/public/athena/JPsiNtuples/data15_13TeV/Run279284_Zmumu_r7511.root");
    s_fileNames.push_back("/afs/cern.ch/work/m/martis/public/athena/JPsiNtuples/data15_13TeV/Run279345_Zmumu_r7511.root");
    s_fileNames.push_back("/afs/cern.ch/work/m/martis/public/athena/JPsiNtuples/data15_13TeV/Run279515_Zmumu_r7562.root");
    s_fileNames.push_back("/afs/cern.ch/work/m/martis/public/athena/JPsiNtuples/data15_13TeV/Run279598_Zmumu_r7511.root");
    s_fileNames.push_back("/afs/cern.ch/work/m/martis/public/athena/JPsiNtuples/data15_13TeV/Run279685_Zmumu_r7511.root");
    s_fileNames.push_back("/afs/cern.ch/work/m/martis/public/athena/JPsiNtuples/data15_13TeV/Run279764_Zmumu_r7562.root");
    s_fileNames.push_back("/afs/cern.ch/work/m/martis/public/athena/JPsiNtuples/data15_13TeV/Run279813_Zmumu_r7511.root");
    s_fileNames.push_back("/afs/cern.ch/work/m/martis/public/athena/JPsiNtuples/data15_13TeV/Run279867_Zmumu_r7562.root");
    // Period F
    s_fileNames.push_back("/afs/cern.ch/work/m/martis/public/athena/JPsiNtuples/data15_13TeV/Run279932_Zmumu_r7562.root");
    s_fileNames.push_back("/afs/cern.ch/work/m/martis/public/athena/JPsiNtuples/data15_13TeV/Run279984_Zmumu_r7511.root");
    s_fileNames.push_back("/afs/cern.ch/work/m/martis/public/athena/JPsiNtuples/data15_13TeV/Run280231_Zmumu_r7562.root");
    s_fileNames.push_back("/afs/cern.ch/work/m/martis/public/athena/JPsiNtuples/data15_13TeV/Run280273_Zmumu_r7562.root");
    s_fileNames.push_back("/afs/cern.ch/work/m/martis/public/athena/JPsiNtuples/data15_13TeV/Run280319_Zmumu_r7511.root");
    s_fileNames.push_back("/afs/cern.ch/work/m/martis/public/athena/JPsiNtuples/data15_13TeV/Run280368_Zmumu_r7511.root");
  }
  
  else if (JpsiDataSamples2016) { 
    if (debug) std::cout << " ** userFiles ** reading JpsiDataSamples2016 sample " << std::endl;
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16/user.oestrada.00296939.physics_Main.merge.DESDM_MCP.f689_m1587_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root"); 
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16/user.oestrada.00297447.physics_Main.merge.DESDM_MCP.f693_m1587_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16/user.oestrada.00298591.physics_Main.merge.DESDM_MCP.f695_m1592_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16/user.oestrada.00302380.physics_Main.merge.DESDM_MCP.f713_m1624_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16/user.oestrada.00298967.physics_Main.merge.DESDM_MCP.f696_m1592_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16/user.oestrada.v2.00298862.physics_Main.merge.DESDM_MCP.f696_m1592_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16/user.oestrada.v2.00300687.physics_Main.merge.DESDM_MCP.f708_m1610_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16/user.oestrada.v2.00300800.physics_Main.merge.DESDM_MCP.f708_m1610_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16/user.oestrada.data16.00300571.physics_Main.merge.DESDM_MCP.f705_m1610_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16/user.oestrada.data16.00300655.physics_Main.merge.DESDM_MCP.f708_m1610_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16/user.oestrada.data16.00300540.physics_Main.merge.DESDM_MCP.f705_m1610_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16/user.oestrada.data16.00298633.physics_Main.merge.DESDM_MCP.f698_m1598_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16/user.oestrada.data16.00300415.physics_Main.merge.DESDM_MCP.f705_m1610_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root"); 
    /*s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16/user.oestrada.data16.00302393.physics_Main.merge.DESDM_MCP.f711_m1624_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16/user.oestrada.data16.00302829.physics_Main.merge.DESDM_MCP.f711_m1624_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16/user.oestrada.data16.00302956.physics_Main.merge.DESDM_MCP.f715_m1624_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16/user.oestrada.data16.00303304.physics_Main.merge.DESDM_MCP.f716_m1624_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16/user.oestrada.data16.00303846.physics_Main.merge.DESDM_MCP.f716_m1624_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16/user.oestrada.data16.00303726.physics_Main.merge.DESDM_MCP.f716_m1624_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16/user.oestrada.data16.00304243.physics_Main.merge.DESDM_MCP.f716_m1624_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16/user.oestrada.data16.00304337.physics_Main.merge.DESDM_MCP.f716_m1624_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root");*/
  }
  
  else if (ZmumuMC2015 == true) { 
    // Z->mumu MC 
      //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/ZmumuMC_20.7.3.8/jobs_500mu_long/atlasb1/mergedOutput/Zmumu_500mu_truth.root");
      //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/ZmumuMC_20.7.3.8/jobs_800mu/atlasb1/mergedOutput/Zmumu_800mu_truth.root");
      //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/ZmumuMC_20.7.3.8/jobs_nominal/atlasb1/mergedOutput/Zmumu_nom_truth.root");
      s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/ZmumuMC_20.7.3.8/Totalnominal.root");
      //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/ZmumuMC_20.7.3.8/jobs_300mu_v2/atlasb1/mergedOutput/Zmumu_300mu_truth.root");
      //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/ZmumuMC_20.7.3.8/jobs_100mu_v2/atlasb1/mergedOutput/Zmumu_100mu_truth.root");
      //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/ZmumuMC_20.7.3.8/jobs_600mu_v2/atlasb1/mergedOutput/Zmumu_600mu_truth.root");


  }
  
  else if (JpsimumuMC2015) {
    if (debug) std::cout << " ** userFiles ** reading JpsimumuMC2015 sample " << std::endl;
    // JPsi -> mumu MC
      s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/user.oestrada.mctestJpsimu4mu4.recon.ESD.e4397_s2608_r7573_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root");
      //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/user.oestrada.mctestjpsimu4mu20.recon.ESD.e4397_s2608_r7573_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root");
      //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/user.oestrada.mctestjpsimu20mu20.recon.ESD.e4397_s2608_r7573_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root");
      //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/user.oestrada.mctest3bbjpsimu4mu4.recon.ESD.e4397_s2608_r7602_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root");
      //****ELLIPTICAL misalignment geometry*************//
      //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/user.oestrada.mcellipsticalmisalignmentjpsimu4mu4.recon.ESD.e4397_s2608_r7573_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root");
      //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/user.oestrada.mcellipsticalmisalignment300mujpsimu4mu4.recon.ESD.e4397_s2608_r7573_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root");
      //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/user.oestrada.mcellipticalmisalignment500muojpsimu4mu4.recon.ESD.e4397_s2608_r7573_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root"); 
      //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/user.oestrada.radialmisalignmentm500mujpsimu4mu4.recon.ESD.e4397_s2608_r7573_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root");
      //****Giulias geometries******//
      //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/user.oestrada.mcellipsticalmisalignment500mujpsimu4mu4.recon.ESD.e4397_s2608_r7573_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root"); 
      //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/user.oestrada.mcellipsticalmisalignment800mujpsimu4mu4.recon.ESD.e4397_s2608_r7573_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root"); 
      //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/user.oestrada.mcradialmisalignment500mujpsimu4mu4.recon.ESD.e4397_s2608_r7573_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root"); 
      //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/user.oestrada.mcradialmisalignment800mujpsimu4mu4.recon.ESD.e4397_s2608_r7573_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root"); 
      //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/user.oestrada.mczmisalignment200mujpsimu4mu4.recon.ESD.e4397_s2608_r7573_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root"); 
      //s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/user.oestrada.mczmisalignment500mujpsimu4mu4.recon.ESD.e4397_s2608_r7573_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root"); 
    }

  else if (UpsilonmumuMC2015 == true) {
    if (debug) std::cout << " ** userFiles ** reading UpsilonmumuMC2015  sample " << std::endl;
    //Upsilon->mumu MC
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/user.oestrada.mctest2upsilonmu4mu4.recon.ESD.e4397_s2608_r7573_ZmumuValidation/mergedOutput/MergedJpsimumuValidation.root");

  }
    
  else if (DataSamplespreTSI2016) {
    if (debug) std::cout << " ** userFiles ** reading DataSamplespreTSI2016 sample " << std::endl;
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_preTSI/user.oestrada.data16preTS1.00298595.physics_Main.merge.DESDM_MCP.f698_m1598_ZmumuValidation/mergedOutput/MergedZmumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_preTSI/user.oestrada.data16preTS1.00298609.physics_Main.merge.DESDM_MCP.f698_m1598_ZmumuValidation/mergedOutput/MergedZmumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_preTSI/user.oestrada.data16preTS1.00298633.physics_Main.merge.DESDM_MCP.f698_m1598_ZmumuValidation/mergedOutput/MergedZmumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_preTSI/user.oestrada.data16preTS1.00298690.physics_Main.merge.DESDM_MCP.f698_m1598_ZmumuValidation/mergedOutput/MergedZmumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_preTSI/user.oestrada.data16preTS1.00298771.physics_Main.merge.DESDM_MCP.f698_m1598_ZmumuValidation/mergedOutput/MergedZmumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_preTSI/user.oestrada.data16preTS1.00298773.physics_Main.merge.DESDM_MCP.f698_m1598_ZmumuValidation/mergedOutput/MergedZmumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_preTSI/user.oestrada.data16preTS1.00298862.physics_Main.merge.DESDM_MCP.f696_m1592_ZmumuValidation/mergedOutput/MergedZmumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_preTSI/user.oestrada.data16preTS1.00298967.physics_Main.merge.DESDM_MCP.f696_m1592_ZmumuValidation/mergedOutput/MergedZmumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_preTSI/user.oestrada.data16preTS1.00299055.physics_Main.merge.DESDM_MCP.f698_m1598_ZmumuValidation/mergedOutput/MergedZmumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_preTSI/user.oestrada.data16preTS1.00299144.physics_Main.merge.DESDM_MCP.f698_m1598_ZmumuValidation/mergedOutput/MergedZmumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_preTSI/user.oestrada.data16preTS1.00299147.physics_Main.merge.DESDM_MCP.f698_m1598_ZmumuValidation/mergedOutput/MergedZmumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_preTSI/user.oestrada.data16preTS1.00299184.physics_Main.merge.DESDM_MCP.f698_m1598_ZmumuValidation/mergedOutput/MergedZmumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_preTSI/user.oestrada.data16preTS1.00299243.physics_Main.merge.DESDM_MCP.f698_m1598_ZmumuValidation/mergedOutput/MergedZmumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_preTSI/user.oestrada.data16preTS1.00300279.physics_Main.merge.DESDM_MCP.f705_m1610_ZmumuValidation/mergedOutput/MergedZmumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_preTSI/user.oestrada.data16preTS1.00300345.physics_Main.merge.DESDM_MCP.f705_m1610_ZmumuValidation/mergedOutput/MergedZmumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_preTSI/user.oestrada.data16preTS1.00300415.physics_Main.merge.DESDM_MCP.f705_m1610_ZmumuValidation/mergedOutput/MergedZmumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_preTSI/user.oestrada.data16preTS1.00300418.physics_Main.merge.DESDM_MCP.f705_m1610_ZmumuValidation/mergedOutput/MergedZmumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_preTSI/user.oestrada.data16preTS1.00300540.physics_Main.merge.DESDM_MCP.f705_m1610_ZmumuValidation/mergedOutput/MergedZmumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_preTSI/user.oestrada.data16preTS1.00300571.physics_Main.merge.DESDM_MCP.f705_m1610_ZmumuValidation/mergedOutput/MergedZmumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_preTSI/user.oestrada.data16preTS1.00300600.physics_Main.merge.DESDM_MCP.f708_m1610_ZmumuValidation/mergedOutput/MergedZmumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_preTSI/user.oestrada.data16preTS1.00300687.physics_Main.merge.DESDM_MCP.f708_m1610_ZmumuValidation/mergedOutput/MergedZmumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_preTSI/user.oestrada.data16preTS1.00300784.physics_Main.merge.DESDM_MCP.f708_m1610_ZmumuValidation/mergedOutput/MergedZmumuValidation.root");
    s_fileNames.push_back("/afs/cern.ch/work/o/oestrada/public/alignment2/JpsiNtuples/data16_preTSI/user.oestrada.data16preTS1.00300863.physics_Main.merge.DESDM_MCP.f708_m1610_ZmumuValidation/mergedOutput/MergedZmumuValidation.root");
    }


    return s_fileNames;
  } 
  
