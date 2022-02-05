/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file RDOReaderDoubleSelector.cxx
 *
 * @brief Test Algorithm for POOL I/O, reads InDetRawData and CaloCells from
 * transient store.
 *
 * @author Miha Muskinja <miha.muskinja@cern.ch>
 *
 */

// Silicon trackers includes
#include "RDOReaderDoubleSelector.h"

#include "AthenaPoolTestData/PixelRDOElemLinkVec.h"

//InDetRawData includes
#include "InDetRawData/PixelRDO_Container.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetRawData/TRT_RDO_Container.h"
#include "InDetRawData/PixelRDO_Collection.h"
#include "InDetRawData/SCT_RDO_Collection.h"
#include "InDetRawData/TRT_RDO_Collection.h"

// Event includes
#include "CaloEvent/CaloCellContainer.h"

// DetDescr includes
#include "CaloIdentifier/CaloCell_ID.h"

// Id includes
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"

// test includes
#include "InDetRawDataFakeCreator.h"
#include "LArCellContFakeCreator.h"

// Gaudi includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

// test includes
#include <map>

// Constructor with parameters:
RDOReaderDoubleSelector::RDOReaderDoubleSelector(const std::string &name
						 , ISvcLocator *pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_pixelId(nullptr)
  , m_sctId(nullptr)
  , m_trtId(nullptr)
  , m_calocellId(nullptr)
{}

// Initialize method:
StatusCode RDOReaderDoubleSelector::initialize()
{
  ATH_MSG_INFO("RDOReaderDoubleSelector::initialize()" );

  ATH_CHECK( detStore()->retrieve(m_pixelId, "PixelID") );
  ATH_CHECK( detStore()->retrieve(m_sctId, "SCT_ID") );
  ATH_CHECK( detStore()->retrieve(m_trtId, "TRT_ID") );

  ATH_CHECK( detStore()->retrieve(m_calocellId, "CaloCell_ID") );
  ATH_CHECK(m_caloMgrKey.initialize());

  return StatusCode::SUCCESS;
}

// Execute method:
StatusCode RDOReaderDoubleSelector::execute()
{
  ATH_MSG_DEBUG("RDOReaderDoubleSelector::execute()" );

  ATH_CHECK( checkPixel() );
  ATH_CHECK( checkSCT() );
  ATH_CHECK( checkTRT() );
  ATH_CHECK( checkCells() );
  return StatusCode::SUCCESS;
}

StatusCode RDOReaderDoubleSelector::checkPixel() const
{
  ATH_MSG_DEBUG("RDOReaderDoubleSelector::checkPixel()" );

  const PixelRDO_Container* pixCont = nullptr;
  ATH_CHECK( evtStore()->retrieve(pixCont, "PixelRDOs") );

  // loop on all RDO collections read in and check them
  int nmatch = 0;
  bool error = false;
  InDetRawDataFakeCreator creator;
  IdContext cntx = m_pixelId->wafer_context();

  ATH_MSG_DEBUG("PixelRDO_Container size "
                << pixCont->size() << " " );

  // Check for non-zero length of container
  if (pixCont->begin() == pixCont->end()) {
    ATH_MSG_ERROR("Container '" << "PixelRDOs" << "' is EMPTY !" );
    return StatusCode::FAILURE;
  }

  for (PixelRDO_Container::const_iterator it = pixCont->begin() ;
       it != pixCont->end() ; ++it) {

    const InDetRawDataCollection<PixelRDORawData>* RDO_Collection(&(**it));

    ATH_MSG_DEBUG("Read RDO collection: size, ids "
                  << RDO_Collection->size() << " "
                  << m_pixelId->show_to_string(RDO_Collection->identify(), &cntx) << " "
                  << MSG::hex << (unsigned int)RDO_Collection->identifyHash() << MSG::dec );

    // create a new pixel RDO collection
    MsgStream log(msgSvc(), name());
    const InDetRawDataCollection<PixelRDORawData> *rdocoll =
      creator.createPixelRawDataColl(RDO_Collection->identifyHash(), m_pixelId, msg());

    ATH_MSG_DEBUG("Test RDO collection: size, ids "
                  << rdocoll->size() << " "
                  << m_pixelId->show_to_string(rdocoll->identify(), &cntx) << " "
                  << MSG::hex << (unsigned int)rdocoll->identifyHash() << MSG::dec );


    // Use one of the specific clustering AlgTools to make clusters
    //typedef InDetRawDataCollection<Pixel1RawData> RDO1_Collection_type;
    typedef InDetRawDataCollection<PixelRDORawData> RDO_Collection_type;

    // loop on all RDOs
    RDO_Collection_type::const_iterator  nextRDO  = RDO_Collection->begin();
    RDO_Collection_type::const_iterator  lastRDO  = RDO_Collection->end();
    RDO_Collection_type::const_iterator nextRDO1 = rdocoll->begin();
    RDO_Collection_type::const_iterator lastRDO1 = rdocoll->end();
    for (; nextRDO != lastRDO && nextRDO1 != lastRDO1 ; ++nextRDO, ++nextRDO1) {
      // Look for match
      if (((*nextRDO)->identify() != (*nextRDO1)->identify()) ||
          ((*nextRDO)->getWord() != (*nextRDO1)->getWord())) {

        ATH_MSG_ERROR("RDO's do NOT match: ids, word "
                      << m_pixelId->show_to_string((*nextRDO)->identify()) << " "
                      << m_pixelId->show_to_string((*nextRDO1)->identify()) << " "
                      << MSG::hex << (*nextRDO)->getWord() << " "
                      << MSG::hex << (*nextRDO1)->getWord() << " "
                     );
        error = true;
      }
      else {
        nmatch++;
      }
    }
    ATH_MSG_DEBUG("Matching RDOs " << nmatch );

    delete rdocoll;
  }

  // print out pixel element links
  // Retrive the element link vector
  const PixelRDOElemLinkVec* linkVec = nullptr;
  ATH_CHECK( evtStore()->retrieve(linkVec, "PixelRDOELs") );

  ATH_MSG_DEBUG("PixelRDOElemLinkVec found with "
                << linkVec->pixelRDOs().size()
                << "  RDOs " );
  for (unsigned int i = 0; i < linkVec->pixelRDOs().size(); ++i) {
    // Print out rdos from element link
    PixelRDOElemLinkVec::elem_type rdo  = linkVec->pixelRDOs()[i];
    PixelRDORawData* ncrdo = const_cast<PixelRDORawData*>(*rdo);
    ATH_MSG_DEBUG("RDO ID=" << m_pixelId->show_to_string((*rdo)->identify())
                  << "RDO ToT=" << ncrdo->getToT() );
  }


  if (error) {
    ATH_MSG_INFO("ERROR reading and checking Pixel RDO collections  " );
  }
  else {
    ATH_MSG_INFO("Successfully read and checked Pixel RDO collections  " );
  }

  return StatusCode::SUCCESS;
}

StatusCode RDOReaderDoubleSelector::checkSCT() const
{
  ATH_MSG_DEBUG("RDOReaderDoubleSelector::checkSCT()" );

  const SCT_RDO_Container* sctCont = nullptr;
  ATH_CHECK( evtStore()->retrieve(sctCont, "SCT_RDOs") );

  // loop on all RDO collections read in and check them
  int nmatch = 0;
  bool error = false;
  InDetRawDataFakeCreator creator;
  IdContext cntx = m_sctId->wafer_context();

  ATH_MSG_DEBUG("SCT_RDO_Container size "
                << sctCont->size() << " " );

  // Check for non-zero length of container
  if (sctCont->begin() == sctCont->end()) {
    ATH_MSG_ERROR("Container '" << "SCT_RDOs" << "' is EMPTY !" );
    return StatusCode::FAILURE;
  }

  for (SCT_RDO_Container::const_iterator it = sctCont->begin() ;
       it != sctCont->end() ; ++it) {

    const InDetRawDataCollection<SCT_RDORawData>* RDO_Collection(&(**it));

    ATH_MSG_DEBUG("Read RDO collection: size, ids "
                  << RDO_Collection->size() << " "
                  << m_sctId->show_to_string(RDO_Collection->identify(), &cntx) << " "
                  << MSG::hex << (unsigned int)RDO_Collection->identifyHash() << MSG::dec );

    // create a new sct RDO collection
    MsgStream log(msgSvc(), name());
    const InDetRawDataCollection<SCT_RDORawData> *rdocoll =
      creator.createSCT_RawDataColl(RDO_Collection->identifyHash(), m_sctId, msg());

    ATH_MSG_DEBUG("Test RDO collection: size, ids "
                  << rdocoll->size() << " "
                  << m_sctId->show_to_string(rdocoll->identify(), &cntx) << " "
                  << MSG::hex << (unsigned int)rdocoll->identifyHash() << MSG::dec );


    // Use one of the specific clustering AlgTools to make clusters
    typedef InDetRawDataCollection<SCT_RDORawData> RDO_Collection_type;

    // loop on all RDOs
    RDO_Collection_type::const_iterator  nextRDO  = RDO_Collection->begin();
    RDO_Collection_type::const_iterator  lastRDO  = RDO_Collection->end();
    RDO_Collection_type::const_iterator nextRDO1 = rdocoll->begin();
    RDO_Collection_type::const_iterator lastRDO1 = rdocoll->end();
    for (; nextRDO != lastRDO && nextRDO1 != lastRDO1 ; ++nextRDO, ++nextRDO1) {
      // Look for match
      if (((*nextRDO)->identify() != (*nextRDO1)->identify()) ||
          ((*nextRDO)->getGroupSize() != (*nextRDO1)->getGroupSize())) {

        ATH_MSG_ERROR("RDO's do NOT match: ids, group, strip "
                      << m_sctId->show_to_string((*nextRDO)->identify()) << " "
                      << m_sctId->show_to_string((*nextRDO1)->identify()) << " "
                      << MSG::hex
                      << (*nextRDO)->getGroupSize() << " "
                      << (*nextRDO1)->getGroupSize() << " "
                      << MSG::dec
                     );
        error = true;
      }
      else {
        nmatch++;
      }
    }
    ATH_MSG_DEBUG("Matching RDOs " << nmatch );

    delete rdocoll;
  }

  if (error) {
    ATH_MSG_INFO("ERROR reading and checking SCT RDO collections  " );
  }
  else {
    ATH_MSG_INFO("Successfully read and checked SCT RDO collections  " );
  }

  return StatusCode::SUCCESS;
}

StatusCode RDOReaderDoubleSelector::checkTRT() const
{
  ATH_MSG_DEBUG("RDOReaderDoubleSelector::checkTRT()" );

  const TRT_RDO_Container* trtCont = nullptr;
  ATH_CHECK( evtStore()->retrieve(trtCont, "TRT_RDOs") );

  // loop on all RDO collections read in and check them
  int nmatch = 0;
  bool error = false;
  InDetRawDataFakeCreator creator;
  IdContext cntx = m_trtId->straw_layer_context();

  ATH_MSG_DEBUG("TRT_RDO_Container size "
                << trtCont->size() << " " );

  // Check for non-zero length of container
  if (trtCont->begin() == trtCont->end()) {
    ATH_MSG_ERROR("Container '" << "TRT_RDOs"
                  << "' is EMPTY !" );
    return StatusCode::FAILURE;
  }

  for (TRT_RDO_Container::const_iterator it = trtCont->begin() ;
       it != trtCont->end() ; ++it) {

    const InDetRawDataCollection<TRT_RDORawData>* RDO_Collection(&(**it));

    ATH_MSG_DEBUG("Read RDO collection: size, ids "
                  << RDO_Collection->size() << " "
                  << m_trtId->show_to_string(RDO_Collection->identify(), &cntx) << " "
                  << MSG::hex << (unsigned int)RDO_Collection->identifyHash() << MSG::dec );

    // create a new trt RDO collection
    MsgStream log(msgSvc(), name());
    const InDetRawDataCollection<TRT_RDORawData> *rdocoll =
      creator.createTRT_RawDataColl(RDO_Collection->identifyHash(), m_trtId, msg());

    ATH_MSG_DEBUG("Test RDO collection: size, ids "
                  << rdocoll->size() << " "
                  << m_trtId->show_to_string(rdocoll->identify(), &cntx) << " "
                  << MSG::hex << (unsigned int)rdocoll->identifyHash() << MSG::dec );


    // Use one of the specific clustering AlgTools to make clusters
    typedef InDetRawDataCollection<TRT_RDORawData>   RDO_Collection_type;

    // loop on all RDOs
    RDO_Collection_type::const_iterator  nextRDO  = RDO_Collection->begin();
    RDO_Collection_type::const_iterator  lastRDO  = RDO_Collection->end();
    RDO_Collection_type::const_iterator nextRDO1 = rdocoll->begin();
    RDO_Collection_type::const_iterator lastRDO1 = rdocoll->end();
    for (; nextRDO != lastRDO && nextRDO1 != lastRDO1 ; ++nextRDO, ++nextRDO1) {
      // Look for match
      if (((*nextRDO)->identify() != (*nextRDO1)->identify()) ||
          ((*nextRDO)->highLevel() != (*nextRDO1)->highLevel()) ||
          ((*nextRDO)->timeOverThreshold() != (*nextRDO1)->timeOverThreshold()) ||
          ((*nextRDO)->driftTimeBin() != (*nextRDO1)->driftTimeBin())) {

        ATH_MSG_ERROR("RDO's do NOT match: ids, highlvl, TOT, drift "
                      << m_trtId->show_to_string((*nextRDO)->identify()) << " "
                      << m_trtId->show_to_string((*nextRDO1)->identify()) << " "
                      << MSG::hex
                      << (*nextRDO)->highLevel() << " "
                      << (*nextRDO1)->highLevel() << " "
                      << (*nextRDO)->timeOverThreshold() << " "
                      << (*nextRDO1)->timeOverThreshold() << " "
                      << (*nextRDO)->driftTimeBin() << " "
                      << (*nextRDO1)->driftTimeBin() << " "
                      << MSG::dec
                     );
        error = true;
      }
      else {
        nmatch++;
      }
    }
    ATH_MSG_DEBUG("Matching RDOs " << nmatch );

    delete rdocoll;
  }

  if (error) {
    ATH_MSG_INFO("ERROR reading and checking TRT RDO collections  " );
  }
  else {
    ATH_MSG_INFO("Successfully read and checked TRT RDO collections  " );
  }

  return StatusCode::SUCCESS;
}

StatusCode RDOReaderDoubleSelector::checkCells() const
{
  ATH_MSG_DEBUG("RDOReaderDoubleSelector::checkCells()" );

  // Retrieve container
  const CaloCellContainer* caloCont = nullptr;
  ATH_CHECK( evtStore()->retrieve(caloCont, "CaloCellCont1") );
  ATH_MSG_DEBUG( "Container '" << "CaloCellCont"
                 << "' retrieved from StoreGate"  );

  SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{m_caloMgrKey};
  ATH_CHECK(caloMgrHandle.isValid());

  LArCellContFakeCreator creator;

  // Create container
  MsgStream log(msgSvc(), name());
  const CaloCellContainer* caloCont1 = creator.createCaloCellContainer(m_calocellId,
                                       *caloMgrHandle,
                                       msg());

  CaloCellContainer::const_iterator first = caloCont->begin();
  CaloCellContainer::const_iterator last  = caloCont->end();
  CaloCellContainer::const_iterator first1 = caloCont1->begin();
  CaloCellContainer::const_iterator last1  = caloCont1->end();

  bool error = false;
  int ncells = 0;
  int ncellErrs = 0;

  for (; first != last && first1 != last1; ++first, ++first1) {
    if (((*first)->ID()    !=  (*first1)->ID()) ||
        (fabs((*first)->energy()  - (*first1)->energy()  ) > 0.01 * fabs((*first)->energy())) ||
        (fabs((*first)->time()    - (*first1)->time()    ) > 0.0002 * fabs((*first)->time())) ||
        (std::abs((*first)->quality() - (*first1)->quality() ) > 0) ||
        (std::abs((*first)->provenance() - (*first1)->provenance() ) > 0) ||
        (fabs((*first)->eta()     - (*first1)->eta()     ) > 0.0000001) ||
        (fabs((*first)->phi()     - (*first1)->phi()     ) > 0.0000001)) {
      ATH_MSG_ERROR("CaloCell1,2 differ: id "
                    << m_calocellId->show_to_string((*first)->ID())
                    << m_calocellId->show_to_string((*first1)->ID())
                    << " energy  " << (*first)->energy() << " "
                    << (*first1)->energy()
                    << " time    " << (*first)->time() << " "
                    << (*first1)->time()
                    << " quality " << (*first)->quality() << " "
                    << (*first1)->quality()
                    << " provenance " << (*first)->provenance() << " "
                    << (*first1)->provenance()
                    << " eta     " << (*first)->eta() << " "
                    << (*first1)->eta()
                    << " phi     " << (*first)->phi() << " "
                    << (*first1)->phi());
      error = true;
      ncellErrs++;
    }
    ncells++;
  }

  printCells(caloCont);

  if (error) {
    ATH_MSG_ERROR("Errors: " << ncellErrs );
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_INFO("Successful check of CaloCellContainer I/O. Cells read: " << ncells );
  }
  return StatusCode::SUCCESS;
}

// Print cells
void RDOReaderDoubleSelector::printCells(const CaloCellContainer* caloCont) const
{

  ATH_MSG_INFO("RDOReaderDoubleSelector::printRDOs()" );

  // loop on all cells

  CaloCellContainer::const_iterator first = caloCont->begin();
  CaloCellContainer::const_iterator last  = caloCont->end();

  for (; first != last; ++first) {

    ATH_MSG_DEBUG("CaloCell: id "
                  << m_calocellId->show_to_string((*first)->ID())
                  << std::setprecision(5)
                  << " energy  " << (*first)->energy()
                  << " time    " << (*first)->time()
                  << " quality " << (*first)->quality()
                  << " provenance " << (*first)->provenance()
                  << " eta     " << (*first)->eta()
                  << " phi     " << (*first)->phi()
                 );
  }

  ATH_MSG_DEBUG("RDOReaderDoubleSelector::printCells() end" );

}


// Finalize method:
StatusCode RDOReaderDoubleSelector::finalize()
{
  ATH_MSG_INFO("RDOReaderDoubleSelector::finalize()" );
  return StatusCode::SUCCESS;
}

