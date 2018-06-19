/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HLTRates2COOL/HLTCounter.h"

#include "TAxis.h"
#include "TH2.h"

int hltca::HLTCounter::s_inputbin   = 0;
int hltca::HLTCounter::s_afterPSbin = 0;
int hltca::HLTCounter::s_rawbin     = 0;
int hltca::HLTCounter::s_totalbin   = 0;

bool
hltca::HLTCounter::hasBinsSet() {
   return s_inputbin != 0;
}

bool
hltca::HLTCounter::setBins(TAxis * axis) {
   int lastbin = axis->GetLast();
   s_inputbin   = axis->FindBin("input");
   s_afterPSbin = axis->FindBin("!PS rate");
   s_rawbin     = axis->FindBin("raw rate");
   s_totalbin   = axis->FindBin("total rate");
   if(s_inputbin>lastbin) {
      // ERROR
      return false;
   }
   if(s_afterPSbin>lastbin) {
      // ERROR
      return false;
   }
   if(s_rawbin>lastbin) {
      // ERROR
      return false;
   }
   if(s_totalbin>lastbin) {
      // ERROR
      return false;
   }
   return true;
}

namespace {
   int round_to_int(double v) {
      return int(v+0.5);
   }
}

void
hltca::HLTCounter::setContent(const TH2* h, int binx) {
   m_input    = round_to_int(h->GetBinContent(binx, s_inputbin));
   m_afterPS  = round_to_int(h->GetBinContent(binx, s_afterPSbin));
   m_raw      = round_to_int(h->GetBinContent(binx, s_rawbin));
   m_total    = round_to_int(h->GetBinContent(binx, s_totalbin));
}

std::ostream & hltca::operator<<(std::ostream & o, const hltca::HLTCounter & c) {
   o << c.level() << " " << c.chain_counter()
     << ": inp=" << c.input()
     << ", aps=" << c.after_prescale()
     << ", raw=" << c.raw()
     << ", tot=" << c.total();
   return o;
}
