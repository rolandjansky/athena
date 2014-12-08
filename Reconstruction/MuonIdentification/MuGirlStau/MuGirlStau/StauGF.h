/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// StauTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUGIRLNSSTAUGF_H
#define MUGIRLNSSTAUGF_H

#include "GaudiKernel/MsgStream.h"
#include "MuGirlStau/StauToolDefinitions.h"
#include "MuGirlStau/StauTool.h"

namespace MuGirlNS
{
    class StauGF
    {
    public:
        StauGF(StauTool* pStau, MsgStream& log, const MuGirlNS::MuonSegmentList& muonSegments);

        /** default destructor */
        virtual ~StauGF();

        void initStepData(GlobalFitData* gfData, double beta, double tTrack = 0);
        void clearStepData(GlobalFitData* gfData);
        void printStepData(GlobalFitData* gfData);
        void processGlobalFitWithBeta(double currentBeta, GlobalFitData* gfData);
        void processGlobalFitWithTTrack(double tTrack, GlobalFitData* gfData, double beta = 1.0);
        void processGlobalFitWithBeta(MuGirlNS::MuonSegmentList& muonSegments);
        void clear();
        const Trk::Track* refitTrack();
        double stauMomentum();

        void setInitialTrackParameters(const Trk::Perigee* trk0);

    private:
        StauTool* m_pStau;
        MsgStream& m_log;
        const MuGirlNS::MuonSegmentList m_muonSegments;
        const Trk::Perigee* m_initialTrackParameters;
        void prepareForGlobalFit(RIO_OnTrackLists* pTriggerHitsInSegments,
                std::vector<const Muon::MdtDriftCircleOnTrack*>* pMdtHits);
        void prepareForGlobalFit(MuGirlNS::MuonSegmentList& muonSegments);

    };

} // end of namespace

#endif
