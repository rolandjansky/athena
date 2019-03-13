/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

  /**
   * AlgoTool to compute OFC on fly
   *

 - OFC are computed consistently with autocorrelation 
   (by default (NMinBias=-1), these OFC are retrieved)

 - possibility to retrieve OFC for a wanted luminosity (set by NMinBias, 
   the number of minimum bias events per bunch-crossing)
   but they won't be consistent with autocorrelation anymore.
   slower since re-do calculation

 - flag FromDB switches between 
     "take OFC stored in DataBase" 
    and 
     "take OFC stored in datamembers (computed consistently from 
      LArAutoCorrTotal) or recomputed for a wanted luminosity"

   *
   */
#ifndef LARRECUTILS_LAROFCTOOL_H
#define LARRECUTILS_LAROFCTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentListener.h"

#include "LArElecCalib/ILArOFCTool.h"

#include "LArElecCalib/ILArShape.h"
#include "LArElecCalib/ILArOFC.h"
#include "LArElecCalib/ILArNoise.h"
#include "LArElecCalib/ILArPedestal.h"

#include "LArElecCalib/ILArAutoCorrTotalTool.h"

#include "StoreGate/DataHandle.h"
#include "LArCabling/LArCablingBase.h"
#include "LArElecCalib/ILArMCSymTool.h"
#include "LArIdentifier/LArOnlineID_Base.h"
#include "CaloIdentifier/LArID.h"

class StoreGateSvc; 
class LArOnlineID_Base; 
class CaloDetDescrManager_Base; 
class LArCablingBase;

class LArOFCTool: public AthAlgTool,
		  virtual public ILArOFCTool,
		  public IIncidentListener 
{
 public:
  
  // constructor
  LArOFCTool(const std::string& type, 
	     const std::string& name, 
	     const IInterface* parent); 
  
  // destructor 
  virtual ~LArOFCTool() {}
  
  // retrieve methods (default: take the value of Nminbias taken for AutoCorr)

     // online ID
  ILArOFCTool::OFCRef_t OFC_a(const HWIdentifier& id,
                              int gain,float Nminbias=-1)  const;    
  ILArOFCTool::OFCRef_t OFC_b(const HWIdentifier& id,
                              int gain,float Nminbias=-1)  const;
     // offline ID
  ILArOFCTool::OFCRef_t OFC_a(const Identifier& id,
                              int gain,float Nminbias=-1)  const;
  ILArOFCTool::OFCRef_t OFC_b(const Identifier& id,
                              int gain,float Nminbias=-1)  const;

  // initialize and finalize methods
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  // methods called in the initialize stage
  // StatusCode retrieveDB();
  StatusCode getOFC(StatusCode databaseRetrieved);
  virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS);

  static const InterfaceID& interfaceID() { 
    return ILArOFCTool::interfaceID();}

  virtual void handle(const Incident&);
 private:

  StatusCode condDataReady();

  enum TYPE_OFC{OFCA,OFCB};

  const LArOnlineID_Base*        m_lar_on_id; 
  //ToolHandle<LArCablingService> m_cablingService;
  LArCablingBase* m_cablingService;
  ToolHandle<ILArMCSymTool>    m_larmcsym;
  
  const LArEM_Base_ID*    m_lar_em_id; 
  const LArHEC_Base_ID*   m_lar_hec_id;
  const LArFCAL_Base_ID*  m_lar_fcal_id;
  const CaloDetDescrManager_Base* m_calo_dd_man;

  const DataHandle<ILArShape>  m_dd_shape;
  const DataHandle<ILArOFC>    m_dd_ofc;
  std::string m_keyShape; 
  std::string m_keyOFC;
  std::string m_keyNoise;
  std::string m_keyPedestal;


  ToolHandle<ILArAutoCorrTotalTool> m_autocorrTool;

  const DataHandle<ILArNoise> m_dd_noise; 
  const DataHandle<ILArPedestal> m_dd_pedestal;
 
  bool m_FromDB;
  bool m_MCSym;
  bool m_Dump;
  // property to determine type of data
  bool m_isMC;
      
  ILArOFC::OFCRef_t computeOFC(int aORb,const HWIdentifier& id,
                               int gain,float Nminbias)  const;

  unsigned int findTheNSamples(ILArShape::ShapeRef_t Shape,
			       unsigned int nsamples_AC_OFC,
			       unsigned int nsamples_shape) const;

  int m_useDelta;
  unsigned int m_deltaBunch;
  bool m_useHighestGainAutoCorr;
  std::vector<float> getShapeAverages( const unsigned n_samples, 
				       const unsigned n_deltaBunch, // in unit of 25 ns
				       const std::vector<float>& shape, unsigned int firstSample) const;


  // storage structure of the OFC:
  //                 channelID      OF coefficients
  typedef std::map< unsigned int, std::vector<float> > MAP ;    
  //   gain
  std::vector<MAP> m_OFCa;  
  std::vector<MAP> m_OFCb;  
  mutable std::vector<float> m_OFCtmp;
  bool m_cacheValid ; 
  bool m_loadAtBegin;
  unsigned int m_firstSample;
  bool m_isSC;

};

#endif
