/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */


// Local include(s):
#include "xAODPrimitives/tools/getIsolationCorrectionAccessor.h"
#include <stdexcept>
namespace xAOD {

const SG::AuxElement::Accessor< uint32_t >
  getIsolationCorrectionBitsetAccessor( Iso::IsolationFlavour type ){
    std::string name(Iso::toCString(type));
    name+="CorrBitset";
    return SG::AuxElement::Accessor< uint32_t >( name );
  }

const SG::AuxElement::Accessor< float >
  getIsolationCorrectionAccessor( Iso::IsolationFlavour type, Iso::IsolationCaloCorrection corr, 
                                  Iso::IsolationCorrectionParameter param  ){
    std::string name(Iso::toCString(type));                                                                       
    if (corr == Iso::coreCone || corr == Iso::coreConeSC || corr == Iso::coreMuon){
      name+=toCString(corr); 
    }else{
        name = toCString(corr);
    }

    if (param==xAOD::Iso::coreEnergy || param==xAOD::Iso::coreArea){
      name+=toCString(param );    
    }else{
      throw std::runtime_error("IsolationCorrectionParameter out of bounds");
    }
      name+="Correction";

    return SG::AuxElement::Accessor< float >( name );                                                                                                              
  }

// Isolation Calo 
const SG::AuxElement::Accessor< float >
  getIsolationCorrectionAccessor( Iso::IsolationType type, Iso::IsolationCaloCorrection corr){
    std::string name(Iso::toCString(type));
    name+=toCString(corr);
    name+="Correction";
    return SG::AuxElement::Accessor< float >( name );
  }

const SG::AuxElement::Accessor< float >
  getIsolationCorrectionAccessor( Iso::IsolationFlavour type, Iso::IsolationTrackCorrection corr ){
      std::string name(Iso::toCString(type));                                                                         
      name+=toCString(corr);    
      name+="Correction";
      return SG::AuxElement::Accessor< float >( name );

  }
} // namespace xAOD
