/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVertexTools/VertexMergingTool.h"
#include "TrkVertexFitterInterfaces/IVertexWeightCalculator.h" 
#include "VxVertex/VxTrackAtVertex.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxContainer.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include <vector> 

namespace Trk{
 
   //constructor
  VertexMergingTool::VertexMergingTool ( const std::string& t, const std::string& n, const IInterface*  p )
          : AthAlgTool ( t,n,p ),
	    m_iBeamCondSvc("BeamCondSvc",n),
	    m_iVertexFitter("Trk::AdaptiveVertexFitter"),
            m_useBeamConstraint(false)
  {
    declareInterface<IVertexMergingTool> ( this );
    declareProperty("BeamPositionSvc", m_iBeamCondSvc);
    declareProperty("VertexFitterTool", m_iVertexFitter);
    declareProperty("useBeamConstraint",m_useBeamConstraint);
  }
  
  //destructor
  VertexMergingTool::~VertexMergingTool(){}

//initialize
  StatusCode VertexMergingTool::initialize()
  {
    
    if ( m_iVertexFitter.retrieve().isFailure() ) {
      msg(MSG::ERROR) << "Failed to retrieve tool " << m_iVertexFitter << endreq;
      return StatusCode::FAILURE;
    } 
    
    if (m_iBeamCondSvc.retrieve().isFailure())
    {
      msg(MSG::ERROR) << "Could not find BeamCondSvc." << endreq;
      return StatusCode::FAILURE;
    }
    
     ATH_MSG_DEBUG("Re-merging tool initialization successful");
     return StatusCode::SUCCESS;
   }///EndOfInitialize

   StatusCode VertexMergingTool::finalize()
   {
     return StatusCode::SUCCESS;
   }

  std::pair<xAOD::VertexContainer*,xAOD::VertexAuxContainer*> VertexMergingTool::mergeVertexContainer(const xAOD::VertexContainer& MyVxCont) {

    ATH_MSG_DEBUG("Run vertex remerging");

    //if beamspot constraint was requested, get it now
    Trk::RecVertex theconstraint;
    if (m_useBeamConstraint) theconstraint=m_iBeamCondSvc->beamVtx();

    //new output containers to be filled
    xAOD::VertexContainer *NewContainer = new xAOD::VertexContainer();
    xAOD::VertexAuxContainer* auxNewContainer = new xAOD::VertexAuxContainer();
    NewContainer->setStore( auxNewContainer );
    
    //add remerged flags to all
    std::vector<bool> remerged( MyVxCont.size(), false );    

    xAOD::VertexContainer::const_iterator beginIter = MyVxCont.begin();
    xAOD::VertexContainer::const_iterator endIter = MyVxCont.end();
    unsigned int Ni=0;
    for(xAOD::VertexContainer::const_iterator i = beginIter; i!=endIter; ++i) {
      xAOD::Vertex * vx = new xAOD::Vertex( **i );

      if( vx->vertexType() == xAOD::VxType::NoVtx ) { //dummy vertex -- just add a copy
        NewContainer->push_back( vx );
      } else if( !remerged[Ni] ) { //skip vertices already merged into another

        unsigned int Nj = Ni+1;
        for(xAOD::VertexContainer::const_iterator j=i+1; j!=endIter; ++j ) {
          const xAOD::Vertex * mergeCand = (*j);
          if( mergeCand->vertexType() != xAOD::VxType::NoVtx && !remerged[Nj] ) {
            //not dummy and not already merged into earlier vertex, so consider it as merging candidate
            if( checkCompatibility( vx, mergeCand ) ) {

              //get all the track particles to fit
              std::vector<const xAOD::TrackParticle*> combinedTracks;
              for(size_t t=0; t<vx->nTrackParticles(); ++t) {
                combinedTracks.push_back( vx->trackParticle(t) );
              }
              for(size_t t=0; t<mergeCand->nTrackParticles(); ++t) {
                combinedTracks.push_back( mergeCand->trackParticle(t) );
              }

              //call the fitter -> using xAOD::TrackParticle it should set the track links for us
              xAOD::Vertex * mergedVtx = 0;
              if(m_useBeamConstraint) {
                mergedVtx = m_iVertexFitter->fit( combinedTracks, theconstraint );
              } else { 
                //no interface for no constraint and no starting point, so use starting point of original vertex
                Trk::Vertex start( vx->position() );
                mergedVtx = m_iVertexFitter->fit( combinedTracks, start );
              }

              //CHECK MERGING IS GOOD? 
              // flag as remerged
              ATH_MSG_DEBUG("Merge vertices " << Ni << " and " << Nj);
              remerged[Nj] = true;
              remerged[Ni] = true;
              //delete copy of first vertex and then overwrite with merged vertex
              delete vx;
              vx = mergedVtx;
            }            
          }

          Nj++;
        } //loop over j

        //whether we merged or not, can add vx to the container
        NewContainer->push_back( vx );

      } //end if vtx[Ni] is not remerged
      Ni++;
    }

    return std::make_pair( NewContainer, auxNewContainer );
 
  }


  VxContainer* VertexMergingTool::mergeVxContainer( const VxContainer& MyVxCont )
  { 
    ATH_MSG_DEBUG("in re-merging tool");

    //if beamspot constraint was requested, get it now
    Trk::RecVertex theconstraint;
    if (m_useBeamConstraint) theconstraint=m_iBeamCondSvc->beamVtx();
    
    //new container that will be filled and returned
    VxContainer *NewContainer = new VxContainer(SG::OWN_ELEMENTS);
    
    //add remerged flags to all but dummy (last vertex)
    std::vector<bool> remerged;    
    for (size_t i = 0 ; i < MyVxCont.size()-1; i++) { 
    	remerged.push_back(false);
    }

    int Ni=0;
    VxContainer::const_iterator beginIter = MyVxCont.begin();
    VxContainer::const_iterator endIter = MyVxCont.end();
   
    ATH_MSG_DEBUG("input size "<<MyVxCont.size());
    for(VxContainer::const_iterator i = beginIter; i<endIter; i++){ 
      Trk::VxCandidate* vxCand = (*i)->clone();
      //skip vertices that were already merged into another
      if(remerged[Ni] == false){
	int Nj=Ni+1;
	for(VxContainer::const_iterator j = i+1; j<endIter; j++){ 
	  if( vxCand->vertexType() != Trk::NoVtx && (*j)->vertexType() != Trk::NoVtx){ // not merge dummy vertices
            if(remerged[Nj] == false){ // don't take vertices already re-merged

              //check if they should be merged
              if( checkCompatibility( vxCand, *j ) ) {

                //get all the perigees from both vertices
                std::vector<const Trk::TrackParameters*> perigeesToFit;
                
                std::vector<Trk::VxTrackAtVertex*>* myVxTracksAtVtx = vxCand->vxTrackAtVertex();
                if(myVxTracksAtVtx) { 
                  std::vector<Trk::VxTrackAtVertex*>::iterator tracksBegin=myVxTracksAtVtx->begin();
                  std::vector<Trk::VxTrackAtVertex*>::iterator tracksEnd=myVxTracksAtVtx->end();
                  for (std::vector<Trk::VxTrackAtVertex*>::iterator tracksIter=tracksBegin; tracksIter!=tracksEnd;++tracksIter) {
                    perigeesToFit.push_back( (*tracksIter)->initialPerigee() );
                  }	
                }
                std::vector<Trk::VxTrackAtVertex*>* myVxTracksAtVtx2=(*j)->vxTrackAtVertex();
                if(myVxTracksAtVtx2) { 
                  std::vector<Trk::VxTrackAtVertex*>::iterator tracksBegin=myVxTracksAtVtx2->begin();
                  std::vector<Trk::VxTrackAtVertex*>::iterator tracksEnd=myVxTracksAtVtx2->end();
                  for (std::vector<Trk::VxTrackAtVertex*>::iterator tracksIter=tracksBegin; tracksIter!=tracksEnd;++tracksIter) {
                    perigeesToFit.push_back( (*tracksIter)->initialPerigee() );
	  				
                  }
                }

                //call the fitter
                Trk::VxCandidate * myVxCandidateNEW = 0;
                if(m_useBeamConstraint)
                  myVxCandidateNEW = m_iVertexFitter->fit( perigeesToFit,theconstraint);
                else 
                  myVxCandidateNEW = m_iVertexFitter->fit( perigeesToFit );

                ATH_MSG_DEBUG("i="<<Ni<<": "<<myVxTracksAtVtx->size());
                ATH_MSG_DEBUG("j="<<Nj<<": "<<myVxTracksAtVtx2->size());

                // check if the fit has converged and do not merge if this is not the case
                //check re-merging quality: TO BE IMPLEMENTED

                double chi2_i = (vxCand->recVertex().fitQuality().chiSquared())/(vxCand->recVertex().fitQuality().doubleNumberDoF());
                double chi2_j = ((*j)->recVertex().fitQuality().chiSquared())/((*j)->recVertex().fitQuality().doubleNumberDoF());
                double chi2_m = (myVxCandidateNEW->recVertex().fitQuality().chiSquared())/(myVxCandidateNEW->recVertex().fitQuality().doubleNumberDoF());
                ATH_MSG_DEBUG("chi2: i="<<chi2_i<<", chi2: j="<<chi2_j<<"re-merged: "<<chi2_m);

                //get the tracks at the new vertex -- these don't have links yet                
                std::vector<Trk::VxTrackAtVertex*>* myVxTracksAtVtx_tot = myVxCandidateNEW->vxTrackAtVertex();
                for( std::vector<Trk::VxTrackAtVertex*>::iterator totit = myVxTracksAtVtx_tot->begin(); totit!= myVxTracksAtVtx_tot->end(); ++totit) {
                  
                  bool found = false;
                  for( std::vector<Trk::VxTrackAtVertex*>::iterator vx1_it = myVxTracksAtVtx->begin(); vx1_it!=myVxTracksAtVtx->end(); ++vx1_it ) {
                    if( (*totit) == (*vx1_it ) ) {
                      (*totit)->setOrigTrack( (*vx1_it)->trackOrParticleLink() );
                      found = true;
                    }
                  }
                  if(found) continue;
                  for( std::vector<Trk::VxTrackAtVertex*>::iterator vx2_it = myVxTracksAtVtx2->begin(); vx2_it!=myVxTracksAtVtx2->end(); ++vx2_it ) {
                    if( (*totit) == (*vx2_it ) ) {
                      (*totit)->setOrigTrack( (*vx2_it)->trackOrParticleLink() );
                      found = true;
                    }
                  }
                  
                } //end adding links loop
                
				
                // flag as remerged
                remerged[Nj] = true;
                remerged[Ni] = true;
                //delete copy of first vertex and then overwrite with merged vertex
                delete vxCand;
                vxCand = myVxCandidateNEW;
                ATH_MSG_DEBUG("after merging: "<<(vxCand->recVertex().fitQuality().chiSquared())/(vxCand->recVertex().fitQuality().doubleNumberDoF()));
              }
            }
          }
          Nj++;
        } 

        //add to the new container -- vxCand == vertex[Ni] if no merging, or == last myVxCandidateNEW if merging was done
        if(vxCand ->vertexType() != Trk::NoVtx){ 
          NewContainer->push_back(vxCand);
        }
      }
      // push back the dummy vertex: TO BE CHANGED!! need to re-create it if the primary vertex is merged
      if(vxCand->vertexType() == Trk::NoVtx){ 
        NewContainer->push_back(vxCand);
      }
      Ni++;
		
    }

     return NewContainer;
   }

  bool VertexMergingTool::checkCompatibility( const Trk::VxCandidate * vx1, const Trk::VxCandidate * vx2 ) {


    double zi = vx1->recVertex().position().z();
    double zj = vx2->recVertex().position().z();
    double err_zi= vx1->recVertex().covariancePosition()(Trk::z,Trk::z);
    double err_zj= vx2->recVertex().covariancePosition()(Trk::z,Trk::z);
    double sigmaZ = (fabs(zi-zj))/(sqrt(err_zi+err_zj));
    
    if(sigmaZ<3){
      return true;
    } else { 
      return false;
    }

  }

  bool VertexMergingTool::checkCompatibility( const xAOD::Vertex * vx1, const xAOD::Vertex * vx2 ) {

    double z1 = vx1->z();
    double z2 = vx2->z();

    double err2_z1 = vx1->covariancePosition()(Trk::z, Trk::z);
    double err2_z2 = vx2->covariancePosition()(Trk::z, Trk::z);

    double sigmaZ = (fabs(z1-z2))/(sqrt(err2_z1+err2_z2));

    ATH_MSG_DEBUG("z1 = " << z1 << ", z2 = " << z2 << ", error = " << sqrt( err2_z1+err2_z2 ) );

    if(sigmaZ<3){
      return true;
    } else { 
      return false;
    }
  }

}///End trk namespace  
