/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COMMONSF_H
#define COMMONSF_H

#include <string>
#include <vector>
#include <map>
#include <assert.h>
#include <iostream>
#include <typeinfo>

#include "TROOT.h"
#include "TClass.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TObject.h"
#include "TLorentzVector.h"
#include "TKey.h"
#include "TParameter.h"

namespace TauCorrUncert {

  class CommonSF : public TObject
  {

  public:

    CommonSF(const std::string& sharepath,
	     const std::string& sSFFileName,
	     bool debug=false);

    ~CommonSF();

  protected:

    std::string ConvertProngToString(int prongness);
    virtual std::string ConvertEtaToString(const float& fEta) = 0;
    typedef std::map<std::string, TH1F*> SFMAP;
    SFMAP* m_mSF;
    bool m_debug;

    template<class T>
    void ReadInputs(TFile* f, std::map<std::string, T>* map)
    {
      if(!f || !f->IsOpen())
	{
	  Fatal("ReadInputs", "Could not open %s", f->GetName());
	  return;
	}
      assert(f->IsOpen());
      TKey *key;
      TIter next(f->GetListOfKeys());
      while ((key = (TKey*)next()))
	{
	  TClass *cl = gROOT->GetClass(key->GetClassName());
	  if (!cl->InheritsFrom("TH1"))
	    continue;
	  T obj = (T)key->ReadObj();
	  obj->SetDirectory(0);
	  (*map)[std::string(obj->GetName())] = obj;
	}
      Info("ReadInputs", "data loaded from %s", f->GetName());
    }

  private:
    ClassDef(CommonSF, 1)

  };
} // namespace TauCorrUncert

#endif // COMMONSF_H
