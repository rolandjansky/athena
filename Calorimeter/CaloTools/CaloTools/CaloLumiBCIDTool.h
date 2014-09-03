/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

/** 
    @class CaloLumiBCIDTool
    @brief Tool to provide expected pedestal shift for Pileup from first principle computation (relevant for non 25ns bunch spacing)
    @author G.Unal
*/

#ifndef CALOTOOLS_CaloLumiBCIDTool_H
#define CALOTOOLS_CaloLumiBCIDTool_H

class Identifier; 
class StoreGateSvc; 
class CaloIdManager;
class CaloCell_ID;
class CaloDetDescrElement;

#include "LArElecCalib/ILArShape.h"
#include "LArElecCalib/ILArMinBiasAverage.h"
#include "LArElecCalib/ILArOFC.h"
#include "LArElecCalib/ILArOFCTool.h"
#include "LArElecCalib/ILArMCSymTool.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArTools/LArCablingService.h"

#include "LumiBlockComps/ILuminosityTool.h"
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"
#include "CaloInterface/ICaloLumiBCIDTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"


class CaloLumiBCIDTool: public AthAlgTool,
			virtual public ICaloLumiBCIDTool, virtual public IIncidentListener
{
private: 
  
//Database  

  const DataHandle<ILArShape> m_dd_shape;
  const DataHandle<ILArMinBiasAverage> m_dd_minbiasAverage; 
  const DataHandle<ILArOFC> m_dd_ofc;
  ToolHandle<LArCablingService> m_cablingService;
  ToolHandle<ILArMCSymTool>  m_larmcsym;
  ToolHandle<ILArOFCTool> m_OFCTool;
  ToolHandle<ILuminosityTool> m_lumiTool;
  ToolHandle<Trig::IBunchCrossingTool> m_bunchCrossingTool;

  const LArOnlineID*        m_lar_on_id;
  const CaloIdManager* m_caloIdMgr;
  const CaloCell_ID* m_calocell_id;


  bool m_isMC;
  std::string m_keyShape, m_keyMinBiasAverage, m_keyOFC;

  unsigned int m_bcidMax;
  int m_ncell;

  mutable bool m_cacheValid;

  //Internal cache:
  std::vector<HWIdentifier> m_hwid_sym;
  std::vector<float> m_eshift_sym;
  std::vector<int> m_symCellIndex;
  unsigned int m_bcid;


//Functions
  StatusCode initialize();

  virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS);

  void handle(const Incident& inc);

  void getListOfCells();


  /** Callback added to handle Data-driven GeoModel initialisation
   */
  virtual StatusCode geoInit(IOVSVC_CALLBACK_ARGS);

  StatusCode computeValues(unsigned int bcid);

public:    
  
  CaloLumiBCIDTool(const std::string& type, 
		const std::string& name, 
		const IInterface* parent); 
  virtual ~CaloLumiBCIDTool();  

  float average(const CaloCell* caloCell, unsigned int bcid);

  float average(const CaloDetDescrElement* caloDDE,unsigned int bcid);


};

#endif
