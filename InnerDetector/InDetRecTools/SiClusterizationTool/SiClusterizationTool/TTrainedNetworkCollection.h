/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TTrainedNetworkCollection_H_
#define _TTrainedNetworkCollection_H_

#include <vector>
#include <memory>
#include "TrkNeuralNetworkUtils/TTrainedNetwork.h"

class TTrainedNetworkCollection 
  :  public std::vector<std::unique_ptr<TTrainedNetwork> >
{
public:
  // for debugging :
  void setNames(const std::vector<std::string> &names) { m_names = names; }
  const vector<std::string> &names() const { return m_names; }

private:
  std::vector<std::string> m_names;
};

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF(TTrainedNetworkCollection, 1316971740, 1)

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF(TTrainedNetworkCollection, 1279883774);

#endif
