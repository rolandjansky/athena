// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MissingETCnvAlg.h 586557 2014-03-06 17:56:05Z krasznaa $
#ifndef XAODMISSINGETCNV_MISSINGETCNVALG_H
#define XAODMISSINGETCNV_MISSINGETCNVALG_H

// System include(s):
#include <vector>
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Local include(s):
#include "xAODMissingETCnv/IMissingETCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Algorithm translating AOD-style MET to xAOD style
    *
    *         Documentation to be written...
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 586557 $
    * $Date: 2014-03-06 18:56:05 +0100 (Thu, 06 Mar 2014) $
    */
   class MissingETCnvAlg : public AthAlgorithm {

   public:
      /// Regular Algorithm constructor
      MissingETCnvAlg( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      virtual StatusCode initialize();
      /// Function executing the algorithm
      virtual StatusCode execute();

   private:
      /// StoreGate keys of the input MET objects
      std::vector< std::string > m_aodKeys;
      /// StoreGate key of the output MET container
      std::string m_xaodKey;

      /// Handle to the converter tool
      ToolHandle< IMissingETCnvTool > m_cnvTool;

   }; // class MissingETCnvAlg

} // namespace xAODMaker

#endif // XAODMISSINGETCNV_MISSINGETCNVALG_H
