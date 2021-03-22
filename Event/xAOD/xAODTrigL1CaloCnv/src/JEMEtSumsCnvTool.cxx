/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JEMEtSumsCnvTool.cxx 645494 2015-02-10 10:42:39Z morrisj $

// EDM include(s):
#include "TrigT1CaloEvent/JEMEtSumsCollection.h"
#include "xAODTrigL1Calo/JEMEtSumsContainer.h"
#include "xAODTrigL1Calo/JEMEtSumsAuxContainer.h"

// Local include(s):
#include "JEMEtSumsCnvTool.h"

namespace {
  template <typename T>
  std::vector<T> convertVector(const std::vector<int>& in) {
    std::vector<T> result;
    for(auto i : in) {
      result.push_back(static_cast<T>(i));
    }
    return result;
  }
}

namespace {
  template <typename T>
  std::vector<T> convertVector(const std::vector<unsigned int>& in) {
    std::vector<T> result;
    result.reserve(in.size());

for(auto i : in) {
      result.push_back(static_cast<T>(i));
    }
    return result;
  }
}

namespace xAODMaker {

   JEMEtSumsCnvTool::JEMEtSumsCnvTool( const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent )
      : AthAlgTool( type, name, parent ) {

      // Declare the interface(s) provided by the tool:
      declareInterface< IJEMEtSumsCnvTool >( this );
   }

   StatusCode JEMEtSumsCnvTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /**
    * This is the important function of the tool. It takes the JEMEtSums object
    * from the ESD and fills an xAOD::JEMEtSumsContainer with them.
    *
    * @param esd The ESD object to take the JEMEtSums from
    * @param xaod The xAOD container to fill the JEMEtSums into
    * @returns <code>StatusCode::SUCCESS</code> if all went fine,
    *          something else if not
    */
   StatusCode JEMEtSumsCnvTool::convert( const JEMEtSumsCollection* esd,
                                         xAOD::JEMEtSumsContainer* xaod) {

      // A small sanity check. The output container should really be empty...
      if( !xaod->empty() ) {
         ATH_MSG_WARNING( "The output xAOD container is not empty (size=="
                          << xaod->size() << ")" );
      }

      // Loop over the ESD objects:     
      JEMEtSumsCollection::const_iterator itr = esd->begin();
      JEMEtSumsCollection::const_iterator end = esd->end();
      for( ; itr != end; ++itr ) {
        
        xAOD::JEMEtSums* x = new xAOD::JEMEtSums();
        xaod->push_back( x );
        
        x->initialize( (uint_least8_t)(*itr)->crate() ,
                       (uint_least8_t)(*itr)->module() ,
                       convertVector<uint_least16_t>((*itr)->EtVec()) ,
                       convertVector<uint_least16_t>((*itr)->ExVec()) ,
                       convertVector<uint_least16_t>((*itr)->EyVec()) ,
                       (uint_least8_t)(*itr)->peak() ); 
      }
      
      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
