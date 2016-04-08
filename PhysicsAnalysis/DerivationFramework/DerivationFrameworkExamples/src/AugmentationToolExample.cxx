/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// AugmentationToolExample.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Thomas Gillam (thomas.gillam@cern.ch)
//
// This is a trivial example of tool that creates extra information
// and places it in the StoreGate
// The example shows how to do it with (a) simple vectors and (b) dressing
// of objects (tracks in this case)
// The same information is written into both

#include "DerivationFrameworkExamples/AugmentationToolExample.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include <vector>
#include <string>

namespace DerivationFramework {

  AugmentationToolExample::AugmentationToolExample(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    AthAlgTool(t,n,p)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
  }

  StatusCode AugmentationToolExample::addBranches() const
  {

      // Set up the vector
      std::unique_ptr<std::vector<float> > track_z0_PV(new std::vector<float>());

      // Set up the decorators 
      SG::AuxElement::Decorator< float > decorator("DFDecoratorExample"); 

      // CALCULATION OF THE NEW VARIABLE
      // Get Primary vertex
      const xAOD::VertexContainer* vertices =  evtStore()->retrieve< const xAOD::VertexContainer >("PrimaryVertices");
    
      if(!vertices) {
        ATH_MSG_ERROR ("Couldn't retrieve VertexContainer with key PrimaryVertices");
        return StatusCode::FAILURE;
      }

      const xAOD::Vertex* pv(0);
      for (const xAOD::Vertex* vx : *vertices) {
        if (vx->vertexType() == xAOD::VxType::PriVtx) {
          pv = vx;
          break;
        }
      }

      // Get the track container
      const xAOD::TrackParticleContainer* tracks = evtStore()->retrieve< const xAOD::TrackParticleContainer >("InDetTrackParticles");
      
      if(!tracks) {
        ATH_MSG_ERROR ("Couldn't retrieve TrackParticleContainer with key InDetTrackParticles");
        return StatusCode::FAILURE;
      }

      // Get track z0 w.r.t PV: this is what we're adding
      for (xAOD::TrackParticleContainer::const_iterator trackIt=tracks->begin(); trackIt!=tracks->end(); ++trackIt) {
        if (pv) {
          float z0wrtPV = (*trackIt)->z0() + (*trackIt)->vz() - pv->z(); // CALCULATE THE QUANTITY
          track_z0_PV->push_back(z0wrtPV); // ADD TO VECTOR
          decorator(**trackIt) = z0wrtPV; // DECORATE THE TRACK
        } else {
          track_z0_PV->push_back(999.);
          decorator(**trackIt) = 999.;
        }
      }

      // Write decision to SG for access by downstream algs 
      if (evtStore()->contains<std::vector<float> >("DFAugmentationExample")) {
          ATH_MSG_ERROR("Tool is attempting to write StoreGate keys which already exists. Please use a different key");
          return StatusCode::FAILURE;
      } else {
        CHECK(evtStore()->record(std::move(track_z0_PV), "DFAugmentationExample"));       
      }

      return StatusCode::SUCCESS;
  }
}
