//Dear emacs, this is -*- c++ -*-
#if !(defined(SIMULATIONBASE) || defined(XAOD_ANALYSIS))
#ifndef CALOCLUSTERKINEHELPER_Truth_H
#define CALOCLUSTERKINEHELPER_Truth_H

#include "xAODCaloEvent/CaloCluster.h"
#include "CaloEvent/CaloClusterCellLink.h"
#include "CaloGeoHelpers/CaloSampling.h"
//#include "xAODCaloEvent/CaloClusterKineHelper_Truth.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloEvent/CaloClusterCellLink.h"
#include "CaloGeoHelpers/proxim.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloEvent/CaloPrefetch.h"
//#include "StoreGate/DataHandle.h"
//#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaBaseComps/AthMemMacros.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthenaBaseComps/AthCheckMacros.h"
#include "AthenaBaseComps/AthMessaging.h"
//#include "StoreGate/ReadHandle.h"
//#include "StoreGate/UpdateHandle.h"
//#include "StoreGate/WriteHandle.h"
//#include "StoreGate/VarHandleProperty.h"
#include "AthenaKernel/IUserDataSvc.h"

/*
struct CellAccum;

//struct CellAccum
typedef struct CellAccum
{
  CellAccum (double& the_posNorm,
       std::array<double,CaloSampling::Unknown>& the_posSamNorm,
             int& the_nBarrel,
             int& the_nEndcap,
             double& the_timeNorm)
    : EnergyInSample(),
      EtaInSample(),
      PhiInSample(),
      MaxEnergyInSample(),
      EtaMaxEnergyInSample(),
      PhiMaxEnergyInSample(),
      PresenceInSample(),
      theNewEnergy(0),
      theNewTime(0),
      theNewEta(0),
      theNewPhi(0),
      phi0(-999),
      posNorm (the_posNorm),
      posSamNorm (the_posSamNorm),
      nBarrel (the_nBarrel),
      nEndcap (the_nEndcap),
      timeNorm (the_timeNorm)
  {}
  double EnergyInSample[CaloSampling::Unknown];
  double EtaInSample[CaloSampling::Unknown];
  double PhiInSample[CaloSampling::Unknown];
  double MaxEnergyInSample[CaloSampling::Unknown];
  double EtaMaxEnergyInSample[CaloSampling::Unknown];
  double PhiMaxEnergyInSample[CaloSampling::Unknown];
  bool PresenceInSample[CaloSampling::Unknown];
  double theNewEnergy;
  double theNewTime;
  double theNewEta;
  double theNewPhi;
  double phi0;

  double& posNorm;
  std::array<double,CaloSampling::Unknown>& posSamNorm;
  int& nBarrel;
  int& nEndcap;
  double& timeNorm;
} CellAccum ;

//template<class WEIGHT>
struct AccumWeight {
//struct AccumWeight;
//typedef struct AccumWeight {
    double operator()(const CaloClusterCellLink::const_iterator& it) const {return it.weight();}
    //double operator()(const CaloClusterCellLink::const_iterator& it) {return it.weight();}
    //double operator()(CaloClusterCellLink::const_iterator& it) {return it.weight();}
//} AccumWeight;
} ;


//template<class WEIGHT>
//struct AccumNoWeight;
//typedef struct AccumNoWeight
struct AccumNoWeight
{
  double operator() (const CaloClusterCellLink::const_iterator&) const { return 1; }
//} AccumNoWeight;
} ;
*/
  /// Helper class to calculate cluster kinematics based on cells
  /// @author Walter Lampl <Walter.Lampl@cern.ch>

class CaloClusterKineHelper_Truth {
 public:

  ///Calculate cluster kinematics from contained cells
  //static void calculateKine(xAOD::CaloCluster* clu, const bool useweight=true, const bool updateLayers=true );
  static void calculateKine(xAOD::CaloCluster* clu, const bool useweight=true, const bool updateLayers=true );
//template <class WEIGHT>
//void accumCell (const CaloClusterCellLink::const_iterator& cellIt, CellAccum& accum, const WEIGHT& w)
//void accumCell (const CaloClusterCellLink::const_iterator& cellIt, const CaloCell* cell, CellAccum& accum, const WEIGHT& w);

//template <class WEIGHT>
//void accumCells (const CaloClusterCellLink* cccl, CellAccum& accum, const WEIGHT& w);
//{

};

#endif
#endif
