/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TESTCASCADEALG_H
#define TESTCASCADEALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "JpsiUpsilonTools/JpsiFinder.h"
#include "TH1D.h"
class TestCascadeAlg : public AthAlgorithm{


public:
    TestCascadeAlg( const std::string& name, ISvcLocator* svcLoc );
    StatusCode initialize() override;
    StatusCode execute() override;


private:
    ToolHandle < Trk::TrkVKalVrtFitter > m_iVertexFitter;
    ToolHandle<Analysis::JpsiFinder>      m_jpsiFinder;
    bool m_constrV0 = true;
    bool m_constrJpsi = true;
    int m_perELimit =10;
};

#endif
