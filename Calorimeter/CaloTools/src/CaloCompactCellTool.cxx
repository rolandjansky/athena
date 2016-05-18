/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: CaloCompactCellTool.cxx,v 1.35 2009-03-19 01:42:15 ssnyder Exp $
//
// Description: see CaloCompactCellTool.h
//
// Environment:
//      Software developed for the ATLAS Detector at the CERN LHC
//
// All compactor versions before version 400 are removed and can be found only in
// old versions in CVS
//-----------------------------------------------------------------------

#include "LArRecEvent/LArCell.h"
#include "TileEvent/TileCell.h"

#include "AthAllocators/DataPool.h"
#include "CaloTools/CaloCompactCellTool.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloTools/CaloCellPacker_400_500.h"
#include "GaudiKernel/MsgStream.h"

CaloCompactCellTool::CaloCompactCellTool(const std::string& type,
					 const std::string& name,
					 const IInterface* parent)
  : AthAlgTool(type,name,parent)
{
declareInterface< ICaloCompactCellTool >( this );
}

CaloCompactCellTool::~CaloCompactCellTool()
{ }

StatusCode CaloCompactCellTool::initialize()
{
  declareInterface<ICaloCompactCellTool>(this);
  return StatusCode::SUCCESS;
}

StatusCode CaloCompactCellTool::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode CaloCompactCellTool::getTransient
(const CaloCompactCellContainer & theCompactContainer,
 CaloCellContainer * theCellContainer)
{
  MsgStream logStream(msgSvc(), name());

  const std::vector<CaloCompactCellContainer::value_type> theHeader = theCompactContainer.getHeader();
  logStream << MSG::DEBUG << " getTransient:    using version: "<<theHeader[1] << endreq;

  DataPool<LArCell> larCellsP(220000);//initialize for the default value will resize latter to full size
  DataPool<TileCell> tileCellsP(13000);

  switch (theHeader[1]) {

  case ICaloCompactCellTool::VERSION_400:
  case ICaloCompactCellTool::VERSION_500:
  case ICaloCompactCellTool::VERSION_501:
  case ICaloCompactCellTool::VERSION_502:
  case ICaloCompactCellTool::VERSION_503:
  case ICaloCompactCellTool::VERSION_504:
    {
      CaloCellPacker_400_500 packer;
      packer.unpack (theCompactContainer, theHeader, *theCellContainer,
                     larCellsP, tileCellsP);
    }
    break;

  default:
    logStream << MSG::FATAL << " unknown version " << theHeader[1]
	      << " requested for unpacking the CaloCompactCellContainer"
	      << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


StatusCode CaloCompactCellTool::getPersistent   //fill the CaloCompactCellContainer
(const CaloCellContainer & theCellContainer,
 CaloCompactCellContainer * theCompactContainer,
 int theVersion )
{
  MsgStream logStream(msgSvc(), name());

  logStream << MSG::DEBUG << "CaloCell container contains " << theCellContainer.size() << " cells. Write compact Ver: " << theVersion <<endreq;

  if (theVersion == ICaloCompactCellTool::VERSION_LATEST)
    theVersion = ICaloCompactCellTool::VERSION_504;

 switch (theVersion ) {

 case ICaloCompactCellTool::VERSION_400:
 case ICaloCompactCellTool::VERSION_500:
 case ICaloCompactCellTool::VERSION_501:
 case ICaloCompactCellTool::VERSION_502:
 case ICaloCompactCellTool::VERSION_503:
 case ICaloCompactCellTool::VERSION_504:
   {
     CaloCellPacker_400_500 packer;

     packer.pack (theCellContainer, *theCompactContainer, theVersion);
   }
   break;

  default:
    logStream << MSG::FATAL << " unknown version " << theVersion
	      << " requested for packing the CaloCellContainer" << endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
