/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HLTRates2COOL/HLTCounter.h"

#include "TAxis.h"
#include "TH2.h"

int hltca::HLTCounter::g_inputbin   = 0;
int hltca::HLTCounter::g_afterPSbin = 0;
int hltca::HLTCounter::g_rawbin     = 0;
int hltca::HLTCounter::g_totalbin   = 0;

bool
hltca::HLTCounter::hasBinsSet() {
   return g_inputbin != 0;
}

bool
hltca::HLTCounter::setBins(TAxis * axis) {
   int lastbin = axis->GetLast();
   g_inputbin   = axis->FindBin("input");
   g_afterPSbin = axis->FindBin("!PS rate");
   g_rawbin     = axis->FindBin("raw rate");
   g_totalbin   = axis->FindBin("total rate");
   if(g_inputbin>lastbin) {
      // ERROR
      return false;
   }
   if(g_afterPSbin>lastbin) {
      // ERROR
      return false;
   }
   if(g_rawbin>lastbin) {
      // ERROR
      return false;
   }
   if(g_totalbin>lastbin) {
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
   m_input    = round_to_int(h->GetBinContent(binx, g_inputbin));
   m_afterPS  = round_to_int(h->GetBinContent(binx, g_afterPSbin));
   m_raw      = round_to_int(h->GetBinContent(binx, g_rawbin));
   m_total    = round_to_int(h->GetBinContent(binx, g_totalbin));
}

std::ostream & hltca::operator<<(std::ostream & o, const hltca::HLTCounter & c) {
   o << c.level() << " " << c.chain_counter()
     << ": inp=" << c.input()
     << ", aps=" << c.after_prescale()
     << ", raw=" << c.raw()
     << ", tot=" << c.total();
   return o;
}
