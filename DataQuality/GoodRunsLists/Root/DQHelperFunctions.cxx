/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GoodRunsLists/DQHelperFunctions.h"
#include "GoodRunsLists/TGoodRunsListReader.h"

void 
DQ::SetXMLFile(const TString& filename)
{
 Root::TGoodRunsListReader reader;   
 reader.SetXMLFile(filename);
 reader.Interpret();                                                
 DQ::GetGRL() = reader.GetMergedGoodRunsList() ;   
}


Root::TGoodRunsList& 
DQ::GetGRL()
{
  static Root::TGoodRunsList internalgrl("internalgrl");
  return internalgrl;
}


bool 
DQ::PassRunLB(int runnr, int lbnr)
{
  return DQ::GetGRL().HasRunLumiBlock(runnr,lbnr);
}

