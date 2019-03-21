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

#include "SCT_Monitoring/SCTMotherTrigMonTool.h"

#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetConditionsSummaryService/IInDetConditionsTool.h"
#include "SCT_Monitoring/SCT_MonitoringNumbers.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrkToolInterfaces/IUpdator.h"
#include "xAODEventInfo/EventInfo.h"

#include "GaudiKernel/ToolHandle.h"

#include <string>
#include <vector>


// Forward declarations
class IInterface;
class TH1I;
class TH1F;
class TH2I;
class TH2F;
class TProfile2D;
class TProfile;
class TH1F_LW;
class TH2F_LW;
class StatusCode;
class SCT_ID;

#define DEBUG(x) ATH_MSG_DEBUG(x)
#define INFO(x) ATH_MSG_INFO(x)
#define WARNING(x) ATH_MSG_WARNING(x)
#define ERROR(x) ATH_MSG_ERROR(x)
#define VERBOSE(x) ATH_MSG_VERBOSE(x)

///Concrete monitoring tool derived from SCTMotherTrigMonTool
class SCTRatioNoiseMonTool : public SCTMotherTrigMonTool{
 public:
  SCTRatioNoiseMonTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SCTRatioNoiseMonTool() = default;
  // initialize
  virtual StatusCode initialize() final;
   /**    @name Book, fill & check (reimplemented from baseclass) */
//@{
  ///Book histograms in initialization
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode bookHistograms();
  ///Fill histograms in each loop
  virtual StatusCode fillHistograms() ;
  ///process histograms at the end (we only use 'isEndOfRun')
  virtual StatusCode procHistograms();
  ///helper function used in procHistograms
  StatusCode checkHists(bool fromFinalize);
//@}


private:
  //@name typedefs centralised to enable easy changing of types
  //@{
  typedef TProfile* Prof_t;
  typedef TProfile2D* Prof2_t;
  typedef TH1F_LW* H1_t;
  typedef TH2F_LW* H2_t;
  typedef TH1I* H1I_t;
  typedef TH2I* H2I_t;
  typedef std::vector<Prof2_t> VecProf2_t;
  typedef std::vector<Prof_t> VecProf_t;
  typedef std::vector<H1_t> VecH1_t;
  typedef std::vector<H2_t> VecH2_t;
  typedef std::vector<H1I_t> VecH1I_t;
  //@}
  int m_eventID;
  int m_numberOfEvents;
  
  
  int m_nNoSides_ev;
  int m_nOneSide_ev;
  int m_nTwoSide_ev;
  int m_nNoSidesBarrel_ev[SCT_Monitoring::N_BARRELS];
  int m_nOneSideBarrel_ev[SCT_Monitoring::N_BARRELS];
  int m_nTwoSideBarrel_ev[SCT_Monitoring::N_BARRELS];
  int m_nNonGoodModulesBarrel_ev[SCT_Monitoring::N_DISKS];
  int m_nNoSidesEndcapA_ev[SCT_Monitoring::N_DISKS];
  int m_nOneSideEndcapA_ev[SCT_Monitoring::N_DISKS];
  int m_nTwoSideEndcapA_ev[SCT_Monitoring::N_DISKS];
  int m_nNonGoodModulesEndcapA_ev[SCT_Monitoring::N_DISKS];
  int m_nNoSidesEndcapC_ev[SCT_Monitoring::N_DISKS];
  int m_nOneSideEndcapC_ev[SCT_Monitoring::N_DISKS];
  int m_nTwoSideEndcapC_ev[SCT_Monitoring::N_DISKS];
  int m_nNonGoodModulesEndcapC_ev[SCT_Monitoring::N_DISKS];
  
  VecProf2_t m_pnoiseoccupancymapHistoVectorECC;
  VecProf2_t m_pnoiseoccupancymapHistoVectorECCSide0;
  VecProf2_t m_pnoiseoccupancymapHistoVectorECCSide1;
  VecProf2_t m_pnoiseoccupancymapHistoVectorBAR;
  VecProf2_t m_pnoiseoccupancymapHistoVectorBARSide0;
  VecProf2_t m_pnoiseoccupancymapHistoVectorBARSide1;
  VecProf2_t m_pnoiseoccupancymapHistoVectorECA;
  VecProf2_t m_pnoiseoccupancymapHistoVectorECASide0;
  VecProf2_t m_pnoiseoccupancymapHistoVectorECASide1;
  
  float m_d1;
  float m_n1;
  float m_n1Barrel[SCT_Monitoring::N_BARRELS];
  float m_n1EndcapA[SCT_Monitoring::N_DISKS];
  float m_n1EndcapC[SCT_Monitoring::N_DISKS];
  float m_d1Barrel[SCT_Monitoring::N_BARRELS];
  float m_d1EndcapA[SCT_Monitoring::N_DISKS];
  float m_d1EndcapC[SCT_Monitoring::N_DISKS];
  
  bool m_noSidesHit;
  bool m_oneSideHit;
  bool m_twoSidesHit;
  bool m_correct_TimeBin;

  int m_nNoSides[SCT_Monitoring::N_MOD_BARREL + 2 * SCT_Monitoring::N_MOD_ENDCAPS];
  int m_nOneSide[SCT_Monitoring::N_MOD_BARREL + 2 * SCT_Monitoring::N_MOD_ENDCAPS];
  int m_nOneSide0[SCT_Monitoring::N_MOD_BARREL + 2 * SCT_Monitoring::N_MOD_ENDCAPS];
  int m_nOneSide1[SCT_Monitoring::N_MOD_BARREL + 2 * SCT_Monitoring::N_MOD_ENDCAPS];
  int m_nTwoSide[SCT_Monitoring::N_MOD_BARREL + 2 * SCT_Monitoring::N_MOD_ENDCAPS];
  
  int m_nLink0[SCT_Monitoring::N_MOD_BARREL + 2 * SCT_Monitoring::N_MOD_ENDCAPS];
  int m_nLink1[SCT_Monitoring::N_MOD_BARREL + 2 * SCT_Monitoring::N_MOD_ENDCAPS];
  bool m_goodModules[SCT_Monitoring::N_MOD_BARREL + 2 * SCT_Monitoring::N_MOD_ENDCAPS];

  int m_nLayer[SCT_Monitoring::N_MOD_BARREL + 2 * SCT_Monitoring::N_MOD_ENDCAPS];
  int m_nEta[SCT_Monitoring::N_MOD_BARREL + 2 * SCT_Monitoring::N_MOD_ENDCAPS];
  int m_nPhi[SCT_Monitoring::N_MOD_BARREL + 2 * SCT_Monitoring::N_MOD_ENDCAPS];
  int m_nNonGoodModule[SCT_Monitoring::N_MOD_BARREL + 2 * SCT_Monitoring::N_MOD_ENDCAPS];

  int m_checkrecent;
  int m_current_lb;
  int m_last_reset_lb;
  
  int m_set_timebin;
  int m_tbin;
  int m_modNum;
  float m_ratio;
  float m_ratioside0;
  float m_ratioside1;
  
  //static const long NBINS_LBs = 2000;
  
  int m_nNoSides_lb[SCT_Monitoring::N_MOD_BARREL + 2 * SCT_Monitoring::N_MOD_ENDCAPS];
  int m_nOneSide_lb[SCT_Monitoring::N_MOD_BARREL + 2 * SCT_Monitoring::N_MOD_ENDCAPS];
  int m_noisyM[SCT_Monitoring::NBINS_LBs+1];
  //@name Histograms related members
  //@{

  //General Histograms
  H1_t m_NOEV;
  H1_t m_NOEVBAR[SCT_Monitoring::N_BARRELS];
  H1_t m_NOEVECC[SCT_Monitoring::N_DISKS];
  H1_t m_NOEVECA[SCT_Monitoring::N_DISKS];
  H2_t m_NOEV_RDO;
  H2_t m_NOEV_Eventnum;
  H1_t m_side;
  H1_t m_num_RDO;
  H1_t m_NZ1;
  H1_t m_N11;
  H1_t m_N21;

  H1_t m_NZ1BAR[SCT_Monitoring::N_BARRELS];
  H1_t m_N11BAR[SCT_Monitoring::N_BARRELS];
  H1_t m_N21BAR[SCT_Monitoring::N_BARRELS];
  Prof_t m_NZ1BAR_vsLB[SCT_Monitoring::N_BARRELS];
  Prof_t m_N11BAR_vsLB[SCT_Monitoring::N_BARRELS];

  H1_t m_NZ1ECC[SCT_Monitoring::N_DISKS];
  H1_t m_N11ECC[SCT_Monitoring::N_DISKS];
  H1_t m_N21ECC[SCT_Monitoring::N_DISKS];
  Prof_t m_NZ1ECC_vsLB[SCT_Monitoring::N_DISKS];
  Prof_t m_N11ECC_vsLB[SCT_Monitoring::N_DISKS];
  
  H1_t m_NZ1ECA[SCT_Monitoring::N_DISKS];
  H1_t m_N11ECA[SCT_Monitoring::N_DISKS];
  H1_t m_N21ECA[SCT_Monitoring::N_DISKS];
  Prof_t m_NZ1ECA_vsLB[SCT_Monitoring::N_DISKS];
  Prof_t m_N11ECA_vsLB[SCT_Monitoring::N_DISKS];

  H1_t m_NO;
  H1_t m_NOSide;
  Prof_t m_NO_vsLB;
  Prof_t m_NoisyModules_vsLB;
  H1_t m_NOBAR;
  H1_t m_NOBARSide;
  H1_t m_NOEC;
  H1_t m_NOECSide;
  H1_t m_NOECASide;
  H1_t m_NOECCSide;
  H1_t m_NOEC_Outer;
  H1_t m_NOEC_ShortMiddle;
  H1_t m_NOEC_Inner;
  H1_t m_NOEC_Middle; 
  H1_t m_NOBAR_layer[SCT_Monitoring::N_BARRELS];
  Prof_t m_NOBAR_layer_vsLB[SCT_Monitoring::N_BARRELS];
  H1_t m_NOECC_disk[SCT_Monitoring::N_DISKS];
  Prof_t m_NOECC_disk_vsLB[SCT_Monitoring::N_DISKS];
  H1_t m_NOECA_disk[SCT_Monitoring::N_DISKS];
  Prof_t m_NOECA_disk_vsLB[SCT_Monitoring::N_DISKS];
  
  H1_t m_numberHitsinBarrel[SCT_Monitoring::N_BARRELS];

  H1_t m_NZ1_vs_modnum;
  H1_t m_N11_vs_modnum;

  std::string m_path;
  //@}
  std::string m_NOTrigger;

  //@name Service members
  //@{
  /// Data object name: for the SCT this is "SCT_RDOs"
  SG::ReadHandleKey<SCT_RDO_Container> m_dataObjectName{this, "RDOKey", "SCT_RDOs"};
  ///SCT Helper class
  const SCT_ID* m_pSCTHelper;
  ToolHandle<IInDetConditionsTool> m_pSummaryTool{this, "conditionsTol",
      "SCT_ConditionsSummaryTool/InDetSCT_ConditionsSummaryTool", "Tool to retrieve SCT Conditions Summary"};
  bool                                     m_checkBadModules;
  bool                                     m_ignore_RDO_cut_online;
  /// For online monitoring
  int m_checkrate;
  //@}
  //@name  Histograms related methods
  //@{
  // Book Histograms
  StatusCode bookRatioNoiseHistos();
  //@}

  //@name Service methods
  //@{
  // Calculate the local angle of incidence
  int findAnglesToWaferSurface ( const float (&vec)[3], const float& sinAlpha, const Identifier& id, float& theta, float& phi );
  float calculateNoiseOccupancyUsingRatioMethod(const float numberOneSide, const float numberZeroSide);
  float calculateOneSideNoiseOccupancyUsingRatioMethod(const float numberOneSide, const float numberZeroSide);
  bool isBarrel(const int moduleNumber);
  bool isEndcap(const int moduleNumber);
  bool isEndcapA(const int moduleNumber);
  bool isEndcapC(const int moduleNumber);

  ///Factory + register for the 2D profiles, returns whether successfully registered
  Prof_t 
    pFactory(const std::string& name, const std::string& title, MonGroup& registry, const float lo, const float hi, const unsigned int nbins);
  H1_t
    h1Factory(const std::string& name, const std::string& title, MonGroup& registry, VecH1_t& storageVector, const float lo, const float hi, const unsigned int nbins);
  H1_t
    h1Factory(const std::string& name, const std::string& title, MonGroup& registry, const float lo, const float hi, const unsigned int nbins);
  H2_t
    h2Factory(const std::string& name, const std::string& title, MonGroup& registry, const float lo_x, const float hi_x, const unsigned int nbins_x, const float lo_y, const float hi_y, const unsigned int nbins_y);
  H1I_t
    h1IFactory(const std::string& name, const std::string& title, MonGroup& registry, VecH1I_t& storageVector, const float lo, const float hi, const unsigned int nbins);
  H1I_t
    h1IFactory(const std::string& name, const std::string& title, MonGroup& registry, const float lo, const float hi, const unsigned int nbins);
  Prof2_t
    prof2Factory(const std::string& name, const std::string& title, const unsigned int&, MonGroup& registry, VecProf2_t& storageVector);
 
  //@}
};

#endif
