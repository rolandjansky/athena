/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CaloBadChanTool_H
#define CaloBadChanTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloConditions/ICaloBadChanTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArRecConditions/LArBadChannelCont.h"


//static const InterfaceID IID_CaloBadChanTool("CaloBadChanTool", 1 , 0); 

class CaloBadChanTool : public AthAlgTool,
			public ICaloBadChanTool {
public:

  CaloBadChanTool(const std::string& type, const std::string& name, 
		  const IInterface* parent);

  virtual ~CaloBadChanTool() override;
					  
  virtual StatusCode initialize() override;

  using ICaloBadChanTool::caloStatus;
  virtual CaloBadChannel caloStatus(const EventContext& ctx,
                                    Identifier id) const override;

private:
  SG::ReadCondHandleKey<LArBadChannelCont> m_larBCKey{
    this,
    "LArBadChanKey",
    "LArBadChannel",
    "LAr bad channel SG key"
  };
  ToolHandle<ICaloBadChanTool> m_tileBCT{ this,
                                          "TileBadChanTool",
                                          "TileBadChanTool",
                                          "Tile bad channel tool" };
  const CaloCell_ID* m_caloID;
};

#endif
