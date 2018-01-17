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
   m_par_read(false),
   m_par_extract(false),
   m_par_write(false),
   m_par_deadTube(false),
   m_par_chan(1),
   m_par_tech(0),
   m_par_folder("/TEST/COOLSTR"),
   m_par_rfile(""),
   m_par_wfile("coolstrfile.dat")
   			     //m_par_calRT(false),
			     //m_par_alignCorr(false)
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
  //  declareProperty("WFile",m_par_wfile);
  //declareProperty("CalT0",m_par_calT0);
  //declareProperty("CalRT",m_par_calRT);
  declareProperty("DeadTube",m_par_deadTube);
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
  if (m_par_write && m_par_deadTube) {
    std::cout<< "condizione on" <<std::endl;
    // only write data to TDS once
    if (!m_done) {
      writeTube();
      std::cout<< "esegui" <<std::endl;
    }
  }
  if (m_par_read && m_par_deadTube) {
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
  m_log << MSG::INFO << "Write data from file " << m_par_wfile << " to folder "
	<< m_par_folder << " at channel " << m_par_chan << endmsg;
  if (m_par_deadTube){
    m_log << MSG::ERROR << "dentro " << endmsg;
  if (StatusCode::SUCCESS!=p_coolsvc->putFileTube(m_par_folder,m_par_wfile,m_par_chan,
       m_par_tech))
    m_log << MSG::ERROR << "putFile Tube failed" << endmsg;
}
}
void MdtTubeCondCool::readTube() {
  m_log << MSG::INFO << "Read data from folder " << m_par_folder << " channel "
	<< m_par_chan << endmsg;
  std::string data;
  if (StatusCode::SUCCESS!=p_coolsvc->getString(m_par_folder,m_par_chan,data)) {
    m_log << MSG::INFO << "MdtCoolStrSvc getString fails for folder " << 
      m_par_folder << " channel " << m_par_chan << endmsg;
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
  if (m_par_extract && !m_done) {
    if (StatusCode::SUCCESS!=p_coolsvc->getFile(m_par_folder,m_par_chan,m_par_rfile))
      m_log << MSG::ERROR << "MdtCoolStrSvc getFile fails for folder " << 
	m_par_folder << " channel " << m_par_chan << endmsg;
  }
}
  
  
}




