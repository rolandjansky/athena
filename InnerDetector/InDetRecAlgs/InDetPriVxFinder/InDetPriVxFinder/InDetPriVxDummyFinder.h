/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          InDetPriVxDummyFinder.h  -  Description
                             -------------------
 ***************************************************************************/

#ifndef INDETPRIVXFINDER_INDETPRIVXDUMMYFINDER_H
#define INDETPRIVXFINDER_INDETPRIVXDUMMYFINDER_H
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

/** DUMMY Primary Vertex Finder.
  InDetPriVxDummyFinder just fills one dummy vertex into the vertex container. The position and error of this
  vertex are that of the beamspot or 0,0,0 if no beam spot is available. The vertex will have no tracks assigned
  to it and will be of type Trk::NoVtx
 */

/* Forward declarations */
class IBeamCondSvc;

namespace InDet
{
  class InDetPriVxDummyFinder : public AthAlgorithm
  {
  public:
    InDetPriVxDummyFinder(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~InDetPriVxDummyFinder();
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();
  private:
    ServiceHandle<IBeamCondSvc> m_iBeamCondSvc;
    std::string m_vxCandidatesOutputName; //!< Name of output container to store results
  };
}
#endif
