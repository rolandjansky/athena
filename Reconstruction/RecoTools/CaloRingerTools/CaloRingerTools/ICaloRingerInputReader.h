/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ICaloRingerInputReader.h 707325 2015-11-12 02:47:45Z wsfreund $
#ifndef CALORINGERTOOLS_ICALORINGERINPUTREADER
#define CALORINGERTOOLS_ICALORINGERINPUTREADER

/**
   @class ICaloRingerInputReader
   @brief Base Interface for CaloRinger particle reading.

   @author Werner S. Freund <wsfreund@cern.ch>

   $Revision: 707325 $
   $$$
*/

// STL Includes:
//#include <map>
#include <vector>
#include <memory>

// Core Includes:
#include "GaudiKernel/IAlgTool.h"

class CaloCellContainer;

namespace Ringer {

static const InterfaceID IID_ICaloRingerInputReader("ICaloRingerInputReader", 1, 0);

class ICaloRingerInputReader : virtual public IAlgTool
{
 public:

   //typedef std::map< 
   //        const xAOD::IParticle*, 
   //        const std::unique_ptr< xAOD::CaloRingsLinks > >
   //    decoMap_t;

  /** @brief Virtual destructor*/
  virtual ~ICaloRingerInputReader() {};
	
  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID();

  /** @brief initialize method*/
  virtual StatusCode initialize() = 0;
  /** @brief execute method **/                                                    
  virtual StatusCode execute() = 0;
  /** @brief finalize method*/
  virtual StatusCode finalize() = 0;

};

inline const InterfaceID& ICaloRingerInputReader::interfaceID()
{
  return IID_ICaloRingerInputReader;
}

} // namespace Ringer


#endif

