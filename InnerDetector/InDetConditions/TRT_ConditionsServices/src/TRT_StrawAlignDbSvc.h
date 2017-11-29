/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_STRAWALIGNDBSVC_H
#define TRT_STRAWALIGNDBSVC_H
/** @file TRT_StrawAlignDbSvc.h
 * @brief interface to TRT straw alignment constants
 * @author Peter Hansen <phansen@nbi.dk>
 */

#include <vector>
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"
#include "InDetIdentifier/TRT_ID.h"
#include "TRT_ConditionsServices/ITRT_StrawAlignDbSvc.h"
#include "TRT_ConditionsData/StrawDxContainer.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"

class  IAthenaOutputStreamTool ;
namespace InDetDD {
  class TRT_DetectorManager ;
}

/** @class TRT_StrawAlignDbSvc
  interface to TRT straw alignment constants
*/
class TRT_StrawAlignDbSvc: public AthService, virtual public ITRT_StrawAlignDbSvc
{
 public:
  
  /// constructor
  TRT_StrawAlignDbSvc( const std::string& name, ISvcLocator* pSvcLocator );
  
  /// destructor
  virtual ~TRT_StrawAlignDbSvc();
  
  /// tool initialize
  virtual StatusCode initialize();

  /// tool finalize
  virtual StatusCode finalize();

  /// access to interfaceID
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvIf ) ;

  // methods to access straw alignment data

  /// create an TRTCond::ExpandedIdentifier from a TRTID identifier
  TRTCond::ExpandedIdentifier trtcondid( const Identifier& id, int level = TRTCond::ExpandedIdentifier::STRAW) const;

  // methods to set straw alignment data

  /// set endpoints for a TRTCond::ExpandedIdentifier
  void setDx( const TRTCond::ExpandedIdentifier& id, float dx1, float dx2, float dxerr ) ;
  
  // methods for persistence
  
  /// write calibration constants to flat text file 
  StatusCode writeTextFile(const std::string& file) const;
  
  /// read calibration from text file into TDS
  StatusCode readTextFile(const std::string& file);

  /// write the calibration objects to output, after cleaning
  StatusCode streamOutObjects () const;
  
  /// register calibration objects with the IoV service
  StatusCode registerObjects(std::string tag, int run1, int event1, int run2, int event2) const;
    

  /// access to containers
  StrawDxContainer* getDxContainer() const ;

  /// IOV call back for dx objects. normally this doesn't do anything.
  StatusCode IOVCallBack(IOVSVC_CALLBACK_ARGS);

  /** get shift near electronics */
  float getDx1( const Identifier& id ) const ;
  /** get shift far from electronics */
  float getDx2( const Identifier& id ) const ;
  /** get error on shift */
  float getDxErr( const Identifier& id ) const ;
  /** set shifts */
  void setDx( const Identifier& id, float dx1, float dx2, float dxerr ) ;

 private:

  ServiceHandle<StoreGateSvc> m_detStore;
  std::string m_par_dxcontainerkey;   //!> folder name
  std::string m_par_strawtextfile;    //!> input text file
  bool m_par_forcecallback;           //!> force callback to manager
  const TRT_ID* m_trtid;              //!> id helper
  const InDetDD::TRT_DetectorManager* m_trtman; //!> detector manager
  
  // pointers to calibration data
  const DataHandle<StrawDxContainer> m_dxcontainer ; //!> persistifiable container

  ToolHandle<IAthenaOutputStreamTool> m_streamer; //!> OutputStreamTool

};

////////////////////////////////////////////////////////////////////////////////////////////
//  inline methods
////////////////////////////////////////////////////////////////////////////////////////////

inline TRT_StrawAlignDbSvc::StrawDxContainer* TRT_StrawAlignDbSvc::getDxContainer() const {
  const StrawDxContainer* rc = &(*m_dxcontainer) ;
  return const_cast<StrawDxContainer*>(rc) ; 
}

inline TRTCond::ExpandedIdentifier 
TRT_StrawAlignDbSvc::trtcondid( const Identifier& id, int level) const
{
  return TRTCond::ExpandedIdentifier( m_trtid->barrel_ec(id),m_trtid->layer_or_wheel(id),
				      m_trtid->phi_module(id),m_trtid->straw_layer(id),
				      m_trtid->straw(id),level ) ;
}


inline void
TRT_StrawAlignDbSvc::setDx( const TRTCond::ExpandedIdentifier& id,float dx1, float dx2, float dxerr ) 
{
  //std::cout << " Setting the dx1 to " << dx1 << " dx2 to " << dx2 << std::endl;
  getDxContainer()->setDx( id, dx1, dx2, dxerr); 
}

inline float
TRT_StrawAlignDbSvc::getDx1( const Identifier& id ) const
{
  return getDxContainer()->getDx1( trtcondid(id,TRTCond::ExpandedIdentifier::STRAW) ) ;
}

inline float
TRT_StrawAlignDbSvc::getDx2( const Identifier& id ) const
{
  return getDxContainer()->getDx2( trtcondid(id,TRTCond::ExpandedIdentifier::STRAW) ) ;
}

inline float
TRT_StrawAlignDbSvc::getDxErr( const Identifier& id ) const
{
  return getDxContainer()->getDxErr( trtcondid(id,TRTCond::ExpandedIdentifier::STRAW) ) ;
}

inline void
TRT_StrawAlignDbSvc::setDx( const Identifier& id,float dx1, float dx2, float dxerr ) 
{
  setDx( trtcondid(id,TRTCond::ExpandedIdentifier::STRAW), dx1, dx2, dxerr) ;
}

inline StatusCode TRT_StrawAlignDbSvc::queryInterface( const InterfaceID& riid, void** ppvIf )
{
  if ( riid == ITRT_StrawAlignDbSvc::interfaceID() )  {
    *ppvIf = dynamic_cast<ITRT_StrawAlignDbSvc*>(this);
    addRef();
    return StatusCode::SUCCESS;
  }
  return AthService::queryInterface( riid, ppvIf );
}

#endif //  TRT_STRAWALIGNDBSVC_H
