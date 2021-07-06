/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SIMULATIONBASE
#ifndef XAOD_ANALYSIS

#include "xAODCaloEvent/CaloClusterKineHelper.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloEvent/CaloClusterCellLink.h"
#include "CaloGeoHelpers/proxim.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloEvent/CaloPrefetch.h"

#include <cmath>

namespace {


// This is factored out in order to allow instantiating the cell loop
// in versions that both use and do not use cell weights.  In the no-weights
// case, the weight is a compile-time constant of 1, so all operations
// involving it will be optimized away.


struct CellAccum
{
  CellAccum (double& the_posNorm,
	     std::array<double,CaloSampling::Unknown>& the_posSamNorm,
             int& the_nBarrel,
             int& the_nEndcap,
             double& the_timeNorm)
    : EnergyInSample(),
      //      NCellsInSample(),  // will be collected in moment maker
      EtaInSample(),
      PhiInSample(),
      MaxEnergyInSample(),
      EtaMaxEnergyInSample(),
      PhiMaxEnergyInSample(),
      PresenceInSample(),
      theNewEnergy(0),
      theNewTime(0),
      theNewSecondTime(0),
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
  //  int    NCellsInSample[CaloSampling::Unknown];
  double EtaInSample[CaloSampling::Unknown];
  double PhiInSample[CaloSampling::Unknown];
  double MaxEnergyInSample[CaloSampling::Unknown];
  double EtaMaxEnergyInSample[CaloSampling::Unknown];
  double PhiMaxEnergyInSample[CaloSampling::Unknown];
  bool PresenceInSample[CaloSampling::Unknown];
  double theNewEnergy;
  double theNewTime;
  double theNewSecondTime;
  double theNewEta;
  double theNewPhi;
  double phi0;

  double& posNorm;
  std::array<double,CaloSampling::Unknown>& posSamNorm;
  int& nBarrel;
  int& nEndcap;
  double& timeNorm;
};


struct AccumWeight {
    double operator() (const CaloClusterCellLink::const_iterator& it) const {return it.weight();}
};


struct AccumNoWeight
{
  double operator() (const CaloClusterCellLink::const_iterator&) const { return 1; }
};



  //FIXME: Compute layer values only if requested
  //FIXME: The template-base optimizaiton might be unnecessary b/c getting the weight of cell is now CPU-cheap
template <class WEIGHT>
inline
void accumCell (const CaloClusterCellLink::const_iterator& cellIt, CellAccum& accum, const WEIGHT& w)
{
  const CaloCell& cell=**cellIt;
  const CaloDetDescrElement* dde = cell.caloDDE();
  if (!dde) return;
  double weight = w (cellIt);
  double fabsweight = fabs (weight);

  double theEnergyNoWeight = cell.e();
  double theEnergy    = weight * theEnergyNoWeight;
  // weight might be negative for removing cells
  // therefore the AbsEnergy can be negative ...
  double theAbsEnergy = weight * fabs(theEnergyNoWeight);
      
  CaloSampling::CaloSample sam = cell.caloDDE()->getSampling();
  accum.PresenceInSample[sam] = true;
  double cellEta = dde->eta();
  double cellPhi = dde->phi();

  accum.EnergyInSample[sam] += theEnergy;
  accum.theNewEnergy += theEnergy;
  // accum.NCellsInSample[sam] += 1;
  double thePhi;
  if (accum.phi0 < -900) {
    thePhi = accum.phi0 = cellPhi;
  }
  else
    thePhi = proxim (cellPhi, accum.phi0);

  accum.theNewEta    += theAbsEnergy * cellEta;
  accum.theNewPhi    += theAbsEnergy * thePhi;
  if ( accum.MaxEnergyInSample[sam] < theEnergy ) {
    accum.MaxEnergyInSample[sam] = theEnergy;
    accum.EtaMaxEnergyInSample[sam] = cellEta;
    accum.PhiMaxEnergyInSample[sam] = cellPhi;
  }

  accum.posSamNorm[sam] += theAbsEnergy;
  accum.posNorm += theAbsEnergy;

  accum.EtaInSample[sam] += theAbsEnergy * cellEta;
  accum.PhiInSample[sam] += theAbsEnergy * thePhi;

  if ( theEnergy != 0 && weight != 0)
  {
    // count cells in endcap
    switch(sam)  {
    case CaloSampling::PreSamplerE:
    case CaloSampling::EME1:
    case CaloSampling::EME2:
    case CaloSampling::EME3:
    case CaloSampling::HEC0:
    case CaloSampling::HEC1:
    case CaloSampling::HEC2:
    case CaloSampling::HEC3:
    case CaloSampling::FCAL0:
    case CaloSampling::FCAL1:
    case CaloSampling::FCAL2:
    case CaloSampling::MINIFCAL0:
    case CaloSampling::MINIFCAL1:
    case CaloSampling::MINIFCAL2:
    case CaloSampling::MINIFCAL3:
      accum.nEndcap++;
      break;
      //Everything else is barrel 
    default:
      accum.nBarrel++;
      break;
    }

    if (sam != CaloSampling::PreSamplerB &&
        sam != CaloSampling::PreSamplerE)
    {
      unsigned int pmask = dde->is_tile() ? 0x8080 : 0x2000;

      // Is time defined?
      if ( cell.provenance() & pmask ) {
        // keep the sign of weight for the time norm in case a cell is removed
        double theTimeNorm      = fabsweight * theEnergy * theEnergyNoWeight;
        accum.theNewTime       += theTimeNorm * cell.time();
	accum.theNewSecondTime += theTimeNorm * cell.time() * cell.time();
        accum.timeNorm         += theTimeNorm;
      }
    }
  }
}


template <class WEIGHT>
inline
void accumCells (const CaloClusterCellLink* cccl, CellAccum& accum, const WEIGHT& w) {


  CaloClusterCellLink::const_iterator it=cccl->begin();
  CaloClusterCellLink::const_iterator it_e=cccl->end();

  for (;it!=it_e;++it) 
    CxxUtils::prefetchObj(*it);
  
  //Reset start iterator
  it=cccl->begin();
  for (;it!=it_e;++it) {
    CaloPrefetch::nextDDE(it, it_e);
    accumCell(it,accum,w);
  }


  /*
  for (const CaloCell* c : cl)
    CxxUtils::prefetchObj (c);

  CaloCluster::cell_iterator it = cl.begin();
  CaloCluster::cell_iterator end = cl.end();
  for (; it != end; ++it) {
    CaloPrefetch::nextDDE(it, end);
    accumCell (**it, accum, w);
  }
  */
}

}//end namespace


void CaloClusterKineHelper::calculateKine(xAOD::CaloCluster* clu, const bool useweight, const bool updateLayers) {
  //static CaloPhiRange range;

  // update global kinematics 
  //
  // for the update of the position the normalization is not a trival
  // thing. The previous implementation used the sum of weighted
  // energies as normalization. This leads to unphysical eta and phi
  // values in case negative energies are added in. The new algorithm
  // takes therefore |E| instead of E which gives the same eta and phi
  // as before for the 2 cases where all cells are negative or all
  // cells are positive. In the mixed case it will give the direction
  // of activity in the calorimeter.

  const CaloClusterCellLink* cccl=clu->getCellLinks();
  if (!cccl || cccl->size()==0) return; //Do nothing if no cells

  double posNorm = 0;
  int nBarrel = 0;
  int nEndcap = 0;
  double timeNorm = 0.;

  std::array<double,CaloSampling::Unknown> posSamNorm;
  std::fill (posSamNorm.begin(), posSamNorm.end(), 0);



  CellAccum accum (posNorm, posSamNorm, nBarrel, nEndcap, timeNorm);
  //accum.theNewPhi = clu->phi(); //????
  if (useweight)
    accumCells (cccl, accum, AccumWeight());
  else
    accumCells (cccl, accum, AccumNoWeight());
  
  if ( posNorm != 0. ) {
    double inorm = 1 / posNorm;
    clu->setEta(accum.theNewEta * inorm);
    clu->setPhi(CaloPhiRange::fix(accum.theNewPhi * inorm));
  }
  else {
    clu->setEta(0);
    clu->setPhi(0);
  }

  clu->setE(accum.theNewEnergy);
  clu->setM(0.0);

  if ( timeNorm != 0. ) {
    clu->setTime(accum.theNewTime/timeNorm);
    // note that standard deviation of cell time distribution is >= 0. 
    // (no particular accommodation for <x^2> < <x><x>!) 
    //    clu->setSecondTime(std::sqrt(std::max(accum.theNewSecondTime/timeNorm-clu->time()*clu->time(),0.)));
    // consistency with other second moments: store variance!
    clu->setSecondTime(accum.theNewSecondTime/timeNorm-clu->time()*clu->time());
  } else { 
    clu->setTime(0.);      
    clu->setSecondTime(0.);
  }


  //Set Sampling pattern:
  uint32_t samplingPattern=0;
  for(int i=0;i<(int)CaloSampling::Unknown;i++) {
    if (accum.PresenceInSample[i]) 
      samplingPattern |= (0x1U<<i);
  }
  if (samplingPattern!=clu->samplingPattern()) { 
    //Clear sampling data (if there is any): Invalidated if the sampling pattern has changed 
    clu->clearSamplingData();
    clu->setSamplingPattern(samplingPattern);
  }

  /////////////////////////////////////
  // Check Sampling Variable Updates //
  /////////////////////////////////////

  if ( !updateLayers ) return;

  //First loop over samplings
  for(int i=0;i<(int)CaloSampling::Unknown;i++) {
    if ( !accum.PresenceInSample[i] ) continue; // check sampling bit
    if ( posSamNorm[i] != 0 ) {
      const double inorm = 1 / posSamNorm[i];
      accum.EtaInSample[i] *= inorm;
      accum.PhiInSample[i] = CaloPhiRange::fix (accum.PhiInSample[i] * inorm);
    }
  }//End loop over sampling

  //Second loop over samplings 
  //Set(Engery/Eta/Phi) methods only allowed once m_samplingPattern is final!
  for(int i=0;i<(int)CaloSampling::Unknown;i++) {
    if ( !accum.PresenceInSample[i] ) continue;
    // check sampling bit
    CaloSampling::CaloSample sam = (CaloSampling::CaloSample)i;
    clu->setEnergy(sam,accum.EnergyInSample[i]);
    // clu->setNCellsPerSampling(sam, accum.NCellsInSample[i]);
    clu->setEta(sam,accum.EtaInSample[i]);
    clu->setPhi(sam,accum.PhiInSample[i]);

    clu->setEmax(sam,accum.MaxEnergyInSample[i]);
    clu->setEtamax(sam,accum.EtaMaxEnergyInSample[i]);
    clu->setPhimax(sam,accum.PhiMaxEnergyInSample[i]);
  }

  }

#endif //not XAOD_ANALYSIS
#endif //not SIMULATIONBASE

