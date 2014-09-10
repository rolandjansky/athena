/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PDMAKER_VERTEXPURITYFILLERTOOL_H
#define TRACKD3PDMAKER_VERTEXPURITYFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk{
class VxCandidate;
class TrkPriVxPurityTool;
}

namespace D3PD {

typedef Trk::VxCandidate VertexPurityFillerObjectType;
typedef D3PD::BlockFillerTool<VertexPurityFillerObjectType> VertexPurityFillerBase;

class VertexPurityFillerTool
  : public VertexPurityFillerBase
{
public:
  VertexPurityFillerTool (const std::string& type,
			  const std::string& name,
			  const IInterface* parent);

  StatusCode initialize();
  virtual StatusCode book();

  virtual StatusCode fill (const VertexPurityFillerObjectType& p);

private:

  void clearData();

  //purity tool
  ToolHandle < Trk::TrkPriVxPurityTool > m_purityTool;

  /* NTuple variables **/
  // vertex purity
  float *m_sum_in;
  float *m_sum_out;
  float *m_sum_pu;
  float *m_sum_nt;
  int *m_n_in;
  int *m_n_out;
  int *m_n_pu;
  int *m_n_nt;
  float *m_purity;

}; // class VertexPurityFillerTool

} // namespace D3PD

#endif // not TRACKD3PDMAKER_VERTEXPURITYFILLERTOOL_H
