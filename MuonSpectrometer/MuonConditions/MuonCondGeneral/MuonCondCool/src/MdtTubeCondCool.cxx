/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MdtTubeCondCool.cxx - simple example of algorithm demonstrating 
// read/write of CondStrFile
// 21 september  2006 monica.verducci@cern.ch
// fix warning  
#include "GaudiKernel/ISvcLocator.h"
#include "MuonCondInterface/MdtICoolStrSvc.h"

#include "MuonCondCool/MdtTubeCondCool.h"
#include "StoreGate/StoreGateSvc.h"
//#include "string.h"
#include <string>
namespace MuonCalib {

MdtTubeCondCool::MdtTubeCondCool(const std::string& name, 
  ISvcLocator* pSvcLocator) :AthAlgorithm(name,pSvcLocator),
   m_log(msgSvc(),name),
   p_detstore(0),
   m_storeGate(0),
   p_coolsvc(0),
   m_mdtDeadTubeStatusContainer(0),
   m_done(false),
   par_read(false),
   par_extract(false),
   par_write(false),
   par_deadTube(false),
   par_chan(1),
   par_tech(0),
   par_folder("/TEST/COOLSTR"),
   par_rfile(""),
   par_wfile("coolstrfile.dat")
   			     //par_calRT(false),
			     //par_alignCorr(false)
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
  //  declareProperty("WFile",par_wfile);
  //declareProperty("CalT0",par_calT0);
  //declareProperty("CalRT",par_calRT);
  declareProperty("DeadTube",par_deadTube);
}

MdtTubeCondCool::~MdtTubeCondCool() {}

StatusCode MdtTubeCondCool::initialize()
{
  m_log << MSG::INFO << "MdtTubeCondCool::initialize() called" << endmsg;

  if (StatusCode::SUCCESS!=service("DetectorStore",p_detstore)) {
    m_log << MSG::FATAL << "Detector store not found" << endmsg;
    return StatusCode::FAILURE;
  }
  if (StatusCode::SUCCESS!=service("MuonCalib::MdtCoolStrSvc",p_coolsvc)) {
    m_log << MSG::ERROR << "Cannot get MdtCoolStrSvc" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
  

}

StatusCode MdtTubeCondCool::execute() {
  if (par_write && par_deadTube) {
    std::cout<< "condizione on" <<std::endl;
    // only write data to TDS once
    if (!m_done) {
      writeTube();
      std::cout<< "esegui" <<std::endl;
    }
  }
  if (par_read && par_deadTube) {
    readTube();
    m_done=true;
    return StatusCode::SUCCESS;
  }

  return StatusCode::FAILURE;
}

StatusCode MdtTubeCondCool::finalize() {
  return StatusCode::SUCCESS;
}

void MdtTubeCondCool::writeTube() {
  m_log << MSG::INFO << "Write data from file " << par_wfile << " to folder "
	<< par_folder << " at channel " << par_chan << endmsg;
  if (par_deadTube){
    m_log << MSG::ERROR << "dentro " << endmsg;
  if (StatusCode::SUCCESS!=p_coolsvc->putFileTube(par_folder,par_wfile,par_chan,
       par_tech))
    m_log << MSG::ERROR << "putFile Tube failed" << endmsg;
}
}
void MdtTubeCondCool::readTube() {
  m_log << MSG::INFO << "Read data from folder " << par_folder << " channel "
	<< par_chan << endmsg;
  std::string data;
  if (StatusCode::SUCCESS!=p_coolsvc->getString(par_folder,par_chan,data)) {
    m_log << MSG::INFO << "MdtCoolStrSvc getString fails for folder " << 
      par_folder << " channel " << par_chan << endmsg;
  } else {
    m_log << MSG::INFO << "Data read is " << data << endmsg;
  }
  // interpret as string stream
  std::istringstream istr(data.c_str());
  
  std::string a, b;
  istr >> a >> b;
  m_log << "Read string1:" << a << " string2:" << b 	<< endmsg;
  

  char * layer; char *tubeid; char *mlayer;
  char *pch;
  std::string seperator(",");
  char *name;
  
  char * parameters=const_cast<char*>(a.c_str());
  
  int i=1;
  printf ("Splitting string \"%s\" in tokens:\n",parameters);
  pch = strtok (parameters,",");
//  int j=0;
  //std::cout << "name of chamber is " << pch << std::endl;
  while (pch != NULL)
    {
      
      
      if (i==1) {
	name= pch;
	//std::cout << "name value is " << pch << std::endl;
      }
      if (i==2) { 
	mlayer= pch;
	//std::cout << "mlayer value is " << pch << std::endl;
      }
      if (i==3) { 
	layer= pch;
	//std::cout << "layer value is " << pch << std::endl;
      }
      if (i==4) {
	tubeid= pch;
	//    std::cout << "tube value is " << pch << std::endl; 
	i=0;
      }
      
      i++;
      m_log << MSG::INFO << "name = " << name
	    << " mlayer = "<< mlayer << " layer =" << layer
	    << "  tubeid=" << tubeid << endmsg;
      pch = strtok (NULL, ","); 
    }
  
  // Need to understand in which class data this information should be stored.....
  
  
  
  // do read of data into file if requested on first event
  if (par_extract && !m_done) {
    if (StatusCode::SUCCESS!=p_coolsvc->getFile(par_folder,par_chan,par_rfile))
      m_log << MSG::ERROR << "MdtCoolStrSvc getFile fails for folder " << 
	par_folder << " channel " << par_chan << endmsg;
  }
}
  
  
}




