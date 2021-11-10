/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LayerProviderCond.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRTOOLS_LAYERPROVIDERCOND_H
#define TRKDETDESCRTOOLS_LAYERPROVIDERCOND_H

// Trk
#include "TrkDetDescrInterfaces/ILayerProviderCond.h"
// Gaudi & Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk {

class Layer;
class ILayerBuilderCond;

/** @class LayerProviderCond

  Wrapper around an ILayerBuilderCond to feed into the StagedGeometryBuilder

  @author Andreas.Salzburger@cern.ch
 */
class LayerProviderCond final
  : public AthAlgTool
  , virtual public ILayerProviderCond
{

public:
  /** Constructor */
  LayerProviderCond(const std::string&, const std::string&, const IInterface*);

  /** Destructor */
  virtual ~LayerProviderCond();

  /** initialize */
  virtual StatusCode initialize() override final;

  /** LayerBuilder interface method - returning the layers at negative side */
  virtual std::pair<EventIDRange, const std::vector<Layer*>> negativeLayers(
    const EventContext& ctx) const override final;

  /** LayerBuilder interface method - returning the central layers */
  virtual std::pair<EventIDRange, const std::vector<Layer*>> centralLayers(
    const EventContext& ctx) const override final;

  /** LayerBuilder interface method - returning the layers at negative side */
  virtual std::pair<EventIDRange, const std::vector<Layer*>> positiveLayers(
    const EventContext& ctx) const override final;

  /** Name identification */
  virtual const std::string& identification() const override final;

private:
  /** LayerBuilder interface method - returning the layers at negative side */
  std::pair<EventIDRange, const std::vector<Layer*>> discLayers(
    const EventContext& ctx,
    int posneg) const;

  ToolHandle<ILayerBuilderCond> m_layerBuilder;
};

} // end of namespace

#endif // TRKDETDESCRTOOLS_LAYERPROVIDERCOND_H

