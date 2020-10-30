/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class LArStrHelper
 * @author Margherita Spalla <margherita.spalla@cern.ch>
 * @date 30-September-2020
 * @brief Helper class to manipulate strings for LArOnlineID names
 *
 * Helper class to manipulate strings for LArOnlineID names:
 * Since most of the naming is done at python level in AthenaMT, we only add strictly needed functions.
 * For now, it just contains the FEB naming scheme used by LArNoiseCorrelations. */



#ifndef LARMONITORING_LARSTRHELPER_H
#define LARMONITORING_LARSTRHELPER_H


#include "TString.h"

class LArStrHelper
{
 public:

  /** @brief constructructor */
  LArStrHelper() {};

  /** @brief default destructor */
  ~LArStrHelper() {};

  /** FEB names */
  std::string febNameString(bool,int,int,int) const;
  std::string fixFEBname(TString) const;
};



#endif
