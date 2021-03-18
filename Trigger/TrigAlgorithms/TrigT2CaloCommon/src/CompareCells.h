
/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 *   */
#ifndef TrigT2CaloCommon_CompareCells_h
#define TrigT2CaloCommon_CompareCells_h

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrigT2CaloCommon/ITrigCaloDataAccessSvc.h"
#include "StoreGate/ReadHandleKey.h"
#include "CaloEvent/CaloBCIDAverage.h"
#include "CaloEvent/CaloCellContainer.h"

class CompareCells : public ::AthReentrantAlgorithm
{
	public:
	CompareCells( const std::string& name, ISvcLocator* pSvcLocator );
	virtual StatusCode initialize() override;
	virtual StatusCode execute(const EventContext& context) const override;

	private:
	ServiceHandle<ITrigCaloDataAccessSvc> m_dataAccessSvc;

	SG::ReadHandleKey<CaloCellContainer> m_calocellcollectionKey ;

	SG::ReadHandleKey<CaloBCIDAverage> m_bcidAvgKey {
        this, "BCIDAvgKey", "CaloBCIDAverage", "" };

};
#endif
