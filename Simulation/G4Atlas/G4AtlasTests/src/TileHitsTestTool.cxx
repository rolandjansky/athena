/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileHitsTestTool.h"

#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileTBID.h"
#include "TileSimEvent/TileHit.h"
#include "TileSimEvent/TileHitVector.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "TileDetDescr/TileDetDescrManager.h"
#include <TH2D.h>
#include <TH1.h>

TileHitsTestTool::TileHitsTestTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SimTestToolBase(type, name, parent),
    m_tileID(0), m_tileTBID(0), m_tileMgr(0),
    m_tile_eta(0), m_tile_phi(0), m_tile_energy(0), m_tile_log_energy(0), m_tile_time(0),
    m_tile_rz(0), m_tile_etaphi(0), m_tile_energyphi(0), m_tile_energyeta(0),
    m_mbts_side(0), m_mbts_eta(0), m_mbts_phi(0),
    m_mbts_sidetaphi(0),
    m_etot(0),
    m_testMBTS(true)
{
  declareProperty("TestMBTS",m_testMBTS,"");
  }


StatusCode TileHitsTestTool::initialize() {
  CHECK(detStore()->retrieve(m_tileMgr));
  CHECK(detStore()->retrieve(m_tileID));
  CHECK(detStore()->retrieve(m_tileTBID));

  std::string detName("Tile"); //could configure this in the future?
  m_path+=detName+"/";

  // TileCal histograms
  _TH1D(m_tile_eta,"tile_eta",17,-1.65,1.65);
  _TH1D(m_tile_phi,"tile_phi",16,-3.2,3.2);
  _TH1D(m_tile_energy,"tile_energy",50,0,100);
  _TH1D(m_tile_log_energy,"tile_log_energy",100,-14.,8.);
  _SET_TITLE(m_tile_log_energy,"logarithm of energy deposits","log(energy [MeV])","dN/dlog(energy [MeV])");
  _TH1D(m_tile_time,"tile_time",100,0,250);

  _TH2D(m_tile_rz,"tile_rz",16,-6200.,6200.,25,1500.,4000.);
  _TH2D(m_tile_etaphi,"tile_etaphi",65,-1.625,1.625,129,-3.175,3.175);
  _TH2D(m_tile_energyphi,"tile_energyphi",100,0.,10.,129,-3.175,3.175);
  _TH2D(m_tile_energyeta,"tile_energyeta",100,0.,10.,129,-3.175,3.175);

  // MBTS histograms
  _TH1D(m_mbts_side,"mbts_side",3,-1.5,1.5);
  _TH1D(m_mbts_eta,"mbts_eta",2,-0.5,1.5);
  _TH1D(m_mbts_phi,"mbts_phi",8,-0.5,7.5);
  _TH2D(m_mbts_sidetaphi,"mbts_sidetaphi",5,-2.5,2.5,7,-0.5,7.5);

  _TH1D(m_etot,(detName+"_etot").c_str(),100,0.,500.);

  return StatusCode::SUCCESS;
}


StatusCode TileHitsTestTool::processEvent() {
  // make sure we still have a TileMgr (SimFlags.ReleaseGeoModel=False)
  CHECK(detStore()->retrieve(m_tileMgr));

  const TileHitVector* hitVec;
  CHECK(evtStore()->retrieve(hitVec,"TileHitVec"));

  double etot=0.;
  for (TileHitVecConstIterator i_hit=hitVec->begin() ; i_hit!=hitVec->end() ; ++i_hit) {

    Identifier pmt_id = (*i_hit).identify();
    Identifier cell_id = m_tileID->cell_id(pmt_id);

    const CaloDetDescrElement *ddElement = m_tileMgr->get_cell_element(cell_id);
    if(ddElement) // skip E4' cells which are not yet described in TileDetDescrManager.
      {
        double eta = ddElement->eta();
        double phi = ddElement->phi();
        double radius = ddElement->r();
        double z  = ddElement->z();

        int pmt = m_tileID->pmt(pmt_id);
        if (pmt>0) phi += ddElement->dphi()/2.; // to see clearly two PMTs in a cell

        // loop over subhits
        double energy=0;
        //    ATH_MSG(INFO) << "TileHit size :" <<(*i_hit).size()<<endreq;
        for (int i=0; i<(*i_hit).size();++i) {
          energy+=(*i_hit).energy(i);
          m_tile_energy->Fill((*i_hit).energy(i));
          m_tile_log_energy->Fill( log((*i_hit).energy(i)) );
          m_tile_time->Fill((*i_hit).time(i));
        }
        etot+=energy;

        m_tile_eta->Fill(eta);
        m_tile_phi->Fill(phi);
        m_tile_rz->Fill(z,radius);
        m_tile_etaphi->Fill(eta,phi);
        m_tile_energyphi->Fill(energy,phi);
        m_tile_energyeta->Fill(energy,eta);
      }
  }
  //Check the Hit container with postfix _Fast which generated with FastCaloSim
  const TileHitVector *hitVec_fast;
  if(  evtStore()->contains<TileHitVector>("TileHitVec_Fast") &&
       (evtStore()->retrieve(hitVec_fast,"TileHitVec_Fast")).isSuccess())
    {
      ATH_MSG_DEBUG ( "Retrieve FastCaloSim container TileHitVec_Fast." );
      for(TileHitVecConstIterator i_hit=hitVec_fast->begin();i_hit!=hitVec_fast->end();++i_hit)
        {
          Identifier pmt_id=i_hit->identify();
          Identifier cell_id=m_tileID->cell_id(pmt_id);
          const CaloDetDescrElement *ddElement=m_tileMgr->get_cell_element(cell_id);
          if(ddElement) // skip E4' cells which are not yet described in TileDetDescrManager.
            {
              double eta=ddElement->eta();
              double phi=ddElement->phi();
              double radius=ddElement->r();
              double z=ddElement->z();

              int pmt=m_tileID->pmt(pmt_id);
              if(pmt>0) phi+=ddElement->dphi()/2.;
              double energy=0;
              for(int i=0;i<(*i_hit).size();++i)
                {
                  energy+=(*i_hit).energy(i);
                  m_tile_energy->Fill((*i_hit).energy(i));
                  m_tile_log_energy->Fill( log((*i_hit).energy(i))) ;
                  m_tile_time->Fill((*i_hit).time(i));
                }
              etot+=energy;

              m_tile_eta->Fill(eta);
              m_tile_phi->Fill(phi);
              m_tile_rz->Fill(z,radius);
              m_tile_etaphi->Fill(eta,phi);
              m_tile_energyphi->Fill(energy,phi);
              m_tile_energyeta->Fill(energy,eta);
            }
        }
    }

  if(m_testMBTS) {
    CHECK(evtStore()->retrieve(hitVec,"MBTSHits"));

    for (TileHitVecConstIterator i_hit=hitVec->begin() ; i_hit!=hitVec->end() ; ++i_hit) {
      Identifier mbts_id = (*i_hit).identify();
      double side = m_tileTBID->side(mbts_id); // -1 or +1
      double ieta = m_tileTBID->eta(mbts_id);  // 0 for inner cell, 1 for outer cell
      double iphi = m_tileTBID->phi(mbts_id);  // 0-7, cell 0 at phi=0
      m_mbts_side->Fill(side);
      m_mbts_eta->Fill(ieta);
      m_mbts_phi->Fill(iphi);
      m_mbts_sidetaphi->Fill((ieta+1)*side,iphi);
    }
  }
  // const HepMC::GenParticle *primary = getPrimary();
  // if (primary) {
  //   m_etot_eta->Fill(primary->momentum().eta(),etot);
  //   m_etot_phi->Fill(primary->momentum().phi(),etot);
  // }
  m_etot->Fill(etot);



  return StatusCode::SUCCESS;
}
