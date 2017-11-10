// emacs -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Created by Davide Gerbaudo on 2017-11-10
// Based on the LAr algorithm by Joerg Stelzer and Veronica Sorin

#ifndef _L1TOPOALGORITHMS_ETAPHIWINDOW_
#define _L1TOPOALGORITHMS_ETAPHIWINDOW_

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

namespace TCS {
/**
   @brief TODO

   @param TODO
*/
class EtaPhiWindow : public DecisionAlg {
public:
    EtaPhiWindow(const std::string & name);
    virtual ~EtaPhiWindow();

    virtual StatusCode initialize();
    virtual StatusCode processBitCorrect(const std::vector<TCS::TOBArray const *> &input,
                                         const std::vector<TCS::TOBArray *> &output,
                                         Decision & decison);
    virtual StatusCode process(const std::vector<TCS::TOBArray const *> &input,
                               const std::vector<TCS::TOBArray *> &output,
                               Decision &decison);
private:
    parType_t      p_NumberLeading1 = { 0 };
    parType_t      p_MinET = { 0 };
    parType_t      p_EtaMin = { 0 };
    parType_t      p_EtaMax = { 0 };
    parType_t      p_PhiMax = { 0 };
    parType_t      p_PhiMin = { 0 };
};

}

#endif
