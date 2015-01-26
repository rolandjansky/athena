/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBRec/CBNT_UpstreamMat.h"

#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "TrackRecord/TrackRecordCollection.h"


using CLHEP::Hep3Vector;


CBNT_UpstreamMat::CBNT_UpstreamMat(const std::string & name, ISvcLocator * pSvcLocator) :
  CBNT_TBRecBase(name, pSvcLocator),
  m_storeGate(0)
{
  declareProperty("CollectionName",m_key);
  m_notFound=false;
  m_nElec = 0;
  m_nPhot = 0;
  m_ElecAng = 0;
  m_ElecEne = 0;
  m_PhotAng = 0;
  m_PhotEne = 0;
}


CBNT_UpstreamMat::~CBNT_UpstreamMat() 
{}


StatusCode CBNT_UpstreamMat::CBNT_initialize() {
  StatusCode sc;
  MsgStream log(msgSvc(), name());
  
  sc = service( "StoreGateSvc", m_storeGate);
  if( sc.isFailure() ){
    log << MSG::FATAL << "Unable to locate the StoreGateSvc Service" <<endreq;
    return sc;
  }

  addBranch("UPSTREAMMAT/up_nElec", m_nElec,"/l");
  addBranch("UPSTREAMMAT/up_nPhot", m_nPhot,"/l");
  
  addBranch("UPSTREAMMAT/up_ElecEne", m_ElecEne);
  addBranch("UPSTREAMMAT/up_ElecAng", m_ElecAng);

  addBranch("UPSTREAMMAT/up_PhotEne", m_PhotEne);
  addBranch("UPSTREAMMAT/up_PhotAng", m_PhotAng);

  return sc;
}

StatusCode CBNT_UpstreamMat::CBNT_execute() {
  StatusCode sc;
  MsgStream log(msgSvc(), name());
  //Set to zero:
  m_nElec=0;
  m_nPhot=0;

  if (!m_storeGate->contains<TrackRecordCollection>(m_key)) {
    if (!m_notFound) {
      log << MSG::WARNING << "TrackRecordCollection with key " << m_key << " not found" << endreq;
      m_notFound=true;
    }
    return StatusCode::SUCCESS;
  }
  const TrackRecordCollection *trackRecordCollection(0);
  sc = m_storeGate->retrieve(trackRecordCollection, m_key);
  if ( sc.isFailure() ) {
    log << MSG::ERROR  << "Error retrieving TrackRecordCollection with key " << m_key << " not found" << endreq;
    return sc;
  }

  TrackRecordCollection::const_iterator it=trackRecordCollection->begin(); 
  TrackRecordCollection::const_iterator it_e=trackRecordCollection->end(); 
  for (;it!=it_e;it++) {
    TrackRecord trackRecord=*it;
    const int pdgCode=trackRecord.GetPDGCode();
    Hep3Vector p=trackRecord.GetMomentum();
    double pr=sqrt(p.y()*p.y()+p.z()*p.z());
    double alpha=atan(pr/p.x());
    if (pdgCode==22) {//Photon
      m_PhotEne->push_back(trackRecord.GetEnergy());
      m_PhotAng->push_back(alpha); //in radiants
      m_nPhot++;
    }
    else if (pdgCode==11 || pdgCode==-11) { //Electron or photon
      m_ElecEne->push_back(trackRecord.GetEnergy());
      m_ElecAng->push_back(alpha); //in radiants
      m_nElec++;
    }
    else
      log << MSG::INFO << "Found particle code " << pdgCode << endreq; 
  }//end loop over TrackRecords

  return StatusCode::SUCCESS;
}

StatusCode CBNT_UpstreamMat::CBNT_finalize() {

  return StatusCode::SUCCESS;
}

StatusCode CBNT_UpstreamMat::CBNT_clear() {
   if(m_ElecEne) m_ElecEne->clear();
   if(m_ElecAng) m_ElecAng->clear();
   if(m_PhotEne) m_PhotEne->clear();
   if(m_PhotAng) m_PhotAng->clear();

   return StatusCode::SUCCESS;
}
