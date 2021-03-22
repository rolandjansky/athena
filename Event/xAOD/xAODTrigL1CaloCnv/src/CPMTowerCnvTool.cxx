/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMTowerCnvTool.cxx 691832 2015-08-28 08:21:40Z morrisj $

// EDM include(s):
#include "TrigT1CaloEvent/CPMTowerCollection.h"
#include "xAODTrigL1Calo/CPMTowerContainer.h"
#include "xAODTrigL1Calo/CPMTowerAuxContainer.h"

// Local include(s):
#include "CPMTowerCnvTool.h"

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

   CPMTowerCnvTool::CPMTowerCnvTool( const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent )
      : AthAlgTool( type, name, parent ) {

      // Declare the interface(s) provided by the tool:
      declareInterface< ICPMTowerCnvTool >( this );
   }

   StatusCode CPMTowerCnvTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /**
    * This is the important function of the tool. It takes the CPMTower object
    * from the ESD and fills an xAOD::CPMTowerContainer with them.
    *
    * @param esd The ESD object to take the CPMTower from
    * @param xaod The xAOD container to fill the CPMTower into
    * @returns <code>StatusCode::SUCCESS</code> if all went fine,
    *          something else if not
    */
   StatusCode CPMTowerCnvTool::convert( const CPMTowerCollection* esd,
                                        xAOD::CPMTowerContainer* xaod) {

      // A small sanity check. The output container should really be empty...
      if( !xaod->empty() ) {
         ATH_MSG_WARNING( "The output xAOD container is not empty (size=="
                          << xaod->size() << ")" );
      }

      // Loop over the ESD objects:     
      CPMTowerCollection::const_iterator itr = esd->begin();
      CPMTowerCollection::const_iterator end = esd->end();
      for( ; itr != end; ++itr ) {
        
        xAOD::CPMTower* x = new xAOD::CPMTower();
        xaod->push_back( x );
        
        x->initialize( (float)(*itr)->eta() ,
                       (float)(*itr)->phi() ,
                       convertVector<uint8_t>((*itr)->emEnergyVec() ) ,
                       convertVector<uint8_t>((*itr)->hadEnergyVec() ) ,
                       convertVector<uint32_t>((*itr)->emErrorVec() ) ,
                       convertVector<uint32_t>((*itr)->hadErrorVec() ) , 
                       (uint8_t)(*itr)->peak() );

      }
      
      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
