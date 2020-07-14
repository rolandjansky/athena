/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_MUFASTDATAPREPARATOROPTIONS_H
#define  TRIGL2MUONSA_MUFASTDATAPREPARATOROPTIONS_H

#include "TgcDataPreparatorOptions.h"
#include "MdtDataPreparatorOptions.h"

namespace TrigL2MuonSA {

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class MuFastDataPreparatorOptions
{
   public:
      MuFastDataPreparatorOptions() : m_tgcOptions(), m_mdtOptions(), m_doTgcPatRec(true)
      {};

      ~MuFastDataPreparatorOptions() {};

   public:
      const TrigL2MuonSA::TgcDataPreparatorOptions& tgcOptions() const { return m_tgcOptions; };
      const TrigL2MuonSA::MdtDataPreparatorOptions& mdtOptions() const { return m_mdtOptions; };
      

   public:
      TrigL2MuonSA::TgcDataPreparatorOptions m_tgcOptions;
      TrigL2MuonSA::MdtDataPreparatorOptions m_mdtOptions;
      bool m_doTgcPatRec;
};

}

#endif  // TRIGL2MUONSA_MUFASTDATAPREPARATOROPTIONS_H
