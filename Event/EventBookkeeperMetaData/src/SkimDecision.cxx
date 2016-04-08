/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// Implementation file for class SkimDecision
// Author: David Cote, September 2008. <david.cote@cern.ch>
/////////////////////////////////////////////////////////////////// 
 
#include "EventBookkeeperMetaData/SkimDecision.h"

////////////////
/// Constructors
////////////////

SkimDecision::SkimDecision()
{ 
  m_name="";
  m_isAccepted=true;
}

SkimDecision::SkimDecision( const SkimDecision& rhs )
{
  m_name=rhs.m_name;
  m_isAccepted=rhs.m_isAccepted;
}
 
void SkimDecision::setName( std::string name ){ m_name=name; }
void SkimDecision::setIsAccepted( bool answer ){ m_isAccepted=answer; }
