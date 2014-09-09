/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETSTATUS_IDETSTATUSSVC_H
#define DETSTATUS_IDETSTATUSSVC_H
// IDetStatusSvc.h - abstract interface to service managing detector status
// Richard Hawkings, started 13/12/06

#include "GaudiKernel/IInterface.h"
#include "DetDescrConditions/DetStatusMap.h"

class IDetStatusSvc : virtual public IInterface {

 public:
  static const InterfaceID& interfaceID();

  // print all the detector status information
  virtual void print() const =0;

  // return a pointer to the DetStatus object with the given name
  // return 0 if there is none, or the DetStatus map cannot be found
  virtual const DetStatus* findStatus(const std::string& name) const =0;

  // return const iterators to the DetStatusMap, or null if it cannot be found
  virtual void getIter(DetStatusMap::const_iterator& begin,
		       DetStatusMap::const_iterator& end) const =0;

  // return true if the event is vetoed, according to the jo requirements
  virtual bool vetoed() const =0;

};

inline const InterfaceID& IDetStatusSvc::interfaceID() {
  static const InterfaceID IID_IDetStatusSvc("IDetStatusSvc",1,0);
  return IID_IDetStatusSvc;
}

#endif // DETSTATUS_IDETSTATUSSVC_H
