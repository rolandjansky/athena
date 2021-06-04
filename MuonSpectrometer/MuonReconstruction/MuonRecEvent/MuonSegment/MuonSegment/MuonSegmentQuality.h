/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonSegmentQuality.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUONSEGMENT_MUONSEGMENTQUALITY_H
#define MUONSEGMENT_MUONSEGMENTQUALITY_H

#include "Identifier/Identifier.h"
#include "TrkEventPrimitives/FitQuality.h"
#include <memory>
#include <vector>

namespace Muon {

  /** @class MuonSegmentQuality

      This is the common muon segment quality object.
      The object stores a list of channels, crossed by the segment, without hit
      For specific cases, like for example for the MDTs, a more complicated quality object is needed.
      It inherites from MuonSegmentQuality.

      The number of holes on the segment is calculated by the MuonSegmentQuality as being the 
      total number of channels without a hit. The function can be overloaded by specific qualty objects to
      provide a more detailed quality measure.

      @author Niels.van.Eldik@cern.ch

  */  

  class MuonSegmentQuality : public Trk::FitQuality {
  public:
    /** Default Constructor needed for POOL */   
    MuonSegmentQuality();

    /** Constructor */   
    MuonSegmentQuality( double chi2, int ndof, const std::vector<Identifier>& channelsWithoutHit, bool isStrict=false );
    MuonSegmentQuality( double chi2, int ndof, std::vector<Identifier>&& channelsWithoutHit, bool isStrict=false );

    /** Destructor */
    virtual ~MuonSegmentQuality() {}
  
    /** number of holes */    
    unsigned int numberOfHoles() const;

    /** vector of identifiers of channels crossed by the segment but without hit */
    const std::vector<Identifier>& channelsWithoutHit() const;
    
    /** bare pointer clone */
    virtual MuonSegmentQuality* clone() const override final;
    
    /** NVI clone to unique_ptr */
    std::unique_ptr<MuonSegmentQuality> uniqueClone() const {
      return std::unique_ptr<MuonSegmentQuality>(clone());
    }
    
    
    bool isStrict() const; //!< Returns true if the segment was created using strict criteria
  private:
    
    std::vector<Identifier> m_channelsWithoutHit{};
    bool m_isStrict{};
  };



  inline unsigned int MuonSegmentQuality::numberOfHoles() const {
      return m_channelsWithoutHit.size();
  }

  inline const std::vector<Identifier>& MuonSegmentQuality::channelsWithoutHit() const {
    return m_channelsWithoutHit;
  }
  
  inline MuonSegmentQuality* MuonSegmentQuality::clone() const {
    return new MuonSegmentQuality(*this);
  }

  inline bool MuonSegmentQuality::isStrict() const {
    return m_isStrict;
  }

}
#endif // MUONSEGMENT_MUONSEGMENTQUALITY_H

