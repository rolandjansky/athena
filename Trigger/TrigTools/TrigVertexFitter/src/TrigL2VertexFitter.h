/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGVERTEXFITTER_TRIGL2VERTEXFITTER_H
#define TRIGVERTEXFITTER_TRIGL2VERTEXFITTER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigInDetToolInterfaces/ITrigL2VertexFitter.h"

class TrigTimer;
class ITrigVertexingTool;
class TrigL2Vertex;

/** This class implements a full vertex fitting algorithm, i.e. the 
  algorithm that estimates the vertex position and refits the track
  parameters. Mathematical details of the algorithm can be found in 
  ATL-COM-DAQ-2007-036 note available at http://cdsweb.cern.ch/record/1062118

  The implementation of the algorithm is based on the "distributed-Kalman-filtering" 
  philosophy - tracks are represented by TrigVertexFittingNodes which incapsulate
  input track parameters and are responsible for updating vertex fit parameters.
  The same representation is used for mass (or any other) constraints - they are derived 
  from  TrigVertexFittingNode and treated mathematically as pseudomeasurements with 
  small variance. 
 
  For more details see documentation for TrigL2Vertex class
*/


class TrigL2VertexFitter: public AthAlgTool, virtual public ITrigL2VertexFitter
{

 public:
  TrigL2VertexFitter( const std::string&, const std::string&, const IInterface* );
  virtual ~TrigL2VertexFitter();
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual StatusCode fit(TrigL2Vertex*);
  virtual StatusCode fitWithConstraints(TrigL2Vertex*);

private:
  
  int m_numIter;
  ToolHandle<ITrigVertexingTool> m_vertexingTool;
  double m_maxChi2Increase;

#define TRIGL2VERTEXFIT_NTIMERS 10
  TrigTimer* m_timer[TRIGL2VERTEXFIT_NTIMERS];
  bool m_timers;
};

#endif
