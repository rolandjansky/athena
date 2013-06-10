// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RecRoIBTester.h 500598 2012-05-14 15:39:58Z krasznaa $
#ifndef TRIGT1ROIB_RECROIBTESTER_H
#define TRIGT1ROIB_RECROIBTESTER_H

// Athena/Gaudi include(s):
#include "AthenaBaseComps/AthAlgorithm.h"

namespace ROIB {

   /**
    *  @short Dump ROIB::RecRoIBResult for testing purpose
    *
    *         RecRoIBTester takes L1 reconstructed RDO (ROIB::RecRoIBResult)
    *         from storegate and tests/prints it.
    *
    * @author Thomas Schoerner-Sadenius <thomas.schoerner@cern.ch>
    * @author last commit \$Author: efeld $
    *
    * $Revision: 500598 $
    * $Date: 2012-05-14 17:39:58 +0200 (Mon, 14 May 2012) $
    */
   class RecRoIBTester : public AthAlgorithm {

   public:
      /// Standard Gaudi Athena constructor
      RecRoIBTester( const std::string& name, ISvcLocator* pSvcLocator );

      // standard algorithm methods:
      virtual StatusCode initialize();
      virtual StatusCode execute();
      virtual StatusCode finalize();

   }; // class RecRoIBTester

} // namespace ROIB

#endif // TRIGT1ROIB_RECROIBTESTER_H
