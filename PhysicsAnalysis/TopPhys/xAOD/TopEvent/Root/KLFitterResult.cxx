/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: SystematicEvent.cxx 661144 2015-04-17 09:09:07Z morrisj $
#include "TopEvent/KLFitterResult.h"

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"
#include "xAODCore/AddDVProxy.h"

namespace xAOD{
  // Aux Container
  KLFitterResultAuxContainer::KLFitterResultAuxContainer() :
    AuxContainerBase() 
  {
    AUX_VARIABLE( minuitDidNotConverge );
    AUX_VARIABLE( fitAbortedDueToNaN );
    AUX_VARIABLE( atLeastOneFitParameterAtItsLimit );
    AUX_VARIABLE( invalidTransferFunctionAtConvergence );
    
    AUX_VARIABLE( bestPermutation );
    AUX_VARIABLE( logLikelihood );
    AUX_VARIABLE( eventProbability );
    AUX_VARIABLE( parameters );
    AUX_VARIABLE( parameterErrors );
    
    AUX_VARIABLE( model_bhad_pt );
    AUX_VARIABLE( model_bhad_eta );    
    AUX_VARIABLE( model_bhad_phi );
    AUX_VARIABLE( model_bhad_E );
    AUX_VARIABLE( model_bhad_jetIndex );
    
    AUX_VARIABLE( model_blep_pt );
    AUX_VARIABLE( model_blep_eta );    
    AUX_VARIABLE( model_blep_phi );
    AUX_VARIABLE( model_blep_E );
    AUX_VARIABLE( model_blep_jetIndex );

    AUX_VARIABLE( model_lq1_pt );
    AUX_VARIABLE( model_lq1_eta );    
    AUX_VARIABLE( model_lq1_phi );
    AUX_VARIABLE( model_lq1_E );
    AUX_VARIABLE( model_lq1_jetIndex );

    AUX_VARIABLE( model_lq2_pt );
    AUX_VARIABLE( model_lq2_eta );    
    AUX_VARIABLE( model_lq2_phi );
    AUX_VARIABLE( model_lq2_E );
    AUX_VARIABLE( model_lq2_jetIndex );

    AUX_VARIABLE( model_Higgs_b1_pt );
    AUX_VARIABLE( model_Higgs_b1_eta );
    AUX_VARIABLE( model_Higgs_b1_phi );
    AUX_VARIABLE( model_Higgs_b1_E );
    AUX_VARIABLE( model_Higgs_b1_jetIndex );

    AUX_VARIABLE( model_Higgs_b2_pt );
    AUX_VARIABLE( model_Higgs_b2_eta );
    AUX_VARIABLE( model_Higgs_b2_phi );
    AUX_VARIABLE( model_Higgs_b2_E );
    AUX_VARIABLE( model_Higgs_b2_jetIndex );

    AUX_VARIABLE( model_lep_pt );
    AUX_VARIABLE( model_lep_eta );    
    AUX_VARIABLE( model_lep_phi );
    AUX_VARIABLE( model_lep_E );     
    AUX_VARIABLE( model_lep_index );

    AUX_VARIABLE( model_lepZ1_pt );
    AUX_VARIABLE( model_lepZ1_eta );
    AUX_VARIABLE( model_lepZ1_phi );
    AUX_VARIABLE( model_lepZ1_E );
    AUX_VARIABLE( model_lepZ1_index );

    AUX_VARIABLE( model_lepZ2_pt );
    AUX_VARIABLE( model_lepZ2_eta );
    AUX_VARIABLE( model_lepZ2_phi );
    AUX_VARIABLE( model_lepZ2_E );
    AUX_VARIABLE( model_lepZ2_index );

    AUX_VARIABLE( model_nu_pt );
    AUX_VARIABLE( model_nu_eta );
    AUX_VARIABLE( model_nu_phi );
    AUX_VARIABLE( model_nu_E );

    AUX_VARIABLE( model_b_from_top1_pt );
    AUX_VARIABLE( model_b_from_top1_eta );    
    AUX_VARIABLE( model_b_from_top1_phi );
    AUX_VARIABLE( model_b_from_top1_E );
    AUX_VARIABLE( model_b_from_top1_jetIndex );
    
    AUX_VARIABLE( model_b_from_top2_pt );
    AUX_VARIABLE( model_b_from_top2_eta );    
    AUX_VARIABLE( model_b_from_top2_phi );
    AUX_VARIABLE( model_b_from_top2_E );
    AUX_VARIABLE( model_b_from_top2_jetIndex );
    
    AUX_VARIABLE( model_lj1_from_top1_pt );
    AUX_VARIABLE( model_lj1_from_top1_eta );    
    AUX_VARIABLE( model_lj1_from_top1_phi );
    AUX_VARIABLE( model_lj1_from_top1_E );
    AUX_VARIABLE( model_lj1_from_top1_jetIndex );
    
    AUX_VARIABLE( model_lj2_from_top1_pt );
    AUX_VARIABLE( model_lj2_from_top1_eta );    
    AUX_VARIABLE( model_lj2_from_top1_phi );
    AUX_VARIABLE( model_lj2_from_top1_E );
    AUX_VARIABLE( model_lj2_from_top1_jetIndex );
    
    AUX_VARIABLE( model_lj1_from_top2_pt );
    AUX_VARIABLE( model_lj1_from_top2_eta );    
    AUX_VARIABLE( model_lj1_from_top2_phi );
    AUX_VARIABLE( model_lj1_from_top2_E );
    AUX_VARIABLE( model_lj1_from_top2_jetIndex );
    
    AUX_VARIABLE( model_lj2_from_top2_pt );
    AUX_VARIABLE( model_lj2_from_top2_eta );    
    AUX_VARIABLE( model_lj2_from_top2_phi );
    AUX_VARIABLE( model_lj2_from_top2_E );
    AUX_VARIABLE( model_lj2_from_top2_jetIndex );
    

  }
  
  /// Interface class
  KLFitterResult::KLFitterResult() :
    SG::AuxElement()
  {        
  } 
  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , short , minuitDidNotConverge , setMinuitDidNotConverge )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , short , fitAbortedDueToNaN , setFitAbortedDueToNaN )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , short , atLeastOneFitParameterAtItsLimit , setAtLeastOneFitParameterAtItsLimit )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , short , invalidTransferFunctionAtConvergence , setInvalidTransferFunctionAtConvergence )
  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , unsigned int , bestPermutation , setBestPermutation )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , logLikelihood , setLogLikelihood )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , eventProbability , setEventProbability )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( KLFitterResult , std::vector<double> , parameters , setParameters )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( KLFitterResult , std::vector<double> , parameterErrors , setParameterErrors )
  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_bhad_pt , setModel_bhad_pt )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_bhad_eta , setModel_bhad_eta )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_bhad_phi , setModel_bhad_phi )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_bhad_E , setModel_bhad_E )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , unsigned int , model_bhad_jetIndex , setModel_bhad_jetIndex )
  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_blep_pt , setModel_blep_pt )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_blep_eta , setModel_blep_eta )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_blep_phi , setModel_blep_phi )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_blep_E , setModel_blep_E )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , unsigned int , model_blep_jetIndex , setModel_blep_jetIndex )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lq1_pt , setModel_lq1_pt )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lq1_eta , setModel_lq1_eta )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lq1_phi , setModel_lq1_phi )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lq1_E , setModel_lq1_E )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , unsigned int , model_lq1_jetIndex , setModel_lq1_jetIndex )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lq2_pt , setModel_lq2_pt )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lq2_eta , setModel_lq2_eta )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lq2_phi , setModel_lq2_phi )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lq2_E , setModel_lq2_E )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , unsigned int , model_lq2_jetIndex , setModel_lq2_jetIndex )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_Higgs_b1_pt , setModel_Higgs_b1_pt )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_Higgs_b1_eta , setModel_Higgs_b1_eta )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_Higgs_b1_phi , setModel_Higgs_b1_phi )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_Higgs_b1_E , setModel_Higgs_b1_E )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , unsigned int , model_Higgs_b1_jetIndex , setModel_Higgs_b1_jetIndex )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_Higgs_b2_pt , setModel_Higgs_b2_pt )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_Higgs_b2_eta , setModel_Higgs_b2_eta )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_Higgs_b2_phi , setModel_Higgs_b2_phi )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_Higgs_b2_E , setModel_Higgs_b2_E )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , unsigned int , model_Higgs_b2_jetIndex , setModel_Higgs_b2_jetIndex )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lep_pt , setModel_lep_pt )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lep_eta , setModel_lep_eta )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lep_phi , setModel_lep_phi )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lep_E , setModel_lep_E )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , unsigned int , model_lep_index , setModel_lep_index )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lepZ1_pt , setModel_lepZ1_pt )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lepZ1_eta , setModel_lepZ1_eta )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lepZ1_phi , setModel_lepZ1_phi )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lepZ1_E , setModel_lepZ1_E )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , unsigned int , model_lepZ1_index , setModel_lepZ1_index )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lepZ2_pt , setModel_lepZ2_pt )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lepZ2_eta , setModel_lepZ2_eta )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lepZ2_phi , setModel_lepZ2_phi )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lepZ2_E , setModel_lepZ2_E )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , unsigned int , model_lepZ2_index , setModel_lepZ2_index )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_nu_pt , setModel_nu_pt )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_nu_eta , setModel_nu_eta )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_nu_phi , setModel_nu_phi )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_nu_E , setModel_nu_E )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_b_from_top1_pt , setModel_b_from_top1_pt )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_b_from_top1_eta , setModel_b_from_top1_eta )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_b_from_top1_phi , setModel_b_from_top1_phi )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_b_from_top1_E , setModel_b_from_top1_E )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , unsigned int , model_b_from_top1_jetIndex , setModel_b_from_top1_jetIndex )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_b_from_top2_pt , setModel_b_from_top2_pt )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_b_from_top2_eta , setModel_b_from_top2_eta )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_b_from_top2_phi , setModel_b_from_top2_phi )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_b_from_top2_E , setModel_b_from_top2_E )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , unsigned int , model_b_from_top2_jetIndex , setModel_b_from_top2_jetIndex )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lj1_from_top1_pt , setModel_lj1_from_top1_pt )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lj1_from_top1_eta , setModel_lj1_from_top1_eta )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lj1_from_top1_phi , setModel_lj1_from_top1_phi )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lj1_from_top1_E , setModel_lj1_from_top1_E )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , unsigned int , model_lj1_from_top1_jetIndex , setModel_lj1_from_top1_jetIndex )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lj2_from_top1_pt , setModel_lj2_from_top1_pt )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lj2_from_top1_eta , setModel_lj2_from_top1_eta )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lj2_from_top1_phi , setModel_lj2_from_top1_phi )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lj2_from_top1_E , setModel_lj2_from_top1_E )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , unsigned int , model_lj2_from_top1_jetIndex , setModel_lj2_from_top1_jetIndex )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lj1_from_top2_pt , setModel_lj1_from_top2_pt )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lj1_from_top2_eta , setModel_lj1_from_top2_eta )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lj1_from_top2_phi , setModel_lj1_from_top2_phi )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lj1_from_top2_E , setModel_lj1_from_top2_E )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , unsigned int , model_lj1_from_top2_jetIndex , setModel_lj1_from_top2_jetIndex )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lj2_from_top2_pt , setModel_lj2_from_top2_pt )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lj2_from_top2_eta , setModel_lj2_from_top2_eta )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lj2_from_top2_phi , setModel_lj2_from_top2_phi )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , float , model_lj2_from_top2_E , setModel_lj2_from_top2_E )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( KLFitterResult , unsigned int , model_lj2_from_top2_jetIndex , setModel_lj2_from_top2_jetIndex )

}
ADD_NS_DV_PROXY( xAOD , KLFitterResultContainer );

