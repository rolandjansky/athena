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
    virtual ~NtupleGoodiesFiller() { _treeVec.clear(); varNameVec.clear(); }

    // call for every new event
    Bool_t execute() ;

    Bool_t addTree(TTree* tree);

    inline Bool_t HasTree() const { return !_treeVec.empty(); }
    inline void SetStripPrefix( const TString& stripprefix ) { _stripprefix=stripprefix; }

   protected:
    std::vector<TTree*> _treeVec; //!
    Bool_t firstTask; //!
    std::vector<TString> varNameVec; //!
    TString _stripprefix; //!

    ClassDef(MET::NtupleGoodiesFiller,1)
  };
}

