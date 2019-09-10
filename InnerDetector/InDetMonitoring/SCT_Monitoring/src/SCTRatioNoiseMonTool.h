// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*    @file SCTRatioNoiseMonTool.h
 *   Class declaration for SCTRatioNoiseMonTool
 *
 *
 *
 *    @author Luca Fiorini, based on code from Shaun Roe, Manuel Diaz Gomez
 *    and Maria Jose Casta.
 *
 *
 *
 *
 */

#ifndef SCTRATIONOISEMONTOOL_H
#define SCTRATIONOISEMONTOOL_H

#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "SCT_MonitoringNumbers.h"

#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetConditionsSummaryService/IInDetConditionsTool.h"
#include "StoreGate/ReadHandleKey.h"

#include "GaudiKernel/ToolHandle.h"

#include <string>
#include <vector>

// Forward declarations
class TProfile2D;
class TProfile;
class TH1F_LW;
class TH2F_LW;
class SCT_ID;

class SCTRatioNoiseMonTool : public ManagedMonitorToolBase {
 public:
  SCTRatioNoiseMonTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SCTRatioNoiseMonTool() = default;
  // initialize
  virtual StatusCode initialize() override final;
   /**    @name Book, fill & check (reimplemented from baseclass) */
//@{
  ///Book histograms in initialization
  virtual StatusCode bookHistogramsRecurrent() override final;
  virtual StatusCode bookHistograms() override final;
  ///Fill histograms in each loop
  virtual StatusCode fillHistograms() override final;
  ///process histograms at the end (we only use 'isEndOfRun')
  virtual StatusCode procHistograms() override final;
  ///helper function used in procHistograms
  virtual StatusCode checkHists(bool fromFinalize) override final;
//@}

private:
  int m_numberOfEvents{0};

  int m_nNoSides_ev{0};
  int m_nOneSide_ev{0};
  int m_nTwoSide_ev{0};
  int m_nNoSidesBarrel_ev[SCT_Monitoring::N_BARRELS]{};
  int m_nOneSideBarrel_ev[SCT_Monitoring::N_BARRELS]{};
  int m_nTwoSideBarrel_ev[SCT_Monitoring::N_BARRELS]{};
  int m_nNonGoodModulesBarrel_ev[SCT_Monitoring::N_DISKS]{};
  int m_nNoSidesEndcapA_ev[SCT_Monitoring::N_DISKS]{};
  int m_nOneSideEndcapA_ev[SCT_Monitoring::N_DISKS]{};
  int m_nTwoSideEndcapA_ev[SCT_Monitoring::N_DISKS]{};
  int m_nNonGoodModulesEndcapA_ev[SCT_Monitoring::N_DISKS]{};
  int m_nNoSidesEndcapC_ev[SCT_Monitoring::N_DISKS]{};
  int m_nOneSideEndcapC_ev[SCT_Monitoring::N_DISKS]{};
  int m_nTwoSideEndcapC_ev[SCT_Monitoring::N_DISKS]{};
  int m_nNonGoodModulesEndcapC_ev[SCT_Monitoring::N_DISKS]{};
  
  bool m_noSidesHit{false};
  bool m_oneSideHit{false};
  bool m_twoSidesHit{false};
  bool m_correct_TimeBin{false};

  int m_nNoSides[SCT_Monitoring::N_MOD_BARREL + 2 * SCT_Monitoring::N_MOD_ENDCAPS]{};
  int m_nOneSide[SCT_Monitoring::N_MOD_BARREL + 2 * SCT_Monitoring::N_MOD_ENDCAPS]{};
  int m_nOneSide0[SCT_Monitoring::N_MOD_BARREL + 2 * SCT_Monitoring::N_MOD_ENDCAPS]{};
  int m_nOneSide1[SCT_Monitoring::N_MOD_BARREL + 2 * SCT_Monitoring::N_MOD_ENDCAPS]{};
  int m_nTwoSide[SCT_Monitoring::N_MOD_BARREL + 2 * SCT_Monitoring::N_MOD_ENDCAPS]{};
  
  int m_nLink0[SCT_Monitoring::N_MOD_BARREL + 2 * SCT_Monitoring::N_MOD_ENDCAPS]{};
  int m_nLink1[SCT_Monitoring::N_MOD_BARREL + 2 * SCT_Monitoring::N_MOD_ENDCAPS]{};
  bool m_goodModules[SCT_Monitoring::N_MOD_BARREL + 2 * SCT_Monitoring::N_MOD_ENDCAPS]{};

  int m_nLayer[SCT_Monitoring::N_MOD_BARREL + 2 * SCT_Monitoring::N_MOD_ENDCAPS]{};
  int m_nEta[SCT_Monitoring::N_MOD_BARREL + 2 * SCT_Monitoring::N_MOD_ENDCAPS]{};
  int m_nPhi[SCT_Monitoring::N_MOD_BARREL + 2 * SCT_Monitoring::N_MOD_ENDCAPS]{};
  int m_nNonGoodModule[SCT_Monitoring::N_MOD_BARREL + 2 * SCT_Monitoring::N_MOD_ENDCAPS]{};

  int m_current_lb{0};
  int m_last_reset_lb{0};
  
  int m_set_timebin{0};
  int m_modNum{0};
  float m_ratio{0.0f};
  float m_ratioside0{0.0f};
  float m_ratioside1{0.0f};
  
  int m_nNoSides_lb[SCT_Monitoring::N_MOD_BARREL + 2 * SCT_Monitoring::N_MOD_ENDCAPS]{};
  int m_nOneSide_lb[SCT_Monitoring::N_MOD_BARREL + 2 * SCT_Monitoring::N_MOD_ENDCAPS]{};
  int m_noisyM[SCT_Monitoring::NBINS_LBs+1]{};

  std::string m_path{""};

  /// Data object name: for the SCT this is "SCT_RDOs"
  SG::ReadHandleKey<SCT_RDO_Container> m_dataObjectName{this, "RDOKey", "SCT_RDOs"};

  //@name Service and Tool members
  //@{
  ToolHandle<IInDetConditionsTool> m_pSummaryTool{this, "conditionsTol",
      "SCT_ConditionsSummaryTool/InDetSCT_ConditionsSummaryTool", "Tool to retrieve SCT Conditions Summary"};
  ///SCT Helper class
  const SCT_ID* m_pSCTHelper{nullptr};
  //@}

  IntegerProperty m_checkrecent{this, "CheckRecent", 1};

  BooleanProperty m_checkBadModules{this, "checkBadModules", true};
  BooleanProperty m_ignore_RDO_cut_online{this, "IgnoreRDOCutOnline", true};
  /// For online monitoring
  IntegerProperty m_checkrate{this, "CheckRate", 1000};

  //@name Histograms related members
  //@{

  std::vector<TProfile2D*> m_pnoiseoccupancymapHistoVectorECC{nullptr};
  std::vector<TProfile2D*> m_pnoiseoccupancymapHistoVectorECCSide0{nullptr};
  std::vector<TProfile2D*> m_pnoiseoccupancymapHistoVectorECCSide1{nullptr};
  std::vector<TProfile2D*> m_pnoiseoccupancymapHistoVectorBAR{nullptr};
  std::vector<TProfile2D*> m_pnoiseoccupancymapHistoVectorBARSide0{nullptr};
  std::vector<TProfile2D*> m_pnoiseoccupancymapHistoVectorBARSide1{nullptr};
  std::vector<TProfile2D*> m_pnoiseoccupancymapHistoVectorECA{nullptr};
  std::vector<TProfile2D*> m_pnoiseoccupancymapHistoVectorECASide0{nullptr};
  std::vector<TProfile2D*> m_pnoiseoccupancymapHistoVectorECASide1{nullptr};
  
  //General Histograms
  TH1F_LW* m_NOEV{nullptr};
  TH1F_LW* m_NOEVBAR[SCT_Monitoring::N_BARRELS]{};
  TH1F_LW* m_NOEVECC[SCT_Monitoring::N_DISKS]{};
  TH1F_LW* m_NOEVECA[SCT_Monitoring::N_DISKS]{};
  TH2F_LW* m_NOEV_RDO{nullptr};
  TH1F_LW* m_side{nullptr};
  TH1F_LW* m_num_RDO{nullptr};
  TH1F_LW* m_NZ1{nullptr};
  TH1F_LW* m_N11{nullptr};
  TH1F_LW* m_N21{nullptr};

  TH1F_LW* m_NZ1BAR[SCT_Monitoring::N_BARRELS]{};
  TH1F_LW* m_N11BAR[SCT_Monitoring::N_BARRELS]{};
  TH1F_LW* m_N21BAR[SCT_Monitoring::N_BARRELS]{};
  TProfile* m_NZ1BAR_vsLB[SCT_Monitoring::N_BARRELS]{};
  TProfile* m_N11BAR_vsLB[SCT_Monitoring::N_BARRELS]{};

  TH1F_LW* m_NZ1ECC[SCT_Monitoring::N_DISKS]{};
  TH1F_LW* m_N11ECC[SCT_Monitoring::N_DISKS]{};
  TH1F_LW* m_N21ECC[SCT_Monitoring::N_DISKS]{};
  TProfile* m_NZ1ECC_vsLB[SCT_Monitoring::N_DISKS]{};
  TProfile* m_N11ECC_vsLB[SCT_Monitoring::N_DISKS]{};
  
  TH1F_LW* m_NZ1ECA[SCT_Monitoring::N_DISKS]{};
  TH1F_LW* m_N11ECA[SCT_Monitoring::N_DISKS]{};
  TH1F_LW* m_N21ECA[SCT_Monitoring::N_DISKS]{};
  TProfile* m_NZ1ECA_vsLB[SCT_Monitoring::N_DISKS]{};
  TProfile* m_N11ECA_vsLB[SCT_Monitoring::N_DISKS]{};

  TH1F_LW* m_NO{nullptr};
  TH1F_LW* m_NOSide{nullptr};
  TProfile* m_NO_vsLB{nullptr};
  TProfile* m_NoisyModules_vsLB{nullptr};
  TH1F_LW* m_NOBAR{nullptr};
  TH1F_LW* m_NOBARSide{nullptr};
  TH1F_LW* m_NOEC{nullptr};
  TH1F_LW* m_NOECSide{nullptr};
  TH1F_LW* m_NOECASide{nullptr};
  TH1F_LW* m_NOECCSide{nullptr};
  TH1F_LW* m_NOEC_Outer{nullptr};
  TH1F_LW* m_NOEC_ShortMiddle{nullptr};
  TH1F_LW* m_NOEC_Inner{nullptr};
  TH1F_LW* m_NOEC_Middle{nullptr};
  TH1F_LW* m_NOBAR_layer[SCT_Monitoring::N_BARRELS]{};
  TProfile* m_NOBAR_layer_vsLB[SCT_Monitoring::N_BARRELS]{};
  TH1F_LW* m_NOECC_disk[SCT_Monitoring::N_DISKS]{};
  TProfile* m_NOECC_disk_vsLB[SCT_Monitoring::N_DISKS]{};
  TH1F_LW* m_NOECA_disk[SCT_Monitoring::N_DISKS]{};
  TProfile* m_NOECA_disk_vsLB[SCT_Monitoring::N_DISKS]{};
  
  TH1F_LW* m_numberHitsinBarrel[SCT_Monitoring::N_BARRELS]{};

  TH1F_LW* m_NZ1_vs_modnum{nullptr};
  TH1F_LW* m_N11_vs_modnum{nullptr};

  //@}

  //@name  Histograms related methods
  //@{
  // Book Histograms
  StatusCode bookRatioNoiseHistos();
  //@}

  //@name Service methods
  //@{
  float calculateNoiseOccupancyUsingRatioMethod(const float numberOneSide, const float numberZeroSide) const;
  float calculateOneSideNoiseOccupancyUsingRatioMethod(const float numberOneSide, const float numberZeroSide) const;
  bool isBarrel(const int moduleNumber) const;
  bool isEndcap(const int moduleNumber) const;
  bool isEndcapA(const int moduleNumber) const;
  bool isEndcapC(const int moduleNumber) const;

  ///Factory + register for the 2D profiles, returns whether successfully registered
  TProfile* pFactory(const std::string& name, const std::string& title, MonGroup& registry,
                  const float lo, const float hi, const unsigned int nbins) const;
  TH1F_LW* h1Factory(const std::string& name, const std::string& title, MonGroup& registry,
                 std::vector<TH1F_LW*>& storageVector, const float lo, const float hi, const unsigned int nbins) const;
  TH1F_LW* h1Factory(const std::string& name, const std::string& title, MonGroup& registry,
                 const float lo, const float hi, const unsigned int nbins) const;
  TH2F_LW* h2Factory(const std::string& name, const std::string& title, MonGroup& registry,
                 const float lo_x, const float hi_x, const unsigned int nbins_x, const float lo_y, const float hi_y, const unsigned int nbins_y) const;
  TProfile2D* prof2Factory(const std::string& name, const std::string& title, const unsigned int&, MonGroup& registry, std::vector<TProfile2D*>& storageVector) const;
 
  //@}
};

#endif
