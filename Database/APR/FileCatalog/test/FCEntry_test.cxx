/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FileCatalog/FCEntry.h"
#include <iostream> 
using namespace pool;
int main()
{
  PFNEntry p1("pfn1","guid1","root");
  PFNEntry p2("pfn2","guid2","root");
  std::vector<PFNEntry> ps;
  ps.push_back(p1);
  ps.push_back(p2);
  std::vector<PFNEntry>::iterator i;
  for(i=ps.begin(); i!=ps.end();++i){
    std::cout<<(*i)<<std::endl;
    std::cout<<(*i).guid()<<std::endl;
    std::cout<<(*i).pfname()<<std::endl;
    std::cout<<(*i).filetype()<<std::endl;
  }

  LFNEntry l1("lfn1","guid1");
  LFNEntry l2("lfn2","guid2");
  std::vector<LFNEntry> ls;
  ls.push_back(l1);
  ls.push_back(l2);
  std::vector<LFNEntry>::iterator j;
  for(j=ls.begin(); j!=ls.end();++j){
    std::cout<<(*j)<<std::endl;
    std::cout<<(*j).guid()<<std::endl;
    std::cout<<(*j).lfname()<<std::endl;
  }

  //test copy
  LFNEntry me("lfn0","guid0");
  LFNEntry cme(me);
  std::cout<<me<<std::endl;
  std::cout<<cme<<std::endl;

  MetaDataEntry meta;
  std::cout<<meta<<std::endl;
  meta.addAttributeSpec("owner","string");
  meta.addAttributeSpec("category","int");
  meta.setGuidValue("guid1");
  meta.setAttributeValue("owner", std::string("me"));
  meta.setAttributeValue("category", 1);
  std::cout<<"again : "<<meta<<std::endl;
  std::cout<<meta.getGuidValue()<<std::endl;
  std::cout<<meta.getAttributeValue<std::string>("owner")<<std::endl;
  int o=meta.getAttributeValue<int>("category");
  std::cout<<o<<std::endl;
  MetaDataEntry metaclone(meta);
  std::cout<<"printing copy"<<std::endl;
  std::cout<<metaclone<<std::endl;
  return 0;
}
