/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PDMAKER_TRTPHASEFILLERTOOL_H
#define TRACKD3PDMAKER_TRTPHASEFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "CommissionEvent/ComTime.h"


namespace D3PD {


class TRTPhaseFillerTool
  : public D3PD::BlockFillerTool<ComTime>
{
public:
  typedef D3PD::BlockFillerTool<ComTime> Base;

  TRTPhaseFillerTool (const std::string& type,
			  const std::string& name,
			  const IInterface* parent);

  virtual StatusCode initialize();
  virtual StatusCode book();

  virtual StatusCode fill (const ComTime &theComTime);

private:

  void clearData();


  /* NTuple variables: **/
  float *m_TRTPhase;

}; // class TRTPhaseFillerTool

} // namespace D3PD

#endif // not TRACKD3PDMAKER_TRTPHASEFILLERTOOL_H
