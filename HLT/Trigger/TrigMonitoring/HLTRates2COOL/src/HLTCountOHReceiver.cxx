/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "HLTRates2COOL/HLTCounter.h"
#include "HLTRates2COOL/HLTCountOHReceiver.h"
#include "HLTRates2COOL/HLTCountCoolWriter.h"

#include "CoolKernel/DatabaseId.h"
#include "CoolKernel/Exception.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/IDatabase.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/IObject.h"

#include <ipc/core.h>

#include <owl/regexp.h>
// OH include files
#include <oh/OHRootReceiver.h>
#include <oh/OHSubscriber.h>
//IS
#include <is/infoT.h>
#include <is/infodictionary.h>
#include <is/infoiterator.h>
#include <is/infodynany.h>

#include "TH2I.h"

using namespace std;

//todo need run number!!!

hltca::HLTCountOHReceiver::HLTCountOHReceiver() 
{
   // 6000 is the current maximum of LB's in one run
   fCountStore[0].resize(6000);
   fCountStore[1].resize(6000);
}


void
hltca::HLTCountOHReceiver::receive( OHRootHistogram & h) {
   //ERS_LOG( std::endl << "Receive histogram '" <<  h.getObject()->GetName() << "' with tag '" << h.tag << "'");
   if(h.tag<=0) {
      // ERROR
   } else {
      updateInternalStore(h);
   }
}



void
hltca::HLTCountOHReceiver::updateInternalStore(const OHRootHistogram & h) {
   
   //    ERS_LOG (std::endl
   //             << "    name  : " << h.histogram->GetName() << std::endl
   //             << "    title : " << h.histogram->GetTitle() << std::endl
   //             << "    tag/lb: " << h.tag);
   
   // get the trigger level
   unsigned int level = 0;
   std::string histoname(h.histogram->GetName());
   if( histoname.find("TrigSteer_EF") != std::string::npos) {
      level=1;
   } else if( histoname.find("TrigSteer_L2") == std::string::npos) {
      hltca::HLTCountReceivingIssue issue(ERS_HERE,std::string("Did not find TrigSteer_L2/EF in the name of the histogram: ")+histoname, "Test");
      ers::warning(issue);
      return;
   }

   // store the histogram (
   // - first time: clone
   // - second time: update, if statistic increased
   unsigned int lb = (unsigned int)h.tag;
   TH2I* hist2d = (TH2I*)h.histogram.get();
   Stat_t hsum = hist2d->GetSum();
   std::cout << "   " << (level==0?"L2":"EF") << " sum for lb " << lb << " : " << hsum << std::endl;

   Content& content = fCountStore[level][lb];
   if(content.h==0) {
      std::string newname(Form("SignatureAcceptance_%s_%i_store", (level==0?"L2":"EF"), lb));
      content.h = (TH2I*) h.histogram->Clone(newname.c_str());
      content.h->SetDirectory(0);
      content.filled= true;
      content.valid = false;
   } else {
      if( hsum > content.h->GetSum() ) {
         CopyHistContent(*hist2d,*content.h);
         content.valid = false;
      } else if (hsum < content.h->GetSum()) {
         // this just means that some notes have already moved farther ahead and are starting to delete this lumiblock
      } else {
         // No changes
      }
   }

   // for the first histos some initialization has to be done
   if(!HLTCounter::hasBinsSet())
      HLTCounter::setBins(h.histogram->GetYaxis());

   if(fChainCounters[level].size()==0) {
      SetChainCounterBins(h.histogram->GetXaxis(),level);
   }

}

void
hltca::HLTCountOHReceiver::SetChainCounterBins(TAxis *axis, int level) {
   int n(axis->GetLast());
   fChainCounters[level].resize(n+1,0);
   for(int i=1; i<=n; ++i) {
      const char * p(axis->GetBinLabel(i)); // label has the form "<ChainName>_ChCo_<chaincounter>"
      if(*p!='L' and *p!='E') continue;
      const char * u(p);
      while(*++p!='\0') if(*p=='_') u=p;
      int cc = atoi(++u);
      fChainCounters[level][i] = cc;
   }
}

void
hltca::HLTCountOHReceiver::CopyHistContent(const TH2& src, TH2& dest) {
   if(src.GetNbinsX()!=dest.GetNbinsX()) {
      hltca::HLTCountReceivingIssue issue(ERS_HERE, "Updated histogram has different number of bins in X, will not use it");
      ers::error(issue);
      return;
   }
   if(src.GetNbinsY()!=dest.GetNbinsY()) {
      hltca::HLTCountReceivingIssue issue(ERS_HERE, "Updated histogram has different number of bins in Y, will not use it");
      ers::error(issue);
      return;
   }
   for(Int_t x=src.GetNbinsX()+1; x>=0; --x)
      for(Int_t y=src.GetNbinsY()+1; y>=0; --y)
         dest.SetBinContent(x,y,src.GetBinContent(x,y));
}


// accessor to the store
const vector<hltca::HLTCounter>&
hltca::HLTCountOHReceiver::hltCounters(int lumiblock, HLTCounter::TriggerLevel level) {
   int levelIndex = (level==HLTCounter::L2)?0:1;
   Content& c = fCountStore[levelIndex][lumiblock];
   updateHLTCounters(c, level);
   std::cout << "After updating LB " << lumiblock << " (level " << ((level==HLTCounter::L2)?"2":"3") << "): " << c.sum << std::endl;
   return c.counters;
}

void
hltca::HLTCountOHReceiver::updateHLTCounters(Content& c, HLTCounter::TriggerLevel level) {
   if(!c.filled || c.valid) return; // either not filled or already up-to-date
   int levelIndex = (level==HLTCounter::L2)?0:1;
   c.counters.clear();
   for(int binx = 1; binx <= c.h->GetNbinsX(); binx++) {
      int chain_counter = fChainCounters[levelIndex][binx];
      if(chain_counter==0) continue; // groups and streams
      c.counters.push_back(HLTCounter(chain_counter,level));
      c.counters.back().setContent(c.h, binx);
   }
   c.sum=c.h->GetSum();
   c.valid=true;
}

void
hltca::HLTCountOHReceiver::ClearStorage() {
   for(int i=0; i<2; ++i) {
      fCountStore[i].clear();
      fChainCounters[i].clear();
      fCountStore[i].resize(6000);
   }
}

int
hltca::HLTCountOHReceiver::last_LB() const {
   int testlb = fCountStore[0].size()-1;
   while(testlb>=0 && !fCountStore[0][testlb].filled) testlb--;
   return testlb;
}
