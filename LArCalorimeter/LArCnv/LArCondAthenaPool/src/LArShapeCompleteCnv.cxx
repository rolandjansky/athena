/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArShapeCompleteCnv.cxx
 * @brief AthenaPool converter LArShapeComplete
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "LArShapeCompleteCnv.h"
#include <cmath>
#include <limits>

static LArShapeSubsetCnv_p1   TPconverter1;
static LArShapeSubsetCnv_p2   TPconverter2;

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

LArShapeSubset_p2*
LArShapeCompleteCnv::createPersistent (LArShapeTransType2* transObj)
{
  MsgStream log(msgSvc(), "LArShapeCompleteCnv" ); 
    //log << MSG::DEBUG << "LArShapeComplete write" << endmsg;
    LArShapePersType2* persObj = TPconverter2.createPersistent( transObj, log );
    //log << MSG::DEBUG << "Success" << endmsg;
    return persObj; 
}

LArConditionsSubset<LArShapeP2>*
LArShapeCompleteCnv::createTransient () 
{
  static pool::Guid   p2_guid("5139AF4A-5947-421A-A775-B2D1134145C7");
  static pool::Guid   p1_guid("95B61750-4C45-412D-B4D4-9758E9DB40D1");
  static pool::Guid   p0_guid("055CF2F5-08D0-4EAA-B154-8CE5B1A599E7");

  MsgStream log(msgSvc(), "LArShapeCompleteCnv" ); 
  if( compareClassGuid(p2_guid) ) {  
    std::unique_ptr< LArShapeSubset_p2 > col_vect( poolReadObject< LArShapeSubset_p2 >() );  
    log << MSG::DEBUG << "Reading LArShapeSubset_p1" << endmsg; 
    return TPconverter2.createTransient( col_vect.get(), log );
  } 
  else if( compareClassGuid(p1_guid) ) {  
    log << MSG::ERROR << "Sorry if you really want to read LArShapeSubset_p1 you will need to provide a legacy converter" << endmsg;
  }
  else if( compareClassGuid(p0_guid) ) {
    log << MSG::ERROR << "Sorry if you really want to read the very old LArShapeSubset you will need to provide a legacy converter" << endmsg;
  } 
  throw std::runtime_error("Unsupported persistent version of LArShapeCompleteCnv");
}

LArConditionsSubset<LArShapeP1>* 
LArShapeCompleteCnv::createTransient(LArConditionsSubset<LArShapeP>* orig)
{

  //MsgStream log(msgSvc(), "LArShapeCompleteCnv" ); 
  //log << MSG::DEBUG << "LArShapeCompleteCnv::createTransient orig " << orig << endmsg; 

    LArConditionsSubset<LArShapeP1>* result = new LArConditionsSubset<LArShapeP1>();
    
    // Copy LArShapeP subset to LArShapeP1
    LArShapeCopy copier;
    copier.copyOldtoNew(orig, result);
    
    return (result);
}

// Copy LArShapeP subset to LArShapeP1
void 
LArShapeCopy::copyOldtoNew(const LArConditionsSubset<LArShapeP>* oldShape,
			   LArConditionsSubset<LArShapeP1>* newShape)
{
  newShape->assign (*oldShape,
                    [] (const LArShapeP& from,
                        LArShapeP1& to)
                    {
                      to.m_vShape.assign (from.m_vShape.begin(),
                                          from.m_vShape.end());

                      unsigned int nShapeDers = from.m_vShapeDer.size();
                      to.m_vShapeDer.resize (nShapeDers);
                      for (size_t k = 0; k < nShapeDers; k++)
                        to.m_vShapeDer[k] = toFloat (from.m_vShapeDer[k]);
                    });
}
