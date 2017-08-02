/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// VertexCaloIsolation.cxx by Matteo Bedognetti
//
// This code is based on CaloIsolationTool of IsolationTools package
//
// Etcone is determined as a topoCluster-isolation value minus Energy Density (ED) correction and minus the energy depositions of the muons
// Muon's energy deposition is already stored in side the xAOD::Muon objects, but the muon-clusters are used to correct for the fact that they muons may have overlapping clusters
// The muon-clusters are stored as well in connection with the muons themselves
//
// The idea of comparing topoClusters with muon-clusters to decide what part of the muon's deposition is of 
// importance had to be abandoned because topCluster cells are not present in xAOD
//
// It enforces the fact that for muons no core-surface is removed for the energy-density correction (thus the corrections are independent from each other)
//
// "isReliable" flag reports of each isolation value if all particles crossing the cone have been correctly corrected for.
// In the case of 2mu+ 1 track it mirrors the fact that the track does not extrapolate into the cone (as tracks have no muon-cluster from which to determine the core-correction)
//




#include "DerivationFrameworkBPhys/VertexCaloIsolation.h"

#include <vector>
#include <string>
#include "TVector3.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODBPhys/BPhysHelper.h"
#include "xAODBPhys/BPhysHypoHelper.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"

//#include "IsolationTool/CaloIsolationTool.h"
//#include "IsolationTool/CaloIsolationTool.h"

#include "RecoToolInterfaces/ICaloTopoClusterIsolationTool.h"

//#include "IsolationTool/IsolationHelper.h"
//#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"
#include "CaloEvent/CaloCell.h"	//Is used (though shown as auto)
//#include "TrkParameters/TrackParameters.h"
#include "CaloInterface/ICaloNoiseTool.h"
#include "TrkCaloExtension/CaloExtension.h"
//#include "CaloUtils/CaloClusterStoreHelper.h"
//#include "CaloUtils/CaloCellList.h"
//#include "CaloEvent/CaloCellContainer.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "xAODPrimitives/IsolationHelpers.h"
#include "TrackToCalo/CaloCellCollector.h"
#include <set>

//#include "Identifier/Identifier32.h"
using namespace std;
namespace DerivationFramework {

  VertexCaloIsolation::VertexCaloIsolation(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_caloIsoTool("xAOD::CaloIsolationTool/CaloIsolationTool"),
    m_trackContainerName("InDetTrackParticles"),
    m_vertexContainerName("NONE"),
    m_caloClusterContainerName("CaloCalTopoClusters"),
    m_muonContainerName("Muons"),
    m_caloExtTool("Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool"),
    //m_caloNoiseTool(""),
    m_cones(),
    m_applyCaloNoiseCut(false),
    m_sigmaCaloNoiseCut(3.4),
    m_vertexType(7)


   // m_cellCollector("")


//  m_caloExtTool
//  m_caloNoiseTool, m_applyCaloNoiseCut, m_sigmaCaloNoiseCut
//  m_cellCollector

  {
        ATH_MSG_DEBUG("in constructor");
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    
    // Declare tools                      
    declareProperty("CaloIsoTool" , m_caloIsoTool);
    declareProperty("TrackContainer" , m_trackContainerName);
    declareProperty("InputVertexContainer" , m_vertexContainerName);
    declareProperty("CaloClusterContainer" , m_caloClusterContainerName);
    declareProperty("ParticleCaloExtensionTool",       m_caloExtTool);
    declareProperty("MuonContainer",       m_muonContainerName);
    declareProperty("PassFlags"                 , m_passFlags);
    declareProperty("IsolationTypes"                 , m_cones);
    declareProperty("DoVertexTypes"                 , m_vertexType);



  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  
  StatusCode VertexCaloIsolation::initialize()
  {
  
    ATH_MSG_DEBUG("in initialize()");
 
    // retrieve CaloIsolationTool
    CHECK( m_caloIsoTool.retrieve() );
    
    // retrieve CaloIsolationTool
    CHECK( m_caloExtTool.retrieve() );

    //Check that flags were given to tag the correct vertices
    if(m_passFlags.empty()){
        ATH_MSG_WARNING("As no pass-flags are given, no vertices will be decorated");
    }

    // Control the IsolationType sequence
    if(m_cones.empty()){
	    m_cones.push_back(xAOD::Iso::etcone40);
	    m_cones.push_back(xAOD::Iso::etcone30);
	    m_cones.push_back(xAOD::Iso::etcone20);
    }

    if(m_applyCaloNoiseCut){
            ATH_MSG_ERROR("No handle to a caloNoiseTool is kept in this tool, ");
            return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
    
  }
  
  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

  StatusCode VertexCaloIsolation::finalize()
  {
    // everything all right
    return StatusCode::SUCCESS;
  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  
  StatusCode VertexCaloIsolation::addBranches() const {


	  // There is also the "MuonClusterCollection" which may already contain all the muon's clusters

    const xAOD::TrackParticleContainer*    idTrackParticleContainer = NULL;
    const xAOD::VertexContainer*  vertexContainer = NULL;

    Rec::CaloCellCollector  cellCollector;  //To keep private dependence for this package it is used here


    const xAOD::MuonContainer* muons = NULL;


    //Load InDetTrackParticles
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


    const xAOD::CaloClusterContainer*  caloClusterContainer = NULL;
    //  load CaloCalTopoClusters
    if(evtStore()->contains<xAOD::CaloClusterContainer>(m_caloClusterContainerName)) {
        CHECK( evtStore()->retrieve(caloClusterContainer, m_caloClusterContainerName) );
    }
    else{ATH_MSG_ERROR("Failed loading vertex container");
    	return StatusCode::FAILURE;
    }


    //Retrieve muon container
    if(evtStore()->contains<xAOD::MuonContainer>(m_muonContainerName)) {
        CHECK( evtStore()->retrieve(muons, m_muonContainerName) );
    }
    else{ATH_MSG_ERROR("Failed loading muon contianer");
    	return StatusCode::FAILURE;
    }


//-------------------------------------------------

    std::vector<xAOD::Iso::IsolationType> cones; cones.resize(m_cones.size());

//    for(unsigned int cone : m_cones)
//    	cones.push_back(xAOD::Iso::IsolationType(cone));

    for (unsigned int i =0; i< m_cones.size(); i++)
    	cones[i] = xAOD::Iso::IsolationType(m_cones[i]);



    //Loop over vertices
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
    	ATH_MSG_DEBUG("Entered loop over vertices");
    	if(vertex->trackParticleLinks().size() != 3)ATH_MSG_WARNING("Vertex without 3 tracks, it has "<< vertex->trackParticleLinks().size() <<" instead");

		TLorentzVector candidate;

		std::set<const xAOD::TrackParticle*> exclusionset;

		for(auto part : vertex->trackParticleLinks()){ //Loop over tracks linked to vertex
			candidate += (*part)->p4();
			exclusionset.insert( *part ); //If it crashes use the direct TP from the vertex
		}

	    //List of corrections: only the pileup correction is applied within the tool
		xAOD::CaloCorrection corrlist;
		corrlist.calobitset.set(static_cast<unsigned int>(xAOD::Iso::pileupCorrection));



	    std::vector<const xAOD::Muon*> vtxMuons;
	    std::vector<TLorentzVector> extrVtxMuons ;
	    std::vector<const xAOD::CaloCluster*> vtxMuonCluster;

	    std::vector<const xAOD::TrackParticle*> usedVtxTracks;
	    //The information whether we are missing some core-corrections in the final isolation value
	    map<xAOD::Iso::IsolationType, bool> is_reliable;


    	    TLorentzVector muonref;  //Place holder for the extrapolated position
	    //Load the caloclusters of the various muons (which you need to load from here)
	    const xAOD::MuonContainer* muons = 0;
	    CHECK( evtStore()->retrieve( muons, "Muons" ) );
	    for ( auto muon : *muons ) {
	    	//I ask for all information to be fine before filling in an entry (so all containers will have the same -matching- objects)
	    	if(muon->inDetTrackParticleLink().isValid() &&  exclusionset.find(*muon->inDetTrackParticleLink() ) != exclusionset.end() ){
	 	       const xAOD::CaloCluster* clus = muon->cluster();
				   if(clus && extrapolateMuon(muonref, clus)){
					   // have a muon, an extrapolation and a cluster (hurray)
					   vtxMuonCluster.push_back(clus);
					   vtxMuons.push_back(muon);
					   usedVtxTracks.push_back( *muon->inDetTrackParticleLink() );
					   extrVtxMuons.push_back(muonref);
				   }else{
					   //If working with the cluster failed, try extrapolating the track
					   if(extrapolateTrack(muonref, *muon)){ //This does not use the muonic cluster, but uses both its tracks to determine a precise position
						   vtxMuonCluster.push_back(clus); //Note clus can also be NULL (for if it's not in the cone there is no point to fret)
						   vtxMuons.push_back(muon);
					   	   usedVtxTracks.push_back( *muon->inDetTrackParticleLink() );
						   extrVtxMuons.push_back(muonref);
					   }
				   }
	    	}
	    }


	    //What if there was a track and not a muon??
	    //Should be treated like the muon-without-cluster case

	    if(vtxMuonCluster.size()  !=3){  //remember that some of the ctxMuonCluster elements may be NULL
		    ATH_MSG_DEBUG( "Attempt at extrapolating the IDtrack" );

		    //Attempt extrapolating the IDtrack for the missing cases
		    for(const xAOD::TrackParticle* missingTrk : exclusionset){
		    	if(std::find(usedVtxTracks.begin(), usedVtxTracks.end(), missingTrk) == usedVtxTracks.end()){
				if(extrapolateTrack(muonref, *missingTrk)){
					vtxMuonCluster.push_back(NULL); //Null, for we didn't start from a muon
					usedVtxTracks.push_back( missingTrk );
					extrVtxMuons.push_back(muonref);
				}
			}
		    }

		    //If there are still missing ones values cannot be guaranteed to be reliable
	            if(vtxMuonCluster.size()  !=3){
                       	ATH_MSG_DEBUG( "For this vertex there were less than 3 muons found (or extrapolated)" );
		        for(xAOD::Iso::IsolationType isoCone : cones) is_reliable[isoCone] = false;
                    }
	    }
	    else{
		    for(xAOD::Iso::IsolationType isoCone : cones)
		    	is_reliable[isoCone] = true;
	    }


	    // Adapt this loop!

		 for(unsigned int vertex_type = 0 ; vertex_type<= xAOD::BPhysHelper::PV_MIN_Z0 ; vertex_type++ ){

			if((m_vertexType & (1 << vertex_type ) ) == 0)continue; //Stop if the type of vertex is not required

			//This can be in an inside loop

			xAOD::BPhysHelper::pv_type this_type = static_cast<xAOD::BPhysHelper::pv_type>( vertex_type );

			xAOD::TrackParticle candidate_slyTrack;
			makeSlyTrack(candidate_slyTrack, candidate, vertex, this_type);


			xAOD::CaloIsolation result;

			ATH_MSG_DEBUG("Check if the caloclus container has to be given or not... see line from 755 on of CaloIsolationTool");

			bool successful = m_caloIsoTool->caloTopoClusterIsolation(result, candidate_slyTrack, cones, corrlist, caloClusterContainer);
			if( !successful ) {
			  ATH_MSG_DEBUG("Calculation of caloTopoClusterIsolation failed");
				 return StatusCode::FAILURE;
			}

			// Make the extension to the calorimeter, as it is done inside the other tools...
			TLorentzVector extr_candidate;
			if( !extrapolateTrack(extr_candidate, candidate_slyTrack) ){
				ATH_MSG_WARNING("Failure extrapolating the slyTrack "<<"pt="<<candidate_slyTrack.pt()<<" eta="<<candidate_slyTrack.eta()<<" phi="<<candidate_slyTrack.phi());
				ATH_MSG_WARNING("Taking the original coordinates");
			}


				 std::map<xAOD::Iso::IsolationType,float> coreCorrections;

				//See if this is inside the cone, to determine the correct correction ^^
				for(xAOD::Iso::IsolationType isoType : cones){

					double conesize = xAOD::Iso::coneSize(isoType);
					//check what is inside the cone
					std::vector<xAOD::CaloCluster> clustersInCone;

					for(unsigned int j=0; j < vtxMuonCluster.size(); j++){
						auto mucluster = vtxMuonCluster[j];
						// I should use the propagated values here, though the variation is very small, coming from the vertex position
						float dr=extrVtxMuons[j].DeltaR(extr_candidate);



						ATH_MSG_DEBUG("Cone size: "<<conesize<<" dr="<<dr);
						ATH_MSG_DEBUG(extrVtxMuons[j].Eta() <<" - "<<extr_candidate.Eta()<<" and "<<extrVtxMuons[j].Phi() <<" - "<<extr_candidate.Phi());

						if(	dr < conesize ){       //This makes a copy, such that I can remove some cells if needed


							//here do the check for the cluster, if it should go in, then prevent and set the bad for this cone
							if(mucluster != NULL) clustersInCone.push_back( xAOD::CaloCluster(*mucluster) );
							else is_reliable[isoType] = false;



						}
					}

	//			    ATH_MSG_DEBUG("Muon clusters in cone "<<xAOD::Iso::toString(isoType)<<"  "<< clustersInCone.size());
	//			    if( msgLvl(MSG::DEBUG) ){
	//					for(auto muon : vtxMuons)
	//						if(muon->isAvailable<float>("ET_Core"))	ATH_MSG_DEBUG("ET_core stored inside: "<< muon->auxdataConst<float>("ET_Core") );
	//			    }
					//remove eventually doubles in cells
					if(clustersInCone.size() == 2){
						for(auto cell : clustersInCone[0]){
							clustersInCone[1].removeCell(cell);
						}
					}
					if(clustersInCone.size() == 3){
						for(auto cell : clustersInCone[0]){
							clustersInCone[1].removeCell(cell);
							clustersInCone[2].removeCell(cell);
						}
						for(auto cell : clustersInCone[1]){
							clustersInCone[2].removeCell(cell);
						}
					}

					//Calculate the core-correction
					std::vector<float> etcore(4, 0.);
					float coreCorr=0.;
					for(auto cl : clustersInCone){
						if(cl.size() != 0){	//Maybe two muons have a full cluster overlap??
							ATH_MSG_DEBUG("Cells in this cluster: "<< cl.size());
                                                         
							cellCollector.collectEtCore( cl, etcore, ToolHandle <ICaloNoiseTool> ("")/*m_caloNoiseTool*/, m_applyCaloNoiseCut, m_sigmaCaloNoiseCut );  //Note an empty handle to ICaloNoiseTool is passed
							coreCorr += etcore[Rec::CaloCellCollector::ET_Core];
							ATH_MSG_DEBUG("Their core-energy: "<< etcore[Rec::CaloCellCollector::ET_Core]);

						}
					}

					//Store the core-correction
					coreCorrections[isoType] = coreCorr;

				}

				//For a pion I have no such cone energy, do I? But then I should also see what the original vertex was
				//If something is not a muon there is no way the calocluster was stored, I think
				//Would need further study


			//Collect all the required information
			string ED("_EDcorr");
			string core("_COREcorr");
			string reliable("_isReliable");


			string vtx_type[3] = {"SumPt", "A0", "Z0"};

			string vtx = vtx_type[ vertex_type ];

			ATH_MSG_DEBUG("Detailed: ");
			for(unsigned int i=0; i< cones.size(); i++){
				xAOD::Iso::IsolationType isoType = cones[i];
				result.etcones[i] -= coreCorrections[isoType];  //Finish correcting the energy


	//	    	if(fabs(result.etcones[i]) < 0.1){
	//
	//	    	ATH_MSG_INFO("Isolation: "<<xAOD::Iso::toString(isoType) ); // The name of the isolation
	//	    	ATH_MSG_ERROR(result.etcones[i]<<" + "<<(result.noncoreCorrections[xAOD::Iso::pileupCorrection])[i]<<" + "<<coreCorrections[isoType] );
	//	    	}

				//Here do the decoration (store all, and as well if three muons are found)


				string variableName = xAOD::Iso::toString(isoType) + vtx; //I corrected for the closest vertex in A0
				SG::AuxElement::Decorator<float> isolation(variableName);
				isolation(*vertex) = result.etcones[i];

				isolation = SG::AuxElement::Decorator<float>(variableName + ED);
				isolation(*vertex) = (result.noncoreCorrections[xAOD::Iso::pileupCorrection])[i];

				isolation = SG::AuxElement::Decorator<float>(variableName + core);
				isolation(*vertex) = coreCorrections[isoType];

				//This variable contains the info whether 3 caloclusters have been found in the muons
				//Future would be to see if their extrapolations are of interest anyhow (if not missing them is no issue)
				//Fore some reason these seem to become chars (instead of bools) in the output
				SG::AuxElement::Decorator<bool> reliability(variableName + reliable);
				reliability(*vertex) = is_reliable[isoType];

			}
		 } //Loop over primaryVertex choice

	    //Decorate the candidate with the new information


//		 return StatusCode::SUCCESS;
//
//	    ///////////////////////////////////

      }

//END OF NEW PART
    }//End of loop over vertices
    return StatusCode::SUCCESS;
  }

  //Note that the full version had a different method for muons!!!! Maybe I should use that one instead!

  //This is almost a perfect copy of CaloIsolationTool::GetExtrapEtaPhi, but only for the part relative to tracks
  bool VertexCaloIsolation::extrapolateTrack(TLorentzVector& extr_tp, const xAOD::IParticle& tp) const{

	    extr_tp = tp.p4(); //Pre-set the output TLorentzVector to the input's 4-momentum

		const Trk::CaloExtension* caloExtension = 0;
		if(!m_caloExtTool->caloExtension(tp,caloExtension)){
		  ATH_MSG_WARNING("Can not get caloExtension.");
		  return false;
		}

		const std::vector<const Trk::CurvilinearParameters*>& intersections = caloExtension->caloLayerIntersections();
		if (intersections.size()>0) {
			Amg::Vector3D avePoint(0,0,0);
			for (unsigned int i = 0; i < intersections.size(); ++i){
			  const Amg::Vector3D& point = intersections[i]->position();
			  avePoint += point;
			}
			avePoint = (1./intersections.size())*avePoint;


			extr_tp.SetPtEtaPhiE(1., avePoint.eta(), avePoint.phi(), 10.); //Using the three-vector constructor
			//eta = avePoint.eta();
			//phi = avePoint.phi();
			ATH_MSG_DEBUG("Successfully extrapolated candidate eta/phi : "<<tp.eta()<<"/"<<tp.phi()<<" --> "<< extr_tp.Eta()<<"/"<<extr_tp.Phi());

		  }
		else{	//This is very unlikely, it happens if a few cases in MC
			ATH_MSG_WARNING("Candidate extrapolation failed. Keeping track's eta/phi values");
			return false;

		}

	  return true;


  }

  //Version for the muons
  bool VertexCaloIsolation::extrapolateMuon(TLorentzVector& extr_tp, const xAOD::CaloCluster* cluster) const
  {
      //auto cluster = mu->cluster(); //done outside
      if(cluster){
        float etaT = 0, phiT = 0;
        int nSample = 0;
        for(unsigned int i=0; i<CaloSampling::Unknown; i++) // dangerous?
        {
          auto s = static_cast<CaloSampling::CaloSample>(i);
          if(!cluster->hasSampling(s)) continue;
          //ATH_MSG_DEBUG("Sampling: " << i << "eta-phi (" << cluster->etaSample(s) << ", " << cluster->phiSample(s) << ")");
          etaT += cluster->etaSample(s);
          phiT += cluster->phiSample(s);
          nSample++;
        }
        if(nSample>0){

            extr_tp.SetPtEtaPhiE(1., etaT/nSample, phiT/nSample, 10.); //Using the three-vector constructor
            return true ;

        }else{
          ATH_MSG_WARNING("Muon calo cluster is empty????");
          return false;
        }
      }else{
        ATH_MSG_WARNING("Muon calo cluster not found. Calo extension can not be obtained!!!");
        return false;
      }
   }

  //Make a sly track to be fed to the CaloIsolationTool
  xAOD::TrackParticle&  VertexCaloIsolation::makeSlyTrack(xAOD::TrackParticle& candidate_slyTrack, const TLorentzVector& candidate, const xAOD::Vertex* vertex, xAOD::BPhysHelper::pv_type vertexType) const {

		candidate_slyTrack.makePrivateStore();
		candidate_slyTrack.setDefiningParameters(0, 0., candidate.Phi(), candidate.Theta(), 0. );  // avoided q/p = 1./candidate.P()

		//I should set the correct d0 and z0, while setting momentum to enormous, to obtain a straight line
		//I fear that q/p == 0 might cause some divide by 0, though.

		//Somewhere this information will be checked, so I need to provide it
		SG::AuxElement::Decorator<uint8_t> hypothesis("particleHypothesis");
		hypothesis(candidate_slyTrack) = xAOD::undefined;  //Value 99 as none of the common types (muon, pion, kaon, etc.)
		SG::AuxElement::Decorator<std::vector<float> > covmat( "definingParametersCovMatrix" );
		covmat(candidate_slyTrack) = std::vector<float>(25, 0.); // I am saying that there are no errors on my parameters
		//The precision goes down a bit, but it's a matter of 10e-7 with our values of interest

		xAOD::BPhysHelper vertex_h(vertex); //Use the BPhysHelper to access vertex quantities

		SG::AuxElement::Decorator<float> vx( "vx" );
		vx(candidate_slyTrack) = vertex_h.pv(vertexType)->x();

		SG::AuxElement::Decorator<float> vy( "vy" );
		vy(candidate_slyTrack) = vertex_h.pv(vertexType)->y();

		SG::AuxElement::Decorator<float> vz( "vz" );
		vz(candidate_slyTrack) = vertex_h.pv(vertexType)->z();
		//The precision goes down a bit, but it's a matter of 10e-7 with our values of interest

	    return candidate_slyTrack;


  }




}//End of namespace DerivationFramework


