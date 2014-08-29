/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//================================================================//
// Chiara Debenedetti, 29/08/2011, CaloCalibrationHitsTestTool.cxx //
//================================================================//

#include "CaloCalibrationHitsTestTool.h"
#include "GeoAdaptors/GeoCaloCalibHit.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include <TH1.h>
#include <TH2D.h>
#include <TProfile.h>

CaloCalibrationHitsTestTool::CaloCalibrationHitsTestTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SimTestToolBase(type, name, parent),m_calibHitType("LArActive"),
    m_eta(0), m_phi(0), m_eEM(0),
    m_eNonEM(0), m_eInv(0), m_eEsc(0),
    m_eTot(0), m_rz(0), m_etaphi(0),
    m_eTot_partID(0), m_eTot_eta(0), m_eTot_phi(0),
    m_partID_large(0), m_partID_small(0)
{
  declareProperty("CalibHitType",  m_calibHitType="LArActive");
}

StatusCode CaloCalibrationHitsTestTool::initialize(){

  //--discriminate between different sections--//
  if (m_calibHitType=="LArActive" || m_calibHitType=="LArInactive" || m_calibHitType=="LArDeadMaterial" ) {
    m_path+="Calib/LAr/";
    m_hitcollkey="LArCalibrationHit"+m_calibHitType.substr(3);
  } else if (m_calibHitType=="TileActiveCell" || m_calibHitType=="TileInactiveCell" || m_calibHitType=="TileDeadMaterial") {
    m_path+="Calib/Tile/";
    m_hitcollkey="TileCalibHit"+m_calibHitType.substr(4);
  } else if (m_calibHitType=="TileDM" || m_calibHitType=="TileCell") { //old naming convention for Tile CaloCalibrationHits
     m_path+="Calib/Tile/";
     m_hitcollkey="TileCalibration"+m_calibHitType.substr(4)+"HitCnt";
  } else{
    ATH_MSG(ERROR) << " unknown key " <<m_calibHitType<<endreq;
    return StatusCode::FAILURE;
  }

  //voglio: eta, phi, tutte le energie, rz, etaphi, 
  // particle ID vs Etot Prof, eta Etot prof, phi etot prof
  _TH1D(m_eta,(m_calibHitType+"_eta").c_str(),25,-5.,5.);
  _TH1D(m_phi,(m_calibHitType+"_phi").c_str(),25,-5.,5.);
  _TH1D(m_eEM,(m_calibHitType+"_eEM").c_str(),100,0.,500.);
  _TH1D(m_eNonEM,(m_calibHitType+"_eNonEM").c_str(),100,0.,500.);
  _TH1D(m_eInv,(m_calibHitType+"_eInv").c_str(),100,0.,500.);
  _TH1D(m_eEsc,(m_calibHitType+"_eEsc").c_str(),100,0.,500.);
  _TH1D(m_eTot,(m_calibHitType+"_eTot").c_str(),100,0.,500.);

  _TH2D(m_rz,(m_calibHitType+"_rz").c_str(),100,-5200.,5200.,100,0.,3000.);
  _TH2D(m_etaphi,(m_calibHitType+"_etaphi").c_str(),100,-5.,5.,100,-4.,4.);

  _TH1D(m_partID_large,(m_calibHitType+"_partID1").c_str(),100,0.,1500000.);
  m_partID_large -> StatOverflows();
  _TH1D(m_partID_small,(m_calibHitType+"_partID2").c_str(),100,10000.,10100.);

  _TH1D_WEIGHTED(m_eTot_partID,(m_calibHitType+"_eTot_partID").c_str(),600,0.,300000.);
  _TH1D_WEIGHTED(m_eTot_eta,(m_calibHitType+"_eTot_eta").c_str(),25,-5.,5.); 
  _TH1D_WEIGHTED(m_eTot_phi,(m_calibHitType+"_eTot_phi").c_str(),25,-3.2,3.2);

  return StatusCode::SUCCESS;
}

StatusCode CaloCalibrationHitsTestTool::processEvent(){

  CaloCalibrationHitContainer::const_iterator hit;

  const DataHandle<CaloCalibrationHitContainer> iter;
  CHECK(evtStore()->retrieve(iter,m_hitcollkey));
  for(hit=(*iter).begin(); hit != (*iter).end(); hit++){

    GeoCaloCalibHit geoHit(**hit, m_hitcollkey);
    if (!geoHit) continue;
    const CaloDetDescrElement* ddElement = geoHit.getDetDescrElement();

    if (!ddElement) {
      ATH_MSG(WARNING) << " could not retrieve DetElement in CaloCalibrationHitsTestTool/" <<m_calibHitType<<endreq;
      continue;
    }
    double eta = ddElement->eta();
    double phi = ddElement->phi();
    double radius = ddElement->r();
    double z = ddElement->z();
    double emEnergy = geoHit.energyEM();
    double nonEmEnergy = geoHit.energyNonEM();
    double invEnergy = geoHit.energyInvisible();
    double escEnergy = geoHit.energyEscaped();
    double totEnergy = geoHit.energyTotal();
    double particleID = (*hit)->particleID();

    m_eta->Fill(eta);
    m_phi->Fill(phi);
    m_eEM->Fill(emEnergy);
    m_eNonEM->Fill(nonEmEnergy);
    m_eInv->Fill(invEnergy);
    m_eEsc->Fill(escEnergy);
    m_eTot->Fill(totEnergy);

    m_rz->Fill(z, radius);
    m_etaphi->Fill(eta, phi);

    // particle id
    m_partID_small->Fill(particleID);
    m_partID_large->Fill(particleID);
    // weighted histos
    m_eTot_partID->Fill(particleID, totEnergy);
    m_eTot_eta->Fill(eta, totEnergy);
    m_eTot_phi->Fill(phi, totEnergy);   
  }

  return StatusCode::SUCCESS;
}
