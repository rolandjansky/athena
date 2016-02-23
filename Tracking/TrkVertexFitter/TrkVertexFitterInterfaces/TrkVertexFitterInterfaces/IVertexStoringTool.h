/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IVertexStoringTool.h, (c) ATLAS Detector software 2016
///////////////////////////////////////////////////////////////////

#ifndef TRK_IVERTEXSTORINGTOOL_H
#define TRK_IVERTEXSTORINGTOOL_H

#include "GaudiKernel/IAlgTool.h"

//xAOD includes
#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/VertexContainerFwd.h"
#include "xAODTracking/VertexAuxContainer.h"

namespace Trk
{

  static const InterfaceID IID_IVertexStoringTool("IVertexStoringTool", 1, 0);

  /**
   @class IVertexStoringTool
   @brief Interface class for storing vertices in a named xAOD::VertexContainer in StoreGate.

   A given instance of this tool manages a single, named VertexContainer in the event store.

   Caller must check validity of returned ElementLink.
  */

  class IVertexStoringTool : virtual public IAlgTool {

  public:
    /** Virtual destructor */
    virtual ~IVertexStoringTool(){};

    /** AlgTool interface methods */
    static const InterfaceID& interfaceID() { return IID_IVertexStoringTool; };

    /** Interface for null (caller can set properties using pointer returned) */
    virtual xAOD::Vertex* storeVertex() = 0;

    /** Interface for xAOD::Vertex */
    virtual xAOD::Vertex* storeVertex(const xAOD::Vertex& myVertex) = 0;

    /** Interface for 3-vector **/
    virtual xAOD::Vertex* storeVertex(const Amg::Vector3D& myVector) = 0;

    /** Interface for 3-vector and covariance matrix **/
    virtual xAOD::Vertex* storeVertex(const Amg::Vector3D& myVector, const AmgSymMatrix(3)& myMatrix) = 0;

  };
}

#endif
