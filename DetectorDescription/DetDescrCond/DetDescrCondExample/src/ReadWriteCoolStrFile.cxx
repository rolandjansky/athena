/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ReadWriteCoolStrFile.cxx - simple example of algorithm demonstrating 
// read/write of CondStrFile
// Richard Hawkings, started 14/11/05

#include "GaudiKernel/ISvcLocator.h"
#include "DetDescrConditions/DetCondKeyTrans.h"
#include "DetDescrCondTools/ICoolStrFileSvc.h"
#include "DetDescrCondExample/ReadWriteCoolStrFile.h"
#include "StoreGate/StoreGateSvc.h"

ReadWriteCoolStrFile::ReadWriteCoolStrFile(const std::string& name, 
  ISvcLocator* pSvcLocator) :AthAlgorithm(name,pSvcLocator),
   p_coolsvc(0),
   m_done(false),
   par_read(false),
   par_extract(false),
   par_write(false),
   par_chan(1),
   par_tech(0),
   par_folder("/TEST/COOLSTR"),
   par_rfile(""),
   par_wfile("coolstrfile.dat")
{
  // declare properties

  declareProperty("Read",par_read);
  declareProperty("Extract",par_extract);
  declareProperty("Write",par_write);
  declareProperty("Channel",par_chan);
  declareProperty("Tech",par_tech);
  declareProperty("Folder",par_folder);
  declareProperty("RFile",par_rfile);
  declareProperty("WFile",par_wfile);
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
  if (par_write) {
    // only write data to TDS once
    if (!m_done) writeData();
  }
  if (par_read) readData();
  m_done=true;
  return StatusCode::SUCCESS;
}

StatusCode ReadWriteCoolStrFile::finalize() {
  return StatusCode::SUCCESS;
}

void ReadWriteCoolStrFile::writeData() {
  ATH_MSG_INFO("Write data from file "+par_wfile+" to folder "+
	       par_folder+" at channel " << par_chan);

  if (StatusCode::SUCCESS!=p_coolsvc->putFile(par_folder,par_wfile,par_chan,
       par_tech))
    ATH_MSG_ERROR("putFile failed");
}

void ReadWriteCoolStrFile::readData() {
  ATH_MSG_INFO("Read data from folder "+par_folder+" channel " << par_chan);
  std::string data;
  if (StatusCode::SUCCESS!=p_coolsvc->getString(par_folder,par_chan,data)) {
    ATH_MSG_INFO("CoolStrFileSvc getString fails for folder " << 
		 par_folder << " channel " << par_chan);
  } else {
    ATH_MSG_INFO("Data read is " << data);
  }
  // interpret as string stream
  std::istringstream istr(data.c_str());
  std::string a,b,c;
  istr >> a >> b >> c;
  ATH_MSG_INFO("Read string1:" << a << " string2:" << b << " string3:" << c);

  // do read of data into file if requested on first event
  if (par_extract && !m_done) {
    if (StatusCode::SUCCESS!=p_coolsvc->getFile(par_folder,par_chan,par_rfile))
      ATH_MSG_ERROR("CoolStrFileSvc getFile fails for folder "+
		    par_folder+" channel " << par_chan);
  }
}
