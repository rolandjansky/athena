/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONPATTERN_H
#define MUON_MUONPATTERN_H

#include "EventPrimitives/EventPrimitives.h"   
#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {
  class PrepRawData;
}

namespace Muon {
  /** 
      @brief Basic class for patterns in the muon spectrometer consistig out of a list of Trk::PrepRawData objects.
             The class also provides the possibility to store a first estimate of the particle parameters 
	     in form of a global position and direction 
      
      The base-class gives access to the PrepRawData.
  */
  class MuonPattern {
  public:
    /** Constructor */
    //MuonPattern( const Trk::GlobalPosition& pos, const Trk::GlobalMomentum& dir );
   MuonPattern( const Amg::Vector3D& pos, const Amg::Vector3D& dir );

    /** Destructor */
    virtual ~MuonPattern();

    /** add hit to pattern */
    virtual void addPrd( const Trk::PrepRawData* prd ) = 0;

    /** Global position of the pattern */
    const Amg::Vector3D& globalPosition() const;

    /** Global direction of the pattern */
    const Amg::Vector3D& globalDirection() const;

    /** Number or PrepRawData contained by this Pattern */
    virtual unsigned int numberOfContainedPrds() const = 0;
     
    /** returns the PrepRawData objects depending on the integer, return zero if index out of range */
    virtual const Trk::PrepRawData* prd(unsigned int) const = 0;
    
    /** clone methode */
    virtual MuonPattern* clone() const = 0;

  private:    
    /** global position of the pattern */
    Amg::Vector3D m_globalPosition;

    /** global direction of the pattern */
    Amg::Vector3D m_globalDirection;

    
  };
  inline const Amg::Vector3D& MuonPattern::globalPosition() const { return m_globalPosition; }
  inline const Amg::Vector3D& MuonPattern::globalDirection() const { return m_globalDirection; }
}

#endif
