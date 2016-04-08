/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ICaloRingerPhotonsReader.h 667886 2015-05-18 17:26:59Z wsfreund $
#ifndef CALORINGERTOOLS_ICALORINGERPHOTONSREADER
#define CALORINGERTOOLS_ICALORINGERPHOTONSREADER

/**
   @class ICaloRingerPhotonsReader
   @brief Interface for tool CaloRingerPhotonsReader

   @author Werner S. Freund <wsfreund@cern.ch>

   $Revision: 667886 $
   $$$
*/

// Core Include
#include "GaudiKernel/IAlgTool.h"

// Interface Includes:
#include "ICaloRingerInputReader.h"

namespace Ringer {

static const InterfaceID IID_ICaloRingerPhotonsReader("ICaloRingerPhotonsReader", 1, 0);

class ICaloRingerPhotonsReader : virtual public ICaloRingerInputReader
{
 public:
  /** @brief Virtual destructor*/
  virtual ~ICaloRingerPhotonsReader() {};
	
  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID();

  /** @brief initialize method*/
  virtual StatusCode initialize() = 0;
  /** @brief execute method **/                                                    
  virtual StatusCode execute() = 0;
  /** @brief finalize method*/
  virtual StatusCode finalize() = 0;

};

inline const InterfaceID& ICaloRingerPhotonsReader::interfaceID()
{
  return IID_ICaloRingerPhotonsReader;
}

} // namespace Ringer

#endif

