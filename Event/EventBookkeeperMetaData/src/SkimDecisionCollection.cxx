/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// Implementation file for class SkimDecisionCollection
// Author: David Cote, September 2008. <david.cote@cern.ch>
/////////////////////////////////////////////////////////////////// 
 
#include "EventBookkeeperMetaData/SkimDecisionCollection.h"

///////////////////////////////////////////////////////////////////
/// Constructors
////////////////

SkimDecisionCollection::SkimDecisionCollection(): DataVector<SkimDecision> (SG::OWN_ELEMENTS) {}

SkimDecisionCollection::SkimDecisionCollection( const SkimDecisionCollection& rhs ) : 
   DataVector<SkimDecision> (rhs) {}
 
SkimDecisionCollection& SkimDecisionCollection::operator=( const SkimDecisionCollection& rhs ) 
   {
     if ( this != &rhs ) {
       DataVector<SkimDecision>::operator=(rhs);
     }
   return *this;
   }

SkimDecisionCollection::SkimDecisionCollection( const SG::OwnershipPolicy own ) :
  DataVector<SkimDecision>(own)
{}

