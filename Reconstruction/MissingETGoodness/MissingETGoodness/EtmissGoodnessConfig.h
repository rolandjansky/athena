/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __MissingETGoodness_EtmissGoodnessConfig_H__
#define __MissingETGoodness_EtmissGoodnessConfig_H__

#include <map>
#include <vector>
#include "TString.h"
#include "MissingETGoodness/TMsgLogger.h"

namespace MET
{
#ifdef METCONFIGINST
  std::map< TString, std::vector<TString> > ConfigMap;
#else
  extern std::map< TString, std::vector<TString> > ConfigMap;
#endif

  Bool_t InitConfigMap();
  std::vector<TString> GetEtmissGoodnessConfig(const TString& qualifier);
  void AddExistingConfigTo(std::vector<TString>& parList, const TString& qualifier);
  void SummaryEtmissGoodnessConfig(const TString& qualifier = "");
  Bool_t ContainsEtmissGoodnessQualifier(const TString& qualifier);
  std::vector<TString> GetEtmissGoodnessQualifierList();
}

#endif

