/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrkVertexTools_VertexCollectionSortingTool_H
#define TrkVertexTools_VertexCollectionSortingTool_H

#include "TrkVertexFitterInterfaces/IVertexCollectionSortingTool.h"
#include "TrkVertexFitterInterfaces/IVertexWeightCalculator.h"
///
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
// xAOD include
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/VertexContainerFwd.h"
#include "xAODTracking/VertexFwd.h"

/**
 * @class Trk::VertexCollectionSortingTool
 *
 * @author Johanna Bronner, April 2009
 *
 * --------------------------------------
 * Changes:
 *
 * David Shope <david.richard.shope@cern.ch> (2016-04-26)
 *
 *   EDM Migration to xAOD - remove method using VxCandidate
 */

namespace Trk {
class IVertexWeightCalculator;

class VertexCollectionSortingTool final
  : public AthAlgTool
  , virtual public IVertexCollectionSortingTool
{
public:
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  /**
   * constructor
   */
  VertexCollectionSortingTool(const std::string& t,
                              const std::string& n,
                              const IInterface* p);
  /**
   * destructor
   */
  virtual ~VertexCollectionSortingTool() = default;
  /** Sort **/
  virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
  sortVertexContainer(const xAOD::VertexContainer& MyVxCont) const override;

private:
  ToolHandle<Trk::IVertexWeightCalculator> m_iVertexWeightCalculator{
    this,
    "VertexWeightCalculator",
    "Trk::IVertexWeightCalculator"
  };
  Gaudi::Property<std::string> m_decorationName{
    this,
    "decorationName",
    "sumPt2",
    "Name to use for the decoration"
  };

}; // end of class description
} // end of namespace definition

#endif
