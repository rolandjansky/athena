/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */

#ifndef INDETD3PDMAKER_TRUTHVERTEXFILLERTOOL_H
#define INDETD3PDMAKER_TRUTHVERTEXFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "AtlasHepMC/GenVertex.h"

namespace D3PD {

typedef HepMC::GenVertex TruthVertexFillerObjectType;
typedef D3PD::BlockFillerTool<TruthVertexFillerObjectType> TruthVertexFillerBase;

class TruthVertexFillerTool
  : public TruthVertexFillerBase
{
public:
  TruthVertexFillerTool (const std::string& type,
			 const std::string& name,
			 const IInterface* parent);

  StatusCode initialize();
  virtual StatusCode book();

  virtual StatusCode fill (const TruthVertexFillerObjectType& p);

private:

  void clearData();

  /* NTuple variables **/
  // position
  float *m_x;
  float *m_y;
  float *m_z;

}; // class TruthVertexFillerTool

} // namespace D3PD

#endif // not INDETD3PDMAKER_TRUTHVERTEXFILLERTOOL_H
