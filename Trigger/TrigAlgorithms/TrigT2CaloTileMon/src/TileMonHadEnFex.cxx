/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileMonHadEnFex.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloTileMon
//
// AUTHOR:   Jens Damaske
//
// ********************************************************************

#include "TrigCaloEvent/TrigEMCluster.h"
#include "TrigT2CaloTileMon/TrigT2TileJet.h"
#include "TrigT2CaloTileMon/TrigT2Tower.h"
#include "TrigCaloEvent/Trig3Momentum.h"
#include "CaloGeoHelpers/CaloSampling.h"

#include "TrigT2CaloTileMon/TileMonHadEnFex.h"
#include "TrigT2CaloCommon/Calo_Def.h"

#include <math.h>
#include <TMath.h>
#include <string>


TileMonHadEnFex::TileMonHadEnFex(const std::string & type, const std::string & name,
                                 const IInterface* parent) :
               IAlgToolCalo(type, name, parent),
               m_thistSvc("THistSvc/THistSvc","TileMonHadEnFex"),
               m_THR_1(400),
               m_THR_2(100),
	       m_hists(NULL),
	       //m_hist_TB0(NULL),
	       //m_hist_TB1(NULL),
	       //m_hist_TB2(NULL),
	       m_2dhists(NULL),
	       m_2dhists_TB0(NULL),
	       m_2dhists_TB1(NULL),
	       m_2dhists_TB2(NULL)
	       //m_mainHist(NULL)
{
   declareProperty("THistSvc",m_thistSvc,"THistSvc output");
   declareProperty("HistogramsPath",m_histPath="/EXPERT/T2CaloTileMon");
   declareProperty("THR_1",m_THR_1);
   declareProperty("THR_2",m_THR_2);
   m_end_detec_eta = 1.6;
   m_end_detec_phi = M_PI;
   m_eta_division =  0.1;
   m_phi_division =  M_PI/32.;
   m_energyCell_Thr =  0.01; //Threshold in Gev
   m_configured=false;
}

TileMonHadEnFex::~TileMonHadEnFex(){
}

StatusCode TileMonHadEnFex::initialize(){ 
   if ( IAlgToolCalo::initialize().isFailure() )
      return StatusCode::FAILURE;
   if ( m_thistSvc.retrieve().isFailure() )
      return StatusCode::FAILURE;
   IIncidentSvc* p_incSvc;
   if ( service("IncidentSvc",p_incSvc, true).isFailure() ) {
      return StatusCode::FAILURE;
   }
   else
   {
      p_incSvc->addListener(this, "BeginRun",100);
   }

   return StatusCode::SUCCESS;
}


double TileMonHadEnFex::logEnergy(double energy)
{
   energy = energy/1.e3; // energy in GeV
   energy = TMath::Log(energy);
   return energy;
}

/**
* @brief changes the Y-axis binning of a given histogram to a logarithmic scale
* 
* NOT SUPPORTED BY ONLINE HISTO GATHERER
*
* For a given histogram the Y-axis binning is changed form a linear to a logarithmic scale
* Take care, that the axis minimum is > 0
*
* @param[in]   h  pointer to the histogram
*/
// void TileMonHadEnFex::BinLogY(TH2* h)
// {
//    TAxis *axis = h->GetYaxis();
//    int bins = axis->GetNbins();
//    Axis_t min = axis->GetXmin();
//    Axis_t max = axis->GetXmax();
//    Axis_t width = (max - min) / bins;
//    Axis_t *new_bins = new Axis_t[bins + 1];
//  
//    double logWidth = ( TMath::Log(max)-TMath::Log(min) ) / bins;
// 
// //    cout << "min:" << min << " max:" << max << " bins:" << bins << " logWidth:" << logWidth << endl;
//    
//    for (int i = 0; i <= bins; i++) {
// //      cout << min + i * width << "\t";
//       new_bins[i] = min*TMath::Exp(i * logWidth);
// //      cout << new_bins[i] << endl;
//    }
//    axis->Set(bins, new_bins);
//    delete new_bins;
// }

void TileMonHadEnFex::handle( const Incident& )
{
   m_configured=false;

   m_2dhists = new TH2F* [(int)floorf((2*m_end_detec_eta)/m_eta_division)];             //2D histo Et vs phi
   m_2dhists_TB0 = new TH2F* [(int)floorf((2*m_end_detec_eta)/m_eta_division)];         //2D histo Et vs phi for TileBar0 and TileExt0
   m_2dhists_TB1 = new TH2F* [(int)floorf((2*m_end_detec_eta)/m_eta_division)];         //2D histo Et vs phi for TileBar1 and TileExt1
   m_2dhists_TB2 = new TH2F* [(int)floorf((2*m_end_detec_eta)/m_eta_division)];         //2D histo Et vs phi for TileBar2 and TileExt2

   for(float eta=-m_end_detec_eta;eta<m_end_detec_eta;eta+=m_eta_division)
   {
      int hist_grid_eta=(int)floorf(((eta)+m_end_detec_eta)/m_eta_division);
      char name_2dhist[60];
      char name_2dhist_TB0[60];
      char name_2dhist_TB1[60];
      char name_2dhist_TB2[60];
      char name_2dhist_toreg[100];
      char name_2dhist_toreg_TB0[100];
      char name_2dhist_toreg_TB1[100];
      char name_2dhist_toreg_TB2[100];

      float etaN = eta;
      float epsilon = 0.00001;

      sprintf(name_2dhist,"TowerMonitor_eta=%-2.1f_%-2.1f",etaN,etaN+m_eta_division); //Tower
      if (etaN <= (-1.1 + epsilon) || etaN >= (1.0 - epsilon) ) //Extended Barrel - Cell
      {
         sprintf(name_2dhist_TB0,"TE0_CellMonitor_eta=%-2.1f_%-2.1f",etaN,etaN+m_eta_division);
         sprintf(name_2dhist_TB1,"TE1_CellMonitor_eta=%-2.1f_%-2.1f",etaN,etaN+m_eta_division);
         sprintf(name_2dhist_TB2,"TE2_CellMonitor_eta=%-2.1f_%-2.1f",etaN,etaN+m_eta_division);
      }
      else if( etaN > (-1.1 + epsilon) && etaN < (1.0 - epsilon) ) //Central Barrel - Cell
      {
         sprintf(name_2dhist_TB0,"TB0_CellMonitor_eta=%-2.1f_%-2.1f",etaN,etaN+m_eta_division);
         sprintf(name_2dhist_TB1,"TB1_CellMonitor_eta=%-2.1f_%-2.1f",etaN,etaN+m_eta_division);
         sprintf(name_2dhist_TB2,"TB2_CellMonitor_eta=%-2.1f_%-2.1f",etaN,etaN+m_eta_division);
      }

      sprintf(name_2dhist_toreg,"%s/TowerMonitor_n=%-2.1f",m_histPath.c_str(),etaN); //Tower
      if (etaN <= (-1.1 + epsilon) || etaN >= (1.0 - epsilon) ) //Extended Barrel - Cell
      {
         sprintf(name_2dhist_toreg_TB0,"%s/TileExt0/TE0_CellMonitor_n=%-2.1f",m_histPath.c_str(),etaN);
         sprintf(name_2dhist_toreg_TB1,"%s/TileExt1/TE1_CellMonitor_n=%-2.1f",m_histPath.c_str(),etaN);
         sprintf(name_2dhist_toreg_TB2,"%s/TileExt2/TE2_CellMonitor_n=%-2.1f",m_histPath.c_str(),etaN);
      }
      else if( etaN > (-1.1 + epsilon) && etaN < (1.0 - epsilon) ) //Central Barrel - Cell
      {
         sprintf(name_2dhist_toreg_TB0,"%s/TileBar0/TB0_CellMonitor_n=%-2.1f",m_histPath.c_str(),etaN);
         sprintf(name_2dhist_toreg_TB1,"%s/TileBar1/TB1_CellMonitor_n=%-2.1f",m_histPath.c_str(),etaN);
         sprintf(name_2dhist_toreg_TB2,"%s/TileBar2/TB2_CellMonitor_n=%-2.1f",m_histPath.c_str(),etaN);
      }

      std::string str1 =name_2dhist;
      std::string str2=name_2dhist_TB0;
      std::string str3=name_2dhist_TB1;
      std::string str4=name_2dhist_TB2;
      std::string str5=name_2dhist_toreg;
      std::string str6=name_2dhist_toreg_TB0;
      std::string str7=name_2dhist_toreg_TB1;
      std::string str8=name_2dhist_toreg_TB2;

      std::string dot(".");

      if ( str1.find(".") != std::string::npos ) str1.replace(str1.find("."),dot.length(),"_");
      if ( str1.find(".") != std::string::npos ) str1.replace(str1.find("."),dot.length(),"_");

      if ( str2.find(".") != std::string::npos ) str2.replace(str2.find("."),dot.length(),"_");
      if ( str2.find(".") != std::string::npos ) str2.replace(str2.find("."),dot.length(),"_");

      if ( str3.find(".") != std::string::npos ) str3.replace(str3.find("."),dot.length(),"_");
      if ( str3.find(".") != std::string::npos ) str3.replace(str3.find("."),dot.length(),"_");

      if ( str4.find(".") != std::string::npos ) str4.replace(str4.find("."),dot.length(),"_");
      if ( str4.find(".") != std::string::npos ) str4.replace(str4.find("."),dot.length(),"_");

      if ( str5.find(".") != std::string::npos ) str5.replace(str5.find("."),dot.length(),"_");

      if ( str6.find(".") != std::string::npos ) str6.replace(str6.find("."),dot.length(),"_");

      if ( str7.find(".") != std::string::npos ) str7.replace(str7.find("."),dot.length(),"_");

      if ( str8.find(".") != std::string::npos ) str8.replace(str8.find("."),dot.length(),"_");

      strcpy(name_2dhist,str1.c_str());
      strcpy(name_2dhist_TB0,str2.c_str());
      strcpy(name_2dhist_TB1,str3.c_str());
      strcpy(name_2dhist_TB2,str4.c_str());
      strcpy(name_2dhist_toreg,str5.c_str());
      strcpy(name_2dhist_toreg_TB0,str6.c_str());
      strcpy(name_2dhist_toreg_TB1,str7.c_str());
      strcpy(name_2dhist_toreg_TB2,str8.c_str());


      /**
      * Energy range should be from 0.02GeV - 50GeV with a precision of 1%
      * we insert tower energy as ln(energy), so we need an energy axis from -4 to 4 with 800 bins
      */
      m_2dhists[hist_grid_eta] = new TH2F(name_2dhist,name_2dhist,(int)floorf(2*m_end_detec_phi/m_phi_division),-m_end_detec_phi,m_end_detec_phi,800,-4.,4.);
      m_2dhists[hist_grid_eta]->SetXTitle("#phi[#circ]");
      m_2dhists[hist_grid_eta]->SetYTitle("ln(E_{T}) [GeV]");

      m_2dhists_TB0[hist_grid_eta] = new TH2F(name_2dhist_TB0,name_2dhist_TB0,(int)floorf(2*m_end_detec_phi/m_phi_division),-m_end_detec_phi,m_end_detec_phi,800,-4.,4.);
      m_2dhists_TB0[hist_grid_eta]->SetXTitle("#phi[#circ]");
      m_2dhists_TB0[hist_grid_eta]->SetYTitle("ln(E_{T}) [GeV]");

      m_2dhists_TB1[hist_grid_eta] = new TH2F(name_2dhist_TB1,name_2dhist_TB1,(int)floorf(2*m_end_detec_phi/m_phi_division),-m_end_detec_phi,m_end_detec_phi,800,-4.,4.);
      m_2dhists_TB1[hist_grid_eta]->SetXTitle("#phi[#circ]");
      m_2dhists_TB1[hist_grid_eta]->SetYTitle("ln(E_{T}) [GeV]");

      m_2dhists_TB2[hist_grid_eta] = new TH2F(name_2dhist_TB2,name_2dhist_TB2,(int)floorf(2*m_end_detec_phi/m_phi_division),-m_end_detec_phi,m_end_detec_phi,800,-4.,4.);
      m_2dhists_TB2[hist_grid_eta]->SetXTitle("#phi[#circ]");
      m_2dhists_TB2[hist_grid_eta]->SetYTitle("ln(E_{T}) [GeV]");

      if ( m_thistSvc->regHist(name_2dhist_toreg,m_2dhists[hist_grid_eta]).isFailure() )         msg() << MSG::ERROR << "Did not register hists" << endmsg; //return StatusCode::FAILURE;
      if ( m_thistSvc->regHist(name_2dhist_toreg_TB0,m_2dhists_TB0[hist_grid_eta]).isFailure() ) msg() << MSG::ERROR << "Did not register hists" << endmsg; //return StatusCode::FAILURE;
      if ( m_thistSvc->regHist(name_2dhist_toreg_TB1,m_2dhists_TB1[hist_grid_eta]).isFailure() ) msg() << MSG::ERROR << "Did not register hists" << endmsg; //return StatusCode::FAILURE;
      if ( m_thistSvc->regHist(name_2dhist_toreg_TB2,m_2dhists_TB2[hist_grid_eta]).isFailure() ) msg() << MSG::ERROR << "Did not register hists" << endmsg; //return StatusCode::FAILURE;
   }

   //    m_mainHist = new
   //    TH2F("TT","TT",(int)floorf(2.*m_end_detec_eta/m_eta_division),-m_end_detec_eta,m_end_detec_eta,(int)floorf(2.*m_end_detec_phi/m_phi_division),-m_end_detec_phi,m_end_detec_phi);
   //    if ( m_thistSvc->regHist("/NT/TT",m_mainHist).isFailure() ) {
   // 	return StatusCode::FAILURE;
   //    }


   if ( msg().level() <= MSG::DEBUG )
     msg() << MSG::DEBUG << "Histograms correctly allocated" << endmsg; //
   m_configured=true;
   //return StatusCode::SUCCESS;
}

StatusCode TileMonHadEnFex::finalize(){
// Don't delete histograms. ROOT takes care of this.
  if ( msg().level() <= MSG::DEBUG )
    msg() << MSG::DEBUG << " REGTEST: HadEnFex:  call finalize()" << endmsg;
//  for(float eta=-m_end_detec_eta;eta<m_end_detec_eta;eta+=m_eta_division){
//    int hist_grid_eta=(int)floorf(((eta)+m_end_detec_eta)/m_eta_division);
//    for(float phi=-m_end_detec_phi;phi<m_end_detec_phi;phi+=m_phi_division){
//         int hist_grid_phi=(int)floorf(((phi)+m_end_detec_phi)/m_phi_division);
//         if ( m_hists[hist_grid_eta][hist_grid_phi]->GetMean() != 0 )
// 	std::cout << "eta : " << eta << " phi : " << phi << " " << 
// 		hist_grid_eta << " " << hist_grid_phi << " " << 
// 		m_hists[hist_grid_eta][hist_grid_phi]->GetMean() << std::endl;
// 		m_mainHist->SetBinContent(hist_grid_eta,hist_grid_phi,
// 			m_hists[hist_grid_eta][hist_grid_phi]->GetMean() );
//    }
//  }
 
 return StatusCode::SUCCESS;
}

StatusCode TileMonHadEnFex::execute(TrigEMCluster &/*rtrigEmCluster*/,double etamin,
		double etamax, double phimin, double phimax)
{
    if ( msg().level() <= MSG::DEBUG )
      msg() << MSG::DEBUG << " REGTEST: HadEnFex:  call execute()" << endmsg;

  // If there was a problem in configuration, stop now
  if ( !m_configured ) return StatusCode::SUCCESS;
        m_tiledecoded=false;
        // Time total AlgTool time
        if (!m_timersvc.empty()) m_timer[0]->start();

        // MsgStream log(msgSvc(), name());
#ifndef NDEBUG
	if ( msg().level() <= MSG::DEBUG )
	  msg() << MSG::DEBUG << "in execute(TrigEMCluster &)" << endmsg;
#endif

  CaloSampling::CaloSample samp;  

  if (!m_timersvc.empty()) {
	m_timer[1]->start();      
	m_timer[1]->pause();      
	m_timer[2]->start();      
	m_timer[2]->pause();      
	m_timer[3]->start();      
	m_timer[3]->pause();      
	m_timer[4]->start();      
	m_timer[4]->pause();      
  }

  
   // TILECAL Assuming no knowledge about first sample
  
   // Time to access RegionSelector
   if (!m_timersvc.empty()) m_timer[1]->resume();

   /**
      ToolHandle<ITrigDataAccess> m_data is an object that provides data access in a Region of Interest.
      is inherited from "/Trigger/TrigAlgorithms/TrigT2CaloCommon/IAlgToolCalo.h"! See TrigT2CaloCommon/ITrigDataAccess.h for more details.
      Region Selector, no sample needed
      Get detector offline ID's for Collections
   */
   m_data->RegionSelector(0,etamin,etamax,phimin,phimax,TILE);

   if (!m_timersvc.empty()) m_timer[1]->pause();

#ifndef NDEBUG
   if ( msg().level() <= MSG::DEBUG )
     msg() << MSG::DEBUG << " REGTEST:   TILE:  TileContSize()" << m_data->TileContSize() << endmsg;
#endif

   TrigT2TileJet jet;
   
   for (unsigned int iR=0;iR<m_data->TileContSize();iR++) { //loop over TileCal drawers
        // Time to access Collection (and ByteStreamCnv ROBs)
        if (!m_timersvc.empty()) m_timer[2]->resume();
	// For the first sample you will create the containers
	// For the others no
        if ( m_data->LoadCollections(m_itBegin,m_itEnd,iR,!iR).isFailure() ) {
	  return StatusCode::FAILURE;
	}
	// Finished to access Collection
        if (!m_timersvc.empty()) m_timer[2]->pause();
        // Algorithmic time
        if (!m_timersvc.empty()) m_timer[3]->resume();

   // Each cell must contribute to only one histogram
   for(m_itt = m_itBegin;m_itt != m_itEnd; ++m_itt) { //loop over cells


//       CaloSampling::CaloSample samp;
//       samp = CaloSampling::getSampling(*(*m_itt));
//       double cellE = (*m_itt)->e();


      const TileCell* tilecell = (*m_itt);
      double etaCell = tilecell->eta();
      double phiCell = tilecell->phi();
      double energyCell = tilecell->energy();
      //samp = CaloSampling::getSampling(*tilecell);
      samp = tilecell->caloDDE()->getSampling();

      // BAD CELLS
//       if ( tilecell->badcell() ) std::cout << "JENSONBUTTON:  e:" << energyCell/1.e3 << ", eta:" << etaCell << ", phi:" << phiCell << ", samp:" << samp << " bad:" << tilecell->badcell() << std::endl;

      // possibly do some tower building
      // Energy Eta Phi Layer
      Trig3Momentum tile_cell(energyCell,etaCell,phiCell,samp);
      jet.insertCell(tile_cell, msg());

   } // end of loop over cells
   
   // Algorithmic time
   if (!m_timersvc.empty()) m_timer[3]->pause();
   
  } // End of loop over TileCal drawers


   if ( msg().level()<MSG::DEBUG )
   jet.print(msg());
   TrigT2Tower hottest_tower;
   jet.findHottestTower(hottest_tower);

   // FILL HISTOS if at least one cell is set. if not RoI is not in Tile Barrel or Ext, Barrel and in consequence Hottest_tower would be emtpy
   // Will get bias because of check for > 0.0 but can't be avoided because we take log(e)
   if ( (hottest_tower.cell(0).inCone() || hottest_tower.cell(1).inCone() || hottest_tower.cell(2).inCone()) && hottest_tower.e() > 0.0 )
   {
      
   if ( msg().level() <= MSG::DEBUG ){
      msg() << MSG::DEBUG << " REGTEST: HOTTEST_TOWER:  print:" << endmsg;
      hottest_tower.print(msg(),MSG::DEBUG);
   }

      // TOWER
      int hist_grid_eta = (int) floorf( ((hottest_tower.eta()) + m_end_detec_eta)/m_eta_division);
      m_2dhists[hist_grid_eta]->Fill((float)hottest_tower.phi(), logEnergy(hottest_tower.e()) );
      if ( msg().level() <= MSG::DEBUG )
        msg() << MSG::DEBUG << " REGTEST: HOTTEST_TOWER:  fill in histo: eta:" << hottest_tower.eta() << " phi:"<< (float)hottest_tower.phi() << " log(e):" << logEnergy(hottest_tower.e()) << endmsg;

      // CELLS
      if ( hottest_tower.cell(0).inCone() && hottest_tower.cell(0).e() > 0.0 )
      {
         m_2dhists_TB0[hist_grid_eta]->Fill((float)hottest_tower.cell(0).phi(), logEnergy(hottest_tower.cell(0).e()) );
   	 if ( msg().level() <= MSG::DEBUG )
   	     msg() << MSG::DEBUG << " REGTEST: HOTTEST_CELL_0:  fill in histo: eta:" << hottest_tower.cell(0).eta() << " phi:"<< (float)hottest_tower.cell(0).phi() << " log(e):" << logEnergy(hottest_tower.cell(0).e()) << endmsg;
      }
      
      if ( hottest_tower.cell(1).inCone() && hottest_tower.cell(1).e() > 0.0 )
      {
         m_2dhists_TB1[hist_grid_eta]->Fill((float)hottest_tower.cell(1).phi(), logEnergy(hottest_tower.cell(1).e()) );
         if ( msg().level() <= MSG::DEBUG )
  	     msg() << MSG::DEBUG << " REGTEST: HOTTEST_CELL_1:  fill in histo: eta:" << hottest_tower.cell(1).eta() << " phi:"<< (float)hottest_tower.cell(1).phi() << " log(e):" << logEnergy(hottest_tower.cell(1).e()) << endmsg;
      }
      
      if ( hottest_tower.cell(2).inCone() && hottest_tower.cell(2).e() > 0.0 )
      {
         // eta is different for layer 2
         hist_grid_eta=(int)floorf( ((hottest_tower.cell(2).eta()) + m_end_detec_eta)/m_eta_division);
         m_2dhists_TB2[hist_grid_eta]->Fill((float)hottest_tower.cell(2).phi(), logEnergy(hottest_tower.cell(2).e()) );
         if ( msg().level() <= MSG::DEBUG )
  	    msg() << MSG::DEBUG << " REGTEST: HOTTEST_CELL_2:  fill in histo: eta:" << hottest_tower.cell(2).eta() << " phi:"<< (float)hottest_tower.cell(2).phi() << " log(e):" << logEnergy(hottest_tower.cell(2).e()) << endmsg;
      }
   }


	// Stop all timers
   if (!m_timersvc.empty()){ 
		m_timer[4]->stop();
		m_timer[3]->stop();
		m_timer[2]->stop();
		m_timer[1]->stop();
		m_timer[0]->stop();
	}
	
  return StatusCode::SUCCESS;
}
