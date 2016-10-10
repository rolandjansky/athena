/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGKERNEL_ITRIGIS_H
#define TRIGKERNEL_ITRIGIS_H

/**@class ITrigIS
 * @brief Interface for IS publication service
 *
 * @author  Frank Winklmeier
 * @version $Id: ITrigIS.h 2 2013-05-10 13:12:14Z ricab $
 */
  
#include "GaudiKernel/IInterface.h"
#include <string>

class ISInfo;
class ISInfoDictionary;

class ITrigIS: virtual public IInterface {
public:
  /*
   * Retrieve interface ID
   */
  static const InterfaceID& interfaceID();

  /*
   * Publish ISInfo object
   *
   * Forwards to IsInfoDictionary::checkin and catches exceptions.
   * Ownership of info remains with caller.
   */
  virtual StatusCode publish(const std::string& name, const ISInfo& info, bool keep_history=false) = 0;
  
  /*
   * Same as publish but might publish ISInfo object asynchronously.
   * Ownership of info is transfered to this service.
   */
  virtual void publish_async(const std::string& name, ISInfo* info, bool keep_history=false) = 0;

  /*
   * Return reference to ISInfoDictionary
   */
  virtual ISInfoDictionary& infoDict() = 0;
};


inline const InterfaceID& ITrigIS::interfaceID()
{
  static const InterfaceID IID_ITrigIS("ITrigIS", 1, 0);
  return IID_ITrigIS;
}

#endif
