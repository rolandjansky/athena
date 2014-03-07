/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class LArCaloId
   @brief Liquid Argon class for standalone storage of cell information
*/

#ifndef LArGeo_H
#define LArGeo_H

#include "TString.h"
#include "TArrayD.h"
#include "LArCafJobs/CaloId.h"
#include <vector>

class TH2;
class TH2D;
class TProfile2D;

namespace LArSamples {

  class Geo {
   
    public:
            
      static TString feedThroughName(PartitionId part, short feedThrough);
      static TH2D* partitionHist(PartitionId part, TString name, TString title);
      static TProfile2D* partitionProfileHist(PartitionId part, TString name, TString title);
      static bool setBinLabels(TH2* hist, PartitionId part);
      static short nFeedThroughs(PartitionId part);
      static short nSlots(PartitionId part);
      static short nPartitionChannels(PartitionId part);
      static short nFEBs(PartitionId part) { return nFeedThroughs(part)*nSlots(part); }
      static short nFEBs();
      static short nPhiRings();

      static short firstLayer(CaloId calo);
      static short nLayers(CaloId calo);
      static short nRegions(CaloId calo, short layer);
      static short firstEta(CaloId calo, short layer, short region = 0);
      static short nEta(CaloId calo, short layer, short region, short iPhi = 1); // max is 31, which is reached for iPhi=1 but not 0
      static short nEta(CaloId calo, short layer);
      static short nPhi(CaloId calo, short layer, short region = 0);      
      
      static double etaSize(CaloId calo, short layer, short region, short iEta = -1); // default (-1) is the "nominal" size
      static double phiSize(CaloId calo, short layer, short region, short iPhi = -1); // default (-1) is the "nominal" size
      static double etaMin(CaloId calo, short layer, short region = -1);
      static double etaMax(CaloId calo, short layer, short region = -1);
      static double phiMin(CaloId calo, short layer, short region = -1);
      static double eta(CaloId calo, short layer, short region, short iEta);
      static double phi(CaloId calo, short layer, short region, short iPhi);
      
      static double etaCenter(CaloId calo, short layer, short region, short iEta);
      static double phiCenter(CaloId calo, short layer, short region, short iPhi);
      static double etaHigh(CaloId calo, short layer, short region, short iEta);
      static double phiHigh(CaloId calo, short layer, short region, short iPhi);

      static short etaIndex(CaloId calo, short layer, short region, short iEta);
      static TArrayD etaBins(CaloId calo, short layer);
      static TH2D* etaPhiHist(CaloId calo, short layer, TString name, TString title);

      static int nChannels(CaloId calo, short layer, short region);
      static int nChannels(CaloId calo, short layer);
      static int nChannels(CaloId calo);

      static TArrayD merge(const std::vector<TArrayD*>& arrays);
      static TArrayD mirror(const TArrayD& bins);
      static TArrayD remove(const TArrayD& bins, int index);
  };
}

#endif
