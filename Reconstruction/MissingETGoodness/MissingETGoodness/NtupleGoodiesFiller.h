/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TObject.h"
#include "TString.h"
#include <vector>
#include "TTree.h"

namespace MET {
  class NtupleGoodiesFiller : public TObject
  {
   public:
    NtupleGoodiesFiller() ;
    virtual ~NtupleGoodiesFiller() { m_treeVec.clear(); m_varNameVec.clear(); }

    // call for every new event
    Bool_t execute() ;

    Bool_t addTree(TTree* tree);

    inline Bool_t HasTree() const { return !m_treeVec.empty(); }
    inline void SetStripPrefix( const TString& stripprefix ) { m_stripprefix=stripprefix; }

   protected:
    std::vector<TTree*> m_treeVec; //!
    Bool_t m_firstTask; //!
    std::vector<TString> m_varNameVec; //!
    TString m_stripprefix; //!

    ClassDef(MET::NtupleGoodiesFiller,1)
  };
}

