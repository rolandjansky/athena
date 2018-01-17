/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_CALDBSVC_H
#define TRT_CALDBSVC_H
/** @file TRT_CalDbSvc.h
 * @brief  interface to TRT calibration constants
 * @author Peter Hansen <phansen@nbi.dk>, Wouter hulsbergen
 */

#include "TRT_ConditionsServices/ITRT_CalDbSvc.h"
//Gaudi Includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ContextSpecificPtr.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ICondSvc.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/DataHandle.h"
#include "InDetIdentifier/TRT_ID.h"
#include "AthenaKernel/IAthenaOutputStreamTool.h"


template <class TYPE> class SvcFactory;
class ISvcLocator;
class StoreGateSvc;
//namespace InDetDD {class TRT_DetectorManager; }

/** @class TRT_CalDbSvc
 *  interface to TRT calibration constants
 */
class TRT_CalDbSvc: public AthService , virtual public ITRT_CalDbSvc
{
 public:
  /// typedefs, enums etc

  
  /// constructor
  TRT_CalDbSvc( const std::string& name, ISvcLocator* pSvcLocator );
  
  /// destructor
  virtual ~TRT_CalDbSvc();
  
  /// tool initialize
  virtual StatusCode initialize();

  /// tool finalize
  virtual StatusCode finalize();

  /// access to interfaceID
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvIf ) ;


  // methods to access calibration data

  /// get T0 for an identifier
  float getT0( const Identifier& id, int level = TRTCond::ExpandedIdentifier::STRAW ) const ;

  /// get an rtrelation for an identifier
  const TRTCond::RtRelation* getRtRelation( const Identifier& id, int level = TRTCond::ExpandedIdentifier::STRAW ) const ;

  /// get errors for an identifier
  const TRTCond::RtRelation* getErrors( const Identifier& id, int level = TRTCond::ExpandedIdentifier::STRAW ) const ;

  /// get errors for an identifier
  const TRTCond::RtRelation* getSlopes( const Identifier& id, int level = TRTCond::ExpandedIdentifier::STRAW ) const ;
  
  /// get a drift radius for a given leading edge time
  double driftRadius(const double& time, float& t0, const Identifier& ident, bool& found) const;

  /// get a drift radius error for a given drifttime
  double driftError(const double& time, const Identifier& ident, bool& found) const;

  /// get a drift radius error for a given drifttime
  double driftSlope(const double& time, const Identifier& ident, bool& found) const;

  /// create an TRTCond::ExpandedIdentifier from a TRTID identifier
  TRTCond::ExpandedIdentifier trtcondid( const Identifier& id, int level = TRTCond::ExpandedIdentifier::STRAW) const;

  // methods to set calibration data

  /// set T0 for a TRTCond::ExpandedIdentifier
  void setT0( const TRTCond::ExpandedIdentifier& id,float t0, float t0err ) ;
  
 // set an rt-relation for a TRTCond::ExpandedIdentifier
  void setRtRelation( const TRTCond::ExpandedIdentifier& id, const TRTCond::RtRelation* rtr) ;

  /// set  rt-errors for a TRTCond::ExpandedIdentifier
  void setRtErrors( const TRTCond::ExpandedIdentifier& id, const TRTCond::RtRelation* rtr) ;

  /// set  rt-errors for a TRTCond::ExpandedIdentifier
  void setRtSlopes( const TRTCond::ExpandedIdentifier& id, const TRTCond::RtRelation* rtr) ;

  
  // methods for persistence
  
  /// write calibration constants to flat text file 
  StatusCode writeTextFile_Format0(std::ostream&) const;
  StatusCode writeTextFile_Format1(std::ostream&) const;
  StatusCode writeTextFile_Format2(std::ostream&) const;
  StatusCode writeTextFile_Format3(std::ostream&) const;

  /// read calibration from text file into TDS
  StatusCode readTextFile(const std::string& file, int& format);
  StatusCode readTextFile_Format0(std::istream&) ;
  StatusCode readTextFile_Format1(std::istream&) ;
  StatusCode readTextFile_Format2(std::istream&) ;
  StatusCode readTextFile_Format3(std::istream&) ;

  /// write the calibration objects to output, after cleaning
  StatusCode streamOutCalibObjects () const;
  
  /// register calibration objects with the IoV service
  StatusCode registerCalibObjects
    (std::string tag, unsigned int run1, unsigned int event1, unsigned int run2, unsigned int event2) const;


  /// access to calibration constant containers
  RtRelationContainer* getRtContainer() const ;
  RtRelationContainer* getErrContainer() const ;
  RtRelationContainer* getSlopeContainer() const ;
  StrawT0Container* getT0Container() const ;
  // Access via a cached ContextSpecificPtr is used for text file input
  void setRtContainer(RtRelationContainer* rc);
  void setErrContainer(RtRelationContainer* rc);
  void setSlopeContainer(RtRelationContainer* rc);
  void setT0Container(StrawT0Container* rc);
  void useCachedPtr(const bool& useit);

  
 private:
  std::string m_par_rtcontainerkey ;       //!< folder name for rt relation 
  std::string m_par_errcontainerkey ;       //!< folder name for rt errors
  std::string m_par_slopecontainerkey ;       //!< folder name for rt slopes
  std::string m_par_t0containerkey ;       //!< folder name for t0 
  std::string m_par_caltextfile;           //!< input text error file

  const TRT_ID* m_trtid;                 //!< id helper
  ToolHandle<IAthenaOutputStreamTool> m_streamer;        //!< OutputStreamTool
  ServiceHandle<StoreGateSvc> m_detstore;

  ServiceHandle<ICondSvc> m_condSvc;
  //  ReadHandle  keys
  SG::ReadCondHandleKey<RtRelationContainer> m_rtReadKey{this,"RtReadKeyName","/TRT/Calib/RT","r-t relation in-key"};
  SG::ReadCondHandleKey<RtRelationContainer> m_errReadKey{this,"ErrorReadKeyName","/TRT/Calib/errors2d","error on r in-key"};
  SG::ReadCondHandleKey<RtRelationContainer> m_slopeReadKey{this,"SlopeReadKeyName","/TRT/Calib/slopes","slope of error in-key"};
  SG::ReadCondHandleKey<StrawT0Container> m_t0ReadKey{this,"T0ReadKeyName","/TRT/Calib/T0","t0 in-key"};

  bool m_useCachedPtr;  
  Gaudi::Hive::ContextSpecificPtr<RtRelationContainer> m_rtContainer;
  Gaudi::Hive::ContextSpecificPtr<RtRelationContainer> m_errContainer;
  Gaudi::Hive::ContextSpecificPtr<RtRelationContainer> m_slopeContainer;
  Gaudi::Hive::ContextSpecificPtr<StrawT0Container> m_t0Container;
  

  /// Keep track of the number of instances
  /*  If multiple instances of this service are instantiated and configured
   *  differently, they can conflict and cause jobs to crash.
   */
  static unsigned int s_numberOfInstances;


};

////////////////////////////////////////////////////////////////////////////////////////////
//  inline methods
////////////////////////////////////////////////////////////////////////////////////////////

inline TRTCond::ExpandedIdentifier 
TRT_CalDbSvc::trtcondid( const Identifier& id, int level) const
{
  return TRTCond::ExpandedIdentifier( m_trtid->barrel_ec(id),m_trtid->layer_or_wheel(id),
				      m_trtid->phi_module(id),m_trtid->straw_layer(id),
				      m_trtid->straw(id),level ) ;
}

inline const TRTCond::RtRelation*
TRT_CalDbSvc::getRtRelation( const Identifier& id, int level ) const 
{ 
  const RtRelationContainer* rc = getRtContainer();
  if(!rc) return 0;
  return rc->get(trtcondid(id,level)) ; 
}

inline const TRTCond::RtRelation*
TRT_CalDbSvc::getErrors( const Identifier& id, int level ) const 
{ 
  const RtRelationContainer* rc = getErrContainer();
  if(!rc) return 0;
  return rc->get(trtcondid(id,level)) ; 
}

inline const TRTCond::RtRelation*
TRT_CalDbSvc::getSlopes( const Identifier& id, int level ) const
{
  const RtRelationContainer* rc = getSlopeContainer();
  if(!rc) return 0;
  return rc->get(trtcondid(id,level)) ;
}

inline float 
TRT_CalDbSvc::getT0( const Identifier& id, int level ) const 
{
  const StrawT0Container* rc = getT0Container();
  return rc->getT0(trtcondid(id,level)) ; 
}


inline void
TRT_CalDbSvc::setT0( const TRTCond::ExpandedIdentifier& id,float t0, float t0err ) 
{
  getT0Container()->setT0( id, t0, t0err ); 
}

inline void 
TRT_CalDbSvc::setRtRelation( const TRTCond::ExpandedIdentifier& id, const TRTCond::RtRelation* rtr) 
{
  // I have a problem with getting the 'consts' in the right place in the template definition. 
  // (Basically "const T& [with T=TRTRelation*]" ~= "const TRTRelation*"
  // Temporarily remove the const.
  getRtContainer()->set( id,const_cast<TRTCond::RtRelation*>(rtr)); 
  //m_rtcontainer->set( id, rtr) ;
}

inline void
TRT_CalDbSvc::setRtErrors( const TRTCond::ExpandedIdentifier& id, const TRTCond::RtRelation* rtr)
{
  // Temporarily remove the const.
  getErrContainer()->set( id,const_cast<TRTCond::RtRelation*>(rtr));
}

inline void
TRT_CalDbSvc::setRtSlopes( const TRTCond::ExpandedIdentifier& id, const TRTCond::RtRelation* rtr)
{
  // Temporarily remove the const.
  getSlopeContainer()->set( id,const_cast<TRTCond::RtRelation*>(rtr));
}

inline void
TRT_CalDbSvc::useCachedPtr( const bool& useit)
{
  m_useCachedPtr=useit;
}


/// Query Interface
inline StatusCode TRT_CalDbSvc::queryInterface( const InterfaceID& riid, void** ppvIf )
{
  if ( ITRT_CalDbSvc::interfaceID() == riid ) {
    *ppvIf = dynamic_cast<ITRT_CalDbSvc*>(this);
    addRef();
  } else {
    return AthService::queryInterface( riid, ppvIf );
  }
  return StatusCode::SUCCESS;
}

#endif //  TRT_CALDBSVC_H
