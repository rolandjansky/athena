/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_MDTDATAPREPARATOROPTIONS_H
#define  TRIGL2MUONSA_MDTDATAPREPARATOROPTIONS_H

namespace TrigL2MuonSA {

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class MdtDataPreparatorOptions
{
   public:
      MdtDataPreparatorOptions() : m_aaa(true) {};
      ~MdtDataPreparatorOptions() {};

   private:
      bool m_aaa;
};

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

}

#endif  // TRIGL2MUONSA_MDTDATAPREPARATOROPTIONS_H
