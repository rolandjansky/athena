/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloConditions/CaloNoise.h"
#include "TMath.h"

CaloNoise::CaloNoise(const size_t nLArCells, const size_t nLArGains, const size_t nTileCells, const size_t nTileGains,
		     const CaloCell_Base_ID* caloCellId) :
  m_caloCellId(caloCellId) {
  m_larNoise.resize(boost::extents[nLArGains][nLArCells]);
  m_tileNoise.resize(boost::extents[nTileGains][nTileCells]);
  IdentifierHash h1,h2;
  m_caloCellId->calo_cell_hash_range(CaloCell_ID::TILE, h1,h2);
  m_tileHashOffset=h1;
}  

void CaloNoise::setTileBlob(const CaloCondBlobFlt* flt, const float lumi) {
  m_tileBlob=flt;
  m_lumi=lumi;
}

CaloNoise::~CaloNoise() {
  if ( m_tileBlob) delete m_tileBlob;
}


//The following method is copied (amost unchagned) from CaloNoiseToolDB
#define sqrt2 1.4142135623730950
#define invsqrt2 0.707106781186547524

float CaloNoise::calcSig(const IdentifierHash subHash, const int gain, const float e) const {
  const unsigned int dbGain = CaloCondUtils::getDbCaloGain(gain);
  if (!m_tileBlob) {
    //No data (like pileup-noise only): return cached noise
    return m_tileNoise[dbGain][subHash];
  }

  const double sigma1 = m_tileBlob->getData(subHash,dbGain,2);
  const double sigma2 = m_tileBlob->getData(subHash,dbGain,3);
  const double ratio  = m_tileBlob->getData(subHash,dbGain,4);
 

  
  if((sigma1 == 0. && sigma2 == 0.) || e == 0.) return 0.;
  if(sigma1 == 0.) return e/sigma2;
  if((ratio  == 0.) || sigma2 == 0.) return e/sigma1;
  const double x1 = e/sigma1;
  const double x2 = e/sigma2;

  constexpr std::array<float,2> valid_range{0.9,7.5};
  const float wide_gauss_sigma = std::min(fabs(x1),fabs(x2));
  if(wide_gauss_sigma > valid_range[1]) return wide_gauss_sigma;

  const float narrow_gauss_sigma= std::max(fabs(x1),fabs(x2));
  if(narrow_gauss_sigma < valid_range[0]) return narrow_gauss_sigma;
  

  const double y1= TMath::Erf(invsqrt2*x1);
  const double y2= TMath::Erf(invsqrt2*x2);

  const double z = ( y1*sigma1 + ratio*y2*sigma2 )/( sigma1 + ratio*sigma2);

  //return the C.L. probability (with sign!)
  //  return z;
 
  // if instead you want to return the sigma-equivalent C.L.
  // (with sign!) use the following line
  const double sigma= sqrt2*TMath::ErfInverse(z);

  const float elecNoise= (sigma != 0.) ? fabs(e/sigma) : 0.0;

  if (m_lumi>0) {
    const float b= m_tileBlob->getData(subHash,dbGain,1);
    return std::sqrt(elecNoise*elecNoise+b*b*m_lumi);
  }
  else {
    return elecNoise;
  }
}
