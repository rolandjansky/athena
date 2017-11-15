/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 properties of a plane based detector allowing for a stereo angle
 ----------------------------------------------------------------------
 Copyright (C) 2013 by ATLAS Collaboration, Sarka Todorova
***************************************************************************/

#ifndef MUONTRACKINGGEOMETRY_MUONCHANNELDESIGN_H
#define MUONTRACKINGGEOMETRY_MUONCHANNELDESIGN_H

#include "GeoPrimitives/GeoPrimitives.h"

namespace MuonGM {

  struct MuonChannelDesign {
  public:
    enum Type {
      etaStrip=0, ///< 0 (eta strips, locX || eta, including stereo angle - MM, sTGC strips)
      phiStrip=1, ///< 1 (phi strips  locX || phi, no stereo angle        - sTGC wire/gangs)
      pad=2       ///< 2 (pads        locX || eta,                        - sTGC pads)
    };
  public:
    int    type;
    int    nch;
    double sAngle;        //  
    double inputPitch;
    double inputWidth;
    double inputLength;
    double deadI;
    double deadO;
    double deadS;
    double signY;
    //Amg::Vector2D firstChannelPos;
    double firstPos;
    double firstPitch; // Pitch of 1st strip or number of wires in 1st group
    double groupWidth; // Number of Wires per group
    double nGroups; // Number of Wire groups
    double wireCutout;
    double xSize;
    double xLength;
    double ysFrame;
    double ylFrame;
    double minYSize;
    double maxYSize;
    double thickness;
    double yCutout;

    /** channel transform */
    //HepGeom::Transform3D  channelTransform( int channel ) const;

    /** distance to readout */
    double distanceToReadout( const Amg::Vector2D& pos ) const;
    //double distanceToReadout( double locY ) const;

    /** distance to channel - residual */
    double distanceToChannel( const Amg::Vector2D& pos, int nChannel=0 ) const;
    //double distanceToReadout( double locY ) const;

    /** calculate local channel number, range 1=nstrips like identifiers. Returns -1 if out of range */
    int channelNumber( const Amg::Vector2D& pos ) const;
    //int stripNumber( double locX ) const;

    /** calculate local wire group number, range 1=64 like identifiers. Returns -1 if out of range */
    int wireGroupNumber( const Amg::Vector2D& pos ) const;


    /** calculate local channel position for a given channel number */
    bool  channelPosition( int channel, Amg::Vector2D& pos  ) const;

    /** calculate local channel width */
    double  channelWidth( Amg::Vector2D pos  ) const;

    /** calculate local stereo angle */
    double  stereoAngle( int channel  ) const;

    /** calculate channel length for a given channel number */
    double  channelLength( int channel ) const;

    /** thickness of gas gap */
    double gasGapThickness() const;
  };

  inline double MuonChannelDesign::distanceToReadout( const Amg::Vector2D& pos ) const {
    
    int chNum = channelNumber( pos );

    if (chNum <0 ) return -1.;
    Amg::Vector2D chPos;
    if (!channelPosition( chNum, chPos) ) return -1.;

    double sA = stereoAngle(chNum);
 
    Amg::Vector2D chLoc( (pos.x()-chPos.x())*cos(sA)-(pos.y()-chPos.y())*sin(sA),
			      + (pos.x()-chPos.x())*sin(sA)+(pos.y()-chPos.y())*cos(sA) );

    double stripL = channelLength(chNum); 
   
    double dist = signY>0 ? chLoc.y()+0.5*stripL : 0.5*stripL-chLoc.y();
    if( dist < 0. ) return 0.;
    if( dist > stripL ) return stripL;
    return dist;

  }

  inline double MuonChannelDesign::distanceToChannel( const Amg::Vector2D& pos, int chNum ) const {
    
    // if channel number not provided, get the nearest channel ( mostly for validation purposes )
    bool validMode = false;

    if (chNum < 1 || chNum> nch ) {
      chNum =  channelNumber( pos);
      validMode = true;
    }

    Amg::Vector2D chPos;
    if (!channelPosition( chNum, chPos) ) return -10000.;

    double sA = stereoAngle(chNum);
 
    Amg::Vector2D chLoc( (pos.x()-chPos.x())*cos(sA)-(pos.y()-chPos.y())*sin(sA),
			      + (pos.x()-chPos.x())*sin(sA)+(pos.y()-chPos.y())*cos(sA) );

    if ( validMode && fabs(chLoc.x()) > 0.5*channelWidth( pos) ) {
      std::cout << "problem in identification of the channel: distance to nearest channel, channel width:"
		<<chLoc.x()<<","<< channelWidth(pos) << std::endl;  
    }
   
    return chLoc.x();
  }

  inline int MuonChannelDesign::channelNumber( const Amg::Vector2D& pos ) const {
 
    if (type==MuonChannelDesign::etaStrip) {      // "eta"  orientation , assumes constant stereo angle

      double xMfirst = firstPos;
      double xMid;
      int chNum;
      // sTGC strips no longer use the "eta" orientation in their local geometry.
      // Its coordinates are "rotated". sTGC strip pitch == 3.2mm is a way to check if sTGC only
      if (inputPitch == 3.2){ // if sTGC strip
        xMid = pos.y() - pos.x()*tan(sAngle);
       if (xMid < xMfirst && xMid > xMfirst - firstPitch) chNum = 1; // If position between bottom boundary and 1st strip
       else if (xMid > xMfirst) // position higher than first Pos
          chNum = int( cos(sAngle)*(xMid - xMfirst)/inputPitch+2 );
        else chNum = -1;
      }
      else {
        xMid = pos.x() - pos.y()*tan(sAngle);
        chNum = int( cos(sAngle)*(xMid - xMfirst)/inputPitch+1.5 );
      }
      if (chNum<1) return -1;
      if (chNum>nch) return -1;     // used also for calculation of the number of strips
      return chNum;

    } else if (type==MuonChannelDesign::phiStrip) {   // "phi" orientation, local coordinates rotated

      // find transverse pannel size for a given locX 
      if (inputPitch == 1.8 && groupWidth == 20) // if sTGC wires
        return wireGroupNumber(pos);
      int chNum = int( (pos.x()-firstPos)/inputPitch +1.5 ) ;
      if (chNum<1) return -1;
      if (chNum>nch) return -1;  
      return chNum;   

    } else if(type==MuonChannelDesign::pad) {
      // DG-2015-11-27 to be implemented
      return -1;
    }
    return -1;

  }

  inline int MuonChannelDesign::wireGroupNumber( const Amg::Vector2D& pos) const {
    // As of 2017-10-12, this is incomplete.
    // The wires in the 1st gas volume of QL1, QS1 can not be read for digits
    // We now hand-modified the xml file to include a new value.
    if (type==MuonChannelDesign::phiStrip && inputPitch == 1.8 && groupWidth == 20) {   // sTGC Wires
      //First, find the wire number associated to the position
      int wireNumber;
      if (pos.x() > -0.5*maxYSize && pos.x() < firstPos) // Before first wire
        wireNumber = 1;
      else wireNumber = (pos.x() - firstPos)/inputPitch + 2;
      //find wire group associated to wire number
      int grNumber;
      if (wireNumber <= firstPitch) grNumber = 1; // firstPitch in this case is number of wires in 1st group
      else grNumber = (wireNumber - firstPitch)/groupWidth +2; // 20 wires per group
      // If hit is in inactive wire region of QL1/QS1, return 63.
      if (wireCutout != 0 && pos.y() < 0.5*xSize - wireCutout) return 63;
      if (grNumber<1) return -1;
      if (grNumber>nGroups) return -1;
      return grNumber;
    } else {
      //Only implemented for sTGC wires
      return -1;
    }
    return -1;

  }

  inline bool MuonChannelDesign::channelPosition( int st, Amg::Vector2D& pos  ) const {
    if( st < 1 ) return false;
    if( st > nch ) return false;

    double dY = 0.5*(maxYSize-minYSize-2.*deadS);

    if ( type==MuonChannelDesign::phiStrip ) {   // swap coordinates on return

      if (inputPitch == 1.8 && groupWidth == 20) { // sTGC Wires: returns center of wire group
        if (st > nGroups) return false;
        double firstX = firstPos + firstPitch*inputPitch; // Position of the end of the first wire group (accounts for staggering)
        double locX;
        if (st == 1) locX = 0.5 * (-0.5*maxYSize + firstX);
        else if (st == nGroups) // account for staggering
          locX = 0.5 * (0.5*maxYSize - (nch - firstPitch - (nGroups-2)*groupWidth)*inputPitch);
        else locX = firstX + groupWidth*inputPitch*(st-1.5);
        pos[0] = locX;
        pos[1] = 0.5*xSize; // This is so that if you get the wiregrp number from a position, followed by getting the position from that channel to again obtain the wiregroup, you get the same results.

        return true;
      }

      double locY = firstPos+ (st-1)*inputPitch;

      double locX = 0.;

      if (fabs(locY)>0.5*(minYSize-deadS)) {
         
        //double gangLength = (0.5*maxYSize - fabs(locY))/dY * xSize;

        locX = 0.5*(xSize-deadO-deadI)*( 1. - (0.5*(maxYSize-deadS) -fabs(locY))/dY )+0.5*(deadI-deadO) ;

      }

      pos[0] = locY;
      pos[1] = locX;
 
      return true; 

    } else if ( type==MuonChannelDesign::etaStrip ) {

      if (sAngle==0.) {

	double x = firstPos + inputPitch*(st-1);
        if (inputPitch == 3.2) { // check if sTGC, preferably 2 unique values
          if (st == nch) { // Strip Staggering: either first strip or last strip is only half-width
            if (firstPitch == 3.2) // if 1st strip is not staggered
              x = x - inputPitch*0.5; // stagger first strip
          }
          // Here we "rotate" the coordinates. We changed the local geometry from a RotatedTrapezoid to a Trapezoid
          pos[0] = 0;
          pos[1] = x;
        }
        else { // default
	  pos[0] = x;
	  pos[1] = 0;
        }
	return true;

      }

      // strip central position
      double xMid = firstPos + (st-1)*inputPitch/cos(sAngle);

      std::vector<std::pair<double,double> > stripEnd;
      // intersection with the xUp boundary
      double yU = (0.5*xSize-deadO-xMid)/tan(sAngle);
      if (fabs(yU)<=0.5*maxYSize-deadS) stripEnd.push_back(std::pair<double,double>(0.5*xSize-deadO,yU));  
      // intersection with the xLow boundary
      double yD =-(0.5*xSize-deadI+xMid)/tan(sAngle);  
      if (fabs(yD)<=0.5*minYSize-deadS) stripEnd.push_back(std::pair<double,double>(-0.5*xSize+deadI,yD));  
      // intersection with phi boundaries
      double xP = (xMid+0.5*tan(sAngle)*(dY+minYSize-deadS))/(1-tan(sAngle)*dY/(xSize-deadO-deadI));
      double xM = (xMid-0.5*tan(sAngle)*(dY+minYSize-deadS))/(1+tan(sAngle)*dY/(xSize-deadO-deadI));
      double yP = 0.5*minYSize-deadS+dY*(xP/(xSize-deadO-deadI)+0.5);
      double yM =-0.5*minYSize+deadS-dY*(xM/(xSize-deadO-deadI)+0.5);
      if (xP<=0.5*xSize-deadO && xP>=-0.5*xSize+deadI) stripEnd.push_back(std::pair<double,double>(xP,yP));
      if (xM<=0.5*xSize-deadO && xM>=-0.5*xSize+deadI) stripEnd.push_back(std::pair<double,double>(xM,yM));
      //std::cout <<"intersections:"<<stripEnd.size()<< std::endl;  

      if (stripEnd.size()==2) {

	double mX = 0.5*(stripEnd[0].first  +stripEnd[1].first);
	double mY = 0.5*(stripEnd[0].second +stripEnd[1].second);
      
	pos[0] = mX;
	pos[1] = mY;
	
	return true;
      } else return false;

    } else if(type==MuonChannelDesign::pad) {
      // DG-2015-11-27 todo
    }

    return false;       

  }

  inline double MuonChannelDesign::stereoAngle( int /*st*/) const {
    
    // to be coded for TGC wire gangs and sTGC pads
 
    //if (sin(sAngle)>0.5) {
    //  double yUp = -0.5*maxYSize + (st-0.5) * maxYSize/nch;
    //  double yDn = -0.5*minYSize + (st-0.5) * minYSize/nch;
    //  return atan((yUp-yDn)/xSize);
    //}
 
    return sAngle; 
    
  }

  inline double MuonChannelDesign::channelLength( int st) const {

    double dY = 0.5*(maxYSize-minYSize-2*deadS);
    
    if ( type==MuonChannelDesign::phiStrip ) {

      double locY = firstPos+ (st-1)*inputPitch;

      double gangLength = xSize-deadO-deadI ;

      if (fabs(locY)>0.5*minYSize-deadS) {
         
        gangLength = (0.5*maxYSize-deadS - fabs(locY))/dY * (xSize-deadI-deadO);

      }
 
      return gangLength; 
      
    } else if ( type==MuonChannelDesign::etaStrip ) {
 
      if (sAngle==0.)  return inputLength +2*(st-0.5)*dY/nch;

      // strip central position
      double xMid = firstPos + (st-1)*inputPitch/cos(sAngle);

      std::vector<std::pair<double,double> > stripEnd;
      // intersection with the xUp boundary
      double yU = (0.5*xSize-deadO-xMid)/tan(sAngle);
      if (fabs(yU)<=0.5*maxYSize-deadS) stripEnd.push_back(std::pair<double,double>(0.5*xSize-deadO,yU));  
      // intersection with the xLow boundary
      double yD =-(0.5*xSize-deadI+xMid)/tan(sAngle);  
      if (fabs(yD)<=0.5*minYSize-deadS) stripEnd.push_back(std::pair<double,double>(-0.5*xSize+deadI,yD));  
      // intersection with phi boundaries
      double xP = (xMid+0.5*tan(sAngle)*(dY+minYSize-deadS))/(1-tan(sAngle)*dY/(xSize-deadI-deadO));
      double xM = (xMid-0.5*tan(sAngle)*(dY+minYSize-deadS))/(1+tan(sAngle)*dY/(xSize-deadI-deadO));
      double yP = 0.5*minYSize-deadS+dY*(xP/(xSize-deadI-deadO)+0.5);
      double yM =-0.5*minYSize+deadS-dY*(xM/(xSize-deadI-deadO)+0.5);
      if (xP<=0.5*xSize-deadO && xP>=-0.5*xSize+deadI) stripEnd.push_back(std::pair<double,double>(xP,yP));
      if (xM<=0.5*xSize-deadO && xM>=-0.5*xSize+deadI) stripEnd.push_back(std::pair<double,double>(xM,yM));
      //std::cout <<"intersections:"<<stripEnd.size()<< std::endl;  

      if ( stripEnd.size()==2 ) {
    
	double difX = stripEnd[0].first-stripEnd[1].first;
	double difY = stripEnd[0].second-stripEnd[1].second;
	return sqrt(difX*difX+difY*difY);
 
      } else return inputLength; 

    } else if(type==MuonChannelDesign::pad) {
      // DG-2015-11-23 todo
    }

    return inputLength;     

  }

  inline double MuonChannelDesign::channelWidth( Amg::Vector2D /*pos*/ ) const {
    // TODO : calculate for TGCs

    return inputWidth;

  }

  inline double MuonChannelDesign::gasGapThickness() const {

    return thickness;

  }

}
#endif // MUONTRACKINGGEOMETRY_MUONCHANNELDESIGN_H
