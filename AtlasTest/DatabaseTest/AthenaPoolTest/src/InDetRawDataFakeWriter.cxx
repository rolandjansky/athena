/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetRawDataFakeWriter.cxx
 *
 * @brief Test Algorithm for POOL I/O, writes InDetRawData to
 * transient store.
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: InDetRawDataFakeWriter.cxx,v 1.14 2008-06-20 16:41:09 schaffer Exp $
 *
 */

// Silicon trackers includes
#include "InDetRawDataFakeWriter.h"

#include "AthenaPoolTestData/PixelRDOElemLinkVec.h"


//InDetRawData includes
#include "InDetRawData/PixelRDO_Collection.h"
#include "InDetRawData/SCT_RDO_Collection.h"
#include "InDetRawData/TRT_RDO_Collection.h"

//#include "InDetRawData/InDetRawDataCLASS_DEF.h"

// Id includes
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"

// test includes
#include "InDetRawDataFakeCreator.h"


#include <map>

// Constructor with parameters:
InDetRawDataFakeWriter::InDetRawDataFakeWriter(const std::string &name, 
                                       ISvcLocator *pSvcLocator) :
    AthAlgorithm(name,pSvcLocator),
    m_pixCont(0),
    m_sctCont(0),
    m_trtCont(0),
    m_pixelId(0),
    m_sctId(0),
    m_trtId(0)
{}

// Initialize method:
StatusCode InDetRawDataFakeWriter::initialize()
{
    ATH_MSG_INFO("InDetRawDataFakeWriter::initialize()" );

    const DataHandle<PixelID> pixel_id;
    ATH_CHECK( detStore()->retrieve(pixel_id, "PixelID") );
    m_pixelId = pixel_id;

    const DataHandle<SCT_ID> sct_id;
    ATH_CHECK( detStore()->retrieve(sct_id, "SCT_ID") );
    m_sctId = sct_id;

    const DataHandle<TRT_ID> trt_id;
    ATH_CHECK( detStore()->retrieve(trt_id, "TRT_ID") );
    m_trtId = trt_id;

    // create the IdentifiableContainers to contain the rdo collections
    try {
        m_pixCont=new PixelRDO_Container(m_pixelId->wafer_hash_max());
    } catch (std::bad_alloc) {
        ATH_MSG_ERROR("Could not create a new Pixel RawDataContainer !");
        return StatusCode::FAILURE;
    }
  
    try {
        m_sctCont=new SCT_RDO_Container(m_sctId->wafer_hash_max());
    } catch (std::bad_alloc) {
        ATH_MSG_ERROR("Could not create a new SCT  RawDataContainer !");
        return StatusCode::FAILURE;
    }
  
    try {
        m_trtCont=new TRT_RDO_Container(m_trtId->straw_layer_hash_max());
    } catch (std::bad_alloc) {
        ATH_MSG_ERROR("Could not create a new TRT RawDataContainer !");
        return StatusCode::FAILURE;
    }
  
    // preventing SG from deleting object:
    m_pixCont->addRef();
    m_sctCont->addRef();
    m_trtCont->addRef();

    return StatusCode::SUCCESS;
}

// Execute method:
StatusCode InDetRawDataFakeWriter::execute() 
{
    // Get the messaging service, print where you are
    ATH_MSG_DEBUG( "InDetRawDataFakeWriter::execute()"  );
    ATH_CHECK( createPixels() );
    ATH_CHECK( createSCTs() );
    ATH_CHECK( createTRTs() );
    ATH_CHECK( printRDOs() );
    return StatusCode::SUCCESS;
}

StatusCode InDetRawDataFakeWriter::createPixels() const
{
    ATH_MSG_DEBUG("InDetRawDataFakeWriter::createPixels()" );


    // IDC creation and registration, done once per job:

    // the IDC is created in the initialize, must clear the IDC every event:
    m_pixCont->cleanup();

    // register the Pixel IdentifiableContainer into StoreGate
    ATH_CHECK( evtStore()->record(m_pixCont,"PixelRDOs") );

    // Create and record the element link vector
    PixelRDOElemLinkVec* linkVec = new PixelRDOElemLinkVec;
    ATH_CHECK(  evtStore()->record(linkVec,"PixelRDOELs") );

    // loop over 10 different pixel wafers
    int deltaWafer = m_pixelId->wafer_hash_max()/15;
    IdentifierHash waferHash = deltaWafer;

    InDetRawDataFakeCreator creator;
  
    IdContext cntx = m_pixelId->wafer_context();

    for (int i=0; i < 10; ++i, waferHash += deltaWafer) {
      
        // Print out contents of RDO
        ATH_MSG_DEBUG("Creating RDO collection: " 
            << m_pixelId->show_to_string(m_pixelId->wafer_id(waferHash), &cntx) << " " 
            << MSG::hex << m_pixelId->wafer_id(waferHash) << MSG::dec << " "
            << MSG::hex << (unsigned int)waferHash << MSG::dec );

        // create a new pixel RDO collection
        MsgStream log(msgSvc(), name());
        const InDetRawDataCollection<PixelRDORawData> *p_rdocoll = 
            creator.createPixelRawDataColl(waferHash, m_pixelId, log);

	// register the rdo collection in StoreGate
	// add rdo collection to the container
	IdentifierHash id_hash = p_rdocoll->identifyHash();
	ATH_CHECK( m_pixCont->addCollection(p_rdocoll, id_hash) );

        // Loop over RDOs and add ELs to vector
        for (unsigned int irdo = 0; irdo < p_rdocoll->size(); ++irdo) {
            unsigned int itest = irdo % 3;
            if (itest == 2) {
                // Create el with element and container
                ATH_MSG_VERBOSE("Pixel RDOs: create EL without index");
                PixelRDOElemLinkVec::elem_type el((*p_rdocoll)[irdo], *m_pixCont);
                linkVec->pixelRDOs().push_back(el);
            }
            else {
                // Create el with hash and sometimes index
                IdentContIndex index;
                index.setCollHash(waferHash);
                if (itest == 1) index.setObjIndex(irdo);
                ATH_MSG_VERBOSE("Pixel RDOs: create EL - hash, index, hashAndIndex: " 
                    << index.collHash() << " " << index.objIndex() << " " 
                    << MSG::hex << index.hashAndIndex() << MSG::dec);
                // Create el with element and index
                PixelRDOElemLinkVec::elem_type el(*m_pixCont, index.hashAndIndex());
                el.setElement((*p_rdocoll)[irdo]);
                linkVec->pixelRDOs().push_back(el);
            }
        }
    }

    return StatusCode::SUCCESS;
}

StatusCode InDetRawDataFakeWriter::createSCTs() const
{
    ATH_MSG_DEBUG("InDetRawDataFakeWriter::execute()" );

    // IDC creation and registration, done once per job:

  // the IDC is created in the initialize, must clear the IDC every event:
    m_sctCont->cleanup();


    ATH_CHECK( evtStore()->record(m_sctCont,"SCT_RDOs") );

    // loop over 10 different sct wafers
    int deltaWafer = m_sctId->wafer_hash_max()/15;
    IdentifierHash waferHash = deltaWafer;

    InDetRawDataFakeCreator creator;

    IdContext cntx = m_sctId->wafer_context();

    for (int i=0; i < 10; ++i, waferHash += deltaWafer) {
      
        // Print out contents of RDO
        ATH_MSG_DEBUG("Creating RDO collection: " 
            << m_sctId->show_to_string(m_sctId->wafer_id(waferHash), &cntx) << " " 
            << MSG::hex << m_sctId->wafer_id(waferHash) << MSG::dec << " "
            << MSG::hex << (unsigned int)waferHash << MSG::dec );

        // create a new sct RDO collection
        MsgStream log(msgSvc(), name());
        const InDetRawDataCollection<SCT_RDORawData> *p_rdocoll = 
            creator.createSCT_RawDataColl(waferHash, m_sctId, msg());

	// register the rdo collection in StoreGate
	IdentifierHash id_hash = p_rdocoll->identifyHash();
	ATH_CHECK(  m_sctCont->addCollection(p_rdocoll, id_hash) );
    }

    return StatusCode::SUCCESS;
}

StatusCode InDetRawDataFakeWriter::createTRTs() const
{
    ATH_MSG_DEBUG("InDetRawDataFakeWriter::execute()" );

    // IDC creation and registration, done once per job:

  // the IDC is created in the initialize, must clear the IDC every event:
    m_trtCont->cleanup();


    // register the TRT_ IdentifiableContainer into StoreGate
    ATH_CHECK( evtStore()->record(m_trtCont,"TRT_RDOs") );

    // loop over 10 different trt straw layers
    int deltaStrLay = m_trtId->straw_layer_hash_max()/15;
    IdentifierHash strLayHash = deltaStrLay;

    InDetRawDataFakeCreator creator;
    
    IdContext cntx = m_trtId->straw_layer_context();
  
    for (int i=0; i < 10; ++i, strLayHash += deltaStrLay) {
      
        // Print out contents of RDO
        ATH_MSG_DEBUG("Creating RDO collection: " 
            << m_trtId->show_to_string(m_trtId->layer_id(strLayHash), &cntx) << " " 
            << MSG::hex << m_trtId->layer_id(strLayHash) << MSG::dec << " "
            << MSG::hex << (unsigned int)strLayHash << MSG::dec );

        // create a new trt RDO collection
        const InDetRawDataCollection<TRT_RDORawData> *p_rdocoll = 
          creator.createTRT_RawDataColl(strLayHash, m_trtId, msg());

	// register the rdo collection in StoreGate
	IdentifierHash id_hash = p_rdocoll->identifyHash();
	ATH_CHECK( m_trtCont->addCollection(p_rdocoll, id_hash) );
    }

    return StatusCode::SUCCESS;
}




// Print rdos
StatusCode InDetRawDataFakeWriter::printRDOs() const
{
    ATH_MSG_INFO("InDetRawDataFakeWriter::printRDOs()" );

    // loop on all RDO collections

    // pixels

    for(PixelRDO_Container::const_iterator it=m_pixCont->begin() ; 
        it!=m_pixCont->end() ; ++it) {

        const InDetRawDataCollection< PixelRDORawData > *rdocoll=&(**it);
      
        ATH_MSG_DEBUG("PixelRDORawData collection found with id "
            << m_pixelId->show_to_string(rdocoll->identify())
            << " and " << rdocoll->size() << " RDOs" );
      
        // loop on all RDOs
        DataVector<PixelRDORawData>::const_iterator rdo;
        for(rdo=rdocoll->begin() ; 
            rdo!=rdocoll->end() ; ++rdo) {
        
            ATH_MSG_DEBUG("RDO ID=" << m_pixelId->show_to_string((*rdo)->identify())
                          << "RDO ToT=" << (*rdo)->getToT() << " id:"
                          << (*rdo)->identify().getString() );
        
        }
    }

    // pixel element links

    // Retrive the element link vector
    const PixelRDOElemLinkVec* linkVec = 0;
    ATH_CHECK( evtStore()->retrieve(linkVec, "PixelRDOELs") );
    
    ATH_MSG_DEBUG("PixelRDOElemLinkVec found with " << linkVec->pixelRDOs().size() << "  RDOs " );
    for(unsigned int i = 0; i < linkVec->pixelRDOs().size(); ++i) {
        // Print out rdos from element link
        PixelRDOElemLinkVec::elem_type rdo  = linkVec->pixelRDOs()[i];
        ATH_MSG_DEBUG("RDO ID=" << m_pixelId->show_to_string((*rdo)->identify())
                      << "RDO ToT=" << (*rdo)->getToT() 
                      << " id:" << (*rdo)->identify().getString() );
    }


    // scts

    for(SCT_RDO_Container::const_iterator it=m_sctCont->begin() ; 
        it!=m_sctCont->end() ; ++it) {

        const InDetRawDataCollection< SCT_RDORawData > *rdocoll=&(**it);
      
        ATH_MSG_DEBUG("SCT_RDORawData collection found with id "
            << m_sctId->show_to_string(rdocoll->identify())
            << " and " << rdocoll->size() << " RDOs" );
      
        // loop on all RDOs
        DataVector<SCT_RDORawData>::const_iterator rdo;
        for(rdo=rdocoll->begin() ; 
            rdo!=rdocoll->end() ; ++rdo) {
        
            ATH_MSG_DEBUG("RDO ID=" << m_sctId->show_to_string((*rdo)->identify()) 
                          << "RDO grp=" << (*rdo)->getGroupSize() );
        
        }
    }

    // trts

    for(TRT_RDO_Container::const_iterator it=m_trtCont->begin() ; 
        it!=m_trtCont->end() ; ++it) {

        const InDetRawDataCollection< TRT_RDORawData > *rdocoll=&(**it);
      
        ATH_MSG_DEBUG("TRT_RDORawData collection found with id "
            << m_trtId->show_to_string(rdocoll->identify())
            << " and " << rdocoll->size() << " RDOs" );
      
        // loop on all RDOs
        DataVector<TRT_RDORawData>::const_iterator rdo;
        for(rdo=rdocoll->begin() ; 
            rdo!=rdocoll->end() ; ++rdo) {
        
            ATH_MSG_DEBUG("RDO ID=" << m_trtId->show_to_string((*rdo)->identify())
                          << "RDO hilvl=" << (*rdo)->highLevel() << " ToT:"
                          <<  (*rdo)->timeOverThreshold() << " drift: " << (*rdo)->driftTimeBin());
        
        }
    }

    ATH_MSG_DEBUG("InDetRawDataFakeWriter::printRDOs() end" );

    return StatusCode::SUCCESS;

}


// Finalize method:
StatusCode InDetRawDataFakeWriter::finalize() 
{
    ATH_MSG_INFO("InDetRawDataFakeWriter::finalize()" );
    return StatusCode::SUCCESS;
}

