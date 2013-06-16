// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: SectorConstants.h 362102 2011-04-28 13:17:28Z krasznaa $
#ifndef TRIGT1MUCTPI_SECTORCONSTANTS_H
#define TRIGT1MUCTPI_SECTORCONSTANTS_H

namespace LVL1MUCTPI {

  static const unsigned int MIN_BARREL_SECTOR = 0;
  static const unsigned int MAX_BARREL_SECTOR = 31;
  static const unsigned int MIN_BARREL_ROI = 1;
  static const unsigned int MAX_BARREL_ROI = 26;

  static const unsigned int MIN_ENDCAP_SECTOR = 0;
  static const unsigned int MAX_ENDCAP_SECTOR = 47;
  static const unsigned int MIN_ENDCAP_ROI = 0;
  static const unsigned int MAX_ENDCAP_ROI = 147;

  static const unsigned int MIN_FORWARD_SECTOR = 0;
  static const unsigned int MAX_FORWARD_SECTOR = 23;
  static const unsigned int MIN_FORWARD_ROI = 0;
  static const unsigned int MAX_FORWARD_ROI = 63;

  static const unsigned int MAX_NUMBER_OF_THRESHOLDS = 6;
  static const unsigned int MAX_MULTIPLICITY = 7;

  static const unsigned int SECTORS_IN_MIOCT = 13;

}

#endif // TRIGT1MUCTPI_SECTORCONSTANTS_H
