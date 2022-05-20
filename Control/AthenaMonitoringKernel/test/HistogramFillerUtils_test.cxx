/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG
#include <cassert>

#include "../src/HistogramFiller/HistogramFillerUtils.h"
#include "TestTools/expect.h"

#include "TH1F.h"

using Monitored::detail::fillWillRebinHistogram;

void checkRebin() {
  TH1F h("h", "", 3, 0, 3);
  VALUE( fillWillRebinHistogram(h.GetXaxis(), 1.0) ) EXPECTED( false );
  VALUE( fillWillRebinHistogram(h.GetXaxis(), -1.0) ) EXPECTED( true );
  VALUE( fillWillRebinHistogram(h.GetXaxis(), 4.0) ) EXPECTED( true );
}

void checkRebin_string() {
  TH1F h("h", "", 3, 0, 3);
  VALUE( fillWillRebinHistogram(h.GetXaxis(), "a") ) EXPECTED( false );
  h.Fill("a", 1.0);
  VALUE( fillWillRebinHistogram(h.GetXaxis(), "a") ) EXPECTED( false );
  h.Fill("b", 1.0);
  h.Fill("c", 1.0);
  // Now all the available bins have labels
  VALUE( fillWillRebinHistogram(h.GetXaxis(), "c") ) EXPECTED( false );
  VALUE( fillWillRebinHistogram(h.GetXaxis(), "d") ) EXPECTED( true );
  h.Fill("d", 1.0); // adds more than one bin so we have free labels again
  // But this should still be considered as rebinning (ADHI-4881)
  VALUE( fillWillRebinHistogram(h.GetXaxis(), "e") ) EXPECTED( true );
}


int main() {
  checkRebin();
  checkRebin_string();
}
