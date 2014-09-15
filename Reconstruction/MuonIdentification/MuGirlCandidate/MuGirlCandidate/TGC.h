/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRL_TGC_H
#define MUGIRL_TGC_H

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuGirlCandidate/Technology.h"
#include <set>

class Identifier;

namespace MuGirlNS
{

    class TGC : public Technology
    {
    public:
        TGC(CandidateTool* pMuGirl, const std::string& sPrepDataCollection);
        virtual ~TGC();
        virtual const MuonGM::MuonReadoutElement* readoutElement(const Identifier& id) const;
        virtual int stationID(const Identifier& id) const;
        virtual int stationNameID(const std::string& name) const;
        virtual StatusCode retrievePrepData();
        virtual unsigned prepData(Chamber* pChamber, PrepDataList& array);
        virtual Amg::Vector3D hitPosition(const Trk::PrepRawData* pPrepData);
        virtual bool isEtaHit(const Trk::PrepRawData* pPrepData);
        virtual void buildSegments(Candidate* pCand, ChamberList& chambers, double dQOverP);

        virtual std::vector<IdentifierHash> retrieveAvailableCollections() const;

    private:
        const Muon::TgcPrepDataContainer* m_pPrepDataContainer;    /**< The PrepData container */
        const Muon::TgcPrepDataContainer* m_pPrepDataContainerN;    /**< The PrepData container */
        const Muon::TgcPrepDataContainer* m_pPrepDataContainerP;    /**< The PrepData container */

        bool addPrd(const Muon::TgcPrepData* prd, PrepDataList& array);
        std::set<Identifier> m_detElements;
    };

    inline TGC::~TGC()
    {
    }

}

#endif //MUGIRL_TGC_H

