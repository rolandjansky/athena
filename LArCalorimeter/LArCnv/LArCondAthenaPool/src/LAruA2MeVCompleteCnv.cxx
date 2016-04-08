/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LAruA2MeVCompleteCnv.cxx
 * @brief AthenaPool converter LAruA2MeVComplete
 * @author Walter Lampl <walter.lampl@cern.ch>
 */

#include "LAruA2MeVCompleteCnv.h"
#include "LArSingleFloatUpdater.h"

LArConditionsSubset<LArSingleFloatP>*
LAruA2MeVCompleteCnv::createTransient ()
{
  static pool::Guid   p0_guid("CBF815A2-F18C-4C18-9E93-A8EE67CEE59F");
  // first try the single-float converter
  LArConditionsSubset<LArSingleFloatP>* p=createTransientSingleFloat();
  if (p) 
    return p;
  else if( compareClassGuid(p0_guid) ) {
    // subset from before TP separation    
    MsgStream log(messageService(), "LAruA2MeVCompleteCnv" ); 
    log << MSG::DEBUG << "Reading LAruA2MeVSubset (original)" << endreq;     
    std::auto_ptr< LArConditionsSubset<LAruA2MeVP> > subset ( poolReadObject< LArConditionsSubset<LAruA2MeVP> >() );
    return (createTransient(subset.get()));
    
  } 
  throw std::runtime_error("Unsupported persistent version of LAruA2MeVComplete");
}

LArConditionsSubset<LArSingleFloatP>* 
LAruA2MeVCompleteCnv::createTransient(LArConditionsSubset<LAruA2MeVP>* orig)
{
    MsgStream log(messageService(), "LAruA2MeVCompleteCnv" ); 
    log << MSG::DEBUG << "LAruA2MeVCompleteCnv::createTransient orig " << orig << endreq; 

    LArConditionsSubset<LArSingleFloatP>* result = new LArConditionsSubset<LArSingleFloatP>();
    
    // Copy LAruA2MeVP subset to LArSingleFloat Subset
    LArSingleFloatUpdater copier;
    copier.update(orig, result);

    return (result);
}
    
