/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMBD3PDMAKER_CTPDECISIONFILLERTOOL_H
#define TRIGMBD3PDMAKER_CTPDECISIONFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include <vector>

class CTP_Decision;

namespace TrigMbD3PD {

typedef CTP_Decision CtpDecisionFillerObjectType;
typedef D3PD::BlockFillerTool<CtpDecisionFillerObjectType> CtpDecisionFillerBase;

class CtpDecisionFillerTool
  : public CtpDecisionFillerBase
{
public:
  CtpDecisionFillerTool (const std::string& type,
			 const std::string& name,
			 const IInterface* parent);

  virtual StatusCode initialize();
  virtual StatusCode book();

  virtual StatusCode fill (const CtpDecisionFillerObjectType& p);

private:

  void clearData();

  /* NTuple variables **/
  std::vector<std::string>* m_ctpDecisionItems;
  std::vector<uint32_t>* m_ctpDecisionWords;
  unsigned int *m_ctpDecisionTypeWord;
  unsigned int *m_ctpNDecisionItems;

}; // class CtpDecisionFillerTool

} // namespace TrigMbD3PD

#endif // not TRIGMBD3PDMAKER_CTPDECISIONFILLERTOOL_H
