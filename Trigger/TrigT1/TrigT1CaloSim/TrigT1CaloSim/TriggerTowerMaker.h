/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ================================================
// TriggerTowerMaker class description
// ================================================
//
// THIS TEXT TO BE REPLACED BY ATLAS STANDARD FORMAT
//
//
// class: TriggerTowerMaker
//
// Description:
//
//  This is an example Algorithm designed to show new users
//  of Athena how to access and use objects from the TES.
//
//
// ................................................................
//

#ifndef _TRIGGERTOWERMAKER
#define _TRIGGERTOWERMAKER

// STL
#include <string>
#include <vector>

// Athena/Gaudi
#include "GaudiKernel/DataSvc.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/DataObject.h"
#include "AthContainers/DataVector.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IToolSvc.h"
#include "CLHEP/Random/RandomEngine.h"

#include "StoreGate/StoreGateSvc.h"
#include "PileUpTools/PileUpMergeSvc.h"

//Calorimeter includes
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloIdentifier/CaloIdManager.h" 

//Calorimeter tower includes
#include "LArRawEvent/LArTTL1.h"
#include "LArRawEvent/LArTTL1Container.h"
#include "TileConditions/TileInfo.h"
#include "TileEvent/TileTTL1.h"
#include "TileEvent/TileTTL1Container.h"
#include "CaloTTDetDescr/CaloTTDescriptor.h"
#include "CaloTTDetDescr/CaloTTDescrManager.h"
#include "CaloIdentifier/CaloLVL1_ID.h" 
#include "CaloIdentifier/CaloID_Exception.h"
//#include "LArIdentifier/LArHardwareID.h" 

//LVL1 Calo trigger includes
#include "TrigT1CaloEvent/TriggerTower.h"
#include "TrigT1CaloUtils/InternalTriggerTower.h"
#include "TrigT1CaloUtils/TriggerTowerKey.h"
// #include "TrigT1CaloCalibTools/L1CaloTTIdTools.h" 
// #include "TrigT1CaloCalibTools/L1CaloCells2TriggerTowers.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloTTIdTools.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloCells2TriggerTowers.h"
#include "TrigT1CaloToolInterfaces/IL1TriggerTowerTool.h"
#include "TrigT1CaloMappingToolInterfaces/IL1CaloMappingTool.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"

// Include for the configuration service:
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
//#include "TrigConfigSvc/ILVL1ConfigSvc.h"
#include "TrigConfL1Data/CaloInfo.h"

//********************************************
//Default for parameters of this algorithm
//These can be overridden by job options.
//Descriptions are given in the class declaration
//********************************************

class IAtRndmGenSvc;
class ILumiBlockMuTool;

namespace LVL1
{

//class IL1CaloMappingTool;
//class StoreGateSvc;

//                 TriggerTowerMaker class declaration
/**
The TriggerTowerMaker class takes calorimeter cells from the TES and
forms Trigger Towers, which it then places back into the TES  The calorimeter
cells can be produced either by GEANT or other fast simulation packages
- this is defined by setting a parameter 
CellType to 1 for CaloCells, 2 to reprocess TriggerTowers and 3 for LAr/Tile TTL1 input (a simulation of analogue towers);
*/

class TriggerTowerMaker : public AthAlgorithm,
                          public IIncidentListener 
{
  typedef DataVector<TriggerTower> t_TTCollection;
  
  public:

    //-------------------------
    // Constructors/Destructors
    //
    // Athena requires that the constructor takes certain arguments
    // (and passes them directly to the constructor of the base class)
    //-------------------------

    TriggerTowerMaker( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~TriggerTowerMaker();

    //------------------------------------------------------
    // Methods used by Athena to run the algorithm
    //------------------------------------------------------

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

    virtual void handle(const Incident&);

  private:

    // These are undefined to disallow
    TriggerTowerMaker(const TriggerTowerMaker& rhs);
    TriggerTowerMaker& operator=(const TriggerTowerMaker& rhs);

    ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
    ServiceHandle <IAtRndmGenSvc> m_rndGenSvc;
    CLHEP::HepRandomEngine* m_rndmPeds;
    CLHEP::HepRandomEngine* m_rndmADCs;
    ToolHandle<IL1TriggerTowerTool> m_TTtool;
    ToolHandle<IL1CaloMappingTool> m_mappingTool; 
    ToolHandle<ILumiBlockMuTool> m_lumiBlockMuTool;
    PileUpMergeSvc* m_mergeSvc;

    // user-defined names for random number engines - keep streams distinct
    std::string m_pedEngine;
    std::string m_digiEngine;

    // More robust way of summing cells -> towers
    const CaloIdManager* m_caloMgr; 
    const CaloLVL1_ID* m_lvl1Helper; 
  //   L1CaloTTIdTools* m_l1CaloTTIdTools; 
  //   L1CaloCells2TriggerTowers* m_cells2tt;
    ToolHandle<LVL1::IL1CaloTTIdTools> m_l1CaloTTIdTools;
    ToolHandle<LVL1::IL1CaloCells2TriggerTowers> m_cells2tt;
    
    enum CellTypes{CELL=1, TRIGGERTOWERS=2, TTL1=3};
    /** tells TTMaker what type of input to look for.
        To set these put e.g. :
        TriggerTowerMaker.CellType = 3;
        in the jobOptions.txt file.*/
    int m_cellType;
    
    /** used in processing calorimeter towers **/
    
    //const CaloTTDescrManager* m_caloTT_dd_man;
    const CaloLVL1_ID* m_caloId;

    /** Different regions may have different configurations */  
    static const int m_FirLength = 5;
    static const int m_TowerTypes = 6;
    enum TowerTypes { EmB=0, EmEC=1, FcalE=2, Tile=3, Hec=4, FcalH=5 };
    
    /** pedestal uncertainty and ADC noise **/
    double m_pedVar;
    double m_adcVar;
    
    /** digitisation, FIR, LUT calibration parameters **/
    double m_adcStep;
    static const int m_adcMax = 1023;
    double m_pedVal;
    double m_gainCorr;
    bool m_MatchFIR;
    std::vector< std::vector<int> > m_FIRCoeff;
    std::map<unsigned int, double> m_pedValue;
    std::map<unsigned int, int> m_pedSub;
    std::map<unsigned int, int> m_slope;
    std::map<unsigned int, int> m_channelNoiseCuts;
    int m_nDrop[m_TowerTypes];
    static const int m_SaturationValue = 255;

    /** These full-precision versions are needed for decision range setting */
    int m_FIRCal[m_TowerTypes];
    int m_FIRped[m_TowerTypes];
    
    /** BCID control, Saturated BCID */
    int m_EnergyLow;
    int m_EnergyHigh;
    int m_EnergyLevelLow[m_TowerTypes];
    int m_EnergyLevelHigh[m_TowerTypes];
    int m_BcidDecision[3];
    int m_SatOverride[3];
    int m_DecisionSource;
    int m_SatLevel;
    int m_SatLow[m_TowerTypes];
    int m_SatHigh[m_TowerTypes];
    int m_PeakFinderCond;
    
    /**
      BCID simulation if TT generation from cell is chosen
      The bcid inefficiencies are different between had an em energies.
    */

    std::vector<double> m_BcidEffEm;
    std::vector<double> m_BcidEffHad;
    int m_numberOfBcidEffEm;
    int m_numberOfBcidEffHad;
    
    /** Global calibration scale (MeV/count, to optimise performance) */
    double m_digitScale;

    /** Flag to disable receiver calibration corrections, i.e. use raw calorimeter scale */
    bool m_DoNotCalibrate;

    /** Flag to enable zero suppression (makes algorithm simulation much
        faster */
    bool m_ZeroSuppress;

    /** LUT strategy selection. Values of other parameters depend on value of this flag */
    bool m_LUTStrategy;

    /** Flag to determine whether round or truncate pedestal in LUT */
    bool m_RoundPed;
    
    /** Receiver gain adjustments  */
    std::vector< std::vector< std::vector<double> > > m_Calib;

    /** Compute LUT slope to return nominal tower ET automatically? */
    bool m_AutoCalibrateLUT;
    
    /** LUT calibration rescaling - either relative to nominal or fully manual,
        depending on the AutoCalibrateLUT setting  */
    std::vector< std::vector< std::vector<double> > > m_CalibLUT;
    
    /** For full granularity eta-binned noise cuts */
    std::vector< std::vector< std::vector<int> > > m_Thresholds;
    
    /** Scale for converting ET -> counts */
    static const int m_MeV = 1000;
    
    /** Convert tile amplitude to MeV */
    double m_TileToMeV;
    
    /** TileTTL1 may come with a pedestal - need to subtract if so */
    double m_TileTTL1Ped;
    /** noise suppression thresholds for EM/had towers */
    int m_emThresh;
    int m_hadThresh;
    int m_thresh[m_TowerTypes];
    /** noise RMS values and eta ranges they apply within - CaloCells only**/
    bool m_towerNoise;
    int m_numberOfEmNoiseRegions;
    std::vector<double> m_EtaEmNoise;
    std::vector<double> m_EmNoise;
    int m_numberOfHadNoiseRegions;
    std::vector<double> m_EtaHadNoise;
    std::vector<double> m_HadNoise;

    bool m_EnableBcidStudy;

    /** Pedestal to subtract from overlay digits */
    float m_overlayPed;
   
    /** this map object holds the trigger towers :
    the towers each have a "key" (in this case
    an integer) which uniquely identifies them. */
    std::map<int, InternalTriggerTower*> * m_IntTTContainer;
    std::map<int, TriggerTower*> * m_TTContainer;
    /** Multi-slice mode stuff */
    TriggerTowerMap_t* m_ADCContainer;
    int m_timeslicesLUT;
    int m_sliceLUT;
    std::string m_fullADCLocation;
    /** location of  CaloCells in TES */
    std::string m_CaloCellLocation;
    /** locations within the TES to store collections of Trigger Towers*/
    std::string   m_outputLocation ;
    /**  location of input TriggerTowers (for reprocessing)*/
    std::string m_inputTTLocation;
    /** location of LAr TTL1 data */
    std::string m_EmTTL1ContainerName;   
    std::string m_HadTTL1ContainerName; 
    std::string m_TileTTL1ContainerName; 
    /** flag to control overlay */
    bool m_doOverlay;
    /** require all calorimeters, or allow running with some missing? */
    bool m_requireAllCalos;
    /** enumeration of tower types for different purposes */
    enum towerType {
      normal = 0,
      gap_scint = 1,
      mbias_scint = 2
    };
  
    //long m_eventNumber;
    
    /**
      instead of calculating the expretion:
      double theta =2.*atan(exp(-fabs(cell_eta)));
      cell_energy=sin(theta)*cell_energy;
      for each em and had TT again, we will use a hash table
    */
    double* m_sinThetaHash;
    unsigned int m_maxIetaBins; // max numbers of eta regions if we choose a granularity of 0.1 in eta

    /**
       First attempt at a dead channel list.
       Whatever form the channels are read from, a std::map is an efficient way of finding
       whether a specific tower is on the list
    */
    std::map<int,int> m_badEMTowers;
    std::map<int,int> m_badHadTowers;

    /**
       Now this bit is a hack, to allow us to read in the edges of dead region(s)
       to simulate the LAr hole(s)
    */
    std::vector< std::vector<double> > m_badEMRegions;
    std::vector< std::vector<double> > m_badHadRegions;
 
    // Pedestal correction
    bool m_elementFir;
    bool m_correctFir;
    static const int m_nElement = 33; // eta bins 
    static const int m_nLayer = 2;    // em and had layers

    std::vector< std::vector< std::vector<int> > > m_FIRCoeffElement; //  layer(em/had), element(eta bin), FIR coefficients(for 5 samples)
    int m_nDropElement[m_nLayer][m_nElement]; //  layer (em/had), element (eta bin)
    int m_FIRCalElement[m_nLayer][m_nElement];
    int m_FIRpedElement[m_nLayer][m_nElement];
    int m_slopeElement[m_nLayer][m_nElement];
    int m_FIRsumElement[m_nLayer][m_nElement];
    int m_offsetElement[m_nLayer][m_nElement]; // for FCal23 mapping
    int m_threshElement[m_nLayer][m_nElement]; // for FCal23 mapping

    // methods

    /** Pulse initialisation */
    StatusCode initPulse();
    /** Update LAr pulse parameters */
    std::vector<double> initLArEm(std::vector <double>& Pulse);
    std::vector<double> initLArHec(std::vector <double>& Pulse);
    std::vector<double> initLArFcal(std::vector <double>& Pulse, int Module);
    /** Update Tile pulse parameters */
    std::vector<double> initTile(std::vector <double>& Pulse);
    /** Impulse initialisation (inputs without pulse history) */
    StatusCode initDelta();
    /** Match FIR coefficients to pulse shape */
    std::vector<int> matchFIR(const std::vector <double>& Pulse);
    std::vector<int> calcFIR(const std::vector<double>& Pulse, int max);
    /** Initialise LUTs */
    void initLUTs(const std::vector< std::vector<double> >& Pulses);
    /** Initialise saturated pulse BCID */
    void initSatBCID(int cal, const std::vector<double>& Pulse);
    
    /** Compute L1CaloCoolChannelId (including support for old geometries) */
    L1CaloCoolChannelId channelId(double eta, double phi, int layer);
    /** fetches calorimeter cells */
    StatusCode getCaloCells();
    /** rescale em cell energy (CaloCell calibration tuning) */
    double emCalib(double tt_eta);
    /** gets collection of input TriggerTowers for reprocessing */
    StatusCode getTriggerTowers();
    /** fetch Calorimeter Towers */
    StatusCode getCaloTowers();
    
    /** Convert analogue pulses to digits */
    void digitize();
    
    /** Overlay recorded noise event on simulation */
    StatusCode overlay();
    
    /** Simulate PreProcessing on analogue amplitudes */
    void preProcess();
    
    /** Stores Trigger Towers in the TES, at a 
    location defined in m_outputLocation.<p>
    Returns FAILURE if the towers could not be saved. **/
    StatusCode store();
    
    /** steps over Calo cells and creates/fills trigger towers */
    void processCaloCells(const CaloCellContainer * cells);
    
    /** extract amplitudes from TTL1 */
    void processLArTowers(const LArTTL1Container * );
    void processTileTowers(const TileTTL1Container *);
    
    /** returns a pointer to a Trigger Tower, an existing one if it can be found, otherwise create one and return the pointer to that. */
    InternalTriggerTower* findTriggerTower(double tt_phi, double tt_eta, unsigned int key);
    /** functions to extract eta, phi coordinates from calo tower identifiers **/
    double IDeta(Identifier& id);
    double IDphi(Identifier& id);
    /** adds noise to all towers within trigger coverage **/
    void addNoise();
    /** return noise RMS for given calorimeter type and eta **/
    double emNoise(double eta);
    double hadNoise(double eta);
    /** identify TTL1 type (normal, gap, mbias scintillator) **/
    towerType TTL1type(const Identifier& id, const CaloLVL1_ID* caloId) const;
    /** functions to extract eta, phi coordinates from calo tower identifiers **/
    double IDeta(const Identifier& id, const CaloLVL1_ID* caloId);
    double IDphi(const Identifier& id, const CaloLVL1_ID* caloId);
    /** function to return calibration scale for tower based on identifier **/
    double calib(const Identifier& id, const CaloLVL1_ID* caloId);
    /** function to return calibration scale for tower based on identifier
        this overload function to the previous one is still an temporary solution,
        to get the same calibration scheme also for trigger towers, which era created from
        calo cells. 
        This function should be removed, as soon as possible **/
    double calib(const double tt_eta, const TowerTypes TTType ); 
    /** function to return LUT slope for tower based on identifier **/
    int slope(const Identifier& id, const CaloLVL1_ID* caloId);
   
    /** Functions to simulate processing of tower signals **/
    std::vector<int> ADC(L1CaloCoolChannelId channel, const std::vector<double>& amps);
    int EtRange(int et, int type);
    /** return digits for current LUT slice */
    std::vector<int> multiSliceDigits(const InternalTriggerTower* itt, int type);
    /** set up map of extended ADC digits for multi-slice mode */
    void setupADCMap();
    /** noise for extra slices */
    void sliceNoise(std::vector<double>& emAmps,
                    std::vector<double>& hadAmps, double eta);
    /** Pedestal Correction */
    int  IetaToElement(int eta, int layer);
    void initLUTsPrepare(const std::vector< std::vector< std::vector<double> > >& PulsesElement);

};

} // end of namespace bracket
#endif
