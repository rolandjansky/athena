/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOREC_TBCellContainerFillerTool_H
#define CALOREC_TBCellContainerFillerTool_H

#include "AthenaBaseComps/AthAlgTool.h"
//#include "CaloUtils/ICaloCellMakerTool.h"
#include "CaloInterface/ICaloCellMakerTool.h"
#include "GaudiKernel/ToolHandle.h"

class StoreGateSvc; 
class MsgStream;
class CaloDetDescrManager;
class CaloCell_ID;
class LArCablingService;
class LArOnlineID;

class TBCellContainerFillerTool: public AthAlgTool, virtual public ICaloCellMakerTool 
{
    
   public:    
        
        TBCellContainerFillerTool(const std::string& type, const std::string& name, 
                                  const IInterface* parent) ;

        virtual StatusCode initialize() ; 

        // update theCellContainer
        virtual StatusCode process( CaloCellContainer * theCellContainer) ;

    private:
        const CaloDetDescrManager *m_theCaloDDM;
        const CaloCell_ID * m_theCaloCCIDM;
        ToolHandle<LArCablingService> m_cablingService;
        const LArOnlineID* m_onlineHelper;
        unsigned int m_hashMax;
};

#endif
