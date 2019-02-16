/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
//Author: Lianyou Shan <lianyou.shan@cern.ch>
// -*- c++ -*-
// InDetSecVtxTrackAccessor.h
// Declarations of objects to access track properties

#ifndef INDETSECVTXTRACKSELECTIONTOOL_INDETTRACKACCESSOR_H
#define INDETSECVTXTRACKSELECTIONTOOL_INDETTRACKACCESSOR_H

#include "AsgTools/StatusCode.h"
#include "AsgTools/AsgMessaging.h"

#include "xAODTracking/TrackingPrimitives.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"

#ifndef XAOD_ANALYSIS
#include "TrkTrackSummary/TrackSummary.h"
namespace Trk {
  class Track;
  class Vertex;
}
#endif


namespace InDet {

  // ---------------- SecVtxTrackAccessor ----------------

  class SecVtxTrackAccessor : public asg::AsgMessaging {
  public:
    SecVtxTrackAccessor(const asg::IAsgTool*);
    virtual ~SecVtxTrackAccessor() = 0;
    virtual StatusCode access( const xAOD::TrackParticle& track,
                               const xAOD::Vertex* vertex = 0 ) = 0;

    // function to get properties from track
  };

// ---------------- SummaryAccessor ----------------
// summary values can be a uint8_t or a float: at some point may templatize,
//    but would require templatizing cuts first
  class SummaryAccessor : public virtual SecVtxTrackAccessor {
  public:
    SummaryAccessor(const asg::IAsgTool*);
    void setSummaryType(xAOD::SummaryType);
    StatusCode access( const xAOD::TrackParticle& track,
                       const xAOD::Vertex* vertex = 0 );
#ifndef XAOD_ANALYSIS
    StatusCode access( const Trk::Track& track,
                       const Trk::TrackParameters* perigee = 0,
                       const Trk::TrackSummary* summary = 0 );
#endif
    uint8_t getValue() const {return m_summaryValue;}
  private:
    uint8_t m_summaryValue;
    xAOD::SummaryType m_summaryType;
  }; // class SummaryAccessor

  // ---------------- ParamAccessor ----------------
  // using a separate accessor that acquires parameter based on index
  //   is useful because the index is the same in xAOD::TrackParticle and Trk::Track
  template <size_t index>
  class svParamAccessor : public virtual SecVtxTrackAccessor {
  public:
    svParamAccessor(const asg::IAsgTool*);
    StatusCode access ( const xAOD::TrackParticle& track,
                        const xAOD::Vertex* vertex = 0 );
    Double_t getValue() const {return m_paramValue;}

  private:
    Double_t m_paramValue;
  }; // class svParamAccessor

} // namespace InDet


template <size_t index>
InDet::svParamAccessor<index>::svParamAccessor(const asg::IAsgTool* tool)
  : InDet::SecVtxTrackAccessor(tool)
  , m_paramValue(0)
{
  static_assert(index < 5, "Index for parameter accessor must be less than 5");
}

template <size_t index>
StatusCode InDet::svParamAccessor<index>::access( const xAOD::TrackParticle& track,
                                                    const xAOD::Vertex* vertex )
{

  m_paramValue = track.definingParameters()[index];
  if (index == 1) { // if this is a z-accessor (should be evaluated at compile-time)
    if (vertex != nullptr) {
      // if vertex is provided we need to cut w.r.t. the vertex
      m_paramValue += track.vz() - vertex->z();
    }
  }
  return StatusCode::SUCCESS;
}

#endif // INDETTRACKSELECTIONTOOL_INDETTRACKACCESSOR_H
