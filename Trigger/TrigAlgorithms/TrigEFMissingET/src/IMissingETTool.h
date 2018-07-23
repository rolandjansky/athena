/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEFMISSINGET_IMISSINGETTOOL_H
#define TRIGEFMISSINGET_IMISSINGETTOOL_H 1

#include "GaudiKernel/IAlgTool.h"

#include "TrigEFMissingET/EFMissingETHelper.h"
#include "xAODTrigMissingET/TrigMissingET.h"

/**
 * @class Adds info the the missing ET object
 * @brief Specific implementations may use various input data types
 **/

class IMissingETTool : virtual public ::IAlgTool { 
  
public: 
  DeclareInterfaceID( IMissingETTool, 1, 0 );  
  virtual ~IMissingETTool(){}

  /**
   * a method to update the met object (and met helper object)
   * The API deliberately does not include EventContext as it is only needed 
   * in one implementation when input is not taken from the regular store. 
   **/
  virtual StatusCode update( xAOD::TrigMissingET *met,
			     TrigEFMissingEtHelper *metHelper ) const = 0;

}; 



#endif //> !TRIGEFMISSINGET_IMISSINGETTOOL_H
