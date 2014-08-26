/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4H6SD/LArGeoH62004EventAction.h"

#include "TBEvent/TBEventInfo.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "StoreGate/StoreGateSvc.h"

//#include "ParticleGenerator/ParticleManager.h"

#include "LArG4RunControl/LArGeoTB2004Options.h"

#include <iomanip>

static LArGeoH62004EventAction sa("LArGeoH62004EventAction");
int LArGeoH62004EventAction::m_evnum=0;

LArGeoH62004EventAction::LArGeoH62004EventAction(std::string s):UserAction(s)
{
  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap.h
  StatusCode status = svcLocator->service("MessageSvc", m_msgSvc);
  if (status.isFailure()) {
     std::cout << "LArGeoH62004EventAction::LArGeoH62004EventAction could not get the MessageSvc !"<<std::endl;
  }
  MsgStream msgStr = MsgStream(m_msgSvc, "LArGeoH62004EventAction");
  
  msgStr << MSG::DEBUG  <<"LArGeoH62004EventAction::LArGeoH62004EventAction constructor"<<endreq;

  status = svcLocator->service("StoreGateSvc", m_storeGate);
  if (status.isFailure()) {
     msgStr << MSG::ERROR << " could not fetch the StoraGateSvc !!!" << endreq;
  }
  status = svcLocator->service("DetectorStore", m_detectorStore);

  if ( !status.isSuccess()  ||  m_detectorStore == 0 ) {
        msgStr << MSG::FATAL << "could not access detector store - " << status << endreq;
        throw GaudiException("Could not get DetectorStore","LArGeoH62004EventAction",StatusCode::FAILURE);
  }
  status = m_detectorStore->retrieve(m_largeoTB2004Options, "LArGeoTB2004Options");
  if(status.isFailure()) {
     msgStr << MSG::WARNING << "\tCan't access LArGeoTB2004Options, using default values" << endreq;
     m_largeoTB2004Options = 0;
  }
}

void LArGeoH62004EventAction::EndOfEventAction(const G4Event * /*theEvent*/)
{ 
   MsgStream msgStr = MsgStream(m_msgSvc, "LArGeoH62004EventAction");
   msgStr << MSG::DEBUG <<"LArGeoH62004EventAction::EndOfEventAction"<<endreq;
   StatusCode status;

   //float beamm = PG::ParticleManager::GetInstance()->getEnergy(0);
   float beamm = 0.;
   //float ypos = PG::KinematicManager::GetManager()->getValue(PG::kTargetY);
   float ypos,xpos;
   if(m_largeoTB2004Options) {
      ypos = m_largeoTB2004Options->TableYPosition();
      xpos = m_largeoTB2004Options->CryoXPosition();
   } else {
      xpos = ypos = 0.;
   }
   //int pdg = PG::ParticleManager::GetInstance()->getPDG(0);
   int pdg = 0;
   int evtype = 1; // Physics !!!!

   msgStr << MSG::DEBUG << "TBEventInfo: "<<m_evnum<<"/"<<pdg<<"/"<<evtype<<"/"<<0<<"/"<<beamm<<"/"<<""<<"/"<<xpos<<"/"<<0<<"/"<<ypos << endreq;
   TBEventInfo *ev = new TBEventInfo(++m_evnum,pdg,evtype,0,beamm,"",xpos,0,ypos);
   status = m_storeGate->record(ev,"TBEventInfo");
   if (status.isFailure()) {
       msgStr << MSG::ERROR << "Failed to record  TBEventInfo in StoreGate!" << endreq;
   }
   status = m_storeGate->setConst(ev);
   if (status.isFailure()) {
       msgStr << MSG::ERROR << "Failed to lock  TBEventInfo in StoreGate!" << endreq;
   }
 return;
}







