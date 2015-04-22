/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRLNSSTAUMDTT_H
#define MUGIRLNSSTAUMDTT_H

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkEventPrimitives/DriftCircleStatus.h"
#include "MuGirlStau/StauToolDefinitions.h"
#include "MuGirlStau/StauCalibration.h"
#include "MuGirlInterfaces/CandidateSummary.h"
#include "CLHEP/Random/RandomEngine.h"

class MsgStream;

namespace Trk
{
    class Track;
}
namespace Muon
{
    class MdtDriftCircleOnTrack;
}
namespace MuonGM
{
    class MdtReadoutElement;
}

namespace MuGirlNS
{
    class StauTool;
    class MdttStepData;

    class StauMDTT
    {
    public:
        StauMDTT(StauTool* pStau, MsgStream& log,
                 CLHEP::HepRandomEngine& randEngine);
        ~StauMDTT();
        bool initialize(const Trk::Track* pTrack);
        void clear();
        std::string toString() const;

        void initStepData(MdttStepData* mdttData, double beta);
        void clearStepData(MdttStepData* mdttData);
        void printStepData(MdttStepData* mdttData);
        void processWithBeta(double currentBeta, MdttStepData* mdttData);
        inline StauHits getHits()
        {
            return m_hits;
        }
        void averageBeta();
        double avgBeta()
        {
            return m_avgBeta;
        }
        double rmsBeta()
        {
            return m_rmsBeta;
        }
        bool hasHits()
        {
            return !m_mdtts.empty();
        }

    private:
        struct MDTT
        {
            Trk::DriftCircleStatus status;
            const Amg::Vector3D pos;
            double mag;
            double driftRadius;
            double driftTime;
            double locR;
            double TlocR;
            double origTOF;
            double TOF;
            double t0Shift;
            double error;
            double error2;
            std::string stype;
            MDTT(StauTool* pStau, MsgStream& log, const Muon::MdtDriftCircleOnTrack* mdcot,
                    double locR, double t0Shift, double error, std::string stype);
            double StationT0Shift(const MuonGM::MdtReadoutElement* detEl);
        }; // class MDTT

        StauTool* m_pStau;
        MsgStream& m_log;
        const Trk::Track* m_pTrack;
        double m_baseBeta;
        std::vector<MDTT*> m_mdtts;
        static double m_inverseSpeedOfLight;
        StauHits m_hits;
        double m_avgBeta; //
        double m_rmsBeta; //

        StauCalibration::id_calibration_map* m_pCalibration;

        CLHEP::HepRandomEngine& m_randEngine;
    };
// class StauMDTT
}// namespace MuGirlsNS

#endif // MUGIRLNSSTAUMDTT_H

