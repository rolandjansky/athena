//Dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARCALIBUTILS_LARAUTOCORRDECODERTOOL_H
#define LARCALIBUTILS_LARAUTOCORRDECODERTOOL_H

#include "LArElecCalib/ILArAutoCorrDecoderTool.h"
#include "GaudiKernel/IIncidentListener.h"

#include "GaudiKernel/ToolHandle.h"

#include "LArElecCalib/ILArAutoCorr.h"
#include <Eigen/Dense>

#include "StoreGate/DataHandle.h"
#include "LArCabling/LArCablingService.h"
#include "LArCabling/LArSuperCellCablingTool.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"

#include "LArRawConditions/LArConditionsContainer.h"

#include "AthenaBaseComps/AthAlgTool.h"

class StoreGateSvc;
class LArCablingService;

class LArAutoCorrDecoderTool: public AthAlgTool,
			      virtual public ILArAutoCorrDecoderTool

{
 public:
  
  // constructor
  LArAutoCorrDecoderTool(const std::string& type, 
			 const std::string& name, 
			 const IInterface* parent); 
  
  // destructor 
  virtual ~LArAutoCorrDecoderTool();
  
  // retrieve methods 
  const Eigen::MatrixXd AutoCorr( const HWIdentifier&  CellID, int gain, unsigned nSamples) const;
  const Eigen::MatrixXd AutoCorr( const Identifier&  CellID, int gain, unsigned nSamples) const;

  // initialize and finalize methods
  virtual StatusCode initialize();
  virtual StatusCode finalize(){return StatusCode::SUCCESS;}

  static const InterfaceID& interfaceID() { 
    return ILArAutoCorrDecoderTool::interfaceID();
  } 

 private:

  unsigned m_decodemode;

  bool m_alwaysHighGain;

  const Eigen::MatrixXd ACDiagonal( const HWIdentifier&  CellID, int gain, unsigned nSamples) const;
  const Eigen::MatrixXd ACPhysics( const HWIdentifier&  CellID, int gain, unsigned nSamples) const;

  const LArOnlineID_Base*  m_onlineID;
  LArCablingBase*    m_cablingService;

  std::string m_keyAutoCorr;

  const DataHandle<ILArAutoCorr> m_autoCorr;

  // Running on cells or supercells?
  bool m_isSC;
};

#endif
