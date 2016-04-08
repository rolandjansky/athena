/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ICandidateTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IMUGIRLNSCANDIDATETOOL_H
#define IMUGIRLNSCANDIDATETOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "MuGirlInterfaces/MdtSegmentMakerInfo.h"
#include "MuGirlInterfaces/RpcSegmentInfo.h"

#include "MuonLayerHough/MuonLayerHough.h"
#include "xAODTracking/TrackParticle.h"


class TrigMuonEFTrack;
class MuonFeature;

class Identifier;

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
  typedef ParametersBase<5, Charged> TrackParameters;
  template <int DIM, class Y, class Z> class ParametersT;
  typedef ParametersT<5,Charged,PerigeeSurface> Perigee;
}
namespace MuGirlNS
{

typedef std::vector<const Muon::MuonSegment*> MuonSegmentList;
class CandidateSummary;
class Candidate;
class SegmentManager;
static const InterfaceID IID_ICandidateTool("MuGirlNS::ICandidateTool", 1, 0);

class ICandidateTool : virtual public IAlgTool
{
public:
    typedef std::vector<const Muon::MuonSegment*> MuonSegmentList;

    static const InterfaceID& interfaceID()
    {
        return IID_ICandidateTool;
    }

    virtual StatusCode fill(int iTrack, const xAOD::TrackParticle* pTrackParticle, const Trk::TrackParameters* extrMuonEntry, bool bSaveMdtSegmentMakerInfo) = 0;
    virtual void fillRefittedTrack(const xAOD::TrackParticle* pRefittedTrack, const Trk::Track* trkRefittedTrack, CandidateSummary* pSummary) = 0;
    virtual void fillMSTrack(std::unique_ptr<const xAOD::TrackParticle> pMSRefittedTrack, 
                             std::unique_ptr<const Trk::Track> trkRefittedTrack,
                             CandidateSummary* pSummary) = 0;

    virtual void setLHR(double lhr) = 0;
    virtual void setAnn(double barrelNN, double endcapNN) = 0;
    virtual void setIsolation(int isolation) = 0;
    virtual StatusCode bookNTuple(NTuple::Tuple* pNTuple) = 0;
    virtual StatusCode fillNTupleEventInfo(unsigned nRunNumber, unsigned nEventNumber) = 0;
    virtual StatusCode fillNTuple() = 0;
    virtual StatusCode fillNTuple(std::vector<MuonHough::MuonLayerHough::Maximum*> hough_data[16][4][2]) = 0;
    virtual StatusCode fillSummary(CandidateSummary* pSummary) = 0;
    virtual const MdtSegmentMakerInfoList& getMdtSegmentMakerInfo() const = 0;
    virtual const MuonSegmentList& vectorMuonSegments() const = 0;
    virtual const Trk::TrackParameters* getPerigee() const =0;
    virtual double innerAngle() const = 0;
    virtual double middleAngle() const = 0;
    virtual void setPassANN(bool pass) = 0;
    virtual void setPassdoStau(bool pass) = 0;
    virtual const RIO_OnTrackLists& getRpcHitsInSegments() = 0;
    virtual const RIO_OnTrackLists& getTgcHitsInSegments() = 0;
    virtual void candidateClear()=0;
    virtual void setSegmentManager(SegmentManager* pSegmentManager) = 0;
    virtual bool hasInnerStationSegments() = 0;
    virtual bool isHoughMaxAssociated(MuonHough::MuonLayerHough::Maximum* max,int sector, int distance, int region) = 0;
};

} // end of namespace

#endif
