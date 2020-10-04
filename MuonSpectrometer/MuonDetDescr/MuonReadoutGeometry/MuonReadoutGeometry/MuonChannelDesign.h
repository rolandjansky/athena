/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 properties of a plane based detector allowing for a stereo angle
 ----------------------------------------------------------------------
***************************************************************************/

#ifndef MUONTRACKINGGEOMETRY_MUONCHANNELDESIGN_H
#define MUONTRACKINGGEOMETRY_MUONCHANNELDESIGN_H

#include "GeoPrimitives/GeoPrimitives.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

namespace MuonGM {

  struct MuonChannelDesign {
  public:
    enum Type {
      etaStrip=0, ///< 0 (eta strips, locX || eta, including stereo angle - MM, sTGC strips)
      phiStrip=1, ///< 1 (phi strips  locX || phi, no stereo angle        - sTGC wire/gangs)
      pad=2       ///< 2 (pads        locX || eta,                        - sTGC pads)
    };
    enum DetType {
      MM=0,
      STGC=1
    };
    int    type;
    int    detType;
    int    nch;   // total #of active strips
    double sAngle; // stereo angle
    double inputPitch; //we use this param to define the pitch for MM
    double inputWidth;
    double inputLength;
    double deadI; //this param is not used for MM
    double deadO; //this param is not used for MM
    double deadS; //this param is not used for MM
    double signY;
    double firstPos; //the position of the first active strip
    double firstPitch; // Pitch of 1st strip or number of wires in 1st group
    double groupWidth; // Number of Wires per group
    double nGroups; // Number of Wire groups
    double wireCutout;
    double xSize;  //Module's radial distance
    double xLength;
    double ysFrame;
    double ylFrame;
    double minYSize; //bottom length (active area)
    double maxYSize; //top length (active area)
    double thickness;  //gas thickness
    double yCutout;
    double minYPhiL; //(left bottom) distance between first eta and stereo strips (MM)
    double minYPhiR; //(right bottom) distance between first eta and stereo strips (MM)
    double maxYPhi; //(top) distance between last eta and stereo strips (MM)
    int nMissedTopEta; //#of strips that are not connected to any FE boards (MM)
    int nMissedBottomEta;
    int nMissedTopStereo;
    int nMissedBottomStereo;
    int nRoutedTop;  //#of strips that have shorter lengths (at the corners of the PCBs) (MM)
    int nRoutedBottom;
    double dlStereoTop; // length between the first eta and stereo
    double dlStereoBottom;
    int totalStrips; //total strips per MM module

    /** distance to readout */
    double distanceToReadout( const Amg::Vector2D& pos ) const;

    /** distance to channel - residual */
    double distanceToChannel( const Amg::Vector2D& pos, int nChannel ) const;

    /** calculate local channel number, range 1=nstrips like identifiers. Returns -1 if out of range */
    int channelNumber( const Amg::Vector2D& pos ) const;

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

    if (chNum <1 ) return -1.;
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
    
    // if channel number is out of bounds, get the nearest channel ( mostly for validation purposes )
    bool validMode = false;
    if (type==MuonChannelDesign::etaStrip && detType==MuonChannelDesign::DetType::MM) {
        if( chNum < 1 || chNum > totalStrips ){
            chNum = channelNumber(pos);
            validMode = true;
        }
    } else {
        if (chNum < 1 || chNum> nch ) {
            chNum =  channelNumber( pos);
            validMode = true;
        }
    }

    Amg::Vector2D chPos;
    if (!channelPosition( chNum, chPos) ) return -10000.;
    //In the new geometry from November 2019 the layers are not anymore tilted, therefore we need
    //to introduce the correction for the stereo angle in the distance
    //Patrick Scholer March 18 2020
    Amg::Vector2D chLoc( (pos.x()-pos.y()*tan(sAngle))-chPos.x() , pos.y()-chPos.y());
    if ( validMode && std::abs(chLoc.x()) > 0.5*channelWidth( pos) ) {
      MsgStream log(Athena::getMessageSvc(),"MuonChannelDesign");
      if (log.level()<=MSG::INFO) log << MSG::INFO << "Problem in identification of the channel: distance to nearest channel, channel width: "
                                      << chLoc.x()<< ", " << channelWidth(pos) << " for channel number " << chNum << endmsg;
    }
   
    return chLoc.x();
  }

  inline int MuonChannelDesign::channelNumber( const Amg::Vector2D& pos ) const {
 
    if (type==MuonChannelDesign::etaStrip) { // "eta" orientation, assumes constant stereo angle, can be MM or STGC

      double xMfirst = firstPos;
      double xMid;
      int chNum;
      if (detType==MuonChannelDesign::DetType::STGC){ // if sTGC strip
          xMid = pos.x() - pos.y()*tan(sAngle);
          if (xMid < xMfirst && xMid > xMfirst - firstPitch) chNum = 1; // If position between bottom boundary and 1st strip
          else if (xMid > xMfirst) // position higher than first Pos
              chNum = std::floor( cos(sAngle)*(xMid - xMfirst)/inputPitch ) + 2;
          else chNum = -1;
          
          if (chNum<1) return -1;
          if (chNum>nch) return -1;     // used also for calculation of the number of strips
          
      } else if (detType==MuonChannelDesign::DetType::MM) {
          xMid = pos.x() - pos.y()*tan(sAngle);
          int strips = sAngle==0 ? nMissedBottomEta : nMissedBottomStereo;
          // +1 accounts for the first strip by xMfirst, and the other +1 since we start counting at 1.
          chNum = ((int) std::floor( (xMid - xMfirst)/inputPitch)) + strips + 2;
          
          // if position is before beginning of first active strip return 1 (no readout) 
          // This serves to protect against stereo layers where there are no strips in the gas volume
          if (xMid < xMfirst-inputPitch) return 1;

          // Protecting stereo layers
          // Stereo layers have gas volume without strips. This makes sure that this area doesn't create bad identifiers
          if (sAngle && xMid > (xMfirst + inputPitch*(totalStrips - nMissedTopStereo - strips)) ) return totalStrips;

          if (chNum<1) return -1;
          if (chNum>totalStrips) return -1;
          
      }
      else return -1;
        
 
      return chNum;
      
     
    } else if (type==MuonChannelDesign::phiStrip) {   // "phi" orientation, local coordinates rotated

      // find transverse pannel size for a given locX 
      if (detType==MuonChannelDesign::DetType::STGC) // if sTGC wires
        return wireGroupNumber(pos);
      int chNum = std::round( (pos.x()-firstPos)/inputPitch +1 ) ;
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
    // The wires in the 1st gas volume of QL1, QS1 can not be read for digits
    if (type==MuonChannelDesign::phiStrip && detType==MuonChannelDesign::DetType::STGC) {   // sTGC Wires
      //First, find the wire number associated to the position
      int wireNumber;
      if (pos.x() > -0.5*maxYSize && pos.x() < firstPos) // Before first wire
        wireNumber = 1;
      else wireNumber = (pos.x() - firstPos)/inputPitch + 1;
      //find wire group associated to wire number
      int grNumber;
      if (wireNumber <= firstPitch) grNumber = 1; // firstPitch in this case is number of wires in 1st group
      else {
        grNumber = (wireNumber - 1 - firstPitch)/groupWidth +2; // 20 wires per group,
        /* If a hit is positionned after the last wire but inside the gas volume
           This is really a check for the few mm on the fringe of the gas volume
           Especially important for QL3. We still consider the digit active */
        if (grNumber > nGroups && pos.x() < 0.5*maxYSize) grNumber = nGroups;
      }
      /* If hit is in inactive wire region of QL1/QS1, return 63
         This digit is not added as 63 is an invalid wire number
         But it allows for better tracking of hits */
      if (wireCutout !=0. && pos.y() < 0.5*xSize - wireCutout) return 63;
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
     

    double dY = 0.5*(maxYSize-minYSize-2.*deadS);

    if ( type==MuonChannelDesign::phiStrip ) {   // swap coordinates on return
    
        if( st < 1 ) return false;
        if( st > nch ) return false;
        
      if (detType==MuonChannelDesign::DetType::STGC) { // sTGC Wires: returns center of wire group
        if (st > nGroups || st ==63) return false; // 63 is because we defined 63 as a unactive digit
        double firstX = firstPos + (firstPitch-1)*inputPitch; // Position of the end of the first wire group (accounts for staggering)
        double locX;
        if (st == 1) locX = 0.5 * (-0.5*maxYSize + firstX);
        else if (st < nGroups) locX = firstX + groupWidth*inputPitch*(st-1.5);
        else if (st == nGroups) // accounts for staggering by averaging last wire group and last wire
          locX = 0.5 * (0.5*maxYSize + firstX + (nGroups-2)*groupWidth*inputPitch);
        else return false;
        pos[0] = locX;
        pos[1] = 0;

        return true;
      }

      double locY = firstPos+ (st-1)*inputPitch;

      double locX = 0.;

      if (std::abs(locY)>0.5*(minYSize-deadS)) {
         
        locX = 0.5*(xSize-deadO-deadI)*( 1. - (0.5*(maxYSize-deadS) -std::abs(locY))/dY )+0.5*(deadI-deadO) ;

      }

      pos[0] = locY;
      pos[1] = locX;
 
      return true; 

    } else if ( type==MuonChannelDesign::etaStrip ) {

    
        
      if (sAngle==0.) {

      // always use same code to calculate strip position for layers with and without stereo angle
	    double x = firstPos + inputPitch*(st-1);
        if (detType==MuonChannelDesign::DetType::STGC) {
          if( st < 1 ) return false;
          if( st > nch ) return false;
          // We return the position in the center of the strip pitch
          // in a 3.2mm pitch strip, we return at the centre of the copper ie after 1.6mm
          // for the half strips (either first or last strips), the centre of the copper is not the centre of the half strip, ie NOT 1.6mm/2 
          // a half strip is 1.35mm of copper + 0.25mm of "gap" = 1.6mm
          if (st == 1){ // if First strip
            if (firstPitch == 1.6) x = firstPos - 0.25 - 0.5 * 1.35; // remove 0.25mm gap and half of the copper pitch for half strips (1.35mm/2) 
            else x = firstPos - 0.5 * firstPitch; // return center of the first strip
          }
          else if (st <= nch) x = firstPos + (st-1.5) * inputPitch;
          else return false;
          // If last strip is staggered (first strip is NOT staggered)
          // remove half the copper to return centre of copper of strip
          if (st == nch && firstPitch == 3.2) x = x - 1.35 * 0.5; // accounts for staggering
          pos[0] = x;
          pos[1] = 0;
        }
       
        else if (detType==MuonChannelDesign::DetType::MM) { //its a MM eta layer | first strip is #1, no longer following electronics convention of #0 
            
            if( st < nMissedBottomEta+1 ) return false;
            if( st > totalStrips-nMissedTopEta ) return false;
            
            else{
                // returns centre of strip
                pos[0] = firstPos + inputPitch*(st-nMissedBottomEta-1.5);
                pos[1] = 0;
            }
          
          
        }
        else { // default
            pos[0] = x;
            pos[1] = 0;
        }
	return true;

      }
      else if (sAngle!=0. && detType==MuonChannelDesign::DetType::MM) { //its a MM stereo layer
        
          if( st < nMissedBottomStereo+1 ) return false;
          if( st > totalStrips-nMissedTopStereo ) return false;
          
          else{
              // returns centre of strip
              pos[0] = firstPos + inputPitch*(st-nMissedBottomStereo-1.5);
              pos[1] = 0;
          }
	    return true;
     
      }

      else return false;



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

      if (std::abs(locY)>0.5*minYSize-deadS) {
         
        gangLength = (0.5*maxYSize-deadS - std::abs(locY))/dY * (xSize-deadI-deadO);

      }
 
      return gangLength; 
      
    } else if ( type==MuonChannelDesign::etaStrip ) {
 
        double stLen = 0;
        if (sAngle==0.){
            
            if(detType==MuonChannelDesign::DetType::STGC) return inputLength +2*(st-0.5)*dY/nch;
            
            else if (detType==MuonChannelDesign::DetType::MM)
            {
                if( (st > nMissedBottomEta) || (st < (totalStrips-nMissedTopEta))){
                    
                    stLen = inputLength + 2*(0.5*(maxYSize-minYSize)*(st-nMissedBottomEta)*inputPitch/xSize);
                    return stLen;
                   
                }
                
                else return -1;
            }
            
            else return -1;
        }
        
         else if (sAngle!=0. && detType==MuonChannelDesign::DetType::MM) {
      
             if(st<(totalStrips-(nMissedTopStereo+nRoutedTop))){
               

                 
             if( st > nMissedBottomStereo && st < (nMissedBottomStereo+nRoutedBottom) )
             stLen = (minYPhiR + (st-nMissedBottomStereo)*inputPitch)/sin(sAngle);
                 
                 if(st > (nMissedBottomStereo+nRoutedBottom) )
                 stLen = (inputLength + 2*(0.5*(maxYSize-minYSize)*(st-nMissedBottomEta)*inputPitch/xSize) )/cos(sAngle);
                 
                 return stLen;
             }
             //length for routed strips is not defined
             else if(st>(totalStrips-(nMissedTopStereo+nRoutedTop)) && st < (totalStrips-nMissedTopStereo))
             {
                 stLen = ( maxYPhi + (totalStrips-nMissedTopStereo-(st+1))*inputPitch)/sin(sAngle);
                 return stLen;
             }
             else return -1;
        }

 
    } else if(type==MuonChannelDesign::pad) {
      // DG-2015-11-23 todo
    }

    return inputLength;     

  }

  inline double MuonChannelDesign::channelWidth( Amg::Vector2D /*pos*/ ) const {
    // TODO : calculate for TGCs

   // sTGC block:     
    if (type==MuonChannelDesign::etaStrip && detType==MuonChannelDesign::DetType::STGC) return inputPitch; // if sTGC strips return 3.2mm which is stored as pitch, not width (2.7mm)     
    if (type==MuonChannelDesign::phiStrip && detType==MuonChannelDesign::DetType::STGC) // if sTGC wires
      return groupWidth * inputPitch; // if sTGC wires return width of full wire group

    if(type==MuonChannelDesign::etaStrip && detType==MuonChannelDesign::DetType::MM) return inputPitch;
    else return inputWidth;
      
  }

  inline double MuonChannelDesign::gasGapThickness() const {

    return thickness;

  }

}
#endif // MUONTRACKINGGEOMETRY_MUONCHANNELDESIGN_H
