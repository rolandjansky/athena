/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PrimaryDPDMaker_CaloTimeFilterTool_H
#define PrimaryDPDMaker_CaloTimeFilterTool_H

/** @file CaloTimeFilterTool.h
 *  @brief This file contains the class definition for the CaloTimeFilterTool class.
 *  @author Max Baak <mbaak@cern.ch>
 **/

#include "AthenaBaseComps/AthAlgTool.h"

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"

/** @class CaloTimeFilterTool
 *  @brief This file contains the class definition for the CaloTimeFilterTool class.
 **/

static const InterfaceID IID_CaloTimeFilterTool("CaloTimeFilterTool", 1, 0);

class CaloTimeFilterTool : public AthAlgTool
{
 public:    
  CaloTimeFilterTool( const std::string&, const std::string&, const IInterface* );
  virtual ~CaloTimeFilterTool();

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_CaloTimeFilterTool; };

  /** Overriding initialize and finalize */
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual StatusCode getTimeDifference(bool& passCut, double& timeDiff, double& timeA, double&timeC, int& ncellA, int& ncellC);

 protected:

  //---------------------------------------------------
  // Member variables
  //---------------------------------------------------

  int   m_nevt;
  float m_timeCut;
  int   m_mincellsperside;
};

#endif

