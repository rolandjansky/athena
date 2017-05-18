/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEvent/PseudoTopResult.h"
#include "xAODCore/AddDVProxy.h"

namespace xAOD{
  /// Aux Container   
  PseudoTopResultAuxContainer::PseudoTopResultAuxContainer() :
    AuxContainerBase() 
  {
  }
  
  /// Interface class
  PseudoTopResult::PseudoTopResult() :
    SG::AuxElement()
  {        
  }

  //Initialize variables
  void PseudoTopResult::IniVar(bool isReco){

    if(isReco){
      
      this->auxdecor< float >( "PseudoTop_Reco_ttbar_eta" )     = -100.0;
      this->auxdecor< float >( "PseudoTop_Reco_ttbar_phi" )     = -100.0;
      this->auxdecor< float >( "PseudoTop_Reco_ttbar_m" )       = -100.0;
      this->auxdecor< float >( "PseudoTop_Reco_ttbar_pt" )      = -100.0;
      this->auxdecor< float >( "PseudoTop_Reco_top_had_pt" )      = -100.0;
      this->auxdecor< float >( "PseudoTop_Reco_top_had_eta" )     = -100.0;
      this->auxdecor< float >( "PseudoTop_Reco_top_had_phi" )     = -100.0;
      this->auxdecor< float >( "PseudoTop_Reco_top_had_m" )       = -100.0;
      this->auxdecor< float >( "PseudoTop_Reco_top_lep_pt" )      = -100.0;
      this->auxdecor< float >( "PseudoTop_Reco_top_lep_eta" )     = -100.0;
      this->auxdecor< float >( "PseudoTop_Reco_top_lep_phi" )     = -100.0;
      this->auxdecor< float >( "PseudoTop_Reco_top_lep_m" )       = -100.0;
      
    }
   else{
     
     this->auxdecor< float >( "PseudoTop_Particle_ttbar_eta" )     = -100.0;
     this->auxdecor< float >( "PseudoTop_Particle_ttbar_phi" )     = -100.0;
     this->auxdecor< float >( "PseudoTop_Particle_ttbar_m" )       = -100.0;
     this->auxdecor< float >( "PseudoTop_Particle_ttbar_pt" )      = -100.0;
     this->auxdecor< float >( "PseudoTop_Particle_top_had_pt" )      = -100.0;
     this->auxdecor< float >( "PseudoTop_Particle_top_had_eta" )     = -100.0;
     this->auxdecor< float >( "PseudoTop_Particle_top_had_phi" )     = -100.0;
     this->auxdecor< float >( "PseudoTop_Particle_top_had_m" )       = -100.0;
     this->auxdecor< float >( "PseudoTop_Particle_top_lep_pt" )      = -100.0;
     this->auxdecor< float >( "PseudoTop_Particle_top_lep_eta" )     = -100.0;
     this->auxdecor< float >( "PseudoTop_Particle_top_lep_phi" )     = -100.0;
     this->auxdecor< float >( "PseudoTop_Particle_top_lep_m" )       = -100.0;
     
   }

  }

}
ADD_NS_DV_PROXY( xAOD , PseudoTopResultContainer );
