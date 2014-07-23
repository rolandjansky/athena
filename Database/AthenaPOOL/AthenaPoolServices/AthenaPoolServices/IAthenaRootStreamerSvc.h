/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  ATHENAPOOLSERVICES_IATHENAROOTSTREAMERSVC_H
#define  ATHENAPOOLSERVICES_IATHENAROOTSTREAMERSVC_H

/**
 * @file IAthenaRootStreamerSvc.h
 * 
 * @brief Interface to the ROOT Streamer Service
 * 
 * @author Marcin Nowak
 * 
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "GaudiKernel/IService.h"

#include <string>

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class T_AthenaRootConverterBase;

/** 
 ** @class IAthenaRootStreamerSvc
 ** 
 ** @brief Gaudi style interface to Athena ROOT streamer service
 **  
 **    Properties:
 **
 **/

static const InterfaceID IID_IAthenaRootStreamerSvc("IAthenaRootStreamerSvc", 2 , 0); 


class IAthenaRootStreamerSvc : virtual public IService
{
public:    
  
  /// Retrieve interface ID
  static const InterfaceID& interfaceID();

  /// Create (or exetend) ROOT streamer
  /// Class name for which the streamer is added is extracted from the converter
  /// One class has one streamer and possibly more than one converter
  /// @param converter_classname [IN] classname of the converter object
  /// @param adopt [IN] should the streamer be enabled now or wait for later Adopt() 
  virtual StatusCode AddStreamer(const std::string& converter_classname, bool adopt=true) = 0;  

  /// Create (or exetend) ROOT streamer
  /// Class name for which the streamer is added is extracted from the converter
  /// One class has one streamer and possibly more than one converter
  /// @param converter [IN] pointer to a converter object
  /// @param adopt [IN] should the streamer be enabled now or wait for later Adopt() 
  virtual StatusCode AddStreamer(T_AthenaRootConverterBase *converter, bool adopt=true) = 0;
  
  /// Adopt ROOT custom streamer for a given class.
  /// @param class_name [IN] class name for which the streamer is adopted.
  virtual StatusCode AdoptStreamerForClass(const std::string& class_name) = 0;

  /// Adopt all ROOT streamers known to the service
  virtual StatusCode AdoptAllStreamers() = 0;
};



inline const InterfaceID& 
IAthenaRootStreamerSvc::interfaceID() 
{
    return IID_IAthenaRootStreamerSvc; 
}

 

#endif 
