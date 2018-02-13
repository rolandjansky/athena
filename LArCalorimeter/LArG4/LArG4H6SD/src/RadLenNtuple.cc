/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RadLenNtuple.h"
#include "LArG4RunControl/LArGeoTBH1GeoOptions.h"
#include "HepMC/GenEvent.h"
#include <iostream>
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartDataPtr.h"
//#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "SimHelpers/ServiceAccessor.h"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "AthenaKernel/errorcheck.h"

namespace G4UA
{
  // ADS why not class members?
  static bool has_cryo, has_em, has_hec, has_fcal;

  void RadLenNtuple::BeginOfEventAction(const G4Event* /*anEvent*/)
  {
    has_cryo = has_em = has_hec = has_fcal = false;
    //   m_tot_x = m_cryo_x = m_em_x = m_hec_x = m_fcal_x = 0.;
    //   m_tot_ni = m_cryo_ni = m_em_ni = m_hec_ni = m_fcal_ni = 0.;
    //   m_fcal_y = m_em_y = m_hec_y = m_cryo_y = 0.;
  }

  void RadLenNtuple::EndOfEventAction(const G4Event* /*anEvent*/)
  {

    m_xcoord = -10000.;

    if (m_mcEvtColl.isValid()) {
      McEventCollection::const_iterator iEvt = m_mcEvtColl->begin();
      HepMC::GenEvent::particle_const_iterator p = (*iEvt)->particles_begin();
      m_xcoord = (*p)->production_vertex()->point3d().x();
    }
   
    ntupleSvc()->writeRecord("/NTUPLES/FILE1/RadLenNtuple/radlen");

  }

  StatusCode RadLenNtuple::initialize(){
  
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
        std::cout << "RadLenNtuple ERROR  failed to get ntuple directory" << std::endl;
      }
  
    NTuplePtr nt(ntupleSvc(), "/NTUPLES/FILE1/RadLenNtuple/radlen");
  
    if ( !nt )    {    // Check if already booked
    
      nt = ntupleSvc()->book (ntdir.ptr(), "radlen",CLID_ColumnWiseTuple, "Some Description");
      if ( nt )    {
      
      
        if(m_verboseLevel>4) { std::cout << "booked ntuple " << std::endl; }
      
        // WARNING!! Force limit to 50k tracks
      
        CHECK( nt->addItem ("tot_x", m_tot_x) );
        CHECK( nt->addItem ("tot_ni", m_tot_ni) );
        CHECK( nt->addItem ("cryo_x", m_cryo_x) );
        CHECK( nt->addItem ("cryo_ni", m_cryo_ni) );
        CHECK( nt->addItem ("em_x", m_em_x) );
        CHECK( nt->addItem ("em_ni", m_em_ni) );
        CHECK( nt->addItem ("hec_x", m_hec_x) );
        CHECK( nt->addItem ("hec_ni", m_hec_ni) );
        CHECK( nt->addItem ("fcal_x", m_fcal_x) );
        CHECK( nt->addItem ("fcal_ni", m_fcal_ni) );
        CHECK( nt->addItem ("cryo_y", m_cryo_y) );
        CHECK( nt->addItem ("em_y", m_em_y) );
        CHECK( nt->addItem ("hec_y", m_hec_y) );
        CHECK( nt->addItem ("fcal_y", m_fcal_y) );
        CHECK( nt->addItem ("coord_x", m_xcoord) );
      
      } else {   // did not manage to book the N tuple....
      
        std::cout << "RadLenNtuple ERROR Could not book ntuple!! " << std::endl;
      
      }
    }

    return StatusCode::SUCCESS;
  }



  void RadLenNtuple::UserSteppingAction(const G4Step* aStep)
  {

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
        if(m_verboseLevel>4) { std::cout <<"RadLenNtuple DEBUG  Has cryo: "<<static_cast<float>(m_cryo_y) << std::endl; }
      }
    }
    if(!has_em) {
      m_em_x += thickstep;
      m_em_ni += radstep;
      npos=volName.find("EMEC");
      if(npos< volName.size()){
        has_em = true;
        m_em_y = preStep->GetPosition().y();
        if(m_verboseLevel>4) { std::cout << "RadLenNtuple DEBUG  Has EMEC: "<<static_cast<float>(m_em_y) << std::endl; }
      }
    }
    if(!has_hec) {
      m_hec_x += thickstep;
      m_hec_ni += radstep;
      npos=volName.find("HEC");
      if(npos< volName.size()){
        has_hec = true;
        m_hec_y = preStep->GetPosition().y();
        if(m_verboseLevel>4) { std::cout <<"RadLenNtuple DEBUG  Has HEC: "<<static_cast<float>(m_hec_y) << std::endl; }
      }
    }
    if(!has_fcal) {
      m_fcal_x += thickstep;
      m_fcal_ni += radstep;
      npos=volName.find("FCAL");
      if(npos< volName.size()){
        has_fcal = true;
        m_fcal_y = preStep->GetPosition().y();
        if(m_verboseLevel>4) { std::cout <<"RadLenNtuple DEBUG  Has FCAL: "<<static_cast<float>(m_fcal_y) << std::endl; }
      }
    }
  }

}
