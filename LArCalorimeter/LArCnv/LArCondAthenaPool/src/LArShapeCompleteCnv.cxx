/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  MsgStream log(messageService(), "LArShapeCompleteCnv" ); 
    //log << MSG::DEBUG << "LArShapeComplete write" << endreq;
    LArShapePersType2* persObj = TPconverter2.createPersistent( transObj, log );
    //log << MSG::DEBUG << "Success" << endreq;
    return persObj; 
}

LArConditionsSubset<LArShapeP2>*
LArShapeCompleteCnv::createTransient () 
{
  static pool::Guid   p2_guid("5139AF4A-5947-421A-A775-B2D1134145C7");
  static pool::Guid   p1_guid("95B61750-4C45-412D-B4D4-9758E9DB40D1");
  static pool::Guid   p0_guid("055CF2F5-08D0-4EAA-B154-8CE5B1A599E7");

  MsgStream log(messageService(), "LArShapeCompleteCnv" ); 
  if( compareClassGuid(p2_guid) ) {  
    std::auto_ptr< LArShapeSubset_p2 > col_vect( poolReadObject< LArShapeSubset_p2 >() );  
    log << MSG::DEBUG << "Reading LArShapeSubset_p1" << endreq; 
    return TPconverter2.createTransient( col_vect.get(), log );
  } 
  else if( compareClassGuid(p1_guid) ) {  
    log << MSG::ERROR << "Sorry if you really want to read LArShapeSubset_p1 you will need to provide a legacy converter" << endreq;
    // using auto_ptr ensures deletion of the persistent object
    //std::auto_ptr< LArShapeSubset_p1 > col_vect( poolReadObject< LArShapeSubset_p1 >() );
    //log << MSG::DEBUG << "Reading LArShapeSubset_p1" << endreq; 
    //return TPconverter1.createTransient( col_vect.get(), log );
  }
  else if( compareClassGuid(p0_guid) ) {
    log << MSG::ERROR << "Sorry if you really want to read the very old LArShapeSubset you will need to provide a legacy converter" << endreq;
    // subset from before TP separation
    //log << MSG::DEBUG << "Reading LArShapeSubset (original)" << endreq; 
    //std::auto_ptr< LArConditionsSubset<LArShapeP> > subset ( poolReadObject< LArConditionsSubset<LArShapeP> >() );
    // Here we must convert from LArShapeP to LArShapeP1
    //log << MSG::VERBOSE << "subset ptr " << subset.get() << endreq; 
    //return (createTransient(subset.get()));
  } 
  throw std::runtime_error("Unsupported persistent version of LArShapeCompleteCnv");
}

LArConditionsSubset<LArShapeP1>* 
LArShapeCompleteCnv::createTransient(LArConditionsSubset<LArShapeP>* orig)
{

  //MsgStream log(messageService(), "LArShapeCompleteCnv" ); 
  //log << MSG::DEBUG << "LArShapeCompleteCnv::createTransient orig " << orig << endreq; 

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
    // Get the number of febs and corrections
    unsigned int nFebs       = oldShape->m_subset.size();
    unsigned int nCorrs      = oldShape->m_correctionVec.size();

    //log << MSG::DEBUG << "LArShapeCompleteCnv::createTransient oldShape 1, nFebs, nCorrs " 
    //    << nFebs << " " << nCorrs << endreq; 

    // Copy conditions

    // Resize subset
    newShape->m_subset.resize(nFebs);
    
    // Loop over febs
    for (unsigned int i = 0; i < nFebs; ++i){
        newShape->m_subset[i].first = oldShape->m_subset[i].first;
	unsigned nChannels=oldShape->m_subset[i].second.size();
        newShape->m_subset[i].second.resize(nChannels);
        // Loop over channels in feb
        for (unsigned int j = 0; j < nChannels; ++j){
            // reserve space for shape vec
            unsigned int nShapes = oldShape->m_subset[i].second[j].m_vShape.size();
            newShape->m_subset[i].second[j].m_vShape.resize(nShapes);
            // Loop over shapes per channel
            for (unsigned int k = 0; k < nShapes; ++k){
                newShape->m_subset[i].second[j].m_vShape[k] = 
                    oldShape->m_subset[i].second[j].m_vShape[k];
            }
            // reserve space for shape der vec
            unsigned int nShapeDers = oldShape->m_subset[i].second[j].m_vShapeDer.size();
            newShape->m_subset[i].second[j].m_vShapeDer.resize(nShapeDers);
            // Loop over shapeders per channel
            for (unsigned int k = 0; k < nShapeDers; ++k){
                newShape->m_subset[i].second[j].m_vShapeDer[k] = 
                  toFloat (oldShape->m_subset[i].second[j].m_vShapeDer[k]);
            }
        }
    }

    //log << MSG::DEBUG << "LArShapeCompleteCnv::createTransient oldShape 2 " << oldShape << endreq; 

    // Copy corrections
    newShape->m_correctionVec.resize(nCorrs);

    // Loop over corrections
    for (unsigned int i = 0; i < nCorrs; ++i){
        newShape->m_correctionVec[i].first = oldShape->m_correctionVec[i].first;
        // reserve space for shape vec
        unsigned int nShapes = oldShape->m_correctionVec[i].second.m_vShape.size();
        newShape->m_correctionVec[i].second.m_vShape.resize(nShapes);
        // Loop over shapes per channel
        for (unsigned int k = 0; k < nShapes; ++k){
            newShape->m_correctionVec[i].second.m_vShape[k] = 
                oldShape->m_correctionVec[i].second.m_vShape[k];
        }
        // reserve space for shape der vec
        unsigned int nShapeDers = oldShape->m_correctionVec[i].second.m_vShapeDer.size();
        newShape->m_correctionVec[i].second.m_vShapeDer.resize(nShapeDers);
        // Loop over shapeders per channel
        for (unsigned int k = 0; k < nShapeDers; ++k){
            newShape->m_correctionVec[i].second.m_vShapeDer[k] = 
                oldShape->m_correctionVec[i].second.m_vShapeDer[k];
        }
    }

    // Copy the rest
    newShape->m_gain          = oldShape->m_gain; 
    newShape->m_channel       = oldShape->m_channel;
    newShape->m_groupingType  = oldShape->m_groupingType;
}
