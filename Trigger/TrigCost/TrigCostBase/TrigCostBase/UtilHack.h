/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_UTILHACK_H
#define ANP_UTILHACK_H

/**********************************************************************************
 * @Package: TrigCostBase
 * @Author : Rustem Ospanov
 *
 * @Brief  : Global utility functions for bypassing insufficient TrigMonEvent interface.
 * 
 **********************************************************************************/

class TrigConfChain;

namespace Anp
{
  bool HasExpressStream(const TrigConfChain &chain);

  double GetExpressPS(const TrigConfChain &chain);

  void SetExpressPS(TrigConfChain &chain, float ps);

  void RemoveChainGroups(TrigConfChain &chain);
}

#endif
