/* Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration */
#include "CaloTopoClusterFromTowerHelpers.h"

#include "CaloEvent/CaloCell.h"

#include "xAODCaloEvent/CaloCluster.h"

#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloGeoHelpers/proxim.h"

#include <cmath>

std::string CaloRec::Helpers::fmtMsg(const char* fmt,...) {
      char _buffer[1024];
      va_list args;
      va_start(args,fmt);
      vsprintf(_buffer,fmt,args);
      va_end(args);
      return std::string(_buffer);
}


bool CaloRec::Helpers::cellAccumulator(const CaloCell& rcell,CaloClusterSignalAccumulator& accum,double weight,bool onlyKine)
{
  ///////////////////////////
  // Collecting Kinematics //
  ///////////////////////////


  // required info
  const CaloDetDescrElement* dde = rcell.caloDDE(); if ( dde == 0 ) { return false; }
  // energy and weights
  accum.cluster.cellWeight    = weight;
  accum.cluster.cellAbsWeight = std::fabs(accum.cluster.cellWeight);
  accum.cluster.cellE         = accum.cluster.cellWeight*rcell.e();
  accum.cluster.cellAbsE      = accum.cluster.cellAbsWeight*std::fabs(rcell.e());
  // direction
  double celleta(dde->eta());
  double cellphi(dde->phi());
  // energy sum and normalizations
  accum.cluster.accumE    += accum.cluster.cellE;
  accum.cluster.accumAbsE += accum.cluster.cellAbsE;
  if ( accum.cluster.firstCell ) { 
    accum.cluster.phiSeed = cellphi; accum.cluster.firstCell = false; 
  } else { 
    cellphi = proxim(cellphi,accum.cluster.phiSeed); 
  }
  accum.cluster.accumPhi  += accum.cluster.cellAbsE*cellphi;
  accum.cluster.accumEta  += accum.cluster.cellAbsE*celleta;
  // kine kinmatic updates done
  if ( onlyKine ) return true;
  
  /////////////////////////
  // Sampling Quantities //
  /////////////////////////

  // check if cell has valid sampling 
  CaloSampling::CaloSample sam = dde->getSampling();
  int isam((int)sam);
  int fsam((int)CaloSampling::Unknown);
  if ( isam < 0 || isam >= fsam ) { return false; }
  
  // mark sampling in cluster
  accum.sampling.presenceInSample[isam] = true;
  
  // collecting in samples
  accum.sampling.energyInSample[isam]  += accum.cluster.cellE;
  accum.sampling.posNormInSample[isam] += accum.cluster.cellAbsE;
  accum.sampling.etaInSample[isam]     += accum.cluster.cellAbsE*celleta;
  accum.sampling.phiInSample[isam]     += accum.cluster.cellAbsE*cellphi;
  
  // maximum energies in samplings
  if ( accum.cluster.cellE > accum.sampling.maxEnergyInSample[isam] ) {
    accum.sampling.maxEnergyInSample[isam]    = accum.cluster.cellE;
    accum.sampling.etaMaxEnergyInSample[isam] = celleta;
    accum.sampling.phiMaxEnergyInSample[isam] = cellphi;
  }
  
  // counting cells in barrel and endcap
  if ( accum.cluster.cellE != 0. && accum.cluster.cellWeight != 0. ) { 
    switch (sam) {
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
      accum.cluster.nEndcap++;
      break;
    default:
      accum.cluster.nBarrel++;
      break;
    }
    
    // time where available
    if ( sam != CaloSampling::PreSamplerB && sam != CaloSampling::PreSamplerE ) { 
      unsigned int pmask = dde->is_tile() ? 0x8080 : 0x2000;
      // is time defined?
      if ( rcell.provenance() & pmask ) {
	double tnorm(accum.cluster.cellAbsE*accum.cluster.cellWeight*rcell.e()); // sign of weight preserved!
	accum.cluster.accumTime      += tnorm*rcell.time();
	accum.cluster.accumTimeNorm  += tnorm;
      } // cell ok for time
    } // sampling contributes to time
  } // cell has energy and weight != 0
  return true;
}

bool CaloRec::Helpers::calculateKine(xAOD::CaloCluster* pClus,bool onlyKine)
{
  // input
  if ( pClus == 0 ) { return false; }

  // get cell links
  const CaloClusterCellLink* clk = pClus->getCellLinks();
  if ( clk == 0 || clk->size() == 0 ) { return false; }

  // accumulator object                        ////////////////////////////////////////////////
  CaloClusterSignalAccumulator accum;          // Strict enforcement of valid cell pointers! //
  // accumulate cells                          ////////////////////////////////////////////////
  bool retflg(true); 
  auto citer(clk->begin());
  while ( citer != clk->end() && retflg ) { 
    if ( *citer != 0 ) { retflg = cellAccumulator(**citer,accum,citer.weight(),onlyKine); } ++citer; }
  if ( !retflg ) { return false; }

  // set cluster kinematics: energy & mass
  pClus->setE(accum.cluster.accumE);
  pClus->setM(0.);

  // set cluster kinematics: directions
  if ( accum.cluster.accumAbsE != 0. ) {
    double invPosNorm(1./accum.cluster.accumAbsE);
    pClus->setEta(accum.cluster.accumEta*invPosNorm);
    pClus->setPhi(CaloPhiRange::fix(accum.cluster.accumPhi*invPosNorm));
  } else {
    pClus->setEta(0.);
    pClus->setPhi(0.);
  }

  // bail out if only global kinematice to be updated
  if ( onlyKine ) { return true; }

  // set cluster kinematics: time
  if ( accum.cluster.accumTimeNorm != 0. ) {
    pClus->setTime(accum.cluster.accumTime/accum.cluster.accumTimeNorm);
  } else {
    pClus->setTime(0.);
  }

  // set sampling pattern
  uint32_t samplingPattern(0);
  for ( int i(0); i<(int)CaloSampling::Unknown; ++i ) {
    if ( accum.sampling.presenceInSample[i] ) { samplingPattern |= (0x1U<<i); }
    if ( samplingPattern != pClus->samplingPattern() ) { 
      pClus->clearSamplingData();
      pClus->setSamplingPattern(samplingPattern,true);
    }
  }

  // set sampling variables
  for ( int i(0); i<(int)CaloSampling::Unknown; ++i ) { 
    if ( accum.sampling.presenceInSample[i] ) { 
      CaloSampling::CaloSample sam = (CaloSampling::CaloSample)i;
      pClus->setEnergy(sam,accum.sampling.energyInSample[i]);
      double enorm(accum.sampling.posNormInSample[i]);
      double eta(accum.sampling.etaInSample[i]);
      double phi(accum.sampling.phiInSample[i]);
      if ( enorm != 0. ) { 
	pClus->setEta(sam,eta/enorm);
	pClus->setPhi(sam,CaloPhiRange::fix(phi/enorm));
      } else { 
	pClus->setEta(sam,eta);
	pClus->setPhi(sam,CaloPhiRange::fix(phi));
      }
      pClus->setEmax(sam,accum.sampling.maxEnergyInSample[i]);
      pClus->setEtamax(sam,accum.sampling.etaMaxEnergyInSample[i]);
      pClus->setPhimax(sam,accum.sampling.phiMaxEnergyInSample[i]);
    } // check if sampling is in cluster
  } // loop on samplings

  return true;
}
