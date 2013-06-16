// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L1MuctpiTestReadResults.h 362102 2011-04-28 13:17:28Z krasznaa $
#ifndef TRIGT1MUCTPI_L1MUCTPITESTREADRESULTS_H
#define TRIGT1MUCTPI_L1MUCTPITESTREADRESULTS_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"

namespace LVL1MUCTPI {

   /**
    *   $Date: 2011-04-28 15:17:28 +0200 (Thu, 28 Apr 2011) $
    *
    *   @short Tester for the MuCTPI simulation's output
    *
    *          This algorithm has little importance now. It reads back the objects
    *          produced by the MuCTPI simulation from StoreGate, and confirms that
    *          they are in fact there.
    *
    *  @author $Author: krasznaa $
    * @version $Revision: 362102 $
    */
   class L1MuctpiTestReadResults : public AthAlgorithm {

   public:
      /// Regular Gaudi algorithm constructor
      L1MuctpiTestReadResults( const std::string& name,
                               ISvcLocator* pSvcLocator );

      /// Regular Gaudi execute function
      virtual StatusCode execute();

   }; // class L1MuctpiTestReadResults

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_L1MUCTPITESTREADRESULTS_H
