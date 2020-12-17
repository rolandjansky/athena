/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "DerivationFrameworkBPhys/VertexTrackIsolation.h"

#include <vector>
#include <string>
#include "TVector3.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODBPhys/BPhysHelper.h"
#include "xAODBPhys/BPhysHypoHelper.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"

//#include "IsolationTool/CaloIsolationTool.h"
//#include "IsolationTool/TrackIsolationTool.h"
#include "RecoToolInterfaces/ITrackIsolationTool.h"
#include "xAODPrimitives/IsolationHelpers.h"  //For the definition of Iso::conesize

//#include "IsolationTool/IsolationHelper.h"
//#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"


//#include "Identifier/Identifier32.h"
using namespace std;
namespace DerivationFramework {

  VertexTrackIsolation::VertexTrackIsolation(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_trackIsoTool("xAOD::TrackIsolationTool"),
    m_trackContainerName("InDetTrackParticles"),
    m_vertexContainerName("NONE"),
    m_cones(),
    m_vertexType(7),
    
    m_doIsoPerTrk(false),
    m_removeDuplicate(2)
  {
        ATH_MSG_DEBUG("in constructor");
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    
    // Declare tools                      
    declareProperty("TrackIsoTool" , m_trackIsoTool);

    declareProperty("TrackContainer" , m_trackContainerName);
    declareProperty("InputVertexContainer" , m_vertexContainerName);
    declareProperty("PassFlags"                 , m_passFlags);
    declareProperty("IsolationTypes"                 , m_cones);
    declareProperty("DoVertexTypes"                 , m_vertexType);

    declareProperty("DoIsoPerTrk"                , m_doIsoPerTrk, "New property to deal with track isolation per track, the default option (m_doIsoPerTrk=false) preserves the old behavior");
    declareProperty("RemoveDuplicate"            , m_removeDuplicate, "Used with DoIsoPerTrk");
  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  
  StatusCode VertexTrackIsolation::initialize()
  {
  
    ATH_MSG_DEBUG("in initialize()");
 
    // retrieve TrackIsolationTool
    CHECK( m_trackIsoTool.retrieve() );
    
    //Check that flags were given to tag the correct vertices
    if(m_passFlags.empty()){
        ATH_MSG_WARNING("As no pass-flags are given, no vertices will be decorated");
    }

    // Control the IsolationType sequence
    if(m_cones.empty()){
        ATH_MSG_INFO("Setting ptcones to default");

	    if(m_doIsoPerTrk) m_cones.push_back(xAOD::Iso::ptcone50);
	    m_cones.push_back(xAOD::Iso::ptcone40);
	    m_cones.push_back(xAOD::Iso::ptcone30);
	    m_cones.push_back(xAOD::Iso::ptcone20);

    }

    return StatusCode::SUCCESS;
    
  }
  
  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

  StatusCode VertexTrackIsolation::finalize()
  {
    // everything all right
    return StatusCode::SUCCESS;
  }

  // check if the two vertices are composed of the same set of tracks
  bool VertexTrackIsolation::isSame(const xAOD::Vertex* theVtx1, const xAOD::Vertex* theVtx2) const {
    if(!theVtx1 || !theVtx2) return false;
    if(theVtx1==theVtx2) return true;
    if(theVtx1->nTrackParticles() != theVtx2->nTrackParticles()) return false;

    if(m_removeDuplicate==2 && theVtx1->nTrackParticles()==4) { // a special case with sub-structure
      bool firstTwoAreSame = std::set<const xAOD::TrackParticle*>( { theVtx1->trackParticle(0), theVtx1->trackParticle(1)} ) == std::set<const xAOD::TrackParticle*>( {theVtx2->trackParticle(0), theVtx2->trackParticle(1)} ); // the 1st pair of tracks
      bool lastTwoAreSame = std::set<const xAOD::TrackParticle*>( { theVtx1->trackParticle(2), theVtx1->trackParticle(3)} ) == std::set<const xAOD::TrackParticle*>( {theVtx2->trackParticle(2), theVtx2->trackParticle(3)} ); // the 2nd pair of tracks
      if(firstTwoAreSame && lastTwoAreSame) return true;
      else return false;
    }
    else { // the general case
      std::set<const xAOD::TrackParticle*> vtxset1;
      std::set<const xAOD::TrackParticle*> vtxset2;
      for(size_t i=0; i<theVtx1->nTrackParticles(); i++) vtxset1.insert(theVtx1->trackParticle(i));
      for(size_t i=0; i<theVtx2->nTrackParticles(); i++) vtxset2.insert(theVtx2->trackParticle(i));
      return vtxset1 == vtxset2;
    }
  }
  
  bool VertexTrackIsolation::isContainedIn(const xAOD::Vertex* theVtx, const std::vector<const xAOD::Vertex*> &theColl) const {
    for ( const auto vtxPtr : theColl ) {
      if ( isSame(vtxPtr, theVtx) ) return true;
    }
    return false;
  }
  
  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  
  StatusCode VertexTrackIsolation::addBranches() const {

    const xAOD::TrackParticleContainer*    idTrackParticleContainer = NULL;
    const xAOD::VertexContainer*  vertexContainer = NULL;

    if(evtStore()->contains<xAOD::TrackParticleContainer>(m_trackContainerName)) {
        CHECK( evtStore()->retrieve(idTrackParticleContainer, m_trackContainerName) );
    }
    else{ATH_MSG_ERROR("Failed loading IdTrackparticleContainer container");
    	return StatusCode::FAILURE;
    }

    //	load vertices
    if(evtStore()->contains<xAOD::VertexContainer>(m_vertexContainerName)) {
        CHECK( evtStore()->retrieve(vertexContainer, m_vertexContainerName) );
    }
    else{ATH_MSG_ERROR("Failed loading vertex container");
    	return StatusCode::FAILURE;
    }

    std::vector<const xAOD::Vertex*> outVtxContainer;

    //Convert m_cones (done per-event to avoid needing extra public dependency)

    std::vector<xAOD::Iso::IsolationType> cones; cones.resize(m_cones.size());

    for (unsigned int i =0; i< m_cones.size(); i++)
    	cones[i] = xAOD::Iso::IsolationType(m_cones[i]);
    //for(unsigned int cone : m_cones)
    //	cones.push_back(xAOD::Iso::IsolationType(cone));

    ATH_MSG_DEBUG("The provided IsolationTypes are re-ordered internally");
    std::sort(cones.begin(),cones.end(),[](xAOD::Iso::IsolationType i, xAOD::Iso::IsolationType j) { return xAOD::Iso::coneSize(i) > xAOD::Iso::coneSize(j); } );

    //	loop over vertices
    for(auto vertex : *vertexContainer){

      bool passed = false;
      for(std::vector<std::string>::const_iterator flagItr = m_passFlags.begin(); flagItr!=m_passFlags.end(); ++flagItr) {
        SG::AuxElement::Accessor<Char_t> flagAcc(*flagItr);
        if(flagAcc.isAvailable(*vertex) && flagAcc(*vertex) != 0) {
          passed = true;
          break;
        }
      } // end of loop over flags
      if(passed){
	        if(!m_doIsoPerTrk) { // for legacy
		  if(vertex->trackParticleLinks().size() != 3)ATH_MSG_WARNING("Vertex without 3 tracks, it has "<< vertex->trackParticleLinks().size() <<" instead");
		}
		else {
		  if(m_removeDuplicate) {
		    if( isContainedIn(vertex, outVtxContainer) ) continue;
		    outVtxContainer.push_back(vertex);
		  }
		}

		TLorentzVector candidate;

		std::set<const xAOD::TrackParticle*> exclusionset;

		for(auto part : vertex->trackParticleLinks()){ //Loop over tracks linked to vertex
			candidate += (*part)->p4();
			exclusionset.insert( *part ); //If it crashes use the direct TP from the vertex
		}
		//No! the above candidate will fail acceptance of isolation() because it's neither a muon nor a TrackParticle

		//Make a dummy TrackParticle, otherwise TrackIsolationTool cannot deal with it
		xAOD::TrackParticle candidate_slyTrack;
		candidate_slyTrack.makePrivateStore();
		candidate_slyTrack.setDefiningParameters(0, 0., candidate.Phi(), candidate.Theta(), 0./*1./candidate.P()*/);
		//The precision goes down a bit, but it's a matter of 10e-7 with our values of interest

		//Make a correctionlist such that the given exclusionset will be removed from the used tracks
		//There is no danger that the input particle will be excluded, as it is not part of inDetTrackContainer
		xAOD::TrackCorrection corrlist;
		corrlist.trackbitset.set(static_cast<unsigned int>(xAOD::Iso::coreTrackPtr));


		string vtxType_name[3] = {"SumPt", "A0", "Z0"};

		xAOD::BPhysHelper vertex_h(vertex); //Use the BPhysHelper to access vertex quantities


		//Loop over refitted primary vertex choice
		for(unsigned int vertex_type = 0 ; vertex_type<= xAOD::BPhysHelper::PV_MIN_Z0 ; vertex_type++ ){

			if((m_vertexType & (1 << vertex_type ) ) == 0)continue; //Stop if the type of vertex is not required


			//if(debug should go outside!!!)

			ATH_MSG_DEBUG("List of cone types" );


					for(unsigned int i =0; i < cones.size(); i++){

						ATH_MSG_DEBUG("cone type = "<< 	xAOD::Iso::toString(xAOD::Iso::IsolationType(cones[i])) );
					//	ATH_MSG_DEBUG("isolation value "<< vtxType_name[vertex_type] << " = "<< result.ptcones[i] );
					//	ATH_MSG_DEBUG("isolation value "<<vtxType_name[vertex_type] <<" = "<< result.ptcones[i] );
					}



			const xAOD::Vertex* refVtx = vertex_h.pv( static_cast<xAOD::BPhysHelper::pv_type>(vertex_type) ); //Fix the cast

			xAOD::TrackIsolation result;

			if(!m_doIsoPerTrk) {
			  m_trackIsoTool->trackIsolation(result, candidate_slyTrack, cones, corrlist, refVtx, &exclusionset, idTrackParticleContainer);


			  //Decorate the vertex with all the isolation values
			  for(unsigned int i =0; i < cones.size(); i++){

				string variableName;

				variableName = xAOD::Iso::toString(xAOD::Iso::IsolationType(cones[i]));
				variableName += vtxType_name[vertex_type];

				SG::AuxElement::Decorator<float> isolation(variableName);
				isolation(*vertex) = result.ptcones[i];

			  }
			}
			else {
			  for(size_t i=0; i<vertex->nTrackParticles(); i++) {
			    m_trackIsoTool->trackIsolation(result, *vertex->trackParticle(i), cones, corrlist, refVtx, &exclusionset, idTrackParticleContainer);

			    for(unsigned int j =0; j < cones.size(); j++) {
			      string variableName;
			      variableName = xAOD::Iso::toString(xAOD::Iso::IsolationType(cones[j]));
			      variableName += vtxType_name[vertex_type] + "_trk" + std::to_string(i+1);
			      SG::AuxElement::Decorator<float> isolation(variableName);
			      isolation(*vertex) = result.ptcones[j];
			    }
			  }
			}
		}
	
      }// End of if passed
    }// end of loop over vertices

    return StatusCode::SUCCESS;
  }

}//End of namespace DerivationFramework

