/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EgammaAccessors_v1.cxx 741489 2016-04-20 01:30:34Z christos $

// System include(s):
#include <iostream>

// Local include(s):
#include "EgammaAccessors_v1.h"

/// Helper macro for Accessor objects
#define DEFINE_ACCESSOR(TYPE, NAME )                               \
   case xAOD::EgammaParameters::NAME:                              \
   {                                                               \
     static const SG::AuxElement::Accessor< TYPE > a( #NAME );	   \
     return &a;							   \
   }                                                               \
   break;

namespace xAOD {

  /// Explicit Instantiation  of Template
   const SG::AuxElement::Accessor< float >*
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
        DEFINE_ACCESSOR( float, Reta );
        DEFINE_ACCESSOR( float, Rphi );
        DEFINE_ACCESSOR( float, Eratio );
        DEFINE_ACCESSOR( float, Rhad );
        DEFINE_ACCESSOR( float, Rhad1 );
        DEFINE_ACCESSOR( float, DeltaE );
      default:                  
         std::cerr << "xAOD::Egamma  ERROR Unknown float ShowerShapeType ("
                   << type << ") requested" << std::endl;
         return 0;
      }
   
   }  

   const SG::AuxElement::Accessor< char >*
   selectionMenuAccessorV1( xAOD::EgammaParameters::SelectionMenu menu ) 
   {   
      switch( menu ) {
        DEFINE_ACCESSOR( char, Loose);
        DEFINE_ACCESSOR( char, Medium);
        DEFINE_ACCESSOR( char, Tight);
        DEFINE_ACCESSOR( char, LHLoose);
        DEFINE_ACCESSOR( char, LHMedium);
        DEFINE_ACCESSOR( char, LHTight);
        DEFINE_ACCESSOR( char, MultiLepton);
      default:                  
         std::cerr << "xAOD::Egamma ERROR Unknown char ElectronSelectionMenu ("
                   << menu << ") requested" << std::endl;
         return 0;
      }
   }

   const SG::AuxElement::Accessor< unsigned int >*
   selectionisEMAccessorV1( xAOD::EgammaParameters::SelectionisEM isEM ) 
   {   
      switch( isEM ) {
        DEFINE_ACCESSOR( unsigned int, isEMLoose);
        DEFINE_ACCESSOR( unsigned int, isEMMedium);
        DEFINE_ACCESSOR( unsigned int, isEMTight);
        DEFINE_ACCESSOR( unsigned int, isEMLHLoose);
        DEFINE_ACCESSOR( unsigned int, isEMLHMedium);
        DEFINE_ACCESSOR( unsigned int, isEMLHTight);
        DEFINE_ACCESSOR( unsigned int, isEMMultiLepton);
      default:                  
         std::cerr << "xAOD::Egamma ERROR Unknown unsigned int ElectronSelectionisEM ("
                   << isEM << ") requested" << std::endl;
         return 0;
      }
   }


} // namespace xAOD
