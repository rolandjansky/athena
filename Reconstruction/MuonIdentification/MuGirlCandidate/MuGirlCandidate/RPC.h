/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRL_RPC_H
#define MUGIRL_RPC_H

#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include "MuGirlCandidate/Technology.h"

namespace MuGirlNS
{

    class RPC : public Technology
    {
    public:
        RPC(CandidateTool* pMuGirl, const std::string& sPrepDataCollection);
        virtual ~RPC();
        virtual const MuonGM::MuonReadoutElement* readoutElement(const Identifier& id) const;
        virtual int stationID(const Identifier& id) const;
        virtual int stationNameID(const std::string& name) const;
        virtual StatusCode retrievePrepData();
        virtual unsigned prepData(Chamber* pChamber, PrepDataList& array);
        virtual Amg::Vector3D hitPosition(const Trk::PrepRawData* pPrepData);
        virtual bool isEtaHit(const Trk::PrepRawData* pPrepData);
        virtual void buildSegments(Candidate* pCand, ChamberList& chambers, double dQOverP);

        virtual std::vector<IdentifierHash> retrieveAvailableCollections() const;
    
    //<S>====================================================
    // stauCandidate functions
    void stauHitBeta(Hit* pHit, double& d_beta, bool& b_isEta);
    //end of stauCandidate functions
    //<S>====================================================

    private:
        const Muon::RpcPrepDataContainer* m_pPrepDataContainer;    /**< The PrepData container */
    };

    inline RPC::~RPC()
    {
    }

}

#endif //MUGIRL_RPC_H

