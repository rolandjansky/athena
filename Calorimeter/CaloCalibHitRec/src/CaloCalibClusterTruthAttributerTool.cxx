/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloCalibHitRec/CaloCalibClusterTruthAttributerTool.h"

CaloCalibClusterTruthAttributerTool::CaloCalibClusterTruthAttributerTool(const std::string& type, const std::string& name,  const IInterface* parent) : base_class(type,name,parent) {
}

CaloCalibClusterTruthAttributerTool::~CaloCalibClusterTruthAttributerTool(){}

StatusCode CaloCalibClusterTruthAttributerTool::calculateTruthEnergies(const xAOD::CaloCluster& theCaloCluster, unsigned int numTruthParticles, const std::map<Identifier,std::vector<const CaloCalibrationHit*> >& identifierToCaloHitMap, const std::map<unsigned int,const xAOD::TruthParticle*>& truthBarcodeToTruthParticleMap, std::vector<std::pair<unsigned int, double > >& barcodeTrueCalHitEnergy) const{

  const CaloClusterCellLink* theCellLinks = theCaloCluster.getCellLinks();

  if (!theCellLinks) {
    ATH_MSG_ERROR("A CaloCluster has no CaloClusterCellLinks");
    return StatusCode::FAILURE;
  }
    
  //loop once over the cells to find the barcodes and pt of truth particles in this cluster
  std::map<unsigned int, double> barcodeTruePtMap;
  
  for (auto thisCaloCell : *theCellLinks){

    if (!thisCaloCell){
      ATH_MSG_WARNING("Have invalid pointer to CaloCell");
      continue;
    }
    
    //get the unique calorimeter cell identifier
    Identifier cellID = thisCaloCell->ID();

    //look up the calibration hit that corresponds to this calorimeter cell - we use find because not all calorimeter cells will have calibration hits
    std::map<Identifier,std::vector<const CaloCalibrationHit*> >::const_iterator identifierToCaloHitMapIterator = identifierToCaloHitMap.find(cellID);
    if (identifierToCaloHitMap.end() == identifierToCaloHitMapIterator) continue;
    const std::vector<const CaloCalibrationHit*>& theseCalibrationHits = (*identifierToCaloHitMapIterator).second;

    for (auto thisCalibrationHit : theseCalibrationHits){
      unsigned int barcode = thisCalibrationHit->particleID();
      const xAOD::TruthParticle* theTruthParticle = truthBarcodeToTruthParticleMap.at(barcode);
      double theTruthParticlePt = theTruthParticle->pt();

      barcodeTruePtMap[barcode] = theTruthParticlePt;

    }//calibration hit loop
    
  }//first loop on calorimeter cells to find leading three truth particles with calibration hits

  std::vector<std::pair<unsigned int, double > > barcodeTruePtPairs;

  for (const auto& thisEntry : barcodeTruePtMap) barcodeTruePtPairs.emplace_back(thisEntry);

  std::sort(barcodeTruePtPairs.begin(),barcodeTruePtPairs.end(),[]( std::pair<unsigned int, double> a, std::pair<unsigned int, double> b) -> bool {return a.second > b.second;} );

  std::vector<std::pair<unsigned int, double > > barcodeTruePtPairs_truncated;
  if (barcodeTruePtPairs.size() < numTruthParticles) numTruthParticles = barcodeTruePtPairs.size();
  barcodeTruePtPairs_truncated.reserve(numTruthParticles);
  for ( unsigned int counter = 0; counter < numTruthParticles; counter++) barcodeTruePtPairs_truncated.push_back(barcodeTruePtPairs[counter]);

  for (const auto& thisPair : barcodeTruePtPairs_truncated) ATH_MSG_DEBUG("Truncated loop: barcode and pt are " << thisPair.first << " and " << thisPair.second);
  
  //make secod vector to store calibraiton hit energies, which should be same size as previous vector
  barcodeTrueCalHitEnergy = barcodeTruePtPairs_truncated;
  for (auto& thisPair : barcodeTrueCalHitEnergy) thisPair.second = 0.0;

  //now loop on calorimeter cells again to sum up the truth energies of the leading three particles.    
  for (auto thisCaloCell : *theCellLinks){
    
    if (!thisCaloCell){
      ATH_MSG_WARNING("Have invalid pointer to CaloCell");
      continue;
    }

    //get the unique calorimeter cell identifier
    Identifier cellID = thisCaloCell->ID();
    
    //look up the calibration hit that corresponds to this calorimeter cell - we use find because not all calorimeter cells will have calibration hits
    std::map<Identifier,std::vector<const CaloCalibrationHit*> >::const_iterator identifierToCaloHitMapIterator = identifierToCaloHitMap.find(cellID);
    if (identifierToCaloHitMap.end() == identifierToCaloHitMapIterator) continue;
    std::vector<const CaloCalibrationHit*> theseCalibrationHits = (*identifierToCaloHitMapIterator).second;

    for (auto thisCalibrationHit : theseCalibrationHits){
      unsigned int barcode = thisCalibrationHit->particleID();
      double thisCalHitTruthEnergy = thisCalibrationHit->energyEM() + thisCalibrationHit->energyNonEM();
      if (true == m_fullTruthEnergy) thisCalHitTruthEnergy += (thisCalibrationHit->energyEscaped() + thisCalibrationHit->energyInvisible());

      for (unsigned int counter =  0; counter < barcodeTruePtPairs_truncated.size(); counter++){
        auto thisPair = barcodeTruePtPairs_truncated[counter];
        if (barcode == thisPair.first) {
          barcodeTrueCalHitEnergy[counter].second += thisCalHitTruthEnergy;
          ATH_MSG_DEBUG("For barcode pair of " << barcode << " and " << thisPair.first << " add truth energy of " << thisCalHitTruthEnergy << " and running total is " << barcodeTrueCalHitEnergy[counter].second);
        }
      }
      
    }//calibration hit loop
    
  }//second loop on calorimeter cells to sum up truth energies of the leading three particles

  for (const auto& thisPair : barcodeTrueCalHitEnergy) ATH_MSG_DEBUG("Truncated loop 2: barcode and true energy are " << thisPair.first << " and " << thisPair.second << " for cluster with e, eta of " << theCaloCluster.e() << " and " << theCaloCluster.eta() );

  return StatusCode::SUCCESS;
  
}
