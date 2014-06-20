/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMBD3PDMAKER_CTPRDOFILLERTOOL_H
#define TRIGMBD3PDMAKER_CTPRDOFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include <vector>

class CTP_RDO;

namespace TrigMbD3PD {

typedef CTP_RDO CtpRdoFillerObjectType;
typedef D3PD::BlockFillerTool<CtpRdoFillerObjectType> CtpRdoFillerBase;

class CtpRdoFillerTool
  : public CtpRdoFillerBase
{
public:
  CtpRdoFillerTool (const std::string& type,
		    const std::string& name,
		    const IInterface* parent);

  virtual StatusCode initialize();
  virtual StatusCode book();

  virtual StatusCode fill (const CtpRdoFillerObjectType& p);

private:

  void clearData();

  /* NTuple variables **/
  uint32_t *m_nBC;
  uint32_t *m_L1A_BC;
  uint32_t *m_timeSec;
  uint32_t *m_timeNanoSec;
  std::vector<uint32_t>* m_pit;
  std::vector<uint32_t>* m_tbp;
  std::vector<uint32_t>* m_tap;
  std::vector<uint32_t>* m_tav;

}; // class CtpRdoFillerTool

} // namespace TrigMbD3PD

#endif // not TRIGMBD3PDMAKER_CTPRDOFILLERTOOL_H
