/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: getIsolationCorrectionAccessor.cxx 625328 2014-10-31 09:39:45Z laplace $

// System include(s):
#include <vector>
#include <memory>
#include <map>
#include <stdexcept>

// Local include(s):
#include "xAODPrimitives/tools/getIsolationCorrectionAccessor.h"

/// The type to be used as the smart pointer
#if __cplusplus >= 201100
typedef std::unique_ptr< SG::AuxElement::Accessor< float > > SmartPointer_t;
typedef std::unique_ptr< SG::AuxElement::Accessor< uint32_t > > SmartPointerUint_t; // Needs to match size of IsolationCalo/TrackCorrectionBitset
#else
typedef std::auto_ptr< SG::AuxElement::Accessor< float > > SmartPointer_t;
typedef std::auto_ptr< SG::AuxElement::Accessor< uint32_t > > SmartPointerUint_t; // Needs to match size of IsolationCalo/TrackCorrectionBitset
#endif // C++11

namespace xAOD {

  SG::AuxElement::Accessor< uint32_t >*
  getIsolationCorrectionBitsetAccessor( Iso::IsolationFlavour type ){
    // Thought we could try it this way, since it's likely to be pretty sparse?
    // Can optimise if necessary
    static std::map<Iso::IsolationFlavour, SmartPointerUint_t > sAccessors;

    if (sAccessors.find(type)==sAccessors.end()){
      // create Accessor
      std::string name(Iso::toString(type));
      name+="CorrBitset";
      sAccessors[type] = SmartPointerUint_t( new SG::AuxElement::Accessor< uint32_t >( name ) ) ;
    }
  
    return sAccessors[ type ].get();    
  }

  SG::AuxElement::Accessor< float >*
  getIsolationCorrectionAccessor( Iso::IsolationFlavour type, Iso::IsolationCaloCorrection corr, Iso::IsolationCorrectionParameter param  ){
      static std::map<uint32_t, SmartPointer_t > sAccessors;                                                          
      uint32_t hash = (static_cast<uint8_t>(corr)<<8)+(static_cast<uint8_t>(param)); 
      if(corr == Iso::coreCone) hash += (static_cast<uint8_t>(type)<<16); 

      assert (0 <= static_cast<int>(param) && static_cast<int>(param) < xAOD::Iso::NumCorrParameters);
      if (sAccessors.find(hash)==sAccessors.end()){                                                                   
        std::string name(Iso::toString(type));                                                                      
        if (corr == Iso::coreCone || corr == Iso::coreConeSC)
	  name+=toString(corr); 
	else
	  name = toString(corr);
        if (param==xAOD::Iso::coreEnergy || param==xAOD::Iso::coreArea){
          name+=toString(param );    
        }else{
          throw std::runtime_error("IsolationCorrectionParameter out of bounds");
        }
        name+="Correction";
                                                                                           
        sAccessors[hash] = SmartPointer_t( new SG::AuxElement::Accessor< float >( name ) ) ;                          
      }                                                                                                               
      return sAccessors[hash].get();  
  }

  // Isolation Calo 
   SG::AuxElement::Accessor< float >*
   getIsolationCorrectionAccessor( Iso::IsolationType type, Iso::IsolationCaloCorrection corr){
      static std::map<uint32_t, SmartPointer_t > sAccessors;
      uint32_t hash = (static_cast<uint8_t>(type)<<16)+(static_cast<uint8_t>(corr)<<8);
      if (sAccessors.find(hash)==sAccessors.end()){
        std::string name(Iso::toString(type));
        name+=toString(corr);
        name+="Correction";
        
        sAccessors[hash] = SmartPointer_t( new SG::AuxElement::Accessor< float >( name ) ) ;
      }
      return sAccessors[hash].get();
  }

  SG::AuxElement::Accessor< float >*
  getIsolationCorrectionAccessor( Iso::IsolationFlavour type, Iso::IsolationTrackCorrection corr ){
      static std::map<uint32_t, SmartPointer_t > sAccessors;                                                           
      uint32_t hash = (static_cast<uint8_t>(type)<<16)+(static_cast<uint8_t>(corr)<<8);  
                                                                                                                       
      if (sAccessors.find(hash)==sAccessors.end()){                                                                    
        std::string name(Iso::toString(type));                                                                         
        name+=toString(corr);    
        name+="Correction";
                                                                                              
        sAccessors[hash] = SmartPointer_t( new SG::AuxElement::Accessor< float >( name ) ) ;                           
      }                                                                                                                
      return sAccessors[hash].get();  
  }
} // namespace xAOD
