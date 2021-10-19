/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef _TrkVKalVrtCore_PGRAPH_H
#define _TrkVKalVrtCore_PGRAPH_H

namespace Trk {
  class PGraph{

    short int m_teit[999000]	/* was [2][499500] */, m_sett[499500];
    long int m_vset[1000], m_wset[1000];
    long int m_tabnr;
    long int m_lteit[1000], m_lweit, m_lsett[1000];
    long int m_lvset, m_lwset, m_choice;

public:
    PGraph() { } //Prevent zeroing of large arrays.
    int pgraphm_(long int *weit, long int edges, long int nodes, long int *set, long int *nptr, long int nth) noexcept;
    static void trevni_(long int *from, long int length, long int *to, long int maxv, long int *newlng, long int *work) noexcept;

  };
}

#endif
