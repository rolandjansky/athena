/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 properties of a plane based strip detector with equidistant parallel strips
 -----------------------------------------
***************************************************************************/

#ifndef MUONGEOMODEL_MUONSTRIPDESIGN_H
#define MUONGEOMODEL_MUONSTRIPDESIGN_H

#include "GeoPrimitives/GeoPrimitives.h"

namespace MuonGM {

  struct MuonStripDesign {
  public:
    int    nstrips;
    double stripPitch;
    double invStripPitch;
    double stripLength;
    double stripWidth;
    double readoutLocY;
    double signY;
    Amg::Vector2D firstStripPos;

    /** distance to readout */
    double distanceToReadout( const Amg::Vector2D& pos ) const;
    double distanceToReadout( double locY ) const;

    /** calculate local strip number, range 1=nstrips like identifiers. Returns -1 if out of range */
    int stripNumber( const Amg::Vector2D& pos ) const;
    int stripNumber( double locX ) const;

    /** calculate local strip position for a given strip number */
    bool  stripPosition( int strip, Amg::Vector2D& pos  ) const;
  };

  inline double MuonStripDesign::distanceToReadout( const Amg::Vector2D& pos ) const { 
    return distanceToReadout(pos.y());    
  }

  inline double MuonStripDesign::distanceToReadout( double locY ) const { 
    double dist = signY*(locY-readoutLocY);
    if( dist < 0. ) return 0.;
    if( dist > stripLength ) return stripLength;
    return dist;
  }

  inline int MuonStripDesign::stripNumber( const Amg::Vector2D& pos ) const { 
    return stripNumber(pos.x());
  }

  inline int MuonStripDesign::stripNumber( double locX ) const { 
    int st = (locX-firstStripPos.x())*invStripPitch+1.5;
    if( st < 1 ) return -1;
    if( st > nstrips ) return -1;
    return st;
  }

  inline bool MuonStripDesign::stripPosition( int st, Amg::Vector2D& pos  ) const {
    if( st < 1 ) return false;
    if( st > nstrips ) return false;
    double x = firstStripPos.x() + stripPitch*(st-1);
    pos[0] = x;
    pos[1] = firstStripPos.y();
    return true;
  }

}
#endif // MUONGEOMODEL_MUONSQUARECLUSTERDETECTORDESCRIPTOR_H
