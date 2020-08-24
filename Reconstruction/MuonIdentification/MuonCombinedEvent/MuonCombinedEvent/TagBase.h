/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDEVENT_TAGBASE_H
#define MUONCOMBINEDEVENT_TAGBASE_H

#include "xAODMuon/Muon.h"
#include <string>
#include <vector>

namespace Muon {
  class MuonSegment;
}

namespace Trk {
  class Track;
}

namespace MuonCombined {
  
  inline int authorRank( const xAOD::Muon::Author& a ) {
    if( a == xAOD::Muon::MuidCo )   return 0;
    if( a == xAOD::Muon::MuGirl )   return 1;
    if( a == xAOD::Muon::MuTagIMO ) return 2;
    if( a == xAOD::Muon::MuTag )    return 3;
    if( a == xAOD::Muon::CaloTag )  return 4;
    if( a == xAOD::Muon::CaloLikelihood ) return 5;
    if( a == xAOD::Muon::CaloLikelihood ) return 6;
    if( a == xAOD::Muon::MuidSA )   return 7;
    if( a == xAOD::Muon::STACO )    return 8;
    return 8;
  }

  inline int typeRank( const xAOD::Muon::MuonType& a ) {
    if( a == xAOD::Muon::Combined )       return 0;
    if( a == xAOD::Muon::SiliconAssociatedForwardMuon ) return 1;
    if( a == xAOD::Muon::SegmentTagged )  return 2;
    if( a == xAOD::Muon::MuonStandAlone ) return 3;
    if( a == xAOD::Muon::CaloTagged )     return 4;
    return 5;
  }

  /** base-class for combined reconstruction output
      Provides access to MuonType and Author
   */
  class TagBase {
  public:
    /** typedefs */
    typedef xAOD::Muon::MuonType Type;
    typedef xAOD::Muon::Author   Author;

    /** access to MuonType */
    Type type() const { return m_type; }

    /** access to Muon Author */
    Author author() const { return m_author; }

    /** destructor */
    virtual ~TagBase(){}

    /** type string */
    virtual std::string name() const = 0;

    /** print content to string */
    virtual std::string toString() const =0;

    /** access to primary muon system track, zero if non available */
    virtual const Trk::Track* primaryTrack() const;
    
    /** access to associated segments, empty vector if non available */
    virtual std::vector<const Muon::MuonSegment*> associatedSegments() const;
    
  protected:
    /** protected base class to be called by concrete implementations */
    TagBase( Author author, Type type ) : m_author(author),m_type(type) {}
    
  private:
    /** author and type info */
    Author     m_author;  /// author 
    Type       m_type;    /// type 
    
  };
  
  inline const Trk::Track* TagBase::primaryTrack() const {
    return 0;
  }
    
  inline std::vector<const Muon::MuonSegment*> TagBase::associatedSegments() const {
    return std::vector<const Muon::MuonSegment*>();
  }

  inline bool operator<( const TagBase& t1,  const TagBase& t2 ){
    int r1 = typeRank(t1.type());
    int r2 = typeRank(t2.type());
    if( r1 != r2 && t1.author()!=xAOD::Muon::STACO && t2.author()!=xAOD::Muon::STACO) return r1 < r2; 
    return authorRank(t1.author()) < authorRank(t2.author());
  }
  
  struct SortTagBasePtr {
    bool operator()( const TagBase* t1,  const TagBase* t2 ) const {
      return *t1<*t2;
    }
  };

}


#endif

