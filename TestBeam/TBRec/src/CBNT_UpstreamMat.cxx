/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CBNT_UpstreamMat.h"

#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "TrackRecord/TrackRecordCollection.h"


using CLHEP::Hep3Vector;


CBNT_UpstreamMat::CBNT_UpstreamMat(const std::string & name, ISvcLocator * pSvcLocator) :
  CBNT_TBRecBase(name, pSvcLocator)
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
  MsgStream log(msgSvc(), name());
  
  addBranch("UPSTREAMMAT/up_nElec", m_nElec,"/l");
  addBranch("UPSTREAMMAT/up_nPhot", m_nPhot,"/l");
  
  addBranch("UPSTREAMMAT/up_ElecEne", m_ElecEne);
  addBranch("UPSTREAMMAT/up_ElecAng", m_ElecAng);

  addBranch("UPSTREAMMAT/up_PhotEne", m_PhotEne);
  addBranch("UPSTREAMMAT/up_PhotAng", m_PhotAng);

  return StatusCode::SUCCESS;
}

StatusCode CBNT_UpstreamMat::CBNT_execute() {
  MsgStream log(msgSvc(), name());
  //Set to zero:
  m_nElec=0;
  m_nPhot=0;

  if (!evtStore()->contains<TrackRecordCollection>(m_key)) {
    if (!m_notFound) {
      log << MSG::WARNING << "TrackRecordCollection with key " << m_key << " not found" << endmsg;
      m_notFound=true;
    }
    return StatusCode::SUCCESS;
  }
  const TrackRecordCollection *trackRecordCollection(0);
  ATH_CHECK( evtStore()->retrieve(trackRecordCollection, m_key) );

  for (const TrackRecord& trackRecord : *trackRecordCollection) {
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
      log << MSG::INFO << "Found particle code " << pdgCode << endmsg; 
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
