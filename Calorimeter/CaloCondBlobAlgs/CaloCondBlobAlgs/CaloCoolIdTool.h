/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCONDBLOBALGS_CALOCOOLIDTOOL_H
#define CALOCONDBLOBALGS_CALOCOOLIDTOOL_H

/** 
    @brief Tool to associated cell offline identifiers with COOL identifiers
    @author Nils Gollub <nils.gollub@cern.ch>
    
    @details Information associated with a given calorimeter cell is
    stored in different COOL channels depending on the calorimter
    subsystem the cell belongs to. A CaloCondBlob object is stored in
    each COOL channel holding the actual data. Information associated
    to a cell within a calo subsystem Blob is accessed using the
    subHash index.
*/

//#include "GaudiKernel/AlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloCondBlobObjs/ICaloCoolIdTool.h"

class CaloCell_ID;

class CaloCoolIdTool : public AthAlgTool, virtual public ICaloCoolIdTool
{
 public:
  /** @brief Ctor. */
  CaloCoolIdTool(const std::string& type, const std::string& name, const IInterface* parent);
  /** @brief Dtor. */
  virtual ~CaloCoolIdTool();
  /** @brief Returns the tool's InterfaceID. */
  static const InterfaceID& interfaceID() {return ICaloCoolIdTool::interfaceID();}

  /** @brief Initializes the tool. */
  StatusCode initialize();

  /** @brief Returns the cool channel id and subHash for a given calorimter cell.
      @param cellHash Calorimeter cell hash
      @param subHash The "channel" number of the cell in the CaloCondBlob */
  unsigned int getCoolChannelId(unsigned int cellHash, unsigned int& subHash) const;

  
 private:
  const CaloCell_ID* m_calo_id;
};

#endif
