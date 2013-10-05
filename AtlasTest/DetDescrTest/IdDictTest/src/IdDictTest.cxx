/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Detector description conversion service package
 -----------------------------------------------
 Copyright (C) 2002 by ATLAS Collaboration
***************************************************************************/
//
// AL Notes: 
// StatusCode IdDictTest::initialize()
//            o Retrieve individual Mgrs from DetectorStore
//            o Set the Id helpers from the Mgrs 
//              or get the Id helpers from DetectorStore
// StatusCode IdDictTest::execute()
//            o StatusCode IdDictTest::testAtlasDetectorID(void)
//            o StatusCode IdDictTest::test_indet_ids()
//            o StatusCode IdDictTest::test_muons_ids()
//            o Statuscode IdDictTest::test_calos_ids()
// StatusCode IdDictTest::finalize()
// void IdDictTest::tab(size_t level) 
// StatusCode IdDictTest::print_ids(void)
// Identifier IdDictTest::lvl1()
// Identifier IdDictTest::lar_dm()
// Identifier IdDictTest::tile_dm()
// Identifier IdDictTest::lar_dm()
// Identifier IdDictTest::mdt()
// Identifier IdDictTest::lar_dm()
// Identifier IdDictTest::csc()
// Identifier IdDictTest::rpc()
// Identifier IdDictTest::tgc()

//
//


//<doc><file>   $Id: IdDictTest.cxx,v 1.52 2009-05-19 16:19:25 lucotte Exp $
//<version>     $Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

// Id-related includes
#include "IdDictTest/IdDictTest.h"
#include "Identifier/Range.h" 
#include "Identifier/IdentifierHash.h"
#include "IdDictDetDescr/IdDictManager.h"

// Tests
#include "TestInDetIds.h"
#include "TestCaloIds.h"
#include "TestLArIds.h"
#include "TestMuonIds.h"

// DetDescr includes
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetIdentifier/SiliconID.h"
#include "LArIdentifier/LArIdManager.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloDM_ID.h"
#include "CaloIdentifier/CaloLVL1_ID.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArMiniFCAL_ID.h"
#include "LArIdentifier/LArOnlineID.h"
#include "MuonIdHelpers/MuonIdHelper.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"

// Athena-related includes
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IChronoStatSvc.h"

#include <set>

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

IdDictTest::IdDictTest(const std::string& name, ISvcLocator* pSvcLocator) 
        :
        Algorithm(name, pSvcLocator),
        m_printFlag(true),
        m_atlasFlag(true),
        m_indetFlag(true),
        m_pixelFlag(true),
        m_sctFlag(true),
        m_trtFlag(true),
        m_calosFlag(true),
        m_caloTTFlag(true),
        m_emBarrelFlag(true),
        m_emEndcapFlag(true),
        m_hecFlag(true),
        m_fcalFlag(true),
        m_miniFcalFlag(true),
        m_laronlineFlag(true),
        m_tileFlag(true),
        m_muonsFlag(true),
        m_cscFlag(true),
        m_mdtFlag(true),
        m_rpcFlag(true),
        m_tgcFlag(true),
        m_idDictMgr(0),
        m_siliconID(0),
        m_larIdMgr(0),
        m_caloIdMgr(0),
        m_caloCellHelper(0),
        m_caloDMHelper(0),
        m_lvl1Helper(0),
        m_emHelper(0),
        m_hecHelper(0),
        m_miniFcalHelper(0),
        m_laronlineHelper(0),
        m_muon_dict(0),
        m_calo_dict(0),
        m_tested_ids(false),
        m_detStore(0)

{
    // Initialize properties
    declareProperty("print_ids",        m_printFlag);
    declareProperty("atlas",            m_atlasFlag);
    declareProperty("indet",            m_indetFlag);
    declareProperty("pixel",            m_pixelFlag);
    declareProperty("sct",              m_sctFlag);
    declareProperty("trt",              m_trtFlag);
    declareProperty("calos",            m_calosFlag);
    declareProperty("caloTT",           m_caloTTFlag);
    declareProperty("emBarrel",         m_emBarrelFlag);
    declareProperty("emEndcap",         m_emEndcapFlag);
    declareProperty("hec",              m_hecFlag);
    declareProperty("fcal",             m_fcalFlag);
    declareProperty("miniFcal",         m_miniFcalFlag);
    declareProperty("LArOnline",        m_laronlineFlag);
    declareProperty("tile",             m_tileFlag);
    declareProperty("muons",            m_muonsFlag);
    declareProperty("csc",              m_cscFlag);
    declareProperty("mdt",              m_mdtFlag);
    declareProperty("rpc",              m_rpcFlag);
    declareProperty("tgc",              m_tgcFlag);
}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////

IdDictTest::~IdDictTest()
{  }

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
// Note that it is NOT NECESSARY to run the initialize of individual
// sub-algorithms.  The framework takes care of it.
/////////////////////////////////////////////////////////////////////

StatusCode IdDictTest::initialize()
{
    MsgStream  log(msgSvc(),name());
    StatusCode sc = StatusCode::FAILURE;
    log << MSG::INFO 
        << "Start initialization" 
        << endreq;

    log << MSG::DEBUG << "Properties: " << endreq;
    
    const std::vector<Property*> properties = getProperties();
    std::vector<Property*>::const_iterator first = properties.begin();
    std::vector<Property*>::const_iterator last  = properties.end();
    for (; first != last; ++first) {
        BooleanProperty* bprop = dynamic_cast<BooleanProperty*>(*first);

        if (bprop) {
            log << MSG::DEBUG << (*first)->name() << " " << (bprop)->value();
        }
        log << MSG::DEBUG << endreq;
    }

    // get DetectorStore service
    sc = service("DetectorStore", m_detStore);
    if (sc.isFailure()) {
        log << MSG::ERROR << "DetectorStore service not found !" << endreq;
        return StatusCode::FAILURE;
    } else {
        log << MSG::INFO << "Found DetectorStore service" << endreq;
    }

    // Get IdHelpers for checks that they are the same
    if (m_indetFlag) {
        sc = m_detStore->retrieve(m_pixIDHelper, "PixelID"); 
        if (sc.isFailure()) {
            log << MSG::ERROR << "Could not get PixelID from DetectorStore !" << endreq;
            return StatusCode::FAILURE;
        } 
        sc = m_detStore->retrieve(m_sctIDHelper, "SCT_ID");
        if (sc.isFailure()) {
            log << MSG::ERROR << "Could not get SCT_ID from DetectorStore !" << endreq;
            return StatusCode::FAILURE;
        } 
        sc = m_detStore->retrieve(m_trtIDHelper, "TRT_ID");
        if (sc.isFailure()) {
            log << MSG::ERROR << "Could not get TRT_ID from DetectorStore !" << endreq;
            return StatusCode::FAILURE;
        } 
        log << MSG::DEBUG << "Retrieved indet id helpers " << endreq;
    }


    if (m_indetFlag) {
        // Get SiliconID helper
        sc = m_detStore->retrieve(m_siliconID,"SiliconID"); 
        if (sc.isFailure()) {
            log << MSG::ERROR << "Could not get SiliconID helper !" << endreq;
            return StatusCode::FAILURE;
        }
        else {
            log << MSG::DEBUG << " Found the SiliconID helper. " << endreq;
        } 
    }
    
    // Get the dictionary manager from the detector store
    sc = m_detStore->retrieve(m_idDictMgr, "IdDict");
    if (sc.isFailure()) {
        log << MSG::ERROR << "Could not get IdDictManager !" << endreq;
        return StatusCode::FAILURE;
    } 
    else {
        log << MSG::DEBUG << " Found the IdDictManager. " << endreq;
        log << MSG::DEBUG << " Tag is "   << m_idDictMgr->manager()->tag() << endreq;
        log << MSG::DEBUG << " DoChecks " << m_idDictMgr->manager()->do_checks() << endreq;
    }

    if(m_laronlineFlag) {
        // Set id helpers for LArOnline
        // Get helper from DetectorStore
        const DataHandle<LArOnlineID> laronlineId;
        sc = m_detStore->retrieve(laronlineId, "LArOnlineID");
        if (sc.isFailure()) {
            log << MSG::ERROR << "Could not get LArOnlineID helper !" << endreq;
            return StatusCode::FAILURE;
        } 
        else {
            m_laronlineHelper=laronlineId;
            log << MSG::DEBUG << " Found LArOnline Helper " << endreq; 
        }
    }


    //
    // Set the id helpers from the managers
    //
    if(m_calosFlag) {// Set id helpers for Calos 
        // Get helpers from DetectorStore 
        const DataHandle<CaloLVL1_ID> caloId;
        sc = m_detStore->retrieve(caloId, "CaloLVL1_ID");
        if (sc.isFailure()) {
            log << MSG::ERROR << "Could not get CaloLVL1_ID helper !" << endreq;
            return StatusCode::FAILURE;
        } 
        else {
            m_lvl1Helper=caloId;
            log << MSG::DEBUG << " Found the CaloLVL1_ID helper. " << endreq;
        }

        const DataHandle<CaloDM_ID> dmId;
        sc = m_detStore->retrieve(dmId, "CaloDM_ID");
        if (sc.isFailure()) {
            log << MSG::ERROR << "Could not get CaloDM_ID helper !" << endreq;
            return StatusCode::FAILURE;
        } 
        else {
            m_caloDMHelper = dmId;
            log << MSG::DEBUG << " Found the CaloDM_ID helper. " << endreq;
        }
            
        const DataHandle<CaloCell_ID> cellId;
        sc = m_detStore->retrieve(cellId, "CaloCell_ID");
        if (sc.isFailure()) {
            log << MSG::ERROR << "Could not get CaloCell_ID helper !" << endreq;
            return StatusCode::FAILURE;
        } 
        else {
            m_caloCellHelper = cellId;
            log << MSG::DEBUG << " Found the CaloCell_ID helper. " << endreq;
        }
            
        const DataHandle<LArEM_ID> em_id;
        sc = m_detStore->retrieve(em_id, "LArEM_ID");
        if (sc.isFailure()) {
            log << MSG::ERROR << "Could not get LArEM_ID helper !" << endreq;
            return StatusCode::FAILURE;
        } 
        else {
            m_emHelper = em_id;
            log << MSG::DEBUG << " Found the LArEM_ID helper. " << endreq;
        }
            
        const DataHandle<LArHEC_ID> hec_id;
        sc = m_detStore->retrieve(hec_id, "LArHEC_ID");
        if (sc.isFailure()) {
            log << MSG::ERROR << "Could not get LArHEC_ID helper !" << endreq;
            return StatusCode::FAILURE;
        } 
        else {
            m_hecHelper = hec_id;
            log << MSG::DEBUG << " Found the LArHEC_ID helper. " << endreq;
        }
            
        const DataHandle<LArFCAL_ID> fcal_id;
        sc = m_detStore->retrieve(fcal_id, "LArFCAL_ID");
        if (sc.isFailure()) {
            log << MSG::ERROR << "Could not get LArFCAL_ID helper !" << endreq;
            return StatusCode::FAILURE;
        } 
        else {
            m_fcalHelper=fcal_id;
            log << MSG::DEBUG << " Found the LArFCAL_ID helper. " << endreq;
        }

        const DataHandle<LArMiniFCAL_ID> miniFcal_id;
        sc = m_detStore->retrieve(miniFcal_id, "LArMiniFCAL_ID");
        if (sc.isFailure()) {
            log << MSG::ERROR << "Could not get LArMiniFCAL_ID helper !" << endreq;
            return StatusCode::FAILURE;
        } 
        else {
            m_miniFcalHelper = miniFcal_id;
            log << MSG::DEBUG << " Found the LArMiniFCAL_ID helper. " << endreq;
        }
    }

    // Now check that the helpers are the same from CaloIdManager
    const CaloIdManager* caloIdMgr = CaloIdManager::instance();
    if (m_caloCellHelper != caloIdMgr->getCaloCell_ID()) {
        log << MSG::ERROR << "CaloCell_ID not the same from mgr: " << m_caloCellHelper 
            << " " << caloIdMgr->getCaloCell_ID() << endreq;
        return StatusCode::FAILURE;
    }
    if (m_emHelper != caloIdMgr->getEM_ID()) {
        log << MSG::ERROR << "LArEM_ID not the same from mgr: " << m_emHelper
            << " " << caloIdMgr->getEM_ID() << endreq;
        return StatusCode::FAILURE;
    }
    if (m_hecHelper != caloIdMgr->getHEC_ID()) {
        log << MSG::ERROR << "LArHEC_ID not the same from mgr: " << m_hecHelper
            << " " << caloIdMgr->getHEC_ID() << endreq;
        return StatusCode::FAILURE;
    }
    if (m_fcalHelper != caloIdMgr->getFCAL_ID()) {
        log << MSG::ERROR << "LArFCAL_ID not the same from mgr: " << m_fcalHelper
            << " " << caloIdMgr->getFCAL_ID() << endreq;
        return StatusCode::FAILURE;
    }
    if (m_miniFcalHelper != caloIdMgr->getMiniFCAL_ID()) {
        log << MSG::ERROR << "LArMiniFCAL_ID not the same from mgr: " << m_miniFcalHelper
            << " " << caloIdMgr->getMiniFCAL_ID() << endreq;
        return StatusCode::FAILURE;
    }
    log << MSG::DEBUG << "Found helpers from CaloIdManager - test OK. " << endreq;

    if(m_muonsFlag) {
        log << MSG::DEBUG << " Load muon id helpers " << endreq;
        const MdtIdHelper*  mdt_id   = 0;
        sc = m_detStore->retrieve(mdt_id, "MDTIDHELPER"); 
        if (sc.isFailure()) {
            log << MSG::FATAL << "Could not get MdtIdHelper from DetectorStore !" << endreq;
            return StatusCode::FAILURE;
        } 
        const CscIdHelper*  csc_id   = 0;
        sc = m_detStore->retrieve(csc_id, "CSCIDHELPER"); 
        if (sc.isFailure()) {
            log << MSG::FATAL << "Could not get CscIdHelper from DetectorStore !" << endreq;
            return StatusCode::FAILURE;
        } 
        const RpcIdHelper*  rpc_id   = 0;
        sc = m_detStore->retrieve(rpc_id, "RPCIDHELPER"); 
        if (sc.isFailure()) {
            log << MSG::FATAL << "Could not get RpcIdHelper from DetectorStore !" << endreq;
            return StatusCode::FAILURE;
        } 
        const TgcIdHelper*  tgc_id   = 0;
        sc = m_detStore->retrieve(tgc_id, "TGCIDHELPER"); 
        if (sc.isFailure()) {
            log << MSG::FATAL << "Could not get TgcIdHelper from DetectorStore !" << endreq;
            return StatusCode::FAILURE;
        } 
    }

    // Get the calorimeter dictionary for special tests 
    m_calo_dict = m_idDictMgr->manager()->find_dictionary ("Calorimeter"); 
    if(!m_calo_dict) {
        log << MSG::ERROR << "Cannot access Calorimeter dictionary " << endreq;
        return StatusCode::FAILURE;
    }

    // Get the muon dictionary for special tests (see mdt())
    m_muon_dict = m_idDictMgr->manager()->find_dictionary ("MuonSpectrometer"); 
    if(!m_muon_dict) {
        log << MSG::ERROR << "Cannot access MuonSpectrometer dictionary " << endreq;
        return StatusCode::FAILURE;
    }


    log << MSG::INFO
        << "Initialization completed successfully"
        << endreq;

    return StatusCode::SUCCESS;

}


/////////////////////////////////////////////////////////////////////
// EXECUTE 
/////////////////////////////////////////////////////////////////////
StatusCode IdDictTest::execute()
/*===============================*/
{
    MsgStream  log(msgSvc(),name());
    log << MSG::INFO 
        << "Entering execute" 
        << endreq;

    if(!m_tested_ids) {
        if (m_printFlag) {
            if(StatusCode::SUCCESS != print_ids()) {
                return StatusCode::FAILURE;
            }    
        }
        m_tested_ids = true;

        if (m_atlasFlag && m_indetFlag) {
            if(StatusCode::SUCCESS != testAtlasDetectorID()) {
                return StatusCode::FAILURE;
            }    
        }
        if (m_indetFlag) {
            if(StatusCode::SUCCESS != test_indet_ids ()) {
                return StatusCode::FAILURE;
            }
        }
        if (m_laronlineFlag) {
            log << MSG::DEBUG << " /////// ==> calling test_laronline_ids " << endreq;   
            if(StatusCode::SUCCESS != test_laronline_ids ()) {
                return StatusCode::FAILURE;
            }
        }
        if (m_calosFlag) {

            if(StatusCode::SUCCESS != test_calos_ids ()) {
                return StatusCode::FAILURE;
            }
        }
        if (m_muonsFlag) {
            if(StatusCode::SUCCESS != test_muons_ids ()) {
                return StatusCode::FAILURE;
            }
        }

        log << MSG::INFO 
            << "Finished dictionary tests - ALL TESTS OK!!" 
            << endreq;
    }
    
    log << MSG::INFO 
        << "Leaving execute" 
        << endreq;
    return StatusCode::SUCCESS;
}


/////////////////////////////////////////////////////////////////////
// FINALIZE:
/////////////////////////////////////////////////////////////////////

StatusCode IdDictTest::finalize()
/*==============================*/
{

    MsgStream  log(msgSvc(),name());
    log << MSG::DEBUG 
        << "finalize completed successfully" 
        << endreq;

    return StatusCode::SUCCESS;

}


/////////////////////////////////////////////////////////////////////
// 
/////////////////////////////////////////////////////////////////////

void IdDictTest::tab(size_t level) const
{ 
    MsgStream  log(msgSvc(),name());
    for (size_t i = 0; i < level; ++i) log << " "; 
} 


StatusCode IdDictTest::print_ids(void) 
{  
    // In DEBUG mode, print out the size requirements of the compact ids   
    MsgStream  log(msgSvc(),name());

    const IdDictMgr::dictionary_map& dm = m_idDictMgr->manager()->get_dictionary_map (); 
    IdDictMgr::dictionary_map::const_iterator it;  
 
    int n = 0; 
 
    for (it = dm.begin (); it != dm.end (); ++it, ++n) { 
        const IdDictDictionary& dictionary = *((*it).second); 
 
 
        log << MSG::DEBUG 
            << "---- " << n << " ----------------------------" 
            << endreq;
        log << MSG::DEBUG 
            << "Dictionary " << dictionary.m_name 
            << endreq;

        if(dictionary.verify()) {
            log << MSG::DEBUG
                << "Dictionary verification is OK"
                << endreq;
        }
        else {
            log << MSG::FATAL
                << "Dictionary verification has failed: "
                << dictionary.m_name << " multirange: "
                << (std::string)dictionary.build_multirange()
                << endreq;
            return StatusCode::FAILURE;
        }
        
 
        std::vector<IdDictRegion*>::const_iterator rit; 
        for (rit = dictionary.m_regions.begin (); rit != dictionary.m_regions.end (); ++rit) { 
            const IdDictRegion& region = *(*rit); 
 
            log << MSG::DEBUG 
                << "region #" << region.m_index 
                << " group "  << region.m_group 
                << " name "   << region.m_name 
                << endreq;
 
            std::vector <IdDictFieldImplementation>::const_iterator fit; 
           
            size_t width = 0; 
 
            for (fit = region.m_implementation.begin ();  
                 fit != region.m_implementation.end (); 
                 ++fit) {
                const IdDictFieldImplementation& impl = *fit; 
 
                size_t w = impl.range()->m_field->m_name.size (); 
 
                if (w > width) width = w; 
            } 
 
            int bits = 0; 
 
            for (fit = region.m_implementation.begin ();  
                 fit != region.m_implementation.end (); 
                 ++fit) { 
                const IdDictFieldImplementation& impl = *fit; 
 
                size_t w = impl.range()->m_field->m_name.size (); 
 
                log << MSG::DEBUG 
                    << "  implement field #" << impl.range()->m_field->m_index <<  
                    " " << impl.range()->m_field->m_name; 
 
                tab (width - w); 
 
//              int index = impl.field();
//              std::string label = impl.range()->m_field->labels[index]->m_name;
//              std::cout << " name " << label << " ";

                log << " -> " << (std::string) impl.field() 
                    <<  "/" << (std::string) impl.ored_field() 
                    <<  " (" << impl.bits() << " bits)" 
                    << endreq;
 
                bits += impl.bits(); 
            } 
 
            Range range = region.build_range (); 
 
            log << MSG::DEBUG 
                << " -> " << (std::string) range <<  
                " (cardinality=" << range.cardinality () << ")" << 
                " (" << bits << " bits)" 
                << endreq;
        } 
    } 
 
    return StatusCode::SUCCESS;
}  

//////////////////////////////////////////////////////
StatusCode IdDictTest::testAtlasDetectorID(void) const
{
    StatusCode sc = StatusCode::SUCCESS;
    MsgStream  log(msgSvc(),name());

    log << MSG::INFO 
        << "Testing AtlasDetectorID ----> " 
        << endreq;

    Identifier id;
    if (id.is_valid()) {
        log << MSG::ERROR << "Failed is_valid check - empty Identifier is valid " << endreq;
        sc = StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO << "Check of Identifier::is_valid OK " << endreq;
    }
    

    //const PixelID* pixel_id  = m_pixMgr->getIdHelper();
    const PixelID* pixel_id;
    if (m_detStore->retrieve(pixel_id, "PixelID").isFailure()) {
        log << MSG::ERROR << "Could not get Pixel ID helper" << endreq;
        return StatusCode::FAILURE;
    }    
    id = pixel_id->indet();
    if (!id.is_valid()) {
        log << MSG::ERROR << "Failed is_valid check - InDet Identifier is NOT valid " << endreq;
        sc = StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO << "Check of Identifier::is_valid OK " << endreq;
    }



    // Check going to/from string
    std::string strId = id.getString();
    Identifier idStr;
    idStr.set(strId);
    if (id != idStr) {
        log << MSG::ERROR << "Failed to convert to string and back - id, string, idStr " 
            << pixel_id->show_to_string(id) << " " << strId << " " << pixel_id->show_to_string(idStr) << endreq;
        sc = StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO << "Check of Identifier::is_valid OK " << endreq;
    }


    
    IdContext detContext = pixel_id->detsystem_context();
    if (!pixel_id->is_indet(id)) {
        pixel_id->show(id);
        pixel_id->show(id, &detContext, '/');
        log << MSG::ERROR << "Failed is_indet check " << endreq;
        sc = StatusCode::FAILURE;
    }
    else if (pixel_id->is_lar          (id) ||
             pixel_id->is_tile         (id) ||
             pixel_id->is_muon         (id) ||
             pixel_id->is_calo         (id) ||
             pixel_id->is_lvl1_trig_towers(id) ||
// is_pixel also satisfies "is_indet" when compact is unpacked.
//           pixel_id->is_pixel        (id) ||   
             pixel_id->is_sct          (id) ||
             pixel_id->is_trt          (id) ||
             pixel_id->is_lar_em       (id) ||
             pixel_id->is_lar_hec      (id) ||
             pixel_id->is_lar_fcal     (id) ||
             pixel_id->is_lar_minifcal (id) ||
             pixel_id->is_lar_dm       (id) ||
             pixel_id->is_tile_dm      (id) ||
             pixel_id->is_mdt          (id) ||
             pixel_id->is_csc          (id) ||
             pixel_id->is_tgc          (id) ||
             pixel_id->is_rpc          (id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_<other> indet check " << endreq;
        log << MSG::INFO << "is_lar      "     <<  pixel_id->is_lar          (id) << endreq;
        log << MSG::INFO << "is_tile     "     <<  pixel_id->is_tile         (id) << endreq;
        log << MSG::INFO << "is_muon     "     <<  pixel_id->is_muon         (id) << endreq;
        log << MSG::INFO << "is_calo     "     <<  pixel_id->is_calo         (id) << endreq;
        log << MSG::INFO << "is_lvl1     "     <<  pixel_id->is_lvl1_trig_towers(id) << endreq;
        log << MSG::INFO << "is_sct      "     <<  pixel_id->is_sct          (id) << endreq;
        log << MSG::INFO << "is_trt      "     <<  pixel_id->is_trt          (id) << endreq;
        log << MSG::INFO << "is_lar_em   "     <<  pixel_id->is_lar_em       (id) << endreq;
        log << MSG::INFO << "is_lar_hec  "     <<  pixel_id->is_lar_hec      (id) << endreq;
        log << MSG::INFO << "is_lar_fcal "     <<  pixel_id->is_lar_fcal     (id) << endreq;
        log << MSG::INFO << "is_lar_minifcal " <<  pixel_id->is_lar_minifcal (id) << endreq;
        log << MSG::INFO << "is_lar_dm   "     <<  pixel_id->is_lar_dm       (id) << endreq;
        log << MSG::INFO << "is_tile_dm  "     <<  pixel_id->is_tile_dm      (id) << endreq;
        log << MSG::INFO << "is_mdt      "     <<  pixel_id->is_mdt          (id) << endreq;
        log << MSG::INFO << "is_csc      "     <<  pixel_id->is_csc          (id) << endreq;
        log << MSG::INFO << "is_tgc      "     <<  pixel_id->is_tgc          (id) << endreq;
        log << MSG::INFO << "is_rpc      "     <<  pixel_id->is_rpc          (id) << endreq;
        sc = StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO << "Check of indet OK " << endreq;
    }
    

    id = pixel_id->lar();
    if (!pixel_id->is_lar(id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_lar check " << endreq;
        sc = StatusCode::FAILURE;
    }
    else if (pixel_id->is_indet(id) ||
             pixel_id->is_tile         (id) ||
             pixel_id->is_muon         (id) ||
             pixel_id->is_calo         (id) ||
             pixel_id->is_lvl1_trig_towers(id) ||
             pixel_id->is_pixel        (id) ||
             pixel_id->is_sct          (id) ||
             pixel_id->is_trt          (id) ||
             //      pixel_id->is_lar_em       (id) ||
             //      pixel_id->is_lar_hec      (id) ||
             //      pixel_id->is_lar_fcal     (id) ||
             pixel_id->is_lar_dm       (id) ||
             pixel_id->is_tile_dm      (id) ||
             pixel_id->is_mdt          (id) ||
             pixel_id->is_csc          (id) ||
             pixel_id->is_tgc          (id) ||
             pixel_id->is_rpc          (id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_<other> lar check " << endreq;
        log << MSG::INFO << "is_indet    "     <<  pixel_id->is_indet        (id) << endreq;
        log << MSG::INFO << "is_tile     "     <<  pixel_id->is_tile         (id) << endreq;
        log << MSG::INFO << "is_muon     "     <<  pixel_id->is_muon         (id) << endreq;
        log << MSG::INFO << "is_calo     "     <<  pixel_id->is_calo         (id) << endreq;
        log << MSG::INFO << "is_lvl1     "     <<  pixel_id->is_lvl1_trig_towers(id) << endreq;
        log << MSG::INFO << "is_sct      "     <<  pixel_id->is_sct          (id) << endreq;
        log << MSG::INFO << "is_trt      "     <<  pixel_id->is_trt          (id) << endreq;
        log << MSG::INFO << "is_lar_hec  "     <<  pixel_id->is_lar_hec      (id) << endreq;
        log << MSG::INFO << "is_lar_fcal "     <<  pixel_id->is_lar_fcal     (id) << endreq;
        log << MSG::INFO << "is_lar_minifcal " <<  pixel_id->is_lar_minifcal (id) << endreq;
        log << MSG::INFO << "is_lar_dm   "     <<  pixel_id->is_lar_dm       (id) << endreq;
        log << MSG::INFO << "is_tile_dm  "     <<  pixel_id->is_tile_dm      (id) << endreq;
        log << MSG::INFO << "is_mdt      "     <<  pixel_id->is_mdt          (id) << endreq;
        log << MSG::INFO << "is_csc      "     <<  pixel_id->is_csc          (id) << endreq;
        log << MSG::INFO << "is_tgc      "     <<  pixel_id->is_tgc          (id) << endreq;
        log << MSG::INFO << "is_rpc      "     <<  pixel_id->is_rpc          (id) << endreq;
        sc = StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO << "Check of lar OK " << endreq;
    }

    id = pixel_id->tile();
    if (!pixel_id->is_tile(id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_tile check " << endreq;
        sc = StatusCode::FAILURE;
    }
    else if (pixel_id->is_indet(id) ||
             pixel_id->is_lar          (id) ||
             pixel_id->is_muon         (id) ||
             pixel_id->is_calo         (id) ||
             pixel_id->is_lvl1_trig_towers(id) ||
             pixel_id->is_pixel        (id) ||
             pixel_id->is_sct          (id) ||
             pixel_id->is_trt          (id) ||
             pixel_id->is_lar_em       (id) ||
             pixel_id->is_lar_hec      (id) ||
             pixel_id->is_lar_fcal     (id) ||
             pixel_id->is_lar_minifcal (id) ||
             pixel_id->is_lar_dm       (id) ||
             pixel_id->is_tile_dm      (id) ||
             pixel_id->is_mdt          (id) ||
             pixel_id->is_csc          (id) ||
             pixel_id->is_tgc          (id) ||
             pixel_id->is_rpc          (id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_<other> tile check " << endreq;
        log << MSG::INFO << "is_indet    "     <<  pixel_id->is_indet        (id) << endreq;
        log << MSG::INFO << "is_lar      "     <<  pixel_id->is_lar          (id) << endreq;
        log << MSG::INFO << "is_tile     "     <<  pixel_id->is_tile         (id) << endreq;
        log << MSG::INFO << "is_muon     "     <<  pixel_id->is_muon         (id) << endreq;
        log << MSG::INFO << "is_calo     "     <<  pixel_id->is_calo         (id) << endreq;
        log << MSG::INFO << "is_lvl1     "     <<  pixel_id->is_lvl1_trig_towers(id) << endreq;
        log << MSG::INFO << "is_sct      "     <<  pixel_id->is_sct          (id) << endreq;
        log << MSG::INFO << "is_trt      "     <<  pixel_id->is_trt          (id) << endreq;
        log << MSG::INFO << "is_lar_em   "     <<  pixel_id->is_lar_em       (id) << endreq;
        log << MSG::INFO << "is_lar_hec  "     <<  pixel_id->is_lar_hec      (id) << endreq;
        log << MSG::INFO << "is_lar_fcal "     <<  pixel_id->is_lar_fcal     (id) << endreq;
        log << MSG::INFO << "is_lar_minifcal " <<  pixel_id->is_lar_minifcal (id) << endreq;
        log << MSG::INFO << "is_lar_dm   "     <<  pixel_id->is_lar_dm       (id) << endreq;
        log << MSG::INFO << "is_tile_dm  "     <<  pixel_id->is_tile_dm      (id) << endreq;
        log << MSG::INFO << "is_mdt      "     <<  pixel_id->is_mdt          (id) << endreq;
        log << MSG::INFO << "is_csc      "     <<  pixel_id->is_csc          (id) << endreq;
        log << MSG::INFO << "is_tgc      "     <<  pixel_id->is_tgc          (id) << endreq;
        log << MSG::INFO << "is_rpc      "     <<  pixel_id->is_rpc          (id) << endreq;
        sc = StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO << "Check of tile OK " << endreq;
    }

    id = pixel_id->muon();
    if (!pixel_id->is_muon(id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_muon check " << endreq;
        sc = StatusCode::FAILURE;
    }
    else if (pixel_id->is_indet(id) ||
             pixel_id->is_lar          (id) ||
             pixel_id->is_tile         (id) ||
             pixel_id->is_calo         (id) ||
             pixel_id->is_lvl1_trig_towers(id) ||
             pixel_id->is_pixel        (id) ||
             pixel_id->is_sct          (id) ||
             pixel_id->is_trt          (id) ||
             pixel_id->is_lar_em       (id) ||
             pixel_id->is_lar_hec      (id) ||
             pixel_id->is_lar_fcal     (id) ||
             pixel_id->is_lar_minifcal (id) ||
             pixel_id->is_lar_dm       (id) ||
             pixel_id->is_tile_dm      (id) ||
//           pixel_id->is_mdt          (id) ||
             pixel_id->is_csc          (id) ||
             pixel_id->is_tgc          (id) ||
             pixel_id->is_rpc          (id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_<other> muon check " << endreq;
        sc = StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO << "Check of muon OK " << endreq;
    }

    id = pixel_id->calo();
    if (!pixel_id->is_calo(id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_calo check " << endreq;
        sc = StatusCode::FAILURE;
    }
    else if (pixel_id->is_indet        (id) ||
             pixel_id->is_lar          (id) ||
             pixel_id->is_tile         (id) ||
             pixel_id->is_muon         (id) ||
             pixel_id->is_pixel        (id) ||
             pixel_id->is_sct          (id) ||
             pixel_id->is_trt          (id) ||
             pixel_id->is_lar_em       (id) ||
             pixel_id->is_lar_hec      (id) ||
             pixel_id->is_lar_fcal     (id) ||
             pixel_id->is_lar_minifcal (id) ||
             pixel_id->is_lvl1_trig_towers(id) ||
             pixel_id->is_lar_dm       (id) ||
             // pixel_id->is_tile_dm      (id) || - calo id DOES satisfy is_tile_dm
             pixel_id->is_mdt          (id) ||
             pixel_id->is_csc          (id) ||
             pixel_id->is_tgc          (id) ||
             pixel_id->is_rpc          (id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_<other> calo check " << endreq;
        log << MSG::INFO << "is_indet    "     <<  pixel_id->is_indet        (id) << endreq;
        log << MSG::INFO << "is_lar      "     <<  pixel_id->is_lar          (id) << endreq;
        log << MSG::INFO << "is_tile     "     <<  pixel_id->is_tile         (id) << endreq;
        log << MSG::INFO << "is_muon     "     <<  pixel_id->is_muon         (id) << endreq;
        log << MSG::INFO << "is_sct      "     <<  pixel_id->is_sct          (id) << endreq;
        log << MSG::INFO << "is_trt      "     <<  pixel_id->is_trt          (id) << endreq;
        log << MSG::INFO << "is_lar_em   "     <<  pixel_id->is_lar_em       (id) << endreq;
        log << MSG::INFO << "is_lar_hec  "     <<  pixel_id->is_lar_hec      (id) << endreq;
        log << MSG::INFO << "is_lar_fcal "     <<  pixel_id->is_lar_fcal     (id) << endreq;
        log << MSG::INFO << "is_lar_minifcal " <<  pixel_id->is_lar_minifcal (id) << endreq;
        log << MSG::INFO << "is_lvl1     "     <<  pixel_id->is_lvl1_trig_towers(id) << endreq;
        log << MSG::INFO << "is_lar_dm   "     <<  pixel_id->is_lar_dm       (id) << endreq;
        log << MSG::INFO << "is_tile_dm  "     <<  pixel_id->is_tile_dm      (id) << endreq;
        log << MSG::INFO << "is_mdt      "     <<  pixel_id->is_mdt          (id) << endreq;
        log << MSG::INFO << "is_csc      "     <<  pixel_id->is_csc          (id) << endreq;
        log << MSG::INFO << "is_tgc      "     <<  pixel_id->is_tgc          (id) << endreq;
        log << MSG::INFO << "is_rpc      "     <<  pixel_id->is_rpc          (id) << endreq;
        sc = StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO << "Check of calo OK " << endreq;
    }

    id = pixel_id->pixel();
    if (!pixel_id->is_pixel(id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_pixel check " << endreq;
        sc = StatusCode::FAILURE;
    }
    else if (!pixel_id->is_indet(id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_indet check for pixel " << endreq;
        sc = StatusCode::FAILURE;
    }
    else if (pixel_id->is_lar          (id) ||
             pixel_id->is_tile         (id) ||
             pixel_id->is_muon         (id) ||
             pixel_id->is_calo         (id) ||
             pixel_id->is_lvl1_trig_towers(id) ||
             pixel_id->is_sct          (id) ||
             pixel_id->is_trt          (id) ||
             pixel_id->is_lar_em       (id) ||
             pixel_id->is_lar_hec      (id) ||
             pixel_id->is_lar_fcal     (id) ||
             pixel_id->is_lar_minifcal (id) ||
             pixel_id->is_lar_dm       (id) ||
             pixel_id->is_tile_dm      (id) ||
             pixel_id->is_mdt          (id) ||
             pixel_id->is_csc          (id) ||
             pixel_id->is_tgc          (id) ||
             pixel_id->is_rpc          (id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_<other> pixel check " << endreq;
        sc = StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO << "Check of pixel OK " << endreq;
    }

    id = pixel_id->sct();
    if (!pixel_id->is_sct(id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_sct check " << endreq;
        sc = StatusCode::FAILURE;
    }
    else if (!pixel_id->is_indet(id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_indet check for sct " << endreq;
        sc = StatusCode::FAILURE;
    }
    else if (pixel_id->is_lar          (id) ||
             pixel_id->is_tile         (id) ||
             pixel_id->is_muon         (id) ||
             pixel_id->is_calo         (id) ||
             pixel_id->is_lvl1_trig_towers(id) ||
             pixel_id->is_pixel        (id) ||
             pixel_id->is_trt          (id) ||
             pixel_id->is_lar_em       (id) ||
             pixel_id->is_lar_hec      (id) ||
             pixel_id->is_lar_fcal     (id) ||
             pixel_id->is_lar_minifcal (id) ||
             pixel_id->is_lar_dm       (id) ||
             pixel_id->is_tile_dm      (id) ||
             pixel_id->is_mdt          (id) ||
             pixel_id->is_csc          (id) ||
             pixel_id->is_tgc          (id) ||
             pixel_id->is_rpc          (id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_<other> sct check " << endreq;
        sc = StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO << "Check of sct OK " << endreq;
    }

    id = pixel_id->trt();
    if (!pixel_id->is_trt(id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_trt check " << endreq;
        sc = StatusCode::FAILURE;
    }
    else if (!pixel_id->is_indet(id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_indet check for trt " << endreq;
        sc = StatusCode::FAILURE;
    }
    else if (pixel_id->is_lar          (id) ||
             pixel_id->is_tile         (id) ||
             pixel_id->is_muon         (id) ||
             pixel_id->is_calo         (id) ||
             pixel_id->is_lvl1_trig_towers(id) ||
             pixel_id->is_pixel        (id) ||
             pixel_id->is_sct          (id) ||
             pixel_id->is_lar_em       (id) ||
             pixel_id->is_lar_hec      (id) ||
             pixel_id->is_lar_fcal     (id) ||
             pixel_id->is_lar_minifcal (id) ||
             pixel_id->is_lar_dm       (id) ||
             pixel_id->is_tile_dm      (id) ||
             pixel_id->is_mdt          (id) ||
             pixel_id->is_csc          (id) ||
             pixel_id->is_tgc          (id) ||
             pixel_id->is_rpc          (id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_<other> trt check " << endreq;
        sc = StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO << "Check of trt OK " << endreq;
    }

    id = pixel_id->lar_em();
    if (!pixel_id->is_lar_em(id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_lar_em check " << endreq;
        sc = StatusCode::FAILURE;
    }
    else if (!pixel_id->is_lar(id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_lar check for lar em " << endreq;
        sc = StatusCode::FAILURE;
    }
    else if (pixel_id->is_indet(id) ||
             pixel_id->is_tile         (id) ||
             pixel_id->is_muon         (id) ||
             pixel_id->is_calo         (id) ||
             pixel_id->is_lvl1_trig_towers(id) ||
             pixel_id->is_pixel        (id) ||
             pixel_id->is_sct          (id) ||
             pixel_id->is_trt          (id) ||
             pixel_id->is_lar_hec      (id) ||
             pixel_id->is_lar_fcal     (id) ||
             pixel_id->is_lar_minifcal (id) ||
             pixel_id->is_lar_dm       (id) ||
             pixel_id->is_tile_dm      (id) ||
             pixel_id->is_mdt          (id) ||
             pixel_id->is_csc          (id) ||
             pixel_id->is_tgc          (id) ||
             pixel_id->is_rpc          (id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_<other> lar em check " << endreq;
        sc = StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO << "Check of lar_em OK " << endreq;
    }

    id = pixel_id->lar_hec();
    if (!pixel_id->is_lar_hec(id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_lar_hec check " << endreq;
        sc = StatusCode::FAILURE;
    }
    else if (!pixel_id->is_lar(id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_lar check for lar hec " << endreq;
        sc = StatusCode::FAILURE;
    }
    else if (pixel_id->is_indet(id) ||
             pixel_id->is_tile         (id) ||
             pixel_id->is_muon         (id) ||
             pixel_id->is_calo         (id) ||
             pixel_id->is_lvl1_trig_towers(id) ||
             pixel_id->is_pixel        (id) ||
             pixel_id->is_sct          (id) ||
             pixel_id->is_trt          (id) ||
             pixel_id->is_lar_em       (id) ||
             pixel_id->is_lar_fcal     (id) ||
             pixel_id->is_lar_minifcal (id) ||
             pixel_id->is_lar_dm       (id) ||
             pixel_id->is_tile_dm      (id) ||
             pixel_id->is_mdt          (id) ||
             pixel_id->is_csc          (id) ||
             pixel_id->is_tgc          (id) ||
             pixel_id->is_rpc          (id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_<other> lar hec check " << endreq;
        sc = StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO << "Check of lar_hec OK " << endreq;
    }

    id = pixel_id->lar_fcal();
    if (!pixel_id->is_lar_fcal(id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_lar_fcal check " << endreq;
        sc = StatusCode::FAILURE;
    }
    else if (!pixel_id->is_lar(id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_lar check for lar fcal " << endreq;
        sc = StatusCode::FAILURE;
    }
    else if (pixel_id->is_indet(id) ||
             pixel_id->is_tile         (id) ||
             pixel_id->is_muon         (id) ||
             pixel_id->is_calo         (id) ||
             pixel_id->is_lvl1_trig_towers(id) ||
             pixel_id->is_pixel        (id) ||
             pixel_id->is_sct          (id) ||
             pixel_id->is_trt          (id) ||
             pixel_id->is_lar_em       (id) ||
             pixel_id->is_lar_hec      (id) ||
             pixel_id->is_lar_dm       (id) ||
             pixel_id->is_tile_dm      (id) ||
             pixel_id->is_mdt          (id) ||
             pixel_id->is_csc          (id) ||
             pixel_id->is_tgc          (id) ||
             pixel_id->is_rpc          (id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_<other> fcal check " << endreq;
        sc = StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO << "Check of lar_fcal OK " << endreq;
    }

    id = lvl1();
    if (!pixel_id->is_calo(id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_calo check for lvl1 " << endreq;
        sc = StatusCode::FAILURE;
    }
    else if (!pixel_id->is_lvl1_trig_towers(id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_lvl1_trig_towers check " << endreq;
        sc = StatusCode::FAILURE;
    }
    else if (pixel_id->is_indet        (id) ||
             pixel_id->is_lar          (id) ||
             pixel_id->is_tile         (id) ||
             pixel_id->is_muon         (id) ||
             pixel_id->is_pixel        (id) ||
             pixel_id->is_sct          (id) ||
             pixel_id->is_trt          (id) ||
             pixel_id->is_lar_em       (id) ||
             pixel_id->is_lar_hec      (id) ||
             pixel_id->is_lar_fcal     (id) ||
             pixel_id->is_lar_minifcal (id) ||
             pixel_id->is_lar_dm       (id) ||
             pixel_id->is_tile_dm      (id) ||
             pixel_id->is_mdt          (id) ||
             pixel_id->is_csc          (id) ||
             pixel_id->is_tgc          (id) ||
             pixel_id->is_rpc          (id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_<other> lvl1 check " << endreq;
        sc = StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO << "Check of lvl1_trig_towers OK " << endreq;
    }

    id = lar_dm();
    if (!pixel_id->is_calo(id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_calo check of lar_dm " << endreq;
        sc = StatusCode::FAILURE;
    }
    else if (!pixel_id->is_lar_dm(id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_lar_dm check " << endreq;
        sc = StatusCode::FAILURE;
    }
    else if (pixel_id->is_indet        (id) ||
             pixel_id->is_lar          (id) ||
             pixel_id->is_tile         (id) ||
             pixel_id->is_muon         (id) ||
             pixel_id->is_pixel        (id) ||
             pixel_id->is_sct          (id) ||
             pixel_id->is_trt          (id) ||
             pixel_id->is_lar_em       (id) ||
             pixel_id->is_lar_hec      (id) ||
             pixel_id->is_lar_fcal     (id) ||
             pixel_id->is_lar_minifcal (id) ||
             pixel_id->is_lvl1_trig_towers (id) ||
             pixel_id->is_tile_dm      (id) ||
             pixel_id->is_mdt          (id) ||
             pixel_id->is_csc          (id) ||
             pixel_id->is_tgc          (id) ||
             pixel_id->is_rpc          (id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_<other> lar_dm check " << endreq;
        sc = StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO << "Check of lar_dm OK " << endreq;
    }

    id = tile_dm();
    if (!pixel_id->is_calo(id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_calo check for tile_dm " << endreq;
        sc = StatusCode::FAILURE;
    }
    else if (!pixel_id->is_tile_dm(id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_tile_dm check " << endreq;
        sc = StatusCode::FAILURE;
    }
    else if (pixel_id->is_indet        (id) ||
             pixel_id->is_lar          (id) ||
             pixel_id->is_tile         (id) ||
             pixel_id->is_muon         (id) ||
             pixel_id->is_pixel        (id) ||
             pixel_id->is_sct          (id) ||
             pixel_id->is_trt          (id) ||
             pixel_id->is_lar_em       (id) ||
             pixel_id->is_lar_hec      (id) ||
             pixel_id->is_lar_fcal     (id) ||
             pixel_id->is_lar_minifcal (id) ||
             pixel_id->is_lvl1_trig_towers (id) ||
             pixel_id->is_lar_dm       (id) ||
             pixel_id->is_mdt          (id) ||
             pixel_id->is_csc          (id) ||
             pixel_id->is_tgc          (id) ||
             pixel_id->is_rpc          (id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_<other> tile_dm check " << endreq;
        sc = StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO << "Check of tile_dm OK " << endreq;
    }

    id = mdt();
    if (!pixel_id->is_mdt(id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_mdt check " << endreq;
        sc = StatusCode::FAILURE;
    }
    else if (!pixel_id->is_muon(id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_muon check for mdt " << endreq;
        sc = StatusCode::FAILURE;
    }
    else if (pixel_id->is_indet(id) ||
             pixel_id->is_lar          (id) ||
             pixel_id->is_tile         (id) ||
             pixel_id->is_calo         (id) ||
             pixel_id->is_lvl1_trig_towers(id) ||
             pixel_id->is_pixel        (id) ||
             pixel_id->is_sct          (id) ||
             pixel_id->is_trt          (id) ||
             pixel_id->is_lar_em       (id) ||
             pixel_id->is_lar_hec      (id) ||
             pixel_id->is_lar_fcal     (id) ||
             pixel_id->is_lar_minifcal (id) ||
             pixel_id->is_csc          (id) ||
             pixel_id->is_tgc          (id) ||
             pixel_id->is_rpc          (id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_<other> mdt check " << endreq;
        sc = StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO << "Check of mdt OK " << endreq;
    }

    id = rpc();
    if (!pixel_id->is_rpc(id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_rpc check " << endreq;
        sc = StatusCode::FAILURE;
    }
    else if (!pixel_id->is_muon(id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_muon check for rpc " << endreq;
        sc = StatusCode::FAILURE;
    }
    else if (pixel_id->is_indet(id) ||
             pixel_id->is_lar          (id) ||
             pixel_id->is_tile         (id) ||
             pixel_id->is_calo         (id) ||
             pixel_id->is_lvl1_trig_towers(id) ||
             pixel_id->is_pixel        (id) ||
             pixel_id->is_sct          (id) ||
             pixel_id->is_trt          (id) ||
             pixel_id->is_lar_em       (id) ||
             pixel_id->is_lar_hec      (id) ||
             pixel_id->is_lar_fcal     (id) ||
             pixel_id->is_lar_minifcal (id) ||
             pixel_id->is_lar_dm       (id) ||
             pixel_id->is_tile_dm      (id) ||
             pixel_id->is_mdt          (id) ||
             pixel_id->is_csc          (id) ||
             pixel_id->is_tgc          (id) ) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_<other> rpc check " << endreq;
        sc = StatusCode::FAILURE;

        log << MSG::INFO << "is_indet    "     <<  pixel_id->is_indet        (id) << endreq;
        log << MSG::INFO << "is_lar      "     <<  pixel_id->is_lar          (id) << endreq;
        log << MSG::INFO << "is_tile     "     <<  pixel_id->is_tile         (id) << endreq;
        log << MSG::INFO << "is_calo     "     <<  pixel_id->is_calo         (id) << endreq;
        log << MSG::INFO << "is_lvl1     "     <<  pixel_id->is_lvl1_trig_towers(id) << endreq;
        log << MSG::INFO << "is_pixel    "     <<  pixel_id->is_pixel        (id) << endreq;
        log << MSG::INFO << "is_sct      "     <<  pixel_id->is_sct          (id) << endreq;
        log << MSG::INFO << "is_trt      "     <<  pixel_id->is_trt          (id) << endreq;
        log << MSG::INFO << "is_lar_em   "     <<  pixel_id->is_lar_em       (id) << endreq;
        log << MSG::INFO << "is_lar_hec  "     <<  pixel_id->is_lar_hec      (id) << endreq;
        log << MSG::INFO << "is_lar_fcal "     <<  pixel_id->is_lar_fcal     (id) << endreq;
        log << MSG::INFO << "is_lar_minifcal " <<  pixel_id->is_lar_minifcal (id) << endreq;
        log << MSG::INFO << "is_lar_dm   "     <<  pixel_id->is_lar_dm       (id) << endreq;
        log << MSG::INFO << "is_tile_dm  "     <<  pixel_id->is_tile_dm      (id) << endreq;
        log << MSG::INFO << "is_mdt      "     <<  pixel_id->is_mdt          (id) << endreq;
        log << MSG::INFO << "is_csc      "     <<  pixel_id->is_csc          (id) << endreq;
        log << MSG::INFO << "is_tgc      "     <<  pixel_id->is_tgc          (id) << endreq;

    }
    else {
        log << MSG::INFO << "Check of rpc OK " << endreq;
    }

    id = csc();
    if (!pixel_id->is_csc(id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_csc check " << endreq;
        sc = StatusCode::FAILURE;
    }
    else if (!pixel_id->is_muon(id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_muon check for csc " << endreq;
        sc = StatusCode::FAILURE;
    }
    else if (pixel_id->is_indet(id) ||
             pixel_id->is_lar          (id) ||
             pixel_id->is_tile         (id) ||
             pixel_id->is_calo         (id) ||
             pixel_id->is_lvl1_trig_towers(id) ||
             pixel_id->is_pixel        (id) ||
             pixel_id->is_sct          (id) ||
             pixel_id->is_trt          (id) ||
             pixel_id->is_lar_em       (id) ||
             pixel_id->is_lar_hec      (id) ||
             pixel_id->is_lar_fcal     (id) ||
             pixel_id->is_lar_minifcal (id) ||
             pixel_id->is_lar_dm       (id) ||
             pixel_id->is_tile_dm      (id) ||
             pixel_id->is_mdt          (id) ||
             pixel_id->is_tgc          (id) ||
             pixel_id->is_rpc          (id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_<other> csc check " << endreq;
        sc = StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO << "Check of csc OK " << endreq;
    }

    id = tgc();
    if (!pixel_id->is_tgc(id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_tgc check " << endreq;
        sc = StatusCode::FAILURE;
    }
    else if (!pixel_id->is_muon(id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_muon check for tgc " << endreq;
        sc = StatusCode::FAILURE;
    }
    else if (pixel_id->is_indet(id) ||
             pixel_id->is_lar          (id) ||
             pixel_id->is_tile         (id) ||
             pixel_id->is_calo         (id) ||
             pixel_id->is_lvl1_trig_towers(id) ||
             pixel_id->is_pixel        (id) ||
             pixel_id->is_sct          (id) ||
             pixel_id->is_trt          (id) ||
             pixel_id->is_lar_em       (id) ||
             pixel_id->is_lar_hec      (id) ||
             pixel_id->is_lar_fcal     (id) ||
             pixel_id->is_lar_minifcal (id) ||
             pixel_id->is_lar_dm       (id) ||
             pixel_id->is_tile_dm      (id) ||
             pixel_id->is_mdt          (id) ||
             pixel_id->is_csc          (id) ||
             pixel_id->is_rpc          (id)) {
        pixel_id->show(id);
        log << MSG::ERROR << "Failed is_<other> tgc check " << endreq;
        sc = StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO << "Check of tgc OK " << endreq;
    }
    
    log << MSG::INFO << "Check of AtlasDetectorID OK " << endreq;

    return (sc);
}



//////////////////////////////////////////////////////
StatusCode IdDictTest::test_indet_ids() const
{

    MsgStream  log(msgSvc(),name());

    TestInDetIds testInDetIds(m_idDictMgr, 
                              m_pixIDHelper,
                              m_sctIDHelper,
                              m_trtIDHelper,
                              m_siliconID,
                              msgSvc(), 
                              chronoSvc());
    
    // Run tests
    if(StatusCode::SUCCESS != testInDetIds.testMinMax()) {
        return StatusCode::FAILURE;
    }
    if(m_pixelFlag) {
        if(StatusCode::SUCCESS != testInDetIds.testPixelIds()) {
            return StatusCode::FAILURE;
        }
        if(StatusCode::SUCCESS != testInDetIds.testPixelDetElem()) {
            return StatusCode::FAILURE;
        }
    }
    if(m_sctFlag) {
        if(StatusCode::SUCCESS != testInDetIds.testSCTIds()) {
            return StatusCode::FAILURE;
        }
        if(StatusCode::SUCCESS != testInDetIds.testSCTNeighbors()) {
            return StatusCode::FAILURE;
        }
    }
    if (m_pixelFlag || m_sctFlag) {
        if(StatusCode::SUCCESS != testInDetIds.testSiliconIds()) {
            return StatusCode::FAILURE;
        }
    }
    if(m_trtFlag) {
        if(StatusCode::SUCCESS != testInDetIds.testTRTDetElem()) {
            return StatusCode::FAILURE;
        }
        if(StatusCode::SUCCESS != testInDetIds.testTRTDetElemSL()) {
            return StatusCode::FAILURE;
        }
        if(StatusCode::SUCCESS != testInDetIds.testTRTStraw()) {
            return StatusCode::FAILURE;
        }
    }

    return StatusCode::SUCCESS;

}

/////////////////////////////////////////////////////////////////////
StatusCode IdDictTest::test_muons_ids() const
{

    MsgStream  log(msgSvc(),name());

    TestMuonIds testMuonIds(m_idDictMgr, 
                            m_detStore, 
                            msgSvc(), 
                            chronoSvc());
    

    // Run tests
  
    if(m_mdtFlag) {
        if(StatusCode::SUCCESS != testMuonIds.testMDTIds()) {
            return StatusCode::FAILURE;
        }
        if(StatusCode::SUCCESS != testMuonIds.testMDTDetElem()) {
            return StatusCode::FAILURE;
        }
    }

    if(m_cscFlag) {
        if(StatusCode::SUCCESS != testMuonIds.testCSCIds()) {
            return StatusCode::FAILURE;
        }
        if(StatusCode::SUCCESS != testMuonIds.testCSCDetElem()) {
            return StatusCode::FAILURE;
        }
    }

    if(m_rpcFlag) {
        if(StatusCode::SUCCESS != testMuonIds.testRPCIds()) {
            return StatusCode::FAILURE;
        }
        if(StatusCode::SUCCESS != testMuonIds.testRPCDetElem()) {
            return StatusCode::FAILURE;
        }
    }

    if(m_tgcFlag) {
        if(StatusCode::SUCCESS != testMuonIds.testTGCIds()) {
            return StatusCode::FAILURE;
        }
        if(StatusCode::SUCCESS != testMuonIds.testTGCDetElem()) {
            return StatusCode::FAILURE;
        }
    }

    return StatusCode::SUCCESS;

}


/////////////////////////////////////////////////////////////////////
StatusCode IdDictTest::test_calos_ids() const
{

    MsgStream  log(msgSvc(),name());
    
    TestCaloIds testCaloIds(m_idDictMgr, 
                            //                      m_caloTTMgr, 
                            //                      m_caloMgr, 
                            m_lvl1Helper,
                            m_caloDMHelper,
                            m_caloCellHelper,
                            msgSvc(), 
                            chronoSvc());
    
    // Run tests
    if (m_caloTTFlag) {
        if(StatusCode::SUCCESS != testCaloIds.testLvl1Ids()) {
            return StatusCode::FAILURE;
        }
        if(StatusCode::SUCCESS != testCaloIds.testLvl1Neighbors()) {
            return StatusCode::FAILURE;
        }
        if(StatusCode::SUCCESS != testCaloIds.testLArDM_Ids()) {
            return StatusCode::FAILURE;
        }
        if(StatusCode::SUCCESS != testCaloIds.testTileDM_Ids()) {
            return StatusCode::FAILURE;
        }
        if(StatusCode::SUCCESS != testCaloIds.testCaloCell_Ids()) {
            return StatusCode::FAILURE;
        }
    }
    
    // TestLArIds Class 
    TestLArIds testLArIds(m_idDictMgr, 
                          m_emHelper, 
                          m_hecHelper, 
                          m_fcalHelper,
                          m_miniFcalHelper,
                          m_laronlineHelper,
                          msgSvc(), 
                          chronoSvc());
    
    if ( m_emBarrelFlag || m_emEndcapFlag ) {
        if(StatusCode::SUCCESS != testLArIds.testEmIds()) {
            return StatusCode::FAILURE;
        }
        if(StatusCode::SUCCESS != testLArIds.testEmNeighbors()) {
            return StatusCode::FAILURE;
        }
    }
    
    if (m_hecFlag) {
        if(StatusCode::SUCCESS != testLArIds.testHecIds()) {
            return StatusCode::FAILURE;
        }
        if(StatusCode::SUCCESS != testLArIds.testHecNeighbors()) {
            return StatusCode::FAILURE;
        }
    }
    
    if (m_fcalFlag) {
        if(StatusCode::SUCCESS != testLArIds.testFcalIds()) {
            return StatusCode::FAILURE;
        }
        if(StatusCode::SUCCESS != testLArIds.testFcalNeighbors()) {
            return StatusCode::FAILURE;
        }
    } 
    if (m_miniFcalFlag) {
        if(StatusCode::SUCCESS != testLArIds.testMiniFcalIds()) {
            return StatusCode::FAILURE;
        }
//             if(StatusCode::SUCCESS != testLArIds.testMiniFcalNeighbors()) {
//                 return StatusCode::FAILURE;
//             }
    } 
    return StatusCode::SUCCESS;
}


////////////////////////////////////////////////////////////////////
StatusCode IdDictTest::test_laronline_ids() const
{

    MsgStream  log(msgSvc(),name());    
    // TestLArIds Class 
    TestLArIds testLArIds(m_idDictMgr, 
                          m_emHelper, 
                          m_hecHelper, 
                          m_fcalHelper,
                          m_miniFcalHelper,
                          m_laronlineHelper,
                          msgSvc(), 
                          chronoSvc());    
    if ( m_laronlineFlag ) 
        {
            log << MSG::DEBUG << " /////// ==> Entering test_laronline_ids " << endreq;         
            if(StatusCode::SUCCESS != testLArIds.testLaronlineIds()) {
                return StatusCode::FAILURE;
            }
        }
    return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////
Identifier IdDictTest::lvl1() const
{
    // Get DetStore
    Identifier result;
    MsgStream  log(msgSvc(),name());    

    if (m_calo_dict) {    
        IdDictGroup* group =  m_calo_dict->find_group ("Reg_Lvl1");
        if(!group) {
            std::cout << "IdDictTest::lvl1 - cannot find Reg_LVL1 group"
                      << std::endl;
        }
        size_t index =  group->regions()[0]->m_index;
        int id[2];
        id[0] = 10;
        id[1] = -1;
        Identifier compact_id(0);
        if (!m_calo_dict->pack32(id,
                                 0,
                                 1,
                                 index,
                                 compact_id)) {
            result = compact_id;
        }
        const PixelID* pixel_id;
        if (m_detStore->retrieve(pixel_id, "PixelID").isFailure()) {
            log << MSG::ERROR << "Could not get Pixel ID helper" << endreq;
        }
        log << MSG::DEBUG << "lvl1: id = " 
            << pixel_id->show_to_string(result) 
            << endreq;  
    }
    else {
        log << MSG::DEBUG << "lvl1: calo dict not available!! " 
            << endreq;  
    }
  
    return (result);
}


/////////////////////////////////////////////////////////////////////
Identifier IdDictTest::lar_dm() const
{
    MsgStream  log(msgSvc(),name());    
    // Get DetStore
    Identifier result;
    if (m_calo_dict) {
        IdDictGroup* group =  m_calo_dict->find_group ("DM_Reg");
        if(!group) {
            std::cout << "IdDictTest::lar_dm - cannot find DM_Reg group"
                      << std::endl;
        }
        size_t index =  group->regions()[0]->m_index;
        int id[2];
        id[0] = 10;
        id[1] = -4;
        Identifier compact_id(0);
        if (!m_calo_dict->pack32(id,
                                 0,
                                 1,
                                 index,
                                 compact_id)) {
            result = compact_id;
        }
        const PixelID * pixel_id;
        if (m_detStore->retrieve(pixel_id, "PixelID").isFailure()) {
            log << MSG::ERROR << "Could not get Pixel ID helper" << endreq;
        }
        log << MSG::DEBUG << "lar_dm: id = " 
            << pixel_id->show_to_string(result) 
            << endreq;  
    }
    else {
        log << MSG::DEBUG << "lar_dm: calo dict not available!! " 
            << endreq;  
    }
    return (result);
}


/////////////////////////////////////////////////////////////////////
Identifier IdDictTest::tile_dm() const
{
    MsgStream  log(msgSvc(),name());    
    // Get DetStore
    Identifier result;
    if (m_calo_dict) {

        IdDictGroup* group =  m_calo_dict->find_group ("DM_Reg");
        if(!group) {
            std::cout << "IdDictTest::tile_dm - cannot find DM_Reg group"
                      << std::endl;
        }
        size_t index =  group->regions()[0]->m_index;
        int id[2];
        id[0] = 10;
        id[1] = -5;
        Identifier compact_id(0);
        if (!m_calo_dict->pack32(id,
                                 0,
                                 1,
                                 index,
                                 compact_id)) {
            result = compact_id;
        }
        const PixelID * pixel_id;
        if (m_detStore->retrieve(pixel_id, "PixelID").isFailure()) {
            log << MSG::ERROR << "Could not get Pixel ID helper" << endreq;
        }
        log << MSG::DEBUG << "tile_dm: id = " 
            << pixel_id->show_to_string(result) 
            << endreq;  
    }
    else {
        log << MSG::DEBUG << "tile_dm: calo dict not available!! " 
            << endreq;  
    }
    return (result);
}




/////////////////////////////////////////////////////////////////////
Identifier IdDictTest::mdt() const
{
    MsgStream  log(msgSvc(),name());    
    // Get DetStore
    Identifier result;
    if (m_muon_dict) {

        IdDictGroup* mdtGroup =  m_muon_dict->find_group ("mdt");
        if(!mdtGroup) {
            std::cout << "IdDictTest::mdt - cannot find mdt group"
                      << std::endl;
        }
        size_t mdtIndex =  mdtGroup->regions()[0]->m_index;
        int id[5];
        id[0] = 7;
        id[1] = 0;
        id[2] = 1;
        id[3] = 1;
        id[4] = 0;
        Identifier compact_id(0);
        if (!m_muon_dict->pack32(id,
                                 0,
                                 4,
                                 mdtIndex,
                                 compact_id)) {
            result = compact_id;
        }
        const PixelID * pixel_id;
        if (m_detStore->retrieve(pixel_id, "PixelID").isFailure()) {
            log << MSG::ERROR << "Could not get Pixel ID helper" << endreq;
        }
        //const PixelID* pixel_id   = m_pixMgr->getIdHelper();
        log << MSG::DEBUG << "mdt: id = " 
            << pixel_id->show_to_string(result) 
            << endreq;  
    }
    else {
        log << MSG::DEBUG << "mdt: muon dict not available!! " 
            << endreq;  
    }
    return (result);
}


/////////////////////////////////////////////////////////////////////
Identifier IdDictTest::csc() const
{
    MsgStream  log(msgSvc(),name());    
    Identifier result;
    if (m_muon_dict) {

        IdDictGroup* cscGroup =  m_muon_dict->find_group ("csc");
        if(!cscGroup) {
            std::cout << "IdDictTest::csc - cannot find csc group"
                      << std::endl;
        }
        size_t cscIndex =  cscGroup->regions()[0]->m_index;
        int id[5];
        id[0] = 7;
        id[1] = 51;
        id[2] = 1;
        id[3] = 1;
        id[4] = 1;
        Identifier compact_id(0);
        if (!m_muon_dict->pack32(id,
                                 0,
                                 4,
                                 cscIndex,
                                 compact_id)) {
            result = compact_id;
        }
        const PixelID * pixel_id;
        if (m_detStore->retrieve(pixel_id, "PixelID").isFailure()) {
            log << MSG::ERROR << "Could not get Pixel ID helper" << endreq;
        }
        log << MSG::DEBUG << "csc: id = " 
            << pixel_id->show_to_string(result) 
            << endreq;  
    }
    else {
        log << MSG::DEBUG << "csc: muon dict not available!! " 
            << endreq;  
    }
    return (result);
}


/////////////////////////////////////////////////////////////////////
Identifier IdDictTest::rpc() const
{
    MsgStream  log(msgSvc(),name());    
    // Get DetStore
    Identifier result;
    if (m_muon_dict) {

        IdDictGroup* rpcGroup =  m_muon_dict->find_group ("rpc");
        if(!rpcGroup) {
            std::cout << "IdDictTest::rpc - cannot find rpc group"
                      << std::endl;
        }
        size_t rpcIndex =  rpcGroup->regions()[0]->m_index;
        int id[6];
        id[0] = 7;
        id[1] = 2;
        id[2] = 2;
        id[3] = 1;
        id[4] = 2;
        id[5] = 1;
        Identifier compact_id(0);
        if (!m_muon_dict->pack32(id,
                                 0,
                                 5,
                                 rpcIndex,
                                 compact_id)) {
            result = compact_id;
        }
        const PixelID * pixel_id;
        if (m_detStore->retrieve(pixel_id, "PixelID").isFailure()) {
            log << MSG::ERROR << "Could not get Pixel ID helper" << endreq;
        }
        log << MSG::DEBUG << "rpc: id = " 
            << pixel_id->show_to_string(result) 
            << endreq;  
    }
    else {
        log << MSG::DEBUG << "rpc: muon dict not available!! " 
            << endreq;  
    }
    return (result);
}


/////////////////////////////////////////////////////////////////////
Identifier IdDictTest::tgc() const
{
    MsgStream  log(msgSvc(),name());    
    // Get DetStore
    Identifier result;
    if (m_muon_dict) {

        IdDictGroup* tgcGroup =  m_muon_dict->find_group ("tgc");
        if(!tgcGroup) {
            std::cout << "IdDictTest::tgc - cannot find tgc group"
                      << std::endl;
        }
        size_t tgcIndex =  tgcGroup->regions()[0]->m_index;
        int id[5];
        id[0] = 7;
        id[1] = 42;
        id[2] = -5;
        id[3] = 1;
        id[4] = 3;
        Identifier compact_id(0);
        if (!m_muon_dict->pack32(id,
                                 0,
                                 4,
                                 tgcIndex,
                                 compact_id)) {
            result = compact_id;
        }
        const PixelID * pixel_id;
        if (m_detStore->retrieve(pixel_id, "PixelID").isFailure()) {
            log << MSG::ERROR << "Could not get Pixel ID helper" << endreq;
        }
        log << MSG::DEBUG << "tgc: id = " 
            << pixel_id->show_to_string(result) 
            << endreq;  
    }
    else {
        log << MSG::DEBUG << "tgc: muon dict not available!! " 
            << endreq;  
    }
    return (result);
}
