// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDMaker/src/egammaTraversedMaterialFillerTool.h
 * @author Bruno Lenzi <Bruno.Lenzi@cern.ch>
 * @date May, 2012
 * @brief Block filler tool for the material traversed by a photon/electron until the entrance of the calorimeter (in X0)
 */

#ifndef EGAMMAD3PDMAKER_EGAMMATRAVERSEDMATERIALFILLERTOOL_H
#define EGAMMAD3PDMAKER_EGAMMATRAVERSEDMATERIALFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/Egamma.h"
#include "TrkParameters/TrackParameters.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include <string>

class ICaloSurfaceBuilder;
namespace Trk { class IExtrapolator; }

namespace D3PD {


/**
 * @brief Block filler tool for the material traversed by a photon/electron until the entrance of the calorimeter (in X0)
 */
class egammaTraversedMaterialFillerTool
  : public BlockFillerTool<Types<xAOD::Electron, xAOD::Photon> >
{
public:
  typedef BlockFillerTool<Types<xAOD::Electron, xAOD::Photon> > Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  egammaTraversedMaterialFillerTool (const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent);

  virtual ~egammaTraversedMaterialFillerTool();
  
  StatusCode initialize();

  /// Book variables for this block.
  virtual StatusCode book();


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill (const xAOD::Electron& p);


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill (const xAOD::Photon& p);


private:
  /**
   * @brief Common part.
   * @param eg The input egamma object.
   * @param parameters Track parameters to extrapolate.
   */
  StatusCode fill (const xAOD::Egamma& eg,
                   const Trk::TrackParameters& parameters);

  /// Variable: Material traversed between vertex (conversion vertex)
  /// and the entrance of the calorimeter (in X0)
  float* m_materialTraversed;

  /// Property: The extrapolator tool.
  ToolHandle<Trk::IExtrapolator> m_extrapolator;

  /// Property: CaloSurfaceBuilder tool.
  ToolHandle<ICaloSurfaceBuilder> m_caloSurfaceBuilder;

  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey{
    this,
    "CaloDetDescrManager",
    "CaloDetDescrManager"
  };

  /// Property: Minimum track pT to do the extrapolation.
  float m_minPt;
};


} // namespace D3PD


#endif // EGAMMAD3PDMAKER_EGAMMATRAVERSEDMATERIALFILLERTOOL_H
