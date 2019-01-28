/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4VALIDATION_AODREADER_H
#define LARG4VALIDATION_AODREADER_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/NTuple.h"

#include <string>

class AODReader : public AthAlgorithm {

 public:

   AODReader (const std::string& name, ISvcLocator* pSvcLocator);
   virtual ~AODReader ();

   virtual StatusCode initialize();
   virtual StatusCode execute();
   virtual StatusCode finalize();

 private:

   NTuple::Tuple *m_nt;
   NTuple::Item<float> m_energy;
   NTuple::Item<float> m_eta;
   NTuple::Item<float> m_pt;
   NTuple::Item<float> m_e237;
   NTuple::Item<float> m_e277;
   NTuple::Item<float> m_weta1;
   NTuple::Item<float> m_weta2;
   NTuple::Item<float> m_e2tsts1;
   NTuple::Item<float> m_fracs1;
   NTuple::Item<float> m_wtots1;
   NTuple::Item<float> m_f1;
   NTuple::Item<float> m_f1core;
   NTuple::Item<float> m_et;
   NTuple::Item<float> m_ethad1;
   NTuple::Item<float> m_emins1;

   NTuple::Item<float> m_truth_energy;
   NTuple::Item<float> m_truth_px;
   NTuple::Item<float> m_truth_py;
   NTuple::Item<float> m_truth_pz;

};

#endif
