/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EgammaAuxContainer_v1.cxx 596434 2014-05-11 17:30:02Z krasznaa $

// Local include(s):
#include "xAODEgamma/versions/EgammaAuxContainer_v1.h"

namespace xAOD {
  
  EgammaAuxContainer_v1::EgammaAuxContainer_v1()
    : AuxContainerBase() {

    ///@name links to calo clusters 
    AUX_VARIABLE(caloClusterLinks);


    ///@name IParticle variables
    AUX_VARIABLE(pt);             
    AUX_VARIABLE(eta);           
    AUX_VARIABLE(phi); 
    AUX_VARIABLE(m);          

    ///@name Covariance Matrix
    AUX_VARIABLE(EgammaCovarianceMatrix);

    ///@name Author    
    AUX_VARIABLE( author );

    ///@name Data Quality flag
    AUX_VARIABLE( OQ );
    
    ///@name Shower Shape Details
    AUX_VARIABLE( e011 );     
    AUX_VARIABLE( e033 );     
    AUX_VARIABLE( e132 );     
    AUX_VARIABLE( e1152 );    
    AUX_VARIABLE( ethad1 );   
    AUX_VARIABLE( ethad );    
    AUX_VARIABLE( ehad1 );    
    AUX_VARIABLE( f1 );       
    AUX_VARIABLE( f3 );       
    AUX_VARIABLE( f1core );   
    AUX_VARIABLE( f3core );   
    AUX_VARIABLE( e233 );     
    AUX_VARIABLE( e235 );     
    AUX_VARIABLE( e255 );     
    AUX_VARIABLE( e237 );     
    AUX_VARIABLE( e277 );     
    AUX_VARIABLE( e333 );     
    AUX_VARIABLE( e335 );     
    AUX_VARIABLE( e337 );     
    AUX_VARIABLE( e377 );     
    AUX_VARIABLE( weta1 );    
    AUX_VARIABLE( weta2 );    
    AUX_VARIABLE( e2ts1 );    
    AUX_VARIABLE( e2tsts1 );  
    AUX_VARIABLE( fracs1 );   
    AUX_VARIABLE( widths1 );  
    AUX_VARIABLE( widths2 );  
    AUX_VARIABLE( poscs1 );   
    AUX_VARIABLE( poscs2 );   
    AUX_VARIABLE( asy1 );     
    AUX_VARIABLE( pos ); 
    AUX_VARIABLE( pos7 ); 
    AUX_VARIABLE( barys1 );   
    AUX_VARIABLE( wtots1 );   
    AUX_VARIABLE( emins1 );   
    AUX_VARIABLE( emaxs1 );   
    AUX_VARIABLE( r33over37allcalo );     
    AUX_VARIABLE( ecore );
    AUX_VARIABLE( zvertex );
    AUX_VARIABLE( errz );
    AUX_VARIABLE( etap ); 
    AUX_VARIABLE( depth );

    ///@name ISoalation Variables
    AUX_VARIABLE( etcone );   
    AUX_VARIABLE( etcone15 ); 
    AUX_VARIABLE( etcone20 ); 
    AUX_VARIABLE( etcone25 ); 
    AUX_VARIABLE( etcone30 ); 
    AUX_VARIABLE( etcone35 ); 
    AUX_VARIABLE( etcone40 ); 
    AUX_VARIABLE( ptcone20 );
    AUX_VARIABLE( ptcone30 );
    AUX_VARIABLE( ptcone40 );
    AUX_VARIABLE( nucone20 );
    AUX_VARIABLE( nucone30 );
    AUX_VARIABLE( nucone40 );
    AUX_VARIABLE( etcone15_ptcorrected );
    AUX_VARIABLE( etcone20_ptcorrected );
    AUX_VARIABLE( etcone25_ptcorrected );
    AUX_VARIABLE( etcone30_ptcorrected );
    AUX_VARIABLE( etcone35_ptcorrected );
    AUX_VARIABLE( etcone40_ptcorrected );
    AUX_VARIABLE( etcone20_corrected );
    AUX_VARIABLE( etcone30_corrected );
    AUX_VARIABLE( etcone40_corrected );
    AUX_VARIABLE( topoetcone20 );
    AUX_VARIABLE( topoetcone30 );
    AUX_VARIABLE( topoetcone40 );
    AUX_VARIABLE( topoetcone40_ptcorrected );
    AUX_VARIABLE( topoetcone40_corrected );
   
  }
  
} // namespace xAOD
