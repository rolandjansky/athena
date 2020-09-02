/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_STOREGATE_H
#define STOREGATE_STOREGATE_H

/** @class StoreGate
 *  @brief a multipleton that provides access to StoreGateSvc instances
 *  @author ATLAS Collaboration
 *  $Id: StoreGate.h,v 1.10 2003-04-16 01:58:11 calaf Exp $
 **/

#include "StoreGate/StoreGateSvc.h"
#include <string>

class NullType;
class ActiveStoreSvc;

class StoreGate {
public:
  /// returns active store ptr (see ActiveStoreSvc). Can be slow: use sparingly
  static StoreGateSvc* pointer();
  /// returns active store ref (see ActiveStoreSvc). Can be slow: use sparingly
  static StoreGateSvc& instance();
  /// returns ptr to ActiveStoreSvc. Cache it and use it to access active store
  /// multiple times
  static ActiveStoreSvc* activeStoreSvc();
  /** multipleton: get a store by name
   * @param sgID name of the StoreGateSvc ptr to be returned */ 
  static StoreGateSvc* pointer(const std::string& sgID); 
  /** multipleton: get a store by name
   *  @param sgID name of the StoreGateSvc ptr to be returned 
   *  @throws std::runtime_error if not found*/ 
  static StoreGateSvc& instance(const std::string& sgID);
  
  friend class NullType; //remove compiler warning

protected:
  ~StoreGate(); //FIXME avoid compiler warning for private
private:
  //standard singleton stuff
  StoreGate();
  StoreGate(StoreGate&);
  StoreGate& operator=(const StoreGate&);
};
#endif // STOREGATE_STOREGATE_H





