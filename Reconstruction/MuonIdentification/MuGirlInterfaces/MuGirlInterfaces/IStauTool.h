/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IStauTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IMUGIRLNSSTAUTOOL_H
#define IMUGIRLNSSTAUTOOL_H
#include "GaudiKernel/IAlgTool.h"
#include "MuGirlInterfaces/MdtSegmentMakerInfo.h"
#include "MuGirlInterfaces/RpcSegmentInfo.h"
#include "MuGirlInterfaces/CandidateSummary.h"

#include "xAODTracking/TrackParticle.h"


namespace NTuple
{
class Tuple;
}

namespace Muon
{
class MuonSegment;
}

namespace Trk
{
  class Charged;
  class PerigeeSurface;
  template <int DIM, class Y, class Z> class ParametersT;
  typedef ParametersT<5,Charged,PerigeeSurface> Perigee;
  class Track;
}

namespace MuGirlNS
{
class SegmentManager;

typedef std::vector<const Muon::MuonSegment*> MuonSegmentList;

  static const InterfaceID IID_IStauTool("MuGirlNS::IStauTool", 1, 0);

  class IStauTool : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    // enter declaration of your interface-defining member functions here
    virtual StatusCode processStau(const xAOD::TrackParticle* trkParticle, 
                                   const MuonSegmentList& muonSegments,
                                   const MdtSegmentMakerInfoList& mdtSegmentMakerInfoList, 
                                   const MuGirlNS::RIO_OnTrackLists& rpcHitsInSegments, 
                                   const MuGirlNS::RIO_OnTrackLists& tgcHitsInSegments,
                                   const Trk::Track* pMuonRefittedTrack = NULL,
                                   const xAOD::TrackParticle* muonParticle = NULL,
                                   Amg::Vector3D trigMomentum = Amg::Vector3D(),
                                   const Trk::Perigee* trk0=NULL,bool isMuonBetaRefit=false) = 0;
    virtual void clearCandidate() = 0;
    virtual StatusCode bookNTuple(NTuple::Tuple* pNTuple) = 0;
    virtual StatusCode fillNTuple() = 0;
    virtual double beta() = 0;
    virtual double mass() = 0;
    virtual double stauP() = 0;
    virtual const Trk::Track* refittedTrack() = 0;
    virtual StatusCode fillStauSummary(const CandidateSummary* summary, CandidateSummary* stauSummary) = 0;
    virtual StatusCode fillStauExtras(const CandidateSummary* stauSummary, StauExtras* stauExtras) = 0;
    virtual StatusCode fillRHExtras(RHExtras* rhExtras) = 0;
    virtual std::vector<const Muon::MuonSegment*>* newMdtSegments() = 0;
    virtual void initTool(bool doGlobalFit) = 0;    
    virtual void recordCollection() = 0;
    virtual void setCandidateAnn(double ann) = 0;      

   // virtual void releaseMdtSegments() = 0;
    virtual void setRefittedTrack(const Trk::Track* refittedTrk) = 0;
    virtual void setSegmentManager(SegmentManager* pSegmentManager) = 0;
  };

  inline const InterfaceID& MuGirlNS::IStauTool::interfaceID()
    {
      return IID_IStauTool;
    }

} // end of namespace

#endif

