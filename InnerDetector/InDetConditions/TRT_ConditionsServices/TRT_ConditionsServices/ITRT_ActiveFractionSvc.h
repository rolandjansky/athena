/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRT_ActiveFractionSvc_H
#define ITRT_ActiveFractionSvc_H

/*----------------------------------------------------------
 *  @file ITRT_ActiveFractionSvc.h
 *  @Service Interface for TRT_ActiveFractionSvc
 *  @author Denver Whittington <Denver.Whittington@cern.ch>
 *  @author Sasa Fratina <Sasa.Fratina@cern.ch>
 *///-------------------------------------------------------

// Header Includes
#include "GaudiKernel/IService.h"
#include "TrkParameters/TrackParameters.h"

// Interface definition
class ITRT_ActiveFractionSvc : virtual public IService {

 public:

  /// Returns the fraction of active straws in an eta-phi bin
  virtual double getActiveFraction( const Trk::TrackParameters* ) = 0;

  /// Interface ID declaration
  static const InterfaceID& interfaceID();

};

/// Interface ID for this Service
inline const InterfaceID& ITRT_ActiveFractionSvc::interfaceID() {
  static const InterfaceID IID("ITRT_ActiveFractionSvc",1,0);
  return IID;
}

#endif // ITRT_ActiveFractionSvc_H
