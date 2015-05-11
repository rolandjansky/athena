/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLTNETTHISTSVC_H
#define HLTNETTHISTSVC_H

/**
 * @file IHltTHistSvc
 * @author Tomasz Bold
 * @date 2005-09-27
 * @brief The extension of the histogramming service THistSvc
 * This interface adds methods used to handle communication.
 * Actually the send method is suppose to handle safely TTrees.
 */
#include "GaudiKernel/IService.h"

//class StatusCode;

static const InterfaceID IID_IHltTHistSvc("IHltTHistSvc", 1 , 0);

class IHltTHistSvc : virtual public IService {
public:
  static const InterfaceID& interfaceID(){return  IID_IHltTHistSvc;};
  /**
   * method which can be used to connet to 
   * histograms sink (do we need it since IService hast such calls?)
   */
  virtual StatusCode connect() = 0;
  /**
   * this method is called each event 
   */
  virtual StatusCode send() = 0;
  /**
   * this method is called at the finalization of the 
   * service (do we need it since IService hast such calls?)
   */
  virtual StatusCode disconnect() = 0;
};


#endif // IHLTNETTHISTSVC_H
