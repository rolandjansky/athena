/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRL_MDT_H
#define MUGIRL_MDT_H

#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include "MuGirlCandidate/Technology.h"

namespace MuGirlNS
{

    class MDT : public Technology
    {
    public:
        MDT(CandidateTool* pMuGirl, const std::string& sPrepDataCollection);
        virtual ~MDT();
        virtual const MuonGM::MuonReadoutElement* readoutElement(const Identifier& id) const;
        virtual int stationID(const Identifier& id) const;
        virtual int stationNameID(const std::string& name) const;
        virtual StatusCode retrievePrepData();
        virtual unsigned prepData(Chamber* pChamber, PrepDataList& array);
        virtual Amg::Vector3D hitPosition(const Trk::PrepRawData* pPrepData);
        virtual bool isEtaHit(const Trk::PrepRawData* pPrepData);
        virtual void buildSegments(Candidate* pCand, ChamberList& chambers, double dQOverP);

        virtual std::vector<IdentifierHash> retrieveAvailableCollections() const;

    //<S>=======================================================
    // stauCandidate functions
    bool stauDeriveIteration(double d_beta,ChamberList& chambers,
             Clusters clusters, int& numOfHits, int& numOfSegments, double& chi2, HepGeom::Point3D<double>& globalPosition, HepGeom::Point3D<double>& globalDirection); // Incase of iteration w/o segment, return FALSE
    int stauModifyPrepData(ChamberList& chamberList, double d_beta, RIOLists& RioLists);
    int stauDelayWRTmuon(double d_beta,HepGeom::Point3D<double> tubePos);
    void stauChamberHitsToPrepData(Chamber* pChamber, PrepDataList& prepDataList);
    stauSegments* stauBuildMDTsegment(const ChamberList chambersList, RIOLists rioLists, Clusters clusters);
    //end of stauCandidate functions
    //<S>=======================================================

    private:
        const Muon::MdtPrepDataContainer* m_pPrepDataContainer;    /**< The PrepData container */
    };
   
    inline MDT::~MDT()
    {
    }

}

#endif //MUGIRL_MDT_H

