/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOINTERFACE_ICOMPACTCELLTOOL_H
#define CALOINTERFACE_ICOMPACTCELLTOOL_H

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: ICaloCompactCellTool.h,v 1.2 2009-03-19 01:32:53 ssnyder Exp $
//
// Description: Tool to convert from CaloCellContainer to
//              CaloCompactCellContainer and back
//
// Environment:
//      Software developed for the ATLAS Detector at the CERN LHC
//
// Author List:
//      Sven Menke
//
//-----------------------------------------------------------------------
// Includes for Gaudi
#include "GaudiKernel/IAlgTool.h"

class CaloCellContainer;
class CaloCompactCellContainer;

static const InterfaceID IID_ICaloCompactCellTool("ICaloCompactCellTool", 1 ,0);

class ICaloCompactCellTool: virtual public IAlgTool
{

  public:

  enum KNOWN_VERSIONS {
    VERSION_400 = 400,
    VERSION_500 = 500,
    VERSION_501 = 501,
    VERSION_502 = 502,
    VERSION_503 = 503,
    VERSION_504 = 504,

    // Select the latest version.
    VERSION_LATEST = -1
  };

  static const InterfaceID& interfaceID() { return IID_ICaloCompactCellTool; }

  virtual StatusCode getTransient(const CaloCompactCellContainer & theCompactContainer,
				  CaloCellContainer * theCellContainer)=0;
  // fills a CaloCellContainer ; caller has the responsibility
  // of creating and deleting the object again

  virtual StatusCode getPersistent(const CaloCellContainer & theCellContainer,
				   CaloCompactCellContainer * theCompactContainer,
  				   const int theVersion = VERSION_LATEST)=0;
  // fills a CaloCompactCellContainer ; caller has the responsibility
  // of creating and deleting the object again

};

#endif
