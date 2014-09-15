/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CluperfViewer.h"

CluperfViewer::CluperfViewer(CscTupleSequencer* pseq)
: m_pseq(pseq), m_ient(-1), m_pread(cluperf_reader()) {
  if ( m_pseq ) {
    m_pseq->sequence(cluperf());
    cout << "Number of entries is " << pseq->size() << endl;
  }
}

int CluperfViewer::size() {
  return m_pseq->size();
}

int CluperfViewer::entry() {
  return m_ient;
}

int CluperfViewer::view(int ient) {
  if ( ient > 0 ) m_ient = ient;
  m_pread->GetEntry(m_ient);
  int run = m_pread->run;
  int evt = m_pread->evt;
  int nclu = m_pread->nentry;
  cout << "Run " << run << " event " << evt << " has " << nclu << " clusters" << endl;
  draw_event_clusters(run,evt);
}

int CluperfViewer::next() {
  while ( true ) {
    cout << m_ient << " >= " << size() << endl;
    if ( m_ient>0 && m_ient >= size() ) {
      cout << "No more clusters" << endl;
      return 0;
    }
    m_pread->GetEntry(++m_ient);
    if ( m_pread->nentry ) break;
    else cout << "Skipping event " << m_pread->evt << endl;
  }
  view(m_ient);
}
