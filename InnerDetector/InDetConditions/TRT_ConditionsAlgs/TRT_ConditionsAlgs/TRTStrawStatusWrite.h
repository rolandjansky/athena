/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////
//TRTStrawStatusWrite.h
//Ashfaq Ahmad 17/02/2005 Based on SCT CondtionsAlg
////////////////////////////////////////////////////


#ifndef TRTSTRAWSTATUSWRITE_H
#define TRTSTRAWSTATUSWRITE_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/ICondSvc.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "TRT_ConditionsServices/ITRT_ConditionsSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "InDetIdentifier/TRT_ID.h"
#include "TRT_ConditionsData/ExpandedIdentifier.h"
#include "TRT_ConditionsData/StrawStatusMultChanContainer.h"

namespace InDetDD{ class TRT_DetectorManager; }



class TRTStrawStatusWrite : public AthAlgorithm
{

 public:

  TRTStrawStatusWrite( const std::string &name, ISvcLocator *pSvcLocator);
  virtual ~TRTStrawStatusWrite()=default;
  typedef TRTCond::StrawStatusMultChanContainer StrawStatusContainer ;

  // Gaudi
  virtual StatusCode initialize( ) override;
  virtual StatusCode execute( ) override;
  virtual StatusCode finalize( ) override;

 /// access to the status
  virtual int getStatus(Identifier offlineId);
  virtual int getStatusPermanent(Identifier offlineId);
  virtual int getStatusHT(Identifier offlineId);

  /// What the bit means
  InDet::TRT_CondFlag condSummaryStatus( const Identifier& id ) {
    if (!(get_status(id)==1)) return InDet::TRT_COND_GOOD; 
    else return InDet::TRT_COND_BAD;
  };

  InDet::TRT_CondFlag condSummaryStatusHT( const Identifier& id ) {
    if (!(get_statusHT(id)==1)) return InDet::TRT_COND_GOOD; 
    else return InDet::TRT_COND_BAD;
  };

  //special bits
  virtual void set_status_temp(StrawStatusContainer* ssc, Identifier offlineID, bool set);
  virtual void set_status_permanent(StrawStatusContainer* ssc, Identifier offlineID, bool set);
  virtual bool get_status(Identifier offlineID);
  virtual bool get_statusHT(Identifier offlineID);

  virtual StatusCode readStatFromTextFile(const std::string& filename);
  virtual StatusCode readStatPermFromTextFile(const std::string& filename);
  virtual StatusCode readStatHTFromTextFile(const std::string& filename);
  virtual StatusCode writeToTextFile(const std::string& filename);

  
  virtual const StrawStatusContainer* getStrawStatusContainer() const;
  virtual const StrawStatusContainer* getStrawStatusPermanentContainer()  const;
  virtual const StrawStatusContainer* getStrawStatusHTContainer() const;



 private:


  ServiceHandle<StoreGateSvc> m_detStore;
  std::string m_par_strawstatuscontainerkey;
  std::string m_par_strawstatuspermanentcontainerkey;
  std::string m_par_strawstatusHTcontainerkey;
  std::string m_par_stattextfile;           //input text file
  std::string m_par_stattextfilepermanent;  //input text file: permanent
  std::string m_par_stattextfileHT;         //input text file: HT


  const TRT_ID* m_trtid;                    //TRT id helper
  std::string m_par_statstream;             //output stream  
  ServiceHandle<ICondSvc> m_condSvc;

  //  ReadHandle  keys
  SG::ReadCondHandleKey<StrawStatusContainer> m_statReadKey{this,"StatReadKeyName","in","StrawStatus in-key"};
  SG::ReadCondHandleKey<StrawStatusContainer> m_permReadKey{this,"PermReadKeyName","in","StrawStatusPermanent in-key"};
  SG::ReadCondHandleKey<StrawStatusContainer> m_statHTReadKey{this,"StatHTReadKeyName","in","StrawStatusHT in-key"};

};


#endif
