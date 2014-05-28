/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __CINT__
#include <map>
#include <iostream>
#include "TH1F.h"
#include "dqm_algorithms/RepeatAlgorithm.h"
#endif

TestRepeatAlgorithm_AddReference()
{
  TH1F* hin = new TH1F("hin","hin",100,-1,1);
  hin->FillRandom("gaus",10000);
  hin->SetBinContent(50,0);
  hin->SetBinContent(51,0);  
  hin->SetBinContent(52,0);
  hin->SetLineWidth(2);
  hin->Draw();
  TH1F* href1= new TH1F("href1","href1",100,-1,1);
  href1->SetBinContent(50,100);
  href1->SetBinContent(51,100);
  href1->SetBinContent(52,100);
  href1->SetLineStyle(2);
  href1->SetLineWidth(2);
  href1->SetLineColor(kRed);
  href1->Draw("same");
  TH1F* href2= new TH1F("href2","href2",100,-1,1);
  href2->SetBinContent(50,-100);
  href2->SetBinContent(51,-100);
  href2->SetBinContent(52,-100);
  href2->SetLineStyle(2);
  href2->SetLineWidth(2);
  href2->SetLineColor(kOrange);
  href2->Draw("same");
  //dqm_algorithms::AddReference * algorithm = new dqm_algorithms::AddReference_All_Bins_Filled();
  dqm_algorithms::RepeatAlgorithm * algorithm = new dqm_algorithms::RepeatAlgorithm();
    //  dqm_algorithms::AddReference * algorithm = new dqm_algorithms::AddReference_Bins_Diff_FromAvg();
  
  TObjArray objarr;
  objarr.Add(href1); objarr.Add(href2);
  dqm_core::test::DummyAlgorithmConfig* aconfig = new dqm_core::test::DummyAlgorithmConfig(&objarr);
  aconfig->addParameter("AuxAlgName--AddReference_Bins_Diff_FromAvg", 1);
  aconfig->addParameter("MinStat",100);
  aconfig->addParameter("Coeff",1);
  aconfig->addParameter("NSigma",1);
  aconfig->addGreenThreshold("NBins",62);
  aconfig->addRedThreshold("NBins",70);
  dqm_core::Result* result = algorithm->execute("test",*hin,*aconfig);
  std::cout<<"Result status "<<result->status_<<std::endl;
  // std::map<std::string,double> tags =result->tags_;
  // std::cout << result->tags_.size() << std::endl;
  // int count(0);
  // for (std::map<std::string,double>::const_iterator imap = result->tags_.begin();
  //      imap != result->tags_.end() && count < result->tags_.size(); ++imap, ++count) {
  //   std::cout << imap->first << " " << imap->second << std::endl;
  // }
  //TObject* o = result->getObject();
  //std::cout<<o->GetName()<<" "<<((TObjArray*)o)->GetEntries()<<std::endl;
  //  o->Print("all");
  //TH1* modifiedhisto = (TH1*)result->getObject();
  //modifiedhisto->SetLineColor(kBlue);
  //modifiedhisto->Draw("same");
  algorithm->printDescription();
}
