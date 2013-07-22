/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArMphysOverMcalCompleteCnv.cxx
 * @brief AthenaPool converter LArMphysOverMcalComplete
 * @author walter.lampl@cern.ch
 * $Id: LArMphysOverMcalCompleteCnv.cxx,v 1.2 2008-04-10 14:54:24 wlampl Exp $
 */

#include "LArMphysOverMcalCompleteCnv.h"
#include "LArCondTPCnv/LArMphysOverMcalSubsetCnv_p1.h"
#include "LArSingleFloatUpdater.h"

static LArMphysOverMcalSubsetCnv_p1 TPconverter;

LArConditionsSubset<LArSingleFloatP>*
LArMphysOverMcalCompleteCnv::createTransient ()
{
  static pool::Guid   p0_guid("9C53AC43-3FD6-470F-A6FF-1DF80E85ACBF");
  static pool::Guid   p1_guid("60FB956A-0B7F-450E-BF6A-2A0B8ED55204");
  // first try the single-float converter
  LArConditionsSubset<LArSingleFloatP>* p=createTransientSingleFloat();
  if (p) 
    return p;
  else if( compareClassGuid(p1_guid) ) {
    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr< LArMphysOverMcalSubset_p1 > col_vect( poolReadObject< LArMphysOverMcalSubset_p1 >() );
    MsgStream log(messageService(), "LArMphysOverMcalCompleteCnv" ); 
    //log << MSG::INFO << "Reading LArMphysOverMcalSubset_p1" << endreq; 
    return TPconverter.createTransient( col_vect.get(), log );
  }
  else if( compareClassGuid(p0_guid) ) {
    // subset from before TP separation
    
    MsgStream log(messageService(), "LArMphysOverMcalCompleteCnv" ); 
    log << MSG::DEBUG << "Reading LArMphysOverMcalSubset (original)" << endreq; 
    
    std::auto_ptr< LArConditionsSubset<LArMphysOverMcalP> > subset ( poolReadObject< LArConditionsSubset<LArMphysOverMcalP> >() );
    // Here we must convert from LArMphysOverMcalP to LArMphysOverMcalP1
    
    log << MSG::DEBUG << "subset ptr " << subset.get() << endreq; 
    
    return (createTransient(subset.get()));
    
  } 
  throw std::runtime_error("Unsupported persistent version of LArMphysOverMcalCompleteCnv");
}

LArConditionsSubset<LArSingleFloatP>* 
LArMphysOverMcalCompleteCnv::createTransient(LArConditionsSubset<LArMphysOverMcalP>* orig)
{
    MsgStream log(messageService(), "LArMphysOverMcalCompleteCnv" ); 
    log << MSG::DEBUG << "LArMphysOverMcalCompleteCnv::createTransient orig " << orig << endreq; 

    LArConditionsSubset<LArSingleFloatP>* result = new LArConditionsSubset<LArSingleFloatP>();
    
    // Copy LArMphysOverMcalP subset to LArMphysOverMcalP1
    LArSingleFloatUpdater copier;
    copier.update(orig, result);

    return (result);
}
