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

#include "AthenaKernel/getMessageSvc.h"
#include "LArRecEvent/LArCell.h"
#include "TileEvent/TileCell.h"

#include "AthAllocators/DataPool.h"
#include "CaloCompactCellTool.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloCellPacker_400_500.h"

CaloCompactCellTool::CaloCompactCellTool() {}

StatusCode CaloCompactCellTool::getTransient
(const CaloCompactCellContainer & theCompactContainer,
 CaloCellContainer * theCellContainer)
{
  const std::vector<CaloCompactCellContainer::value_type> theHeader = theCompactContainer.getHeader();
  //ATH_MSG_DEBUG( " getTransient:    using version: "<<theHeader[1]  );

  DataPool<LArCell> larCellsP(220000);//initialize for the default value will resize latter to full size
  DataPool<TileCell> tileCellsP(13000);

  switch (theHeader[1]) {

  case VERSION_400:
  case VERSION_500:
  case VERSION_501:
  case VERSION_502:
  case VERSION_503:
  case VERSION_504:
    {
      CaloCellPacker_400_500 packer;
      packer.unpack (theCompactContainer, theHeader, *theCellContainer,
                     larCellsP, tileCellsP);
    }
    break;

  default:
    MsgStream msg(Athena::getMessageSvc(), "CaloCompactCellTool");
    msg << " unknown version " << theHeader[1]
	<< " requested for unpacking the CaloCompactCellContainer" << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


StatusCode CaloCompactCellTool::getPersistent   //fill the CaloCompactCellContainer
(const CaloCellContainer & theCellContainer,
 CaloCompactCellContainer * theCompactContainer,
 int theVersion )
{
  //ATH_MSG_DEBUG( "CaloCell container contains " << theCellContainer.size() << " cells. Write compact Ver: " << theVersion  );

  if (theVersion == VERSION_LATEST)
    theVersion = VERSION_504;

 switch (theVersion ) {

 case VERSION_400:
 case VERSION_500:
 case VERSION_501:
 case VERSION_502:
 case VERSION_503:
 case VERSION_504:
   {
     CaloCellPacker_400_500 packer;

     packer.pack (theCellContainer, *theCompactContainer, theVersion);
   }
   break;

  default:
    MsgStream msg(Athena::getMessageSvc(), "CaloCompactCellTool");
    msg << " unknown version " << theVersion
	<< " requested for packing the CaloCellContainer" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
