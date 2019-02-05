/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/Electron.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODTracking/TrackParticle.h"


bool xAOD::EgammaHelpers::isElectron(const xAOD::Egamma *eg){
  return ( (eg->type()==xAOD::Type::Electron) && 
	   (!eg->author(xAOD::EgammaParameters::AuthorFwdElectron)));
}
// ==================================================================
bool xAOD::EgammaHelpers::isFwdElectron(const xAOD::Egamma *eg){
  return eg->author(xAOD::EgammaParameters::AuthorFwdElectron);
}
// ==================================================================
bool xAOD::EgammaHelpers::isPhoton(const xAOD::Egamma *eg){
  return (eg->type()==xAOD::Type::Photon);
}
// ==================================================================
bool xAOD::EgammaHelpers::isConvertedPhoton(const xAOD::Egamma *eg){
  if (!eg || eg->type()!=xAOD::Type::Photon){
    return false;
  }
  const xAOD::Photon *ph = static_cast<const xAOD::Photon*>(eg);
  return (ph->nVertices()>0);
}
// ==================================================================
bool xAOD::EgammaHelpers::isBarrel(const xAOD::Egamma *eg){
  return (eg ? isBarrel(eg->caloCluster()) : false);
}
// ==================================================================
bool xAOD::EgammaHelpers::isBarrel(const xAOD::CaloCluster *cluster){
  if (cluster->inBarrel() &&  cluster->inEndcap()){
    return  cluster->eSample(CaloSampling::EMB2) >= cluster->eSample(CaloSampling::EME2);
  }
  return cluster->inBarrel();
}
// ==================================================================
const std::vector< ElementLink< xAOD::CaloClusterContainer > > xAOD::EgammaHelpers::getAssociatedTopoClustersLinks(const xAOD::CaloCluster *cluster){ 

  static const SG::AuxElement::Accessor < std::vector< ElementLink< xAOD::CaloClusterContainer > > > caloClusterLinks("constituentClusterLinks");
  std::vector< ElementLink< xAOD::CaloClusterContainer > > veclinks; 
  if(caloClusterLinks.isAvailable(*cluster)){ 
    veclinks=caloClusterLinks(*cluster);
  }
  return veclinks;
}
// ==================================================================
const std::vector<const xAOD::CaloCluster*> xAOD::EgammaHelpers::getAssociatedTopoClusters(const xAOD::CaloCluster *cluster){ 
  std::vector< const xAOD::CaloCluster* > topoclusters;
  std::vector< ElementLink< xAOD::CaloClusterContainer > > veclinks = xAOD::EgammaHelpers::getAssociatedTopoClustersLinks(cluster);
  for (auto i : veclinks){
    if(i.isValid()){
      topoclusters.push_back(*i);
    }
    else{
      topoclusters.push_back(0);
    }
  }  
  return topoclusters;
}
// ==================================================================
const std::set<const xAOD::TrackParticle*> xAOD::EgammaHelpers::getTrackParticles(const xAOD::Egamma *eg, 
										  bool useBremAssoc /* = true */, 
										  bool allParticles /* = true */){  

  if (eg) {
    if (eg->type()==xAOD::Type::Electron) {
      const xAOD::Electron* el = static_cast<const xAOD::Electron*> (eg);
      if (el) {
        return getTrackParticles(el, useBremAssoc, allParticles);
      }
    }
    else if (eg->type()==xAOD::Type::Photon) {
      const xAOD::Photon* ph = static_cast<const xAOD::Photon*> (eg);  
      if (ph) {
        return getTrackParticles(ph, useBremAssoc);
      }
    }
  }
  return std::set<const xAOD::TrackParticle*>{};
}
// ==================================================================
const std::vector<const xAOD::TrackParticle*> xAOD::EgammaHelpers::getTrackParticlesVec(const xAOD::Egamma *eg, 
											bool useBremAssoc /* = true */, 
											bool allParticles /* = true */){  
  if (eg) {
    if (eg->type()==xAOD::Type::Electron) {
      const xAOD::Electron* el = static_cast<const xAOD::Electron*> (eg);
      if (el) {
        return getTrackParticlesVec(el, useBremAssoc, allParticles);
      }
    }
    else if (eg->type()==xAOD::Type::Photon) {
      const xAOD::Photon* ph = static_cast<const xAOD::Photon*> (eg);  
      if (ph) {
        return getTrackParticlesVec(ph, useBremAssoc);
      }
    }
  }
  return std::vector<const xAOD::TrackParticle*>{};
}
// ==================================================================
int xAOD::EgammaHelpers::summaryValueInt(const xAOD::TrackParticle& tp, 
					 const xAOD::SummaryType& info, 
					 int deflt /* = -999 */){
  uint8_t dummy(0);
  return (tp.summaryValue(dummy, info) ? dummy : deflt);
}
// ==================================================================
float xAOD::EgammaHelpers::summaryValueFloat(const xAOD::TrackParticle& tp, 
					     const xAOD::SummaryType& info, 
					     int deflt /* = -999. */){
  float dummy(0);
  return (tp.summaryValue(dummy, info) ? dummy : deflt);
}
