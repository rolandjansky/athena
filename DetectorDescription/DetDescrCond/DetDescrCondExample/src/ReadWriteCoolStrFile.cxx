/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// ReadWriteCoolStrFile.cxx - simple example of algorithm demonstrating 
// read/write of CondStrFile
// Richard Hawkings, started 14/11/05

#include "ReadWriteCoolStrFile.h"
#include "GaudiKernel/ISvcLocator.h"
#include "DetDescrConditions/DetCondKeyTrans.h"
#include "DetDescrCondTools/ICoolStrFileSvc.h"

ReadWriteCoolStrFile::ReadWriteCoolStrFile(const std::string& name, 
  ISvcLocator* pSvcLocator) :AthAlgorithm(name,pSvcLocator),
   p_coolsvc(0),
   m_done(false),
   m_par_read(false),
   m_par_extract(false),
   m_par_write(false),
   m_par_chan(1),
   m_par_tech(0),
   m_par_folder("/TEST/COOLSTR"),
   m_par_rfile(""),
   m_par_wfile("coolstrfile.dat")
{
  // declare properties

  declareProperty("Read",m_par_read);
  declareProperty("Extract",m_par_extract);
  declareProperty("Write",m_par_write);
  declareProperty("Channel",m_par_chan);
  declareProperty("Tech",m_par_tech);
  declareProperty("Folder",m_par_folder);
  declareProperty("RFile",m_par_rfile);
  declareProperty("WFile",m_par_wfile);
}

ReadWriteCoolStrFile::~ReadWriteCoolStrFile() {}

StatusCode ReadWriteCoolStrFile::initialize()
{
  ATH_MSG_INFO("ReadWriteCoolStrFile::initialize() called");

  if (StatusCode::SUCCESS!=service("CoolStrFileSvc",p_coolsvc)) {
    ATH_MSG_ERROR("Cannot get CoolStrFileSvc");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode ReadWriteCoolStrFile::execute() {
  if (m_par_write) {
    // only write data to TDS once
    if (!m_done) writeData();
  }
  if (m_par_read) readData();
  m_done=true;
  return StatusCode::SUCCESS;
}

void ReadWriteCoolStrFile::writeData ATLAS_NOT_THREAD_SAFE() {
  ATH_MSG_INFO("Write data from file "+m_par_wfile+" to folder "+
	       m_par_folder+" at channel " << m_par_chan);

  if (StatusCode::SUCCESS!=p_coolsvc->putFile(m_par_folder,m_par_wfile,m_par_chan,
       m_par_tech))
    ATH_MSG_ERROR("putFile failed");
}

void ReadWriteCoolStrFile::readData ATLAS_NOT_THREAD_SAFE() {
  ATH_MSG_INFO("Read data from folder "+m_par_folder+" channel " << m_par_chan);
  std::string data;
  if (StatusCode::SUCCESS!=p_coolsvc->getString(m_par_folder,m_par_chan,data)) {
    ATH_MSG_INFO("CoolStrFileSvc getString fails for folder " << 
		 m_par_folder << " channel " << m_par_chan);
  } else {
    ATH_MSG_INFO("Data read is " << data);
  }
  // interpret as string stream
  std::istringstream istr(data.c_str());
  std::string a,b,c;
  istr >> a >> b >> c;
  ATH_MSG_INFO("Read string1:" << a << " string2:" << b << " string3:" << c);

  // do read of data into file if requested on first event
  if (m_par_extract && !m_done) {
    if (StatusCode::SUCCESS!=p_coolsvc->getFile(m_par_folder,m_par_chan,m_par_rfile))
      ATH_MSG_ERROR("CoolStrFileSvc getFile fails for folder "+
		    m_par_folder+" channel " << m_par_chan);
  }
}
