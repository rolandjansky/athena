/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonIdHelpersAlgs/TestMuonIdHelpers.h"

#include <algorithm>
#include <cmath>

TestMuonIdHelpers::TestMuonIdHelpers(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator) {

  m_testMDT = true;
  m_testCSC = true;
  m_testRPC = true;
  m_testTGC = true;

  // Declare the properties
  declareProperty ("testMDT", m_testMDT);
  declareProperty ("testCSC", m_testCSC);
  declareProperty ("testRPC", m_testRPC);
  declareProperty ("testTGC", m_testTGC);

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TestMuonIdHelpers::initialize(){

  ATH_MSG_DEBUG(" in initialize()");

  ATH_CHECK(m_idHelperSvc.retrieve());

  ATH_CHECK(m_mdtDigitContKey.initialize());
  ATH_CHECK(m_rpcDigitContKey.initialize());
  ATH_CHECK(m_cscDigitContKey.initialize());
  ATH_CHECK(m_tgcDigitContKey.initialize());

  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 


StatusCode TestMuonIdHelpers::execute() {

  ATH_MSG_DEBUG( "in execute()"  );

  if (m_testMDT) ATH_CHECK( testMdtIdHelper() );
  if (m_testCSC) ATH_CHECK( testCscIdHelper() );
  if (m_testRPC) ATH_CHECK( testRpcIdHelper() );
  if (m_testTGC) ATH_CHECK( testTgcIdHelper() );

  m_nTries       += 1;

  return StatusCode::SUCCESS;
}

StatusCode TestMuonIdHelpers::finalize() {
 
  ATH_MSG_DEBUG( "in finalize()"  );

   ATH_MSG_INFO( "ntries, user, kernel, elapsed "
             << m_nTries << " "
	     << m_deltaUser << " "
	     << m_deltaKernel << " "
	     << m_deltaElapsed );

   return StatusCode::SUCCESS;

}

StatusCode TestMuonIdHelpers::testMdtIdHelper() const {
  ATH_MSG_DEBUG( "in execute(): testing MDT IdHelper"  );

  // get the helper from the manager
 
  if (!m_idHelperSvc->hasMDT()) {
    ATH_MSG_ERROR( "cannot retrieve the MDT id helper from MuonDetDescrMgr"  );
    return StatusCode::FAILURE;
  }

  // retrieve data from StoreGate
  typedef MdtDigitContainer::const_iterator collection_iterator;
  typedef MdtDigitCollection::const_iterator digit_iterator;

  SG::ReadHandle<MdtDigitContainer> container(m_mdtDigitContKey);
  if (!container.isValid()) {
    ATH_MSG_ERROR("Could not find MdtDigitContainer called " << container.name() << " in store " << container.store());
    return StatusCode::FAILURE;
  }
  collection_iterator it1_coll= container->begin(); 
  collection_iterator it2_coll= container->end(); 

  ATH_MSG_INFO( "Start Testing MDT Ids"  );

  bool error = false;

  int nc1 = 0 ; 
  for (  ; it1_coll!=it2_coll; ++it1_coll) {
    const MdtDigitCollection* mdtCollection = *it1_coll; 
    ++nc1; 
    Identifier moduleId = mdtCollection->identify();
    if (!m_idHelperSvc->mdtIdHelper().validElement(moduleId)) {
      ATH_MSG_ERROR( "Invalid MDT module identifier " << m_idHelperSvc->mdtIdHelper().show_to_string(moduleId));
      return StatusCode::FAILURE;
    }
    digit_iterator it1_digit = mdtCollection->begin();
    digit_iterator it2_digit = mdtCollection->end();
    int nc2 = 0;
    for ( ; it1_digit!=it2_digit; ++it1_digit) {
      const MdtDigit* mdtDigit = *it1_digit;
      Identifier id = mdtDigit->identify();
      int testMDT = testMDTIds(id);
      if (testMDT != 0) error = true;
      ++nc2;
    }
    ATH_MSG_DEBUG("Number of valid digits in the collection " << m_idHelperSvc->mdtIdHelper().show_to_string(moduleId) 
                  <<" = " << nc2  );
  }
  ATH_MSG_DEBUG("  Number of MDT Collections  Accessed " << nc1  );

  if(error) {
    ATH_MSG_ERROR( "Check of MDT ids FAILED "  );
    return StatusCode::FAILURE;
  }
  else ATH_MSG_INFO( "Check of MDT ids OK "  );
 return StatusCode::SUCCESS;
}


StatusCode TestMuonIdHelpers::testCscIdHelper() const {
  ATH_MSG_DEBUG( "in execute(): testing CSC IdHelper"  );
  
  // get the helper from the manager
 
  if (!m_idHelperSvc->hasCSC()) {
    ATH_MSG_ERROR( "cannot retrieve the CSC id helper from MuonDetDescrMgr"  );
    return StatusCode::FAILURE;
  }

  // retrieve data from StoreGate
  typedef CscDigitContainer::const_iterator collection_iterator;
  typedef CscDigitCollection::const_iterator digit_iterator;

  SG::ReadHandle<CscDigitContainer> container(m_cscDigitContKey);
  if (!container.isValid()) {
    ATH_MSG_ERROR("Could not find CscDigitContainer called " << container.name() << " in store " << container.store());
    return StatusCode::FAILURE;
  }
  collection_iterator it1_coll= container->begin(); 
  collection_iterator it2_coll= container->end(); 

  ATH_MSG_INFO( "Start Testing CSC Ids"  );

  bool error = false;

  int nc1 = 0 ; 
  for (  ; it1_coll!=it2_coll; ++it1_coll) {
    const  CscDigitCollection* cscCollection = *it1_coll; 
    ++nc1; 
    Identifier moduleId = cscCollection->identify();
    if (!m_idHelperSvc->cscIdHelper().validElement(moduleId)) {
      ATH_MSG_ERROR( "Invalid CSC module identifier " << m_idHelperSvc->cscIdHelper().show_to_string(moduleId));
      return StatusCode::FAILURE;
    }
    digit_iterator it1_digit = cscCollection->begin();
    digit_iterator it2_digit = cscCollection->end();
    int nc2 = 0;
    for ( ; it1_digit!=it2_digit; ++it1_digit) {
      const CscDigit* cscDigit = *it1_digit;
      Identifier id = cscDigit->identify();
      int testCSC = testCSCIds(id);
      if (testCSC != 0) error = true;
      ++nc2;
    }
    ATH_MSG_DEBUG("Number of valid digits in the collection " << m_idHelperSvc->cscIdHelper().show_to_string(moduleId) 
                  <<" = " << nc2  );
  }
  ATH_MSG_DEBUG("Number of CSC Collections  Accessed " << nc1  );

  if(error) {
    ATH_MSG_ERROR( "Check of CSC ids FAILED "  );
    return StatusCode::FAILURE;
  } else ATH_MSG_INFO( "Check of CSC ids OK "  );
  return StatusCode::SUCCESS;
}

StatusCode TestMuonIdHelpers::testRpcIdHelper() const {
  ATH_MSG_DEBUG( "in execute(): testing RPC IdHelper"  );
  
  // get the helper from the manager
 
  if (!m_idHelperSvc->hasRPC()) {
    ATH_MSG_ERROR( "cannot retrieve the RPC id helper from MuonDetDescrMgr"  );
    return StatusCode::FAILURE;
  }

  // retrieve data from StoreGate
  typedef RpcDigitContainer::const_iterator collection_iterator;
  typedef RpcDigitCollection::const_iterator digit_iterator;

  SG::ReadHandle<RpcDigitContainer> container(m_rpcDigitContKey);
  if (!container.isValid()) {
    ATH_MSG_ERROR("Could not find RpcDigitContainer called " << container.name() << " in store " << container.store());
    return StatusCode::FAILURE;
  }
  collection_iterator it1_coll= container->begin(); 
  collection_iterator it2_coll= container->end(); 

  ATH_MSG_INFO( "Start Testing RPC Ids"  );

  bool error = false;

  int nc1 = 0 ; 
  for (  ; it1_coll!=it2_coll; ++it1_coll) {
    const  RpcDigitCollection* rpcCollection = *it1_coll; 
    ++nc1; 
    Identifier moduleId = rpcCollection->identify();
    if (!m_idHelperSvc->rpcIdHelper().validElement(moduleId)) {
      ATH_MSG_ERROR( "Invalid RPC module identifier " << m_idHelperSvc->rpcIdHelper().show_to_string(moduleId));
      return StatusCode::FAILURE;
    }
    digit_iterator it1_digit = rpcCollection->begin();
    digit_iterator it2_digit = rpcCollection->end();
    int nc2 = 0;
    for ( ; it1_digit!=it2_digit; ++it1_digit) {
      const RpcDigit* rpcDigit = *it1_digit;
      Identifier id = rpcDigit->identify();
      int testRPC = testRPCIds(id);
      if (testRPC != 0) error = true;
      ++nc2;
    }
    ATH_MSG_DEBUG("Number of valid digits in the collection " << m_idHelperSvc->rpcIdHelper().show_to_string(moduleId) 
                  <<" = " << nc2  );
  }
  ATH_MSG_DEBUG("  Number of RPC Collections  Accessed " << nc1  );

  if(error) {
    ATH_MSG_ERROR("Check of RPC ids FAILED");
    return StatusCode::FAILURE;
  }
  else ATH_MSG_INFO( "Check of RPC ids OK "  );
  
  return StatusCode::SUCCESS;
}

StatusCode TestMuonIdHelpers::testTgcIdHelper() const {
  ATH_MSG_DEBUG( "in execute(): testing TGC IdHelper"  );
  
  // get the helper from the manager
 
  if (!m_idHelperSvc->hasTGC()) {
    ATH_MSG_ERROR( "cannot retrieve the TGC id helper from MuonDetDescrMgr"  );
    return StatusCode::FAILURE;
  }

  // retrieve data from StoreGate
  typedef TgcDigitContainer::const_iterator collection_iterator;
  typedef TgcDigitCollection::const_iterator digit_iterator;

  SG::ReadHandle<TgcDigitContainer> container(m_tgcDigitContKey);
  if (!container.isValid()) {
    ATH_MSG_ERROR("Could not find TgcDigitContainer called " << container.name() << " in store " << container.store());
    return StatusCode::FAILURE;
  }
  collection_iterator it1_coll= container->begin(); 
  collection_iterator it2_coll= container->end(); 

  ATH_MSG_INFO( "Start Testing TGC Ids"  );

  bool error = false;

  int nc1 = 0 ; 
  for (  ; it1_coll!=it2_coll; ++it1_coll) {
    const  TgcDigitCollection* tgcCollection = *it1_coll; 
    ++nc1; 
    Identifier moduleId = tgcCollection->identify();
    if (!m_idHelperSvc->tgcIdHelper().validElement(moduleId)) {
      ATH_MSG_ERROR( "Invalid TGC module identifier " << m_idHelperSvc->tgcIdHelper().show_to_string(moduleId));
      return StatusCode::FAILURE;
    }
    digit_iterator it1_digit = tgcCollection->begin();
    digit_iterator it2_digit = tgcCollection->end();
    int nc2 = 0;
    for ( ; it1_digit!=it2_digit; ++it1_digit) {
      const TgcDigit* tgcDigit = *it1_digit;
      Identifier id = tgcDigit->identify();
      int testTGC = testTGCIds(id);
      if (testTGC != 0) error = true;
      ++nc2;
    }
    ATH_MSG_DEBUG("Number of valid digits in the collection " << m_idHelperSvc->tgcIdHelper().show_to_string(moduleId) 
                  <<" = " << nc2  );
  }
  ATH_MSG_DEBUG("Number of TGC Collections  Accessed " << nc1  );

  if(error) {
    ATH_MSG_ERROR( "Check of TGC ids FAILED ");
    return StatusCode::FAILURE;
  } else ATH_MSG_INFO( "Check of TGC ids OK ");
  
  return StatusCode::SUCCESS;
}

int TestMuonIdHelpers::testMDTIds(const Identifier& id) const {

  bool error = false;

  longlong startOfUserTime    = System::userTime( System::microSec );
  longlong startOfKernelTime  = System::kernelTime   ( System::microSec );
  longlong startOfElapsedTime = System::ellapsedTime ( System::microSec );

  IdContext   context = m_idHelperSvc->mdtIdHelper().channel_context();    

  m_idHelperSvc->mdtIdHelper().test_id(id,context);
  if (!m_idHelperSvc->mdtIdHelper().valid(id)) {
    ATH_MSG_ERROR( " testMDTIds invalid identifier "
                   << m_idHelperSvc->mdtIdHelper().show_to_string(id) );
    error = true;
  }
  int stationName = m_idHelperSvc->mdtIdHelper().stationName(id);
  std::string name = m_idHelperSvc->mdtIdHelper().stationNameString(stationName);
  if (m_idHelperSvc->mdtIdHelper().isBarrel(id) && name[0] != 'B') {
    ATH_MSG_ERROR( " testMDTIds problem in isBarrel() and stationName() "
                   << m_idHelperSvc->mdtIdHelper().show_to_string(id) );
    error = true;
  }
  if (m_idHelperSvc->mdtIdHelper().isEndcap(id) && name[0] == 'B') {
    ATH_MSG_ERROR( " testMDTIds problem in isEndCap() and stationName() "
                   << m_idHelperSvc->mdtIdHelper().show_to_string(id) );
    error = true;
  }
  if (m_idHelperSvc->mdtIdHelper().stationNameIndex(name) != m_idHelperSvc->mdtIdHelper().stationName(id)) {
    ATH_MSG_ERROR( " testMDTIds problem in stationNameIndex() and stationName() "
                   << m_idHelperSvc->mdtIdHelper().show_to_string(id) );
    error = true;
  }
  const int etaMin = m_idHelperSvc->mdtIdHelper().stationEtaMin(id);
  const int etaMax = m_idHelperSvc->mdtIdHelper().stationEtaMax(id);
  const int eta    = m_idHelperSvc->mdtIdHelper().stationEta(id);
  if (eta < etaMin || eta > etaMax) {
    ATH_MSG_ERROR( " testMDTIds problem in etaMin, etaMax or eta "
                   << "etaMin = " << etaMin
                   << "etaMax = " << etaMax
                   << "eta    = " << eta << " "
                   << m_idHelperSvc->mdtIdHelper().show_to_string(id) );
    error = true;
  }
  const int phiMin = m_idHelperSvc->mdtIdHelper().stationPhiMin(id);
  const int phiMax = m_idHelperSvc->mdtIdHelper().stationPhiMax(id);
  const int phi    = m_idHelperSvc->mdtIdHelper().stationPhi(id);
  if (phi < phiMin || phi > phiMax) {
    ATH_MSG_ERROR( " testMDTIds problem in phiMin, phiMax or phi "
                   << "phiMin = " << phiMin
                   << "phiMax = " << phiMax
                   << "phi    = " << phi << " "
                   << m_idHelperSvc->mdtIdHelper().show_to_string(id) );
    error = true;
  }
  const int technology = m_idHelperSvc->mdtIdHelper().technology(id);
  if (technology != m_idHelperSvc->mdtIdHelper().technologyIndex("MDT")) {
    ATH_MSG_ERROR( " testMDTIds problem in technology() "
                   << "technolog(id) = " << technology
                   << "technologyIndex = " << m_idHelperSvc->mdtIdHelper().technologyIndex("MDT")
                   << m_idHelperSvc->mdtIdHelper().show_to_string(id) );
    error = true;
  }
  if ("MDT" != m_idHelperSvc->mdtIdHelper().technologyString(0) || technology != 0) {
    ATH_MSG_ERROR( " testMDTIds problem in technology() "
	<< "technolog(id) = " << technology
	<< "technologyIndex = " << m_idHelperSvc->mdtIdHelper().technologyIndex("MDT")
	<< m_idHelperSvc->mdtIdHelper().show_to_string(id) );
    error = true;
  }
  const int multilayerMin = m_idHelperSvc->mdtIdHelper().multilayerMin(id);
  const int multilayerMax = m_idHelperSvc->mdtIdHelper().multilayerMax(id);
  const int multilayer    = m_idHelperSvc->mdtIdHelper().multilayer(id);
  if (multilayer < multilayerMin || multilayer > multilayerMax) {
    ATH_MSG_ERROR( " testMDTIds problem in multilayerMin, multilayerMax or multilayer "
                   << "multilayerMin = " << multilayerMin
                   << "multilayerMax = " << multilayerMax
                   << "multilayer    = " << multilayer << " "
                   << m_idHelperSvc->mdtIdHelper().show_to_string(id) );
    error = true;
  }
  const int tubeLayerMin = m_idHelperSvc->mdtIdHelper().tubeLayerMin(id);
  const int tubeLayerMax = m_idHelperSvc->mdtIdHelper().tubeLayerMax(id);
  const int tubeLayer    = m_idHelperSvc->mdtIdHelper().tubeLayer(id);
  if (tubeLayer < tubeLayerMin || tubeLayer > tubeLayerMax) {
    ATH_MSG_ERROR( " testMDTIds problem in tubeLayerMin, tubeLayerMax or tubeLayer "
                   << "tubeLayerMin = " << tubeLayerMin
                   << "tubeLayerMax = " << tubeLayerMax
                   << "tubeLayer    = " << tubeLayer << " "
                   << m_idHelperSvc->mdtIdHelper().show_to_string(id) );
    error = true;
  }

  const int tube    = m_idHelperSvc->mdtIdHelper().tube(id);
  const int tubeMin = m_idHelperSvc->mdtIdHelper().tubeMin(id);
  const int tubeMax = m_idHelperSvc->mdtIdHelper().tubeMax(id);
 
  if (tube < tubeMin || tube > tubeMax) {
    ATH_MSG_ERROR( " testMDTIds problem in tubeMin, tubeMax or tube "
                   << "tubeMin = " << tubeMin
                   << "tubeMax = " << tubeMax
                   << "tube    = " << tube << " "
                   << m_idHelperSvc->mdtIdHelper().show_to_string(id) );
    error = true;
  }

  Identifier oldChild  = m_idHelperSvc->mdtIdHelper().channelID(stationName,eta,phi,multilayer,tubeLayer,tube);
  if (!m_idHelperSvc->mdtIdHelper().valid(oldChild)) error = true; 
  Identifier oldParent = m_idHelperSvc->mdtIdHelper().parentID(oldChild);
  if (!m_idHelperSvc->mdtIdHelper().validElement(oldParent)) error = true;
  Identifier newParent = m_idHelperSvc->mdtIdHelper().elementID(stationName,eta,phi);
  if (!m_idHelperSvc->mdtIdHelper().validElement(newParent)) error = true;
  Identifier newChild  = m_idHelperSvc->mdtIdHelper().channelID(newParent,multilayer,tubeLayer,tube);
  if (!m_idHelperSvc->mdtIdHelper().valid(newChild)) error = true;

   /// following lines could be platform dependent!
  m_deltaUser    += System::userTime    ( System::microSec ) - startOfUserTime   ;
  m_deltaKernel  += System::kernelTime  ( System::microSec ) - startOfKernelTime ;
  m_deltaElapsed += System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

  if(error) {
    ATH_MSG_ERROR( "Check of MDT ids FAILED "  );
    return 1;
  }
  else {
    ATH_MSG_DEBUG( "Check of MDT ids OK "  );
  }

  return 0;
}

int TestMuonIdHelpers::testCSCIds(const Identifier& id) const {

  bool error = false;
 
  longlong startOfUserTime    = System::userTime( System::microSec );
  longlong startOfKernelTime  = System::kernelTime   ( System::microSec );
  longlong startOfElapsedTime = System::ellapsedTime ( System::microSec );

  IdContext   context = m_idHelperSvc->cscIdHelper().channel_context();   
 
  m_idHelperSvc->cscIdHelper().test_id(id,context);
  if (!m_idHelperSvc->cscIdHelper().valid(id)) {
    ATH_MSG_ERROR( " testCSCIds invalid identifier "
                   << m_idHelperSvc->cscIdHelper().show_to_string(id) );
    error = true;
  }
  int stationName = m_idHelperSvc->cscIdHelper().stationName(id);
  std::string name = m_idHelperSvc->cscIdHelper().stationNameString(stationName);
  if (m_idHelperSvc->cscIdHelper().isEndcap(id) && name[0] != 'C') {
    ATH_MSG_ERROR( " testCSCIds problem in isEndcap() and stationName() "
                   << m_idHelperSvc->cscIdHelper().show_to_string(id) );
    error = true;
  }
  if (!m_idHelperSvc->cscIdHelper().isBarrel(id) && name[0] != 'C') {
    ATH_MSG_ERROR( " testCSCIds problem in isBarrel() and stationName() "
	<< m_idHelperSvc->cscIdHelper().show_to_string(id) );
    error = true;
  }
  if (m_idHelperSvc->cscIdHelper().stationNameIndex(name) != m_idHelperSvc->cscIdHelper().stationName(id)) {
    ATH_MSG_ERROR( " testCSCIds problem in stationNameIndex() and stationName() "
                   << m_idHelperSvc->cscIdHelper().show_to_string(id) );
    error = true;
  }
  const int etaMin = m_idHelperSvc->cscIdHelper().stationEtaMin(id);
  const int etaMax = m_idHelperSvc->cscIdHelper().stationEtaMax(id);
  const int eta    = m_idHelperSvc->cscIdHelper().stationEta(id);
  if (eta < etaMin || eta > etaMax) {
    ATH_MSG_ERROR( " testCSCIds problem in etaMin, etaMax or eta "
                   << "etaMin = " << etaMin
                   << "etaMax = " << etaMax
                   << "eta    = " << eta << " "
                   << m_idHelperSvc->cscIdHelper().show_to_string(id) );
    error = true;
  }
  const int phiMin = m_idHelperSvc->cscIdHelper().stationPhiMin(id);
  const int phiMax = m_idHelperSvc->cscIdHelper().stationPhiMax(id);
  const int phi    = m_idHelperSvc->cscIdHelper().stationPhi(id);
  if (phi < phiMin || phi > phiMax) {
    ATH_MSG_ERROR( " testCSCIds problem in phiMin, phiMax or phi "
                   << "phiMin = " << phiMin
                   << "phiMax = " << phiMax
                   << "phi    = " << phi << " "
                   << m_idHelperSvc->cscIdHelper().show_to_string(id) );
    error = true;
  }
  const int technology = m_idHelperSvc->cscIdHelper().technology(id);
  if (technology != m_idHelperSvc->cscIdHelper().technologyIndex("CSC")) {
    ATH_MSG_ERROR( " testCSCIds problem in technology() "
                   << "technolog(id) = " << technology
                   << "technologyIndex = " << m_idHelperSvc->cscIdHelper().technologyIndex("CSC")
                   << m_idHelperSvc->cscIdHelper().show_to_string(id) );
    error = true;
  }
  if ("CSC" != m_idHelperSvc->cscIdHelper().technologyString(1) || technology != 1) {
    ATH_MSG_ERROR( " testCSCIds problem in technology() "
                   << "technolog(id) = " << technology
                   << "technologyIndex = " << m_idHelperSvc->cscIdHelper().technologyIndex("CSC")
                   << m_idHelperSvc->cscIdHelper().show_to_string(id) );
    error = true;
  }
  const int chamberLayerMin = m_idHelperSvc->cscIdHelper().chamberLayerMin(id);
  const int chamberLayerMax = m_idHelperSvc->cscIdHelper().chamberLayerMax(id);
  const int chamberLayer    = m_idHelperSvc->cscIdHelper().chamberLayer(id);
  if (chamberLayer < chamberLayerMin || chamberLayer > chamberLayerMax) {
    ATH_MSG_ERROR( " testCSCIds problem in chamberLayerMin, chamberLayerMax or chamberLayer "
                   << "chamberLayerMin = " << chamberLayerMin
                   << "chamberLayerMax = " << chamberLayerMax
                   << "chamberLayer    = " << chamberLayer << " "
                   << m_idHelperSvc->cscIdHelper().show_to_string(id) );
    error = true;
  }
  const int wireLayerMin = m_idHelperSvc->cscIdHelper().wireLayerMin(id);
  const int wireLayerMax = m_idHelperSvc->cscIdHelper().wireLayerMax(id);
  const int wireLayer    = m_idHelperSvc->cscIdHelper().wireLayer(id);
  if (wireLayer < wireLayerMin || wireLayer > wireLayerMax) {
    ATH_MSG_ERROR( " testCSCIds problem in wireLayerMin, wireLayerMax or wireLayer "
                   << "wireLayerMin = " << wireLayerMin
                   << "wireLayerMax = " << wireLayerMax
                   << "wireLayer    = " << wireLayer << " "
                   << m_idHelperSvc->cscIdHelper().show_to_string(id) );
    error = true;
  }
  const int measuresPhiMin = m_idHelperSvc->cscIdHelper().measuresPhiMin(id);
  const int measuresPhiMax = m_idHelperSvc->cscIdHelper().measuresPhiMax(id);
  const int measuresPhi    = m_idHelperSvc->cscIdHelper().measuresPhi(id);
  if (measuresPhi < measuresPhiMin || measuresPhi > measuresPhiMax) {
    ATH_MSG_ERROR( " testCSCIds problem in measuresPhiMin, measuresPhiMax or measuresPhi "
                   << "measuresPhiMin = " << measuresPhiMin
                   << "measuresPhiMax = " << measuresPhiMax
                   << "measuresPhi    = " << measuresPhi << " "
                   << m_idHelperSvc->cscIdHelper().show_to_string(id) );
    error = true;
  }
  const int stripMin = m_idHelperSvc->cscIdHelper().stripMin(id);
  const int stripMax = m_idHelperSvc->cscIdHelper().stripMax(id);
  const int strip    = m_idHelperSvc->cscIdHelper().strip(id);
  if (strip < stripMin || strip > stripMax) {
    ATH_MSG_ERROR( " testCSCIds problem in stripMin, stripMax or strip "
                   << "stripMin = " << stripMin
                   << "stripMax = " << stripMax
                   << "strip    = " << strip << " "
                   << m_idHelperSvc->cscIdHelper().show_to_string(id) );
    error = true;
  }

  Identifier oldChild  = m_idHelperSvc->cscIdHelper().channelID(stationName,eta,phi,chamberLayer,wireLayer,measuresPhi,strip);
  if (!m_idHelperSvc->cscIdHelper().valid(oldChild)) error = true;
  Identifier oldParent = m_idHelperSvc->cscIdHelper().parentID(oldChild);
  if (!m_idHelperSvc->cscIdHelper().validElement(oldParent)) error = true;
  Identifier newParent = m_idHelperSvc->cscIdHelper().elementID(stationName,eta,phi);
  if (!m_idHelperSvc->cscIdHelper().validElement(newParent)) error = true;
  Identifier newChild  = m_idHelperSvc->cscIdHelper().channelID(newParent,chamberLayer,wireLayer,measuresPhi,strip);
  if (!m_idHelperSvc->cscIdHelper().valid(newChild)) error = true;

  /// following lines could be platform dependent!
  m_deltaUser    += System::userTime    ( System::microSec ) - startOfUserTime   ;
  m_deltaKernel  += System::kernelTime  ( System::microSec ) - startOfKernelTime ;
  m_deltaElapsed += System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

  if(error) {
    ATH_MSG_ERROR( "Check of CSC ids FAILED "  );
    return 1;
  }
  else {
    ATH_MSG_DEBUG( "Check of CSC ids OK "  );

  }

  return 0;
}

int TestMuonIdHelpers::testRPCIds(const Identifier& id) const {

  bool error = false;
  
  longlong startOfUserTime    = System::userTime( System::microSec );
  longlong startOfKernelTime  = System::kernelTime   ( System::microSec );
  longlong startOfElapsedTime = System::ellapsedTime ( System::microSec );

  IdContext    context = m_idHelperSvc->rpcIdHelper().channel_context();    

  m_idHelperSvc->rpcIdHelper().test_id(id,context);
  if (!m_idHelperSvc->rpcIdHelper().valid(id)) {
    ATH_MSG_ERROR( " testRPCIds invalid identifier "
                   << m_idHelperSvc->rpcIdHelper().show_to_string(id) );
    error = true;
  }
  int stationName = m_idHelperSvc->rpcIdHelper().stationName(id);
  std::string name = m_idHelperSvc->rpcIdHelper().stationNameString(stationName);
  if (m_idHelperSvc->rpcIdHelper().isBarrel(id) && name[0] != 'B') {
    ATH_MSG_ERROR( " testRPCIds problem in isBarrel() and stationName() "
                   << m_idHelperSvc->rpcIdHelper().show_to_string(id) );
    error = true;
  }
  if (!m_idHelperSvc->rpcIdHelper().isEndcap(id) && name[0] != 'B') {
    ATH_MSG_ERROR( " testRPCIds problem in isEndCap() and stationName() "
                   << m_idHelperSvc->rpcIdHelper().show_to_string(id) );
    error = true;
  }
  if (m_idHelperSvc->rpcIdHelper().stationNameIndex(name) != m_idHelperSvc->rpcIdHelper().stationName(id)) {
    ATH_MSG_ERROR( " testRPCIds problem in stationNameIndex() and stationName() "
                   << m_idHelperSvc->rpcIdHelper().show_to_string(id) );
    error = true;
  }
  const int etaMin = m_idHelperSvc->rpcIdHelper().stationEtaMin(id);
  const int etaMax = m_idHelperSvc->rpcIdHelper().stationEtaMax(id);
  const int eta    = m_idHelperSvc->rpcIdHelper().stationEta(id);
  if (eta < etaMin || eta > etaMax) {
    ATH_MSG_ERROR( " testRPCIds problem in etaMin, etaMax or eta "
                   << "etaMin = " << etaMin
                   << "etaMax = " << etaMax
                   << "eta    = " << eta << " "
                   << m_idHelperSvc->rpcIdHelper().show_to_string(id) );
    error = true;
  }
  const int phiMin = m_idHelperSvc->rpcIdHelper().stationPhiMin(id);
  const int phiMax = m_idHelperSvc->rpcIdHelper().stationPhiMax(id);
  const int phi    = m_idHelperSvc->rpcIdHelper().stationPhi(id);
  if (phi < phiMin || phi > phiMax) {
    ATH_MSG_ERROR( " testRPCIds problem in phiMin, phiMax or phi "
                   << "phiMin = " << phiMin
                   << "phiMax = " << phiMax
                   << "phi    = " << phi << " "
                   << m_idHelperSvc->rpcIdHelper().show_to_string(id) );
    error = true;
  }
  const int technology = m_idHelperSvc->rpcIdHelper().technology(id);
  if (technology != m_idHelperSvc->rpcIdHelper().technologyIndex("RPC")) {
    ATH_MSG_ERROR( " testRPCIds problem in technology() "
                   << "technolog(id) = " << technology
                   << "technologyIndex = " << m_idHelperSvc->rpcIdHelper().technologyIndex("RPC")
                   << m_idHelperSvc->rpcIdHelper().show_to_string(id) );
    error = true;
  }
  if ("RPC" != m_idHelperSvc->rpcIdHelper().technologyString(2) || technology != 2) {
    ATH_MSG_ERROR( " testRPCIds problem in technology() "
                   << "technolog(id) = " << technology
                   << "technologyIndex = " << m_idHelperSvc->rpcIdHelper().technologyIndex("RPC")
                   << m_idHelperSvc->rpcIdHelper().show_to_string(id) );
    error = true;
  }
  const int doubletRMin = m_idHelperSvc->rpcIdHelper().doubletRMin(id);
  const int doubletRMax = m_idHelperSvc->rpcIdHelper().doubletRMax(id);
  const int doubletR    = m_idHelperSvc->rpcIdHelper().doubletR(id);
  if (doubletR < doubletRMin || doubletR > doubletRMax) {
    ATH_MSG_ERROR( " testRPCIds problem in doubletRMin, doubletRMax or doubletR "
                   << "doubletRMin = " << doubletRMin
                   << "doubletRMax = " << doubletRMax
                   << "doubletR    = " << doubletR << " "
                   << m_idHelperSvc->rpcIdHelper().show_to_string(id) );
    error = true;
  }
  const int doubletZMin = m_idHelperSvc->rpcIdHelper().doubletZMin(id);
  const int doubletZMax = m_idHelperSvc->rpcIdHelper().doubletZMax(id);
  const int doubletZ    = m_idHelperSvc->rpcIdHelper().doubletZ(id);
  if (doubletZ < doubletZMin || doubletZ > doubletZMax) {
    ATH_MSG_ERROR( " testRPCIds problem in doubletZMin, doubletZMax or doubletZ "
                   << "doubletZMin = " << doubletZMin
                   << "doubletZMax = " << doubletZMax
                   << "doubletZ    = " << doubletZ << " "
                   << m_idHelperSvc->rpcIdHelper().show_to_string(id) );
    error = true;
  }
  const int doubletPhiMin = m_idHelperSvc->rpcIdHelper().doubletPhiMin(id);
  const int doubletPhiMax = m_idHelperSvc->rpcIdHelper().doubletPhiMax(id);
  const int doubletPhi    = m_idHelperSvc->rpcIdHelper().doubletPhi(id);
  if (doubletPhi < doubletPhiMin || doubletPhi > doubletPhiMax) {
    ATH_MSG_ERROR( " testRPCIds problem in doubletPhiMin, doubletPhiMax or doubletPhi "
                   << "doubletPhiMin = " << doubletPhiMin
                   << "doubletPhiMax = " << doubletPhiMax
                   << "doubletPhi    = " << doubletPhi << " "
                   << m_idHelperSvc->rpcIdHelper().show_to_string(id) );
    error = true;
  }
  const int gasGapMin = m_idHelperSvc->rpcIdHelper().gasGapMin(id);
  const int gasGapMax = m_idHelperSvc->rpcIdHelper().gasGapMax(id);
  const int gasGap    = m_idHelperSvc->rpcIdHelper().gasGap(id);
  if (gasGap < gasGapMin || gasGap > gasGapMax) {
    ATH_MSG_ERROR( " testRPCIds problem in gasGapMin, gasGapMax or gasGap "
                   << "gasGapMin = " << gasGapMin
                   << "gasGapMax = " << gasGapMax
                   << "gasGap    = " << gasGap << " "
                   << m_idHelperSvc->rpcIdHelper().show_to_string(id) );
    error = true;
  }
  const int measuresPhiMin = m_idHelperSvc->rpcIdHelper().measuresPhiMin(id);
  const int measuresPhiMax = m_idHelperSvc->rpcIdHelper().measuresPhiMax(id);
  const int measuresPhi    = m_idHelperSvc->rpcIdHelper().measuresPhi(id);
  if (measuresPhi < measuresPhiMin || measuresPhi > measuresPhiMax) {
    ATH_MSG_ERROR( " testRPCIds problem in measuresPhiMin, measuresPhiMax or measuresPhi "
                   << "measuresPhiMin = " << measuresPhiMin
                   << "measuresPhiMax = " << measuresPhiMax
                   << "measuresPhi    = " << measuresPhi << " "
                   << m_idHelperSvc->rpcIdHelper().show_to_string(id) );
    error = true;
  }
  const int stripMin = m_idHelperSvc->rpcIdHelper().stripMin(id);
  const int stripMax = m_idHelperSvc->rpcIdHelper().stripMax(id);
  const int strip    = m_idHelperSvc->rpcIdHelper().strip(id);
  if (strip < stripMin || strip > stripMax) {
    ATH_MSG_ERROR( " testRPCIds problem in stripMin, stripMax or strip "
                   << "stripMin = " << stripMin
                   << "stripMax = " << stripMax
                   << "strip    = " << strip << " "
                   << m_idHelperSvc->rpcIdHelper().show_to_string(id) );
    error = true;
  }

  Identifier oldChild  = m_idHelperSvc->rpcIdHelper().channelID(stationName,eta,phi,doubletR,
					    doubletZ,doubletPhi,gasGap,measuresPhi,strip);
  if (!m_idHelperSvc->rpcIdHelper().valid(oldChild)) error = true;
  Identifier oldParent = m_idHelperSvc->rpcIdHelper().parentID(oldChild);
  if (!m_idHelperSvc->rpcIdHelper().validElement(oldParent)) error = true;
  Identifier newParent = m_idHelperSvc->rpcIdHelper().elementID(stationName,eta,phi,doubletR);
  if (!m_idHelperSvc->rpcIdHelper().validElement(newParent)) error = true;
  Identifier newChild  = m_idHelperSvc->rpcIdHelper().channelID(newParent,doubletZ,doubletPhi,gasGap,measuresPhi,strip);
  if (!m_idHelperSvc->rpcIdHelper().valid(newChild)) error = true;

  /// following lines could be platform dependent!
  m_deltaUser    += System::userTime    ( System::microSec ) - startOfUserTime   ;
  m_deltaKernel  += System::kernelTime  ( System::microSec ) - startOfKernelTime ;
  m_deltaElapsed += System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

  if(error) {
    ATH_MSG_ERROR( "Check of RPC ids FAILED "  );
    return 1;
  }
  else {
    ATH_MSG_DEBUG( "Check of RPC ids OK "  );
  }
  
  return 0;
}

int TestMuonIdHelpers::testTGCIds(const Identifier& id) const {

  bool error = false;
 
  longlong startOfUserTime    = System::userTime( System::microSec );
  longlong startOfKernelTime  = System::kernelTime   ( System::microSec );
  longlong startOfElapsedTime = System::ellapsedTime ( System::microSec );

  IdContext    context = m_idHelperSvc->tgcIdHelper().channel_context();    

  m_idHelperSvc->tgcIdHelper().test_id(id,context);
  if (!m_idHelperSvc->tgcIdHelper().valid(id)) {
    ATH_MSG_ERROR( " testTGCIds invalid identifier "
                   << m_idHelperSvc->tgcIdHelper().show_to_string(id) );
    error = true;
  }
  int stationName = m_idHelperSvc->tgcIdHelper().stationName(id);
  std::string name = m_idHelperSvc->tgcIdHelper().stationNameString(stationName);
  if (!m_idHelperSvc->tgcIdHelper().isForward(id) && name[2] != 'E') {
    ATH_MSG_ERROR( " testTGCIds problem in isEndcap() and stationName() "
                   << m_idHelperSvc->tgcIdHelper().show_to_string(id) );
    error = true;
  }
  if (m_idHelperSvc->tgcIdHelper().isForward(id) && name[2] != 'F') {
    ATH_MSG_ERROR( " testTGCIds problem in isForward() and stationName() "
                   << m_idHelperSvc->tgcIdHelper().show_to_string(id) );
    error = true;
  }
  if (m_idHelperSvc->tgcIdHelper().stationNameIndex(name) != m_idHelperSvc->tgcIdHelper().stationName(id)) {
    ATH_MSG_ERROR( " testTGCIds problem in stationNameIndex() and stationName() "
                   << m_idHelperSvc->tgcIdHelper().show_to_string(id) );
    error = true;
  }
  const int etaMin = m_idHelperSvc->tgcIdHelper().stationEtaMin(id);
  const int etaMax = m_idHelperSvc->tgcIdHelper().stationEtaMax(id);
  const int eta    = m_idHelperSvc->tgcIdHelper().stationEta(id);
  if (eta < etaMin || eta > etaMax) {
    ATH_MSG_ERROR( " testTGCIds problem in etaMin, etaMax or eta "
                   << "etaMin = " << etaMin
                   << "etaMax = " << etaMax
                   << "eta    = " << eta << " "
                   << m_idHelperSvc->tgcIdHelper().show_to_string(id) );
    error = true;
  }
  const int phiMin = m_idHelperSvc->tgcIdHelper().stationPhiMin(id);
  const int phiMax = m_idHelperSvc->tgcIdHelper().stationPhiMax(id);
  const int phi    = m_idHelperSvc->tgcIdHelper().stationPhi(id);
  if (phi < phiMin || phi > phiMax) {
    ATH_MSG_ERROR( " testTGCIds problem in phiMin, phiMax or phi "
                   << "phiMin = " << phiMin
                   << "phiMax = " << phiMax
                   << "phi    = " << phi << " "
                   << m_idHelperSvc->tgcIdHelper().show_to_string(id) );
    error = true;
  }
  const int technology = m_idHelperSvc->tgcIdHelper().technology(id);
  if (technology != m_idHelperSvc->tgcIdHelper().technologyIndex("TGC")) {
    ATH_MSG_ERROR( " testTGCIds problem in technology() "
                   << "technolog(id) = " << technology
                   << "technologyIndex = " << m_idHelperSvc->tgcIdHelper().technologyIndex("TGC")
                   << m_idHelperSvc->tgcIdHelper().show_to_string(id) );
    error = true;
  }
  if ("TGC" != m_idHelperSvc->tgcIdHelper().technologyString(3) || technology != 3) {
    ATH_MSG_ERROR( " testTGCIds problem in technology() "
                   << "technolog(id) = " << technology
                   << "technologyIndex = " << m_idHelperSvc->tgcIdHelper().technologyIndex("TGC")
                   << m_idHelperSvc->tgcIdHelper().show_to_string(id) );
    error = true;
  }
  const int gasGapMin = m_idHelperSvc->tgcIdHelper().gasGapMin(id);
  const int gasGapMax = m_idHelperSvc->tgcIdHelper().gasGapMax(id);
  const int gasGap    = m_idHelperSvc->tgcIdHelper().gasGap(id);
  if (gasGap < gasGapMin || gasGap > gasGapMax) {
    ATH_MSG_ERROR( " testTGCIds problem in gasGapMin, gasGapMax or gasGap "
                   << "gasGapMin = " << gasGapMin
                   << "gasGapMax = " << gasGapMax
                   << "gasGap    = " << gasGap << " "
                   << m_idHelperSvc->tgcIdHelper().show_to_string(id) );
    error = true;
  }
  const int isStripMin = m_idHelperSvc->tgcIdHelper().isStripMin(id);
  const int isStripMax = m_idHelperSvc->tgcIdHelper().isStripMax(id);
  const int isStrip    = m_idHelperSvc->tgcIdHelper().isStrip(id);
  if (isStrip < isStripMin || isStrip > isStripMax) {
    ATH_MSG_ERROR( " testTGCIds problem in isStripMin, isStripMax or isStrip "
                   << "isStripMin = " << isStripMin
                   << "isStripMax = " << isStripMax
                   << "isStrip    = " << isStrip << " "
                   << m_idHelperSvc->tgcIdHelper().show_to_string(id) );
    error = true;
  }
  const int channelMin = m_idHelperSvc->tgcIdHelper().channelMin(id);
  const int channelMax = m_idHelperSvc->tgcIdHelper().channelMax(id);
  const int channel    = m_idHelperSvc->tgcIdHelper().channel(id);
  if (channel < channelMin || channel > channelMax) {
    ATH_MSG_ERROR( " testTGCIds problem in channelMin, channelMax or channel "
                   << "channelMin = " << channelMin
                   << "channelMax = " << channelMax
                   << "channel    = " << channel << " "
                   << m_idHelperSvc->tgcIdHelper().show_to_string(id) );
    error = true;
  }
  
  Identifier oldChild  = m_idHelperSvc->tgcIdHelper().channelID(stationName,eta,phi,gasGap,isStrip,channel);
  if (!m_idHelperSvc->tgcIdHelper().valid(oldChild)) error = true;
  Identifier oldParent = m_idHelperSvc->tgcIdHelper().parentID(oldChild);
  if (!m_idHelperSvc->tgcIdHelper().validElement(oldParent)) error = true;
  Identifier newParent = m_idHelperSvc->tgcIdHelper().elementID(stationName,eta,phi);
  if (!m_idHelperSvc->tgcIdHelper().validElement(newParent)) error = true;
  Identifier newChild  = m_idHelperSvc->tgcIdHelper().channelID(newParent,gasGap,isStrip,channel);
  if (!m_idHelperSvc->tgcIdHelper().valid(newChild)) error = true;

  /// following lines could be platform dependent!
  m_deltaUser    += System::userTime    ( System::microSec ) - startOfUserTime   ;
  m_deltaKernel  += System::kernelTime  ( System::microSec ) - startOfKernelTime ;
  m_deltaElapsed += System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

  if(error) {
    ATH_MSG_ERROR( "Check of TGC ids FAILED "  );
    return 1;
  }
  else {
    ATH_MSG_DEBUG( "Check of TGC ids OK "  );
  }

  return 0;
}

