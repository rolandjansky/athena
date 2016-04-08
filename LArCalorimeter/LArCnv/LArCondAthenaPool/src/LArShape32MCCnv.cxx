/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArShape32MCCnv.cxx
 * @brief AthenaPool converter LArShape32MC
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "LArShape32MCCnv.h"
#include "LArCondTPCnv/LArShapeSubsetCnv_p1.h"
#include "LArShapeCompleteCnv.h"
#include <cmath>
#include <limits>

static LArShapeSubsetCnv_p1   TPconverter;

#if 0
namespace {

/// Helper for double -> float conversion, to avoid overflows.
inline
float toFloat (double x)
{
  if (std::abs (x) > std::numeric_limits<float>::max()) {
    if (x > 0)
      return std::numeric_limits<float>::max();
    else
      return - std::numeric_limits<float>::max();
  }
  return x;
}

} // anonymous namespace
#endif

LArShapeSubset_p1*
LArShape32MCCnv::createPersistent (LArShapeTransType* transObj)
{
  MsgStream log(messageService(), "LArShape32MCCnv" ); 
    //log << MSG::DEBUG << "LArShape32MC write" << endreq;
    LArShapePersType* persObj = TPconverter.createPersistent( transObj, log );
    //log << MSG::DEBUG << "Success" << endreq;
    return persObj; 
}

LArConditionsSubset<LArShapeP1>*
LArShape32MCCnv::createTransient ()
{
    static pool::Guid   p1_guid("95B61750-4C45-412D-B4D4-9758E9DB40D1");
    static pool::Guid   p0_guid("055CF2F5-08D0-4EAA-B154-8CE5B1A599E7");
    MsgStream log(messageService(), "LArShape32MCCnv" ); 
    if( compareClassGuid(p1_guid) ) {  
        // using auto_ptr ensures deletion of the persistent object
        std::auto_ptr< LArShapeSubset_p1 > col_vect( poolReadObject< LArShapeSubset_p1 >() );
        //
        log << MSG::DEBUG << "Reading LArShapeSubset_p1" << endreq; 
        return TPconverter.createTransient( col_vect.get(), log );
    }
    else if( compareClassGuid(p0_guid) ) {
        // subset from before TP separation
        log << MSG::DEBUG << "Reading LArShapeSubset (original)" << endreq; 

        std::auto_ptr< LArConditionsSubset<LArShapeP> > subset ( poolReadObject< LArConditionsSubset<LArShapeP> >() );
        // Here we must convert from LArShapeP to LArShapeP1
        
        log << MSG::VERBOSE << "subset ptr " << subset.get() << endreq; 

        return (createTransient(subset.get()));

    } 
    throw std::runtime_error("Unsupported persistent version of LArShape32MCCnv");
}

LArConditionsSubset<LArShapeP1>* 
LArShape32MCCnv::createTransient(LArConditionsSubset<LArShapeP>* orig)
{

  //MsgStream log(messageService(), "LArShapeMCCnv" ); 
  //log << MSG::DEBUG << "LArShapeMCCnv::createTransient orig " << orig << endreq; 

    LArConditionsSubset<LArShapeP1>* result = new LArConditionsSubset<LArShapeP1>();
    
    // Copy LArShapeP subset to LArShapeP1
    LArShapeCopy copier;
    copier.copyOldtoNew(orig, result);
    
    return (result);
}

