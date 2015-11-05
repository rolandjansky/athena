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
#include "AthenaKernel/MsgStreamMember.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

namespace MuonGM {

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
    double thickness;
    mutable double radialDistance;

    double sPadWidth;
    double lPadWidth;
    double xFrame;
    double ysFrame;
    double ylFrame;
    double yCutout;
    int    nPadH;
    int    nPadColumns;
    double PadPhiShift;

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

  inline std::pair<int,int> MuonPadDesign::channelNumber( const Amg::Vector2D& pos) const {

    // perform check of the sensitive area
    if ( pos.y() > (0.5*Length-ylFrame )|| pos.y() < (-0.5*Length+ysFrame ) ){
       ATH_MSG_DEBUG("pos.y() out of bounds");
       return std::pair<int,int>(-1,-1);
    }
    if ( pos.y() <= (0.5*Length-ylFrame-yCutout)){
      if ( fabs(pos.x()) > (0.5*sPadWidth+0.5*(lPadWidth-sPadWidth)*((pos.y()+(0.5*Length-ysFrame))/(Length-ysFrame-ylFrame))) ) {
         ATH_MSG_DEBUG("pos.x() out of bounds" );
         return std::pair<int,int>(-1,-1);
      }
    }
    else if ( pos.y() > (0.5*Length-ylFrame-yCutout)) {
      if ( fabs(pos.x()) > (0.5*sPadWidth+0.5*(lPadWidth-sPadWidth)*(Length-ysFrame-ylFrame-yCutout)/(Length-ysFrame-ylFrame))) {
         ATH_MSG_DEBUG("pos.x() out of bounds" );
         return std::pair<int,int>(-1,-1);
      }
    }

    // padEta
    double y1 = 0.5*Length + pos.y(); //distance from small edge to hit
    double padEtadouble;
    int padEta = 0;
    if (y1>ysFrame+firstRowPos) {
       padEtadouble =((y1-ysFrame-firstRowPos)/inputRowPitch)+2;//+1 for firstRow, +1 because a remainder means another row (3.1=4)
       padEta=padEtadouble;
    }
    else if (y1>ysFrame) {
       padEta=1;
    }
    else if (y1>0 && y1<ysFrame) {
       ATH_MSG_DEBUG("Hit the ysFrame" );
       return std::pair<int,int>(-1,-1);
    }
    else if (y1<0) {
       ATH_MSG_ERROR("negative distance to hit" );
       return std::pair<int,int>(-1,-1);
    }
    else {
       ATH_MSG_ERROR("undefined distance to hit" );
       return std::pair<int,int>(-1,-1);
    }

    if (padEta==nPadH+1){
       padEta-=1;//the top row can be bigger, therefore it is really in the nPadH row.
    } 
    else if (padEta>nPadH+1) {
       ATH_MSG_ERROR("padEta too high" );
       return std::pair<int,int>(-1,-1);
    }

   // padPhi
   double locPhi = 180*atan( pos.x()/( radialDistance + pos.y()))/M_PI;
   double maxlocPhi = 180*atan( 0.5*sPadWidth/( radialDistance + (-0.5*Length+ysFrame ) ))/M_PI;
//    double maxlocPhi2 = 180*atan( 0.5*lPadWidth/( radialDistance + (0.5*Length-ylFrame) ))/M_PI;
   if (abs(locPhi)>maxlocPhi) {
      ATH_MSG_ERROR("locPhi too large" );
      return std::pair<int,int>(-1,-1);
   }
   double fuzziedX = pos.x() - (PadPhiShift /cos(locPhi*M_PI/180)); //fuzziness for negative z must be fixed (need to take negative of PadPhiShift)
   double fuzziedlocPhi = 180*atan( fuzziedX/( radialDistance + pos.y()))/M_PI;
   if (abs(fuzziedlocPhi)>maxlocPhi) {
      ATH_MSG_DEBUG("close to outer border" );
      fuzziedlocPhi=locPhi;
   }
   double padPhidouble = (fuzziedlocPhi-firstPhiPos)/inputPhiPitch;
   int padPhi = padPhidouble+2; //(+1 because remainder means next column e.g. 1.1=2, +1 so rightmostcolumn=1)

   if (padPhi == 0) {
      padPhi=1;
      ATH_MSG_DEBUG("adjusted rightmost. padPhi="<<padPhi );
   }
   else if(padPhi == nPadColumns+1){
      padPhi=nPadColumns;
      ATH_MSG_DEBUG("adjusted leftmost. padPhi="<<padPhi );
   }
   else if ( padPhi < 0 || padPhi > nPadColumns+1 ){
      ATH_MSG_ERROR("padPhi out of bounds" );
      return std::pair<int,int>(-1,-1);
   }
   ATH_MSG_DEBUG("padEta,padPhi: " <<padEta<<" , "<<padPhi );
   return std::pair<int,int>(padEta,padPhi);

  }


  inline bool MuonPadDesign::channelPosition( std::pair<int,int> pad, Amg::Vector2D& /*pos*/ ) const {

    if ( pad.first<1 || pad.second<1 ) return false;


    /* DT 2/9/2015 The test on the indices for pads on subsequent layers is not implemented in MuonPadDesign,but in TrigT1NSW (IIRC).

    int padEta = pad.first-1+padEtaMin;
    int padPhi = pad.second-1+padPhiMin;

    if( padEta < padEtaMin || padEta > padEtaMax ) return false;
    if( padPhi < padPhiMin || padPhi > padPhiMax ) return false;

    pos[1] = firstRowPos + (padEta-0.5)*inputRowPitch - radialDistance;     

    double phi = firstPhiPos + padPhi*inputPhiPitch;

    pos[0] =  tan(phi)*( radialDistance + pos.y() );

    return true; */

    return false;

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
