// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// InDetTrackAccessor.h
// Declarations of objects to access track properties

#ifndef INDETTRACKSELECTIONTOOL_INDETTRACKACCESSOR_H
#define INDETTRACKSELECTIONTOOL_INDETTRACKACCESSOR_H

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

  // ---------------- TrackAccessor ----------------

  class TrackAccessor : public asg::AsgMessaging {
  public:
    TrackAccessor(const asg::IAsgTool*);
    virtual ~TrackAccessor() = 0;
    // function to get properties from track
    virtual StatusCode access( const xAOD::TrackParticle& track,
			       const xAOD::Vertex* vertex = 0 ) = 0;
    //Trk::Track access will need to be passed e.g. the summary, perigee directly
#ifndef XAOD_ANALYSIS
    virtual StatusCode access( const Trk::Track& track,
			       const Trk::TrackParameters* perigee = 0,
			       const Trk::TrackSummary* summary = 0) = 0;
#endif    
    bool isValid() const {return m_valid;} // accessor to validity flag
  protected:
    bool m_valid; // whether to trust the value
  };

  // ---------------- SummaryAccessor ----------------

  class SummaryAccessor : public virtual TrackAccessor {
  public:
    SummaryAccessor(const asg::IAsgTool*);
    void setSummaryType(xAOD::SummaryType);
    virtual StatusCode access( const xAOD::TrackParticle& track,
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

  class ParamAccessor : public virtual TrackAccessor {
  public:
    ParamAccessor(const asg::IAsgTool*);
    void setIndex(Int_t index);
    StatusCode access ( const xAOD::TrackParticle& track,
			const xAOD::Vertex* vertex = 0 );
#ifndef XAOD_ANALYSIS
    StatusCode access ( const Trk::Track& track,
			const Trk::TrackParameters* perigee = 0,
			const Trk::TrackSummary* summary = 0 );
#endif
    Double_t getValue() const {return m_paramValue;}

  private:
    Int_t m_index;
    Double_t m_paramValue;
  }; // class ParamAccessor

  // ---------------- ParamCovAccessor ----------------

  class ParamCovAccessor : public virtual TrackAccessor {
  public:
    ParamCovAccessor(const asg::IAsgTool*);
    void setIndices( Int_t, Int_t );
    StatusCode access ( const xAOD::TrackParticle& track,
			const xAOD::Vertex* vertex = 0 );
#ifndef XAOD_ANALYSIS
    StatusCode access ( const Trk::Track& track,
			const Trk::TrackParameters* perigee = 0,
			const Trk::TrackSummary* summary = 0 );
#endif
    Double_t getValue() const {return m_paramCovValue;}

  private:
    Int_t m_index1;
    Int_t m_index2;
    Double_t m_paramCovValue;
  }; // class ParamAccessor

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
  private:
    Double_t m_chiSq;
    Double_t m_nDoF;
  };
  
  // ---------------- PtAccessor ----------------
  class PtAccessor : public virtual TrackAccessor {
  public:
    PtAccessor(const asg::IAsgTool*);
    StatusCode access ( const xAOD::TrackParticle& track,
			const xAOD::Vertex* = 0 );
#ifndef XAOD_ANALYSIS
    StatusCode access ( const Trk::Track& track,
			const Trk::TrackParameters* perigee = 0,
			const Trk::TrackSummary* summary = 0 );
#endif
    Double_t getValue() const {return m_pt;}
  private:
    Double_t m_pt;
  };


  // ---------------- EtaAccessor ----------------
  class EtaAccessor : public virtual TrackAccessor {
  public:
    EtaAccessor(const asg::IAsgTool*);
    StatusCode access ( const xAOD::TrackParticle& track,
			const xAOD::Vertex* = 0 );
#ifndef XAOD_ANALYSIS
    StatusCode access ( const Trk::Track& track,
			const Trk::TrackParameters* perigee = 0,
			const Trk::TrackSummary* summary = 0 );
#endif
    Double_t getValue() const {return m_eta;}
  private:
    Double_t m_eta;
  };


  // ---------------- PAccessor ----------------
  class PAccessor : public virtual TrackAccessor {
  public:
    PAccessor(const asg::IAsgTool*);
    StatusCode access ( const xAOD::TrackParticle& track,
			const xAOD::Vertex* = 0 );
#ifndef XAOD_ANALYSIS
    StatusCode access ( const Trk::Track& track,
			const Trk::TrackParameters* perigee = 0,
			const Trk::TrackSummary* summary = 0 );
#endif
    Double_t getValue() const {return m_p;}
  private:
    Double_t m_p;
  };

} // namespace InDet

#endif // INDETTRACKSELECTIONTOOL_INDETTRACKACCESSOR_H
