/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CaloBadChanTool_H
#define CaloBadChanTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloConditions/ICaloBadChanTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloIdentifier/CaloCell_ID.h"

//static const InterfaceID IID_CaloBadChanTool("CaloBadChanTool", 1 , 0); 

class CaloBadChanTool : public AthAlgTool,
			public ICaloBadChanTool {
public:

  CaloBadChanTool(const std::string& type, const std::string& name, 
		  const IInterface* parent);

  virtual ~CaloBadChanTool();
					  
  virtual StatusCode initialize();

  virtual CaloBadChannel caloStatus( Identifier id) const;


private:

  ToolHandle<ICaloBadChanTool> m_larBCT;
  ToolHandle<ICaloBadChanTool> m_tileBCT;
  const CaloCell_ID* m_caloID;
};

#endif
