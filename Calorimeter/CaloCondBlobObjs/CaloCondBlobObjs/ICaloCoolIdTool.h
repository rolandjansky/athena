/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCONDBLOBOBJS_ICALOCOOLIDTOOL_H
#define CALOCONDBLOBOBJS_ICALOCOOLIDTOOL_H

/** 
    @brief abstract interface for Tool to associated cell offline identifiers with COOL identifiers
    
    @details Information associated with a given calorimeter cell is
    stored in different COOL channels depending on the calorimter
    subsystem the cell belongs to. A CaloCondBlob object is stored in
    each COOL channel holding the actual data. Information associated
    to a cell within a calo subsystem Blob is accessed using the
    subHash index.
*/

#include "GaudiKernel/IAlgTool.h"

class CaloCell_ID;

static const InterfaceID IID_ICaloCoolIdTool("ICaloCoolIdTool", 1 , 0);

class ICaloCoolIdTool : virtual public IAlgTool
{
 public:

  /** @brief Dtor. */
  virtual ~ICaloCoolIdTool() {};

  /** @brief Returns the cool channel id and subHash for a given calorimter cell.
      @param cellHash Calorimeter cell hash
      @param subHash The "channel" number of the cell in the CaloCondBlob */
  virtual unsigned int getCoolChannelId(unsigned int cellHash, unsigned int& subHash) const = 0;

  /** @brief returns tool Id */

  static const InterfaceID& interfaceID() { return IID_ICaloCoolIdTool; }
  
};

#endif
