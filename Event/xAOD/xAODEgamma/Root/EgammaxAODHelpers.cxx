/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
std::vector< ElementLink< xAOD::CaloClusterContainer > > xAOD::EgammaHelpers::getAssociatedTopoClustersLinks(const xAOD::CaloCluster *cluster){

  static const SG::AuxElement::Accessor < std::vector< ElementLink< xAOD::CaloClusterContainer > > > caloClusterLinks("constituentClusterLinks");
  std::vector< ElementLink< xAOD::CaloClusterContainer > > veclinks; 
  if(caloClusterLinks.isAvailable(*cluster)){ 
    veclinks=caloClusterLinks(*cluster);
  }
  return veclinks;
}
// ==================================================================
std::vector<const xAOD::CaloCluster*> xAOD::EgammaHelpers::getAssociatedTopoClusters(const xAOD::CaloCluster *cluster){ 
  std::vector< const xAOD::CaloCluster* > topoclusters;
  std::vector< ElementLink< xAOD::CaloClusterContainer > > veclinks = xAOD::EgammaHelpers::getAssociatedTopoClustersLinks(cluster);
  for (const auto& i : veclinks){
    if(i.isValid()){
      topoclusters.push_back(*i);
    }
    else{
      topoclusters.push_back(nullptr);
    }
  }  
  return topoclusters;
}
// ==================================================================
std::vector< ElementLink< xAOD::FlowElementContainer > > xAOD::EgammaHelpers::getAssociatedFlowElementsLinks(const xAOD::Egamma *eg,
													     bool neutral){
  static const SG::AuxElement::Accessor < std::vector< ElementLink< xAOD::FlowElementContainer > > > nflowElementLinks("neutralFELinks");
  static const SG::AuxElement::Accessor < std::vector< ElementLink< xAOD::FlowElementContainer > > > cflowElementLinks("chargedFELinks");
  std::vector< ElementLink< xAOD::FlowElementContainer > > veclinks;
  if(neutral && nflowElementLinks.isAvailable(*eg)){
    veclinks=nflowElementLinks(*eg);
  } else if(!neutral && cflowElementLinks.isAvailable(*eg)){
    veclinks=cflowElementLinks(*eg);
  }
  return veclinks;
}
// ==================================================================
std::vector<const xAOD::FlowElement*> xAOD::EgammaHelpers::getAssociatedFlowElements(const xAOD::Egamma *eg,
										     bool neutral, bool charged){
  std::vector< const xAOD::FlowElement* > flowelements;
  if (!neutral && !charged)
    return flowelements;
  std::vector< ElementLink< xAOD::FlowElementContainer > > veclinks = xAOD::EgammaHelpers::getAssociatedFlowElementsLinks(eg,neutral);
  for (const auto& i : veclinks){
    if(i.isValid()){
      flowelements.push_back(*i);
    }
    else{
      flowelements.push_back(nullptr);
    }
  }
  if (neutral && charged) {
    std::vector< ElementLink< xAOD::FlowElementContainer > > oveclinks = xAOD::EgammaHelpers::getAssociatedFlowElementsLinks(eg,false);
    for (const auto& i : oveclinks){
      if(i.isValid()){
	flowelements.push_back(*i);
      }
      else{
	flowelements.push_back(nullptr);
      }
    }
  }
  return flowelements;
}
// ==================================================================
std::set<const xAOD::TrackParticle*> xAOD::EgammaHelpers::getTrackParticles(const xAOD::Egamma *eg, 
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
std::vector<const xAOD::TrackParticle*> xAOD::EgammaHelpers::getTrackParticlesVec(const xAOD::Egamma *eg, 
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
