/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SiDistWriteAlg.cxx
// Richard Hawkings, started 27/3/07
// simple algorithm to manage silicon (PIXEL/SCT) module distortion data

#include <fstream>
#include <iostream>

#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "DetDescrConditions/DetCondCFloat.h"
#include "InDetAlignGenAlgs/SiDistWriteAlg.h"

SiDistWriteAlg::SiDistWriteAlg(const std::string& name, ISvcLocator* pSvcLocator)
    :AthAlgorithm   (name, pSvcLocator),
        
     m_pixid(0),m_sctid(0),
     m_first(true),
     par_print(false),
     par_size(3),
     par_readfile(""),
     par_distkey("/Indet/PixelDist")
{

 
  declareProperty("Print",par_print);
  declareProperty("ReadFile",par_readfile);
  declareProperty("ArraySize",par_size);
  declareProperty("KeyName",par_distkey);
}

SiDistWriteAlg::~SiDistWriteAlg()
{}

StatusCode SiDistWriteAlg::initialize()
{
  msg (MSG::DEBUG) << "SiDistWriteAlg::initialize()" << endreq;
  // check StoreGate service available
  if (evtStore().retrieve().isFailure())
    {
      ATH_MSG_VERBOSE("StoreGate service not found");
      return StatusCode::FAILURE;
    }
  
  // get detector store
  if (detStore().retrieve().isFailure())
    {
      msg( MSG::FATAL) << "Detector store not found" << endreq; 
    }
  
  
  // get identifier helpers
  if ((StatusCode::SUCCESS!=detStore()->retrieve(m_pixid)) ||
      (StatusCode::SUCCESS!=detStore()->retrieve(m_sctid))) {
    msg (MSG::ERROR) << "Could not get helpers from detector store" 
    << endreq;
  }
   ATH_MSG_DEBUG( "Distortion information in TDS at key " <<
    par_distkey );
  if (!par_readfile.empty())
     ATH_MSG_DEBUG ( "Read distortion parameters from file" 
    << par_readfile );
  if (par_print) ATH_MSG_DEBUG ( 
    "Distortion information will be printed on first event" );
  return StatusCode::SUCCESS;
}

StatusCode SiDistWriteAlg::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode SiDistWriteAlg::execute() {
  if (m_first) {
    if (!par_readfile.empty()) readFile();
    if (par_print) print();
  m_first=false;
  }
  return StatusCode::SUCCESS;
}

bool SiDistWriteAlg::readFile() {
  // create structure in TDS
  ATH_MSG_DEBUG ( "Create new DetCondCFloat of size " << par_size
  << " in TDS" );
  std::unique_ptr<DetCondCFloat> pdist(new DetCondCFloat(par_size,par_distkey));
  // read data from text file
  ATH_MSG_DEBUG ( "Opening text file " << par_readfile << 
    " to get distortion data" );
  std::ifstream infile;
  infile.open(par_readfile.c_str());
  if (!infile) {
    msg (MSG::ERROR) << "Problem opening input file" << endreq;
    return false;
  }
  // loop over lines in file
  int nline=0;
  int nadd=0;
  while (infile) {
    std::string tmpline;
    std::getline(infile,tmpline);
    if (!infile) break;
    std::istringstream instring(tmpline);
    std::string ident;
    Identifier32::value_type identhash;
    float x,y,z;
    instring >> ident >> identhash >> x >> y >> z;
    ATH_MSG_DEBUG( "Read identifier: " << ident << " hash " << 
      identhash << " x" << x << " y " << y << " z " << z );
    // construct the identifier and crosscheck
    Identifier identifier;
    if (makeIdent(ident,identhash,identifier)) {
      float fbuf[3];
      fbuf[0]=x;
      fbuf[1]=y;
      fbuf[2]=z;
      pdist->add(identifier,fbuf);
      ++nadd;
    } else {
      msg (MSG::ERROR) << "Problem constructing identifier " << ident << 
  " / " << identhash << endreq;
    }
    ++nline;
  }
  infile.close();
  ATH_MSG_DEBUG ( "Read distortions for " << nline << " modules" << 
      " of which " << nadd << " successfully added" );
  // record in StoreGate
  if (StatusCode::SUCCESS==detStore()->record(pdist.get(),par_distkey)) {
    pdist.release();
    ATH_MSG_DEBUG ( "Recorded DetCondCFloat " << par_distkey << " in TDS" );
  } else {
    msg(MSG::ERROR) << "Failed to record DetCondCFloat " << par_distkey << " in TDS"
    << endreq;
    return false;
  }
  return true;
}

bool SiDistWriteAlg::makeIdent(const std::string& ident,
             const Identifier32::value_type identhash, Identifier& identifier) {
  // parse the [a.b.c.d.e.f.CLHEP::g] string to extract 7 fields
  // remove the leading [ immediately
  std::string lident=ident.substr(1);
  for (unsigned int i=0;i<lident.size();++i) {
    // transform ] and . to spaces
    if (lident[i]==']' || lident[i]=='.') lident[i]=' ';
  }
  std::istringstream instring(lident);
  int atlas,det,bec,layer,phi,eta,side;
  instring >> atlas >> det >> bec >> layer >> phi >> eta >> side;
  if (det==1) {
    identifier=m_pixid->wafer_id(bec,layer,phi,eta);
  } else if (det==2) {
    identifier=m_sctid->wafer_id(bec,layer,phi,eta,side);
  } else {
    msg(MSG::ERROR) << "Unknown detector type " << det << endreq;
    return false;
  }
  // cross check with the supplied identifier compact value
  // (get 32-bit part, as it's only a wafer_id)
  return (identifier.get_identifier32().get_compact()==identhash);
}

void SiDistWriteAlg::print() {
  ATH_MSG_DEBUG ( "Print distortion parameters at " << par_distkey );
  // retrieve distortions from Storegate and print in detail
  const DetCondCFloat* pdist=0;
  if (StatusCode::SUCCESS==detStore()->retrieve(pdist,par_distkey)) {
    pdist->print2();
  } else {
    msg (MSG::ERROR) << "Unable to retrieve distortion parameters " <<
      endreq;
  }
}
