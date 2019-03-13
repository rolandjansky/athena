/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

  /**
   * AlgoTool to compute the total (electronic + pile-up parts) 
      autocorrelation function

 - OFC are computed consistently with autocorrelation 
   (by default (NMinBias=-1), these OFC are retrieved)

 - possibility to retrieve AutoCorrTotal for a wanted luminosity 
   (set by NMinBias, the number of minimum bias events per bunch-crossing)
   but they won't be consistent with OFC anymore, since OFC would have been 
   calculated with the initially stored AutoCorrTotal.
   slower since re-do calculation

   *
   */
#ifndef LARRECUTILS_LARAUTOCORRTOTALTOOL_H
#define LARRECUTILS_LARAUTOCORRTOTALTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "LArElecCalib/ILArAutoCorrTotalTool.h"

#include "LArElecCalib/ILArShape.h"
#include "LArElecCalib/ILArAutoCorr.h"
#include "LArElecCalib/ILArNoise.h"
#include "LArElecCalib/ILArPedestal.h"
#include "LArElecCalib/ILArfSampl.h"
#include "LArElecCalib/ILArMinBias.h"
#include "LArElecCalib/ILArADC2MeVTool.h"

#include "StoreGate/DataHandle.h"
#include "LArElecCalib/ILArMCSymTool.h"
#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/LArID.h"
#include "GaudiKernel/IIncidentListener.h"

class StoreGateSvc; 
class LArCablingBase ;

class LArAutoCorrTotalTool: public AthAlgTool,
			    virtual public ILArAutoCorrTotalTool,
			    public IIncidentListener 
{
 public:
  
  // constructor
  LArAutoCorrTotalTool(const std::string& type, 
		       const std::string& name, 
		       const IInterface* parent); 
  
  // destructor 
  virtual ~LArAutoCorrTotalTool() {}
  
  // retrieve methods (default: take the value of Nminbias in the property)
  const std::vector<double> autoCorrTotal(const HWIdentifier& id, 
					  int gain, float Nminbias=-1 )  const;
  
  const std::vector<double> autoCorrTotal(const Identifier& id, 
					  int gain, float Nminbias=-1 )  const;

  const std::vector<double> samplRMS(const HWIdentifier& id,
                                     int gain,
                                     float Nminbias=-1) const;

  const std::vector<double> samplRMS(const Identifier& id,
                                     int gain,
                                     float Nminbias=-1) const;
  
  
  // initialize and finalize methods
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  // methods called in the initialize stage
  StatusCode getTerms()  ;
  virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS);

  static const InterfaceID& interfaceID() { 
    return ILArAutoCorrTotalTool::interfaceID();}

  virtual void handle(const Incident&);

 private:

  // compute method 
  const std::vector<double> 
    computeAutoCorr(const std::vector<float>& terms, float Nminbias)  const;
  const std::vector<double>
    computeRMS(const std::vector<float>& terms, float Nminbias) const;

  float m_Nminbias;
  bool m_NoPile;
  bool m_MCSym;
  bool m_useMixedOFCOpt;

  const LArOnlineID_Base*        m_lar_on_id; 
  LArCablingBase*           m_cablingService;
  ToolHandle<ILArMCSymTool>     m_larmcsym;
  ToolHandle<ILArADC2MeVTool>   m_adc2mevTool;
  
  const LArEM_Base_ID*    m_lar_em_id; 
  const LArHEC_Base_ID*   m_lar_hec_id;
  const LArFCAL_Base_ID*  m_lar_fcal_id;
  
  const DataHandle<ILArShape>    m_dd_shape; 
  const DataHandle<ILArAutoCorr> m_dd_autocorr; //only electronic part
  const DataHandle<ILArNoise>    m_dd_noise; 
  const DataHandle<ILArPedestal> m_dd_pedestal;
  const DataHandle<ILArfSampl>   m_dd_fSampl;
  const DataHandle<ILArMinBias>  m_dd_minbias;

  std::string  m_keyShape,m_keyAutoCorr,m_keyNoise,m_keyfSampl,m_keyMinBias,m_keyPedestal;
  bool m_isMC;

  int m_nsamples_AC_OFC;

  // storage structure
  //                 channelID     data
  typedef std::map< unsigned int, std::vector<float> > MAP ;  
  //   vector for gains
  std::vector<MAP> m_terms;
  bool m_cacheValid ; 
  bool m_loadAtBegin;
  
  int m_deltaBunch;

  int m_nsamples;
  unsigned int m_firstSample;
  bool m_isSC;

};


#endif
