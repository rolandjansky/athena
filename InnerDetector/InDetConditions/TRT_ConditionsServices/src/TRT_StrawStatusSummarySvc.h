/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_STRAWSTATUSSUMMARYSVC_H
#define TRT_STRAWSTATUSSUMMARYSVC_H
/** @file TRT_StrawStatusSummarySvc.h
 * @brief interface to TRT straw status constants
 * @author Esben Klinkby <klinkby@nbi.dk>
 */

#include <vector>
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/DataHandle.h"
//#include "InDetIdentifier/TRT_ID.h"
#include "TRT_ConditionsServices/ITRT_StrawStatusSummarySvc.h" 
#include "TRT_ConditionsServices/ITRT_ConditionsSvc.h"
#include "StoreGate/StoreGateSvc.h"


class TRT_ID;
namespace InDetDD{ class TRT_DetectorManager; }

class IToolSvc;
/** @class TRT_StrawStatusSummarySvc
 interface to TRT straw status constants
*/
class TRT_StrawStatusSummarySvc: public AthService,
  virtual public ITRT_StrawStatusSummarySvc,
  virtual public ITRT_ConditionsSvc
{
 public:
  
  /// constructor
  TRT_StrawStatusSummarySvc( const std::string& name, ISvcLocator* pSvcLocator );
  
  /// destructor
  virtual ~TRT_StrawStatusSummarySvc();
  
  /// tool initialize
  virtual StatusCode initialize();

  /// tool finalize
  virtual StatusCode finalize();

  /// Evaluation for TRT_ConditionsSummarySvc
  InDet::TRT_CondFlag condSummaryStatus( const Identifier& id ) {
    if (!(get_status(id)==1)) return InDet::TRT_COND_GOOD; 
    else return InDet::TRT_COND_BAD;
  };

  InDet::TRT_CondFlag condSummaryStatusHT( const Identifier& id ) {
    if (!(get_statusHT(id)==1)) return InDet::TRT_COND_GOOD; 
    else return InDet::TRT_COND_BAD;
  };


  /*
    // EK: Consider to put something like this
    if (get_LT_occ(Identifier)<0.1 ) return InDet::TRT_COND_GOOD;
    else if (get_LT_occ(Identifier)>=0.1 ) return InDet::TRT_COND_BAD;
    };
  */

  /// access to interfaceID
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvIf ) ;


 /// access to the status
  int getStatus(Identifier offlineId);// const;
  int getStatusPermanent(Identifier offlineId);// const;
  int getStatusHT(Identifier offlineId);// const;

  //  void setStatus( const TRTCond::ExpandedIdentifier& id, char status ) ;
  void setStatus( const TRTCond::ExpandedIdentifier& id, unsigned int status ) ;

  void  set_LT_occ(Identifier offlineID, double lt_occ) ;
  double get_LT_occ(Identifier offlineID);

  void set_HT_occ(Identifier offlineID, double ht_occ);
  double get_HT_occ(Identifier offlineID);

  //special bits
  //void set_status(Identifier offlineID, bool set);
  void set_status_temp(Identifier offlineID, bool set);
  void set_status_permanent(Identifier offlineID, bool set);
  bool get_status(Identifier offlineID);
  bool get_statusHT(Identifier offlineID);
  void set_pid_status(Identifier offlineID, bool set);
  bool get_pid_status(Identifier offlineID);
  void set_track_status(Identifier offlineID, bool set);
  bool get_track_status(Identifier offlineID);

  StatusCode readFromTextFile(const std::string& filename) ;
  StatusCode writeToTextFile(const std::string& filename) const ;

  StatusCode streamOutStrawStatObjects () const ;
  StatusCode registerObjects(std::string tag, int run1, int event1, int run2, int event2) const;
  
  StatusCode IOVCallBack(IOVSVC_CALLBACK_ARGS);
  
  StatusCode ComputeAliveStraws() ;

  StrawStatusContainer* getStrawStatusContainer() const ;
  StrawStatusContainer* getStrawStatusPermanentContainer() const ;
  StrawStatusContainer* getStrawStatusHTContainer() const ;


  int  *getStwTotal()		;	 
  int **getStwLocal() 		;
  int **getStwWheel()   	;
  int **getStwLocalStraw()  	; 



 private:
  ServiceHandle<StoreGateSvc> m_detStore;
  std::vector<double> m_ltvaluevector;
  std::vector<double> m_htvaluevector;
  ServiceHandle<IToolSvc> m_toolsvc; 
  std::string m_par_strawstatuscontainerkey;
  std::string m_par_strawstatuspermanentcontainerkey;
  std::string m_par_strawstatusHTcontainerkey;

  std::string m_par_stattextfile;           //input text file

  //std::string m_par_strawstatuspermanentcontainerkey;
  std::string m_par_stattextfilepermanent;           //input text file: permanent
  std::string m_par_stattextfileHT;           //input text file

  std::string m_par_statstream;            //output stream
  std::string m_inputFile;
  const TRT_ID* m_trtid;
  const InDetDD::TRT_DetectorManager* m_trtDetMgr; // TRT detector manager (to get ID helper)
  TRTCond::StrawStatusMultChanContainer m_deadstraws ;
  
  bool m_strawstatuscontainerexists;
  bool m_strawstatuspermanentcontainerexists;
  bool m_strawstatusHTcontainerexists;


  const DataHandle<TRTCond::StrawStatusMultChanContainer> m_strawstatuscontainer ; //Same name as in getStrawStatusContainer!
  const DataHandle<TRTCond::StrawStatusMultChanContainer> m_strawstatuspermanentcontainer ; //Same name as in getStrawStatusContainer!
  const DataHandle<TRTCond::StrawStatusMultChanContainer> m_strawstatusHTcontainer ; //Same name as in getStrawStatusContainer!


      int  *m_stw_total;
      int **m_stw_local;
      int **m_stw_wheel;

  void resetArrays();

	// COPIED FROM TRT LOCAL OCCUPANCY TOOL NOT OPTIMAL!!!
  int findArrayTotalIndex(const int det, const int lay);
  int findArrayLocalWheelIndex(const int det, const int lay);
  int findArrayLocalStrawIndex(const int det, const int lay, const int strawlay);

};



////////////////////////////////////////////////////////////////////////////////////////////
//  inline methods
////////////////////////////////////////////////////////////////////////////////////////////

inline int*  TRT_StrawStatusSummarySvc::getStwTotal()
{
	return m_stw_total;
}

inline int**  TRT_StrawStatusSummarySvc::getStwLocal()
{
	return m_stw_local;
}

inline int**  TRT_StrawStatusSummarySvc::getStwWheel()
{
	return m_stw_wheel;
}



inline void TRT_StrawStatusSummarySvc::setStatus( const TRTCond::ExpandedIdentifier& id, unsigned int status ) 
{
  getStrawStatusContainer()->set( id, status) ;//, lt_occ, ht_occ); 
}


inline TRT_StrawStatusSummarySvc::StrawStatusContainer* TRT_StrawStatusSummarySvc::getStrawStatusContainer() const {
  const StrawStatusContainer* rcesben = &(*m_strawstatuscontainer) ;
  return const_cast<StrawStatusContainer*>(rcesben) ; 
}
inline TRT_StrawStatusSummarySvc::StrawStatusContainer* TRT_StrawStatusSummarySvc::getStrawStatusPermanentContainer() const {
  const StrawStatusContainer* rcesben = &(*m_strawstatuspermanentcontainer) ;
  return const_cast<StrawStatusContainer*>(rcesben) ; 
}
inline TRT_StrawStatusSummarySvc::StrawStatusContainer* TRT_StrawStatusSummarySvc::getStrawStatusHTContainer() const {
  const StrawStatusContainer* rcesben = &(*m_strawstatusHTcontainer) ;
  return const_cast<StrawStatusContainer*>(rcesben) ; 
}

inline StatusCode TRT_StrawStatusSummarySvc::queryInterface( const InterfaceID& riid, void** ppvIf )
{
  if ( riid == ITRT_StrawStatusSummarySvc::interfaceID() )  {
    *ppvIf = dynamic_cast<ITRT_StrawStatusSummarySvc*>(this);
    addRef();
    return StatusCode::SUCCESS;
  } else if ( ITRT_ConditionsSvc::interfaceID() == riid ) {
    *ppvIf = dynamic_cast<ITRT_ConditionsSvc*>(this);
    addRef();
    return StatusCode::SUCCESS;
  }
  return AthService::queryInterface( riid, ppvIf );

}

#endif 
