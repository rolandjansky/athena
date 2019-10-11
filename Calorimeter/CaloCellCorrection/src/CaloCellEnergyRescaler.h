//Dear emacs, this is -*-c++-*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef CALOCELLENERGYRESCALER_H
#define CALOCELLENERGYRESCALER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloInterface/ICaloCellMakerTool.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"  
#include "StoreGate/ReadHandle.h"

class CaloCellContainer;

/** @class CaloCellEnergyRescaler
    @brief CaloCellMakerTool to re-scale cell energies.
*/

class CaloCellEnergyRescaler
  : public extends<AthAlgTool, ICaloCellMakerTool>
{
public:
  
  CaloCellEnergyRescaler (const std::string& type, const std::string& name, 
			  const IInterface* parent);

  ~CaloCellEnergyRescaler();
  virtual StatusCode initialize() override;

  virtual StatusCode process (CaloCellContainer* theCellContainer,
                              const EventContext& ctx) const override;

private: 
  SG::ReadCondHandleKey<AthenaAttributeList> m_eneShiftFldr{this,"Folder","/CALO/Ofl/CellEnergyCorr","Key (=foldername) of AttrListCollection"};
  
};

#endif     
