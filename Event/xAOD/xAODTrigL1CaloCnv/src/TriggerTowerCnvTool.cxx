/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerTowerCnvTool.cxx 646315 2015-02-11 23:28:47Z morrisj $

// EDM include(s):
#include "TrigT1CaloEvent/TriggerTowerCollection.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "xAODTrigL1Calo/TriggerTowerAuxContainer.h"

// Local include(s):
#include "TriggerTowerCnvTool.h"

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

   TriggerTowerCnvTool::TriggerTowerCnvTool( const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent )
      : AthAlgTool( type, name, parent )
   {

      // Declare the interface(s) provided by the tool:
      declareInterface< ITriggerTowerCnvTool >( this );
   }

   StatusCode TriggerTowerCnvTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /**
    * This is the important function of the tool. It takes the TriggerTower object
    * from the ESD and fills an xAOD::TriggerTowerContainer with them.
    *
    * @param esd The ESD object to take the TriggerTower from
    * @param xaod The xAOD container to fill the TriggerTower into
    * @returns <code>StatusCode::SUCCESS</code> if all went fine,
    *          something else if not
    */
   StatusCode TriggerTowerCnvTool::convert( const TriggerTowerCollection* esd,
                                            xAOD::TriggerTowerContainer* xaod) {

      // A small sanity check. The output container should really be empty...
      if( !xaod->empty() ) {
         ATH_MSG_WARNING( "The output xAOD container is not empty (size=="
                          << xaod->size() << ")" );
      }

      // Loop over the ESD objects:     
      for( const auto *const tt : *esd) {
      // TriggerTowerCollection::const_iterator itr = esd->begin();
      // TriggerTowerCollection::const_iterator end = esd->end();
      // for( ; itr != end; ++itr ) {
        // EM
        xAOD::TriggerTower* x = new xAOD::TriggerTower;
        xaod->push_back( x );

        x->initialize(0u, // coolId
                      tt->eta(), // eta
                      tt->phi(), // phi
                      convertVector<uint_least8_t>(tt->emLUT()), // lut_cp
                      convertVector<uint_least8_t>(tt->emLUT()),  // lut_jep
                      std::vector<int_least16_t>(tt->emLUT().size(), 0), // correction
                      std::vector<uint_least8_t>(tt->emLUT().size(), 0u), // correnctionEnabled
                      convertVector<uint_least8_t>(tt->emBCIDvec()), // bcidVec
                      convertVector<uint_least16_t>(tt->emADC()), // adc
                      convertVector<uint_least8_t>(tt->emBCIDext()), //bcidExt
                      std::vector<uint_least8_t>(tt->emLUT().size(), 0u), // Sat80Vec
                      static_cast<uint_least16_t>(tt->emError()), // error
                      static_cast<uint_least8_t>(tt->emPeak()), // peak
                      static_cast<uint_least8_t>(tt->emADCPeak())); //adcPeak

        // HAD
        x = new xAOD::TriggerTower;
        xaod->push_back( x );

        x->initialize(0u,
                      tt->eta(),
                      tt->phi(),
                      convertVector<uint_least8_t>(tt->hadLUT()),
                      convertVector<uint_least8_t>(tt->hadLUT()), 
                      std::vector<int_least16_t>(tt->hadLUT().size(), 0),
                      std::vector<uint_least8_t>(tt->hadLUT().size(), 0u),
                      convertVector<uint_least8_t>(tt->hadBCIDvec()),
                      convertVector<uint_least16_t>(tt->hadADC()),
                      convertVector<uint_least8_t>(tt->hadBCIDext()),
                      std::vector<uint_least8_t>(tt->hadLUT().size(), 0u),
                      static_cast<uint_least16_t>(tt->hadError()),
                      static_cast<uint_least8_t>(tt->hadPeak()),
                      static_cast<uint_least8_t>(tt->hadADCPeak()));
      }      
      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
