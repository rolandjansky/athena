/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include <math.h>

namespace Rec {

  MuonCaloEnergyTool::MuonCaloEnergyTool(const std::string& t, const std::string& n, const IInterface*  p )
    : AthAlgTool(t,n,p),
    m_caloExtensionTool("Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool"),
    m_caloCellAssociationTool("Rec::ParticleCaloCellAssociationTool/ParticleCaloCellAssociationTool"),
    m_particleCreator("Trk::TrackParticleCreatorTool/MuonCaloParticleCreator"),
    m_caloNoiseTool("CaloNoiseToolDefault"),
    m_sigmasAboveNoise(4.),
    m_emEtCut(2.5*CLHEP::GeV),
    m_emF1Cut(0.15),
    m_emipEM(0.78),
    m_emipTile(0.86),
    m_emipHEC(0.94)
  {
    declareInterface<IMuonCaloEnergyTool>(this);
    declareProperty("ParticleCaloExtensionTool",      m_caloExtensionTool );
    declareProperty("ParticleCaloCellAssociationTool",m_caloCellAssociationTool );
    declareProperty("TrackParticleCreator",           m_particleCreator );
    declareProperty("CaloNoiseTool",                  m_caloNoiseTool);
    declareProperty("TrackParticleLocation",          m_indetTrackParticleLocation = "InDetTrackParticles");
    declareProperty("MuonSpectrometerTrackPArticleLocation",m_muonTrackParticleLocation = "MuonSpectrometerTrackParticles");    

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
    ATH_CHECK(m_caloNoiseTool.retrieve());

    m_indetTrackParticles = 0;
    if(evtStore()->contains<xAOD::TrackParticleContainer>(m_indetTrackParticleLocation)) {
      if(evtStore()->retrieve(m_indetTrackParticles,m_indetTrackParticleLocation).isFailure()) {
        ATH_MSG_FATAL( "Unable to retrieve " << m_indetTrackParticleLocation );
        return StatusCode::FAILURE;
      }
    }

    m_muonTrackParticles = 0;
    if(evtStore()->contains<xAOD::TrackParticleContainer>(m_muonTrackParticleLocation)) {
      if(evtStore()->retrieve(m_muonTrackParticles,m_muonTrackParticleLocation).isFailure()) {
        ATH_MSG_FATAL( "Unable to retrieve " << m_muonTrackParticleLocation );
        return StatusCode::FAILURE;
      }
    }


    return StatusCode::SUCCESS;
  }

  StatusCode MuonCaloEnergyTool::finalize() {
    return StatusCode::SUCCESS;
  }

  void MuonCaloEnergyTool::calculateMuonEnergies( const Trk::Track* trk, 
                                                  double deltaE, double meanIoni, double sigmaIoni,
                                                  double& E,     double& sigma,   double& E_FSR   , double& E_expected, 
                                          double &E_em_meas, double &E_em_exp, double &E_tile_meas, double &E_tile_exp, 
                                          double &E_HEC_meas, double &E_HEC_exp, double &E_dead_exp ) const {

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
    double scale_Ionization = 0.9;
    if(fabs(deltaE)>0&&fabs(meanIoni)>0) scale_Ionization = meanIoni / deltaE;
    double error_ratio = 0.3;
    if(fabs(meanIoni)>0&&sigmaIoni>0) error_ratio = 3.59524*fabs(sigmaIoni/meanIoni);

    ATH_MSG_DEBUG( " Inputs deltaE " << deltaE << " meanIoni " << meanIoni << " sigmaIoni " << sigmaIoni );
    if(deltaE==0||meanIoni==0||sigmaIoni<0) ATH_MSG_WARNING( " Strange Inputs deltaE " << deltaE << " meanIoni " << meanIoni << " sigmaIoni " << sigmaIoni );

    // associate muon to calorimeter 

    const xAOD::TrackParticle* tp = 0;

// check ID trackparticles

    if(m_indetTrackParticles) {
      for(auto it : *m_indetTrackParticles){
        if((*it).track()==trk) {
           tp = &(*it);
           break;
        }
      }
      if(tp) ATH_MSG_DEBUG( " ID xAOD::TrackParticle found " << tp );
    }

// look for Muon trackparticles 

    if(m_muonTrackParticles&&!tp) {
      for(auto it : *m_muonTrackParticles){
        if((*it).track()==trk) {
           tp = &(*it);
           break;
        }
      }
      if(tp) ATH_MSG_DEBUG( " Muon xAOD::TrackParticle found " << tp );
    }
    
    if(!tp) {
      tp = m_particleCreator->createParticle(*trk, NULL, NULL, xAOD::muon);
      if(tp) ATH_MSG_DEBUG( " xAOD::TrackParticle created from scratch " << tp );
    }

    if(!tp) return;

    const Rec::ParticleCellAssociation* association = 0;
    m_caloCellAssociationTool->particleCellAssociation(*tp,association,0.1,NULL,true);

    if(!association) return;
    ATH_MSG_DEBUG( " particleCellAssociation done  " << association );

    std::vector< std::pair<const CaloCell*,Rec::ParticleCellIntersection*> > cellIntersections = association->cellIntersections();

    const Trk::CaloExtension& caloExtension = association->caloExtension();

    if(!(&caloExtension)) {
      ATH_MSG_WARNING( " No caloExtension found ");
      return;
    }

    if(!caloExtension.caloEntryLayerIntersection()) {
      ATH_MSG_WARNING( " No caloEntryLayerIntersection found ");
      return;
    }   

    ATH_MSG_DEBUG( " nr of cell intersections " << cellIntersections.size() );
    if( cellIntersections.size() < 3) ATH_MSG_WARNING( " Strange nr of calorimeter cell intersections " << cellIntersections.size() );

    double theta = caloExtension.caloEntryLayerIntersection()->position().theta();

    double Eloss = 0.;
    if(!caloExtension.muonEntryLayerIntersection()) {
      ATH_MSG_WARNING( " No muonEntryLayerIntersection found and therefore the expected Eloss is not calculated properly ");
    } else {     
      Eloss = caloExtension.caloEntryLayerIntersection()->momentum().mag() - caloExtension.muonEntryLayerIntersection()->momentum().mag();
      ATH_MSG_DEBUG( " Energy loss from CaloExtension " << Eloss << " R muon Entry " << caloExtension.muonEntryLayerIntersection()->position().perp() << " Z muon Entry " << caloExtension.muonEntryLayerIntersection()->position().z() << " R calo entry " << caloExtension.caloEntryLayerIntersection()->position().perp() << " Z calo entry " << caloExtension.caloEntryLayerIntersection()->position().z() );
    }

    //auto cellcoll = association->data();

    // measured and expected energies 

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

//    const char* sampname[24] = {
//    	"PreSamplerB", "EMB1", "EMB2", "EMB3",
//    	"PreSamplerE", "EME1", "EME2", "EME3",
//    	"HEC0", "HEC1","HEC2", "HEC3",
//    	"TileBar0", "TileBar1", "TileBar2",
//    	"TileExt0", "TileExt1", "TileExt2",
//    	"TileGap1", "TileGap2", "TileGap3",
//    	"FCAL0", "FCAL1", "FCAL2"};
//    for(const auto& curr_cell : cellcoll ){
   
    E_expected = 0.;
    double phiPos = caloExtension.caloEntryLayerIntersection()->position().phi();
    double thetaPos = caloExtension.caloEntryLayerIntersection()->position().theta();

    for(auto it : cellIntersections){ 

      const CaloCell* curr_cell = it.first;
      int cellSampling = curr_cell->caloDDE()->getSampling();
      bool badCell     = curr_cell->badcell();
      double cellEn    = curr_cell->energy();

      double f_exp = (it.second)->pathLength(); 
      double E_exp = (it.second)->expectedEnergyLoss();

//      if(f_exp<0.1) f_exp = 0.1;

//    cellnoisedb = m_caloNoiseTool->getNoise(cell,ICalorimeterNoiseTool::ELECTRONICNOISE);
      double sigma_Noise = m_caloNoiseTool->getEffectiveSigma(curr_cell,ICalorimeterNoiseTool::MAXSYMMETRYHANDLING,ICalorimeterNoiseTool::ELECTRONICNOISE);
//      double sigma_NoiseA  = m_caloNoiseTool->totalNoiseRMS(curr_cell);
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
        if(f_exp>0&&cellEn>m_sigmasAboveNoise*sigma_Noise&&!badCell)  E_em1 += cellEn; 
      }
      if(curr_cell->caloDDE()->getSubCalo() == CaloCell_ID::LAREM) {
        E_em_exptot += scale_Ionization*f_exp*E_exp;
        if(f_exp>0) E_em_expall += scale_Ionization*E_exp;
        if(f_exp>0&&cellEn>m_sigmasAboveNoise*sigma_Noise&&!badCell) {
          E_em  += cellEn;
          E_em_expected += scale_Ionization*f_exp*E_exp;
          ATH_MSG_VERBOSE( " EM cell " << cellEn << " sigma_Noise " << sigma_Noise << " f_exp " << f_exp << " E_exp " << E_exp);
        }
        if(f_exp>0&&!badCell) nlay_em++; 
        if(f_exp>0&&!badCell) sigma_Noise_em += sigma_Noise; 
      } else if(curr_cell->caloDDE()->getSubCalo() == CaloCell_ID::TILE) {
        E_tile_exptot += scale_Ionization*f_exp*E_exp;
        if(f_exp>0) E_tile_expall += scale_Ionization*E_exp;
        if(f_exp>0&&cellEn>m_sigmasAboveNoise*sigma_Noise&&!badCell) {
//                       &&f_exp*E_exp>m_sigmasAboveNoise*sigma_Noise/4) {
          E_tile  += cellEn;
          E_tile_expected += scale_Ionization*f_exp*E_exp;
          ATH_MSG_VERBOSE( " Tile cell " << cellEn << " sigma_Noise " << sigma_Noise << " f_exp " << f_exp << " E_exp " << E_exp);
        }
        if(f_exp>0&&!badCell) nlay_tile++; 
        if(f_exp>0&&!badCell) sigma_Noise_tile += sigma_Noise; 
      } else if(curr_cell->caloDDE()->getSubCalo() == CaloCell_ID::LARHEC) {
        E_HEC_exptot += scale_Ionization*f_exp*E_exp;
        if(f_exp>0) E_HEC_expall += scale_Ionization*E_exp;
// lower threshold for HEC 
        if(f_exp>0&&cellEn>m_sigmasAboveNoise*sigma_Noise/2.&&!badCell) {
//                        &&f_exp*E_exp>m_sigmasAboveNoise*sigma_Noise/4) {
          E_HEC  += cellEn;
          E_HEC_expected += scale_Ionization*f_exp*E_exp;
          ATH_MSG_VERBOSE( " HEC cell " << cellEn << " sigma_Noise " << sigma_Noise << " f_exp " << f_exp << " E_exp " << E_exp);
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

    double sigma_em   = sqrt( 500.*E_em);    
    double sigma_tile = sqrt(1000.*E_tile);    
    double sigma_HEC  = sqrt(1000.*E_HEC);    
    
// go from e.m. scale to Muon Energy scale

    E_em   /= m_emipEM;
    E_tile /= m_emipTile;
    E_HEC  /= m_emipHEC;
    ATH_MSG_DEBUG( " e.m. to Muon scale measured Energies em " << E_em << " tile " << E_tile << " HEC " << E_HEC);

// also for errors

    sigma_em   /= m_emipEM;
    sigma_tile /= m_emipTile;
    sigma_HEC  /= m_emipHEC;

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

     if(scale_Ionization*Eloss>E_expected) E_expected = scale_Ionization*Eloss;

     double E_dead = E_expected - E_em_expected - E_tile_expected - E_HEC_expected;

//  treatment of FSR

     E_FSR = 0.;
     double E_measured = 0.;
     double E_measured_expected = E_em_expected + E_tile_expected + E_HEC_expected;
//     if(E_em*cos(theta)>m_emEtCut&&E_em1>0.15*E_em) {
     if(E_em*cos(theta)>m_emEtCut) {
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

    ATH_MSG_DEBUG( " Total energy " << E << " sigma " << sigma  << " E calo measured in cells " << E_measured << " E calo expected in cells " << E_measured_expected << " E_expected meanIoni from TG " << E_expected );
//    
// add for validation (temporarily) 
//
//   E_em_meas,E_em_exp,E_tile_meas,E_tile_exp,E_HEC_meas,E_HEC_exp,E_dead_exp
//
     E_em_meas   = E_em + E_em_threshold;
     E_em_exp    = E_em_expected;
     E_tile_meas = E_tile + E_tile_threshold;
     E_tile_exp  = E_tile_expected;
     E_HEC_meas  = E_HEC + E_HEC_threshold;
     E_HEC_exp   = E_HEC_expected;
     E_dead_exp  = E_dead; 

  } // calculateMuonEnergies


  double MuonCaloEnergyTool::thresholdCorrection(double E_observed,double E_expected,double sigma_Noise) const {

//
// Total energy observed and expected in cells of LAr, Tile, or HEC after 4*sigma_Noise cut
//
// returns a correction to the energy based on the expected energy
//
//   one should so use: Etotal = E_observed + E_dead + ThresholdCorrection(E_observed,E_expected,sigma_Noise)

    Double_t par[5] = {1.33548e+00,-1.00854e+01,5.38111e+01,-9.33379e+00,5.32913e+01};

    double E = E_expected;

    if(E==0) return 0.;
 
    double x = sigma_Noise/E;
    if(x>1.) x = 1.;

  // formula is for a 4 sigma Noise cut and obtained by a small simulation programm

    double fraction = (par[0]+par[1]*x+par[2]*x*x)/(1.+par[3]*x+par[4]*x*x);

  // for low x values (low thresholds) the fraction is bigger than 1 
  // (1.33) because the observed energy overestimates

    ATH_MSG_DEBUG( " ThresholdCorrection E "  << E << " E_observed not used " << E_observed  << " sigma_Noise " << sigma_Noise <<   " x = sigma_Noise/E  " << x << " fraction " << fraction << " E correction " << (1-fraction)*E );

    return (1.-fraction)*E;
  } // thresholdCorrection

} // end of namespace Trk
