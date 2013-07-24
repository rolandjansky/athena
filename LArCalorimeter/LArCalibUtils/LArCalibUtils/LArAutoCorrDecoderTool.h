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
#include "LArTools/LArCablingService.h"
#include "LArIdentifier/LArOnlineID.h"

#include "LArRawConditions/LArConditionsContainer.h"

#include "AthenaBaseComps/AthAlgTool.h"

class StoreGateSvc;
class LArCablingService;

class LArAutoCorrDecoderTool: public AthAlgTool,
			      virtual public ILArAutoCorrDecoderTool,
			      public IIncidentListener

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

  //IOV Callback functions
  virtual StatusCode LoadAutoCorr(IOVSVC_CALLBACK_ARGS);
  virtual void handle(const Incident&);

  static const InterfaceID& interfaceID() { 
    return ILArAutoCorrDecoderTool::interfaceID();
  } 

 private:

  unsigned m_decodemode;

  const Eigen::MatrixXd ACDiagonal( const HWIdentifier&  CellID, int gain, unsigned nSamples) const;
  const Eigen::MatrixXd ACPhysics( const HWIdentifier&  CellID, int gain, unsigned nSamples) const;

  const LArOnlineID*  m_lar_on_id;
  ToolHandle<LArCablingService> m_cablingService;

  std::string m_keyAutoCorr;
  bool m_loadAtBegin;

  const DataHandle<ILArAutoCorr> m_AutoCorr;

  mutable bool m_cacheValid;

};

#endif
