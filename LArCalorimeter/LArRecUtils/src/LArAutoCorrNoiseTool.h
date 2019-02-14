/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

  /**
   * AlgoTool to compute the sqrt of the autocorrelation matrix
     to use for noise generation

 - get autocorrelation function from database
 - allow different number of samples
    if Nsamples>Nsamples in database, assume 0 for the autocorrelation
  between Nsample(database) and Nsamples(requested)
      autocorrelation function

 Interfaces defined in ILArAutoCorrNoiseTool.h

   */
#ifndef LARRECUTILS_LARAUTOCORRNOISETOOL_H
#define LARRECUTILS_LARAUTOCORRNOISETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "LArElecCalib/ILArAutoCorrNoiseTool.h"

#include "LArElecCalib/ILArAutoCorr.h"

#include "StoreGate/DataHandle.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArElecCalib/ILArMCSymTool.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArID.h"
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

class StoreGateSvc; 

class LArAutoCorrNoiseTool: public AthAlgTool,
			    virtual public ILArAutoCorrNoiseTool,
		      public IIncidentListener 
{
 public:
  
  // constructor
  LArAutoCorrNoiseTool(const std::string& type, 
		       const std::string& name, 
		       const IInterface* parent); 
  
  // destructor 
  virtual ~LArAutoCorrNoiseTool() {}
  
  const std::vector<float>& autoCorrSqrt(const HWIdentifier& id, int gain, int Nsamp=-1);
  
  const std::vector<float>& autoCorrSqrt(const Identifier& id, int gain, int Nsamp=-1);
  
  // initialize and finalize methods
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  // methods called in the initialize stage
  StatusCode getTerms();
  virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS);

  static const InterfaceID& interfaceID() { 
    return ILArAutoCorrNoiseTool::interfaceID();}


  virtual void handle(const Incident&);
  
 private:

  int m_Nsampl;
  bool m_MCSym;

  const LArOnlineID*        m_lar_on_id; 
  const LArOnline_SuperCellID*        m_lar_scon_id; 
  ToolHandle<ILArMCSymTool>  m_larmcsym;

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKeySC{this,"ScCablingKey","LArOnOffIdMapSC","SG Key of SC LArOnOffIdMapping object"};
  
  const DataHandle<ILArAutoCorr> m_dd_autocorr; //only electronic part

  std::string  m_keyAutoCorr;

  // storage structure
  //                 channelID     data
  typedef std::map< unsigned int, std::vector<float> > MAP ;  
  //   vector for gains
  std::vector<MAP> m_terms;
  bool m_cacheValid ; 
  bool m_loadAtBegin;
  bool m_isSC;

};


#endif
