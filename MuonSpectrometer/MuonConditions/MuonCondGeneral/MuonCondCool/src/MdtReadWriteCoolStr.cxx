/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MdtReadWriteCoolStr.cxx - simple example of algorithm demonstrating 
// read/write of CondStrFile
// 18 may 2006 monica.verducci@cern.ch

#include "GaudiKernel/ISvcLocator.h"
#include "MuonCondInterface/MdtICoolStrSvc.h"
#include "MuonCondCool/MdtReadWriteCoolStr.h"
#include "MuonCondSvc/MdtStringUtils.h"
#include "StoreGate/StoreGateSvc.h"
//#include "string.h"
#include <string>
namespace MuonCalib {

MdtReadWriteCoolStr::MdtReadWriteCoolStr(const std::string& name, 
  ISvcLocator* pSvcLocator) :AthAlgorithm(name,pSvcLocator),
   m_log(msgSvc(),name),
   p_detstore(0),
   p_coolsvc(0),
   m_done(false),
   par_read(false),
   par_extract(false),
   par_write(false),
   par_chan(1),
   par_tech(0),
   par_folder("/TEST/COOLSTR"),
   par_rfile(""),
   par_wfile("coolstrfile.dat"),
   par_calT0(false),
   par_calRT(false),
   par_alignCorr(false),
   par_alignasciiformat(false),			     
   par_deadTube(false)
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
  declareProperty("WFile",par_wfile);
  declareProperty("CalT0",par_calT0);
  declareProperty("CalRT",par_calRT);
  declareProperty("AlignCorr",par_alignCorr);
  declareProperty("Alignasciiformat",par_alignasciiformat);
  declareProperty("DeadTube",par_deadTube);
}

MdtReadWriteCoolStr::~MdtReadWriteCoolStr() {}

StatusCode MdtReadWriteCoolStr::initialize()
{
  m_log << MSG::INFO << "MdtReadWriteCoolStr::initialize() called" << endmsg;

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

StatusCode MdtReadWriteCoolStr::execute() {
  if (par_write) {
    // only write data to TDS once
    if (!m_done) writeData();
    if (!par_read) return StatusCode::SUCCESS;
  }
  if (par_read && par_calT0) {
    loadData();
    m_done=true;
    return StatusCode::SUCCESS;
  }
  if (par_read && par_calRT) {
    loadDataRt();
    m_done=true;
    return StatusCode::SUCCESS;
  }
  if (par_read && par_alignCorr) {
    loadDataAlign();
    m_done=true;
    return StatusCode::SUCCESS;
  }
  if (par_read && par_alignasciiformat) {
    loadDataAlignAscii();
    m_done=true;
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode MdtReadWriteCoolStr::finalize() {
  return StatusCode::SUCCESS;
}

void MdtReadWriteCoolStr::writeData() {
  m_log << MSG::INFO << "Write data from file " << par_wfile << " to folder "
	<< par_folder << " at channel " << par_chan << endmsg;
 if(par_calT0) {
  if (StatusCode::SUCCESS!=p_coolsvc->putFileT0(par_folder,par_wfile,par_chan,
       par_tech))
    m_log << MSG::ERROR << "putFile T0 failed" << endmsg;
 }
 if(par_calRT) {
  if (StatusCode::SUCCESS!=p_coolsvc->putFileRT(par_folder,par_wfile,par_chan,
       par_tech))
    m_log << MSG::ERROR << "putFile RT failed" << endmsg;
 }
 if(par_alignCorr) {
  if (StatusCode::SUCCESS!=p_coolsvc->putFileAlignCorr(par_folder,par_wfile,par_chan,
       par_tech))
    m_log << MSG::ERROR << "putFile Align corrections failed" << endmsg;
 }
 if(par_alignasciiformat) {
  if (StatusCode::SUCCESS!=p_coolsvc->putAligFromFile(par_folder,par_wfile,par_chan,
       par_tech))
    m_log << MSG::ERROR << "putFile Align corrections failed" << endmsg;
 }
 if (par_deadTube){
   if (StatusCode::SUCCESS!=p_coolsvc->putFileTube(par_folder,par_wfile,par_chan,
						   par_tech))
     m_log << MSG::ERROR << "putFile Tube failed" << endmsg;
 }

}

void MdtReadWriteCoolStr::readData() {
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
  std::string a,b,c;
  istr >> a >> b >> c;
  m_log << "Read string1:" << a << " string2:" << b << " string3:" << c 
	<< endmsg;

  // do read of data into file if requested on first event
  if (par_extract && !m_done) {
    if (StatusCode::SUCCESS!=p_coolsvc->getFile(par_folder,par_chan,par_rfile))
      m_log << MSG::ERROR << "MdtCoolStrSvc getFile fails for folder " << 
       par_folder << " channel " << par_chan << endmsg;
  }
}


void MdtReadWriteCoolStr::loadData() {
  m_log << MSG::INFO << "Load data from folder " << par_folder << " channel "
	<< par_chan << endmsg;
  std::string data;
  if (StatusCode::SUCCESS!=p_coolsvc->getString(par_folder,par_chan,data)) {
    m_log << MSG::INFO << "MdtCoolStrSvc getString fails for folder " << 
      par_folder << " channel " << par_chan << endmsg;
  } else {
    m_log << MSG::INFO << "Data load is " << data << endmsg;
  }
  // interpret as string stream
  std::istringstream istr(data.c_str());
  std::string a,b,c;
  istr >> a >> b >> c;
  m_log << "Read string1:" << a << " string2:" << b << " string3:" << c 
	<< endmsg;
  

  char * eta(NULL); char *phi(NULL);
  std::string seperator("_");
  std::string name;
  std::string rName;
  int nlayers=4; int nml=2; char* ntubesc(NULL);
  // parameters for the MdtTubeContainer
  // header filename,version,region,tubes
  char * parameters=const_cast<char*>(a.c_str());
  char *region;
  char * pch;
  int i=0;
  //  printf ("Splitting string \"%s\" in tokens:\n",parameters);
  pch = strtok (parameters," _,");
  name= pch;
  //std::cout << "name of chamber is " << pch << std::endl;
  while (pch != NULL)
  {
    pch = strtok (NULL, "_,"); 
    i++;
    if (i==1) {
      phi= pch;
      //std::cout << "phi value is " << pch << std::endl;
    }
    if (i==2) eta= pch;
    if (i==4) region= pch;
    if (i==5) ntubesc= pch;
    m_log  << MSG::INFO<< "region:" << region << endmsg;
    if( eta != NULL ) m_log  << MSG::INFO<< "eta:" << eta << endmsg;
    else m_log  << MSG::INFO<< "eta: NULL" << endmsg;
    if( phi != NULL ) m_log  << MSG::INFO<< "phi:" << phi << endmsg;
    else m_log  << MSG::INFO<< "phi: NULL" << endmsg;
  }
  
  int ntubes =atoi(ntubesc);
  m_log  << MSG::INFO<< "number of tubes" << ntubesc << endmsg;

  int ntubesLay = ntubes/(nml*nlayers);
  //m_log << "number of tubes per layer  " << ntubesLay << endmsg;
//  int size = nml*nlayers*ntubes;
  //m_log << "name:" << name << " phi" << phi << " eta" << eta 
  //<< endmsg;
  
  rName += name + seperator + eta + seperator + phi;

  int ml=1; int l= 1; int t=1;
  int k=0;  

  char * TubePar=const_cast<char*>(b.c_str());
  char * pch1;
  float t0; float adcCal; float inversePropSpeed;  
  pch1 = strtok (TubePar,",");
  while(pch1 != NULL) {
    
      ++k;
      if (k==1) {
	t0=atof(pch1);
	std::cout << "t0  " << pch1 << std::endl;
	m_log << MSG::INFO<< "t0:" << t0 << endmsg;
      }
      
//      if (k==2) float statusCode = atof(pch1); 
      if (k==3) { 

	adcCal = atof(pch1);
	inversePropSpeed = (1./300.);
	m_log  << MSG::INFO<< "adc:" <<adcCal <<" speed= " <<inversePropSpeed  << endmsg;
	t++; k=0;

	if (t>ntubesLay) {
	  l++; t=1;}
	if (l>nlayers) {
	  ml++;
	  l=1;
	}

	
      }
      pch1 = strtok (NULL, ", "); 
  }


  if (par_extract && !m_done) {
    if (StatusCode::SUCCESS!=p_coolsvc->getFile(par_folder,par_chan,par_rfile))
      m_log << MSG::ERROR << "MdtCoolStrSvc getFile fails for folder " << 
	par_folder << " channel " << par_chan << endmsg;
  }
}

// Rt Relation

void MdtReadWriteCoolStr::loadDataRt() {
  m_log << MSG::INFO << "Load data from folder " << par_folder << " channel "
	<< par_chan << endmsg;
  std::string data;
  if (StatusCode::SUCCESS!=p_coolsvc->getString(par_folder,par_chan,data)) {
    m_log << MSG::INFO << "MdtCoolStrSvc getString fails for folder " << 
      par_folder << " channel " << par_chan << endmsg;
  } else {
    m_log << MSG::INFO << "Data load is " << data << endmsg;
  }
  // interpret as string stream
  std::istringstream istr(data.c_str());
  std::string a,b,c;
  istr >> a >> b >> c;
  m_log << "Read string1:" << a << " string2:" << b << " string3:" << c 
	<< endmsg;

  
  char * parameters=const_cast<char*>(a.c_str());
  int region, npoints(0);
  char * pch;
  int i=0;
  //  printf ("Splitting string \"%s\" in tokens:\n",parameters);
  pch = strtok (parameters," _,");
  region=atoi(pch);
  //  std::cout << "region " << pch << std::endl;
  m_log << MSG::INFO << "region == " << region <<endmsg; 
 while (pch != NULL)
  {
    pch = strtok (NULL, "_,"); 
    i++;
    if (i==1) {
      npoints= atoi(pch);
      std::cout << "number of points " << pch << std::endl;
    }
  }  
  
  int k=0;  
  int n=1;


  char * RTPar=const_cast<char*>(b.c_str());
  char * pch1;
  pch1 = strtok (RTPar,",");
  
//  unsigned int regionId = region;

  double t_min=0;
  double bin_size=0;

  while(pch1 != NULL&&n<=npoints ) {
    ++k;
  
   if (k==1) {
     float rad=atof(pch1);
     std::cout<< rad <<"k=1 "<< std::endl;

   }  
   if (k==2) {
     float tim= atof(pch1);
     if (n==1) t_min=tim;
     if (n==2) bin_size=tim-t_min; 
     m_log << MSG::INFO<<" bin_size = " << 
	 bin_size<< endmsg;
   }
   if (k==3) {
     float sigma= atof(pch1);

     std::cout<< sigma <<" npoint" << n <<std::endl;
     k=0;
     n++;
   }
   pch1 = strtok (NULL, ", ");
  }

}

// Align Corrections
void MdtReadWriteCoolStr::loadDataAlign() {
  m_log << MSG::INFO << "Load data from folder " << par_folder << " channel "
	<< par_chan << endmsg;
  std::string data;
  if (StatusCode::SUCCESS!=p_coolsvc->getString(par_folder,par_chan,data)) {
    m_log << MSG::INFO << "MdtCoolStrSvc getString fails for folder " << 
      par_folder << " channel " << par_chan << endmsg;
  } else {
    m_log << MSG::INFO << "Data load is " << data << endmsg;
  }


  // Check the first word to see if it is a correction
  std::string type;
  //Get Timestamp from Header: define variables to store it
  std::string since_str;
  std::string till_str;

  std::string delimiter = "\n";
  std::vector<std::string> lines;
  MuonCalib::MdtStringUtils::tokenize(data,lines,delimiter);
  for (unsigned int i=0; i<lines.size();i++) {

    std::string blobline = lines[i];

    std::string delimiter = ":";
    std::vector<std::string> tokens;
    MuonCalib::MdtStringUtils::tokenize(blobline,tokens,delimiter);
    type = tokens[0];
    //Parse line
    if (type.find("#")==0) {
      //skip it
      continue;
    }
    //std::cout << type ;
    if (type.find("Header")==0) {
      std::string delimiter = "|";
      std::vector<std::string> tokens;
      MuonCalib::MdtStringUtils::tokenize(blobline,tokens,delimiter);
      since_str = tokens[1];
      till_str = tokens[2];
      std::cout << "Since: " << since_str << std::endl;
      std::cout << "Till: " << till_str << std::endl;
    }
    
    if (type.find("Corr")==0) {
      std::string delimiter = " ";
      std::vector<std::string> tokens;
      MuonCalib::MdtStringUtils::tokenize(blobline,tokens,delimiter);
      for (unsigned int i=0; i<tokens.size(); i++)
	std::cout << " field:" << tokens[i];
      std::cout << std::endl;
    }
  }


}

// Align Corrections From Ascii File
void MdtReadWriteCoolStr::loadDataAlignAscii() {
  m_log << MSG::INFO << "Load data from folder " << par_folder << " channel "
	<< par_chan << endmsg;
  std::string data;
  if (StatusCode::SUCCESS!=p_coolsvc->getString(par_folder,par_chan,data)) {
    m_log << MSG::INFO << "MdtCoolStrSvc getString fails for folder " << 
      par_folder << " channel " << par_chan << endmsg;
  } else {
    m_log << MSG::INFO << "Data load is " << data << endmsg;
  }
  
 
  std::string name;  std::string corr; std::string stationType;
 

  char * AlignPar=const_cast<char*>(data.c_str());
  char * pch1;
  int k =0;
  pch1 = strtok (AlignPar,",");
  while(pch1 != NULL) {
    ++k;
    if (k==1) {
      corr=(pch1);
      
    }
    if (k==2) {name = (pch1);
    std::cout << "Station type " << name << std::endl;
    }
    if (k==3) {//int jff = atoi(pch1);
    std::cout << "jff " << pch1 << std::endl;
    }
    if (k==4) {//int jzz = atoi(pch1);
    std::cout << "jzz " << pch1 << std::endl;}
    if (k==5) {int job = atoi(pch1);
    std::cout << "job " << job << std::endl;}
    if (k==6) {float s = atof(pch1);
    std::cout << "s " << s << std::endl;}
    if (k==7) {float z = atof(pch1);
    std::cout << "z " << z << std::endl;}
    if (k==8) {float t = atof(pch1);
    std::cout << "t " << t << std::endl;}
    if (k==9) {float ths = atof(pch1);
    std::cout << "ths " << ths << std::endl;}
    if (k==10) {float thz = atof(pch1);
    std::cout << "thz " << thz << std::endl;}
    if (k==11) {
      float tht = atof(pch1);
      std::cout << "tht " << tht << std::endl;
      k=0;}
 
    pch1 = strtok (NULL, ", "); 
  }
}
    
} //namespace

