/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EgammaAccessors_v1.cxx 582601 2014-02-11 15:46:49Z christos $

// System include(s):
#include <iostream>

// Local include(s):
#include "EgammaAccessors_v1.h"

/// Helper macro for Accessor objects
#define DEFINE_ACCESSOR(TYPE, NAME )                               \
   case xAOD::EgammaParameters::NAME:                              \
   {                                                               \
     static SG::AuxElement::Accessor< TYPE > a( #NAME );	   \
     return &a;							   \
   }                                                               \
   break;

namespace xAOD {

  /// Explicit Instantiation  of Template
   SG::AuxElement::Accessor< float >*
   showerShapeAccessorV1( xAOD::EgammaParameters::ShowerShapeType type ) 
   {   
      switch( type ) {
        DEFINE_ACCESSOR( float, e011 );     
        DEFINE_ACCESSOR( float, e033 );     
        DEFINE_ACCESSOR( float, e132 );     
        DEFINE_ACCESSOR( float, e1152 );    
        DEFINE_ACCESSOR( float, ethad1 );   
        DEFINE_ACCESSOR( float, ethad );    
        DEFINE_ACCESSOR( float, ehad1 );    
        DEFINE_ACCESSOR( float, f1 );       
        DEFINE_ACCESSOR( float, f3 );       
        DEFINE_ACCESSOR( float, f1core );   
        DEFINE_ACCESSOR( float, f3core );   
        DEFINE_ACCESSOR( float, e233 );     
        DEFINE_ACCESSOR( float, e235 );     
        DEFINE_ACCESSOR( float, e255 );     
        DEFINE_ACCESSOR( float, e237 );     
        DEFINE_ACCESSOR( float, e277 );     
        DEFINE_ACCESSOR( float, e333 );     
        DEFINE_ACCESSOR( float, e335 );     
        DEFINE_ACCESSOR( float, e337 );     
        DEFINE_ACCESSOR( float, e377 );     
        DEFINE_ACCESSOR( float, weta1 );    
        DEFINE_ACCESSOR( float, weta2 );    
        DEFINE_ACCESSOR( float, e2ts1 );    
        DEFINE_ACCESSOR( float, e2tsts1 );  
        DEFINE_ACCESSOR( float, fracs1 );   
        DEFINE_ACCESSOR( float, widths1 );  
        DEFINE_ACCESSOR( float, widths2 );  
        DEFINE_ACCESSOR( float, poscs1 );   
        DEFINE_ACCESSOR( float, poscs2 );   
        DEFINE_ACCESSOR( float, asy1 );     
        DEFINE_ACCESSOR( float, pos ); 
        DEFINE_ACCESSOR( float, pos7 ); 
        DEFINE_ACCESSOR( float, barys1 );   
        DEFINE_ACCESSOR( float, wtots1 );   
        DEFINE_ACCESSOR( float, emins1 );   
        DEFINE_ACCESSOR( float, emaxs1 );   
        DEFINE_ACCESSOR( float, r33over37allcalo );     
        DEFINE_ACCESSOR( float, ecore );
        DEFINE_ACCESSOR( float, zvertex );
        DEFINE_ACCESSOR( float, errz );
        DEFINE_ACCESSOR( float, etap ); 
        DEFINE_ACCESSOR( float, depth );
      default:                  
         std::cerr << "xAOD::Egamma_v1 ERROR Unknown float ShowerShapeType ("
                   << type << ") requested" << std::endl;
         return 0;
      }
   
   }  

   SG::AuxElement::Accessor< float >*
   isolationAccessorV1( xAOD::EgammaParameters::IsolationType type ) 
   {
      switch( type ) {
        DEFINE_ACCESSOR( float, etcone );
        DEFINE_ACCESSOR( float, etcone15 );
        DEFINE_ACCESSOR( float, etcone20 );
        DEFINE_ACCESSOR( float, etcone25 );
        DEFINE_ACCESSOR( float, etcone30 );
        DEFINE_ACCESSOR( float, etcone35 );
        DEFINE_ACCESSOR( float, etcone40 );
        DEFINE_ACCESSOR( float, ptcone20 );
        DEFINE_ACCESSOR( float, ptcone30 );
        DEFINE_ACCESSOR( float, ptcone40 );
        DEFINE_ACCESSOR( float, etcone15_ptcorrected );
        DEFINE_ACCESSOR( float, etcone20_ptcorrected );
        DEFINE_ACCESSOR( float, etcone25_ptcorrected );
        DEFINE_ACCESSOR( float, etcone30_ptcorrected );
        DEFINE_ACCESSOR( float, etcone35_ptcorrected );
        DEFINE_ACCESSOR( float, etcone40_ptcorrected );
        DEFINE_ACCESSOR( float, etcone20_corrected );
        DEFINE_ACCESSOR( float, etcone30_corrected );
        DEFINE_ACCESSOR( float, etcone40_corrected );
        DEFINE_ACCESSOR( float, topoetcone20 );
        DEFINE_ACCESSOR( float, topoetcone30 );
        DEFINE_ACCESSOR( float, topoetcone40 );
        DEFINE_ACCESSOR( float, topoetcone40_ptcorrected );
        DEFINE_ACCESSOR( float, topoetcone40_corrected );
        DEFINE_ACCESSOR( float, nucone20 );
        DEFINE_ACCESSOR( float, nucone30 );
        DEFINE_ACCESSOR( float, nucone40 );
      default:
         std::cerr << "xAOD::Egamma_v1 ERROR Unknown float IsolationType ("
                   << type << ") requested" << std::endl;
         return 0;
      }

   }

} // namespace xAOD
