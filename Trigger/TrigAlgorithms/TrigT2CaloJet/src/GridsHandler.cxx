/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT2CaloJet/GridsHandler.h"

#include "TrigT2CaloJet/HashedTrig3Momentum.h"

#include <iostream>


GridsHandler::GridsHandler()
{
  using namespace std;
  vector< CaloSampling::CaloSample > samples;
  vector< size_t > defaultSizes;
  // need to ajust the default sizes? (granularity ~0.1)
  samples.push_back(CaloSampling::PreSamplerB); defaultSizes.push_back(128);
  samples.push_back(CaloSampling::EMB1	     ); defaultSizes.push_back(128);
  samples.push_back(CaloSampling::EMB2	     ); defaultSizes.push_back(128);
  samples.push_back(CaloSampling::EMB3       ); defaultSizes.push_back(128);
  samples.push_back(CaloSampling::PreSamplerE); defaultSizes.push_back(128);
  samples.push_back(CaloSampling::EME1	     ); defaultSizes.push_back(128);
  samples.push_back(CaloSampling::EME2	     ); defaultSizes.push_back(128);
  samples.push_back(CaloSampling::EME3       ); defaultSizes.push_back(128);
  samples.push_back(CaloSampling::HEC0	     ); defaultSizes.push_back(128);
  samples.push_back(CaloSampling::HEC1	     ); defaultSizes.push_back(128);
  samples.push_back(CaloSampling::HEC2	     ); defaultSizes.push_back(128);
  samples.push_back(CaloSampling::HEC3       ); defaultSizes.push_back(128);
  samples.push_back(CaloSampling::TileBar0   ); defaultSizes.push_back(128);
  samples.push_back(CaloSampling::TileBar1   ); defaultSizes.push_back(128);
  samples.push_back(CaloSampling::TileBar2   ); defaultSizes.push_back(128);
  samples.push_back(CaloSampling::TileGap1   ); defaultSizes.push_back(128);
  samples.push_back(CaloSampling::TileGap2   ); defaultSizes.push_back(128);
  samples.push_back(CaloSampling::TileGap3   ); defaultSizes.push_back(128);
  samples.push_back(CaloSampling::TileExt0   ); defaultSizes.push_back(128);
  samples.push_back(CaloSampling::TileExt1   ); defaultSizes.push_back(128);
  samples.push_back(CaloSampling::TileExt2   ); defaultSizes.push_back(128);
  samples.push_back(CaloSampling::FCAL0	     ); defaultSizes.push_back(128);
  samples.push_back(CaloSampling::FCAL1      ); defaultSizes.push_back(128);
  samples.push_back(CaloSampling::FCAL2      ); defaultSizes.push_back(128);
  samples.push_back(CaloSampling::Unknown    ); defaultSizes.push_back(128);

  for(unsigned int iS=0; iS<samples.size(); iS++){
    CaloSampling::CaloSample &sample = samples[iS];
    grids_[sample] = vector< HashedTrig3Momentum >(0);
    grids_[sample].reserve(defaultSizes[iS]);
  } // end for(iS)

  copy(samples.begin(), samples.end(),
       back_inserter< vector< CaloSampling::CaloSample> >(allSamples_));
}

//------------------------------------------------
void clearGrid(CsVmcPair &map){ map.second.clear(); }
//------------------------------------------------
void GridsHandler::clearAllGrids()
{
  std::for_each(grids_.begin(), grids_.end(), clearGrid);
}

//------------------------------------------------
Vmc& GridsHandler::grid(const CaloSampling::CaloSample &sample)
{
  return grids_[sample];
}
//------------------------------------------------
void GridsHandler::appendAllGrids(Vt3m &output) const
{
  for(CsVmcMap::const_iterator it=grids_.begin(); it != grids_.end(); ++it)
    output.insert(output.end(), it->second.begin(), it->second.end());
}
