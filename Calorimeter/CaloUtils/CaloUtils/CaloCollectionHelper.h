/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOUTILS_CALOCOLLECTIONHELPER_H
#define CALOUTILS_CALOCOLLECTIONHELPER_H
///////////////////////////////////////////////////////////////////////////////
/// \brief some use full helper functions for all CaloEvent collections
///
/// CaloCollectionHelper contains a set of functions supporting the retrieval
/// and recording of CaloEvent data objects.
///
///////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/StatusCode.h"

#include "StoreGate/StoreGateSvc.h"

#include <string>

struct CaloCollectionHelper
{
  public:
  /// record and lock a storable object into TES
  template<typename CONT>
    static StatusCode recordStorable(StoreGateSvc* theStore,
				     CONT* theStorable,
				     const std::string& theKey)
    {
      // invalid pointer
      if ( theStore == 0 || theStorable == 0 ) { return StatusCode::FAILURE; }

      // record and lock
      return (theStore->record(theStorable,theKey)).isSuccess()
	? theStore->setConst(theStorable)
	: StatusCode::FAILURE;
    }

  /// record, lock and symLink a storable object into TES
  template<typename CONT, typename COLL>
    static StatusCode recordStorable(StoreGateSvc* theStore,
				     CONT* theStorable,
				     const std::string& theKey,
				     COLL* theAltStorable)
    {
      // check input
      if ( theStore == 0 || theStorable == 0 ) return StatusCode::FAILURE;
      // copy if possible
      StatusCode checkOut(StatusCode::SUCCESS);
      if ( theAltStorable != 0 )
	{
	  typedef  typename COLL::base_value_type object_t;
	  typename CONT::const_iterator fObj = theStorable->begin();
	  typename CONT::const_iterator lObj = theStorable->end();
	  for ( ; fObj != lObj; fObj++ )
	    { theAltStorable->push_back((object_t*)(*fObj)); }
	  // problem
	  if ( theAltStorable->size() != theStorable->size() )
	    { return StatusCode::FAILURE; }
	  // all ok up to here
	  else
	    {
	      checkOut = recordStorable<COLL>(theStore,theAltStorable,theKey);
	    }
	}
      // record and lock original container
      return checkOut.isSuccess()
	? recordStorable<CONT>(theStore,theStorable,theKey)
	: checkOut;
    }
};
#endif
