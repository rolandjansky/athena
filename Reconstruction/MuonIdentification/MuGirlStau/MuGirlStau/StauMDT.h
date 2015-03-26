/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// StauTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUGIRLNSSTAUMDT_H
#define MUGIRLNSSTAUMDT_H

#include "GaudiKernel/MsgStream.h"
#include "MuGirlStau/StauToolDefinitions.h"
#include "MuGirlStau/StauTool.h"
#include "MuGirlStau/StauCalibration.h"
#include "CLHEP/Random/RandomEngine.h"


namespace MuGirlNS
{

    class StauMDT
    {
    public:
        StauMDT(StauTool* pStau,
                MsgStream& log,
                CLHEP::HepRandomEngine& randEngine,
                const MuGirlNS::MdtSegmentMakerInfoList& mdtSegmentMakerInfoList);

        /** default destructor */
        virtual ~StauMDT();

        void initStepData(MdtStepData* mdtData, double beta, double tTrack = 0);
        void clearStepData(MdtStepData* mdtData);
        void printStepData(MdtStepData* mdtData);
        void processMdtWithBeta(double currentBeta, MdtStepData* mdtData);
        void processMdtWithTTrack(double tTrack, MdtStepData* mdtData, double beta = 1.0);
        void findNewSegments(double beta);
        void clear();
        void setBeta(double beta)
        {
            m_beta = beta;
        }
        double beta()
        {
            return m_beta;
        }
    private:
        StauTool* m_pStau;
        MsgStream& m_log;
        const MuGirlNS::MdtSegmentMakerInfoList* m_pMdtSegmentMakerInfoList;
        double m_beta; //the beta of the MDT alone
        int recreateMdcots(MDCOTLists& mdts, MDCOTLists* MDCOTLists);
        const Trk::RIO_OnTrack* recreateRio(const Muon::MdtDriftCircleOnTrack* pMdcot);
        void deleteMdcotLists(MuGirlNS::MDCOTLists* pMdocotLists);
        void bestMdtSegment(MdtSegments* pSegments, const Muon::MuonSegment*& pBestSegment,
                bool ignoreNumHits = false);
        void fillStationData(MdtStepData* mdtData, MdtSegments* pSegments);

        //Calibration parameters
        StauCalibration::id_calibration_map* m_pCalibration;
        std::vector<std::vector<double> > m_shifts;
        void initCalibrationParameters();
        int m_segmentNumber;
        int m_chamberNumber;
        CLHEP::HepRandomEngine& m_randEngine;
    };

} // end of namespace

#endif
