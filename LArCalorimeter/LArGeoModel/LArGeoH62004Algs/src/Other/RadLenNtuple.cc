/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArGeoH62004Algs/RadLenNtuple.h"
#include "LArG4RunControl/LArGeoTBH1GeoOptions.h"
#include "AtlasHepMC/GenEvent.h"
#include "GeneratorObjects/McEventCollection.h"
#include <iostream>
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/INTupleSvc.h" 
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "StoreGate/StoreGateSvc.h"
#include "SimHelpers/ServiceAccessor.h"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "GaudiKernel/MsgStream.h"

static RadLenNtuple sntp("RadLenNtuple");

static bool has_cryo, has_em, has_hec, has_fcal;

void RadLenNtuple::BeginOfEventAction(const G4Event* anEvent)
{
   has_cryo = has_em = has_hec = has_fcal = false;
//   m_tot_x = m_cryo_x = m_em_x = m_hec_x = m_fcal_x = 0.;
//   m_tot_ni = m_cryo_ni = m_em_ni = m_hec_ni = m_fcal_ni = 0.;
//   m_fcal_y = m_em_y = m_hec_y = m_cryo_y = 0.;
}

void RadLenNtuple::EndOfEventAction(const G4Event* anEvent)
{

   m_xcoord = -10000.;

   /*
   StoreGateSvc* detStore;
   const LArGeoTBH1GeoOptions      *largeoTBH1geoOptions ;
  
   StatusCode status;
   ISvcLocator* svcLocator = Gaudi::svcLocator(); 
   status = svcLocator->service("DetectorStore", detStore);

   if( status.isSuccess() ) {
      status = detStore->retrieve(largeoTBH1geoOptions, "LArGeoTBH1GeoOptions");
      if ( !status.isFailure() ) {
	 m_xcoord = largeoTBH1geoOptions->CryoXPosition();
      }
   }
   */
   StoreGateSvc* m_storeGate;
   const McEventCollection* m_mcEvtColl;
    // Get a handle to StoreGate
   ISvcLocator* svcLocator = Gaudi::svcLocator();
  if ( svcLocator->service("StoreGateSvc", m_storeGate).isSuccess() ) {
     if (! m_storeGate->retrieve(m_mcEvtColl, "GEN_EVENT").isFailure() ) {
	 McEventCollection::const_iterator iEvt = m_mcEvtColl->begin(); 
	 HepMC::GenEvent::particle_const_iterator p = (*iEvt)->particles_begin();
	 m_xcoord = (*p)->production_vertex()->position().x();  
     }
  }
   
   ntupleSvc()->writeRecord("/NTUPLES/FILE1/RadLenNtuple/radlen");
 
}

void RadLenNtuple::BeginOfRunAction(const G4Run* aRun)
{

  MsgStream log(msgSvc(), "RadLenNtuple");

  NTupleFilePtr file1(ntupleSvc(), "/NTUPLES/FILE1");

  SmartDataPtr<NTuple::Directory>
    ntdir(ntupleSvc(),"/NTUPLES/FILE1/RadLenNtuple");
  if ( !ntdir )
    { 
      //    otherwise create the directory
      ntdir = ntupleSvc()->createDirectory(file1,"RadLenNtuple");
    }
  if ( ! ntdir )  
    {
      log << MSG::ERROR << " failed to get ntuple directory" << endmsg;
    }
  
  NTuplePtr nt(ntupleSvc(), "/NTUPLES/FILE1/RadLenNtuple/radlen");

  if ( !nt )    {    // Check if already booked
 
   nt = ntupleSvc()->book (ntdir.ptr(), "radlen",CLID_ColumnWiseTuple, "Some Description");
    if ( nt )    {
      
      
      log << MSG::DEBUG << "booked ntuple " << endmsg;

      // WARNING!! Force limit to 50k tracks
      
      StatusCode status = nt->addItem ("tot_x", m_tot_x);
      status = nt->addItem ("tot_ni", m_tot_ni);
      status = nt->addItem ("cryo_x", m_cryo_x);
      status = nt->addItem ("cryo_ni", m_cryo_ni);
      status = nt->addItem ("em_x", m_em_x);
      status = nt->addItem ("em_ni", m_em_ni);
      status = nt->addItem ("hec_x", m_hec_x);
      status = nt->addItem ("hec_ni", m_hec_ni);
      status = nt->addItem ("fcal_x", m_fcal_x);
      status = nt->addItem ("fcal_ni", m_fcal_ni);
      status = nt->addItem ("cryo_y", m_cryo_y);
      status = nt->addItem ("em_y", m_em_y);
      status = nt->addItem ("hec_y", m_hec_y);
      status = nt->addItem ("fcal_y", m_fcal_y);
      status = nt->addItem ("coord_x", m_xcoord);
      
    } else {   // did not manage to book the N tuple....
      
      log << MSG::ERROR << "Could not book ntuple!! " << endmsg;
      
    }
  }   
  
}

void RadLenNtuple::EndOfRunAction(const G4Run* aRun)
{
}
void RadLenNtuple::SteppingAction(const G4Step* aStep)
{
   MsgStream log(msgSvc(), "RadLenNtuple");

   G4StepPoint *preStep=aStep->GetPreStepPoint();
   G4TouchableHistory* touchHist = (G4TouchableHistory*)aStep->GetPreStepPoint()->GetTouchable();
   G4LogicalVolume *lv=touchHist->GetVolume()->GetLogicalVolume();
   std::string volName=lv->GetName();
   G4Material *mat=lv->GetMaterial();
   double intl=mat->GetNuclearInterLength();
   double stepl=aStep->GetStepLength();
   double thickstep=stepl/intl;
   double radl=mat->GetRadlen();
   double radstep=100 * stepl/radl;
   std::string::size_type npos;
   m_tot_x += thickstep;
   m_tot_ni += radstep;
   if(!has_cryo) {
      m_cryo_x += thickstep;
      m_cryo_ni += radstep;
      npos=volName.find("Cryostat");
      if(npos< volName.size()){
         has_cryo = true;
         m_cryo_y = preStep->GetPosition().y();
         log << MSG::DEBUG <<" Has cryo: "<<m_cryo_y <<endmsg;
      }
   }
   if(!has_em) {
      m_em_x += thickstep;
      m_em_ni += radstep;
      npos=volName.find("EMEC");
      if(npos< volName.size()){
         has_em = true;
         m_em_y = preStep->GetPosition().y();
         log << MSG::DEBUG <<" Has EMEC: "<<m_em_y <<endmsg;
      }
   }
   if(!has_hec) {
      m_hec_x += thickstep;
      m_hec_ni += radstep;
      npos=volName.find("HEC");
      if(npos< volName.size()){
         has_hec = true;
         m_hec_y = preStep->GetPosition().y();
         log << MSG::DEBUG <<" Has HEC: "<<m_hec_y <<endmsg;
      }
   }
   if(!has_fcal) {
      m_fcal_x += thickstep;
      m_fcal_ni += radstep;
      npos=volName.find("FCAL");
      if(npos< volName.size()){
         has_fcal = true;
         m_fcal_y = preStep->GetPosition().y();
         log << MSG::DEBUG <<" Has FCAL: "<<m_fcal_y <<endmsg;
      }
   }
}
