/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_STOREID_H
#define ATHENAKERNEL_STOREID_H
// $Id: StoreID.h,v 1.4 2008-01-04 21:40:11 gemmeren Exp $

/** 
 * @class StoreID
 * @brief defines an enum used by address providers to decide what kind of 
 * StoreGateSvc they are providing addresses for. A hack, really...
 */

#include <string>

class StoreID
{

 public:

  // DEFINITIONS HERE MUST BE KEPT SYNCHRONIZED WITH AthenaKernel/python/StoreID.py.

  typedef enum {

    EVENT_STORE = 0,
    DETECTOR_STORE,
    CONDITION_STORE,
    METADATA_STORE,
    SIMPLE_STORE,
    SPARE_STORE,
    PILEUP_STORE,
    UNKNOWN
  } type;

public:
  static StoreID::type findStoreID(const std::string& storeName);
  static const std::string& storeName( const StoreID::type& s );

};

#endif
