/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "sTgcRdoToPrepDataToolCore.h"

#include "MuonReadoutGeometry/MuonStation.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"

using namespace MuonGM;
using namespace Trk;
using namespace Muon;

//============================================================================
Muon::sTgcRdoToPrepDataToolCore::sTgcRdoToPrepDataToolCore(const std::string& t, const std::string& n, const IInterface* p) 
: base_class(t,n,p)
{
    //  template for property decalration
    declareProperty("OutputCollection", m_stgcPrepDataContainerKey = std::string("STGC_Measurements"), "Muon::sTgcPrepDataContainer to record");
    declareProperty("InputCollection", m_rdoContainerKey = std::string("sTGCRDO"), "RDO container to read");
    declareProperty("Merge",  m_merge = true);
}


//============================================================================
StatusCode Muon::sTgcRdoToPrepDataToolCore::initialize()
{  
    ATH_MSG_DEBUG(" in initialize()");
    ATH_CHECK( m_idHelperSvc.retrieve() );
    // check if the initialization of the data container is success
    ATH_CHECK(m_stgcPrepDataContainerKey.initialize());
    ATH_CHECK(m_rdoContainerKey.initialize());
    ATH_CHECK(m_muDetMgrKey.initialize());
    ATH_MSG_INFO("initialize() successful in " << name());
    return StatusCode::SUCCESS;
}


//============================================================================
StatusCode Muon::sTgcRdoToPrepDataToolCore::processCollection(Muon::sTgcPrepDataContainer* stgcPrepDataContainer, const STGC_RawDataCollection *rdoColl, std::vector<IdentifierHash>& idWithDataVect) const
{
    const IdentifierHash hash = rdoColl->identifyHash();

    ATH_MSG_DEBUG(" ***************** Start of process STGC Collection with hash Id: " << hash);

    sTgcPrepDataCollection* prdColl = nullptr;
  
    // check if the collection already exists, otherwise add it
    if ( stgcPrepDataContainer->indexFindPtr(hash) != nullptr ) {
        ATH_MSG_DEBUG("In processCollection: collection already contained in the sTGC PrepData container");
        return StatusCode::FAILURE;

    } else {
        prdColl = new sTgcPrepDataCollection(hash);
        idWithDataVect.push_back(hash);

        // set the offline identifier of the collection Id
        IdContext  context = m_idHelperSvc->stgcIdHelper().module_context();
        Identifier moduleId;
        int getId = m_idHelperSvc->stgcIdHelper().get_id(hash, moduleId, &context);
        if ( getId != 0 ) {
            ATH_MSG_ERROR("Could not convert the hash Id: " << hash << " to identifier");
        } else {
            prdColl->setIdentifier(moduleId);
        }

        if (StatusCode::SUCCESS != stgcPrepDataContainer->addCollection(prdColl, hash)) {
            ATH_MSG_DEBUG("In processCollection - Couldn't record in the Container sTGC Collection with hashID = " << (int)hash );
            return StatusCode::FAILURE;
        }

    }

    std::vector<sTgcPrepData> sTgcStripPrds;
    std::vector<sTgcPrepData> sTgcWirePrds;
    std::vector<sTgcPrepData> sTgcPadPrds;
    sTgcStripPrds.reserve(rdoColl->size());
    sTgcPadPrds.reserve(rdoColl->size());
    sTgcWirePrds.reserve(rdoColl->size());
    
    // convert the RDO collection to a PRD collection
    STGC_RawDataCollection::const_iterator it = rdoColl->begin();
  
    // MuonDetectorManager from the conditions store
    SG::ReadCondHandle<MuonGM::MuonDetectorManager> detMgrHandle{m_muDetMgrKey};
    const MuonGM::MuonDetectorManager* muonDetMgr = detMgrHandle.cptr(); 
    if(!muonDetMgr){
        ATH_MSG_ERROR("Null pointer to the read MuonDetectorManager conditions object");
        return StatusCode::FAILURE;
    }
  
    for (; it != rdoColl->end() ; ++it ) {

        ATH_MSG_DEBUG("Adding a new sTgc PrepRawData");

        const STGC_RawData* rdo = *it;
        const Identifier  rdoId = rdo->identify();

        if (!m_idHelperSvc->issTgc(rdoId)) {
            ATH_MSG_WARNING("The given Identifier "<<rdoId.get_compact()<<" ("<<m_idHelperSvc->stgcIdHelper().print_to_string(rdoId)<<") is no sTGC Identifier, continuing");
            continue;
        }

        std::vector<Identifier> rdoList;
        rdoList.push_back(rdoId);

        // get the local and global positions
        const MuonGM::sTgcReadoutElement* detEl = muonDetMgr->getsTgcReadoutElement(rdoId);
        Amg::Vector2D localPos;

        int channelType = m_idHelperSvc->stgcIdHelper().channelType(rdoId);
        if (channelType < 0 || channelType > 2) {
            ATH_MSG_ERROR("Unknown sTGC channel type");
            return StatusCode::FAILURE;
        }

        bool getLocalPos = detEl->stripPosition(rdoId, localPos);
        if ( !getLocalPos ) {
            ATH_MSG_ERROR("Could not get the local strip position for sTgc");
            return StatusCode::FAILURE;
        } 

        // get the resolution from strip width
        // to be fixed: for now do not set the resolution, it will be added in the next update    
        const int     gasGap = m_idHelperSvc->stgcIdHelper().gasGap(rdoId);
        const int    channel = m_idHelperSvc->stgcIdHelper().channel(rdoId);
        const int     charge = (int) rdo->charge();
        const int    rdoTime = (int) rdo->time();
        const uint16_t bcTag = rdo->bcTag();
        
        ATH_MSG_DEBUG("Adding a new STGC PRD, gasGap: " << gasGap << " channel: " << channel << " type: " << channelType );

        double width{0.};
        if (channelType == 0) { // Pads
            const MuonGM::MuonPadDesign* design = detEl->getPadDesign(rdoId);
            if (!design) {
                ATH_MSG_WARNING("Failed to get design for sTGC pad" );
            } else {
                width = design->channelWidth(localPos, true);
            } 
        } else { // Strips and wires
            const MuonGM::MuonChannelDesign* design = detEl->getDesign(rdoId);
            if (!design) {
                ATH_MSG_WARNING("Failed to get design for sTGC strip/wire" );
            } else {
                width = design->channelWidth(localPos);
            }
        }

        double resolution = width/sqrt(12.); 
        auto   cov = Amg::MatrixX(1,1);
        cov.setIdentity();
        (cov)(0,0) = resolution*resolution;  

        ATH_MSG_DEBUG("Adding a new STGC PRD, gasGap: " << gasGap << " channel: " << channel << " type: " << channelType << " resolution " << resolution );

        if(m_merge) {

            std::vector<sTgcPrepData>& sTgcPrds = channelType == 0 ? sTgcPadPrds : (channelType == 1 ? sTgcStripPrds : sTgcWirePrds);
        
            // check if the same RdoId is already present; keep the one with the smallest time
            auto it = std::find_if(sTgcPrds.begin(), sTgcPrds.end(), [&rdoId](auto prd) { return (prd.identify() == rdoId); });
            if (it == sTgcPrds.end()) {
                sTgcPrds.emplace_back(rdoId, hash, localPos, rdoList, cov, detEl, charge, rdoTime, bcTag);
            } else if (it->time() > rdoTime) {
                *it = sTgcPrepData(rdoId, hash, localPos, rdoList, cov, detEl, charge, rdoTime, bcTag);
            }
        } else {
            // if not merging just add the PRD to the collection
            prdColl->push_back(new sTgcPrepData(rdoId,hash,localPos,rdoList,cov,detEl,charge,rdoTime,bcTag));
        } 
    }

    if(m_merge) {
        // merge strip prds that fire closeby channels (not clusterizing wires and pads)
        std::vector<Muon::sTgcPrepData*> sTgcStripClusters;
        ATH_CHECK(m_clusterBuilderTool->getClusters(sTgcStripPrds, sTgcStripClusters)); // Clusterize strips

        for ( auto it : sTgcStripClusters ) {
            it->setHashAndIndex(prdColl->identifyHash(), prdColl->size());
            prdColl->push_back(it);
        } 
        for ( Muon::sTgcPrepData& prd : sTgcWirePrds ) {
            prd.setHashAndIndex(prdColl->identifyHash(), prdColl->size());
            prdColl->emplace_back(new sTgcPrepData(prd));
        }
        for (Muon::sTgcPrepData& prd : sTgcPadPrds ) {
            prd.setHashAndIndex(prdColl->identifyHash(), prdColl->size());
            prdColl->emplace_back(new sTgcPrepData(prd));
        }
    }

    // clear vector and delete elements
    sTgcStripPrds.clear();
    sTgcWirePrds.clear();
    sTgcPadPrds.clear();

    return StatusCode::SUCCESS;
}


//============================================================================
const STGC_RawDataContainer* Muon::sTgcRdoToPrepDataToolCore::getRdoContainer() const 
{
    auto rdoContainerHandle  = SG::makeHandle(m_rdoContainerKey);
    if(rdoContainerHandle.isValid()) {
        ATH_MSG_DEBUG("STGC_getRdoContainer success");
        return rdoContainerHandle.cptr();  
    }
    ATH_MSG_WARNING("Retrieval of STGC_RawDataContainer failed !");

    return nullptr;
}


//============================================================================
void Muon::sTgcRdoToPrepDataToolCore::processRDOContainer( Muon::sTgcPrepDataContainer* stgcPrepDataContainer, std::vector<IdentifierHash>& idWithDataVect ) const
{
    ATH_MSG_DEBUG("In processRDOContainer");
    const STGC_RawDataContainer* rdoContainer = getRdoContainer();
    if (!rdoContainer) return;
  
    // run in unseeded mode
    for (STGC_RawDataContainer::const_iterator it = rdoContainer->begin(); it != rdoContainer->end(); ++it ) {
        auto rdoColl = *it;
        if (rdoColl->empty()) continue;
        ATH_MSG_DEBUG("New RDO collection with " << rdoColl->size() << "STGC Hits");

        if(processCollection(stgcPrepDataContainer, rdoColl, idWithDataVect).isFailure()) {
            ATH_MSG_DEBUG("processCsm returns a bad StatusCode - keep going for new data collections in this event");
        }
    } 
}


// methods for ROB-based decoding
//============================================================================
StatusCode Muon::sTgcRdoToPrepDataToolCore::decode( std::vector<IdentifierHash>& idVect, std::vector<IdentifierHash>& idWithDataVect ) const
{
    ATH_MSG_DEBUG("Size of the input hash id vector: " << idVect.size());

    // clear the output vector of selected data
    idWithDataVect.clear();

    Muon::sTgcPrepDataContainer* stgcPrepDataContainer = setupSTGC_PrepDataContainer();

    if (!stgcPrepDataContainer) return StatusCode::FAILURE;

    processRDOContainer(stgcPrepDataContainer, idWithDataVect);
    return StatusCode::SUCCESS;
} 


//============================================================================
StatusCode Muon::sTgcRdoToPrepDataToolCore::decode( const std::vector<uint32_t>& robIds ) const
{
    ATH_MSG_DEBUG("Size of the robIds" << robIds.size() );
    return StatusCode::SUCCESS;
}

// printout methods
void Muon::sTgcRdoToPrepDataToolCore::printInputRdo() const { return; }
void Muon::sTgcRdoToPrepDataToolCore::printPrepData() const { return; }

