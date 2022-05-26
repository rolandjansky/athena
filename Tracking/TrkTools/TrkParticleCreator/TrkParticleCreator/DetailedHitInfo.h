/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETAILEDHITINFO_H
#define DETAILEDHITINFO_H

#include <vector>
#include <tuple>

namespace Trk {

  enum DetectorRegion {
    pixelBarrelFlat     = 0,
    pixelBarrelInclined = 1,
    pixelEndcap         = 2,
    stripBarrel         = 3,
    stripEndcap         = 4
  };
  
  
  
  class DetailedHitInfo 
  {
  /** This class containes the detailed information on the contributing layers and regions to the hit count. */
    
    public:
      DetailedHitInfo() {};
      ~DetailedHitInfo() {};
      
      DetailedHitInfo( const DetailedHitInfo& ph );
      DetailedHitInfo& operator=(const DetailedHitInfo& ph);
      
      void addHit(Trk::DetectorRegion region, int layer, int etaModule, int hit=1);
      int getHits(Trk::DetectorRegion region, int layer);
      int getContributionFromRegion(Trk::DetectorRegion region);
      int getAllContributions();
      int getPixelContributions();
      int getStripContributions();
      int getHitsFromRegion(Trk::DetectorRegion region);
      int getAllHits();
      int getPixelHits();
      int getStripHits();
      
      // the tuple elements are: layer number, # contributing layers, # hits
      std::vector < std::pair < Trk::DetectorRegion, std::vector < std::tuple <int , int , int> > > > getHitInfo();
      
    private:
      // the tuple elements are: layer number, # contributing layers, # hits
      std::vector < std::pair < Trk::DetectorRegion, std::vector < std::tuple <int , int , int> > > > m_detailedHitInfo;
      int m_prevEta;
  };
  
}

#endif
