/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonIdHelpers/MuonIdHelper.h"

#include "MuGirlCandidate/Technology.h"
#include "MuGirlCandidate/CandidateTool.h"
#include "MuGirlCandidate/StationManager.h"

namespace MuGirlNS
{

    Technology::Technology(CandidateTool* pMuGirl, const std::string& sPrepDataCollection) :
    m_eType(ID_TECH),
    m_pMuGirl(pMuGirl),
    m_pIdHelper(NULL),
    m_detId((DETID)0),
    m_sPrepDataCollection(sPrepDataCollection)
    {
    }

    int Technology::id(const IdentifierHash& hashId, Identifier& id) const
    {
        if (  m_eType == STGC_TECH || m_eType == MM_TECH ) {
            // the MM and sTGC preprawData (from fast digitization) have been arranged according to the detector element
            IdContext detElem_context = m_pIdHelper->detectorElement_context();
            return m_pIdHelper->get_id(hashId, id, &detElem_context);
        }

        // other technologies use the readout element (i.e. the multiplet / multilayer) for the collections
        IdContext module_context = m_pIdHelper->module_context();
        return m_pIdHelper->get_id(hashId, id, &module_context);
    }

    int Technology::hashId(const Identifier& id, IdentifierHash& hashId) const
    {
        if (  m_eType == STGC_TECH || m_eType == MM_TECH ) {
            // the MM and sTGC preprawData (from fast digitization) have been arranged according to the detector element
            IdContext detElem_context = m_pIdHelper->detectorElement_context();
            return m_pIdHelper->get_hash(id, hashId, &detElem_context);
        }

        // other technologies use the readout element (i.e. the multiplet / multilayer) for the collections
        IdContext module_context = m_pIdHelper->module_context();
        return m_pIdHelper->get_hash(id, hashId, &module_context);
    }

    Station* Technology::station(const Identifier& id) const
    {
        int stationId = stationID(id);
        return m_pMuGirl->stationManager()->station(stationId, m_eType);
    }

}
