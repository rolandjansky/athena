/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CscClusterUtilTool.h"

#include "CscClusterization/ICscClusterFitter.h"
#include "CscClusterization/ICscStripFitter.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonPrepRawData/CscPrepData.h"
#include "MuonPrepRawData/CscStripPrepDataCollection.h"
#include "MuonPrepRawData/CscStripPrepDataContainer.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"

using Muon::CscPrepData;
using Muon::CscStripPrepData;
using Muon::CscStripPrepDataCollection;
using Muon::CscStripPrepDataContainer;

//**********************************************************************

CscClusterUtilTool::CscClusterUtilTool(const std::string& type, const std::string& aname, const IInterface* parent)
    : AthAlgTool(type, aname, parent),
      m_phelper(0),
      m_cscStripLocation("CSC_Measurements")
{
    declareInterface<ICscClusterUtilTool>(this);
    declareProperty("CscStripPrepDataLocation", m_cscStripLocation);
}

//**********************************************************************

StatusCode
CscClusterUtilTool::initialize()
{

    ATH_MSG_DEBUG("Initializing " << name());
    ATH_MSG_DEBUG("  Strip fitter is " << m_stripFitter.typeAndName());
    ATH_CHECK(m_cscStripLocation.initialize());
    ATH_MSG_DEBUG("  CscStripPrepDataLocation is " << m_cscStripLocation.key());

    // Retrieve the strip fitting tool.
    ATH_CHECK(m_stripFitter.retrieve());
    ATH_MSG_DEBUG("Retrieved strip fitting tool " << m_stripFitter);

    // Retrieve the precision cluster fitting tool.
    ATH_CHECK(m_precClusterFitter.retrieve());
    ATH_MSG_DEBUG("Retrieved CSC precision cluster fitting tool");

    const MuonGM::MuonDetectorManager* muDetMgr=nullptr;
    ATH_CHECK_RECOVERABLE(detStore()->retrieve(muDetMgr));
    ATH_MSG_DEBUG("Retrieved geometry.");
    m_phelper = muDetMgr->cscIdHelper();

    return StatusCode::SUCCESS;
}

//**********************************************************************
void
CscClusterUtilTool::getStripFits(const Trk::RIO_OnTrack* rot, ICscClusterFitter::StripFitList& sfits) const
{

    if (!rot) {
        ATH_MSG_WARNING(" Trk::RIO_OnTrack* rot is empty !");
        return;
    }
    const Muon::CscClusterOnTrack* pclu = dynamic_cast<const Muon::CscClusterOnTrack*>(rot);
    if (!pclu) {
        ATH_MSG_WARNING(" Muon::CscClusterOnTrack* pclu is empty !");
        return;
    }
    getStripFits(pclu, sfits);

    return;
}

void
CscClusterUtilTool::getStripFits(const Muon::CscClusterOnTrack* pclu, ICscClusterFitter::StripFitList& sfits) const
{

    if (!pclu) {
        ATH_MSG_WARNING(" Muon::CscClusterOnTrack* pclu is empty !");
        return;
    }
    const Muon::CscPrepData* pprd = pclu->prepRawData();
    if (!pprd) {
        ATH_MSG_WARNING(" Muon::CscPrepData* pprd is empty !");
        return;
    }

    getStripFits(pprd, sfits);
    return;
}
////////////////////////////////////////////////////////////
void
CscClusterUtilTool::getStripFits(const CscPrepData* MClus, ICscClusterFitter::StripFitList& sfits) const
{

    if (!MClus) {
        ATH_MSG_WARNING(" Muon::CscPrepData* pprd is empty !");
        return;
    }

    sfits.clear();

    std::vector<const CscStripPrepData*> strPrepDatas = getStrips(MClus);
    for (unsigned int i = 0; i < strPrepDatas.size(); ++i) {
        ICscClusterFitter::StripFit sfit;
        sfit = m_stripFitter->fit(*strPrepDatas[i]);
        ATH_MSG_VERBOSE(" in loop charge " << sfit.charge);
        //    if (sfit.charge == 0.) // It's the case of narrow strips...in QratStripFitter
        //      sfit.charge = 40.;  // 0 charge is set to minimal non zero to do fit e.g. dead channel case
        sfits.push_back(sfit);
    }
    return;
}
///////////////////////////////////////////////////////////
ICscClusterFitter::Results
CscClusterUtilTool::getRefitCluster(const CscPrepData* MClus, double tantheta) const
{

    ICscClusterFitter::Results results;
    if (!MClus) {
        ATH_MSG_WARNING(" Muon::CscPrepData* pprd is empty !");
        return results;
    }

    ICscClusterFitter::StripFitList sfits;
    sfits.clear();

    getStripFits(MClus, sfits);
    results = m_precClusterFitter->fit(sfits, tantheta);

    return results;
}
/////////////////////////////////////////////////////////////////////////////

std::vector<const CscStripPrepData*>
CscClusterUtilTool::getStrips(const CscPrepData* MClus) const
{

    std::vector<const CscStripPrepData*> strips;
    if (!MClus) {
        ATH_MSG_WARNING(" Muon::CscPrepData* pprd is empty !");
        return strips;
    }

    std::vector<Identifier> prd_digit_ids = MClus->rdoList();

    SG::ReadHandle<Muon::CscStripPrepDataContainer> pdigcont(m_cscStripLocation);

    if (!pdigcont.isValid()) {
        ATH_MSG_WARNING("Strip container " << m_cscStripLocation.key() << " not found in StoreGate!");
        return strips;
    }
    ATH_MSG_DEBUG("Retrieved " << m_cscStripLocation.key() << " successfully. ");


    IdentifierHash elhash = MClus->collectionHash();
    auto           it     = pdigcont->indexFindPtr(elhash);

    ATH_MSG_VERBOSE("Hash " << elhash << " converted to iterator of container successfully");

    if (it != nullptr) {
        ATH_MSG_VERBOSE(" it == pdigcont.end() passed");
        for (unsigned int istrip = 0; istrip < prd_digit_ids.size(); ++istrip) {
            const CscStripPrepDataCollection& col = *it;
            // Loop over digits and fill these arrays.
            for (CscStripPrepDataCollection::const_iterator idig = col.begin(); idig != col.end(); ++idig) {
                const CscStripPrepData& dig  = **idig;
                Identifier              stid = dig.identify();
                if (stid != prd_digit_ids[istrip]) continue;
                const CscStripPrepData* pstrip = &dig;
                ATH_MSG_VERBOSE("strip " << pstrip->timeOfFirstSample());

                strips.push_back(pstrip);
                break;
            }
        }
    }
    return strips;
}
