/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALODETDESCRUTILS_CALODETDESCRBUILDER_H
#define CALODETDESCRUTILS_CALODETDESCRBUILDER_H

#include <memory>

class CaloDetDescrManager;
class ISvcLocator;
class IMessageSvc;
class GeoAlignmentStore;
namespace CaloRec {
  class CaloCellPositionShift;
}

std::unique_ptr<CaloDetDescrManager> buildCaloDetDescr(ISvcLocator* svcLocator
						       , IMessageSvc* msgSvc
						       , const GeoAlignmentStore* geoAlignStore
						       , const CaloRec::CaloCellPositionShift* cellPosShift);

#endif
