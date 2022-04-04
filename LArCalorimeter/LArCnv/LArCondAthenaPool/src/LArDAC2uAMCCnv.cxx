/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArDAC2uAMCCnv.cxx
 * @brief AthenaPool converter LArDAC2uAMC
 * @author Walter Lampl <walter.lampl@cern.ch>
 */

#include "LArDAC2uAMCCnv.h"
#include "LArSingleFloatUpdater.h"

LArConditionsSubset<LArSingleFloatP>*
LArDAC2uAMCCnv::createTransient ()
{
  static const pool::Guid   p0_guid("00BB980F-D174-4AD7-A5C7-DC66BA71C4D2");
  // first try the single-float converter
  LArConditionsSubset<LArSingleFloatP>* p=createTransientSingleFloat();
  if (p) 
    return p;
  else if( compareClassGuid(p0_guid) ) {
    // subset from before TP separation    
    MsgStream log(msgSvc(), "LArDAC2uAMCCnv" ); 
    log << MSG::DEBUG << "Reading LArDAC2uASubset (original)" << endmsg;     
    std::unique_ptr< LArConditionsSubset<LArDAC2uAP> > subset ( poolReadObject< LArConditionsSubset<LArDAC2uAP> >() );
    return (createTransient(subset.get()));
    
  } 
  throw std::runtime_error("Unsupported persistent version of LArDAC2uAMC");
}

LArConditionsSubset<LArSingleFloatP>* 
LArDAC2uAMCCnv::createTransient(LArConditionsSubset<LArDAC2uAP>* orig)
{
    MsgStream log(msgSvc(), "LArDAC2uAMCCnv" ); 
    log << MSG::DEBUG << "LArDAC2uAMCCnv::createTransient orig " << orig << endmsg; 

    LArConditionsSubset<LArSingleFloatP>* result = new LArConditionsSubset<LArSingleFloatP>();
    
    // Copy LArDAC2uAP subset to LArSingleFloat Subset
    LArSingleFloatUpdater copier;
    copier.update(orig, result);

    return (result);
}
    
