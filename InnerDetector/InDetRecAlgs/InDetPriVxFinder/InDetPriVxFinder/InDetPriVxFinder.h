/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          InDetPriVxFinder.h  -  Description
                             -------------------
    begin   : 27-01-2004
    authors : Andreas Wildauer (CERN PH-ATC), Fredrik Akesson (CERN PH-ATC)
    email   : andreas.wildauer@cern.ch, fredrik.akesson@cern.ch
    changes :
 ***************************************************************************/

#ifndef INDETPRIVXFINDER_INDETPRIVXFINDER_H
#define INDETPRIVXFINDER_INDETPRIVXFINDER_H
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

/** Primary Vertex Finder.
  InDetPriVxFinder uses the InDetPrimaryVertexFinderTool in the package
  InnerDetector/InDetRecTools/InDetPriVxFinderTool. It only gives the trackcollection from storegate to it
  and records the returned VxContainer.
 */

/* Forward declarations */

namespace Trk
{
  class IVertexCollectionSortingTool;
  class IVxCandidateXAODVertex;  
}

namespace InDet
{
  class IVertexFinder;
  
  class InDetPriVxFinder : public AthAlgorithm
  {
  public:
    InDetPriVxFinder(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~InDetPriVxFinder();
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();
  private:
    std::string m_tracksName;             //!< Name of track container in StoreGate
    std::string m_vxCandidatesOutputName; //!< Name of output container to store results
    std::string m_vxCandidatesOutputNameAuxPostfix; //!< Postfix of output auxiliary container to store results (xAOD only)

    ToolHandle< IVertexFinder > m_VertexFinderTool;
    ToolHandle<Trk::IVertexCollectionSortingTool > m_VertexCollectionSortingTool;
    ToolHandle< Trk::IVxCandidateXAODVertex > m_VertexEdmFactory;
    
    bool m_doVertexSorting;

    // for summary output at the end
    unsigned int m_numEventsProcessed;
    unsigned int m_totalNumVerticesWithoutDummy;
  };
}
#endif
