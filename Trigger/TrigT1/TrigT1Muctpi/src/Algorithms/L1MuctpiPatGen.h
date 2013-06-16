// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L1MuctpiPatGen.h 362102 2011-04-28 13:17:28Z krasznaa $
#ifndef TRIGT1MUCTPI_L1MUCTPIPATGEN_H
#define TRIGT1MUCTPI_L1MUCTPIPATGEN_H

// Athena/Gaudi include(s):
#include "AthenaBaseComps/AthAlgorithm.h"

namespace LVL1MUCTPI {

   /**
    *   $Date: 2011-04-28 15:17:28 +0200 (Thu, 28 Apr 2011) $
    *
    *   @short Pattern generator for the MuCTPI simulation
    *
    *          This algorithm can be used to test the functioning of the MuCTPI simulation in
    *          a stand-alone mode. (By stand-alone I mean inside Athena of course.)
    *
    *  @author $Author: krasznaa $
    * @version $Revision: 362102 $
    */
   class L1MuctpiPatGen : public AthAlgorithm {

   public:
      /// Regular Gaudi algorithm constructor
      L1MuctpiPatGen( const std::string& name, ISvcLocator* pSvcLocator );

      /// Regular Gaudi execute function
      virtual StatusCode execute();

   }; // class L1MuctpiPatGen

} // namespace LVL1MUCTPI

#endif // end of TRIGT1MUCTPI_L1MUCTPIPATGEN_H
