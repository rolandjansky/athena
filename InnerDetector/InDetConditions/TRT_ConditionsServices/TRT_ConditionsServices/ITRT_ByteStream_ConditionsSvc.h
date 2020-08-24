/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRT_BYTESTREAM_CONDITIONSSVC_H
#define ITRT_BYTESTREAM_CONDITIONSSVC_H

/** @file ITRT_ByteStream_ConditionsSvc.h
 *  @brief Service for accessing TRT ByteStream conditions information.
 *  @author Denver Whittington
 *  @author Paul Keener
 */

#include <set>
#include "GaudiKernel/IService.h"

namespace TRTByteStreamErrors {
  enum errorTypes {
    BCIDError=0,
    L1IDError,
    MISSINGError,
    ERRORError,
    SIDError,
    RobStatusError
  };
}


class ITRT_ByteStream_ConditionsSvc : virtual public IService {
 public:

   static const InterfaceID& interfaceID() ;

   virtual void resetSets( void ) = 0;
   virtual void resetCounts( void ) = 0;

   /*
    * !!! WARNING !!!
    * getNumberOfErrors( int ) is an obsolete interface.  It is not guaranteed to be maintained
    * to even to be correct.  It only supports error types defined as of 1 July 2009
    */
   int getNumberOfErrors( int ErrType);

   /*
    * Use getNumberOfErrors( TRTByteStreamErrors::errorTypes ) instead!
    */
   virtual int getNumberOfErrors( TRTByteStreamErrors::errorTypes ErrType) = 0;

   virtual const std::set<std::pair<uint32_t,uint32_t> > &getL1idErrorSet( void ) = 0;
   virtual const std::set<std::pair<uint32_t,uint32_t> > &getBcidErrorSet( void ) = 0;
   virtual const std::set<std::pair<uint32_t,uint32_t> > &getRobErrorSet( void ) = 0;
   virtual const std::set<uint32_t> & getMissingErrorSet( void ) = 0;
   virtual const std::set<uint32_t> & getErrorErrorSet( void ) = 0;
   virtual const std::set<uint32_t> & getSidErrorSet( void ) = 0;

   virtual std::set<std::pair<uint32_t,uint32_t> > * getRodRobErrorSet( TRTByteStreamErrors::errorTypes ErrType ) = 0;
   virtual std::set<std::pair<uint32_t,uint32_t> > * getIdErrorSet( TRTByteStreamErrors::errorTypes ErrType ) = 0;
   virtual std::set<uint32_t> * getErrorSet( TRTByteStreamErrors::errorTypes ErrType ) = 0;

   virtual void add_missing_error( uint32_t DTMROC_idx ) = 0;
   virtual void add_error_error( uint32_t DTMROC_idx ) = 0;
   virtual void add_sid_error( uint32_t DTMROC_idx ) = 0;

   virtual void add_l1id_error( uint32_t DTMROC_idx, uint32_t l1id ) = 0;
   virtual void add_bcid_error( uint32_t DTMROC_idx, uint32_t bcid ) = 0;

   virtual void add_rob_error( uint32_t robSourceId, uint32_t robStatus ) = 0;
   
};

inline const InterfaceID& ITRT_ByteStream_ConditionsSvc::interfaceID() {
  static const InterfaceID id("ITRT_ByteStream_ConditionsSvc",1,0);
  return id; 
}


/*
 * !!! WARNING !!!
 * getNumberOfErrors( int ) is an obsolete interface.  It is not guaranteed to be maintained
 * to even to be correct.  It only supports error types defined as of 1 July 2009.
 *
 * Use getNumberOfErrors( TRTByteStreamErrors::errorTypes ) instead!
 */
inline int
ITRT_ByteStream_ConditionsSvc::getNumberOfErrors( int ErrType )
{
  switch( ErrType )
  {
  case 0:
    return getNumberOfErrors( TRTByteStreamErrors::BCIDError );
  case 1:
    return getNumberOfErrors( TRTByteStreamErrors::L1IDError );
  case 2:
    return getNumberOfErrors( TRTByteStreamErrors::MISSINGError );
  case 3:
    return getNumberOfErrors( TRTByteStreamErrors::ERRORError );
  case 4:
    return getNumberOfErrors( TRTByteStreamErrors::SIDError );
  default:
    return 0;
  }
}


#endif // ITRT_BYTESTREAM_CONDITIONSSVC_H
