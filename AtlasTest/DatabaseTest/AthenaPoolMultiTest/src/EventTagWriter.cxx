/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file EventTagWriter.cxx 
 * @brief Implementation of class EventTagWriter
 * @author K. Karr (Kristo.Karr@cern.ch)
 * $Id: EventTagWriter.cxx,v 1.4 2009-02-19 22:04:49 cranshaw Exp $
 */

#include "EventTagWriter.h"

#include "StoreGate/StoreGateSvc.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "GaudiKernel/RndmGenerators.h"

#include "CoralBase/AttributeListSpecification.h"

#include <cassert>
#include <stdlib.h>
#include <math.h>
#include <exception>


EventTagWriter::EventTagWriter(const std::string& name, 
                               ISvcLocator* pSvcLocator) 
    : AthAlgorithm(name, pSvcLocator),
      m_evt("EventInfo"),
      m_attribListSpec(0),
      m_maxNum(5)
{
  declareProperty("MaxParticleNumber", m_maxNum);
}

 
EventTagWriter::~EventTagWriter() 
{
  if ( m_attribListSpec )
  {
    m_attribListSpec->release();
  }
}


StatusCode EventTagWriter::initialize() 
{
  char attribNum[128];
  std::string attribName;

  ATH_MSG_DEBUG( "Initializing " << name()  );

  ATH_CHECK( m_evt.initialize() );

  ATH_MSG_DEBUG( "Defining the attribute list specification."  );
  m_attribListSpec = new coral::AttributeListSpecification;

  m_attribListSpec->extend("TestBool", "bool");
  m_attribListSpec->extend("TestShort", "short");
  m_attribListSpec->extend("TestUShort", "unsigned short");
  m_attribListSpec->extend("TestInt", "int");
  m_attribListSpec->extend("TestUInt", "unsigned int");
  //m_attribListSpec->extend("TestLong", "long");
  //m_attribListSpec->extend("TestULong", "unsigned long");
  m_attribListSpec->extend("TestFloat", "float");
  m_attribListSpec->extend("TestDouble", "double");
  m_attribListSpec->extend("TestString", "string");
  
  m_attribListSpec->extend("NGlobal", "unsigned short");
  m_attribListSpec->extend("RunNumber", "unsigned int");
  m_attribListSpec->extend("EventNumber", "unsigned int");
  m_attribListSpec->extend("Luminosity", "float");
  m_attribListSpec->extend("GoodVertex", "bool");
  m_attribListSpec->extend("XVertex", "float");
  m_attribListSpec->extend("YVertex", "float");
  m_attribListSpec->extend("ZVertex", "float");
  m_attribListSpec->extend("PxMiss", "float");
  m_attribListSpec->extend("PyMiss", "float");
  m_attribListSpec->extend("NEGamma", "unsigned short");
  for(unsigned short i=0; i<m_maxNum; i++)
  {
    std::sprintf(attribNum, "%i", i+1);
    attribName = std::string("EEGamma") + "_" + attribNum;
    m_attribListSpec->extend(attribName, "float");
  }
  for(unsigned short i=0; i<m_maxNum; i++)
  {
    std::sprintf(attribNum, "%i", i+1);
    attribName = std::string("EtaEGamma") + "_" + attribNum;
    m_attribListSpec->extend(attribName, "float");
  }
  for(unsigned short i=0; i<m_maxNum; i++)
  {
    std::sprintf(attribNum, "%i", i+1);
    attribName = std::string("PhiEGamma") + "_" + attribNum;
    m_attribListSpec->extend(attribName, "float");
  }
  for(unsigned short i=0; i<m_maxNum; i++)
  {
    std::sprintf(attribNum, "%i", i+1);
    attribName = std::string("IdEGamma") + "_" + attribNum;
    m_attribListSpec->extend(attribName, "unsigned short");
  }
  m_attribListSpec->extend("NMuon", "unsigned short");
  for(unsigned short i=0; i<m_maxNum; i++)
  {
    std::sprintf(attribNum, "%i", i+1);
    attribName = std::string("PtMuon") + "_" + attribNum;
    m_attribListSpec->extend(attribName, "float");
  }
  for(unsigned short i=0; i<m_maxNum; i++)
  {
    std::sprintf(attribNum, "%i", i+1);
    attribName = std::string("CotetaMuon") + "_" + attribNum;
    m_attribListSpec->extend(attribName, "float");
  }
  for(unsigned short i=0; i<m_maxNum; i++)
  {
    std::sprintf(attribNum, "%i", i+1);
    attribName = std::string("PhiMuon") + "_" + attribNum;
    m_attribListSpec->extend(attribName, "float");
  }
  m_attribListSpec->extend("NTau", "unsigned short");
  for(unsigned short i=0; i<m_maxNum; i++)
  {
    std::sprintf(attribNum, "%i", i+1);
    attribName = std::string("ETau") + "_" + attribNum;
    m_attribListSpec->extend(attribName, "float");
  }
  for(unsigned short i=0; i<m_maxNum; i++)
  {
    std::sprintf(attribNum, "%i", i+1);
    attribName = std::string("EtaTau") + "_" + attribNum;
    m_attribListSpec->extend(attribName, "float");
  }
  for(unsigned short i=0; i<m_maxNum; i++)
  {
    std::sprintf(attribNum, "%i", i+1);
    attribName = std::string("PhiTau") + "_" + attribNum;
    m_attribListSpec->extend(attribName, "float");
  }
  for(unsigned short i=0; i<m_maxNum; i++)
  {
    std::sprintf(attribNum, "%i", i+1);
    attribName = std::string("NtrackTau") + "_" + attribNum;
    m_attribListSpec->extend(attribName, "unsigned short");
  }
  for(unsigned short i=0; i<m_maxNum; i++)
  {
    std::sprintf(attribNum, "%i", i+1);
    attribName = std::string("IdTau") + "_" + attribNum;
    m_attribListSpec->extend(attribName, "unsigned short");
  }
  m_attribListSpec->extend("NJet", "unsigned short");
  for(unsigned short i=0; i<m_maxNum; i++)
  {
    std::sprintf(attribNum, "%i", i+1);
    attribName = std::string("EJet") + "_" + attribNum;
    m_attribListSpec->extend(attribName, "float");
  }
  for(unsigned short i=0; i<m_maxNum; i++)
  {
    std::sprintf(attribNum, "%i", i+1);
    attribName = std::string("PxJet") + "_" + attribNum;
    m_attribListSpec->extend(attribName, "float");
  }
  for(unsigned short i=0; i<m_maxNum; i++)
  {
    std::sprintf(attribNum, "%i", i+1);
    attribName = std::string("PyJet") + "_" + attribNum;
    m_attribListSpec->extend(attribName, "float");
  }
  for(unsigned short i=0; i<m_maxNum; i++)
  {
    std::sprintf(attribNum, "%i", i+1);
    attribName = std::string("PzJet") + "_" + attribNum;
    m_attribListSpec->extend(attribName, "float");
  }
  for(unsigned short i=0; i<m_maxNum; i++)
  {
    std::sprintf(attribNum, "%i", i+1);
    attribName = std::string("BTagged") + "_" + attribNum;
    m_attribListSpec->extend(attribName, "float");
  }

  ATH_MSG_DEBUG( "Printing out attribute list specification:"  );
  {
     //if (msgSvc && (msgSvc->outputLevel() <= MSG::DEBUG)) {
     ATH_MSG_DEBUG( "Attribute List Specification: "  );
       coral::AttributeListSpecification::const_iterator first = m_attribListSpec->begin();
       coral::AttributeListSpecification::const_iterator last  = m_attribListSpec->end();
       for (; first != last; ++first) 
       {
         ATH_MSG_DEBUG( " name " << (*first).name() 
                        << " type " << (*first).typeName() );
       }
     //}
  }
  return (StatusCode::SUCCESS);
}


StatusCode EventTagWriter::execute() 
{
  char attribNum[128];
  std::string attribName = "";
  int randInt = 0;
  Rndm::Numbers genericDist(randSvc(), Rndm::Flat(0.0, 10.0));
  Rndm::Numbers etaDist(randSvc(), Rndm::Gauss(-5.0, 5.0));
  Rndm::Numbers phiDist(randSvc(), Rndm::Flat(0.0, 6.28));
  Rndm::Numbers eDist(randSvc(), Rndm::Gauss(0.0, 50000));
  Rndm::Numbers vtxDist(randSvc(), Rndm::Gauss(-8.0, 8.0));
  Rndm::Numbers lumDist(randSvc(), Rndm::Flat(10.0e+33, 10.0e+35));

  ATH_MSG_DEBUG( "Executing " << name()  );

  AthenaAttributeList* attribList = 0;
  ATH_MSG_DEBUG( "Creating AthenaAttributeList object."  );
  try 
  {
    attribList = new AthenaAttributeList(*m_attribListSpec);
  } 
  catch (const std::exception& e) 
  {
    ATH_MSG_ERROR( "Caught exception during creation of AthenaAttributeList object."
                   << "Message: " << e.what()  );
    return (StatusCode::FAILURE);
  }

  if (!attribList)
  {
    ATH_MSG_ERROR( "Attribute list object is NULL."  );
    return (StatusCode::FAILURE);
  }

  ATH_MSG_DEBUG( "Adding type test data to AthenaAttributeList." );
  bool testBool = true;
  randInt = (int) ( genericDist.shoot() );
  if( randInt%2 ) testBool = false;
  short testShort = (short) ( genericDist.shoot() );
  unsigned short testUShort = (unsigned short) ( genericDist.shoot() );
  int testInt = (int) ( genericDist.shoot() );
  unsigned int testUInt = (unsigned int) ( genericDist.shoot() );
  //long testLong = (long) ( genericDist.shoot() );
  //unsigned long testULong = (unsigned long) ( genericDist.shoot() );
  float testFloat = (float) ( genericDist.shoot() );
  double testDouble = (double) ( genericDist.shoot() );
  std::sprintf(attribNum, "%i", randInt);
  std::string testString = std::string("TestString") + std::string(attribNum);
  
  try
  {
    (*attribList)["TestBool"].data<bool>() = testBool;
    (*attribList)["TestShort"].data<short>() = testShort;
    (*attribList)["TestUShort"].data<unsigned short>() = testUShort;
    (*attribList)["TestInt"].data<int>() = testInt;
    (*attribList)["TestUInt"].data<unsigned int>() = testUInt;
    //(*attribList)["TestLong"].data<long>() = testLong;
    //(*attribList)["TestULong"].data<unsigned long>() = testULong;
    (*attribList)["TestFloat"].data<float>() = testFloat;
    (*attribList)["TestDouble"].data<double>() = testDouble;
    (*attribList)["TestString"].data<std::string>() = testString;
  } 
  catch (const std::exception& e) 
  {
    ATH_MSG_ERROR( "Caught exception from data() when setting type test "
                   << "attributes; Message:" << e.what() );
    return (StatusCode::FAILURE);
  }
  ATH_MSG_DEBUG( "Finished adding type test data to AthenaAttributeList." );

  ATH_MSG_DEBUG( "Retrieving event info from TDS."  );
  SG::ReadHandle<xAOD::EventInfo> evt (m_evt);
  if (!evt.isValid()) {
    ATH_MSG_FATAL( "Could not find event info"  );
    return(StatusCode::FAILURE);
  }

  unsigned short runNumber = evt->runNumber();
  unsigned short eventNumber = evt->eventNumber();

  ATH_MSG_DEBUG( "Adding AOD global data to AthenaAttributeList." );
  unsigned short nGlobal = 0;
  float luminosity = (float) ( lumDist.shoot());
  bool goodVertex = true;
  randInt = (int) ( genericDist.shoot() );
  if ( randInt%2 ) goodVertex = false;
  float xVertex = (float) ( vtxDist.shoot() );
  float yVertex = (float) ( vtxDist.shoot() );
  float zVertex = (float) ( vtxDist.shoot() );
  float pxMiss = (float) ( eDist.shoot() );
  float pyMiss = (float) ( eDist.shoot() );
  try
  {
    (*attribList)["RunNumber"].data<unsigned int>() = runNumber;
    (*attribList)["EventNumber"].data<unsigned int>() = eventNumber;
    (*attribList)["Luminosity"].data<float>() = luminosity;
    (*attribList)["GoodVertex"].data<bool>() = goodVertex;
    (*attribList)["XVertex"].data<float>() = xVertex;
    (*attribList)["YVertex"].data<float>() = yVertex;
    (*attribList)["ZVertex"].data<float>() = zVertex;
    (*attribList)["PxMiss"].data<float>() = pxMiss;
    (*attribList)["PyMiss"].data<float>() = pyMiss;
    nGlobal++;
    (*attribList)["NGlobal"].data<unsigned short>() = nGlobal;
  } 
  catch (const std::exception& e) 
  {
    ATH_MSG_ERROR( "Caught exception from data() when setting AOD global "
                   << "attributes; Message:" << e.what() );
    return (StatusCode::FAILURE);
  }
  ATH_MSG_DEBUG( "Finished adding AODglobal data to AthenaAttributeList." );

  ATH_MSG_DEBUG( "Adding AOD electron data to AthenaAttributeList." );
  unsigned short nEGamma = 0;
  float eEGamma = 0.0;
  float etaEGamma = 0.0;
  float phiEGamma = 0.0;
  unsigned short idEGamma = 0;
  for(unsigned short i=0; i<m_maxNum; i++) 
  {
    eEGamma = fabs( (float) ( eDist.shoot() ) );
    etaEGamma = (float) ( etaDist.shoot() );
    phiEGamma = (float) ( phiDist.shoot() );
    idEGamma = (unsigned short) ( genericDist.shoot() );
    std::sprintf(attribNum, "%i", nEGamma+1);
    attribName = std::string("EEGamma") + "_" + attribNum;
    (*attribList)[attribName].data<float>() = eEGamma;
    attribName = std::string("EtaEGamma") + "_" + attribNum;
    (*attribList)[attribName].data<float>() = etaEGamma;
    attribName = std::string("PhiEGamma") + "_" + attribNum;
    (*attribList)[attribName].data<float>() = phiEGamma;
    attribName = std::string("IdEGamma") + "_" + attribNum;
    (*attribList)[attribName].data<unsigned short>() = idEGamma;
    nEGamma++;
    assert(nEGamma <= m_maxNum);
  }
  (*attribList)["NEGamma"].data<unsigned short>() = nEGamma;
  ATH_MSG_DEBUG( "Finished adding AOD electron data to "
                 << "AthenaAttributeList."  );

  ATH_MSG_DEBUG( "Adding AOD muon data to AthenaAttributeList." );
  unsigned short nMuon = 0;
  float ptMuon = 0.0;
  float cotetaMuon = 0.0;
  float phiMuon = 0.0;
  for(unsigned short i=0; i<m_maxNum; i++)
  {
    ptMuon = fabs( (float) ( eDist.shoot() ) );
    cotetaMuon = (float) ( etaDist.shoot() );
    phiMuon = (float) ( phiDist.shoot() );
    std::sprintf(attribNum, "%i", nMuon+1);
    attribName = std::string("PtMuon") + "_" + attribNum;
    (*attribList)[attribName].data<float>() = ptMuon;
    attribName = std::string("CotetaMuon") + "_" + attribNum;
    (*attribList)[attribName].data<float>() = cotetaMuon;
    attribName = std::string("PhiMuon") + "_" + attribNum;
    (*attribList)[attribName].data<float>() = phiMuon;
    nMuon++;
    assert(nMuon <= m_maxNum);
  }
  (*attribList)["NMuon"].data<unsigned short>() = nMuon;
  ATH_MSG_DEBUG( "Finished adding AOD muon data to AthenaAttributeList." );

  ATH_MSG_DEBUG( "Adding AOD tau data to AthenaAttributeList."  );
  unsigned short nTau = 0;
  float eTau = 0.0;
  float etaTau = 0.0;
  float phiTau = 0.0;
  unsigned short ntrackTau = 0;
  unsigned short idTau = 0;
  for(unsigned short i=0; i<m_maxNum; i++)
  {
    eTau = fabs( (float) ( eDist.shoot() ) );
    etaTau = (float) ( etaDist.shoot() );
    phiTau = (float) ( phiDist.shoot() );
    ntrackTau = (unsigned short) ( genericDist.shoot() );
    idTau = (unsigned short) ( genericDist.shoot() );
    std::sprintf(attribNum, "%i", nTau+1);
    attribName = std::string("ETau") + "_" + attribNum;
    (*attribList)[attribName].data<float>() = eTau;
    attribName = std::string("EtaTau") + "_" + attribNum;
    (*attribList)[attribName].data<float>() = etaTau;
    attribName = std::string("PhiTau") + "_" + attribNum;
    (*attribList)[attribName].data<float>() = phiTau;
    attribName = std::string("NtrackTau") + "_" + attribNum;
    (*attribList)[attribName].data<unsigned short>() = ntrackTau;
    attribName = std::string("IdTau") + "_" + attribNum;
    (*attribList)[attribName].data<unsigned short>() = idTau;
    nTau++;
    assert(nTau <= m_maxNum);
  }
  (*attribList)["NTau"].data<unsigned short>() = nTau;
  ATH_MSG_DEBUG( "Finished adding AOD tau data to AthenaAttributeList." );

  ATH_MSG_DEBUG( "Adding AOD jet data to AthenaAttributeList." );
  unsigned short nJet = 0;
  float eJet = 0.0;
  float pxJet = 0.0;
  float pyJet = 0.0;
  float pzJet = 0.0;
  float bTagged = 0;
  for(unsigned short i=0; i<m_maxNum; i++)
  {
    eJet = fabs( (float) ( eDist.shoot() ) );
    pxJet = (float) ( eDist.shoot() );
    pyJet = (float) ( eDist.shoot() );
    pzJet = (float) ( eDist.shoot() );
    bTagged = (float) ( eDist.shoot() );
    std::sprintf(attribNum, "%i", nJet+1);
    attribName = std::string("EJet") + "_" + attribNum;
    (*attribList)[attribName].data<float>() = eJet;
    attribName = std::string("PxJet") + "_" + attribNum;
    (*attribList)[attribName].data<float>() = pxJet;
    attribName = std::string("PyJet") + "_" + attribNum;
    (*attribList)[attribName].data<float>() = pyJet;
    attribName = std::string("PzJet") + "_" + attribNum;
    (*attribList)[attribName].data<float>() = pzJet;
    attribName = std::string("BTagged") + "_" + attribNum;
    (*attribList)[attribName].data<float>() = bTagged;
    nJet++;
    assert(nJet <= m_maxNum);
  }
  (*attribList)["NJet"].data<unsigned short>() = nJet;
  ATH_MSG_DEBUG( "Finished adding AOD jet data to AthenaAttributeList." );

  ATH_CHECK( evtStore()->record(attribList, "SimpleTag") );

  ATH_MSG_DEBUG( "Printing out attribute list:"  );
  {
    coral::AttributeList::const_iterator first = attribList->begin();
    coral::AttributeList::const_iterator last  = attribList->end();
    for (; first != last; ++first) {
      std::string name = (*first).specification().name();
      std::string typeName = (*first).specification().typeName();
      if ( typeName == "bool" ) {
        const bool* value = static_cast<const bool*>( (*first).addressOfData() );
        ATH_MSG_DEBUG( " name " << name
                       << " type " << typeName
                       << " value " << *value );
      }
      else if ( typeName == "char" ) {
        const char* value = static_cast<const char*>( (*first).addressOfData() );
        ATH_MSG_DEBUG( " name " << name
                       << " type " << typeName
                       << " value " << *value );
      }
      else if ( typeName == "unsigned char" ) {
        const unsigned char* value = 
            static_cast<const unsigned char*>( (*first).addressOfData() );
        ATH_MSG_DEBUG( " name " << name
                       << " type " << typeName
                       << " value " << *value );
      }
      else if ( typeName == "short") {
        const short* value = static_cast<const short*>( (*first).addressOfData() );
        ATH_MSG_DEBUG( " name " << name
                       << " type " << typeName
                       << " value " << *value );
      }
      else if ( typeName == "unsigned short" ) {
        const unsigned short* value = 
            static_cast<const unsigned short*>( (*first).addressOfData() );
        ATH_MSG_DEBUG( " name " << name
                       << " type " << typeName
                       << " value " << *value );
      }
      else if ( typeName == "int" ) {
        const int* value = static_cast<const int*>( (*first).addressOfData() );
        ATH_MSG_DEBUG( " name " << name
                       << " type " << typeName
                       << " value " << *value );
      }
      else if ( typeName == "unsigned int" ) {
        const unsigned int* value = 
            static_cast<const unsigned int*>( (*first).addressOfData() );
        ATH_MSG_DEBUG( " name " << name
                       << " type " << typeName
                       << " value " << *value );
      }
      else if ( typeName == "float" ) {
        const float* value = static_cast<const float*>( (*first).addressOfData() );
        ATH_MSG_DEBUG( " name " << name
                       << " type " << typeName
                       << " value " << *value );
      }
      else if ( typeName == "double" ) {
        const double* value = static_cast<const double*>( (*first).addressOfData() );
        ATH_MSG_DEBUG( " name " << name
                       << " type " << typeName
                       << " value " << *value );
      }
/*
      else if ( typeName == "long double" ) {
        const long double* value = 
            static_cast<const long double*>( (*first).addressOfData() );
            ATH_MSG_DEBUG( " name " << name
                           << " type " << typeName
                           << " value " << *value );
      }
*/
      else if ( typeName == "string" ) {
        const std::string* value = 
            static_cast<const std::string*>( (*first).addressOfData() );
        ATH_MSG_DEBUG( " name " << name
                       << " type " << typeName
                       << " value " << *value );
      }
      else
      {
        ATH_MSG_ERROR( "Unrecognized attribute type: " << typeName );
      }
    }
  }

  return (StatusCode::SUCCESS);
}


StatusCode EventTagWriter::finalize() 
{
  ATH_MSG_DEBUG( "Finalizing " << name()  );
  return (StatusCode::SUCCESS);
}

