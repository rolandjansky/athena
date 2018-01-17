// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: QuadrupletContainer.h 485033 2012-02-24 16:54:50Z krasznaa $
#ifndef HSG2Event_QuadrupletContainer_H
#define HSG2Event_QuadrupletContainer_H

// Gaudi/Athena include(s):
#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/BaseInfo.h"

// Local include(s):
#include "Quadruplet.h"

/**
 *  @class QuadrupletContainer
 *  @short Container for reconstructed quadruplets
 *
 *         Such containers can be put into StoreGate to hold the
 *         4-lepton candidates that were reconstructed by some algorithm.
 *
 * @author Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 485033 $
 * $Date: 2012-02-24 17:54:50 +0100 (Fri, 24 Feb 2012) $
 */
class QuadrupletContainer : public DataVector< HSG2::Quadruplet > {

public:
   /// Constructor with ownership policy
   QuadrupletContainer( SG::OwnershipPolicy own = SG::OWN_ELEMENTS )
      : DataVector< HSG2::Quadruplet >( own ) {} 

}; // class QuadrupletContainer

CLASS_DEF( QuadrupletContainer, 1146749485, 1 )
SG_BASE( QuadrupletContainer, DataVector<HSG2::Quadruplet> );

#endif // HSG2Event_QuadrupletContainer_H
