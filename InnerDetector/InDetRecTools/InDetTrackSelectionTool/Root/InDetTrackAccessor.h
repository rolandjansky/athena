// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// InDetTrackAccessor.h
// Declarations of objects to access track properties

#ifndef INDETTRACKSELECTIONTOOL_INDETTRACKACCESSOR_H
#define INDETTRACKSELECTIONTOOL_INDETTRACKACCESSOR_H

#include "AsgMessaging/StatusCode.h"
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

  // ---------------- TrackAccessor ----------------

  class TrackAccessor : public asg::AsgMessaging {
  public:
    TrackAccessor(const asg::IAsgTool*);
    virtual ~TrackAccessor() = 0;
    // function to get properties from track
    virtual StatusCode access( const xAOD::TrackParticle& track,
			       const xAOD::Vertex* vertex = 0 ) = 0;
#ifndef XAOD_ANALYSIS
    //Trk::Track access will need to be passed e.g. the summary, perigee directly
    virtual StatusCode access( const Trk::Track& track,
			       const Trk::TrackParameters* perigee = 0,
			       const Trk::TrackSummary* summary = 0) = 0;
#endif    
  };

  // ---------------- SummaryAccessor ----------------
  // summary values can be a uint8_t or a float: at some point may templatize,
  //    but would require templatizing cuts first
  class SummaryAccessor : public virtual TrackAccessor {
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
  class ParamAccessor : public virtual TrackAccessor {
  public:
    ParamAccessor(const asg::IAsgTool*);
    StatusCode access ( const xAOD::TrackParticle& track,
			const xAOD::Vertex* vertex = 0 );
#ifndef XAOD_ANALYSIS
    StatusCode access ( const Trk::Track& track,
			const Trk::TrackParameters* perigee = 0,
			const Trk::TrackSummary* summary = 0 );
#endif
    Double_t getValue() const {return m_paramValue;}

  private:
    Double_t m_paramValue;
  }; // class ParamAccessor

  // ---------------- ParamCovAccessor ----------------
  template <size_t index_i, size_t index_j>
  class ParamCovAccessor : public virtual TrackAccessor {
  public:
    ParamCovAccessor<index_i, index_j>(const asg::IAsgTool*);
    StatusCode access ( const xAOD::TrackParticle& track,
			const xAOD::Vertex* vertex = 0 );
#ifndef XAOD_ANALYSIS
    StatusCode access ( const Trk::Track& track,
			const Trk::TrackParameters* perigee = 0,
			const Trk::TrackSummary* summary = 0 );
#endif
    Double_t getValue() const {return m_paramCovValue;}

  private:
    Double_t m_paramCovValue;
  }; // class ParamCovAccessor


  // ---------------- FitQualityAccessor ----------------
  class FitQualityAccessor : public virtual TrackAccessor {
  public:
    FitQualityAccessor(const asg::IAsgTool*);
    StatusCode access ( const xAOD::TrackParticle& track,
			const xAOD::Vertex* = 0 );
#ifndef XAOD_ANALYSIS
    StatusCode access ( const Trk::Track& track,
			const Trk::TrackParameters* perigee = 0,
			const Trk::TrackSummary* summary = 0 );
#endif
    Double_t getChiSq() const {return m_chiSq;}
    Double_t getNumberDoF() const {return m_nDoF;}
    Double_t getProb() const {return TMath::Prob( m_chiSq, m_nDoF );}
  private:
    Double_t m_chiSq;
    Double_t m_nDoF;
  };

  
  // ---------------- FuncAccessor ----------------
  // template class for accessing member functions of TrackParticles
  template <typename T, T (xAOD::TrackParticle::*Func)() const>
  class FuncAccessor : public virtual TrackAccessor {
  public:
    FuncAccessor(const asg::IAsgTool*);
    StatusCode access ( const xAOD::TrackParticle& track,
			const xAOD::Vertex* = 0 );
#ifndef XAOD_ANALYSIS
    // these will need specialization in InDetTrackAccessor.cxx
    StatusCode access ( const Trk::Track& track,
			const Trk::TrackParameters* perigee = 0,
			const Trk::TrackSummary* summary = 0 );
#endif
    T getValue() const {return m_value;}
  private:
    T m_value;
  };


  // ---------------- eProbabilityHTAccessor ----------------
  // unfortunately, for now we need a special case as it is not
  //   implemented in the Trk::Track summary values yet (02-2015)
  // effort has not been put into generalization as this is
  // hopefully temporary.
  class eProbabilityHTAccessor : public virtual TrackAccessor {
  public:
    eProbabilityHTAccessor(const asg::IAsgTool*);
    StatusCode access ( const xAOD::TrackParticle& track,
			const xAOD::Vertex* = 0 );
#ifndef XAOD_ANALYSIS
    StatusCode access ( const Trk::Track& track,
                        const Trk::TrackParameters* perigee = 0,
                        const Trk::TrackSummary* summary = 0 );
#endif
    float getValue() const {return m_eProbHT;}
  private:
    float m_eProbHT;
  };

#ifndef XAOD_ANALYSIS
  // ---------------- SiHitsTopBottomAccessor ----------------
  class SiHitsTopBottomAccessor : public virtual TrackAccessor {
  public:
    SiHitsTopBottomAccessor(const asg::IAsgTool*);
    StatusCode access ( const xAOD::TrackParticle& track,
			const xAOD::Vertex* = 0 );
    StatusCode access ( const Trk::Track& track,
                        const Trk::TrackParameters* perigee = 0,
                        const Trk::TrackSummary* summary = 0 );
    uint8_t getHitsTop() const {return m_top;}
    uint8_t getHitsBottom() const {return m_bottom;}
  private:
    uint8_t m_top;
    uint8_t m_bottom;
  };
#endif

} // namespace InDet


// --------------------------------------------------------
// ---------------- Template Implementations --------------
// --------------------------------------------------------


// ---------------- ParamAccessor ----------------

template <size_t index>
InDet::ParamAccessor<index>::ParamAccessor(const asg::IAsgTool* tool)
  : InDet::TrackAccessor(tool)
  , m_paramValue(0)
{
  static_assert(index < 5, "Index for parameter accessor must be less than 5");
}

template <size_t index>
StatusCode InDet::ParamAccessor<index>::access( const xAOD::TrackParticle& track,
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

#ifndef XAOD_ANALYSIS
template <size_t index>
StatusCode InDet::ParamAccessor<index>::access( const Trk::Track&,
						const Trk::TrackParameters* perigee,
						const Trk::TrackSummary* )
{
  if (perigee==nullptr) {
    ATH_MSG_ERROR( "Zero pointer to perigee." );
    m_paramValue = std::nan("");
    return StatusCode::FAILURE;
  }
  m_paramValue = perigee->parameters()[index];
  return StatusCode::SUCCESS;
}
#endif

// ---------------- ParamCovAccessor ----------------
template <size_t index_i, size_t index_j>
InDet::ParamCovAccessor<index_i, index_j>::ParamCovAccessor(const asg::IAsgTool* tool)
  : InDet::TrackAccessor(tool)
  , m_paramCovValue(0)
{
  static_assert(index_i < 5, "Index for parameter covariance accessor must be less than 5");
  static_assert(index_j < 5, "Index for parameter covariance accessor must be less than 5");
}

template <size_t index_i, size_t index_j>
StatusCode InDet::ParamCovAccessor<index_i, index_j>::access( const xAOD::TrackParticle& track,
							      const xAOD::Vertex* )
{
  m_paramCovValue = track.definingParametersCovMatrix()(index_i, index_j);
  return StatusCode::SUCCESS;
}

#ifndef XAOD_ANALYSIS
template <size_t index_i, size_t index_j>
StatusCode InDet::ParamCovAccessor<index_i, index_j>::
access( const Trk::Track&,
	const Trk::TrackParameters* perigee,
	const Trk::TrackSummary* )
{
  if (!perigee) {
    ATH_MSG_ERROR( "Recieved zero pointer to perigee." );
    m_paramCovValue = std::nan("");
    return StatusCode::FAILURE;
  }
  m_paramCovValue = (*perigee->covariance())(index_i, index_j);
  return StatusCode::SUCCESS;
}
#endif


// ---------------- FuncAccessor ----------------
template <typename T, T (xAOD::TrackParticle::*Func)() const>
InDet::FuncAccessor<T,Func>::FuncAccessor(const asg::IAsgTool* tool)
  : TrackAccessor(tool)
  , m_value(false) // set to zero bits
{
}

template <typename T, T (xAOD::TrackParticle::*Func)() const>
StatusCode InDet::FuncAccessor<T,Func>::access( const xAOD::TrackParticle& track,
						const xAOD::Vertex* )
{
  m_value = (track.*Func)();
  return StatusCode::SUCCESS;
}


#endif // INDETTRACKSELECTIONTOOL_INDETTRACKACCESSOR_H
