/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
//Author: Lianyou Shan <lianyou.shan@cern.ch>

#include "TrkVertexTools/SecVertexMergingTool.h"
#include "TrkVertexFitterInterfaces/IVertexWeightCalculator.h" 
#include "VxVertex/VxTrackAtVertex.h"
#include <vector> 

namespace Trk{
 
   //constructor
  SecVertexMergingTool::SecVertexMergingTool ( const std::string& t, const std::string& n, const IInterface*  p )
          : AthAlgTool ( t,n,p ),
            m_Compatidime(1),
            m_minDist(3),
	    m_iVertexFitter("Trk::AdaptiveVertexFitter", this )
  {
    declareInterface<IVertexMergingTool> ( this );
    declareProperty("VertexFitterTool", m_iVertexFitter);
    declareProperty("CompatibilityDimension", m_Compatidime, "0 for z0, 1 for d0, 2 for all" ) ;
    declareProperty("MininumDistance", m_minDist, "in sigma" ) ;
  }
  
  //destructor
  SecVertexMergingTool::~SecVertexMergingTool(){}

//initialize
  StatusCode SecVertexMergingTool::initialize()
  {
    
    if ( m_iVertexFitter.retrieve().isFailure() ) {
      ATH_MSG_ERROR("Failed to retrieve tool " << m_iVertexFitter);
      return StatusCode::FAILURE;
    } 
    
    ATH_MSG_DEBUG("Re-merging tool initialization successful");
    return StatusCode::SUCCESS;
   }///EndOfInitialize

   StatusCode SecVertexMergingTool::finalize()
   {
     return StatusCode::SUCCESS;
   }

  std::pair<xAOD::VertexContainer*,xAOD::VertexAuxContainer*> SecVertexMergingTool::mergeVertexContainer(const xAOD::VertexContainer& MyVxCont) {

    ATH_MSG_DEBUG("Run vertex remerging");

    //new output containers to be filled
    xAOD::VertexContainer *NewContainer = new xAOD::VertexContainer();
    xAOD::VertexAuxContainer* auxNewContainer = new xAOD::VertexAuxContainer();
    NewContainer->setStore( auxNewContainer );
    
    bool moreDeco = ( MyVxCont.front() )->isAvailable<float>( "MomentaDirection" ) ;

    if ( ! moreDeco ) ATH_MSG_DEBUG( "Missing decoration !!! " ) ;

/**
    SG::AuxElement::Decorator<std::vector<float> > mDecor_trkWght( "trkWeight" ) ;
    SG::AuxElement::Decorator<float> mDecor_trkDOE( "trkDistOverError" ) ;
    SG::AuxElement::Decorator<float> mDecor_direction( "MomentaDirection" );
    SG::AuxElement::Decorator< float > mDecor_HitsFilter( "radiiPattern" );
**/

    SG::AuxElement::Decorator<float> mDecor_sumPt2( "sumPt2" );
    SG::AuxElement::Decorator<float> mDecor_mass( "mass" );
    SG::AuxElement::Decorator<float> mDecor_energy( "ee" );
    SG::AuxElement::Decorator<int> mDecor_nrobbed( "nrobbed" );
    SG::AuxElement::Decorator<int> mDecor_intrk( "NumInputTrk" );

    //add remerged flags to all
    std::vector<bool> remerged( MyVxCont.size(), false );    

    xAOD::VertexContainer::const_iterator beginIter = MyVxCont.begin();
    xAOD::VertexContainer::const_iterator endIter = MyVxCont.end();
    unsigned int Ni=0;
    for(xAOD::VertexContainer::const_iterator i = beginIter; i!=endIter; ++i, Ni++ ) {
      xAOD::Vertex * vx = new xAOD::Vertex( **i );

      if( remerged[Ni] ) continue ;  //skip vertices already merged into another

      std::vector<const xAOD::TrackParticle*> combinedTracks;
      std::vector< ElementLink< xAOD::TrackParticleContainer > > tpLinks1 = vx->trackParticleLinks();
      if ( tpLinks1.size() >= 1 )
      {
        for(const auto & tp_EL : tpLinks1 )
        {
          const xAOD::TrackParticle* trk = *tp_EL ;
          combinedTracks.push_back( trk ) ;
        }

        unsigned int Nj = Ni+1;
        bool newmerge = false ;
        for(xAOD::VertexContainer::const_iterator j=i+1; j!=endIter; ++j, Nj++ ) 
        {
          const xAOD::Vertex * mergeCand = (*j);
          if ( remerged[Nj] ) continue ;

          if ( newmerge )    
          {
            combinedTracks.clear() ;
            tpLinks1 = vx->trackParticleLinks();
            if ( tpLinks1.size() < 1 ) break ;
            for(const auto & tp_EL : tpLinks1 )
            {
              const xAOD::TrackParticle* trk = *tp_EL ;
              combinedTracks.push_back( trk ) ;
            }
            newmerge = false ;
          }

        //not dummy and not already merged into earlier vertex, so consider it as merging candidate
          if( ! checkCompatibility( vx, mergeCand ) ) continue ;

          ATH_MSG_DEBUG("To merge vertices " << Ni << " and " << Nj);
              //get all the track particles to fit

          const std::vector< ElementLink< xAOD::TrackParticleContainer > >
                  tpLinks2 = mergeCand->trackParticleLinks();
          if ( tpLinks2.size() < 1 ) continue ;

          for(const auto & tp_EL : tpLinks2 )
          {
            const xAOD::TrackParticle* trk = *tp_EL ;
            combinedTracks.push_back( trk ) ;
          }

          ATH_MSG_DEBUG("Tracks input : " << tpLinks1.size() << " + " << tpLinks2.size() );

          //call the fitter -> using xAOD::TrackParticle it should set the track links for us
          xAOD::Vertex * mergedVtx = 0;
          //no interface for no constraint and no starting point, so use starting point of original vertex
          Amg::Vector3D start( 0.5*( vx->position() + mergeCand->position() ));
          mergedVtx = m_iVertexFitter->fit( combinedTracks, start );

          ATH_MSG_DEBUG("Merged vertices " << mergedVtx->nTrackParticles() );

          remerged[Nj] = true;
          remerged[Ni] = true;
          newmerge = true ;

 // update the decors 
          float pt1 = sqrt( vx->auxdata<float>( "sumPt2" ) );
          float pt2 = sqrt( mergeCand->auxdata<float>( "sumPt2" ) ) ;
          float ntrk1 = 1.0*( ( vx->trackParticleLinks() ).size() ) ;
          float ntrk2 = 1.0*( ( mergeCand->trackParticleLinks() ).size() ) ;
          float wght1 = 0.6*pt1/( pt1 + pt2 ) + 0.4*ntrk1/( ntrk1 + ntrk2 ) ;
          float wght2 = 0.6*pt2/( pt1 + pt2 ) + 0.4*ntrk2/( ntrk1 + ntrk2 ) ;

          xAOD::VxType::VertexType typ1 = vx->vertexType() ;
          xAOD::VxType::VertexType typ2 = mergeCand->vertexType() ;
          float mas1 = vx->auxdata<float>( "mass" ) ;
          float mas2 = mergeCand->auxdata<float>( "mass" ) ;
          float e1 = vx->auxdata<float>( "ee" ) ;
          float e2 = mergeCand->auxdata<float>( "ee" ) ;
          int inNtrk1 = vx->auxdata<int>( "NumInputTrk" ) ;
          int inNtrk2 = mergeCand->auxdata<int>( "NumInputTrk" ) ;

          int ntrks = 0 ;
          float md1=0., md2=0., hf1=0., hf2=0. ;
          std::vector<float> trkW1, trkW2, doe1, doe2 ;
          if ( moreDeco ) 
          {
            doe1 = vx->auxdataConst<std::vector<float>> ( "trkDistOverError") ;
            doe2 = mergeCand->auxdataConst<std::vector<float>> ( "trkDistOverError") ;
            doe2.insert( doe2.end(), doe1.begin(), doe1.end() ) ;
            md1 = vx->auxdata<float>( "MomentaDirection" );
            md2 = mergeCand->auxdata<float>( "MomentaDirection" );
            hf1 = vx->auxdataConst<float>( "radiiPattern" );
            hf2 = mergeCand->auxdataConst<float>( "radiiPattern" );
            trkW1 = vx->auxdata< std::vector<float> >( "trkWeight" ) ;
            trkW2 = mergeCand->auxdata< std::vector<float> >( "trkWeight" ) ;
            trkW2.insert( trkW2.end(), trkW1.begin(), trkW1.end() ) ;
            ntrks = vx->auxdata<int>( "NumTrkAtVtx" ) + mergeCand->auxdata<int>( "NumTrkAtVtx" ) ;
          }

              //delete copy of first vertex and then overwrite with merged vertex
          delete vx;
          vx = mergedVtx;

          if ( wght1 >= wght2 ) 
            vx->setVertexType( typ1 ) ;
          else 
            vx->setVertexType( typ2 ) ;

          if ( moreDeco )
          { 
            vx->auxdata<std::vector<float> >( "trkDistOverError" ) = doe2 ;
            vx->auxdata<float>( "MomentaDirection" ) = wght1*md1 + wght2*md2 ;
            vx->auxdata<float>( "radiiPattern" ) = wght1*hf1 + wght2*hf2 ;
            vx->auxdata< std::vector<float> >( "trkWeight" ) = trkW2 ;
            vx->auxdata< int >( "NumTrkAtVtx" ) = ntrks ;
          }

          mDecor_sumPt2( *vx ) = pt1*pt1 + pt2*pt2 ;
          mDecor_mass( *vx ) =  wght1*mas1 + wght2*mas2 ;
          mDecor_energy( *vx ) =  wght1*e1 + wght2*e2 ;
          mDecor_nrobbed( *vx ) = 0 ;
          mDecor_intrk( *vx ) = (int)(wght1*inNtrk1 + wght1*inNtrk2)  ;


        } //loop over j
      }  // if vx found partner in compatibility

      ATH_MSG_DEBUG( "Merged sumPt2 " << vx->auxdataConst<float>( "sumPt2" ) ) ;

        //whether we merged or not, can add vx to the container
      if ( vx != NULL ) NewContainer->push_back( vx );

    }

    return std::make_pair( NewContainer, auxNewContainer );
 
  }

  bool SecVertexMergingTool::checkCompatibility( const xAOD::Vertex * v1, const xAOD::Vertex * v2 ) 
  {

    float sigma = 100 ;

    Amg::Vector3D vdif = v1->position() - v2->position() ;
    AmgSymMatrix(3) vErrs = v1->covariancePosition() + v2->covariancePosition() ;
    vErrs = vErrs.inverse().eval(); 

    if ( m_Compatidime == 2 )  //  3 dimension
    {
      sigma = sqrt( vdif.dot( vErrs * vdif ) ) ;
    } else if (  m_Compatidime == 1 )  // d0 
    {
      sigma = vdif(0)*vdif(0)*vErrs(0,0) + vdif(1)*vdif(1)*vErrs(1,1) + 2*vdif(0)*vdif(1)*vErrs(0,1) ;
      sigma = sqrt( sigma ) ;

    } else {  // z0

      sigma = vdif(2)*sqrt( vErrs(2,2) );
    }

//    ATH_MSG_DEBUG(" Compatibility/significance when merging vertices : " << sigma );
    ATH_MSG_DEBUG(" Compatibility/significance when merging vertices : " << sigma );

    if( sigma < m_minDist ) return true;
    else  return false;
    
  }

}///End trk namespace  
