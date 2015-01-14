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
#include "DataModel/DataVector.h"

//Calorimeter tower includes
#include "LArRawEvent/LArTTL1Container.h"
#include "TileEvent/TileTTL1Container.h"

//LVL1 Calo trigger includes
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"

// EDM include(s)
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "xAODTrigL1Calo/TriggerTowerAuxContainer.h"

// forward decl(s)
class IAtRndmGenSvc;
class ILumiBlockMuTool;
class CaloLVL1_ID;
class CaloTriggerTowerService;
namespace CLHEP { class HepRandomEngine; }
namespace TrigConf { class ILVL1ConfigSvc; }

namespace LVL1
{
class IL1TriggerTowerTool;
class IL1CaloMappingTool;
class InternalTriggerTower;
class TriggerTower;

class Run2TriggerTowerMaker : public AthAlgorithm, virtual public IIncidentListener
{
  typedef DataVector<TriggerTower> t_TTCollection;

public:
  //-------------------------
  // Constructors/Destructors
  //
  // Athena requires that the constructor takes certain arguments
  // (and passes them directly to the constructor of the base class)
  //-------------------------
  Run2TriggerTowerMaker( const std::string& name, ISvcLocator* pSvcLocator );

  // These are undefined to disallow
  Run2TriggerTowerMaker(const Run2TriggerTowerMaker& rhs) = delete;
  Run2TriggerTowerMaker& operator=(const Run2TriggerTowerMaker& rhs) = delete;

  virtual ~Run2TriggerTowerMaker();

  //------------------------------------------------------
  // Methods used by Athena to run the algorithm
  //------------------------------------------------------
  StatusCode initialize() override;
  StatusCode execute() override;
  StatusCode finalize() override;

  void handle(const Incident&) override;

private:
  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
  ServiceHandle<IAtRndmGenSvc> m_rndGenSvc;
  CLHEP::HepRandomEngine* m_rndmPeds; // non-owning ptr
  CLHEP::HepRandomEngine* m_rndmADCs; // non owning ptr

  ToolHandle<IL1TriggerTowerTool> m_TTtool;
  ToolHandle<IL1CaloMappingTool> m_mappingTool;
  ToolHandle<CaloTriggerTowerService> m_triggerTowerService; // a tool despite its name

  // user-defined names for random number engines - keep streams distinct
  std::string m_pedEngine;
  std::string m_digiEngine;

  enum CellTypes{CELL=1, TRIGGERTOWERS=2, TTL1=3};
  /** tells TTMaker what type of input to look for.
      To set these put e.g. :
      Run2TriggerTowerMaker.CellType = 3;
      in the jobOptions file.*/
  int m_cellType;

  /** used in processing calorimeter towers **/
  const CaloLVL1_ID* m_caloId; //non-owning ptr

  // LumiBlockMuTool
  ToolHandle<ILumiBlockMuTool> m_lumiBlockMuTool;

  /** Different regions may have different configurations */
  constexpr static int s_FIRLENGTH = 5;
  constexpr static int s_TOWERTYPES = 6;
  enum TowerTypes { EmB=0, EmEC=1, FcalE=2, Tile=3, Hec=4, FcalH=5 };

  /** pedestal uncertainty and ADC noise **/
  double m_pedVar;
  double m_adcVar;

  /** digitisation, FIR, LUT calibration parameters **/
  double m_adcStep;
  constexpr static int m_adcMax = 1023;
  double m_pedVal;
  double m_gainCorr;
  bool m_MatchFIR;
  bool m_decorateFIR;

  struct PedestalInfo {
    double val;
    int sub;
  };
  std::map<unsigned int, PedestalInfo> m_pedestals;
  std::map<unsigned int, int> m_channelNoiseCuts;

  constexpr static int m_SaturationValue = 255;

  /** BCID control, Saturated BCID */
  int m_EnergyLow;
  int m_EnergyHigh;
  int m_BcidDecision[3];
  int m_SatOverride[3];
  int m_DecisionSource;
  int m_SatLevel;
  int m_PeakFinderCond;

  /** Global calibration scale (MeV/count, to optimise performance) */
  double m_digitScale;

  /** Global scale for CP-LUT */
  double m_cpLutScale;

  /** Global scale for JEP-LUT */
  double m_jepLutScale;

  /** Flag to enable zero suppression (makes algorithm simulation much
      faster */
  bool m_ZeroSuppress;

  /** LUT strategy selection. Values of other parameters depend on value of this flag */
  bool m_LUTStrategy;

  /** Flag to determine whether round or truncate pedestal in LUT */
  bool m_RoundPed;

  /** Compute LUT slope to return nominal tower ET automatically? */
  bool m_AutoCalibrateLUT;

  /** Scale for converting ET -> counts */
  constexpr static int s_MEV = 1000;

  /** Convert tile amplitude to MeV */
  double m_TileToMeV;

  /** TileTTL1 may come with a pedestal - need to subtract if so */
  double m_TileTTL1Ped;

  /** noise suppression thresholds for EM/had towers */
  int m_emThresh;
  int m_hadThresh;

  /** this map object holds the trigger towers :
      the towers each have a "key" (in this case
      an integer) which uniquely identifies them. */
  // std::map<int, InternalTriggerTower*> * m_IntTTContainer;
  std::map<int, std::unique_ptr<TriggerTower>> m_TTMap;

  /** locations within the TES to store collections of Trigger Towers*/
  std::string m_outputLocation;
  /**  location of input TriggerTowers (for reprocessing)*/
  std::string m_inputTTLocation;
  /** location of LAr TTL1 data */
  std::string m_EmTTL1ContainerName;
  std::string m_HadTTL1ContainerName;
  std::string m_TileTTL1ContainerName;

  std::string m_xOutputLocation;
  std::unique_ptr<xAOD::TriggerTowerContainer> m_xaodTowers;
  std::unique_ptr<xAOD::TriggerTowerAuxContainer> m_xaodTowersAux;
  std::vector<std::vector<double>> m_xaodTowersAmps; // stores the Amps (vector<double>) for each xaodTower
  std::size_t m_curIndex = 0u;

  /** require all calorimeters, or allow running with some missing? */
  bool m_requireAllCalos;

  /**
     instead of calculating the expression:
     double theta =2.*atan(exp(-fabs(cell_eta)));
     cell_energy=sin(theta)*cell_energy;
     for each em and had TT again, we will use a hash table
  */
  constexpr static unsigned int m_maxIetaBins = 51; // max numbers of eta regions if we choose a granularity of 0.1 in eta
  std::array<double, m_maxIetaBins> m_sinThetaHash;

  // Pedestal correction
  bool m_correctFir;
  constexpr static int s_NELEMENT = 33; // eta bins
  constexpr static int s_NLAYER = 2;    // em and had layers

  struct ElementInfo {
    std::vector<int> FIRCoeff;
    int nDrop;
    int FIRCal;
    int FIRped;
    int slope;
    int FIRsum;
    int offset;
    int thresh;

    // BCID decisions
    int EnergyLow;
    int EnergyHigh;
    int SatLow;
    int SatHigh;
  };
  // outer index: layer; inner index: element
  std::array<std::array<ElementInfo, s_NELEMENT>, s_NLAYER> m_elementInfo;

  // the two following should be added to struct ElementInfo, but can't because
  // they need to be initialized by @declareProperty
  /** For full granularity eta-binned noise cuts */
  std::array<std::vector<int>, s_NLAYER> m_ThresholdElement;
  /** LUT calibration rescaling - either relative to nominal or fully manual,
      depending on the AutoCalibrateLUT setting  */
  std::array<std::vector<double>, s_NLAYER> m_CalibLUTElement;

  // threshold for BCID logic per element
  std::array<std::vector<int>, s_NLAYER> m_SatLowElement;
  std::array<std::vector<int>, s_NLAYER> m_SatHighElement;

  // methods
  void handy(int cal, int iLayer, int iElement);

  /** Pulse initialisation */
  StatusCode initPulse();
  /** Update LAr pulse parameters */
  void initLArPulse(std::vector<double>& Pulse, TowerTypes tType, const std::string& dataFile);
  /** Update Tile pulse parameters */
  void initTile(std::vector <double>& Pulse);
  /** Match FIR coefficients to pulse shape */
  std::vector<int> matchFIR(const std::vector <double>& Pulse);
  std::vector<int> calcFIR(const std::vector<double>& Pulse, int max);
  /** Initialise LUTs */
  void initLUTs(const std::vector< std::vector<double> >& Pulses);
  /** Initialise saturated pulse BCID */
  void initSatBCID(TowerTypes cal, const std::vector<double>& Pulse);

  /** Compute L1CaloCoolChannelId (including support for old geometries) */
  L1CaloCoolChannelId channelId(double eta, double phi, int layer);


  /** gets collection of input TriggerTowers for reprocessing */
  StatusCode getTriggerTowers();
  /** fetch Calorimeter Towers */
  StatusCode getCaloTowers();

  /** Convert analogue pulses to digits */
  void digitize();

  /** Simulate PreProcessing on analogue amplitudes */
  StatusCode preProcess();

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
  std::vector<int> ADC(L1CaloCoolChannelId channel, const std::vector<double>& amps);
  int EtRange(int et, const ElementInfo& ei);

  int  etaToElement(float feta, int layer) const;
  std::pair<float,float> elementToEta(int element, int layer) const;
  std::pair<int, int> calToElementRange(TowerTypes cal) const;
  TowerTypes elementToCal(int layer, int element) const;
  void initLUTsElement(TowerTypes cal, const std::vector<double>& pulse);

  // void preProcessLayer(int layer, int m_eventBCID, InternalTriggerTower* tower, std::vector<int>& etResultVector, std::vector<int>& bcidResultVector);
  void preProcessTower(xAOD::TriggerTower* tower, int m_eventBCID);
};



} // end of namespace bracket
#endif
