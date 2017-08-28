/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h"

#include "MuonDigitContainer/MdtDigitContainer.h"
#include "MuonDigitContainer/MdtDigitCollection.h"
#include "MuonDigitContainer/MdtDigit.h"
#include "MuonIdHelpers/MdtIdHelper.h"

#include "MuonDigitContainer/CscDigitContainer.h"
#include "MuonDigitContainer/CscDigitCollection.h"
#include "MuonDigitContainer/CscDigit.h"
#include "MuonIdHelpers/CscIdHelper.h"

#include "MuonDigitContainer/RpcDigitContainer.h"
#include "MuonDigitContainer/RpcDigitCollection.h"
#include "MuonDigitContainer/RpcDigit.h"
#include "MuonIdHelpers/RpcIdHelper.h"

#include "MuonDigitContainer/TgcDigitContainer.h"
#include "MuonDigitContainer/TgcDigitCollection.h"
#include "MuonDigitContainer/TgcDigit.h"
#include "MuonIdHelpers/TgcIdHelper.h"

#include "MuonIdHelpersAlgs/TestMuonIdHelpers.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include <algorithm>
#include <cmath>

using namespace std;


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

TestMuonIdHelpers::TestMuonIdHelpers(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_activeStore("ActiveStoreSvc", name),
  m_mdtId(0), m_cscId(0),
  m_rpcId(0), m_tgcId(0), m_muon_mgr(0),
  m_deltaUser(0), m_deltaKernel(0), m_deltaElapsed(0), m_nTries(0)
  
   {

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

TestMuonIdHelpers::~TestMuonIdHelpers()
{}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TestMuonIdHelpers::initialize(){

  ATH_MSG_DEBUG( " in initialize()"  );

  ATH_CHECK( m_activeStore.retrieve() );

  ATH_CHECK( detStore()->retrieve( m_muon_mgr ) );
  m_mdtId = m_muon_mgr->mdtIdHelper();
  m_cscId = m_muon_mgr->cscIdHelper();
  m_rpcId = m_muon_mgr->rpcIdHelper();
  m_tgcId = m_muon_mgr->tgcIdHelper();

  m_deltaUser    = 0;
  m_deltaKernel  = 0;
  m_deltaElapsed = 0;
  m_nTries       = 0;
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

   std::cout << "ntries, user, kernel, elapsed "
             << m_nTries << " "
	     << m_deltaUser << " "
	     << m_deltaKernel << " "
	     << m_deltaElapsed << " "
	     << std::endl;

   return StatusCode::SUCCESS;

}

StatusCode TestMuonIdHelpers::testMdtIdHelper() {
  ATH_MSG_DEBUG( "in execute(): testing MDT IdHelper"  );

  // get the helper from the manager
 
  if (!m_mdtId) {
    ATH_MSG_ERROR( "cannot retrieve the MDT id helper from MuonDetDescrMgr"  );
    return StatusCode::FAILURE;
  }

  // retrieve data from StoreGate
  typedef MdtDigitContainer::const_iterator collection_iterator;
  typedef MdtDigitCollection::const_iterator digit_iterator;
  
  string key = "MDT_DIGITS";
  const DataHandle <MdtDigitContainer> container;
  ATH_CHECK( (*m_activeStore)->retrieve(container,key) );
  collection_iterator it1_coll= container->begin(); 
  collection_iterator it2_coll= container->end(); 

  ATH_MSG_INFO( "Start Testing MDT Ids"  );

  bool error = false;

  int nc1 = 0 ; 
  for (  ; it1_coll!=it2_coll; ++it1_coll) {
    const  MdtDigitCollection* mdtCollection = *it1_coll; 
    ++nc1; 
    Identifier moduleId = mdtCollection->identify();
    if (!m_mdtId->validElement(moduleId)) {
      ATH_MSG_ERROR( "Invalid MDT module identifier " << m_mdtId->show_to_string(moduleId)  );
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
    ATH_MSG_DEBUG("Number of valid digits in the collection " << m_mdtId->show_to_string(moduleId) 
                  <<" = " << nc2  );
  }
  ATH_MSG_DEBUG("  Number of MDT Collections  Accessed " << nc1  );

  if(error) ATH_MSG_ERROR( "Check of MDT ids FAILED "  );
  else      ATH_MSG_INFO( "Check of MDT ids OK "  );
  
 return StatusCode::SUCCESS;
}


StatusCode TestMuonIdHelpers::testCscIdHelper() {
  ATH_MSG_DEBUG( "in execute(): testing CSC IdHelper"  );
  
  // get the helper from the manager
 
  if (!m_cscId) {
    ATH_MSG_ERROR( "cannot retrieve the CSC id helper from MuonDetDescrMgr"  );
    return StatusCode::FAILURE;
  }

  // retrieve data from StoreGate
  typedef CscDigitContainer::const_iterator collection_iterator;
  typedef CscDigitCollection::const_iterator digit_iterator;
  
  string key = "CSC_DIGITS";
  const DataHandle <CscDigitContainer> container;
  ATH_CHECK( (*m_activeStore)->retrieve(container,key) );
  collection_iterator it1_coll= container->begin(); 
  collection_iterator it2_coll= container->end(); 

  ATH_MSG_INFO( "Start Testing CSC Ids"  );

  bool error = false;

  int nc1 = 0 ; 
  for (  ; it1_coll!=it2_coll; ++it1_coll) {
    const  CscDigitCollection* cscCollection = *it1_coll; 
    ++nc1; 
    Identifier moduleId = cscCollection->identify();
    if (!m_cscId->validElement(moduleId)) {
      ATH_MSG_ERROR( "Invalid CSC module identifier " << m_cscId->show_to_string(moduleId)  );
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
    ATH_MSG_DEBUG("Number of valid digits in the collection " << m_cscId->show_to_string(moduleId) 
                  <<" = " << nc2  );
  }
  ATH_MSG_DEBUG("Number of CSC Collections  Accessed " << nc1  );

  if(error) ATH_MSG_ERROR( "Check of CSC ids FAILED "  );
  else      ATH_MSG_INFO( "Check of CSC ids OK "  );
  
  return StatusCode::SUCCESS;
}

StatusCode TestMuonIdHelpers::testRpcIdHelper() {
  ATH_MSG_DEBUG( "in execute(): testing RPC IdHelper"  );
  
  // get the helper from the manager
 
  if (!m_rpcId) {
    ATH_MSG_ERROR( "cannot retrieve the RPC id helper from MuonDetDescrMgr"  );
    return StatusCode::FAILURE;
  }

  // retrieve data from StoreGate
  typedef RpcDigitContainer::const_iterator collection_iterator;
  typedef RpcDigitCollection::const_iterator digit_iterator;
  
  string key = "RPC_DIGITS";
  const DataHandle <RpcDigitContainer> container;
  ATH_CHECK( (*m_activeStore)->retrieve(container,key) );
  collection_iterator it1_coll= container->begin(); 
  collection_iterator it2_coll= container->end(); 

  ATH_MSG_INFO( "Start Testing RPC Ids"  );

  bool error = false;

  int nc1 = 0 ; 
  for (  ; it1_coll!=it2_coll; ++it1_coll) {
    const  RpcDigitCollection* rpcCollection = *it1_coll; 
    ++nc1; 
    Identifier moduleId = rpcCollection->identify();
    if (!m_rpcId->validElement(moduleId)) {
      ATH_MSG_ERROR( "Invalid RPC module identifier " << m_rpcId->show_to_string(moduleId)  );
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
    ATH_MSG_DEBUG("Number of valid digits in the collection " << m_rpcId->show_to_string(moduleId) 
                  <<" = " << nc2  );
  }
  ATH_MSG_DEBUG("  Number of RPC Collections  Accessed " << nc1  );

  if(error) ATH_MSG_ERROR( "Check of RPC ids FAILED "  );
  else      ATH_MSG_INFO( "Check of RPC ids OK "  );
  
  return StatusCode::SUCCESS;
}

StatusCode TestMuonIdHelpers::testTgcIdHelper() {
  ATH_MSG_DEBUG( "in execute(): testing TGC IdHelper"  );
  
  // get the helper from the manager
 
  if (!m_tgcId) {
    ATH_MSG_ERROR( "cannot retrieve the TGC id helper from MuonDetDescrMgr"  );
    return StatusCode::FAILURE;
  }

  // retrieve data from StoreGate
  typedef TgcDigitContainer::const_iterator collection_iterator;
  typedef TgcDigitCollection::const_iterator digit_iterator;
  
  string key = "TGC_DIGITS";
  const DataHandle <TgcDigitContainer> container;
  ATH_CHECK( (*m_activeStore)->retrieve(container,key) );
  collection_iterator it1_coll= container->begin(); 
  collection_iterator it2_coll= container->end(); 

  ATH_MSG_INFO( "Start Testing TGC Ids"  );

  bool error = false;

  int nc1 = 0 ; 
  for (  ; it1_coll!=it2_coll; ++it1_coll) {
    const  TgcDigitCollection* tgcCollection = *it1_coll; 
    ++nc1; 
    Identifier moduleId = tgcCollection->identify();
    if (!m_tgcId->validElement(moduleId)) {
      ATH_MSG_ERROR( "Invalid TGC module identifier " << m_tgcId->show_to_string(moduleId)  );
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
    ATH_MSG_DEBUG("Number of valid digits in the collection " << m_tgcId->show_to_string(moduleId) 
                  <<" = " << nc2  );
  }
  ATH_MSG_DEBUG("Number of TGC Collections  Accessed " << nc1  );

  if(error) ATH_MSG_ERROR( "Check of TGC ids FAILED "  );
  else      ATH_MSG_INFO( "Check of TGC ids OK "  );
  
  return StatusCode::SUCCESS;
}

int TestMuonIdHelpers::testMDTIds(const Identifier& id) {

  bool error = false;

  longlong startOfUserTime    = System::userTime( System::microSec );
  longlong startOfKernelTime  = System::kernelTime   ( System::microSec );
  longlong startOfElapsedTime = System::ellapsedTime ( System::microSec );

  IdContext   context = m_mdtId->channel_context();    

  m_mdtId->test_id(id,context);
  if (!m_mdtId->valid(id)) {
    ATH_MSG_ERROR( " testMDTIds invalid identifier "
                   << m_mdtId->show_to_string(id) );
    error = true;
  }
  int stationName = m_mdtId->stationName(id);
  std::string name = m_mdtId->stationNameString(stationName);
  if (m_mdtId->isBarrel(id) && name[0] != 'B') {
    ATH_MSG_ERROR( " testMDTIds problem in isBarrel() and stationName() "
                   << m_mdtId->show_to_string(id) );
    error = true;
  }
  if (m_mdtId->isEndcap(id) && name[0] == 'B') {
    ATH_MSG_ERROR( " testMDTIds problem in isEndCap() and stationName() "
                   << m_mdtId->show_to_string(id) );
    error = true;
  }
  if (m_mdtId->stationNameIndex(name) != m_mdtId->stationName(id)) {
    ATH_MSG_ERROR( " testMDTIds problem in stationNameIndex() and stationName() "
                   << m_mdtId->show_to_string(id) );
    error = true;
  }
  const int etaMin = m_mdtId->stationEtaMin(id);
  const int etaMax = m_mdtId->stationEtaMax(id);
  const int eta    = m_mdtId->stationEta(id);
  if (eta < etaMin || eta > etaMax) {
    ATH_MSG_ERROR( " testMDTIds problem in etaMin, etaMax or eta "
                   << "etaMin = " << etaMin
                   << "etaMax = " << etaMax
                   << "eta    = " << eta << " "
                   << m_mdtId->show_to_string(id) );
    error = true;
  }
  const int phiMin = m_mdtId->stationPhiMin(id);
  const int phiMax = m_mdtId->stationPhiMax(id);
  const int phi    = m_mdtId->stationPhi(id);
  if (phi < phiMin || phi > phiMax) {
    ATH_MSG_ERROR( " testMDTIds problem in phiMin, phiMax or phi "
                   << "phiMin = " << phiMin
                   << "phiMax = " << phiMax
                   << "phi    = " << phi << " "
                   << m_mdtId->show_to_string(id) );
    error = true;
  }
  const int technology = m_mdtId->technology(id);
  if (technology != m_mdtId->technologyIndex("MDT")) {
    ATH_MSG_ERROR( " testMDTIds problem in technology() "
                   << "technolog(id) = " << technology
                   << "technologyIndex = " << m_mdtId->technologyIndex("MDT")
                   << m_mdtId->show_to_string(id) );
    error = true;
  }
  if ("MDT" != m_mdtId->technologyString(0) || technology != 0) {
    ATH_MSG_ERROR( " testMDTIds problem in technology() "
	<< "technolog(id) = " << technology
	<< "technologyIndex = " << m_mdtId->technologyIndex("MDT")
	<< m_mdtId->show_to_string(id) );
    error = true;
  }
  const int multilayerMin = m_mdtId->multilayerMin(id);
  const int multilayerMax = m_mdtId->multilayerMax(id);
  const int multilayer    = m_mdtId->multilayer(id);
  if (multilayer < multilayerMin || multilayer > multilayerMax) {
    ATH_MSG_ERROR( " testMDTIds problem in multilayerMin, multilayerMax or multilayer "
                   << "multilayerMin = " << multilayerMin
                   << "multilayerMax = " << multilayerMax
                   << "multilayer    = " << multilayer << " "
                   << m_mdtId->show_to_string(id) );
    error = true;
  }
  const int tubeLayerMin = m_mdtId->tubeLayerMin(id);
  const int tubeLayerMax = m_mdtId->tubeLayerMax(id);
  const int tubeLayer    = m_mdtId->tubeLayer(id);
  if (tubeLayer < tubeLayerMin || tubeLayer > tubeLayerMax) {
    ATH_MSG_ERROR( " testMDTIds problem in tubeLayerMin, tubeLayerMax or tubeLayer "
                   << "tubeLayerMin = " << tubeLayerMin
                   << "tubeLayerMax = " << tubeLayerMax
                   << "tubeLayer    = " << tubeLayer << " "
                   << m_mdtId->show_to_string(id) );
    error = true;
  }

  const int tube    = m_mdtId->tube(id);
  const int tubeMin = m_mdtId->tubeMin(id);
  const int tubeMax = m_mdtId->tubeMax(id);
 
  if (tube < tubeMin || tube > tubeMax) {
    ATH_MSG_ERROR( " testMDTIds problem in tubeMin, tubeMax or tube "
                   << "tubeMin = " << tubeMin
                   << "tubeMax = " << tubeMax
                   << "tube    = " << tube << " "
                   << m_mdtId->show_to_string(id) );
    error = true;
  }

  Identifier oldChild  = m_mdtId->channelID(stationName,eta,phi,multilayer,tubeLayer,tube);
  if (!m_mdtId->valid(oldChild)) error = true; 
  Identifier oldParent = m_mdtId->parentID(oldChild);
  if (!m_mdtId->validElement(oldParent)) error = true;
  Identifier newParent = m_mdtId->elementID(stationName,eta,phi);
  if (!m_mdtId->validElement(newParent)) error = true;
  Identifier newChild  = m_mdtId->channelID(newParent,multilayer,tubeLayer,tube);
  if (!m_mdtId->valid(newChild)) error = true;

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

int TestMuonIdHelpers::testCSCIds(const Identifier& id) {

  bool error = false;
 
  longlong startOfUserTime    = System::userTime( System::microSec );
  longlong startOfKernelTime  = System::kernelTime   ( System::microSec );
  longlong startOfElapsedTime = System::ellapsedTime ( System::microSec );

  IdContext   context = m_cscId->channel_context();   
 
  m_cscId->test_id(id,context);
  if (!m_cscId->valid(id)) {
    ATH_MSG_ERROR( " testCSCIds invalid identifier "
                   << m_cscId->show_to_string(id) );
    error = true;
  }
  int stationName = m_cscId->stationName(id);
  std::string name = m_cscId->stationNameString(stationName);
  if (m_cscId->isEndcap(id) && name[0] != 'C') {
    ATH_MSG_ERROR( " testCSCIds problem in isEndcap() and stationName() "
                   << m_cscId->show_to_string(id) );
    error = true;
  }
  if (!m_cscId->isBarrel(id) && name[0] != 'C') {
    ATH_MSG_ERROR( " testCSCIds problem in isBarrel() and stationName() "
	<< m_cscId->show_to_string(id) );
    error = true;
  }
  if (m_cscId->stationNameIndex(name) != m_cscId->stationName(id)) {
    ATH_MSG_ERROR( " testCSCIds problem in stationNameIndex() and stationName() "
                   << m_cscId->show_to_string(id) );
    error = true;
  }
  const int etaMin = m_cscId->stationEtaMin(id);
  const int etaMax = m_cscId->stationEtaMax(id);
  const int eta    = m_cscId->stationEta(id);
  if (eta < etaMin || eta > etaMax) {
    ATH_MSG_ERROR( " testCSCIds problem in etaMin, etaMax or eta "
                   << "etaMin = " << etaMin
                   << "etaMax = " << etaMax
                   << "eta    = " << eta << " "
                   << m_cscId->show_to_string(id) );
    error = true;
  }
  const int phiMin = m_cscId->stationPhiMin(id);
  const int phiMax = m_cscId->stationPhiMax(id);
  const int phi    = m_cscId->stationPhi(id);
  if (phi < phiMin || phi > phiMax) {
    ATH_MSG_ERROR( " testCSCIds problem in phiMin, phiMax or phi "
                   << "phiMin = " << phiMin
                   << "phiMax = " << phiMax
                   << "phi    = " << phi << " "
                   << m_cscId->show_to_string(id) );
    error = true;
  }
  const int technology = m_cscId->technology(id);
  if (technology != m_cscId->technologyIndex("CSC")) {
    ATH_MSG_ERROR( " testCSCIds problem in technology() "
                   << "technolog(id) = " << technology
                   << "technologyIndex = " << m_cscId->technologyIndex("CSC")
                   << m_cscId->show_to_string(id) );
    error = true;
  }
  if ("CSC" != m_cscId->technologyString(1) || technology != 1) {
    ATH_MSG_ERROR( " testCSCIds problem in technology() "
                   << "technolog(id) = " << technology
                   << "technologyIndex = " << m_cscId->technologyIndex("CSC")
                   << m_cscId->show_to_string(id) );
    error = true;
  }
  const int chamberLayerMin = m_cscId->chamberLayerMin(id);
  const int chamberLayerMax = m_cscId->chamberLayerMax(id);
  const int chamberLayer    = m_cscId->chamberLayer(id);
  if (chamberLayer < chamberLayerMin || chamberLayer > chamberLayerMax) {
    ATH_MSG_ERROR( " testCSCIds problem in chamberLayerMin, chamberLayerMax or chamberLayer "
                   << "chamberLayerMin = " << chamberLayerMin
                   << "chamberLayerMax = " << chamberLayerMax
                   << "chamberLayer    = " << chamberLayer << " "
                   << m_cscId->show_to_string(id) );
    error = true;
  }
  const int wireLayerMin = m_cscId->wireLayerMin(id);
  const int wireLayerMax = m_cscId->wireLayerMax(id);
  const int wireLayer    = m_cscId->wireLayer(id);
  if (wireLayer < wireLayerMin || wireLayer > wireLayerMax) {
    ATH_MSG_ERROR( " testCSCIds problem in wireLayerMin, wireLayerMax or wireLayer "
                   << "wireLayerMin = " << wireLayerMin
                   << "wireLayerMax = " << wireLayerMax
                   << "wireLayer    = " << wireLayer << " "
                   << m_cscId->show_to_string(id) );
    error = true;
  }
  const int measuresPhiMin = m_cscId->measuresPhiMin(id);
  const int measuresPhiMax = m_cscId->measuresPhiMax(id);
  const int measuresPhi    = m_cscId->measuresPhi(id);
  if (measuresPhi < measuresPhiMin || measuresPhi > measuresPhiMax) {
    ATH_MSG_ERROR( " testCSCIds problem in measuresPhiMin, measuresPhiMax or measuresPhi "
                   << "measuresPhiMin = " << measuresPhiMin
                   << "measuresPhiMax = " << measuresPhiMax
                   << "measuresPhi    = " << measuresPhi << " "
                   << m_cscId->show_to_string(id) );
    error = true;
  }
  const int stripMin = m_cscId->stripMin(id);
  const int stripMax = m_cscId->stripMax(id);
  const int strip    = m_cscId->strip(id);
  if (strip < stripMin || strip > stripMax) {
    ATH_MSG_ERROR( " testCSCIds problem in stripMin, stripMax or strip "
                   << "stripMin = " << stripMin
                   << "stripMax = " << stripMax
                   << "strip    = " << strip << " "
                   << m_cscId->show_to_string(id) );
    error = true;
  }

  Identifier oldChild  = m_cscId->channelID(stationName,eta,phi,chamberLayer,wireLayer,measuresPhi,strip);
  if (!m_cscId->valid(oldChild)) error = true;
  Identifier oldParent = m_cscId->parentID(oldChild);
  if (!m_cscId->validElement(oldParent)) error = true;
  Identifier newParent = m_cscId->elementID(stationName,eta,phi);
  if (!m_cscId->validElement(newParent)) error = true;
  Identifier newChild  = m_cscId->channelID(newParent,chamberLayer,wireLayer,measuresPhi,strip);
  if (!m_cscId->valid(newChild)) error = true;

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

int TestMuonIdHelpers::testRPCIds(const Identifier& id) {

  bool error = false;
  
  longlong startOfUserTime    = System::userTime( System::microSec );
  longlong startOfKernelTime  = System::kernelTime   ( System::microSec );
  longlong startOfElapsedTime = System::ellapsedTime ( System::microSec );

  IdContext    context = m_rpcId->channel_context();    

  m_rpcId->test_id(id,context);
  if (!m_rpcId->valid(id)) {
    ATH_MSG_ERROR( " testRPCIds invalid identifier "
                   << m_rpcId->show_to_string(id) );
    error = true;
  }
  int stationName = m_rpcId->stationName(id);
  std::string name = m_rpcId->stationNameString(stationName);
  if (m_rpcId->isBarrel(id) && name[0] != 'B') {
    ATH_MSG_ERROR( " testRPCIds problem in isBarrel() and stationName() "
                   << m_rpcId->show_to_string(id) );
    error = true;
  }
  if (!m_rpcId->isEndcap(id) && name[0] != 'B') {
    ATH_MSG_ERROR( " testRPCIds problem in isEndCap() and stationName() "
                   << m_rpcId->show_to_string(id) );
    error = true;
  }
  if (m_rpcId->stationNameIndex(name) != m_rpcId->stationName(id)) {
    ATH_MSG_ERROR( " testRPCIds problem in stationNameIndex() and stationName() "
                   << m_rpcId->show_to_string(id) );
    error = true;
  }
  const int etaMin = m_rpcId->stationEtaMin(id);
  const int etaMax = m_rpcId->stationEtaMax(id);
  const int eta    = m_rpcId->stationEta(id);
  if (eta < etaMin || eta > etaMax) {
    ATH_MSG_ERROR( " testRPCIds problem in etaMin, etaMax or eta "
                   << "etaMin = " << etaMin
                   << "etaMax = " << etaMax
                   << "eta    = " << eta << " "
                   << m_rpcId->show_to_string(id) );
    error = true;
  }
  const int phiMin = m_rpcId->stationPhiMin(id);
  const int phiMax = m_rpcId->stationPhiMax(id);
  const int phi    = m_rpcId->stationPhi(id);
  if (phi < phiMin || phi > phiMax) {
    ATH_MSG_ERROR( " testRPCIds problem in phiMin, phiMax or phi "
                   << "phiMin = " << phiMin
                   << "phiMax = " << phiMax
                   << "phi    = " << phi << " "
                   << m_rpcId->show_to_string(id) );
    error = true;
  }
  const int technology = m_rpcId->technology(id);
  if (technology != m_rpcId->technologyIndex("RPC")) {
    ATH_MSG_ERROR( " testRPCIds problem in technology() "
                   << "technolog(id) = " << technology
                   << "technologyIndex = " << m_rpcId->technologyIndex("RPC")
                   << m_rpcId->show_to_string(id) );
    error = true;
  }
  if ("RPC" != m_rpcId->technologyString(2) || technology != 2) {
    ATH_MSG_ERROR( " testRPCIds problem in technology() "
                   << "technolog(id) = " << technology
                   << "technologyIndex = " << m_rpcId->technologyIndex("RPC")
                   << m_rpcId->show_to_string(id) );
    error = true;
  }
  const int doubletRMin = m_rpcId->doubletRMin(id);
  const int doubletRMax = m_rpcId->doubletRMax(id);
  const int doubletR    = m_rpcId->doubletR(id);
  if (doubletR < doubletRMin || doubletR > doubletRMax) {
    ATH_MSG_ERROR( " testRPCIds problem in doubletRMin, doubletRMax or doubletR "
                   << "doubletRMin = " << doubletRMin
                   << "doubletRMax = " << doubletRMax
                   << "doubletR    = " << doubletR << " "
                   << m_rpcId->show_to_string(id) );
    error = true;
  }
  const int doubletZMin = m_rpcId->doubletZMin(id);
  const int doubletZMax = m_rpcId->doubletZMax(id);
  const int doubletZ    = m_rpcId->doubletZ(id);
  if (doubletZ < doubletZMin || doubletZ > doubletZMax) {
    ATH_MSG_ERROR( " testRPCIds problem in doubletZMin, doubletZMax or doubletZ "
                   << "doubletZMin = " << doubletZMin
                   << "doubletZMax = " << doubletZMax
                   << "doubletZ    = " << doubletZ << " "
                   << m_rpcId->show_to_string(id) );
    error = true;
  }
  const int doubletPhiMin = m_rpcId->doubletPhiMin(id);
  const int doubletPhiMax = m_rpcId->doubletPhiMax(id);
  const int doubletPhi    = m_rpcId->doubletPhi(id);
  if (doubletPhi < doubletPhiMin || doubletPhi > doubletPhiMax) {
    ATH_MSG_ERROR( " testRPCIds problem in doubletPhiMin, doubletPhiMax or doubletPhi "
                   << "doubletPhiMin = " << doubletPhiMin
                   << "doubletPhiMax = " << doubletPhiMax
                   << "doubletPhi    = " << doubletPhi << " "
                   << m_rpcId->show_to_string(id) );
    error = true;
  }
  const int gasGapMin = m_rpcId->gasGapMin(id);
  const int gasGapMax = m_rpcId->gasGapMax(id);
  const int gasGap    = m_rpcId->gasGap(id);
  if (gasGap < gasGapMin || gasGap > gasGapMax) {
    ATH_MSG_ERROR( " testRPCIds problem in gasGapMin, gasGapMax or gasGap "
                   << "gasGapMin = " << gasGapMin
                   << "gasGapMax = " << gasGapMax
                   << "gasGap    = " << gasGap << " "
                   << m_rpcId->show_to_string(id) );
    error = true;
  }
  const int measuresPhiMin = m_rpcId->measuresPhiMin(id);
  const int measuresPhiMax = m_rpcId->measuresPhiMax(id);
  const int measuresPhi    = m_rpcId->measuresPhi(id);
  if (measuresPhi < measuresPhiMin || measuresPhi > measuresPhiMax) {
    ATH_MSG_ERROR( " testRPCIds problem in measuresPhiMin, measuresPhiMax or measuresPhi "
                   << "measuresPhiMin = " << measuresPhiMin
                   << "measuresPhiMax = " << measuresPhiMax
                   << "measuresPhi    = " << measuresPhi << " "
                   << m_rpcId->show_to_string(id) );
    error = true;
  }
  const int stripMin = m_rpcId->stripMin(id);
  const int stripMax = m_rpcId->stripMax(id);
  const int strip    = m_rpcId->strip(id);
  if (strip < stripMin || strip > stripMax) {
    ATH_MSG_ERROR( " testRPCIds problem in stripMin, stripMax or strip "
                   << "stripMin = " << stripMin
                   << "stripMax = " << stripMax
                   << "strip    = " << strip << " "
                   << m_rpcId->show_to_string(id) );
    error = true;
  }

  Identifier oldChild  = m_rpcId->channelID(stationName,eta,phi,doubletR,
					    doubletZ,doubletPhi,gasGap,measuresPhi,strip);
  if (!m_rpcId->valid(oldChild)) error = true;
  Identifier oldParent = m_rpcId->parentID(oldChild);
  if (!m_rpcId->validElement(oldParent)) error = true;
  Identifier newParent = m_rpcId->elementID(stationName,eta,phi,doubletR);
  if (!m_rpcId->validElement(newParent)) error = true;
  Identifier newChild  = m_rpcId->channelID(newParent,doubletZ,doubletPhi,gasGap,measuresPhi,strip);
  if (!m_rpcId->valid(newChild)) error = true;

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

int TestMuonIdHelpers::testTGCIds(const Identifier& id) {

  bool error = false;
 
  longlong startOfUserTime    = System::userTime( System::microSec );
  longlong startOfKernelTime  = System::kernelTime   ( System::microSec );
  longlong startOfElapsedTime = System::ellapsedTime ( System::microSec );

  IdContext    context = m_tgcId->channel_context();    

  m_tgcId->test_id(id,context);
  if (!m_tgcId->valid(id)) {
    ATH_MSG_ERROR( " testTGCIds invalid identifier "
                   << m_tgcId->show_to_string(id) );
    error = true;
  }
  int stationName = m_tgcId->stationName(id);
  std::string name = m_tgcId->stationNameString(stationName);
  if (!m_tgcId->isForward(id) && name[2] != 'E') {
    ATH_MSG_ERROR( " testTGCIds problem in isEndcap() and stationName() "
                   << m_tgcId->show_to_string(id) );
    error = true;
  }
  if (m_tgcId->isForward(id) && name[2] != 'F') {
    ATH_MSG_ERROR( " testTGCIds problem in isForward() and stationName() "
                   << m_tgcId->show_to_string(id) );
    error = true;
  }
  if (m_tgcId->stationNameIndex(name) != m_tgcId->stationName(id)) {
    ATH_MSG_ERROR( " testTGCIds problem in stationNameIndex() and stationName() "
                   << m_tgcId->show_to_string(id) );
    error = true;
  }
  const int etaMin = m_tgcId->stationEtaMin(id);
  const int etaMax = m_tgcId->stationEtaMax(id);
  const int eta    = m_tgcId->stationEta(id);
  if (eta < etaMin || eta > etaMax) {
    ATH_MSG_ERROR( " testTGCIds problem in etaMin, etaMax or eta "
                   << "etaMin = " << etaMin
                   << "etaMax = " << etaMax
                   << "eta    = " << eta << " "
                   << m_tgcId->show_to_string(id) );
    error = true;
  }
  const int phiMin = m_tgcId->stationPhiMin(id);
  const int phiMax = m_tgcId->stationPhiMax(id);
  const int phi    = m_tgcId->stationPhi(id);
  if (phi < phiMin || phi > phiMax) {
    ATH_MSG_ERROR( " testTGCIds problem in phiMin, phiMax or phi "
                   << "phiMin = " << phiMin
                   << "phiMax = " << phiMax
                   << "phi    = " << phi << " "
                   << m_tgcId->show_to_string(id) );
    error = true;
  }
  const int technology = m_tgcId->technology(id);
  if (technology != m_tgcId->technologyIndex("TGC")) {
    ATH_MSG_ERROR( " testTGCIds problem in technology() "
                   << "technolog(id) = " << technology
                   << "technologyIndex = " << m_tgcId->technologyIndex("TGC")
                   << m_tgcId->show_to_string(id) );
    error = true;
  }
  if ("TGC" != m_tgcId->technologyString(3) || technology != 3) {
    ATH_MSG_ERROR( " testTGCIds problem in technology() "
                   << "technolog(id) = " << technology
                   << "technologyIndex = " << m_tgcId->technologyIndex("TGC")
                   << m_tgcId->show_to_string(id) );
    error = true;
  }
  const int gasGapMin = m_tgcId->gasGapMin(id);
  const int gasGapMax = m_tgcId->gasGapMax(id);
  const int gasGap    = m_tgcId->gasGap(id);
  if (gasGap < gasGapMin || gasGap > gasGapMax) {
    ATH_MSG_ERROR( " testTGCIds problem in gasGapMin, gasGapMax or gasGap "
                   << "gasGapMin = " << gasGapMin
                   << "gasGapMax = " << gasGapMax
                   << "gasGap    = " << gasGap << " "
                   << m_tgcId->show_to_string(id) );
    error = true;
  }
  const int isStripMin = m_tgcId->isStripMin(id);
  const int isStripMax = m_tgcId->isStripMax(id);
  const int isStrip    = m_tgcId->isStrip(id);
  if (isStrip < isStripMin || isStrip > isStripMax) {
    ATH_MSG_ERROR( " testTGCIds problem in isStripMin, isStripMax or isStrip "
                   << "isStripMin = " << isStripMin
                   << "isStripMax = " << isStripMax
                   << "isStrip    = " << isStrip << " "
                   << m_tgcId->show_to_string(id) );
    error = true;
  }
  const int channelMin = m_tgcId->channelMin(id);
  const int channelMax = m_tgcId->channelMax(id);
  const int channel    = m_tgcId->channel(id);
  if (channel < channelMin || channel > channelMax) {
    ATH_MSG_ERROR( " testTGCIds problem in channelMin, channelMax or channel "
                   << "channelMin = " << channelMin
                   << "channelMax = " << channelMax
                   << "channel    = " << channel << " "
                   << m_tgcId->show_to_string(id) );
    error = true;
  }
  
  Identifier oldChild  = m_tgcId->channelID(stationName,eta,phi,gasGap,isStrip,channel);
  if (!m_tgcId->valid(oldChild)) error = true;
  Identifier oldParent = m_tgcId->parentID(oldChild);
  if (!m_tgcId->validElement(oldParent)) error = true;
  Identifier newParent = m_tgcId->elementID(stationName,eta,phi);
  if (!m_tgcId->validElement(newParent)) error = true;
  Identifier newChild  = m_tgcId->channelID(newParent,gasGap,isStrip,channel);
  if (!m_tgcId->valid(newChild)) error = true;

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

