/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagTools/SVForIPTool.h"

//get the bad track info...
//#include "TrkParticleBase/LinkToTrackParticleBase.h"
//#include "TrkParticleBase/TrackParticleBase.h"

//#include "VxVertex/VxTrackAtVertex.h"
//#include "VxJetVertex/TwoTrackVerticesInJet.h"
//#include "VxSecVertex/VxJetFitterVertexInfo.h"
//#include "VxVertex/VxCandidate.h"
//#include "VxJetVertex/VxJetCandidate.h"
//#include "JetEvent/Jet.h"
//#include "xAODJet/Jet.h" // in header

//#include "VxVertex/RecVertex.h"

#include "GeoPrimitives/GeoPrimitivesHelpers.h"

namespace Analysis {


  SVForIPTool::SVForIPTool(const std::string& name,
                           const std::string& n, const IInterface* p):
    AthAlgTool(name, n,p)
  {
    declareInterface<SVForIPTool>(this);
  }  

  SVForIPTool::~SVForIPTool() {}

  StatusCode SVForIPTool::initialize() {
    
    ATH_MSG_INFO(" Initialization of SVForIPTool succesful");
    return StatusCode::SUCCESS;
  }

  StatusCode SVForIPTool::finalize() {
    
    ATH_MSG_INFO(" Finalization of SVForIPTool succesful");
    return StatusCode::SUCCESS;
  }

  void SVForIPTool::getDirectionFromSecondaryVertexInfo(Amg::Vector3D & SvxDirection,
                                                        bool & canUseSvxDirection,
                                                        const xAOD::Jet & /*jetToTag*/,
                                                        xAOD::BTagging* BTag,
                                                        const std::string & secVxFinderName,
                                                        const xAOD::Vertex & priVtx)
                                                        // const Trk::RecVertex & priVtx)
  {
    std::vector< ElementLink< xAOD::VertexContainer > > myVertices;
    BTag->variable<std::vector<ElementLink<xAOD::VertexContainer> > >(secVxFinderName, "vertices", myVertices);
    // const ISvxAssociation* newSvxAssociation=jetToTag.getAssociation<ISvxAssociation>(secVxFinderName);
    
    if (myVertices.size() == 0) {
      ATH_MSG_DEBUG(" No secondary vertex found for getting the B flight direction (for the IP sign calculation)");
    } else {
      //const Trk::VxSecVertexInfo* myVertexInfo=newSvxAssociation->vertexInfo();
     
      // int n2track = 0;
      // BTag->variable<int>(secVxFinderName, "n2trackvertices", n2track);
      // canUseSvxDirection= n2track>=1?true:false;
      //if (canUseSvxDirection)
      //{
	  if (myVertices[0].isValid())
	    {
	      canUseSvxDirection=true;
	      SvxDirection=(*myVertices[0])->position()-priVtx.position();
	      ATH_MSG_VERBOSE(" Get direction from InDetVKalVertex: phi: " << SvxDirection.phi() << 
			      " theta: " << SvxDirection.theta() );
	    }
	  else 
	    {
	      ATH_MSG_WARNING("SVX info seems usable, but no SVX available !!!");
	    }
	  //}
    }	
    /*      else if (secVxFinderName.find("JetFitter") != std::string::npos) // not yet implemented in BTagging/BTagSecVertexing
	    {
	    Trk::VxJetCandidate* myVxJetCandidate=dynamic_cast<Trk::VxJetCandidate*>(myVertices[0]);
	    if (myVxJetCandidate)
	    {
	    canUseSvxDirection=true;
	    const Trk::RecVertexPositions & recVertexPositions=myVxJetCandidate->getRecVertexPositions();
	    const Amg::VectorX & vertexPosition=recVertexPositions.position();
	    SvxDirection=Amg::Vector3D(1,1,1);
	    Amg::setPhi(SvxDirection, vertexPosition[Trk::jet_phi]);
	    Amg::setTheta(SvxDirection, vertexPosition[Trk::jet_theta]);
	  
	    ATH_MSG_VERBOSE(" Get Jet direction from JetFitter: phi: " << vertexPosition[Trk::jet_phi] << 
	    " theta: " << vertexPosition[Trk::jet_theta]);
	    }
	    }*/
    
  }
  
  
    
  void SVForIPTool::getTrkFromV0FromSecondaryVertexInfo(std::vector<const xAOD::TrackParticle*> & TrkFromV0,
                                                        const xAOD::Jet & /*jetToTag*/,
							xAOD::BTagging* BTag,
                                                        const std::string & secVxFinderName)
  {
    std::vector<ElementLink<xAOD::TrackParticleContainer> > TrkFromV0_ELs;
    //std::vector<const Trk::TrackParticleBase*> TrkBaseFromV0;
    //const ISvxAssociation* newSvxAssociation=jetToTag.getAssociation<ISvxAssociation>(secVxFinderName);
    //std::vector< ElementLink< xAOD::VertexContainer > > myVertices;
    //BTag->variable<std::vector<ElementLink<xAOD::VertexContainer> > >(secVxFinderName, "vertices", myVertices);

    //if (myVertices.size()==0) {
    //ATH_MSG_DEBUG(" No secondary vertex found for getting the V0s from the secondary vertex finder info");
    //} else {
      
      BTag->variable<std::vector<ElementLink<xAOD::TrackParticleContainer> > >(secVxFinderName, "badTracksIP", TrkFromV0_ELs);
      /* // not yet implemented in BTagging/BTagSecVertexing
	 else
	 {
	 const Trk::VxJetFitterVertexInfo* myJetFitterInfo=dynamic_cast<const Trk::VxJetFitterVertexInfo*>(myVertexInfo);
	 if (myJetFitterInfo!=0)
	 {
	 const Trk::TwoTrackVerticesInJet* twoTrackVerticesInJet=myJetFitterInfo->getTwoTrackVerticesInJet();
          
	 if (twoTrackVerticesInJet)
	 {
	 const std::vector<const Trk::TrackParticleBase*> & neutralTracksInJet=twoTrackVerticesInJet->getNeutralTrackOfVertices();
            
	 std::vector<const Trk::TrackParticleBase*>::const_iterator neutralBegin=neutralTracksInJet.begin();
	 std::vector<const Trk::TrackParticleBase*>::const_iterator neutralEnd=neutralTracksInJet.end();
            
	 for (std::vector<const Trk::TrackParticleBase*>::const_iterator neutralIter=neutralBegin;
	 neutralIter!=neutralEnd;++neutralIter)
	 {
	 const Trk::VxCandidate* v0candPtr=(*neutralIter)->reconstructedVertex();
              
	 if (!v0candPtr) {
	 ATH_MSG_WARNING(" V0 candidate from JetFitter is empy. ");
	 continue;
	 }
              
	 const Trk::VxTrackAtVertex* firstTrack((*(v0candPtr->vxTrackAtVertex()))[0]);
	 const Trk::VxTrackAtVertex* secondTrack((*(v0candPtr->vxTrackAtVertex()))[1]);
	      
	 if ((!firstTrack)||(!secondTrack))
	 {
	 ATH_MSG_WARNING(" Empty track. ");
	 continue;
	 }
              
	 //going from Link To TrackParticleBase (quite involved...!)
	 const Trk::ITrackLink* trackLink1=firstTrack->trackOrParticleLink();
	 const Trk::ITrackLink* trackLink2=secondTrack->trackOrParticleLink();
              
	 const Trk::LinkToTrackParticleBase * trkCastedLinkTPBase1=
	 dynamic_cast<const Trk::LinkToTrackParticleBase *>(trackLink1);
	      
	 const Trk::LinkToTrackParticleBase * trkCastedLinkTPBase2=
	 dynamic_cast<const Trk::LinkToTrackParticleBase *>(trackLink2);
	      
	 if ((!trkCastedLinkTPBase1)||(!trkCastedLinkTPBase2))
	 {
	 ATH_MSG_WARNING(" JetFitter has Tracks, not TrackParticles... Rejection of V0s impossible...");
	 continue;
	 }
	      
	 TrkBaseFromV0.push_back(**trkCastedLinkTPBase1);
	 TrkBaseFromV0.push_back(**trkCastedLinkTPBase2);
	 }
	 }
	 }
	 }*/

      //}
  


    for (std::vector<ElementLink<xAOD::TrackParticleContainer> >::iterator itr=TrkFromV0_ELs.begin();itr!=TrkFromV0_ELs.end();itr++)
      {
	if (itr->isValid())
	  {
	    TrkFromV0.push_back((**itr));
	  }
      }
  }
  
}//end namespace

