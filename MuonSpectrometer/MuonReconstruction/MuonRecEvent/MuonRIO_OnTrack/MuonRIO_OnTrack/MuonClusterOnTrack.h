/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonClusterOnTrack.h
//   Header file for class MuonClusterOnTrackOnTrack
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// /** Class to implement Cluster On Track for Si */
///////////////////////////////////////////////////////////////////
// Version 1.0 06/07/2004 Ketevi A. Assamagan
// adapted from Veronique Boisvert
///////////////////////////////////////////////////////////////////

#ifndef MUONRIOONTRACK_MUONCLUSTERONTRACK_H
#define MUONRIOONTRACK_MUONCLUSTERONTRACK_H

// Base classes
#include "MuonPrepRawData/MuonCluster.h"
#include "MuonReadoutGeometry/MuonClusterReadoutElement.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkSurfaces/Surface.h"
// needed classes

class MuonClusterOnTrackCnv_p1;

namespace Muon {

  /** @brief Base class for Muon cluster RIO_OnTracks.*/
  class MuonClusterOnTrack :   public Trk::RIO_OnTrack {
    
    // /////////////////////////////////////////////////////////////////
    // Public methods:
    // /////////////////////////////////////////////////////////////////
  public:

    /** @brief Pool constructor*/
    MuonClusterOnTrack();
    

    /**@brief Constructor with parameters.
       @warning You should not try to construct this class - use one of the concrete implementations,
       i.e. CscClusterOnTrack.*/
    MuonClusterOnTrack( const Trk::LocalParameters& locpos, 
            const Amg::MatrixX& locerr, 
            const Identifier& id,
            const double positionAlongStrip
            ); 


    MuonClusterOnTrack(const MuonClusterOnTrack &);
    MuonClusterOnTrack &operator=(const MuonClusterOnTrack &);

    /** @brief Destructor*/
    virtual ~MuonClusterOnTrack();
    
    /** @brief Clone this ROT */
    virtual MuonClusterOnTrack* clone()const=0  ;

    /** @brief Returns the Trk::PrepRawData - is a MuonCluster in this scope*/
    virtual const MuonCluster* prepRawData() const = 0;

    /** @brief Returns global position */
    virtual const Amg::Vector3D& globalPosition() const;

    virtual double positionAlongStrip() const {
      return m_positionAlongStrip;
    }

    /** @brief Returns the hashID of the PRD collection */
    virtual IdentifierHash collectionHash() const;

    /** @brief Returns an invalid hash */
    virtual IdentifierHash idDE() const ; 
    
    /** @brief Dumps information about the PRD*/
    virtual MsgStream&    dump( MsgStream&    stream) const;

    /** @brief Dumps information about the PRD*/
    virtual std::ostream& dump( std::ostream& stream) const;
    
    /** @brief Returns the detector element, assoicated with the PRD of this class*/
    virtual const MuonGM::MuonClusterReadoutElement* detectorElement() const = 0;


  protected:
    /** cache global position, the global position has to be calculated in the inheriting classes */
    mutable const Amg::Vector3D*              m_globalPosition;
    
    /** The position along the strip - used to calculate the GlobalPosition*/
    double                                          m_positionAlongStrip;
    
  private:
    friend class  ::MuonClusterOnTrackCnv_p1;

    /** Used by custom convertors*/
    virtual void setValues(const Trk::TrkDetElementBase*, const Trk::PrepRawData*) = 0 ;
    
  };

  // /////////////////////////////////////////////////////////////////
  // Inline methods:
  // /////////////////////////////////////////////////////////////////
  
  inline IdentifierHash MuonClusterOnTrack::collectionHash() const { 
    if( this->prepRawData() ) return prepRawData()->collectionHash();
    return IdentifierHash();
  }
  
  inline IdentifierHash MuonClusterOnTrack::idDE() const { 
      return IdentifierHash(); 
  } 
  
} // end of namespace

#endif // MUONRIOONTRACK_MUONCLUSTERONTRACK_H

