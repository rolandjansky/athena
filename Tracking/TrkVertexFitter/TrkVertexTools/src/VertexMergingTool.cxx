/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVertexTools/VertexMergingTool.h"
#include "TrkVertexFitterInterfaces/IVertexWeightCalculator.h" 
#include "VxVertex/VxTrackAtVertex.h"
#include <vector> 

namespace Trk{
 
   //constructor
  VertexMergingTool::VertexMergingTool ( const std::string& t, const std::string& n, const IInterface*  p )
          : AthAlgTool ( t,n,p ),
	    m_iVertexFitter("Trk::AdaptiveVertexFitter"),
            m_useBeamConstraint(false)
  {
    declareInterface<IVertexMergingTool> ( this );
    declareProperty("VertexFitterTool", m_iVertexFitter);
    declareProperty("useBeamConstraint",m_useBeamConstraint);
  }
  
  //destructor
  VertexMergingTool::~VertexMergingTool(){}

//initialize
  StatusCode VertexMergingTool::initialize()
  {
    
    if ( m_iVertexFitter.retrieve().isFailure() ) {
      msg(MSG::ERROR) << "Failed to retrieve tool " << m_iVertexFitter << endmsg;
      return StatusCode::FAILURE;
    } 
    
     ATH_CHECK(m_beamSpotKey.initialize());
    
     ATH_MSG_DEBUG("Re-merging tool initialization successful");
     return StatusCode::SUCCESS;
   }///EndOfInitialize


  std::pair<xAOD::VertexContainer*,xAOD::VertexAuxContainer*> VertexMergingTool::mergeVertexContainer(const xAOD::VertexContainer& MyVxCont) const
  {

    ATH_MSG_DEBUG("Run vertex remerging");

    //if beamspot constraint was requested, get it now
    xAOD::Vertex theconstraint;
    if (m_useBeamConstraint) {
      SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };
      if(not beamSpotHandle.isValid()) ATH_MSG_ERROR("Cannot Retrieve " << m_beamSpotKey.key() );
      theconstraint = xAOD::Vertex(); // Default constructor creates a private store
      theconstraint.setPosition( beamSpotHandle->beamVtx().position() );
      theconstraint.setCovariancePosition( beamSpotHandle->beamVtx().covariancePosition() );
      theconstraint.setFitQuality( beamSpotHandle->beamVtx().fitQuality().chiSquared(), beamSpotHandle->beamVtx().fitQuality().doubleNumberDoF() );
    }

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
              xAOD::Vertex * mergedVtx = nullptr;
              if(m_useBeamConstraint) {
                mergedVtx = m_iVertexFitter->fit( combinedTracks, theconstraint );
              } else { 
                //no interface for no constraint and no starting point, so use starting point of original vertex
                const Amg::Vector3D& start( vx->position() );
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

  bool VertexMergingTool::checkCompatibility( const xAOD::Vertex * vx1, const xAOD::Vertex * vx2 ) const {

    double z1 = vx1->z();
    double z2 = vx2->z();

    double err2_z1 = vx1->covariancePosition()(Trk::z, Trk::z);
    double err2_z2 = vx2->covariancePosition()(Trk::z, Trk::z);

    double sigmaZ = (fabs(z1-z2))/(sqrt(err2_z1+err2_z2));

    ATH_MSG_DEBUG("z1 = " << z1 << ", z2 = " << z2 << ", error = " << sqrt( err2_z1+err2_z2 ) );

    return sigmaZ<3;
  }

}///End trk namespace  
