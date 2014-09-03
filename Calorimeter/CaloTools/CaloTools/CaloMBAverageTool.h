/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
    @class CaloMBAverageTool
    @brief Tool to provide expected pedestal shift for Pileup from first principle computation (relevant for non 25ns bunch spacing)
    @author G.Unal
*/

#ifndef CALOTOOLS_CaloMBAverageTool_H
#define CALOTOOLS_CaloMBAverageTool_H

class Identifier; 
class StoreGateSvc; 
class CaloIdManager;
class CaloCell_ID;
class CaloDetDescrElement;

#include "CaloIdentifier/CaloGain.h"
#include "LArElecCalib/ILArShape.h"
#include "LArElecCalib/ILArfSampl.h"
#include "LArElecCalib/ILArMinBiasAverage.h"
#include "LArElecCalib/ILArOFCTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"
#include "CaloInterface/ICaloMBAverageTool.h"
#include "AthenaKernel/IOVSvcDefs.h"


class CaloMBAverageTool: public AthAlgTool,
	             virtual public ICaloMBAverageTool
{
private: 
//Database  

  const DataHandle<ILArShape> m_dd_shape;
  const DataHandle<ILArfSampl> m_dd_fsampl;
  const DataHandle<ILArMinBiasAverage> m_dd_minbiasAverage; 
  ToolHandle<ILArOFCTool> m_OFCTool;

  const DataHandle<CaloIdManager> m_caloIdMgr;
  const CaloCell_ID*       m_calo_id;


  float m_Nminbias;
  int m_deltaBunch;
  std::string m_keyShape, m_keyfSampl, m_keyMinBiasAverage;

  unsigned int m_ncell;
  std::vector<float> m_shift;

//Functions
  StatusCode initialize();

  virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS);

  /** Callback added to handle Data-driven GeoModel initialisation
   */
  virtual StatusCode geoInit(IOVSVC_CALLBACK_ARGS);

public:    
  
  CaloMBAverageTool(const std::string& type, 
		const std::string& name, 
		const IInterface* parent); 
  virtual ~CaloMBAverageTool();  

  float average(const CaloCell* caloCell);

  float average(const CaloDetDescrElement* caloDDE,const CaloGain::CaloGain gain);


};

#endif
