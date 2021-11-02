/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBCELLCONTAINERFILLERTOOL_H
#define TBREC_TBCELLCONTAINERFILLERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloInterface/ICaloCellMakerTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "StoreGate/ReadCondHandleKey.h"

class MsgStream;
class CaloCell_ID;
class LArOnlineID;

class TBCellContainerFillerTool
  : public extends<AthAlgTool, ICaloCellMakerTool>
{
   public:    
        
        TBCellContainerFillerTool(const std::string& type, const std::string& name, 
                                  const IInterface* parent) ;

        virtual StatusCode initialize() override;

        // update theCellContainer
        virtual StatusCode process (CaloCellContainer* theCellContainer,
                                    const EventContext& ctx) const override;

    private:
	SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey { this
	    , "CaloDetDescrManager"
	    , "CaloDetDescrManager"
	    , "SG Key for CaloDetDescrManager in the Condition Store" };

        const CaloCell_ID * m_theCaloCCIDM{nullptr};
        const LArOnlineID* m_onlineHelper{nullptr};
        unsigned int m_hashMax{0};
};

#endif
