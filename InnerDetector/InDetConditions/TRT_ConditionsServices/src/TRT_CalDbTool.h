/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_CALDBTOOL_H
#define TRT_CALDBTOOL_H
/** @file TRT_CalDbTool.h
 * @brief  interface to TRT calibration constants
 * @author Peter Hansen <phansen@nbi.dk>
 */

#include "TRT_ConditionsServices/ITRT_CalDbTool.h"
// STL
#include <mutex>
//Gaudi includes
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ContextSpecificPtr.h"
#include "GaudiKernel/ICondSvc.h"
// Storegate
#include "StoreGate/ReadCondHandleKey.h"
//Athena
#include "AthenaBaseComps/AthAlgTool.h"
// TRT
#include "InDetIdentifier/TRT_ID.h"



/** @class TRT_CalDbTool
 *  interface to TRT calibration constants
 */
class TRT_CalDbTool: public extends<AthAlgTool, ITRT_CalDbTool>
{
 public:
  /// typedefs, enums etc

  
  /// constructor
  TRT_CalDbTool( const std::string& type, const std::string& name, const IInterface* parent);
  
  /// destructor
  virtual ~TRT_CalDbTool() = default;
  
  /// tool initialize
  virtual StatusCode initialize() override;

  /// tool finalize
  virtual StatusCode finalize() override;


  // methods to access calibration data

  /// get T0 for an identifier
  virtual float getT0( const Identifier& id, int level = TRTCond::ExpandedIdentifier::STRAW ) const override;

  /// get an rtrelation for an identifier
  virtual const TRTCond::RtRelation* getRtRelation( const Identifier& id, int level = TRTCond::ExpandedIdentifier::STRAW ) const override;

  /// get errors for an identifier
  virtual const TRTCond::RtRelation* getErrors( const Identifier& id, int level = TRTCond::ExpandedIdentifier::STRAW ) const override;

  /// get errors for an identifier
  virtual const TRTCond::RtRelation* getSlopes( const Identifier& id, int level = TRTCond::ExpandedIdentifier::STRAW ) const override;
  
  /// get a drift radius for a given leading edge time
  virtual double driftRadius(const double& time, float& t0, const Identifier& ident, bool& found) const override;

  /// get a drift radius error for a given drifttime
  virtual double driftError(const double& time, const Identifier& ident, bool& found) const override;

  /// get a drift radius error for a given drifttime
  virtual double driftSlope(const double& time, const Identifier& ident, bool& found) const override;

  /// create an TRTCond::ExpandedIdentifier from a TRTID identifier
  virtual TRTCond::ExpandedIdentifier trtcondid( const Identifier& id, int level = TRTCond::ExpandedIdentifier::STRAW) const override;

  

  /// access to calibration constant containers
  virtual const RtRelationContainer* getRtContainer() const override;
  virtual const RtRelationContainer* getErrContainer() const override;
  virtual const RtRelationContainer* getSlopeContainer() const override;
  virtual const StrawT0Container* getT0Container() const override;
  
 private:
  const TRT_ID* m_trtId;                 //!< id helper

  ///  ReadHandle  keys
  SG::ReadCondHandleKey<RtRelationContainer> m_rtReadKey{this,"RtFolderName","/TRT/Calib/RT","r-t relation in-key"};
  SG::ReadCondHandleKey<RtRelationContainer> m_errReadKey{this,"ErrorFolderName","/TRT/Calib/errors2d","error on r in-key"};
  SG::ReadCondHandleKey<RtRelationContainer> m_slopeReadKey{this,"ErrorSlopeFolderName","/TRT/Calib/slopes","slope of error in-key"};
  SG::ReadCondHandleKey<StrawT0Container> m_t0ReadKey{this,"T0FolderName","/TRT/Calib/T0","t0 in-key"};

};

////////////////////////////////////////////////////////////////////////////////////////////
//  inline methods
////////////////////////////////////////////////////////////////////////////////////////////

inline TRTCond::ExpandedIdentifier 
TRT_CalDbTool::trtcondid( const Identifier& id, int level) const
{
  return TRTCond::ExpandedIdentifier( m_trtId->barrel_ec(id),m_trtId->layer_or_wheel(id),
				      m_trtId->phi_module(id),m_trtId->straw_layer(id),
				      m_trtId->straw(id),level ) ;
}

inline const TRTCond::RtRelation*
TRT_CalDbTool::getRtRelation( const Identifier& id, int level ) const 
{ 
  const RtRelationContainer* rc = getRtContainer();
  if(!rc) return 0;
  return rc->get(trtcondid(id,level)) ; 
}

inline const TRTCond::RtRelation*
TRT_CalDbTool::getErrors( const Identifier& id, int level ) const 
{ 
  const RtRelationContainer* rc = getErrContainer();
  if(!rc) return 0;
  return rc->get(trtcondid(id,level)) ; 
}

inline const TRTCond::RtRelation*
TRT_CalDbTool::getSlopes( const Identifier& id, int level ) const
{
  const RtRelationContainer* rc = getSlopeContainer();
  if(!rc) return 0;
  return rc->get(trtcondid(id,level)) ;
}

inline float 
TRT_CalDbTool::getT0( const Identifier& id, int level ) const 
{
  const StrawT0Container* rc = getT0Container();
  return rc->getT0(trtcondid(id,level)) ; 
}



#endif //  TRT_CALDBTOOL_H
