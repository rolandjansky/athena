/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCaloEnergyTool.h"
// forward declares
#include "ParticleCaloExtension/ParticleCellAssociationCollection.h"

#include "TrackToCalo/CaloCellHelpers.h"
#include "ParticleCaloExtension/ParticleCellIntersection.h"
#include "ParticleCaloExtension/ParticleCaloAssociation.h"

#include "CaloUtils/CaloCellList.h"
#include "CaloEvent/CaloCellContainer.h"
#include "TrkCaloExtension/CaloExtension.h"
#include "TrkCaloExtension/CaloExtensionHelpers.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>

namespace Rec {

  MuonCaloEnergyTool::MuonCaloEnergyTool(const std::string& t, const std::string& n, const IInterface*  p )
    : AthAlgTool(t,n,p),
    m_caloExtensionTool("Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool", this),
    m_caloCellAssociationTool("Rec::ParticleCaloCellAssociationTool/ParticleCaloCellAssociationTool", this),
    m_particleCreator("Trk::TrackParticleCreatorTool/MuonCaloParticleCreator"),
    m_sigmasAboveNoise(4.),
    m_emEtCut(2.5*Gaudi::Units::GeV),
    m_emF1Cut(0.15),
    m_emipEM(0.42), // 0.42
    m_emipTile(0.86), // 0.86
    m_emipHEC(0.65)  // 0.65
  {
    declareInterface<IMuonCaloEnergyTool>(this);
    declareProperty("ParticleCaloExtensionTool",      m_caloExtensionTool );
    declareProperty("ParticleCaloCellAssociationTool",m_caloCellAssociationTool );
    declareProperty("TrackParticleCreator",           m_particleCreator );

    //coneSize for including calo cells around track
    declareProperty("SigmasAboveNoise", m_sigmasAboveNoise = 4.);
    declareProperty("EmEtCut",          m_emEtCut = 2.5*1000.);  // in MeV
    declareProperty("EmF1Cut",          m_emF1Cut = 0.15);
  }

  MuonCaloEnergyTool::~MuonCaloEnergyTool() {}

  StatusCode MuonCaloEnergyTool::initialize() {

    // RETRIEVE TOOLS
    ATH_CHECK(m_caloExtensionTool.retrieve());
    ATH_CHECK(m_caloCellAssociationTool.retrieve());
    ATH_CHECK(m_particleCreator.retrieve());

    ATH_CHECK(m_caloNoiseCDOKey.initialize());
    ATH_CHECK(m_indetTrackParticleLocation.initialize());
    ATH_CHECK(m_muonTrackParticleLocation.initialize());

    return StatusCode::SUCCESS;
  }

  StatusCode MuonCaloEnergyTool::finalize() {
    return StatusCode::SUCCESS;
  }

  void MuonCaloEnergyTool::calculateMuonEnergies( const Trk::Track* trk, 
                                                  double deltaE, double meanIoni, double sigmaIoni,
                                                  double& E,     double& sigma,   double& E_FSR   , double& E_expected, 
                                          double &E_em_meas, double &E_em_exp, double &E_tile_meas, double &E_tile_exp, 
                                          double &E_HEC_meas, double &E_HEC_exp, double &E_dead_exp,
  					  std::vector<Identifier>* crossedCells, std::vector<double>* sigmaNoise_cell, std::vector<double>* E_exp_cell) const {
//
// Input parameters trk:        (muon) track pointer
//                  deltaE:     Mean Energy loss in Calorimeter
//                  meanIoni:   Mean ionization Energy loss in Calorimeter 
//                  sigmaIoni:  sigma of the ionization Energy loss in Calorimeter
//
// Ouput parameters E:          Calorimeter energy measured (corrected for dead material etc.)
//                  sigma:      Error on measured energy
//                  E_FSR:      Energy of Final State Radiation: e.m. = photon energy (if above ET and F1 cut)   
//                              if EFSR>0 the corrected hadronic energy is used for the measured Energy E 
//                  E_expected: expected meanIonization Energyloss from the TrackingGeometry

    E          = 0.;
    sigma      = 0.;
    E_FSR      = 0.;
    E_expected = 0.;

    E_em_meas   = 0.;
    E_em_exp    = 0.;      
    E_tile_meas = 0.;
    E_tile_exp  = 0.;
    E_HEC_meas  = 0.;
    E_HEC_exp   = 0.;
    E_dead_exp  = 0.;

    bool storeCells = false;
    if(crossedCells!=nullptr&&sigmaNoise_cell!=nullptr&&E_exp_cell!=nullptr) {   
      storeCells = true;
      crossedCells->clear();
      sigmaNoise_cell->clear();
      E_exp_cell->clear();
    }
//
// For the expected Eloss in the dead or not instrumented material we will use the meanIonization loss
//     this meanIoni is stored on the extended Eloss object
//     the sigmaIoni corresponds to the Landau width
//     a factor 3.59524 gives the sigma of the upper tail
//
// The Eloss derived from the momentum differences on the calo extension correponds to 
//     deltaE on the Eloss object = mean Eloss including ionization and radiation
//
// To go to the  meanIonization loss one needs a scale factor scale_Ionization    
//
     if(!trk) return;
     if(!trk->perigeeParameters()) return; 
     if(trk->perigeeParameters()->parameters()[Trk::qOverP]==0.) return; 

//    int isign = 1;
//    if(meanIoni<0) isign = -1;
//    double mopIoni =  meanIoni - isign*3.59524*fabs(sigmaIoni);
    double mopIoni =  meanIoni;
    
    double scale_Ionization = 0.9;
    if(fabs(deltaE)>0&&fabs(meanIoni)>0) scale_Ionization = mopIoni / deltaE;
    double error_ratio = 0.3;
    if(fabs(meanIoni)>0&&sigmaIoni>0) error_ratio = 3.59524*fabs(sigmaIoni/meanIoni);

    ATH_MSG_DEBUG( " Inputs deltaE " << deltaE << " meanIoni " << meanIoni << " sigmaIoni " << sigmaIoni << " scale_Ionization " << scale_Ionization << " error_ratio " << error_ratio );
    if(deltaE==0||meanIoni==0||sigmaIoni<0) ATH_MSG_WARNING( " Strange Inputs deltaE " << deltaE << " meanIoni " << meanIoni << " sigmaIoni " << sigmaIoni );

    // associate muon to calorimeter 

    const xAOD::TrackParticle* tp = nullptr;

    SG::ReadHandle<xAOD::TrackParticleContainer> indetTrackParticles(m_indetTrackParticleLocation);
    if(indetTrackParticles.isValid()){
      // check ID trackparticles
      for(auto it : *indetTrackParticles){
	if((*it).track()==trk) {
	  tp = &(*it);
	  break;
	}
      }
      if(tp) ATH_MSG_DEBUG( " ID xAOD::TrackParticle found " << tp );
    }
    else{
      ATH_MSG_WARNING("Failed to retrieve "<<m_indetTrackParticleLocation.key());
    }

// look for Muon trackparticles 

    SG::ReadHandle<xAOD::TrackParticleContainer> muonTrackParticles(m_muonTrackParticleLocation);
    if(!tp && muonTrackParticles.isValid()){
      for(auto it : *muonTrackParticles){
        if((*it).track()==trk) {
           tp = &(*it);
           break;
        }
      }
      if(tp) ATH_MSG_DEBUG( " Muon xAOD::TrackParticle found " << tp );
    }
    else if(!tp){
      ATH_MSG_WARNING("Failed to retrieve "<<m_muonTrackParticleLocation.key());
    }
   
    std::unique_ptr<const xAOD::TrackParticle> tpholder;
    if(!tp) {
      tpholder = std::unique_ptr<const xAOD::TrackParticle>
        (m_particleCreator->createParticle(*trk, nullptr, nullptr, xAOD::muon) );

      tp = tpholder.get();
      if(tp) ATH_MSG_DEBUG( " xAOD::TrackParticle created from scratch " << tp );
    } 

    if(!tp) return;

    std::unique_ptr<const Rec::ParticleCellAssociation> association = 
      m_caloCellAssociationTool->particleCellAssociation(*tp,0.2,nullptr);

    if(!association) {return;}
    ATH_MSG_DEBUG( " particleCellAssociation done  " << association.get());

    std::vector< std::pair<const CaloCell*,Rec::ParticleCellIntersection*> > cellIntersections = association->cellIntersections();

    const Trk::CaloExtension& caloExtension = association->caloExtension();

    if(!caloExtension.caloEntryLayerIntersection()) {
      ATH_MSG_WARNING( " No caloEntryLayerIntersection found ");
      return;
    }   

    ATH_MSG_DEBUG( " nr of cell intersections " << cellIntersections.size() );
    if( cellIntersections.size() < 3) ATH_MSG_DEBUG( " Strange nr of calorimeter cell intersections " << cellIntersections.size() );

    double theta = caloExtension.caloEntryLayerIntersection()->position().theta();

    double Eloss = 0.;
    double scaleTG = 1.0;
    if(!caloExtension.muonEntryLayerIntersection()) {
      if(caloExtension.caloEntryLayerIntersection()->momentum().mag()-fabs(deltaE)>1000.) {
        ATH_MSG_WARNING( " No muonEntryLayerIntersection found and therefore the expected Eloss is not calculated properly for momentum " 
                         << caloExtension.caloEntryLayerIntersection()->momentum().mag() );
      }
      ATH_MSG_DEBUG( " No muonEntryLayerIntersection found and therefore the expected Eloss is not calculated properly ");
    } else {     
      Eloss = caloExtension.caloEntryLayerIntersection()->momentum().mag() - caloExtension.muonEntryLayerIntersection()->momentum().mag();
      ATH_MSG_DEBUG( " Energy loss from CaloExtension " << Eloss << " R muon Entry " << caloExtension.muonEntryLayerIntersection()->position().perp() << " Z muon Entry " << caloExtension.muonEntryLayerIntersection()->position().z() << " R calo entry " << caloExtension.caloEntryLayerIntersection()->position().perp() << " Z calo entry " << caloExtension.caloEntryLayerIntersection()->position().z() );
     if(Eloss>25000.&&Eloss>0.1*caloExtension.muonEntryLayerIntersection()->momentum().mag()) { 
       ATH_MSG_WARNING( " Crazy Energy loss from CaloExtension " << Eloss << " p at CaloEntry " << caloExtension.caloEntryLayerIntersection()->momentum().mag() << " p at Muon Entry " << caloExtension.muonEntryLayerIntersection()->momentum().mag()); 
       scaleTG = mopIoni/Eloss;
     }
  
    }

    CaloExtensionHelpers::EntryExitLayerMap  entryExitLayerMap;
    CaloExtensionHelpers::entryExitLayerMap( caloExtension, entryExitLayerMap );
    ATH_MSG_DEBUG("EntryExitLayerMap " << entryExitLayerMap.size() );

    CaloExtensionHelpers::ScalarLayerMap  eLossLayerMap;
    CaloExtensionHelpers::eLossLayerMap( caloExtension, eLossLayerMap );
    ATH_MSG_DEBUG("eLossLayerMap " << eLossLayerMap.size() );


    double scale_em_expected   = 0.97; // 0.73
    double scale_tile_expected = 1.17; // 0.89
    double scale_HEC_expected  = 1.11; // 0.84
//   
// TG expectations
//   
    double EE_emB  = 0.;
    double EE_emE  = 0.;
    double EE_HEC  = 0.;
    double EE_tile = 0.;
//
//    const char* sampname[24] = {
//    	"PreSamplerB", "EMB1", "EMB2", "EMB3",
//    	"PreSamplerE", "EME1", "EME2", "EME3",
//    	"HEC0", "HEC1","HEC2", "HEC3",
//    	"TileBar0", "TileBar1", "TileBar2",
//    	"TileExt0", "TileExt1", "TileExt2",
//    	"TileGap1", "TileGap2", "TileGap3",
//    	"FCAL0", "FCAL1", "FCAL2"};
//
    for(int i = CaloSampling::PreSamplerB ; i < CaloSampling::CaloSample::Unknown; i++) {
      CaloSampling::CaloSample sample = static_cast<CaloSampling::CaloSample>(i);
      auto pos = entryExitLayerMap.find(sample);
      if( pos == entryExitLayerMap.end() ) continue;
      auto eLossPair = eLossLayerMap.find(sample);
      double eLoss = 0.;
      if( eLossPair != eLossLayerMap.end() ){
         eLoss = eLossPair->second;
         ATH_MSG_DEBUG(" sample " << i  << " eLoss " <<  scale_Ionization*eLoss);
         if(i<=3) {
            EE_emB += scale_em_expected*scale_Ionization*eLoss;
         } else if(i<=7) {        
            EE_emE += scale_em_expected*scale_Ionization*eLoss;
         } else if(i<=11) {        
            EE_HEC += scale_HEC_expected*scale_Ionization*eLoss;
        } else if(i<=20) {        
            EE_tile += scale_tile_expected*scale_Ionization*eLoss;
        } 
      } 
    }
    if(caloExtension.caloEntryLayerIntersection()&&caloExtension.muonEntryLayerIntersection()) {
      double  Eloss = caloExtension.caloEntryLayerIntersection()->momentum().mag() - caloExtension.muonEntryLayerIntersection()->momentum().mag();
      ATH_MSG_DEBUG(" eta " << caloExtension.caloEntryLayerIntersection()->momentum().eta() << " expected energies from TG EE_emB " << EE_emB << " EE_emE " << EE_emE << " EE_tile " << EE_tile << " EE_HEC " << EE_HEC << " total Eloss " << Eloss);
    }
//
// do not continue with crazy values for the Eloss from TG
//
    if(scaleTG!=1.0) return; 

    // measured and expected energies 

    //Get Calo-Noise CDO:
    SG::ReadCondHandle<CaloNoise> caloNoiseHdl{m_caloNoiseCDOKey};
    const CaloNoise* caloNoise=*caloNoiseHdl;


    double E_em1  = 0.;
    double E_em   = 0.;
    double E_em_expected = 0.;
    double E_em_exptot = 0.;
    double E_em_expall = 0.;
    double E_em_threshold = 0.;
    int    nlay_em = 0;
    double sigma_Noise_em = 0.;

    double E_HEC  = 0.;
    double E_HEC_expected = 0.;
    double E_HEC_threshold = 0.;
    double E_HEC_exptot = 0.;
    double E_HEC_expall = 0.;
    int    nlay_HEC = 0;
    double sigma_Noise_HEC = 0.;

    double E_tile = 0.;
    double E_tile_expected = 0.;
    double E_tile_threshold = 0.;
    double E_tile_exptot = 0.;
    double E_tile_expall = 0.;
    int    nlay_tile = 0;
    double sigma_Noise_tile = 0.;

    E_expected = 0.;
    double phiPos = caloExtension.caloEntryLayerIntersection()->position().phi();
    double thetaPos = caloExtension.caloEntryLayerIntersection()->position().theta();
 

    for(auto it : cellIntersections){ 

      const CaloCell* curr_cell = it.first;
      int cellSampling = curr_cell->caloDDE()->getSampling();
      bool badCell     = curr_cell->badcell();
      double cellEn    = curr_cell->energy();
      Identifier id    = curr_cell->ID();

      double f_exp = (it.second)->pathLength(); 
      double E_exp = (it.second)->expectedEnergyLoss();

      if(f_exp>1.) f_exp = 1.;

//      if(f_exp<0.1) f_exp = 0.1;

      double sigma_Noise = caloNoise->getEffectiveSigma(id,curr_cell->gain(),cellEn);

      double thetaCell = atan2(sqrt(curr_cell->x()*curr_cell->x()+curr_cell->y()*curr_cell->y()),curr_cell->z());
      double phiCell = atan2(curr_cell->y(),curr_cell->x());

      double dtheta = thetaCell - thetaPos;
      double dphi = acos(cos(phiPos)*cos(phiCell)+sin(phiPos)*sin(phiCell));
      double celldr = curr_cell->caloDDE()->dr();
      double celldz = curr_cell->caloDDE()->dz();
      double celldtheta = celldr/sqrt(curr_cell->x()*curr_cell->x()+curr_cell->y()*curr_cell->y());
      double celldphi = curr_cell->caloDDE()->dphi();
 
 
      ATH_MSG_DEBUG( " cell sampling " << cellSampling << " energy " << cellEn << " position R " << sqrt(curr_cell->x()*curr_cell->x()+curr_cell->y()*curr_cell->y()) << " z " << curr_cell->z() << " f_exp " << f_exp << " E_exp " << E_exp << " dtheta " << dtheta << " dphi " << dphi <<  " cell dtheta " << celldtheta << " cell dr " << celldr <<  " cell dz " << celldz << " cell dphi " << celldphi );
//      ATH_MSG_DEBUG( " sigma_Noise totalNoiseRMS from Alan " << sigma_NoiseA << " sigma_Noise getEffectiveSigma " << sigma_Noise);

//   Use expected energy if measured energy below noise level
// - this will bias the energy measurement towards too high values
// - the bias is corrected in the thresholdCorrection function below
//
// sum measured, expected energies for crossed cells after noise cuts 
//
      if(cellSampling == CaloSampling::PreSamplerB || cellSampling == CaloSampling::PreSamplerE) {
        if(f_exp>0&&cellEn>m_sigmasAboveNoise*sigma_Noise&&!badCell) {
          E_em1 += cellEn; 
          if(storeCells) {
            crossedCells->push_back(id); 
  	    sigmaNoise_cell->push_back(sigma_Noise);
 	    E_exp_cell->push_back(scale_Ionization*f_exp*E_exp*scale_em_expected); // I don't want sum, but value for each cell
          }
        }
      }
      if(curr_cell->caloDDE()->getSubCalo() == CaloCell_ID::LAREM) {
        E_em_exptot += scale_Ionization*f_exp*E_exp*scale_em_expected;
        if(f_exp>0) E_em_expall += scale_Ionization*E_exp*scale_em_expected;
        if(f_exp>0&&cellEn>m_sigmasAboveNoise*sigma_Noise&&!badCell) {
//        if(f_exp>0&&cellEn>m_sigmasAboveNoise*sigma_Noise&&!badCell) {
          E_em  += cellEn;
          E_em_expected += scale_Ionization*f_exp*E_exp*scale_em_expected;
          ATH_MSG_VERBOSE( " EM cell " << cellEn << " sigma_Noise " << sigma_Noise << " f_exp " << f_exp << " E_exp " << E_exp);
          if(storeCells) {
            crossedCells->push_back(id); 
  	    sigmaNoise_cell->push_back(sigma_Noise);
 	    E_exp_cell->push_back(scale_Ionization*f_exp*E_exp*scale_em_expected);
          }
        }
        if(f_exp>0&&!badCell) nlay_em++; 
        if(f_exp>0&&!badCell) sigma_Noise_em += sigma_Noise; 
      } else if(curr_cell->caloDDE()->getSubCalo() == CaloCell_ID::TILE) {
        E_tile_exptot += scale_Ionization*f_exp*E_exp*scale_tile_expected;
        if(f_exp>0) E_tile_expall += scale_Ionization*E_exp*scale_tile_expected;
        if(f_exp>0&&cellEn>m_sigmasAboveNoise*sigma_Noise&&!badCell) {
//                       &&f_exp*E_exp>m_sigmasAboveNoise*sigma_Noise/4) {
          E_tile  += cellEn;
          E_tile_expected += scale_Ionization*f_exp*E_exp*scale_tile_expected;
          ATH_MSG_VERBOSE( " Tile cell " << cellEn << " sigma_Noise " << sigma_Noise << " f_exp " << f_exp << " E_exp " << E_exp);
          if(storeCells) {
            crossedCells->push_back(id); 
  	    sigmaNoise_cell->push_back(sigma_Noise);
 	    E_exp_cell->push_back(scale_Ionization*f_exp*E_exp*scale_tile_expected);
          }
        }
        if(f_exp>0&&!badCell) nlay_tile++; 
        if(f_exp>0&&!badCell) sigma_Noise_tile += sigma_Noise; 
      } else if(curr_cell->caloDDE()->getSubCalo() == CaloCell_ID::LARHEC) {
        E_HEC_exptot += scale_Ionization*f_exp*E_exp*scale_HEC_expected;
        if(f_exp>0) E_HEC_expall += scale_Ionization*E_exp*scale_HEC_expected;
// lower threshold for HEC 
        if(f_exp>0&&cellEn>m_sigmasAboveNoise*sigma_Noise/2.&&!badCell) {
//                        &&f_exp*E_exp>m_sigmasAboveNoise*sigma_Noise/4) {
          E_HEC  += cellEn;
          E_HEC_expected += scale_Ionization*f_exp*E_exp*scale_HEC_expected;
          ATH_MSG_VERBOSE( " HEC cell " << cellEn << " sigma_Noise " << sigma_Noise << " f_exp " << f_exp << " E_exp " << E_exp);
          if(storeCells) {
            crossedCells->push_back(id); 
  	    sigmaNoise_cell->push_back(sigma_Noise);
 	    E_exp_cell->push_back(scale_Ionization*f_exp*E_exp*scale_HEC_expected);
          }
        } 
        if(f_exp>0&&!badCell) nlay_HEC++; 
        if(f_exp>0&&!badCell) sigma_Noise_HEC += sigma_Noise; 
      } 
      E_expected += scale_Ionization*f_exp*E_exp;
    } 
    ATH_MSG_DEBUG( " After cuts measured Energies em " << E_em << " tile " << E_tile << " HEC " << E_HEC);
    ATH_MSG_DEBUG( " After cuts expected Energies em " << E_em_expected << " tile " << E_tile_expected << " HEC " << E_HEC_expected);
    ATH_MSG_DEBUG( " No cuts with length expected Energies em " << E_em_exptot << " tile " << E_tile_exptot << " HEC " << E_HEC_exptot);
    ATH_MSG_DEBUG( " No cuts NO length expected Energies em " << E_em_expall << " tile " << E_tile_expall << " HEC " << E_HEC_expall);

// E resolution of calorimeters 
// Resolution of Hadronic at low energies is not 100% but more like 50%/sqrt(E)
    
    double scaleError_tile = 0.20 + 0.80*E_tile/(10000.+E_tile);
    double scaleError_HEC  = 0.20 + 0.80*E_HEC/(10000.+E_HEC);
    double sigma_em   = sqrt( 500.*E_em);  
    double sigma_tile = scaleError_tile*sqrt(1000.*E_tile);    
    double sigma_HEC  = scaleError_HEC*sqrt(1000.*E_HEC);    
    
// go from e.m. scale to Muon Energy scale

    E_em   *= m_emipEM;
    E_tile *= m_emipTile;
    E_HEC  *= m_emipHEC;
    ATH_MSG_DEBUG( " e.m. to Muon scale measured Energies em " << E_em << " tile " << E_tile << " HEC " << E_HEC);

// also for errors

    sigma_em   *= m_emipEM;
    sigma_tile *= m_emipTile;
    sigma_HEC  *= m_emipHEC;

// Average Noise per layer 

    if(nlay_em>0)   sigma_Noise_em   /= nlay_em;
    if(nlay_tile>0) sigma_Noise_tile /= nlay_tile;
    if(nlay_HEC>0)  sigma_Noise_HEC  /= nlay_HEC;
    ATH_MSG_DEBUG( " Average Noise em " << sigma_Noise_em << " nlay " << nlay_em << "  tile " << sigma_Noise_tile << " nlay " << nlay_tile << "  HEC " << sigma_Noise_HEC << " nlay " << nlay_HEC );

//  apply energy correction using the expected Eloss for noise cut 

    E_em_threshold   += thresholdCorrection(E_em,E_em_expected,m_sigmasAboveNoise*sigma_Noise_em/m_emipEM/4);
    E_tile_threshold += thresholdCorrection(E_tile,E_tile_expected,m_sigmasAboveNoise*sigma_Noise_tile/m_emipTile/4);
    E_HEC_threshold  += thresholdCorrection(E_HEC,E_HEC_expected,m_sigmasAboveNoise*sigma_Noise_HEC/m_emipHEC/8);

    ATH_MSG_DEBUG( " Threshold correction to Energies em " << E_em_threshold << " tile " << E_tile_threshold << " HEC " << E_HEC_threshold);

    ATH_MSG_DEBUG( " total Energies em " << E_em+E_em_threshold << " tile " << E_tile+E_tile_threshold << " HEC " << E_HEC+E_HEC_threshold);

//  Eloss in dead material (so everything not accounted for in the measurement)

//     if(scale_Ionization*Eloss>E_expected) E_expected = scale_Ionization*Eloss;
     E_expected = scale_Ionization*Eloss;

// Corrections for dead material 0.15*E_tile_expected + 0.20*E_HEC_expected;

     double E_dead = E_expected - E_em_expected - E_tile_expected - E_HEC_expected + 0.12*E_tile_expected + 0.27*E_HEC_expected;
;

//  treatment of FSR

     E_FSR = 0.;
     double E_measured = 0.;
     double E_measured_expected = E_em_expected + E_tile_expected + E_HEC_expected;
//     if(E_em*cos(theta)>m_emEtCut&&E_em1>0.15*E_em) {
     if(E_em*sin(theta)>m_emEtCut) {
// large e.m. deposit starting in first e.m. layer
       E_FSR = E_em;
// do not use measured e.m. energy for muons and use expected (tile and HEC are fine)
       E = E_em_expected + E_tile + E_tile_threshold + E_HEC + E_HEC_threshold + E_dead;
       double sigma_expdead = error_ratio*(E_em_expected + E_dead);
       sigma = sqrt(sigma_tile*sigma_tile + sigma_HEC*sigma_HEC + sigma_expdead*sigma_expdead);
       E_measured = E_tile + E_tile_threshold + E_HEC + E_HEC_threshold;
     } else {
// no FSR 
       E = E_em + E_em_threshold + E_tile + E_tile_threshold + E_HEC + E_HEC_threshold + E_dead;
       double sigma_threshold = error_ratio*E_dead;
       sigma = sqrt(sigma_em*sigma_em + sigma_tile*sigma_tile + sigma_HEC*sigma_HEC+sigma_threshold*sigma_threshold);
       E_measured = E_em + E_em_threshold + E_tile + E_tile_threshold + E_HEC + E_HEC_threshold;
     }

// eta dependent correction

     double etaPos = caloExtension.caloEntryLayerIntersection()->position().eta();
     E = E + etaCorr(etaPos) * E_expected;

    ATH_MSG_DEBUG( " Total energy " << E << " sigma " << sigma  << " E calo measured in cells " << E_measured << " E calo expected in cells " << E_measured_expected << " E_expected meanIoni from TG " << E_expected );

     if(E_em+E_tile+E_HEC<0.1*E&&E_em+E_tile+E_HEC>1000.) {
        ATH_MSG_WARNING( " Real Measured Calorimeter energy " << E_em+E_tile+E_HEC  << " is much lower than total " << E << " expected energy too large " << E_expected << " put measured energy to zero ");   
        E = 0.;
        sigma = 0.;
     }
//    
// add for validation (temporarily) 
//
//   E_em_meas,E_em_exp,E_tile_meas,E_tile_exp,E_HEC_meas,E_HEC_exp,E_dead_exp
//

//     E_em_meas   = E_em + E_em_threshold;
//     E_em_exp    = E_em_expected;
//     E_tile_meas = E_tile + E_tile_threshold;
//     E_tile_exp  = E_tile_expected;
//     E_HEC_meas  = E_HEC + E_HEC_threshold;
//     E_HEC_exp   = E_HEC_expected;
//     E_dead_exp  = E_dead; 
       
//     double EE_dead = E_expected - EE_emB - EE_emE - EE_tile - EE_HEC + 0.15*EE_tile + 0.20*EE_HEC;
     double EE_dead = E_expected - EE_emB - EE_emE - EE_tile - EE_HEC + 0.12*EE_tile + 0.27*EE_HEC;
//
//   move expected (to match the TG expected Eloss) and measured energies
//
     E_em_meas   = E_em + E_em_threshold + EE_emB + EE_emE - E_em_expected;
     E_em_exp    = EE_emB + EE_emE;      
     E_tile_meas = E_tile + E_tile_threshold + EE_tile - E_tile_expected;
     E_tile_exp  = EE_tile;
     E_HEC_meas  = E_HEC + E_HEC_threshold + EE_HEC - E_HEC_expected;
     E_HEC_exp   = EE_HEC;
     E_dead_exp  = EE_dead;

  } // calculateMuonEnergies

  double MuonCaloEnergyTool::etaCorr(double eta) const{
// measured energy* = measured energy + etaCorr(eta) * expected

      int eta_index = int(fabs(eta) * (60./3.));
      if(eta_index>59) return 0;

      double corr[60] = {
                        0.00368146 , 0.0419526 , 0.0419322 , 0.0392922 , 0.030304 , 0.0262424 , 0.0156346 , 0.00590235 , 0.00772249 , -0.0141775 , -0.0152247 , -0.0174432 , -0.0319056 , -0.0670813 , -0.128678 , -0.0982326 , -0.0256406 , -0.200244 , -0.178975 , -0.120156 , -0.124606 , -0.0961311 , -0.0163201 , 0.00357829 , 0.0292199 , 0.0110466 , -0.0375598 , 0.0417912 , 0.0386369 , -0.0689454 , -0.21496 , -0.126157 , -0.210573 , -0.215757 , -0.202019 , -0.164546 , -0.168607 , -0.128602 , -0.124629 , -0.0882631 , -0.100869 , -0.0460344 , -0.0709039 , -0.0163041 , -0.0521138 , -0.0125259 , -0.0378681 , 0.00396062 , -0.0636308 , -0.032199 , -0.0588335 , -0.0470752 , -0.0450315 , -0.0301302 , -0.087378 , -0.0115615 , -0.0152037 , 0 , 0 , 0
      }; // corrections

// additional correction release 21

     double cor21[20] = {0.999793 ,1.00017 ,0.990946 ,0.995358 ,1.01377 ,1.02676 ,1.03111 ,1.01483 ,0.995585 ,1.00465 ,1.05224 ,1.05238 ,1.03208 ,1.02373 ,1.02305 ,1.03975 ,1.06547 ,1.0364 ,1.0361 ,1.0361};

      int i21 = fabs(eta*20./3.);
      if(i21>19) i21 = 19;

      return (cor21[i21]*corr[eta_index]);
  } // etaCorr

  double MuonCaloEnergyTool::thresholdCorrection(double E_observed,double E_expected,double sigma_Noise) const {

//
// Total energy observed and expected in cells of LAr, Tile, or HEC after 4*sigma_Noise cut
//                       for a resolution of dE/E of 20%
//
// returns a correction to the energy based on the expected energy
//
//   one should so use: Etotal = E_observed + E_dead + ThresholdCorrection(E_observed,E_expected,sigma_Noise)

    Double_t par[6] = {1.01431e+00,-2.26317e+01,1.80901e+02 ,-2.33105e+01, 1.82930e+02,1.11356e-02};
 
    double E = E_expected;

    if(E==0) return 0.;
 
    double x = sigma_Noise/E;
    if(x>1.) x = 1.;

  // formula is for a 4 sigma Noise cut and obtained by a small simulation programm

    double fraction = (par[0]+par[1]*x+par[2]*x*x)/(1.+par[3]*x+par[4]*x*x)+par[5]*x;

  // for low x values (low thresholds) the fraction is bigger than 1 
  // (1.33) because the observed energy overestimates

    ATH_MSG_DEBUG( " ThresholdCorrection E "  << E << " E_observed not used " << E_observed  << " sigma_Noise " << sigma_Noise <<   " x = sigma_Noise/E  " << x << " fraction " << fraction << " E correction " << (1-fraction)*E );

    return (1.-fraction)*E;
  } // thresholdCorrection

} // end of namespace Trk
