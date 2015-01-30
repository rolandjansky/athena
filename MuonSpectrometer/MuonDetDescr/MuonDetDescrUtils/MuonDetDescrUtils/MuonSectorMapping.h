/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONSECTORMAPPING_H
#define MUON_MUONSECTORMAPPING_H

#include <math.h>
#include <vector>
#include <iostream>

namespace Muon {

  class MuonSectorMapping {
  public:
    /** checks whether the phi position is consistent with sector */
    bool insideSector( int sector, double phi ) const;

    /** returns the sector corresponding to the phi position */
    int getSector( double phi ) const;

    /** returns the main sector plus neighboring if the phi position is in an overlap region */
    void getSectors( double phi, std::vector<int>& sectors ) const;

    /** returns the centeral phi position of a sector in radians */
    double sectorPhi( int sector ) const;

    /** transforms a phi position from and to the sector coordinate system in radians */
    double transformPhiToSector( double phi, int sector, bool toSector = true ) const;

    /** expresses a radial position from and to the sector coordinate frame, the phi position should always be global  in mm */
    double transformRToSector( double r, double phi, int sector, bool toSector = true ) const;

    /** returns the phi position of the overlap between the two sectors (which have to be neighboring) in radians */
    double sectorOverlapPhi( int sector1, int sector2 ) const;

    /** checks whether the phi position is close to a sector boundary */
    bool   closeToSectorBoundary( double phi ) const;

    /** transform a radial position from one sector frame into another */
    double transformRToNeighboringSector( double r, int sectorHit, int sectorTarget ) const;

    /** sector size (exclusive) in radians */
    double sectorSize( int sector ) const;

    /** sector width (with overlap) in radians */
    double sectorWidth( int sector ) const;

  private:


    static double m_oneEightsOfPi;  //  pi/8
    static double m_inverseOneEightsOfPi;  // 8/pi
    static double m_sectorSize[2]; // side of a sector in radiants 
    static double m_sectorOverlap; // size of the overlap between small and large sectors
    static bool m_debug; // if true addition cout is enabled
  };

  inline double MuonSectorMapping::sectorSize( int sector ) const {
    return m_sectorSize[sector%2];
  }

  inline double MuonSectorMapping::sectorWidth( int sector ) const {
    return sectorSize(sector) + m_sectorOverlap;
  }

  inline bool MuonSectorMapping::insideSector( int sector, double phi ) const {
    double phiInSec = transformPhiToSector(phi,sector);
    if( phiInSec < -sectorWidth(sector) ) return false;
    if( phiInSec >  sectorWidth(sector) ) return false;
    return true;
  }


  inline double MuonSectorMapping::sectorPhi( int sector ) const {
    if( sector < 10 ) return M_PI*(sector-1)/8.;
    return -M_PI*(2-(sector-1)/8.);
  }

  inline int MuonSectorMapping::getSector( double phi ) const {
    // remap phi onto sector structure
    double val = (phi+sectorSize(1))*m_inverseOneEightsOfPi; // convert to value between -8 and 8, shift by width first sector
    if( val < 0 ) val += 16;
    int    sliceIndex = static_cast<int>(val/2); // large/small wedge
    double valueInSlice = val-2*sliceIndex;
    int    sector = 2*sliceIndex+1;
    if( valueInSlice > 1.2 ) ++sector; 
    return sector;
  }

  inline void MuonSectorMapping::getSectors( double phi, std::vector<int>& sectors ) const {
    
    int sector = getSector(phi);
    int sectorNext = sector != 16 ? sector + 1 : 1;
    int sectorBefore = sector != 1 ? sector -1 : 16;
    if( insideSector(sectorBefore,phi) ) sectors.push_back(sectorBefore);
    if( insideSector(sector,phi) )       sectors.push_back(sector);
    if( insideSector(sectorNext,phi) )   sectors.push_back(sectorNext);
  }
  
  inline bool MuonSectorMapping::closeToSectorBoundary( double phi ) const {
    std::vector<int> sectors;
    getSectors(phi,sectors);
    return sectors.size() > 1;
  }

  inline double MuonSectorMapping::transformPhiToSector( double phi, int sector, bool toSector ) const {
    double sign = toSector ? -1 : 1;
    double dphi = phi+sign*sectorPhi(sector);
    if( dphi > M_PI )  dphi -= 2*M_PI;
    if( dphi < -M_PI ) dphi += 2*M_PI;
    return dphi;
  }

  inline double MuonSectorMapping::sectorOverlapPhi( int sector1, int sector2 ) const {
    if( sector1 == sector2 ) return sectorPhi(sector1);

    int s1 = sector1 < sector2 ? sector1 : sector2;
    int s2 = sector1 > sector2 ? sector1 : sector2;
    if( s2 == 16 && s1 == 1 ){
      s1 = 16;
      s2 = 1;
    }else if( std::abs(s1-s2) > 1 ){
      if( m_debug ) std::cout << " bad sector combination: not neighbouring " << s1 << "   " << s2 << std::endl;
      return 0;
    }

    double phi1 = sectorPhi(s1);
    double phio1 = phi1 + sectorSize(s1);
    if( phio1 > M_PI ) phio1 -= 2*M_PI;

    return phio1;
  }

  inline double MuonSectorMapping::transformRToSector( double r, double phi, int sector, bool toSector ) const {
    double dphi = transformPhiToSector(phi,sector); 
    if( fabs(dphi) > 0.3 && m_debug ){ 
      std::cout << " large dphi detected!!: phi " << phi 
                << " sector " << sector << " phi "  << sectorPhi(sector) << " " << phi << " dphi " << dphi << std::endl; 
    } 
    if( toSector ) return r*cos(dphi); 
    else           return r/cos(dphi); 
  } 


  inline double MuonSectorMapping::transformRToNeighboringSector( double r, int sectorHit, int sector ) const {
    double phi = sectorPhi(sectorHit);
    double phio = sectorOverlapPhi(sectorHit,sector);
    double dphio = phi-phio;
    if( dphio < -M_PI ) dphio += 2* M_PI;
    double redge = r/cos(dphio);
    double phi_sec = sectorPhi(sector);
    double dphi = phio-phi_sec;
    if( dphi < -M_PI ) dphi += 2* M_PI;
    if( fabs(dphi) > 0.3 && m_debug ){
      std::cout << " large dphi detected!!: sector " << sector << " of hit " << sectorHit << " phi ref sector "  << phi_sec
                << " hit " << phi << " dphi " << dphi << std::endl;;
    }
    return redge*cos(dphi);
  }

}

#endif
