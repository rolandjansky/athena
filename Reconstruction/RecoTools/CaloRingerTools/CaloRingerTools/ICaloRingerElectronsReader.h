/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ICaloRingerElectronsReader.h 667886 2015-05-18 17:26:59Z wsfreund $
#ifndef CALORINGERTOOLS_ICALORINGERELECTRONSREADER
#define CALORINGERTOOLS_ICALORINGERELECTRONSREADER

/**
   @class ICaloRingerElectronsReader
   @brief Interface for tool CaloRingerElectronsReader

   @author Werner S. Freund <wsfreund@cern.ch>

   $Revision: 667886 $
   $$$
*/

// Core Include
#include "GaudiKernel/IAlgTool.h"

// Interface Includes:
#include "ICaloRingerInputReader.h"


namespace Ringer {

static const InterfaceID IID_ICaloRingerElectronsReader("ICaloRingerElectronsReader", 1, 0);

class ICaloRingerElectronsReader : virtual public ICaloRingerInputReader
{
 public:
  /** @brief Virtual destructor*/
  virtual ~ICaloRingerElectronsReader() {};
	
  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID();

  /** @brief initialize method*/
  virtual StatusCode initialize() = 0;
  /** @brief execute method **/                                                    
  virtual StatusCode execute() = 0;
  /** @brief finalize method*/
  virtual StatusCode finalize() = 0;

};

inline const InterfaceID& ICaloRingerElectronsReader::interfaceID()
{
  return IID_ICaloRingerElectronsReader;
}

} // namespace Ringer

#endif

