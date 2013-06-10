// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RoIBTester.h 500598 2012-05-14 15:39:58Z krasznaa $
#ifndef TRIGT1ROIB_ROIBTESTER_H
#define TRIGT1ROIB_ROIBTESTER_H

// STL include(s):
#include <string>

// Athena/Gaudi include(s):
#include "AthenaBaseComps/AthAlgorithm.h"

namespace ROIB {

   /**
    *
    *  @short Dump ROIB::RoIBResult for testing purpose
    *
    *         RoIBTester takes L1 RDO (ROIB::RoIBResult) from storegate
    *         and tests/prints it.
    *
    *  @author Thomas Schoerner-Sadenius <thomas.schoerner@cern.ch>
    *  @author Wolfgang Ehrenfeld <Wolfgang.Ehrenfeld@desy.de>
    *
    * $Revision: 500598 $
    * $Date: 2012-05-14 17:39:58 +0200 (Mon, 14 May 2012) $
    */
   class RoIBTester : public AthAlgorithm {

   public:
      /// Standard Gaudi algorithm constructor
      RoIBTester( const std::string& name, ISvcLocator* pSvcLocator );

      // standard algorithm methods:
      virtual StatusCode initialize();
      virtual StatusCode execute();
      virtual StatusCode finalize();

   private:
      /// property, see @link RoIBTester::RoIBTester @endlink
      std::string m_roibRDOLocation;

   }; // class RoIBTester

} // namespace ROIB

#endif // TRIGT1ROIB_ROIBTESTER_H
