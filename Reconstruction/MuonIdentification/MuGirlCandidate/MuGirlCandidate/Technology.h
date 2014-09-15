/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRL_TECHNOLOGY_H
#define MUGIRL_TECHNOLOGY_H

#include "IRegionSelector/RegSelEnums.h"
#include "GaudiKernel/StatusCode.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "MuGirlCandidate/MuGirlNS.h"
#include "StoreGate/DataHandle.h"

class MuonIdHelper;

namespace MuonGM
{
    class MuonReadoutElement;
}

namespace MuGirlNS
{
    class CandidateTool;
    class Candidate;

    class Technology
    {
    public:
        Technology(CandidateTool* pMuGirl, const std::string& sPrepDataCollection);
        virtual ~Technology();

        TechnologyType type() const;
        const MuonIdHelper* idHelper() const;
        int id(const IdentifierHash& idHash, Identifier& id) const;
        int hashId(const Identifier& id, IdentifierHash& idHash) const;
        DETID detId() const;
        Station* station(const Identifier& id) const;

        virtual const MuonGM::MuonReadoutElement* readoutElement(const Identifier& id) const = 0;
        virtual int stationID(const Identifier& id) const = 0;
        virtual int stationNameID(const std::string& name) const = 0;
        virtual StatusCode retrievePrepData() = 0;
        virtual unsigned prepData(Chamber* pChamber, PrepDataList& array) = 0;
        virtual Amg::Vector3D hitPosition(const Trk::PrepRawData* pPrepData) = 0;
        virtual bool isEtaHit(const Trk::PrepRawData* pPrepData) = 0;
        virtual void buildSegments(Candidate* pCand, ChamberList& chambers, double dQOverP) = 0;

        virtual std::vector<IdentifierHash> retrieveAvailableCollections() const = 0;

    protected:
        TechnologyType m_eType;
        CandidateTool* m_pMuGirl;      /**< A pointer to the MuGirl tool */
        const MuonIdHelper* m_pIdHelper;
        DETID m_detId;
        std::string m_sPrepDataCollection;
    };

    inline Technology::~Technology()
    {
    }

    inline TechnologyType Technology::type() const
    {
        return m_eType;
    }

    inline const MuonIdHelper* Technology::idHelper() const
    {
        return m_pIdHelper;
    }

    inline DETID Technology::detId() const
    {
        return m_detId;
    }

}

#endif //MUGIRL_TECHNOLOGY_H
