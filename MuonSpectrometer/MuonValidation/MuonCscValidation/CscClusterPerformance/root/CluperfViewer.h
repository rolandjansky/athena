/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CluperfViewer.h

#ifndef CluperfViewer_H
#define CluperfViewer_H

class CluperfReader;

class CluperfViewer {

private:  // data
  CscTupleSequencer* m_pseq;
  int m_ient;
  CluperfReader* m_pread;

public:  // methods

  // Constructor.
  CluperfViewer(CscTupleSequencer* pseq);

  // Return the number of entries in the sequence.
  int size();

  // Return the current position in the sequence.
  int entry();

  // Draw an entry.
  int view(int ient);

  // Draw the next entry with clusters.
  int next();

};

#endif
