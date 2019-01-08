// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ================================================
// Run2TriggerTowerMaker class description
// ================================================
//
// THIS TEXT TO BE REPLACED BY ATLAS STANDARD FORMAT
//
//
// class: Run2TriggerTowerMaker
//
// Description:
//
//The Run2TriggerTowerMaker class takes calorimeter cells from the TES and
// forms Trigger Towers, which it then places back into the TES  The calorimeter
// cells can be produced either by GEANT or other fast simulation packages
// - this is defined by setting a parameter
// CellType to 1 for CaloCells, 2 to reprocess TriggerTowers and 3 for LAr/Tile TTL1 input (a simulation of analogue towers);
//
// ................................................................
//

#ifndef TRIGT1CALOSIM_RUN2TRIGGERTOWERMAKER_H
#define TRIGT1CALOSIM_RUN2TRIGGERTOWERMAKER_H

// STL
#include <array>
#include <map>
#include <string>
#include <utility> // for std::pair, std::make_pair
#include <vector>

// Athena/Gaudi
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthContainers/DataVector.h"
#include "StoreGate/ReadHandleKey.h"
#include "EventInfo/EventInfo.h"
#include "xAODEventInfo/EventInfo.h"

//Calorimeter tower includes
#include "LArRawEvent/LArTTL1Container.h"
#include "TileEvent/TileTTL1Container.h"

//LVL1 Calo trigger includes
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"
#include "TrigT1CaloCalibConditions/L1CaloPprChanDefaults.h"

// EDM include(s)
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "xAODTrigL1Calo/TriggerTowerAuxContainer.h"

// forward decl(s)
class IAtRndmGenSvc;
class ILumiBlockMuTool;
class CaloLVL1_ID;
class CaloTriggerTowerService;
class L1CaloCondSvc;
class L1CaloPprChanCalibContainer;
class L1CaloPprChanCalib;
class L1CaloPprChanDefaults;
class L1CaloDisabledTowersContainer;
class L1CaloDisabledTowers;
class L1CaloPpmDeadChannelsContainer;
class L1CaloPpmDeadChannels;

namespace CLHEP { class HepRandomEngine; }
namespace TrigConf { class ILVL1ConfigSvc; }

namespace LVL1BS {
   class ITrigT1CaloDataAccessV2;
}

namespace LVL1
{
class IL1TriggerTowerTool;
class IL1CaloMappingTool;
class InternalTriggerTower;
class TriggerTower;

class Run2TriggerTowerMaker : public AthAlgorithm, virtual public IIncidentListener
{
public:
  //-------------------------
  // Constructors/Destructors
  //-------------------------
  Run2TriggerTowerMaker(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~Run2TriggerTowerMaker();

  // These are disallowed
  Run2TriggerTowerMaker(const Run2TriggerTowerMaker&) = delete;
  Run2TriggerTowerMaker& operator=(const Run2TriggerTowerMaker&) = delete;

  //------------------------------------------------------
  // Methods used by Athena to run the algorithm
  //------------------------------------------------------
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  void handle(const Incident&);

private:
  // constants
  constexpr static int s_FIRLENGTH = 5; // number of FIR coefficients
  constexpr static int s_MEV = 1000; // scale for converting ET -> counts
  constexpr static unsigned int m_maxIetaBins = 51;   // max numbers of eta regions if we choose a granularity of 0.1 in eta

  // enums  -- SIZE must be the last entry in each enum
  enum CellTypes{CELL=1, TRIGGERTOWERS=2, TTL1=3, SIZE=4};

  // properties
  // user-defined names for random number engines - keep streams distinct
  std::string m_digiEngine;

  // Input for this Algorithm, meaning in enum CellTypes 
  int m_cellType;

  // pedestal uncertainty and ADC noise
  double m_adcVar;

  // digitisation, FIR, LUT calibration parameters
  double m_adcStep;
  double m_gainCorr;
  bool m_decorateFIR;

  //Flag to enable zero suppression (makes algorithm simulation much faster
  bool m_ZeroSuppress;

  // locations within the TES to store collections of Trigger Towers
  std::string m_outputLocation;

  // require all calorimeters, or allow running with some missing?
  bool m_requireAllCalos;

  // database keys for the variousfolders
  std::string m_chanCalibKey;
  std::string m_chanDefaultsKey;
  std::string m_disabledTowersKey;
  std::string m_deadChannelsKey;
  std::string m_chanCalibKeyoverlay;
  std::string m_chanDefaultsKeyoverlay;
  std::string m_disabledTowersKeyoverlay;
  std::string m_deadChannelsKeyoverlay;

  // Tools/Services
  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
  ServiceHandle<IAtRndmGenSvc> m_rndGenSvc;
  ServiceHandle<L1CaloCondSvc> m_condSvc;
  CLHEP::HepRandomEngine* m_rndmADCs; // non owning ptr

  ToolHandle<IL1TriggerTowerTool> m_TTtool;
  ToolHandle<IL1CaloMappingTool> m_mappingTool;
  ToolHandle<ILumiBlockMuTool> m_lumiBlockMuTool;
  ToolHandle<LVL1BS::ITrigT1CaloDataAccessV2> m_bstowertool;

  const CaloLVL1_ID* m_caloId; //non-owning ptr

  // Global calibration scale (MeV/count, to optimise performance)
  double m_digitScale;
  // Global LUT scales
  double m_cpLutScale;
  double m_jepLutScale;
  // conversion factor from tile amplitude to MeV
  double m_TileToMeV;
  // TileTTL1 may come with a pedestal - need to subtract if so
  double m_TileTTL1Ped;

  // flag whether we reprocess data
  // if yes, special treatment of the pedestal correction is necessary
  bool m_isDataReprocessing;
  // Do overlay?
  bool m_doOverlay;
  bool m_isReco;

  // non-owning pointers
  L1CaloPprChanCalibContainer* m_chanCalibContainer = nullptr;
  L1CaloDisabledTowersContainer* m_disabledTowersContainer = nullptr;
  L1CaloPpmDeadChannelsContainer* m_deadChannelsContainer = nullptr;
  L1CaloPprChanDefaults m_chanDefaults;
  L1CaloPprChanCalibContainer* m_chanCalibContaineroverlay = nullptr;
  L1CaloDisabledTowersContainer* m_disabledTowersContaineroverlay = nullptr;
  L1CaloPpmDeadChannelsContainer* m_deadChannelsContaineroverlay = nullptr;
  L1CaloPprChanDefaults m_chanDefaultsoverlay;

  std::unique_ptr<xAOD::TriggerTowerContainer> m_xaodTowers;
  std::unique_ptr<xAOD::TriggerTowerAuxContainer> m_xaodTowersAux;
  std::vector<std::vector<double>> m_xaodTowersAmps; // stores the Amps (vector<double>) for each xaodTower
  std::size_t m_curIndex = 0u;

  /**
     instead of calculating the expression:
     double theta =2.*atan(exp(-fabs(cell_eta)));
     cell_energy=sin(theta)*cell_energy;
     for each em and had TT again, we will use a hash table
  */
  std::array<double, m_maxIetaBins> m_sinThetaHash;

  /** Compute L1CaloCoolChannelId (including support for old geometries) */
  L1CaloCoolChannelId channelId(double eta, double phi, int layer);


  /** gets collection of input TriggerTowers for reprocessing */
  StatusCode getTriggerTowers();
  /** fetch Calorimeter Towers */
  StatusCode getCaloTowers();

  /** Convert analogue pulses to digits */
  void digitize();

  /** Simulate PreProcessing on analogue amplitudes */
  StatusCode preProcess(const int eventBCID);
  StatusCode preProcessTower(const int eventBCID,xAOD::TriggerTower* tower);
  
  /** Add overlay data **/
  virtual StatusCode addOverlay(const int eventBCID);
  virtual StatusCode addOverlay(const int eventBCID,xAOD::TriggerTower* sigTT,xAOD::TriggerTower* ovTT);
  
  /** PreProcess up to LUT in **/
  StatusCode preProcessTower_getLutIn(const int eventBCID,xAOD::TriggerTower* tower,const L1CaloPprChanCalib* db,const std::vector<int>& digits,std::vector<int>& output);
  
  /** calculate LUT out **/
  StatusCode calcLutOutCP(const std::vector<int>& sigLutIn,const L1CaloPprChanCalib* sigDB,const std::vector<int>& ovLutIn,const L1CaloPprChanCalib* ovDB,std::vector<int>& output);
  StatusCode calcLutOutJEP(const std::vector<int>& sigLutIn,const L1CaloPprChanCalib* sigDB,const std::vector<int>& ovLutIn,const L1CaloPprChanCalib* ovDB,std::vector<int>& output);
  void calcCombinedLUT(const std::vector<int>& sigIN,const int sigSlope,const int sigOffset,
                       const std::vector<int>& ovIN,const int ovSlope,const int ovOffset,const int ovNoiseCut,std::vector<int>& output);  
  
  /** Database helper functions for dead and disabled towers **/
  bool IsDeadChannel(const L1CaloPpmDeadChannels* db) const;
  bool IsDisabledChannel(const L1CaloDisabledTowers* db) const;
  bool IsGoodTower(const xAOD::TriggerTower* tt,const L1CaloPpmDeadChannelsContainer* dead,const L1CaloDisabledTowersContainer* disabled) const;

  
  /** normalise the number of ADC digits for overlay **/
  void normaliseDigits(const std::vector<int>& sigDigits,const std::vector<int>& ovDigits,std::vector<int>& normDigits);

  /** Stores Trigger Towers in the TES, at a
      location defined in m_outputLocation.<p>
      Returns FAILURE if the towers could not be saved. **/
  StatusCode store();

  /** extract amplitudes from TTL1 */
  void processLArTowers(const LArTTL1Container * );
  void processTileTowers(const TileTTL1Container *);

  /** functions to extract eta, phi coordinates from calo tower identifiers **/
  double IDeta(const Identifier& id, const CaloLVL1_ID* caloId);
  double IDphi(const Identifier& id, const CaloLVL1_ID* caloId);

  /** Functions to simulate processing of tower signals **/
  std::vector<int> ADC(L1CaloCoolChannelId channel, const std::vector<double>& amps) const;
  int EtRange(int et, unsigned short bcidEnergyRangeLow, unsigned short bcidEnergyRangeHigh) const;

  // void preProcessLayer(int layer, int eventBCID, InternalTriggerTower* tower, std::vector<int>& etResultVector, std::vector<int>& bcidResultVector);
  StatusCode preProcessTower(xAOD::TriggerTower* tower, int eventBCID);

  int etaToElement(float feta, int layer) const;
  
  // non-linear LUT 
  int non_linear_lut(int lutin, unsigned short offset, unsigned short slope, unsigned short noiseCut, unsigned short scale, short par1, short par2, short par3, short par4);  

  // --------------------------------------------------------------------------
  // Read and Write Handlers
  // --------------------------------------------------------------------------
  SG::ReadHandleKey<EventInfo> m_evtKey;
  SG::ReadHandleKey<xAOD::EventInfo> m_xaodevtKey;
  //  location of input TriggerTowers (for reprocessing)
  SG::ReadHandleKey<xAOD::TriggerTowerContainer> m_inputTTLocation;
  // locations within the TES to store collections of Trigger Towers
  //SG::WriteHandleKey<xAOD::TriggerTowerContainer> m_outputLocation;
  //SG::WriteHandleKey<xAOD::TriggerTowerAuxContainer> m_outputAuxLocation;

  // location of LAr TTL1 data
  SG::ReadHandleKey<LArTTL1Container> m_EmTTL1ContainerName;
  SG::ReadHandleKey<LArTTL1Container> m_HadTTL1ContainerName;
  SG::ReadHandleKey<TileTTL1Container> m_TileTTL1ContainerName;
};

} // namespace LVL1
#endif
