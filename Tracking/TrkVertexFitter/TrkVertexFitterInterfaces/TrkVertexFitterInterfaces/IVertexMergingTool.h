/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IVertexMergingTool.h, (c) ATLAS Detector software 2009
///////////////////////////////////////////////////////////////////

#ifndef TRK_IVERTEXMERGINGTOOL_H
#define TRK_IVERTEXMERGINGTOOL_H

#include "GaudiKernel/IAlgTool.h"

//xAOD includes
#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexContainerFwd.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/TrackParticleContainerFwd.h"

namespace Trk
{

  static const InterfaceID IID_IVertexMergingTool("IVertexMergingTool", 1, 0);

  /**
   @class IVertexMergingTool
   @brief Interface class for merging compatible vertices in a single collection.

   For more detailed information, take a look at the header file of the actual
   implementation files.

   -----------------------------------------
   Changes:

   David Shope <david.richard.shope@cern.ch> (2016-04-26)

     EDM Migration to xAOD - remove method using VxCandidate

  */

  class IVertexMergingTool : virtual public IAlgTool {

  public:
    /** Virtual destructor */
    virtual ~IVertexMergingTool(){};

    /** AlgTool interface methods */
    static const InterfaceID& interfaceID() { return IID_IVertexMergingTool; };

    /** Interface for xAOD vertices **/
    virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
    mergeVertexContainer(const xAOD::VertexContainer& MyVxCont) const = 0;
  };
}

#endif
