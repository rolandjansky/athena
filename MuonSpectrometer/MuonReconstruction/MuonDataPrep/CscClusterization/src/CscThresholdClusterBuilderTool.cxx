/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


// CscThresholdClusterBuilderTool.cxx
#include "CscThresholdClusterBuilderTool.h"

#include <sstream>

#include "CscCalibTools/ICscCalibTool.h"
#include "CscClusterization/ICscClusterFitter.h"
#include "CscClusterization/ICscStripFitter.h"
#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "EventPrimitives/EventPrimitivesToStringConverter.h"
#include "Gaudi/Property.h"
#include "MuonPrepRawData/CscClusterStatus.h"
#include "MuonPrepRawData/CscPrepDataContainer.h"
#include "MuonPrepRawData/CscStripPrepData.h"
#include "MuonPrepRawData/CscStripPrepDataContainer.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "TrkSurfaces/Surface.h"

using Muon::CscClusterStatus;
using Muon::CscPrepData;
using Muon::CscPrepDataCollection;
using Muon::CscPrepDataContainer;
using Muon::CscStripPrepData;
using Muon::CscStripPrepDataCollection;
using Muon::CscStripPrepDataContainer;
using MuonGM::CscReadoutElement;
using std::ostringstream;
using std::setw;
using std::vector;

//******************************************************************************
// Local definitions.
namespace {
//******************************************************************************

// Convert chamber identifier to string.
std::string
chamber(int istation, int zsec, int phi)
{
    ostringstream ssout;
    if (istation == 1)
        ssout << "CSS";
    else if (istation == 2)
        ssout << "CSL";
    else
        ssout << "???";
    if (zsec == -1)
        ssout << "-";
    else if (zsec == 1)
        ssout << "+";
    else
        ssout << "?";
    ssout << phi;
    return ssout.str();
}

// Convert measphi to string.
std::string
setaphi(bool measphi)
{
    if (measphi) return "phi";
    return "eta";
}

//******************************************************************************
}  // end unnamed namespace
//******************************************************************************

CscThresholdClusterBuilderTool::CscThresholdClusterBuilderTool(const std::string& type, const std::string& aname,
                                                               const IInterface* parent)
    : AthAlgTool(type, aname, parent),
      m_noiseOption(rms),
      m_digit_key("CSC_Measurements"),
      m_pclusters("CSC_Clusters"),
      m_cscCalibTool("CscCalibTool/CscCalibTool"),
      m_pstrip_fitter("CalibCscStripFitter/CalibCscStripFitter"),
      m_pfitter_def("SimpleCscClusterFitter/SimpleCscClusterFitter"),
      m_pfitter_prec("QratCscClusterFitter/QratCscClusterFitter"),
      m_pfitter_split("CscSplitClusterFitter/CscSplitClusterFitter"),
      m_fullEventDone(false)
{

    declareInterface<ICscClusterBuilder>(this);

    declareProperty("threshold", m_threshold = 20000.0);
    declareProperty("kFactor", m_kFactor = 6.5);
    declareProperty("noiseOption", m_noiseOptionStr = "f001");
    declareProperty("digit_key", m_digit_key);
    declareProperty("cluster_key", m_pclusters);
    declareProperty("cscCalibTool", m_cscCalibTool);
    declareProperty("strip_fitter", m_pstrip_fitter);
    declareProperty("default_fitter", m_pfitter_def);
    declareProperty("precision_fitter", m_pfitter_prec);
    declareProperty("split_fitter", m_pfitter_split);
    declareProperty("makeNarrowClusterThreeStrips", m_makeNarrowClusterThreeStrips = true);
}

//******************************************************************************

// Destructor.

CscThresholdClusterBuilderTool::~CscThresholdClusterBuilderTool() {}

//******************************************************************************

StatusCode
CscThresholdClusterBuilderTool::initialize()
{

    // Display algorithm properties.
    ATH_MSG_DEBUG("Properties for " << name() << ":");
    ATH_MSG_DEBUG("  Strip threshold is Max( " << m_threshold << ", " << m_kFactor
                                               << "*stripNoise ) where stripNoise is from " << m_noiseOptionStr);

    ATH_CHECK(m_digit_key.initialize());
    ATH_CHECK(m_pclusters.initialize());
    if (m_noiseOptionStr != "rms" && m_noiseOptionStr != "sigma" && m_noiseOptionStr != "f001") {
        ATH_MSG_DEBUG(" noiseOption is not among rms/sigma/f001. rms is used for default!!");
        m_noiseOptionStr = "rms";
    }
    if (m_noiseOptionStr == "rms")
        m_noiseOption = rms;
    else if (m_noiseOptionStr == "sigma")
        m_noiseOption = sigma;
    else if (m_noiseOptionStr == "f001")
        m_noiseOption = f001;

    ATH_MSG_DEBUG("  Strip fitter is " << m_pstrip_fitter.typeAndName());
    ATH_MSG_DEBUG("  Default cluster fitter is " << m_pfitter_def.typeAndName());
    ATH_MSG_DEBUG("  Precision cluster fitter is " << m_pfitter_prec.typeAndName());
    ATH_MSG_DEBUG("  Split cluster fitter is " << m_pfitter_split.typeAndName());
    ATH_MSG_DEBUG("  Input digit key is " << m_digit_key.key());
    ATH_MSG_DEBUG("  Output cluster key is " << m_pclusters.key());

    // CSC calibratin tool for the Condtiions Data base access //
    if (m_cscCalibTool.retrieve().isFailure()) {
        ATH_MSG_ERROR("Can't get handle on CSC calibration tools");
        return StatusCode::RECOVERABLE;
    }

    // Retrieve the strip fitting tool.
    if (m_pstrip_fitter.retrieve().isFailure()) {
        ATH_MSG_ERROR("Unable to retrieve strip fitting tool " << m_pstrip_fitter);
        return StatusCode::RECOVERABLE;
    }
    ATH_MSG_DEBUG("Retrieved strip fitting tool " << m_pstrip_fitter);

    // Retrieve the default cluster fitting tool.
    if (m_pfitter_def.retrieve().isFailure()) {
        ATH_MSG_ERROR("Unable to retrieve CSC default cluster fitting tool " << m_pfitter_def->name());
        return StatusCode::RECOVERABLE;
    }
    ATH_MSG_DEBUG("Retrieved CSC default cluster fitting tool");

    // Retrieve the precision cluster fitting tool.
    if (m_pfitter_prec.retrieve().isFailure()) {
        ATH_MSG_ERROR("Unable to retrieve CSC precision cluster fitting tool " << m_pfitter_prec->name());
        return StatusCode::RECOVERABLE;
    }
    ATH_MSG_DEBUG("Retrieved CSC precision cluster fitting tool");

    // Retrieve the split cluster fitting tool.
    if (m_pfitter_split.retrieve().isFailure()) {
        ATH_MSG_ERROR("Unable to retrieve CSC split cluster fitting tool " << m_pfitter_split->name());
        return StatusCode::RECOVERABLE;
    }
    ATH_MSG_DEBUG("Retrieved CSC split cluster fitting tool");

    // retrieve MuonDetectorManager from the conditions store
    ATH_CHECK(m_DetectorManagerKey.initialize());
    ATH_CHECK(m_idHelperSvc.retrieve());

    return StatusCode::SUCCESS;
}

//******************************************************************************

StatusCode
CscThresholdClusterBuilderTool::getClusters(std::vector<IdentifierHash>& givenIDs,
                                            std::vector<IdentifierHash>& decodedIds)
{

    // clear output vector of selected data collections containing data
    decodedIds.clear();
    SG::WriteHandle<Muon::CscPrepDataContainer> wh_pclusters(m_pclusters);
    CscPrepDataContainer* object = new CscPrepDataContainer(m_idHelperSvc->cscIdHelper().module_hash_max());

    if (!wh_pclusters.isPresent()) {
        /// record the container in storeGate
        if (wh_pclusters.record(std::unique_ptr<Muon::CscPrepDataContainer>(object)).isFailure()) {
            ATH_MSG_ERROR("Could not record container of CSC Cluster PrepData at " << m_pclusters.key());
            return StatusCode::RECOVERABLE;
        }
        m_fullEventDone = false;
        if (givenIDs.size() == 0) m_fullEventDone = true;

    } else {
        ATH_MSG_DEBUG("CSC Cluster PrepData Container is already in StoreGate ");
        if (m_fullEventDone) {
            ATH_MSG_DEBUG("Whole event has already been processed; nothing to do");
            return StatusCode::SUCCESS;
        }
        if (givenIDs.size() == 0) m_fullEventDone = true;
    }

    if (givenIDs.size() != 0) {
        for (unsigned int i = 0; i < givenIDs.size(); ++i) {
            if (getClusters(givenIDs[i], decodedIds, object).isFailure()) {
                ATH_MSG_ERROR("Unable to decode CSC RDO " << i << "th into CSC PrepRawData");
                return StatusCode::RECOVERABLE;
            }
        }
    } else {
        // Clusterization is done for every area
        if (getClusters(decodedIds, object).isFailure()) {
            ATH_MSG_ERROR("Unable to decode CSC RDO into CSC PrepRawData");
            return StatusCode::RECOVERABLE;
        }
    }


    return StatusCode::SUCCESS;
}


//******************************************************************************

StatusCode
CscThresholdClusterBuilderTool::getClusters(IdentifierHash givenHashId, std::vector<IdentifierHash>& decodedIds,
                                            Muon::CscPrepDataContainer* pclusters)
{

    // identifiers of collections already decoded and stored in the container will be skipped
    if (pclusters->indexFindPtr(givenHashId) != nullptr) {
        decodedIds.push_back(givenHashId);
        ATH_MSG_DEBUG("A collection already exists in the container for offline id hash. " << (int)givenHashId);
        return StatusCode::SUCCESS;
    }


    // Retrieve the CSC digits for this event.
    SG::ReadHandle<CscStripPrepDataContainer> pdigcon(m_digit_key);
    if (pdigcon.isValid()) {
        ATH_MSG_DEBUG("Retrieved strip container " << m_digit_key.key() << " with " << pdigcon->size() << " entries.");
    } else {
        ATH_MSG_WARNING("Failure to retrieve strip container " << m_digit_key.key());
        return StatusCode::SUCCESS;
    }


    //**********************************************
    // retrieve specific collection for the givenID
    const CscStripPrepDataCollection* col = pdigcon->indexFindPtr(givenHashId);
    if (nullptr == col) {
        unsigned int coll_hash = givenHashId;
        ATH_MSG_WARNING("Specific CSC Strip PrepData collection retrieving failed for collection hash = " << coll_hash);
        return StatusCode::SUCCESS;
    }

    ATH_MSG_DEBUG("Retrieved " << col->size() << " CSC Strip PrepDatas.");

    Identifier colid    = col->identify();
    int        istation = m_idHelperSvc->cscIdHelper().stationName(colid) - 49;
    int        zsec     = m_idHelperSvc->cscIdHelper().stationEta(colid);
    int        phisec   = m_idHelperSvc->cscIdHelper().stationPhi(colid);

    ATH_MSG_DEBUG("  Strip collection " << chamber(istation, zsec, phisec) << " has " << col->size() << " strips");

    // Create arrays to hold digits and cathode plane parameters.
    vector<const CscStripPrepData*> strips[8];
    int                             maxstrip[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    // retrieve MuonDetectorManager from the conditions store
    SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey};
    const MuonGM::MuonDetectorManager*              MuonDetMgr = DetectorManagerHandle.cptr();
    if (MuonDetMgr == nullptr) {
        ATH_MSG_ERROR("Null pointer to the MuonDetectorManager conditions object");
        return StatusCode::FAILURE;
    }

    IdentifierHash hash;
    // Loop over digits and fill these arrays.
    for (CscStripPrepDataCollection::const_iterator idig = col->begin(); idig != col->end(); ++idig) {
        const CscStripPrepData& dig      = **idig;
        Identifier              did      = dig.identify();
        hash                             = dig.collectionHash();
        const CscReadoutElement* pro     = MuonDetMgr->getCscReadoutElement(did);
        int                      wlay    = m_idHelperSvc->cscIdHelper().wireLayer(did);
        int                      measphi = m_idHelperSvc->cscIdHelper().measuresPhi(did);
        int                      idx     = 2 * (wlay - 1) + measphi;
        // First entry for a cathode plane, initialize.
        if (maxstrip[idx] == 0) {
            maxstrip[idx] = pro->maxNumberOfStrips(measphi);
            for (int istrip = 0; istrip < maxstrip[idx]; ++istrip) strips[idx].push_back(0);
        }
        int istrip = m_idHelperSvc->cscIdHelper().strip(did) - 1;
        if (istrip < 0 || istrip >= maxstrip[idx]) {
            ATH_MSG_WARNING("Invalid strip number");
            continue;
        }
        strips[idx][istrip] = &dig;
    }

    // Cluster.
    CscPrepDataCollection* newCollection = 0;
    for (int measphi = 0; measphi < 2; ++measphi) {
        for (int wlay = 1; wlay < 5; ++wlay) {
            int idx = 2 * (wlay - 1) + measphi;
            if (maxstrip[idx]) {
                make_clusters(measphi, strips[idx], newCollection);
                ATH_MSG_DEBUG("    " << wlay << "th layer ");
            }
        }
    }
    if (newCollection) {
        if (pclusters->addCollection(newCollection, hash).isFailure()) {
            ATH_MSG_ERROR("Couldn't add CscPrepdataCollection to container!");
            return StatusCode::RECOVERABLE;
        }
        decodedIds.push_back(hash);  // Record that this collection contains data
    }

    return StatusCode::SUCCESS;
}


//******************************************************************************

StatusCode
CscThresholdClusterBuilderTool::getClusters(std::vector<IdentifierHash>& decodedIds,
                                            Muon::CscPrepDataContainer*  pclusters)
{

    // Retrieve the CSC digits for this event.
    SG::ReadHandle<CscStripPrepDataContainer> pdigcon(m_digit_key);
    if (pdigcon.isValid()) {
        ATH_MSG_DEBUG("Retrieved strip container " << m_digit_key.key() << " with " << pdigcon->size() << " entries.");
    } else {
        ATH_MSG_WARNING("Failure to retrieve strip container " << m_digit_key.key());
        return StatusCode::SUCCESS;
    }

    // Loop over digit collections.
    // This a loop over chambers (each with 4 wire planes).
    const CscStripPrepDataContainer& con = *pdigcon;
    for (CscStripPrepDataContainer::const_iterator icol = con.begin(); icol != con.end(); ++icol) {
        const CscStripPrepDataCollection& col = **icol;
        // check if the collection is already used
        if (pclusters->indexFindPtr(col.identifyHash()) != nullptr) {
            // store the identifier hash and continue
            decodedIds.push_back(col.identifyHash());
            continue;
        }
        Identifier colid    = col.identify();
        int        istation = m_idHelperSvc->cscIdHelper().stationName(colid) - 49;
        int        zsec     = m_idHelperSvc->cscIdHelper().stationEta(colid);
        int        phisec   = m_idHelperSvc->cscIdHelper().stationPhi(colid);
        ATH_MSG_DEBUG("**Strip collection " << chamber(istation, zsec, phisec) << " sector "
                                            << m_idHelperSvc->cscIdHelper().sector(colid) << " has " << col.size()
                                            << " strips");

        // Create arrays to hold digits and cathode plane parameters.
        vector<const CscStripPrepData*> strips[8];
        int                             maxstrip[8] = {0, 0, 0, 0, 0, 0, 0, 0};

        // retrieve MuonDetectorManager from the conditions store
        SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey};
        const MuonGM::MuonDetectorManager*              MuonDetMgr = DetectorManagerHandle.cptr();
        if (MuonDetMgr == nullptr) {
            ATH_MSG_ERROR("Null pointer to the MuonDetectorManager conditions object");
            return StatusCode::FAILURE;
        }

        IdentifierHash hash;
        // Loop over digits and fill these arrays.
        for (CscStripPrepDataCollection::const_iterator idig = col.begin(); idig != col.end(); ++idig) {
            const CscStripPrepData& dig      = **idig;
            Identifier              did      = dig.identify();
            hash                             = dig.collectionHash();
            const CscReadoutElement* pro     = MuonDetMgr->getCscReadoutElement(did);
            int                      wlay    = m_idHelperSvc->cscIdHelper().wireLayer(did);
            int                      measphi = m_idHelperSvc->cscIdHelper().measuresPhi(did);
            int                      idx     = 2 * (wlay - 1) + measphi;
            // First entry for a cathode plane, initialize.
            if (maxstrip[idx] == 0) {
                maxstrip[idx] = pro->maxNumberOfStrips(measphi);
                for (int istrip = 0; istrip < maxstrip[idx]; ++istrip) strips[idx].push_back(0);
            }
            int istrip = m_idHelperSvc->cscIdHelper().strip(did) - 1;
            if (istrip < 0 || istrip >= maxstrip[idx]) {
                ATH_MSG_WARNING("Invalid strip number");
                continue;
            }
            strips[idx][istrip] = &dig;
        }

        // Cluster.
        CscPrepDataCollection* newCollection = 0;
        for (int measphi = 0; measphi < 2; ++measphi) {
            for (int wlay = 1; wlay < 5; ++wlay) {
                int idx = 2 * (wlay - 1) + measphi;
                if (maxstrip[idx]) {
                    ATH_MSG_DEBUG("*** " << chamber(istation, zsec, phisec) << " sector "
                                         << m_idHelperSvc->cscIdHelper().sector(colid) << "  " << wlay << "th layer ");
                    make_clusters(measphi, strips[idx], newCollection);
                }
            }
        }
        if (newCollection) {
            if (pclusters->addCollection(newCollection, hash).isFailure()) {
                ATH_MSG_ERROR("Couldn't add CscPrepdataCollection to container!");
                return StatusCode::FAILURE;
            }
            decodedIds.push_back(hash);  // Record that this collection contains data
        }
    }  // end loop over chambers

    return StatusCode::SUCCESS;
}
//******************************************************************************

StatusCode
CscThresholdClusterBuilderTool::finalize()
{
    ATH_MSG_VERBOSE("Finalizing " << name());
    return StatusCode::SUCCESS;
}

//******************************************************************************

// Build clusters.
//   dump - whether to write messages
//   dstrip = CSC digit pointer for each strip.
//   qstrip - charge on each strip
// Note strip numbering is 0, maxstA, shifted by 1 from ATLAS strip numbers.
// Center of strip is at pitch * (istrip + 0.5 - maxstrip/2).

// NOTE: vector<CscStripPrepData*> strips is filled up with full strips (48/192)
// some of them have null pointer. Useful to find adjacent strip CscStripPrepData...

int
CscThresholdClusterBuilderTool::make_clusters(bool measphi, const vector<const CscStripPrepData*>& strips,
                                              CscPrepDataCollection*& newCollection)
{

    // Loop over channels.
    unsigned int maxstrip = strips.size();

    ATH_MSG_DEBUG("    Clustering for " << setaphi(measphi) << " plane with " << maxstrip << " strips");

    ////////////////////////////////////////////////////////////////////
    // Phase I:
    //
    // Loop over strips and fetch the charge and time for each.
    // Also set flag indicating if this strip has pointer and charge is above threshold(active)
    /////////////////
    ICscClusterFitter::StripFitList allStripfits;
    std::vector<bool>               astrip;  // check active strip
    std::vector<bool>               bstrip;  // check bad strip
    IdentifierHash                  cscHashId;

    // Always [0, 191] or [0, 47]
    for (unsigned int istrip = 0; istrip < strips.size(); ++istrip) {
        const CscStripPrepData*     pstrip = strips[istrip];
        ICscClusterFitter::StripFit res;
        bool                        active       = false;
        bool                        isBadChannel = false;
        if (pstrip) {
            if (!newCollection) {
                Identifier elementId = m_idHelperSvc->cscIdHelper().elementID(pstrip->identify());
                cscHashId            = pstrip->collectionHash();
                newCollection        = new CscPrepDataCollection(cscHashId);
                newCollection->setIdentifier(elementId);
            }
            res = m_pstrip_fitter->fit(*pstrip);


            IdentifierHash stripHash;
            Identifier     stripId = pstrip->identify();
            if (m_idHelperSvc->cscIdHelper().get_channel_hash(stripId, stripHash)) {
                ATH_MSG_WARNING("Unable to get CSC striphash id "
                                << " the identifier is ");
                stripId.show();
            }

            if (res.stripStatus == Muon::CscStrStatHot || res.stripStatus == Muon::CscStrStatDead) isBadChannel = true;

            float stripNoise = 0;
            if (m_noiseOption == rms) {
                stripNoise = m_cscCalibTool->stripRMS(stripHash);
            } else if (m_noiseOption == sigma) {
                stripNoise = m_cscCalibTool->stripNoise(stripHash);
            } else if (m_noiseOption == f001) {  // f001 is rawADC +1
                stripNoise = m_cscCalibTool->stripF001(stripHash) - m_cscCalibTool->stripPedestal(stripHash);
                stripNoise /= 3.251;
            }

            active = res.charge >= m_threshold && res.charge >= m_kFactor * stripNoise;
            if (isBadChannel) active = false;  // Let's remove Bad channel First...

            if (msgLvl(MSG::DEBUG)) {
                // Log message.
                ostringstream strlog;
                strlog << "      Strip " << setw(3) << istrip + 1 << ": charge= " << setw(7) << int(res.charge)
                       << " dcharge= " << setw(7) << int(res.dcharge);
                if (std::fabs(res.time) < 1e8)
                    strlog << " time=" << setw(3) << int(res.time + 0.5);
                else
                    strlog << " time=OVERFLOW";
                if (active)
                    strlog << " *";
                else if (isBadChannel)
                    strlog << " b";
                else
                    strlog << " .";
                if (res.status)
                    strlog << " x";
                else
                    strlog << " o";
                strlog << "   :" << res.status;
                ATH_MSG_DEBUG(strlog.str());
            }
        }
        allStripfits.push_back(res);
        astrip.push_back(active);
        bstrip.push_back(isBadChannel);
    }

    //////////////////////////////////////////////////////
    // Phase II:
    //
    // Bad Channel recovery in case of strip above strip being nearby
    /////////////////////////////////////////////////////////

    // 1. identify strips to recover
    std::vector<bool> rstrip;  // check recover strip
    bool              IsAnyStripRecovered = false;
    for (unsigned int istrip = 0; istrip < strips.size(); ++istrip) {
        bool adjacentActive = false;
        if (bstrip[istrip]) {
            if (istrip > 0 && astrip[istrip - 1]) adjacentActive = true;
            if (istrip + 1 < strips.size() && astrip[istrip + 1]) adjacentActive = true;
            if (adjacentActive) IsAnyStripRecovered = true;
        }
        rstrip.push_back(adjacentActive);
    }

    // 2. make it active if strip to recover is not active
    if (IsAnyStripRecovered)
    {  // This loop is needed if there is any bad strip recovered because of adjacent active strip

        if (msgLvl(MSG::DEBUG)) {

            ostringstream checklog1;
            ostringstream checklog2;

            for (unsigned int istrip = 0; istrip < strips.size(); ++istrip) {
                if (istrip % 24 == 0) checklog1 << "\n";
                checklog1 << int(astrip[istrip]) << " ";

                if (!astrip[istrip] && rstrip[istrip]) {  // not active but bad strip with adjacent strip active
                    ATH_MSG_DEBUG("**** Strip " << istrip << " is recovered!!");
                }
                if (istrip % 24 == 0) checklog2 << "\n";
                checklog2 << int(astrip[istrip]) << " ";
            }
            ATH_MSG_DEBUG("Strip active map before and after");
            ATH_MSG_DEBUG(checklog1.str());
            ATH_MSG_DEBUG(checklog2.str());
        }

        for (unsigned int istrip = 0; istrip < strips.size(); ++istrip) {
            if (!astrip[istrip] && rstrip[istrip]) {  // not active but bad strip with adjacent strip active
                astrip[istrip] = rstrip[istrip];      // insert true
            }
        }
    }
    //////////////////////////////////////////////////////
    // Phase III:
    //
    // Find clusters : save first strip and nstrip
    /////////////////////////////////////////////////////////
    vector<unsigned int> strip0s;
    vector<unsigned int> nstrips;

    // Loop over strips and create clusters.
    int  nstrip      = 0;
    int  first_strip = 0;  // First strip in the cluster.
    bool incluster   = false;
    for (unsigned int istrip = 0; istrip < strips.size(); ++istrip) {
        // If the current strip is not active, skip it.
        if (!astrip[istrip]) continue;
        assert(strips[istrip] != 0);  //  CscStripPrepData* pstrip = strips[istrip];

        if (!incluster) {
            incluster   = true;
            nstrip      = 0;
            first_strip = istrip;
        }
        ++nstrip;

        // If this is not the last strip in the plane, and the next
        // strip is active, add the next strip to the cluster.
        if (istrip != maxstrip - 1 && astrip[istrip + 1]) continue;

        // Recover narrow cluster
        if (!measphi && nstrip < 3) {
            bool collectLeftStrip  = false;
            bool collectRightStrip = false;

            if (nstrip == 1) {
                if (int(istrip) >= nstrip     // left adjacent strip should be inside of CSC chamber >0
                    && istrip + 1 < maxstrip  // the other side strip should be available < 192
                    && (allStripfits[istrip - 1].charge > 0.1
                        && allStripfits[istrip + 1].charge > 0.1)  // charge should be positive
                    && strips[istrip - 1] && strips[istrip + 1])
                {  // both adjacent strip identifier should exist
                    collectLeftStrip  = true;
                    collectRightStrip = true;
                }
            } else if (nstrip == 2) {
                if (allStripfits[istrip - 1].charge > allStripfits[istrip].charge) {  // In case of left strip not fired
                    if (int(istrip) >= nstrip                                         // nstrip 2
                        && allStripfits[istrip - 2].charge > 0.1                      // charge should be positive
                        && strips[istrip - 2])  // left strip Identifier should exist
                        collectLeftStrip = true;
                } else {  // In case of right strip not fired
                    if (istrip + 1 < maxstrip
                        && allStripfits[istrip + 1].charge
                               > 0.1            // charge should be positive if 0, then 0.341E-134 will enter
                        && strips[istrip + 1])  // right strip Identifier should exist
                        collectRightStrip = true;
                }
            }

            if (collectLeftStrip) {
                first_strip = first_strip - 1;
                nstrip += 1;
            }
            if (collectRightStrip) {
                nstrip += 1;
            }


            if (msgLvl(MSG::DEBUG)) {
                // Log message.
                ostringstream narrowlog;
                narrowlog << "  ** narrow Clusters   " << first_strip + 1 << "  " << nstrip << "  L:R  "
                          << collectLeftStrip << " " << collectRightStrip;
                for (int i = 0; i < nstrip; ++i) {
                    narrowlog << "  " << allStripfits[first_strip + i].charge;
                }
                for (int i = 0; i < nstrip; ++i) {
                    narrowlog << "  " << strips[first_strip + i];
                }
                ATH_MSG_DEBUG(narrowlog.str());
            }
        }  // Only for eta plane nstrip <3

        strip0s.push_back(first_strip);
        nstrips.push_back(nstrip);

        // Reset incluster.
        incluster = false;
    }


    //////////////////////////////////////////////////////
    // Phase IV:
    //
    // Merge narrow cluster into adjacent cluster if any exists.
    // Only for eta strips...
    /////////////////////////////////////////////////////////
    vector<unsigned int> newStrip0s;
    vector<unsigned int> newNstrips;

    int nMerged = 0;  // the difference b/w old Nclu and new Nclu
    for (unsigned int icl = 0; icl < nstrips.size(); ++icl) {
        unsigned int nstrip = nstrips[icl];
        unsigned int strip0 = strip0s[icl];

        ATH_MSG_VERBOSE("          " << icl << "th cluster merger " << strip0 << " " << nstrip);

        //#### if you find narrow cluster
        if (!measphi) {
            if (nstrip < 3) {
                // at least one cluster before to check left cluster and continuous
                if (icl > 0 && (strip0 == strip0s[icl - 1] + nstrips[icl - 1])) {
                    unsigned int newStrip0 = strip0s[icl - 1];
                    unsigned int newNstrip = nstrips[icl - 1] + nstrip;

                    ATH_MSG_DEBUG("   " << icl << "   ** narrow Cluster merger Type I" << newStrip0 << "  "
                                        << newNstrip);

                    newStrip0s[icl - 1 - nMerged] = newStrip0;
                    newNstrips[icl - 1 - nMerged] = newNstrip;
                    ++nMerged;
                    continue;
                }
                // at least one cluster after to check right cluster and continuous
                if (icl + 1 < nstrips.size() && (strip0 + nstrip == strip0s[icl + 1])) {
                    unsigned int newStrip0 = strip0;
                    unsigned int newNstrip = nstrip + nstrips[icl + 1];

                    ATH_MSG_DEBUG("   " << icl << "   ** narrow Cluster merger Type II" << newStrip0 << "  "
                                        << newNstrip);

                    newStrip0s.push_back(newStrip0);
                    newNstrips.push_back(newNstrip);

                    icl += 1;
                    ++nMerged;
                    continue;
                }
            }
        }  // !measphi
        // if nstrip >2 OR
        // still narrow strip then just keep it...
        newStrip0s.push_back(strip0);
        newNstrips.push_back(nstrip);
    }  // for

    if (strip0s.size() != newStrip0s.size()) {
        ATH_MSG_DEBUG(" Phase II -> III Merged " << strip0s.size() << ":" << nstrips.size() << "  " << newStrip0s.size()
                                                 << ":" << newNstrips.size());
        for (unsigned int icl = 0; icl < nstrips.size(); ++icl)
            ATH_MSG_DEBUG("   *** " << icl << "   [" << strip0s[icl] << "," << strip0s[icl] + nstrips[icl] - 1 << "]  "
                                    << nstrips[icl]);
        for (unsigned int icl = 0; icl < newNstrips.size(); ++icl)
            ATH_MSG_DEBUG("   ****** " << icl << "   [" << newStrip0s[icl] << ","
                                       << newStrip0s[icl] + newNstrips[icl] - 1 << "]  " << newNstrips[icl]);
    }


    //////////////////////////////////////////////////////
    // Phase V:
    //
    // Using strip0 and nstrip fill up collection
    /////////////////////////////////////////////////////////

    ICscClusterFitter::StripFitList      sfits;
    std::vector<const CscStripPrepData*> clusterStrips;
    clusterStrips.reserve(50);
    std::vector<Identifier> prd_digit_ids;
    prd_digit_ids.reserve(50);

    for (unsigned int icl = 0; icl < newNstrips.size(); ++icl) {  // for each cluster

        ATH_MSG_VERBOSE("      Creating " << icl << "th cluster");

        unsigned int nstrip = newNstrips[icl];  // only used here
        unsigned int strip0 = newStrip0s[icl];  // only used here


        sfits.clear();
        clusterStrips.clear();
        prd_digit_ids.clear();

        for (unsigned int ist = strip0; ist < strip0 + nstrip; ++ist) {
            const CscStripPrepData*     pstrip = strips[ist];
            ICscClusterFitter::StripFit sfit   = allStripfits[ist];

            sfits.push_back(sfit);
            clusterStrips.push_back(pstrip);
            prd_digit_ids.push_back(pstrip->identify());
        }

        ATH_MSG_VERBOSE("     ++++++++++++++  nstrip +++++   " << nstrip);
        //////////////////////////////////////////////////////////
        if (nstrip < 3 && m_makeNarrowClusterThreeStrips) {
            ////////////////////////////////////////////////////////////////////////////////////
            /// Make every cluster have three strips ///////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////////////////////

            bool leftToFill  = false;
            bool rightToFill = false;
            if (nstrip == 1) {
                leftToFill  = true;
                rightToFill = true;
            } else {
                if (sfits[0].charge > sfits[1].charge) {
                    leftToFill = true;
                } else if (sfits[0].charge < sfits[1].charge) {
                    rightToFill = true;
                } else {
                    ATH_MSG_WARNING("  It should be CHECKED!!! ");
                    if (strip0 > 0) {
                        if (strips[strip0 - 1]) {
                            leftToFill = true;
                        } else if (strips[strip0 + 2]) {
                            rightToFill = true;
                        }
                    } else if (strips[strip0 + 2]) {
                        rightToFill = true;
                    }
                }
            }

            ATH_MSG_VERBOSE(" strip0  nstrip  filling left or right " << strip0 << "  " << nstrip << " " << leftToFill
                                                                      << " " << rightToFill);
            ATH_MSG_VERBOSE("  sfits[0] " << sfits[0].charge);
            if (nstrip == 2) ATH_MSG_VERBOSE("  sfits[1] " << sfits[1].charge);

            for (unsigned int i = 0; i < allStripfits.size(); ++i) {
                ATH_MSG_VERBOSE("index " << i << "  " << allStripfits[i].charge);
            }


            if (leftToFill) {
                //        ATH_MSG_DEBUG( " Left to fill " << allStripfits[strip0-1].charge);
                bool fillTheOtherSide = false;
                if (strip0 == 0) {
                    fillTheOtherSide = true;
                } else {
                    if (strips[strip0 - 1] == NULL) fillTheOtherSide = true;
                }

                if (strip0 + nstrip >= allStripfits.size()) {
                    fillTheOtherSide = false;
                }


                if (!fillTheOtherSide) {
                    if (strips[strip0 - 1]) {
                        sfits.insert(sfits.begin(), allStripfits[strip0 - 1]);
                        clusterStrips.insert(clusterStrips.begin(), strips[strip0 - 1]);
                        prd_digit_ids.insert(prd_digit_ids.begin(), strips[strip0 - 1]->identify());
                        strip0--;
                        nstrip = prd_digit_ids.size();
                    }
                } else {
                    if (strips[strip0 + nstrip]) {                       // for edge this can happen
                        sfits.push_back(allStripfits[strip0 + nstrip]);  // This is the case for example
                        // 12799.6 39183.9 39698
                        clusterStrips.push_back(strips[strip0 + nstrip]);
                        prd_digit_ids.push_back(strips[strip0 + nstrip]->identify());
                        nstrip = prd_digit_ids.size();
                    }
                }
            }

            if (rightToFill) {

                bool fillTheOtherSide = false;
                if (strip0 + nstrip >= allStripfits.size()) {
                    fillTheOtherSide = true;
                } else {
                    if (strips[strip0 + nstrip] == NULL) fillTheOtherSide = true;
                }

                if (strip0 == 0) {
                    fillTheOtherSide = false;
                }

                if (!fillTheOtherSide) {
                    if (strips[strip0 + nstrip]) {
                        sfits.push_back(allStripfits[strip0 + nstrip]);
                        clusterStrips.push_back(strips[strip0 + nstrip]);
                        prd_digit_ids.push_back(strips[strip0 + nstrip]->identify());
                        nstrip = prd_digit_ids.size();
                    }
                } else {
                    if (strips[strip0 - 1]) {  // for edge this can happen
                        sfits.insert(sfits.begin(), allStripfits[strip0 - 1]);
                        clusterStrips.insert(clusterStrips.begin(), strips[strip0 - 1]);
                        prd_digit_ids.insert(prd_digit_ids.begin(), strips[strip0 - 1]->identify());
                        strip0--;
                        nstrip = prd_digit_ids.size();
                    }
                }
            }
        }
        ///////////////////////////////////////////////////////////////////////////


        int                                    fitresult = 99;
        std::vector<ICscClusterFitter::Result> results;

        // Precision fit.
        if (!measphi) {
            results   = m_pfitter_prec->fit(sfits);
            fitresult = results[0].fitStatus;
            ATH_MSG_VERBOSE("        Performing precision fit " << m_pfitter_prec << " result return=" << fitresult);

            // in case of multipeak cluster
            if (fitresult == 6) {
                results   = m_pfitter_split->fit(sfits);
                fitresult = results[0].fitStatus;
                for (unsigned int i = 0; i < results.size(); ++i)
                    ATH_MSG_VERBOSE("    Performing split fit with " << m_pfitter_split
                                                                     << " result return=" << results[i].fitStatus);
            }
        }

        bool precisionFitFailed = fitresult > 0 && fitresult < 20;  // splitclusterFit fail => 19
        // Default fit for phi and eta failed
        if (measphi || precisionFitFailed) {
            ICscClusterFitter::Result res;
            CscClusterStatus          oldclustatus;
            if (!measphi) {
                res          = results[0];
                oldclustatus = res.clusterStatus;
            } else {
                oldclustatus = Muon::CscStatusSimple;
            }
            results = m_pfitter_def->fit(sfits);
            if (!results.empty()) {
                res       = results[0];
                fitresult = results[0].fitStatus;
                if (msgLvl(MSG::VERBOSE)) {
                    ostringstream deflog;
                    deflog << "      Performing default fit with " << m_pfitter_def;
                    if (fitresult) {
                        deflog << " failed: return=" << fitresult;
                    } else {
                        deflog << " succeeded";
                    }
                    ATH_MSG_VERBOSE(deflog.str());
                }
                // Keep the status from the first fit if it is defined.
                if (oldclustatus != Muon::CscStatusUndefined) {
                    res.clusterStatus = oldclustatus;
                    // we want to keep oldcluster status
                    results[0] = res;
                }
            }
        }

        ////////
        //
        // Phase V. For multiple results, fill up collection
        //
        ///////////////
        unsigned int nresults = results.size();
        for (unsigned int ire = 0; ire < nresults; ++ire) {
            CscClusterStatus    clustatus      = results[ire].clusterStatus;
            Muon::CscTimeStatus timeStatus     = results[ire].timeStatus;
            double              pos            = results[ire].position;
            double              err            = results[ire].dposition;
            unsigned int        id_strip       = results[ire].strip;  // return peak strip index (unsigned integer)
            double              cluster_charge = results[ire].charge;
            double              cluster_time   = results[ire].time;
            if (clustatus == Muon::CscStatusUndefined) ATH_MSG_DEBUG("      Csc Cluster Status is not defined.");

            if (id_strip >= sfits.size()) {
                ATH_MSG_WARNING("        Fit size check failed: ");
                continue;
            }
            // Fetch the strip used to identify this cluster.
            const CscStripPrepData* pstrip_id = 0;
            if (id_strip < clusterStrips.size()) pstrip_id = clusterStrips[id_strip];
            if (!pstrip_id) {
                ATH_MSG_WARNING("        Fit ID check failed: ");
                continue;
            }

            // Create ATLAS CSC cluster.
            Identifier     cluster_id   = pstrip_id->identify();
            IdentifierHash cluster_hash = pstrip_id->collectionHash();
            int            zsec         = m_idHelperSvc->cscIdHelper().stationEta(cluster_id);
            int            wlay         = m_idHelperSvc->cscIdHelper().wireLayer(cluster_id);
            // This local position is in the muon (not tracking) coordinate system.
            // retrieve MuonDetectorManager from the conditions store
            SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey};
            const MuonGM::MuonDetectorManager*              MuonDetMgr = DetectorManagerHandle.cptr();
            if (MuonDetMgr == nullptr) {
                ATH_MSG_ERROR("Null pointer to the MuonDetectorManager conditions object");
                return 0;
            }
            const CscReadoutElement* pro = MuonDetMgr->getCscReadoutElement(cluster_id);
            // Amg::Vector3D local_pos = pro->localClusterPos(zsec, wlay, measphi, pos);
            Amg::Vector3D localTrk_pos = pro->nominalLocalClusterPos(zsec, wlay, measphi, pos);

            Amg::MatrixX* cov = new Amg::MatrixX(1, 1);
            (*cov)(0, 0)      = err * err;
            Amg::Vector2D plpos(measphi ? localTrk_pos.y() : localTrk_pos.z(),
                                measphi ? localTrk_pos.z() : localTrk_pos.y());
            if (msgLvl(MSG::DEBUG)) {

                ATH_MSG_DEBUG("        Cluster parameters: " << nresults);
                ATH_MSG_DEBUG("                ID strip: " << first_strip + id_strip << "(" << first_strip << ":"
                                                           << id_strip << ")");
                ATH_MSG_DEBUG("          local position: " << plpos.x() << " " << plpos.y()
                                                           << " error: " << Amg::toString(*cov));
                ATH_MSG_DEBUG("                  charge: " << cluster_charge);
                ATH_MSG_DEBUG("                    time: " << cluster_time);
                ATH_MSG_DEBUG("                  status: " << Muon::toString(clustatus));
            }
            unsigned int            fstrip = results[ire].fstrip;
            unsigned int            lstrip = results[ire].lstrip;
            std::vector<Identifier> prd_digit_ids_submit;
            for (unsigned int ids_index = fstrip; ids_index < lstrip + 1; ++ids_index) {
                if (ids_index >= prd_digit_ids.size()) {
                    ATH_MSG_WARNING("index out of range " << ids_index << " size " << prd_digit_ids.size());
                    continue;
                }
                prd_digit_ids_submit.push_back(prd_digit_ids[ids_index]);
            }
            unsigned int nstrip = prd_digit_ids_submit.size();
            ATH_MSG_DEBUG("                    size: " << nstrip << " " << sfits.size());
            ATH_MSG_DEBUG("   all              size: " << strips.size() << " " << allStripfits.size());


            //      allStripfits.push_back(res);

            CscPrepData* pclus = new CscPrepData(cluster_id, cluster_hash, plpos, prd_digit_ids_submit, cov, pro,
                                                 int(cluster_charge + 0.5), cluster_time, clustatus, timeStatus);
            pclus->setHashAndIndex(newCollection->identifyHash(), newCollection->size());

            newCollection->push_back(pclus);
        }
    }  // end loop over clusters

    return 0;
}

//******************************************************************************
