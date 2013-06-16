// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Detector.h 362102 2011-04-28 13:17:28Z krasznaa $
#ifndef TRIGT1MUCTPI_DETECTOR_H
#define TRIGT1MUCTPI_DETECTOR_H

namespace LVL1MUCTPI {

  /**
  *******************************************************************
  *
  *    $Date: 2011-04-28 15:17:28 +0200 (Thu, 28 Apr 2011) $
  *
  *    @short Definition of the detector types and Hemispheres
  *
  *           This Header file defines the different detetcor types
  *           and Hemispheres which exists in the Muon Trigger system
  *           of ATLAS
  *
  *   @author  Author: Thorsten Wengler
  *  @version $Revision: 362102 $
  *
  *******************************************************************
  */

  /// Detector type enumeration
  /**
  * This enumeration is used to identify the detector type of a given muon
  * candidate. The ATLAS muon system is divided into 3 parts: barrel,
  * endcap and forward.
  */
  enum Detector { BARREL = 0, ENDCAP = 1, FORWARD = 2 };
  /// Detector hemisphere enumeration
  /**
  * This enumeration is used to identify which half of the detector a given
  * muon candidate is coming from. The values POSITIVE and NEGATIVE state
  * the sign of eta in the given part of the detector.
  */
  enum Hemisphere { POSITIVE = 1, NEGATIVE = 0 };

}

#endif // TRIGT1MUCTPI_DETECTOR_H
