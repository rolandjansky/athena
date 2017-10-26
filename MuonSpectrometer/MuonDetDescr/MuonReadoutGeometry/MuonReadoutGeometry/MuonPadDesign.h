/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 properties of a plane based detector allowing for a stereo angle
 ----------------------------------------------------------------------
***************************************************************************/

#ifndef MUONTRACKINGGEOMETRY_MUONPADDESIGN_H
#define MUONTRACKINGGEOMETRY_MUONPADDESIGN_H

#include "GeoPrimitives/GeoPrimitives.h"
#include "AthenaKernel/MsgStreamMember.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

namespace MuonGM {

  /// Parameters defining the design of the readout sTGC pads
  /**
     The parameters below are the ones from the parameter book.  The
     naming convention used here refers to the one explained on p.90
     ("naming of .xml tags and .h variables") of
     https://twiki.cern.ch/twiki/bin/view/Atlas/NSWParameterBook#Parameter_book

     Note that the pads do not exist as objects in memory. Instead, a
     MuonPadDesign is defined for each layer in a module. From the
     readout parameters one can map position <-> channel:
     - for each sim hit position identify the pad (channel)
     - for each channel determine the center position
  */
  struct MuonPadDesign {
  public:
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
    //double xSize;
    //double minYSize;
    //double maxYSize;
    
    double Length;
    double sWidth;
    double lWidth;
    double Size;
    double thickness;
    mutable double radialDistance; ///< DT-2015-11-29 distance from the beamline to the center of the module

    double sPadWidth;
    double lPadWidth;
    double xFrame;
    double ysFrame;
    double ylFrame;
    double yCutout;
    int    nPadH;
    int    nPadColumns;
    double PadPhiShift;
    int etasign;
    int isLargeSector;
    double sectorOpeningAngle;

    const double largeSectorOpeningAngle = 28.0;
    const double smallSectorOpeningAngle = 17.0;

    /** channel transform */
    //HepGeom::Transform3D  channelTransform( int channel ) const;

    /** distance to readout */
    double distanceToReadout( /*const Amg::Vector2D& pos*/ ) const;

    /** distance to channel - residual */
    double distanceToChannel( const Amg::Vector2D& pos, bool measPhi, int channel=0 ) const;

    /// whether pos is within the sensitive area of the module
    bool withinSensitiveArea(const Amg::Vector2D& pos) const;
    /// lowest y (local) of the sensitive volume
    double minSensitiveY() const;
    /// highest y (local) of the sensitive volume
    double maxSensitiveY() const;
    /// largest (abs, local) x of the sensitive volume
    double maxAbsSensitiveX(const double &y) const;

    /** calculate local channel number, range 1=nstrips like identifiers. Returns -1 if out of range */
    std::pair<int,int> channelNumber( const Amg::Vector2D& pos) const;

    /** calculate local channel position for a given channel number */
    bool  channelPosition( std::pair<int,int> pad, Amg::Vector2D& pos ) const;

    /** calculate local channel corners for a given channel number */
    bool  channelCorners( std::pair<int,int> pad, std::vector<Amg::Vector2D> &corners ) const;

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

    MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
    bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }

  protected:
    mutable Athena::MsgStreamMember    m_msg = Athena::MsgStreamMember("MuonPadDesign");

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
