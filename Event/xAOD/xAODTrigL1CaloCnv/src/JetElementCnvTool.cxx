/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetElementCnvTool.cxx 691832 2015-08-28 08:21:40Z morrisj $

// EDM include(s):
#include "TrigT1CaloEvent/JetElementCollection.h"
#include "xAODTrigL1Calo/JetElementContainer.h"
#include "xAODTrigL1Calo/JetElementAuxContainer.h"

// Local include(s):
#include "JetElementCnvTool.h"

namespace {
  template <typename T>
  std::vector<T> convertVector(const std::vector<int>& in) {
    std::vector<T> result;
    result.reserve(in.size());

for(auto i : in) {
      result.push_back(static_cast<T>(i));
    }
    return result;
  }
}

namespace xAODMaker {

   JetElementCnvTool::JetElementCnvTool( const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent )
      : AthAlgTool( type, name, parent ) {

      // Declare the interface(s) provided by the tool:
      declareInterface< IJetElementCnvTool >( this );
   }

   StatusCode JetElementCnvTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /**
    * This is the important function of the tool. It takes the JetElement object
    * from the ESD and fills an xAOD::JetElementContainer with them.
    *
    * @param esd The ESD object to take the JetElement from
    * @param xaod The xAOD container to fill the JetElement into
    * @returns <code>StatusCode::SUCCESS</code> if all went fine,
    *          something else if not
    */
   StatusCode JetElementCnvTool::convert( const JetElementCollection* esd,
                                          xAOD::JetElementContainer* xaod) {

      // A small sanity check. The output container should really be empty...
      if( !xaod->empty() ) {
         ATH_MSG_WARNING( "The output xAOD container is not empty (size=="
                          << xaod->size() << ")" );
      }

      // Loop over the ESD objects:     
      JetElementCollection::const_iterator itr = esd->begin();
      JetElementCollection::const_iterator end = esd->end();
      for( ; itr != end; ++itr ) {
        
        xAOD::JetElement* x = new xAOD::JetElement();
        xaod->push_back( x );
        
        x->initialize( (float)(*itr)->eta() ,
                       (float)(*itr)->phi() ,
                       (*itr)->key() ,
                       convertVector<uint16_t>((*itr)->emEnergyVec()) ,
                       convertVector<uint16_t>((*itr)->hadEnergyVec()) ,
                       convertVector<uint32_t>((*itr)->emErrorVec()) ,
                       convertVector<uint32_t>((*itr)->hadErrorVec()) ,
                       convertVector<uint32_t>((*itr)->linkErrorVec()) ,
                       (uint8_t)(*itr)->peak() );
      }
      
      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
