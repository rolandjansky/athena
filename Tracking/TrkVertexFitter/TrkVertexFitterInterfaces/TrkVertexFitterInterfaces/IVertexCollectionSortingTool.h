/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IVertexCollectionSortingTool.h, (c) ATLAS Detector software 2009
///////////////////////////////////////////////////////////////////

#ifndef TRK_IVERTEXCOLLECTIONSORTINGTOOL_H
#define TRK_IVERTEXCOLLECTIONSORTINGTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include <utility>

//xAOD includes
#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexContainerFwd.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/TrackParticleContainerFwd.h"

namespace Trk
{

  static const InterfaceID IID_IVertexCollectionSortingTool("IVertexCollectionSortingTool", 1, 0);

  /**
   @class IVertexCollectionSortingTool
   @brief Interface class for vertex Container Sorting.

   For more detailed information, take a look at the header file of the actual
   implementation files.

   -----------------------------------------
   Changes:

   David Shope <david.richard.shope@cern.ch> (2016-04-26)

     EDM Migration to xAOD - remove method using VxCandidate

  */

  class IVertexCollectionSortingTool : virtual public IAlgTool {

     public:
       /** Virtual destructor */

       virtual ~IVertexCollectionSortingTool(){};

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IVertexCollectionSortingTool; };


       /** Interface for xAOD::VertexContainer.
	* A new container and auxiliary store is returned.
	*/
       virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
       sortVertexContainer(const xAOD::VertexContainer& MyVxCont) const = 0;
  };
}

#endif
