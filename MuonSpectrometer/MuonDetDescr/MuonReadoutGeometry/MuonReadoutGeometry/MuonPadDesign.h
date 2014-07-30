/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 properties of a plane based detector allowing for a stereo angle
 ----------------------------------------------------------------------
 Copyright (C) 2013 by ATLAS Collaboration, Sarka Todorova
***************************************************************************/

#ifndef MUONTRACKINGGEOMETRY_MUONPADDESIGN_H
#define MUONTRACKINGGEOMETRY_MUONPADDESIGN_H


#include "GeoPrimitives/GeoPrimitives.h"

namespace MuonGM {

  struct MuonPadDesign {
  public:
    int    padPhiMin;
    int    padPhiMax;
    int    padEtaMin;
    int    padEtaMax;
    double sAngle;        //  
    double inputRowPitch;
    double inputRowWidth;
    double inputPhiPitch;
    double inputPhiWidth;
    double deadI;
    double deadO;
    double deadS;
    double signY;
    double firstRowPos;
    double firstPhiPos;
    double xSize;
    double minYSize;
    double maxYSize;
    double thickness;
    mutable double radialDistance;

    /** channel transform */
    //HepGeom::Transform3D  channelTransform( int channel ) const;

    /** distance to readout */
    double distanceToReadout( /*const Amg::Vector2D& pos*/ ) const;

    /** distance to channel - residual */
    double distanceToChannel( const Amg::Vector2D& pos, bool measPhi, int channel=0 ) const;

    /** calculate local channel number, range 1=nstrips like identifiers. Returns -1 if out of range */
    std::pair<int,int> channelNumber( const Amg::Vector2D& pos) const;

    /** calculate local channel position for a given channel number */
    bool  channelPosition( std::pair<int,int> pad, Amg::Vector2D& pos ) const;

    /** calculate local channel width */
    double  channelWidth( const Amg::Vector2D& pos, bool measPhi  ) const;

    /** calculate local stereo angle */
    double  stereoAngle( /*int channel*/  ) const;

    /** calculate channel length for a given channel number */
    double  channelLength( /*int channel*/ ) const;

    /** thickness of gas gap */
    double gasGapThickness() const;

    /** access to cache */
    void setR( double R) const {this->radialDistance = R; }

  };

  inline double MuonPadDesign::distanceToReadout( /*const Amg::Vector2D& pos*/ ) const {
    
    return 0.;

  }

  inline double MuonPadDesign::distanceToChannel( const Amg::Vector2D& pos, bool measPhi, int channel ) const {

    // if channel number not provided, get the nearest channel ( mostly for validation purposes )
    
    std::pair<int,int> pad(0,0);
    
    if ( channel<1 ) {     // retrieve nearest pad indices       
      pad = channelNumber( pos);
      
    } else {   // hardcode - or add a member saving idHelper max
      pad.first = ( (channel-1) % 13 ) +1;
      pad.second = ( (channel-1)/13 ) +1;
    }

    Amg::Vector2D chPos;
    if (!channelPosition( pad, chPos) ) return -10000.;

    if (!measPhi) return  (pos.y()-chPos.y());

    // the "phi" distance to be compared with channel width (taking into account the stereo angle)      

    return   (pos.x()-chPos.x());
  }

  inline std::pair<int,int> MuonPadDesign::channelNumber( const Amg::Vector2D& pos) const {

    // perform check of the sensitive area
    if ( fabs(pos.y()) > 0.5*xSize ) return std::pair<int,int>(-1,-1);
    if ( fabs(pos.x()) > (0.5*minYSize+0.5*(maxYSize-minYSize)*(pos.y()/xSize+0.5)) ) return std::pair<int,int>(-1,-1);

    // padEta
 
    int padEta = ( radialDistance + pos.y() - firstRowPos )/inputRowPitch;

    padEta += 1;           // for match with NSWgeometry
   
    if ( padEta < padEtaMin || padEta > padEtaMax ) return std::pair<int,int>(-1,-1);

    padEta -= padEtaMin;
    padEta += 1;           // index starts at 1

    // padPhi
  
    double locPhi = atan( pos.x()/( radialDistance + pos.y()));

    double fr = (locPhi-firstPhiPos)/inputPhiPitch;

    int padPhi = int( fabs(fr) + 0.5);

    if ( fr<0. ) padPhi*=-1;
   
    if ( padPhi < padPhiMin || padPhi > padPhiMax ) return std::pair<int,int>(-1,-1);

    padPhi -= padPhiMin;
    padPhi += 1;           // index starts at 1  

    return std::pair<int,int>(padEta,padPhi);

  }


  inline bool MuonPadDesign::channelPosition( std::pair<int,int> pad, Amg::Vector2D& pos ) const {

    if ( pad.first<1 || pad.second<1 ) return false;

    int padEta = pad.first-1+padEtaMin;
    int padPhi = pad.second-1+padPhiMin;

    if( padEta < padEtaMin || padEta > padEtaMax ) return false;
    if( padPhi < padPhiMin || padPhi > padPhiMax ) return false;

    pos[1] = firstRowPos + (padEta-0.5)*inputRowPitch - radialDistance;     

    double phi = firstPhiPos + padPhi*inputPhiPitch;

    pos[0] =  tan(phi)*( radialDistance + pos.y() );

    return true; 

  }

  inline double MuonPadDesign::stereoAngle( /*int st*/) const {
    
    // to be coded for TGC wire gangs and sTGC pads
 
    //if (sin(sAngle)>0.5) {
    //  double yUp = -0.5*maxYSize + (st-0.5) * maxYSize/nch;
    //  double yDn = -0.5*minYSize + (st-0.5) * minYSize/nch;
    //  return atan((yUp-yDn)/xSize);
    //}
 
    return sAngle; 
    
  }

  inline double MuonPadDesign::channelLength( /*int st*/) const {

    return 0.;     

  }

  inline double MuonPadDesign::channelWidth( const Amg::Vector2D& pos, bool measPhi ) const {

    // calculate the phi width 

    return ( measPhi ? 2.*tan(0.5*inputPhiPitch)*(radialDistance+pos.y()) : inputRowPitch );

  }

  inline double MuonPadDesign::gasGapThickness() const {

    return thickness;

  }

}
#endif // MUONTRACKINGGEOMETRY_MUONPADDESIGN_H
