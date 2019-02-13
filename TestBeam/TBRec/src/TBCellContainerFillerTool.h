/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBCELLCONTAINERFILLERTOOL_H
#define TBREC_TBCELLCONTAINERFILLERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
//#include "CaloUtils/ICaloCellMakerTool.h"
#include "CaloInterface/ICaloCellMakerTool.h"
#include "GaudiKernel/ToolHandle.h"

class StoreGateSvc; 
class MsgStream;
class CaloDetDescrManager;
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
        const CaloDetDescrManager *m_theCaloDDM;
        const CaloCell_ID * m_theCaloCCIDM;
        const LArOnlineID* m_onlineHelper;
        unsigned int m_hashMax;
};

#endif
