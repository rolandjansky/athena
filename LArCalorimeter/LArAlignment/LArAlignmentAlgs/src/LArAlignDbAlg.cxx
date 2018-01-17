/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArAlignmentAlgs/LArAlignDbAlg.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "DetDescrConditions/DetCondKeyTrans.h"

#include <fstream>

#define LAR_ALIGN "/LAR/Align"

using HepGeom::Translate3D;
using HepGeom::Rotate3D;
using CLHEP::Hep3Vector;

/////////////////////////////////////////////////////////////////////////////

LArAlignDbAlg::LArAlignDbAlg(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator),
    m_writeCondObjs(false),
    m_regIOV(false),
    m_streamName("CondStream1"),
    m_inpFile("LArAlign.inp"),
    m_outpFile("LArAlign-TEST.pool.root"),
    m_outpTag("LARAlign-TEST"),
    m_evt(0),
    m_regSvc("IOVRegistrationSvc",name),
    m_streamer("AthenaPoolOutputStreamTool")
{
    declareProperty("WriteCondObjs",     m_writeCondObjs);
    declareProperty("RegisterIOV",       m_regIOV);
    declareProperty("StreamName",        m_streamName);
    declareProperty("InpFile",           m_inpFile);
    declareProperty("OutpFile",           m_outpFile);
    declareProperty("TagName",           m_outpTag);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
LArAlignDbAlg::~LArAlignDbAlg()
{ 
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode LArAlignDbAlg::initialize()
{
  ATH_MSG_DEBUG(" in initialize()"  );

  // Get Output Stream tool for writing
  if(m_writeCondObjs) {
    ATH_CHECK( m_streamer.retrieve() );
  }
    
  // Get the IOVRegistrationSvc when needed
  if(m_regIOV) {
    ATH_CHECK( m_regSvc.retrieve() );
    ATH_MSG_DEBUG( "Found IOVRegistrationSvc "   );
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode LArAlignDbAlg::execute() 
{
  ATH_MSG_DEBUG(" in execute() "  );

  ATH_CHECK( evtStore()->retrieve(m_evt) );

  int nrun = m_evt->event_ID()->run_number();
  int nevt = m_evt->event_ID()->event_number();
  ATH_MSG_DEBUG( " Event: [" << nrun << "," << nevt << "]"  );

  // If I need to write out the conditions object I'll do that on the first event
  if (m_writeCondObjs && nevt==1) {
    ATH_MSG_DEBUG( "Creating conditions objects for run " << nrun  );
      
    ATH_CHECK( createCondObjects() );
    ATH_CHECK( printCondObjects() );
  } else {
    //  Read objects from DetectorStore
    ATH_CHECK( printCondObjects() );
  }
    
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode LArAlignDbAlg::finalize()
{
  ATH_MSG_DEBUG( " in finalize() "  );
  if(m_writeCondObjs) {
    ATH_CHECK( streamOutCondObjects() );
    ATH_MSG_DEBUG( " Streamed out OK "  );
  }

  if(m_regIOV) {
    ATH_CHECK( registerCondObjects() );
    ATH_MSG_DEBUG( " Register OK "  );
  }
  
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode LArAlignDbAlg::createCondObjects()
{
  ATH_MSG_INFO(" in createCondObjects() "  );
  
  if(detStore()->contains<DetCondKeyTrans>(LAR_ALIGN)) {
    ATH_MSG_INFO( " DetCondKeyTrans already exists, do nothing "  );
    return StatusCode::SUCCESS;
  }

  // Read input file, construct relevant transforms
  std::ifstream infile;
  infile.open(m_inpFile.value().c_str());

  if(!infile.is_open()) {
    ATH_MSG_ERROR( "Unable to open " << m_inpFile << " for reading"  );
    return StatusCode::FAILURE;
  }

  auto transforms = std::make_unique<DetCondKeyTrans>();

  char commentSign = '#';
  std::string commentLine;
  std::string key;
  double x, y, z, theta, phi, rotationAngle;

  while(!infile.eof()) {
    infile >> key;
    if(key.empty()) continue;
    if(key[0]==commentSign)
      std::getline(infile,commentLine);
    else {
      infile >> theta >> phi >> rotationAngle >> x >> y >> z;

      Hep3Vector axis(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta));
      transforms->setTransform(key,Translate3D(x,y,z)*Rotate3D(rotationAngle,axis));
    }
  }
  infile.close();

  ATH_CHECK( detStore()->record(std::move(transforms),LAR_ALIGN) );
  ATH_MSG_DEBUG( " Recorded LAr/Align "  );
  
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode LArAlignDbAlg::printCondObjects()
{
  const DetCondKeyTrans* align;
  StatusCode sc = detStore()->retrieve(align, LAR_ALIGN);

  if(sc.isFailure()) 
    ATH_MSG_WARNING( " Could not find DetCondKeyTrans"  );
  else if(0 == align) 
    ATH_MSG_WARNING(" DetCondKeyTrans ptr is 0"  );
  else {
    std::cout << " \n\n**************************************************** \n";
    std::cout << " ****                                            **** \n";
    std::cout << " ****         Printing Conditions Objects        **** \n";
    std::cout << " ****                                            **** \n";
    std::cout << " **************************************************** \n";
    
    align->print();
    
    std::cout << " ****  **** **** ****     END     **** **** **** **** \n\n\n";
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode LArAlignDbAlg::streamOutCondObjects()
{
  ATH_MSG_DEBUG( " entering streamOutCondObjects "   );

  ATH_CHECK( m_streamer->connectOutput(m_outpFile.value()) );
  ATH_MSG_DEBUG(" Did connect stream to output"  );

  int npairs = 1;

  IAthenaOutputStreamTool::TypeKeyPairs typeKeys(npairs);

  IAthenaOutputStreamTool::TypeKeyPair align("DetCondKeyTrans", LAR_ALIGN);
  typeKeys[0] = align;
  
  ATH_CHECK( m_streamer->streamObjects(typeKeys) );
  ATH_CHECK( m_streamer->commitOutput() );

  return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode LArAlignDbAlg::registerCondObjects()
{
  ATH_MSG_DEBUG( "entering registerCondObject()"   );
  
  std::string objname = "DetCondKeyTrans";

  // Register the IOV DB with the conditions data written out
  ATH_CHECK( m_regSvc->registerIOV(objname, m_outpTag) );
  return StatusCode::SUCCESS;
}

