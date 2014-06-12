/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTest/LArReadHadDMCoeffFile2.h"
#include "GaudiKernel/MsgStream.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "StoreGate/StoreGateSvc.h"
#include "PathResolver/PathResolver.h"

#define MAX_BUFFER_LEN 1024


/* ***************************************************************************
c-tor
*************************************************************************** */
LArReadHadDMCoeffFile2::LArReadHadDMCoeffFile2(const std::string & name, 
             ISvcLocator * pSvcLocator) : Algorithm(name,pSvcLocator) {
  declareProperty("HadDMCoeffFileName",m_hadDMCoeffFileName);
  declareProperty("CorrectionKey",m_key="HadDMCoeff");

  m_data = 0;
}

LArReadHadDMCoeffFile2::~LArReadHadDMCoeffFile2() {}


/* ***************************************************************************
initialize
*************************************************************************** */
StatusCode LArReadHadDMCoeffFile2::initialize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "=== LArReadHadDMCoeffFile2::initialize() ===  m_key:" << m_key << endreq;
  StatusCode sc;
  StoreGateSvc* detStore;
  sc=service("DetectorStore",detStore);
  if (sc.isFailure()) {
     log << MSG::ERROR << "Unable to get the DetectorStore" << endreq;
     return sc;
   }
   initDataFromFile(m_hadDMCoeffFileName);

   int outputLevel = msgSvc()->outputLevel( name() );
   if(outputLevel == MSG::DEBUG) m_data->PrintData(std::cout);

   sc=detStore->record(m_data,m_key);
   if (sc.isFailure()) {
      log << MSG::ERROR << "Unable to record CaloHadWeight" << endreq;
      return sc;
   }
   sc=detStore->setConst(m_data);
   if (sc.isFailure()) {
      log << MSG::ERROR << "Unable to lock CaloHadWeight" << endreq;
      return sc;
   }
   return StatusCode::SUCCESS;
}


StatusCode  LArReadHadDMCoeffFile2::execute() 
{
  return StatusCode::SUCCESS;
}

StatusCode  LArReadHadDMCoeffFile2::finalize()
{
  return StatusCode::SUCCESS;
}


/* ***************************************************************************
reading text file
*************************************************************************** */
StatusCode LArReadHadDMCoeffFile2::initDataFromFile(std::string hadDMCoeffFileName)
{
  MsgStream log(msgSvc(), name());

  char cLine[MAX_BUFFER_LEN];

  m_data = new CaloHadDMCoeff2();

  // Find the full path to filename
  std::string file = hadDMCoeffFileName;
  log << MSG::INFO << "Reading file  " << file << endreq;

  std::ifstream fin(file.c_str());
  if ( !fin ) {
    log << MSG::ERROR << "LArReadHadDMCoeffFile2::initDataFromFile() - Can't open file " << file << endreq;
    return StatusCode::FAILURE;
  }

  int m_offset = 0;
  int m_indx = 0;

  std::string sLine;
  std::string sdummy;
  std::istringstream ist;
  while(fin.getline(cLine,sizeof(cLine)-1)) {
    if( strlen(cLine)==0 ||  cLine[0] == '#' || cLine[0] == '\n') continue;

    // creating new dead material area
    CaloHadDMCoeff2::HadDMArea dmArea;
    // parsing zone line
    sLine = cLine;
    ist.clear(); ist.str(sLine);
    if( !(ist >> sdummy >> dmArea.m_indx >> dmArea.m_is_on >> dmArea.m_title) && sdummy.find("zone")==std::string::npos ) {
      log << MSG::ERROR << "LArReadHadDMCoeffFile2::initDataFromFile() -> Error! Could not parse line '" << cLine << "' at p1." << endreq;
      return StatusCode::FAILURE;
    }
    dmArea.m_type = CaloHadDMCoeff2::kAREA_PROF;
    if(dmArea.m_title.find("LOOKUP") != std::string::npos) dmArea.m_type = CaloHadDMCoeff2::kAREA_LOOKUP;
    // loop over dimensions of this zones
    std::vector<CaloHadDMCoeff2::HadDMDimension > v_dims;
    while(fin.getline(cLine,sizeof(cLine)-1)){
      if( cLine[0] == '#') continue;
      sLine = cLine;
      // reading number of parameters
      if(sLine.find("npars") != std::string::npos){
        ist.clear(); ist.str(sLine);
        if( !(ist >> sdummy >> dmArea.m_nPars) || dmArea.m_nPars < 0 || dmArea.m_nPars > 1000 ){
          log << MSG::ERROR << "LArReadHadDMCoeffFile2::initDataFromFile() ->Error! Could not parse line '" << cLine << "' at p2." << endreq;
          return StatusCode::FAILURE;
        }
        // this is the end of zone
        break;
      }
      CaloHadDMCoeff2::HadDMDimension dim;
      if( !parse_dim(sLine, dim) ) {
        log << MSG::ERROR << "LArReadHadDMCoeffFile2::initDataFromFile() ->Error! Could not parse line '" << sLine << "' at p2a." << endreq;
        return StatusCode::FAILURE;
      }
      v_dims.push_back(dim);
    }
    // calculation offset for this area
    int ndim = (int) v_dims.size();
    int length =  1;
    for(int i=0; i<ndim; i++){
      length = length*v_dims[i].m_vBins.size();
    }
    dmArea.m_offset = m_offset;
    dmArea.m_length = length;
    // calculation of dimension locator coefficient
    for(int i_dim=0; i_dim<ndim; i_dim++){
      int xloc = 1;
      for(int j=i_dim+1; j<ndim; j++){
        xloc = xloc*v_dims[j].m_vBins.size();
      }
      v_dims[i_dim].xloc = xloc;
    }
    if( v_dims.size() != 4 ){
      log << MSG::ERROR << "LArReadHadDMCoeffFile2::initDataFromFile() ->Error! Wrong number of dimensions for area'" << dmArea.m_title << "' at p3." << endreq;
      return StatusCode::FAILURE;
    }
    dmArea.m_dimFrac = v_dims[0];
    dmArea.m_dimEner = v_dims[1];
    dmArea.m_dimLambda = v_dims[2];
    dmArea.m_dimEta = v_dims[3];

    m_data->addHadDMArea(dmArea);
    m_offset += length; // ofset for next DM area
    m_indx++;
    // putting zero parameters
    CaloHadDMCoeff2::HadDMCoeff pars;
    pars.resize(dmArea.m_nPars, 0.0);
    for(int i_len=0; i_len<dmArea.m_length; i_len++) m_data->addHadDMCoeff(pars);
    // now reading parameters
    for(int i_len=0; i_len<dmArea.m_length; i_len++){
      if(!fin.getline(cLine,sizeof(cLine)-1)) {
        log << MSG::ERROR << "panic " << endreq;
        return StatusCode::FAILURE;
      }
      sLine = cLine;
      ist.clear(); ist.str(sLine);
      int idummy;
      if( !(ist >> idummy) ) {
        log << MSG::ERROR << "LArReadHadDMCoeffFile2::initDataFromFile() -> Warning! Area " << dmArea.m_title << " doesn't have parameters." << endreq;
        break;
      }
      if(idummy != dmArea.m_offset+i_len){
        log << MSG::ERROR << "LArReadHadDMCoeffFile2::initDataFromFile() ->Error! Could not parse line '" << cLine << "' at p3." << endreq;
        return StatusCode::FAILURE;
      }
      for(int j=0; j<(int)v_dims.size(); j++) {
        if(!(ist >> idummy)) {
          log << MSG::ERROR << "LArReadHadDMCoeffFile2::initDataFromFile() -> panic!" << endreq;
          return StatusCode::FAILURE;
        }
      }
      CaloHadDMCoeff2::HadDMCoeff pars;
      pars.resize(dmArea.m_nPars,0.0);
      for(int j=0; j<dmArea.m_nPars; j++) {
        if( !(ist >> pars[j]) ) {
          log << MSG::ERROR << "LArReadHadDMCoeffFile2::initDataFromFile() ->Error! Could not parse line '" << cLine << "' at p4." << endreq;
          log << MSG::ERROR << " dmArea.m_title" << dmArea.m_title << endreq;
          return StatusCode::FAILURE;
        }
      }
      m_data->setHadDMCoeff(dmArea.m_offset+i_len,pars);
    }
  }
  fin.close();
  return StatusCode::SUCCESS;
}


/* **************************************************************************
parsing dimension string
*************************************************************************** */
int LArReadHadDMCoeffFile2::parse_dim(std::string &sLine, CaloHadDMCoeff2::HadDMDimension &dim)
{
  std::string dummy;
  std::string stype;
  std::istringstream ist(sLine.c_str());

  if( !(ist >> dim.m_title >> dim.m_nbins >> dim.m_vMin >> dim.m_vMax >> stype) || dim.m_nbins < 0 || dim.m_nbins > 1000){
    std::cout << "LArReadHadDMCoeffFile2::parse_dim() -> Error! Could not parse line '" << sLine << "' at pp1." << std::endl;
    return 0;
  }
  // bins according to flat scale
  if(stype.find("flat") != std::string::npos) {
    dim.m_type = CaloHadDMCoeff2::kDIM_FLAT;
    dim.m_bin_size = (dim.m_vMax - dim.m_vMin)/float(dim.m_nbins);
    for(int i=0; i<dim.m_nbins; i++) dim.m_vBins.push_back(dim.m_vMin + i*(dim.m_vMax - dim.m_vMin)/float(dim.m_nbins));
  // bins are setted in the string
  }else if(stype.find("hand") != std::string::npos) {
    dim.m_type = CaloHadDMCoeff2::kDIM_HAND;
    float e;
    for(int i=0; i<dim.m_nbins; i++) if( ist >> e ) dim.m_vBins.push_back(e);
  }else{
    std::cout << "LArReadHadDMCoeffFile2::parse_dim() -> Error! Could not parse line '" << sLine << "' at pp3." << std::endl;
    return 0;
  }
  if((int)dim.m_vBins.size() != dim.m_nbins){
    std::cout << "LArReadHadDMCoeffFile2::parse_dim() -> Error! Could not parse line '" << sLine << "' at pp4." << std::endl;
    return 0;
  }
  return (int) dim.m_vBins.size();
}
