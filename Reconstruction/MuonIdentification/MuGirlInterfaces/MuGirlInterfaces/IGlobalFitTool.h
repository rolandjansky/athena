/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IGlobalFitTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IMUGIRLNSGLOBALFITTOOL_H
#define IMUGIRLNSGLOBALFITTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "MuGirlInterfaces/RpcSegmentInfo.h"

namespace Muon
{
    class MuonSegment;
    class MdtDriftCircleOnTrack;
}
namespace Trk
{
    class Charged;
    class PerigeeSurface;
    template <int DIM, class Y, class Z> class ParametersT;
    typedef ParametersT<5,Charged,PerigeeSurface> Perigee;

    class Track;
    //class TrackSurfaceIntersection;
}
namespace Rec
{
    class TrackParticle;
}
namespace NTuple
{
class Tuple;
}

namespace MuGirlNS
{
    static const InterfaceID IID_IGlobalFitTool("MuGirlNS::IGlobalFitTool", 1, 0);

    class IGlobalFitTool : virtual public IAlgTool
    {

    public:
        static const InterfaceID& interfaceID()
        {
            return IID_IGlobalFitTool;
        }

        typedef std::vector<const Muon::MuonSegment*> MuonSegmentList;

        virtual const Trk::Track*  globalFit(const Trk::Track * IDTrack,
                                              const MuonSegmentList& pMuonSegments,
                                              BooleanProperty m_doNTuple)=0;
        virtual const Trk::Track*  globalFit(const MuonSegmentList & pMuonSegments, const Trk::Perigee*trk1, BooleanProperty doNTuple) = 0;
        virtual const Trk::Track*  standAloneRefit(const Trk::Track& cbtrk) = 0;
        
        virtual StatusCode bookNTuple(NTuple::Tuple* pNTuple, const std::string type) = 0;
        virtual StatusCode fillNTuple(const Trk::Track* RefittedTrack) = 0;
    };
} // end of namespace

#endif
