/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************

 NAME:     T2CaloSwSeed.h
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloEgamma

 AUTHOR:   A. Phan

 NOTES:    Based loosely on T2CaloEgamma.cxx and closely to EgammaSamp2Fex.cxx.

 *****************************************************************************/

#ifndef TRIGT2CALOEGAMMA_T2CALOSWSEED_H
#define TRIGT2CALOEGAMMA_T2CALOSWSEED_H

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"

//#include "TrigCaloEvent/TrigEMCluster.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "TrigInterfaces/AllTEAlgo.h"
#include "TrigT2CaloCommon/Calo_Def.h"
#include "TrigT2CaloCommon/TrigDataAccess.h"
#include "TrigT2CaloCommon/T2CaloConfig.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigT2CaloCalibration/IEgammaCalibration.h"

#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "Identifier/IdentifierHash.h"
#include "LArRawUtils/LArTT_Selector.h"
#include "LArRecEvent/LArCell.h"

#include <map>

class IEgammaCalibration;
class IRegionSelector;

class T2CaloSwSeed : public HLT::AllTEAlgo {

  public:

    /** Constructor*/
    T2CaloSwSeed(const std::string & name, ISvcLocator* pSvcLocator);

    /** Destructor */
    virtual ~T2CaloSwSeed();

    /** Steering functions */
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();
    HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& input,
                              unsigned int output);

    /** Algorithm functions */

     virtual bool reset() {
       m_cachedTEList.clear();
       return true;
     }

  protected:

    /** Eta width of Region of Interest. Should be 0.2 for EGamma. */
    double 		      m_etaWidth;

    /** Phi width of Region of Interest. Should be 0.2 for EGamma. */
    double 		      m_phiWidth;

    /** Eta width of Region of Interest for the ID. Should be 0.1 for EGamma. */
    double 		      m_etaWidthForID;

    /** Phi width of Region of Interest for the ID. Should be 0.1 for EGamma. */
    double 		      m_phiWidthForID;

    /** Flag for NoCut chains. Default is false. */
    bool 		        m_noCut;

    /** Window energy threshold for sliding window. */
    double 		      m_windowThreshold;

    /** Noise threshold for mapping cells. */
    double 		      m_noiseThreshold;

    /** Overlap Distance */
    double 		      m_overlap;

    /** StoreGateSvc pointer */
    StoreGateSvc*		 m_storeGate;

    /** Provides data access in a Region of Interest. */
    ToolHandle<ITrigDataAccess> m_data;

    /** Iterators to LArCells. */
    LArTT_Selector<LArCellCont>::const_iterator m_iBegin;
    LArTT_Selector<LArCellCont>::const_iterator m_iEnd;
    LArTT_Selector<LArCellCont>::const_iterator m_it;

    /** Calorimeter Id Manager for calorimeter part determination */
    const DataHandle<CaloIdManager> m_larMgr;

    /** Cached MsgStream */
    MsgStream* m_log;

    /** Properties */
    std::string m_trigEmClusterKey;

    /** Internal caching variables */
    std::multimap<unsigned int,HLT::TriggerElement*> m_cachedTEList;

    /** Internal variables */

    // Algorithm quantities
	std::map < IdentifierHash , const LArCell* > m_cellMap;
	std::map < IdentifierHash , const LArCell* > m_stripMap;
    std::map < IdentifierHash , std::vector<IdentifierHash> > m_neighboursMap;

    std::map < IdentifierHash , double > m_seedMap;
    std::map < IdentifierHash , double > m_windowMap;

    // Monitored variables
    int mm_nRoIs;
    int mm_nCachedRoIs;

    //  Per Roi
    std::vector<int> mm_nCachedClusters;
    std::vector<int> mm_nWindows;
    std::vector<int> mm_nSeeds;
    std::vector<int> mm_nClusters;
    std::vector<int> mm_nOverlaps;

    //  Per Cluster
    std::vector<double> mm_Eta;    // Position of seed
    std::vector<double> mm_Phi;
    std::vector<double> mm_L1dR;
    std::vector<double> mm_L2dR;

    // Seed quantities
    static const int m_maxSeeds = 10;

    std::vector<const LArCell*> m_cellSeeds;
    std::vector<double> m_etaSeeds;
    std::vector<double> m_phiSeeds;

    std::vector<double> m_energy37Lay2;       // sum Et in 3*7 cell
    std::vector<double> m_energy37Lay2NegPhi; // for phi wrap-around
    std::vector<double> m_energy37Lay2PosPhi; // for phi wrap-around
    std::vector<double> m_energy77Lay2;       // sum Et in 7*7 cell
    std::vector<double> m_energy35Lay2;       // sum Et in 3*5 cell
    std::vector<double> m_weightEta1;         // temp variable for width
    std::vector<double> m_weightEta2;         // ditto
    std::vector<double> m_clusterWidth35;     // cluster width in eta

    std::vector<double> m_totalEnergy;
    std::vector<double> m_energySamp2BR;       // samp 2 barrel calibrated energy
    std::vector<double> m_energyRawSamp2BR;    // samp 2 barrel raw energy
    std::vector<double> m_energySamp2EC;       // samp 2 endcap calibrated energy
    std::vector<double> m_energyRawSamp2EC;    // samp 2 endcap raw energy
    std::vector<double> m_energyEta;
    std::vector<double> m_energyPhi;

    std::vector<double> m_energyNegPhi;       // fix phi wrap around
    std::vector<double> m_energyNegPhiConv;
    std::vector<double> m_energyPosPhi;

  private:
    inline void setCaloDetDescrElement(const CaloDetDescrElement *caloDDE);
    inline double etaSizeLArEMSamp2(const double eta, const int calo) const;
    inline double phiSizeLArEMSamp2(const double eta, const int calo) const;

     /** Calibration tools */
     ToolHandleArray<IEgammaCalibration> m_calibs;
     const CaloDetDescrElement* m_caloDDE;

};


inline double T2CaloSwSeed::etaSizeLArEMSamp2(double cellEta, int calo) const{
  double sizeEta;
  if( calo == Calorimeter::EMBAR ){
      if ( fabs(cellEta) < 1.4 ){
        sizeEta = 0.025;
      }else{
        sizeEta = 0.075;
      }
  } else {
      if ( fabs(cellEta) < 2.5 ){
        sizeEta = 0.025;
      } else{
        sizeEta = 0.1;
      }
  }
  return sizeEta;
}


inline double T2CaloSwSeed::phiSizeLArEMSamp2(double cellEta, int calo) const{
  double sizePhi;
  if( calo == Calorimeter::EMBAR ){
    sizePhi = 0.025;
  } else {
      if ( fabs(cellEta) < 2.5 ){
        sizePhi = 0.025;
      } else {
        sizePhi = 0.1;
      }
  }
  return sizePhi;
}


inline void T2CaloSwSeed::setCaloDetDescrElement
  (const CaloDetDescrElement *caloDDE) {m_caloDDE = caloDDE;}

#endif
