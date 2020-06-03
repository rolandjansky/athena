// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1INTERFACES_MUCTPITOROIBSLINK_H
#define TRIGT1INTERFACES_MUCTPITOROIBSLINK_H

// STL etc.
#include <vector>

/**
 *  @short Dedicated namespace for the MuCTPI output class(es)
 *
 *         I guess the author of this file actually wanted to put the
 *         MuCTPIToRoIBSLink class into the LVL1MUONIF namespace, but
 *         made a big typo. However it happened, this namespace contains
 *         this single class at the moment...
 *
 * $Revision: 187728 $
 * $Date: 2009-05-27 18:18:06 +0200 (Wed, 27 May 2009) $
 */
namespace L1MUINT {

   /**
    *  @short MuCTPI input class to the RoIB simulation
    *
    *         This class is used to send the LVL1 muon data from the MuCTPI
    *         simulation to the RoIB simulation.
    *
    * $Revision: 187728 $
    * $Date: 2009-05-27 18:18:06 +0200 (Wed, 27 May 2009) $
    */
   class MuCTPIToRoIBSLink {

   public:
      MuCTPIToRoIBSLink( std::vector< unsigned int > );
      ~MuCTPIToRoIBSLink();

      const std::vector< unsigned int >& getMuCTPIToRoIBWords() const;

   private:
      std::vector< unsigned int > m_MuCTPIToRoIBSLinkVector;

   }; // class MuCTPIToRoIBSLink

} // namespace L1MUINT

#ifndef CLIDSVC_CLASSDEF_H
#include "CLIDSvc/CLASS_DEF.h"
#endif
CLASS_DEF( L1MUINT::MuCTPIToRoIBSLink, 6103, 0 )

#endif // TRIGT1INTERFACES_MUCTPITOROIBSLINK_H
