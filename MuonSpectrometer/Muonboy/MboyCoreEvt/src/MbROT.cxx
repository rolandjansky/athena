/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MbROT.h"

MbROT::MbROT():MbPOT()
{ 

 Type_ROT () ;
 
 m_LinkPrd = -1 ;

}
MbROT::~MbROT(){}

void MbROT::SetLinkPrd(int LinkPrd) { m_LinkPrd = LinkPrd ; }

int MbROT:: GetLinkPrd() const { return m_LinkPrd ; }
