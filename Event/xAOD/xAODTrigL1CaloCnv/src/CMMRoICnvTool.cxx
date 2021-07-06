/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


// EDM include(s):
#include "TrigT1CaloEvent/CMMRoI.h"
#include "xAODTrigL1Calo/CMMRoI.h"
#include "xAODTrigL1Calo/CMMRoIAuxInfo.h"

// Local include(s):
#include "CMMRoICnvTool.h"

namespace xAODMaker {

   CMMRoICnvTool::CMMRoICnvTool( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent )
      : AthAlgTool( type, name, parent ) {

      // Declare the interface(s) provided by the tool:
      declareInterface< ICMMRoICnvTool >( this );
   }

   /**
    * This is the important function of the tool. It takes the CMMRoI object
    * from the ESD and fills an xAOD::CMMRoIContainer with them.
    *
    * @param esd The ESD object to take the CMMRoI from
    * @param xaod The xAOD container to fill the CMMRoI into
    * @returns <code>StatusCode::SUCCESS</code> if all went fine,
    *          something else if not
    */
   StatusCode CMMRoICnvTool::convert( const LVL1::CMMRoI* esd,
                                      xAOD::CMMRoI* xaod) {
                                                      
      xaod->setRoiWord( esd->jetEtRoiWord() );
      xaod->setRoiWord( esd->energyRoiWord0() );
      xaod->setRoiWord( esd->energyRoiWord1() );
      xaod->setRoiWord( esd->energyRoiWord2() );
            
      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
