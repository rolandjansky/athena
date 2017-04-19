/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local includes
#include "PhotonVertexSelection/ElectronRemovalTool.h"
#include "PhotonVertexSelection/PhotonVertexHelpers.h"

// EDM includes
#include "xAODEgamma/ElectronContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/EgammaDefs.h"

// Framework includes
#include "PathResolver/PathResolver.h"

// ROOT includes

// std includes
#include <algorithm>

namespace CP {

  //____________________________________________________________________________


  //____________________________________________________________________________
  ElectronRemovalTool::ElectronRemovalTool(const std::string &name)
  : asg::AsgTool(name)
  { 
  
  }

  //____________________________________________________________________________
  ElectronRemovalTool::~ElectronRemovalTool()
  {
  
  }

  //____________________________________________________________________________
  StatusCode ElectronRemovalTool::initialize()
  {
    ATH_MSG_INFO("Initializing ElectronRemovalTool...");
    
    return StatusCode::SUCCESS;
  }

  //____________________________________________________________________________
  StatusCode ElectronRemovalTool::removeElectronsFromVertex(xAOD::VertexContainer & output, const xAOD::VertexContainer & input, xAOD::ElectronContainer & toRemove, bool updateAuxInfo) const{
 

    std::vector<const xAOD::TrackParticle *> electronsInDetTrackParticles = getElectronTPs(toRemove);

    int element_counter=0;
    //loop over input vertices
    for (const xAOD::Vertex* v : input) {
      bool containsElectrons = false;
      xAOD::Vertex * nv = new xAOD::Vertex;
      //set output aux info to be same as input (for time being)
      nv->makePrivateStore(v);
      
      std::vector< ElementLink< xAOD::TrackParticleContainer > >::iterator InDetTrackParticleLinksIter;
      std::vector< ElementLink< xAOD::TrackParticleContainer > > InDetTrackParticleLinksToKeep =  v->trackParticleLinks(); //for modifying
      
      //loop over TrackPArticles associated to vertex
      for(InDetTrackParticleLinksIter = InDetTrackParticleLinksToKeep.begin();InDetTrackParticleLinksIter != InDetTrackParticleLinksToKeep.end();){
	const xAOD::TrackParticle * TPtoTest = (*(InDetTrackParticleLinksIter->cptr()));
	//checking if any of the Vertex TrackParticles are the same as the electron TrackParticles
	if(find(electronsInDetTrackParticles.begin(),electronsInDetTrackParticles.end(),TPtoTest)!=electronsInDetTrackParticles.end()){
	  //remove the common ones
	  InDetTrackParticleLinksIter = InDetTrackParticleLinksToKeep.erase(InDetTrackParticleLinksIter);//iterator should be moved to next valid one after removal
	  containsElectrons = true;	
	}
	
	else ++InDetTrackParticleLinksIter; //move iterator forward 'by hand' if we don't remove link
      }   
      
      if(containsElectrons){
	//if the vertex contains an electron TP, clear its tracks, and replace with the element links from above (excluding electron)
	nv->clearTracks();
	nv->setTrackParticleLinks(InDetTrackParticleLinksToKeep);
      }
      
      if(updateAuxInfo){
	nv->auxdecor<float>("sumPt")= xAOD::PVHelpers::getVertexSumPt(nv, 1,false);
	nv->auxdecor<float>("sumPt2")= xAOD::PVHelpers::getVertexSumPt(nv, 2,false);
      }
      
      output.push_back(nv);
      element_counter++;
    }
    
    return StatusCode::SUCCESS;
  }

  StatusCode ElectronRemovalTool::removeElectronsFromVertex(xAOD::VertexContainer & vertices, xAOD::ElectronContainer & toRemove, bool updateAuxInfo) const{

    
    std::vector<const xAOD::TrackParticle *> electronsInDetTrackParticles = getElectronTPs(toRemove);

    int element_counter=0;
    //loop over input vertices
    for (xAOD::Vertex* v : vertices) {
      bool containsElectrons = false;
      
      std::vector< ElementLink< xAOD::TrackParticleContainer > >::iterator InDetTrackParticleLinksIter;
      std::vector< ElementLink< xAOD::TrackParticleContainer > > InDetTrackParticleLinksToKeep =  v->trackParticleLinks(); //for modifying
      
      //loop over TrackPArticles associated to vertex
      for(InDetTrackParticleLinksIter = InDetTrackParticleLinksToKeep.begin();InDetTrackParticleLinksIter != InDetTrackParticleLinksToKeep.end();){
	const xAOD::TrackParticle * TPtoTest = (*(InDetTrackParticleLinksIter->cptr()));
	//checking if any of the Vertex TrackParticles are the same as the electron TrackParticles
	if(find(electronsInDetTrackParticles.begin(),electronsInDetTrackParticles.end(),TPtoTest)!=electronsInDetTrackParticles.end()){
	  //remove the common ones
	  InDetTrackParticleLinksIter = InDetTrackParticleLinksToKeep.erase(InDetTrackParticleLinksIter);//iterator should be moved to next valid one after removal
	  containsElectrons = true;	
	}
	
	else ++InDetTrackParticleLinksIter; //move iterator forward 'by hand' if we don't remove link
      }   
      
      if(containsElectrons){
	//if the vertex contains an electron TP, clear its tracks, and replace with the element links from above (excluding electron)
	v->clearTracks();
	v->setTrackParticleLinks(InDetTrackParticleLinksToKeep);
      }
      
      if(updateAuxInfo){
	v->auxdecor<float>("sumPt")= xAOD::PVHelpers::getVertexSumPt(v, 1,false);
	v->auxdecor<float>("sumPt2")= xAOD::PVHelpers::getVertexSumPt(v, 2,false);
      }
      
      element_counter++;
    }

    return StatusCode::SUCCESS;

  }

  xAOD::VertexContainer * ElectronRemovalTool::removeElectronsFromVertex(const xAOD::VertexContainer & input, xAOD::ElectronContainer & toRemove, bool updateAuxInfo) const{

    xAOD::VertexContainer * output = new xAOD::VertexContainer;
    
    std::vector<const xAOD::TrackParticle *> electronsInDetTrackParticles = getElectronTPs(toRemove);
    
    int element_counter=0;
    //loop over input vertices
    for (const xAOD::Vertex* v : input) {
      bool containsElectrons = false;
      xAOD::Vertex * nv = new xAOD::Vertex;
      //set output aux info to be same as input (for time being)
      nv->makePrivateStore(v);
      
      std::vector< ElementLink< xAOD::TrackParticleContainer > >::iterator InDetTrackParticleLinksIter;
      std::vector< ElementLink< xAOD::TrackParticleContainer > > InDetTrackParticleLinksToKeep =  v->trackParticleLinks(); //for modifying
      
      //loop over TrackPArticles associated to vertex
      for(InDetTrackParticleLinksIter = InDetTrackParticleLinksToKeep.begin();InDetTrackParticleLinksIter != InDetTrackParticleLinksToKeep.end();){
	const xAOD::TrackParticle * TPtoTest = (*(InDetTrackParticleLinksIter->cptr()));
	//checking if any of the Vertex TrackParticles are the same as the electron TrackParticles
	if(find(electronsInDetTrackParticles.begin(),electronsInDetTrackParticles.end(),TPtoTest)!=electronsInDetTrackParticles.end()){
	  //remove the common ones
	  InDetTrackParticleLinksIter = InDetTrackParticleLinksToKeep.erase(InDetTrackParticleLinksIter);//iterator should be moved to next valid one after removal
	  containsElectrons = true;	
	}
	
	else ++InDetTrackParticleLinksIter; //move iterator forward 'by hand' if we don't remove link
      }   
      
      if(containsElectrons){
	//if the vertex contains an electron TP, clear its tracks, and replace with the element links from above (excluding electron)
	nv->clearTracks();
	nv->setTrackParticleLinks(InDetTrackParticleLinksToKeep);
      }
      
      if(updateAuxInfo){
	nv->auxdecor<float>("sumPt")= xAOD::PVHelpers::getVertexSumPt(nv, 1,false);
	nv->auxdecor<float>("sumPt2")= xAOD::PVHelpers::getVertexSumPt(nv, 2,false);
      }
      
      output->push_back(nv);
      element_counter++;
    }

    return output;

  }


  std::vector<const xAOD::TrackParticle *> ElectronRemovalTool::getElectronTPs( xAOD::ElectronContainer & electrons) const {
    
    //get TrackParticles for electrons
    std::vector<const xAOD::TrackParticle *> electronsInDetTrackParticles;

    for (const xAOD::Electron* e : electrons) {
      std::set<const xAOD::TrackParticle *> singleElectronInDetTrackParticles =  xAOD::EgammaHelpers::getTrackParticles(e);	 
      //track particles from both electrons in single vector, since we want to remove them all anyway
      electronsInDetTrackParticles.insert(electronsInDetTrackParticles.begin(), singleElectronInDetTrackParticles.begin(),singleElectronInDetTrackParticles.end());
    } 
  
    return electronsInDetTrackParticles;
  }

} // namespace CP

