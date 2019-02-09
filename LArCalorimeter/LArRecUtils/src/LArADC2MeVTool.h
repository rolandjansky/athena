//Dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


  /**
   * AlgoTool to compute ADC2MEV factor from the 3 subfactors ADC2DAC, DAC2UA, UA2MEV
   *
   * @author S. Laplace
   * @version 0-0-1, 28/01/2004
   *
   */
#ifndef LARRECUTILS_LARADC2MEVTOOL_H
#define LARRECUTILS_LARADC2MEVTOOL_H

#include "LArElecCalib/ILArADC2MeVTool.h"
#include "GaudiKernel/IIncidentListener.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaKernel/IIOVDbSvc.h" 


#include "LArElecCalib/ILArRamp.h"
#include "LArElecCalib/ILArDAC2uA.h"
#include "LArElecCalib/ILAruA2MeV.h"
#include "LArElecCalib/ILArMphysOverMcal.h"
#include "LArElecCalib/ILArHVScaleCorr.h"
#include "LArElecCalib/ILArFEBConfigReader.h"

#include "StoreGate/DataHandle.h"
#include "LArCabling/LArCablingBase.h"
#include "LArElecCalib/ILArMCSymTool.h"
#include "LArIdentifier/LArOnlineID_Base.h"

#include "LArRawConditions/LArConditionsContainer.h"

#include "AthenaBaseComps/AthAlgTool.h"


class LArADC2MeVTool: public AthAlgTool,
		      virtual public ILArADC2MeVTool,
		      public IIncidentListener 
{
 public:
  
  // constructor
  LArADC2MeVTool(const std::string& type, 
		 const std::string& name, 
		 const IInterface* parent); 
  
  // destructor 
  virtual ~LArADC2MeVTool();
  
  // retrieve methods 
  const std::vector<float>& ADC2MEV(const HWIdentifier& id, int gain) const;  
  const std::vector<float>& ADC2MEV(const Identifier& id, int gain) const;
  
  // initialize and finalize methods
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  //IOV Callback functions
  virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS);
  virtual void handle(const Incident&); 

  // methods called in the initialize stage
  StatusCode getADC2MeV() const; 

  static const InterfaceID& interfaceID() { 
    return ILArADC2MeVTool::interfaceID();}

 private:

  const LArOnlineID_Base*        m_lar_on_id; 
  LArCablingBase*            m_cablingService;
  ToolHandle<ILArMCSymTool>  m_larmcsym;
  ServiceHandle<IIOVDbSvc> m_IOVDbSvc; //!< Handle on the IOVDb service

  const DataHandle<ILArRamp>   m_dd_ADC2DAC; 
  const DataHandle<ILArDAC2uA> m_dd_DAC2uA; 
  const DataHandle<ILAruA2MeV> m_dd_uA2MeV; 
  const DataHandle<ILArMphysOverMcal> m_dd_MphysMcal;
  const DataHandle<ILArHVScaleCorr> m_dd_HVScaleCorr;

  //tool properties
  std::string m_keyADC2DAC;
  std::string m_keyDAC2uA;
  std::string m_keyuA2MeV;
  std::string m_keyMphysMcal; 
  std::string m_keyHVScaleCorr;
  bool m_MCSym;
  bool m_useMphysOverMcal;
  bool m_useHVScaleCorr;
  bool m_loadAtBegin;
  bool m_useFEBGainThresholds;


  mutable bool m_cacheValid;

  //Internal cache:
  mutable LArConditionsContainer< std::vector<float> >* m_ADC2MeV;
  mutable std::vector< std::vector < float > > m_ADC2MeV_vec;

  ToolHandle<ILArFEBConfigReader> m_febConfigReader;
  bool m_isSC;
};

#endif
