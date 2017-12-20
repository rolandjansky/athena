/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONIDCUTTOOL_H
#define MUON_IMUONIDCUTTOOL_H
 
#include "GaudiKernel/IAlgTool.h"
#include <vector>
#include "MuonCalibTools/IdToFixedIdTool.h"
#include "MuonCalibIdentifier/MuonFixedId.h"
#include "Identifier/Identifier.h"



static const InterfaceID IID_IMuonIdCutTool("IMuonIdCutTool",1,0);




  /** @brief The IMuonIdCutTool is a virtual interface for a practice tool which will cut on an muon ID  */  
class IMuonIdCutTool : virtual public IAlgTool 
{      
 public:
  /** access to tool interface */
  static const InterfaceID& interfaceID();

  virtual bool isCut(Identifier ID) const = 0;
  virtual bool isCut(MuonCalib::MuonFixedId id) const = 0;
  
  
};

inline const InterfaceID& IMuonIdCutTool::interfaceID()
{
  return IID_IMuonIdCutTool;
}



#endif 

