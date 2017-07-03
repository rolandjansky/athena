/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TauPFOCalHitDecorator.cxx
 * @author W. Davey - will.davey@cern.ch
 * @date September 2016
 *
 * Simplification of Mark Hodgkinsons CalHit code:
 * https://svnweb.cern.ch/cern/wsvn/atlas-mhodgkin/mhodgkin/MakeCellInfo
 *
 * Decorates cal-hit summary info onto neutral PFOs
 *
 */

#include "DerivationFrameworkTau/TauPFOCalHitDecorator.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauxAODHelpers.h"

// new includes
#include "xAODPFlow/PFOContainer.h"

#include "CaloSimEvent/CaloCalibrationHit.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloClusterCellLink.h"

#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODTruth/TruthParticleContainer.h"


/** local helper functions */
// recursive caller (internal for main child function)
std::vector<const xAOD::TruthParticle*> getChildren(
    const xAOD::TruthParticle* p, 
    std::vector<const xAOD::TruthParticle*>& children) 
{
  children.push_back(p);
  for (size_t i=0; i<p->nChildren(); i++)
    getChildren(p->child(i),children);
  return children;
}
// main child function (returns all children of particle) 
std::vector<const xAOD::TruthParticle*> getChildren(const xAOD::TruthParticle* p) {
  std::vector<const xAOD::TruthParticle*> children;
  return getChildren(p,children);
}

std::set<int> getChildBarcodes(const xAOD::TruthParticle* p) {
    std::vector<const xAOD::TruthParticle*> children = getChildren(p);
    std::set<int> barcodes;
    for (auto p : children) barcodes.insert(p->barcode());
    return barcodes;
}


/** TauPFOCalHitDecorator implementation */
namespace DerivationFramework {

  TauPFOCalHitDecorator::TauPFOCalHitDecorator(const std::string& t, const std::string& n, const IInterface* p) : 
    AthAlgTool(t, n, p),
    m_tauContainerName("TauJets")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("TauContainerName", m_tauContainerName);
  }

  StatusCode TauPFOCalHitDecorator::initialize()
  {
    ATH_MSG_VERBOSE("initialize() ...");

    if (m_tauContainerName.empty()) {
      ATH_MSG_ERROR("No StoreGate key provided for tau container of the TauPFOCalHitDecorator tool!");
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

  StatusCode TauPFOCalHitDecorator::finalize()
  {
    return StatusCode::SUCCESS;
  }

  StatusCode TauPFOCalHitDecorator::addBranches() const
  {
    // retrieve tau container
    const xAOD::TauJetContainer* xTauContainer = evtStore()->retrieve< const xAOD::TauJetContainer >( m_tauContainerName );
    if( not xTauContainer ) {
      ATH_MSG_ERROR ("Couldn't retrieve tau container with key: " << m_tauContainerName );
      return StatusCode::FAILURE;
    }

    // retrieve calhit containers
    std::vector<const CaloCalibrationHitContainer*> calHitContainers;
    addCalHitContainer("TileCalibHitActiveCell", calHitContainers); 
    addCalHitContainer("TileCalibHitInactiveCell", calHitContainers); 
    addCalHitContainer("TileCalibHitDeadMaterial", calHitContainers); 
    addCalHitContainer("LArCalibrationHitActive", calHitContainers); 
    addCalHitContainer("LArCalibrationHitInactive", calHitContainers); 
    addCalHitContainer("LArCalibrationHitDeadMaterial", calHitContainers); 

    // process taus
    ATH_MSG_VERBOSE("Number of taus: " << xTauContainer->size());
    for(auto xTau : *xTauContainer) {
      const xAOD::TruthParticle* xTruthTau = xAOD::TauHelpers::getLink<xAOD::TruthParticle>(xTau, "truthParticleLink");
      if(!xTruthTau) continue;
      if(!xTruthTau->isTau()) continue;
      if(!(bool)xTruthTau->auxdata<char>("IsHadronicTau")) continue;
      if(!xTruthTau->hasDecayVtx()) continue;
      if(xTruthTau->nChildren()<2) continue;
      ATH_MSG_VERBOSE("Found true hadtau match, nTrack: " << xTau->nTracks() 
                    << ", e: " << xTau->e() 
                    << ", pt: " << xTau->pt() );

      // get barcodes of children
      std::set<int> chargedBarcodes;
      std::set<int> neutralBarcodes;
      std::set<int> otherBarcodes;
      bool invalidTau = false;
      for( size_t iChild=0; iChild<xTruthTau->nChildren(); iChild++) {
        const xAOD::TruthParticle* child = xTruthTau->child(iChild);
        // tau (invalid)
        if (child->isTau()) {
          invalidTau = true;
          break;
        }
        // pi0
        if (child->pdgId() == 111) {
          ATH_MSG_VERBOSE("found pi0 daughter, eta: " << child->eta() << ", phi: " << child->phi() << ", energy: " << child->e());
          for(auto v : getChildren(child))
            ATH_MSG_VERBOSE("--> " << v->pdgId() << ", barcode: " << v->barcode() << ", energy: " << v->e()); 
          for(auto v : getChildBarcodes(child)) 
            neutralBarcodes.insert(v);
        }
        // charged
        else if (child->isCharged() and child->isHadron()) {
          for(auto v : getChildBarcodes(child))
            chargedBarcodes.insert(v);
        }
        // other
        else {
          for(auto v : getChildBarcodes(child))
            otherBarcodes.insert(v);
        }
      }
      if ( invalidTau ) continue;
     
      // process PFOs 
      ATH_MSG_VERBOSE("Number of PFOs: " << xTau->nNeutralPFOs());
      for(size_t iPFO=0; iPFO<xTau->nNeutralPFOs(); iPFO++) {
        // init total cal-hit energy in cluster
        float energyCalHit = 0.;
        // init total cal-hit energy from pi0, charged hadron 
        // or other daughters of tau 
        float energyCalHitNeutral = 0.;
        float energyCalHitCharged = 0.;
        float energyCalHitOther   = 0.;

        // get PFO
        const xAOD::PFO* pfo = xTau->neutralPFO(iPFO);
        if( not pfo ) {
          ATH_MSG_WARNING("Tau PFO link invalid"); 
          continue;
        } 
        // get cluster
        const xAOD::CaloCluster* cluster = pfo->cluster(0);
        if( not cluster ) {
          ATH_MSG_WARNING("Neutral PFO cluster link invalid");
          continue;
        }
        SG::AuxElement::Decorator< float > ptClusterDeco( "ptCluster" );
        ptClusterDeco(*pfo) = cluster->pt();

        // get cells
        const CaloClusterCellLink* cellLink = cluster->getCellLinks(); 
        if( not cellLink ) {
          ATH_MSG_WARNING("PFO cluster cellLinks invalid");
          continue;
        }

        // store cell IDs
        std::set<Identifier> cellIDs;
        for (auto cell : *cellLink) {
          if( not cell ) {
            ATH_MSG_WARNING("Cell link invalid");
            continue;
          }
          cellIDs.insert(cell->ID());
        }
        
        // sum energy from cal-hits 
        int nCalHits=0; 
        for (auto vec : calHitContainers ) {
          if( not vec ) {
            ATH_MSG_WARNING("Calibration Hit Vector invalid");
            continue;
          }
          for (auto calHit : *vec ) {
            if( not calHit ) {
              ATH_MSG_WARNING("Calibration Hit invalid");
              continue;
            }
            if( not cellIDs.count(calHit->cellID())) continue;
            nCalHits++;
            float energy = calHit->energyTotal();
            energyCalHit += energy;

            if( neutralBarcodes.count(calHit->particleID())) {
              energyCalHitNeutral += energy; 
            }
            else if( chargedBarcodes.count(calHit->particleID())) {
              energyCalHitCharged += energy; 
            }
            else if( otherBarcodes.count(calHit->particleID())) {
              energyCalHitOther += energy; 
            }
          }
        }
        ATH_MSG_VERBOSE("Cluster calHits: " << nCalHits 
                   << ", eta: " << cluster->eta() 
                   << ", energy: " << cluster->e() 
                   << ", total calhit energy: " << energyCalHit 
                   << ", neutral calhit energy: " << energyCalHitNeutral );

        // decorate cal-hit summary info 
        float ptOnE = 1. / cosh(cluster->eta()); // (assume m=0) 
        SG::AuxElement::Decorator< float > ptCalHitDeco( "ptCalHit" );
        ptCalHitDeco(*pfo) = energyCalHit * ptOnE;
        SG::AuxElement::Decorator< float > ptCalHitNeutralDeco( "ptCalHitNeutral" );
        ptCalHitNeutralDeco(*pfo) = energyCalHitNeutral * ptOnE;
        SG::AuxElement::Decorator< float > ptCalHitChargedDeco( "ptCalHitCharged" );
        ptCalHitChargedDeco(*pfo) = energyCalHitCharged * ptOnE;
        SG::AuxElement::Decorator< float > ptCalHitOtherDeco( "ptCalHitOther" );
        ptCalHitOtherDeco(*pfo) = energyCalHitOther * ptOnE;
  
      } // pfo loop
      ATH_MSG_VERBOSE("End PFO loop"); 
    } // tau loop
    ATH_MSG_VERBOSE("Finished decorating PFO calHit info");
    return StatusCode::SUCCESS;
  } 

  /** private helper functions */
  void TauPFOCalHitDecorator::addCalHitContainer( 
    const char* name, 
    std::vector<const CaloCalibrationHitContainer*>& vec) const
  {
    auto cnt = evtStore()->retrieve<const CaloCalibrationHitContainer>(name);
    if (not cnt) ATH_MSG_WARNING("Could not retrieve " << name << " container");
    else vec.push_back(cnt);
  }
}

