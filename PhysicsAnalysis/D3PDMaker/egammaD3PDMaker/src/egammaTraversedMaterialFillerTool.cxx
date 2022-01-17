/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDMaker/src/egammaTraversedMaterialFillerTool.cxx
 * @author Bruno Lenzi <Bruno.Lenzi@cern.ch>
 * @date May, 2012
 * @brief Block filler tool for the material traversed by a converted photon/electron
 *  from the conversion/production vertex until the entrance of the calorimeter (in X0)
 */


#include "egammaTraversedMaterialFillerTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "CaloTrackingGeometry/ICaloSurfaceBuilder.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "TrkParameters/TrackParameters.h"
#include "AthenaKernel/errorcheck.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include <vector>


using CLHEP::GeV;


namespace D3PD {

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  egammaTraversedMaterialFillerTool::egammaTraversedMaterialFillerTool
      (const std::string& type,
       const std::string& name,
       const IInterface* parent)
    : Base (type, name, parent),
      m_extrapolator ("Trk::Extrapolator/AtlasExtrapolator"),
      m_caloSurfaceBuilder ("CaloSurfaceBuilder")
  {  
    declareProperty ("Extrapolator", 
                     m_extrapolator,
                     "Extrapolator tool.");

    declareProperty ("CaloSurfaceBuilder", 
                     m_caloSurfaceBuilder,
                     "CaloSurfaceBuilder tool.");

    declareProperty ("MinPt",
                     m_minPt = 1 * GeV,
                     "Minimum track pT to do the extrapolation.");

    book().ignore(); // avoid coverity warnings.
  }
  
  egammaTraversedMaterialFillerTool::~egammaTraversedMaterialFillerTool()
  {
  }
  
  StatusCode egammaTraversedMaterialFillerTool::initialize()
  {
    CHECK( m_extrapolator.retrieve() );
    CHECK( m_caloSurfaceBuilder.retrieve() );
    CHECK(m_caloMgrKey.initialize());   
    return StatusCode::SUCCESS;
  }
  
  /**
   * @brief Book variables for this block.
   */
  StatusCode egammaTraversedMaterialFillerTool::book()
  {
    CHECK( addVariable ("materialTraversed", m_materialTraversed,
                        "Material traversed between vertex "
                        "(conversion vertex) and the entrance of the "
                        "calorimeter (in X0)") );
    return StatusCode::SUCCESS;
  }
  
  
  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  StatusCode egammaTraversedMaterialFillerTool::fill (const xAOD::Electron& p)
  {
    const xAOD::TrackParticle* trackParticle = p.trackParticle();
    if (trackParticle)
      CHECK( fill (p, trackParticle->perigeeParameters()) );
    return StatusCode::SUCCESS;
  }


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  StatusCode egammaTraversedMaterialFillerTool::fill (const xAOD::Photon& p)
  {
    const xAOD::Vertex* vx = p.vertex();
    if (!vx || vx->nTrackParticles() < 1) return StatusCode::SUCCESS;

    // Set as parameters the ones of the highest pT track at the conv. vertex
    const Trk::TrackParameters* parameters =
      &vx->trackParticle(0)->perigeeParameters();

    if (vx->nTrackParticles() > 1 && parameters) {
      const Trk::TrackParameters* p2 =
        &vx->trackParticle(1)->perigeeParameters();
      if (p2 && p2->pT() > parameters->pT()) parameters = p2;
    }

    if(parameters){
    CHECK( fill (p, *parameters) );
    }
    return StatusCode::SUCCESS;
  }


  /**
   * @brief Common part.
   * @param eg The input egamma object.
   * @param parameters Track parameters to extrapolate.
   */
  StatusCode
  egammaTraversedMaterialFillerTool::fill (const xAOD::Egamma& eg,
                                           const Trk::TrackParameters& parameters)
  {

    SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{ m_caloMgrKey };
    const CaloDetDescrManager* caloDDMgr = *caloMgrHandle;

    if (parameters.pT() < m_minPt)
    {
      ATH_MSG_DEBUG("Momentum too low");
      return StatusCode::SUCCESS;
    }
    
    // Define surface to extrapolate to (PS barrel, endcap or strips)
    const xAOD::CaloCluster *cl = eg.caloCluster();
    if (!cl)
    {
      ATH_MSG_DEBUG("Invalid pointer to cluster object");
      return StatusCode::SUCCESS;
    }
    
    CaloCell_ID::CaloSample sample = CaloCell_ID::EME1;
    if (fabs(cl->eta()) < 1.5)
      sample = CaloCell_ID::PreSamplerB;
    else //if (fabs(cl->eta()) < 1.8)
      sample = CaloCell_ID::PreSamplerE;
    
    // create a surface at the entrance of the sample (0 is the offset)
    std::unique_ptr<Trk::Surface> surface
      (m_caloSurfaceBuilder->CreateUserSurface(sample, 0., cl->eta(), caloDDMgr));
    if (!surface)
    {
      ATH_MSG_DEBUG("Could not create surface at entrance of sample " << sample);
      return StatusCode::SUCCESS;
    }

    // Extrapolate to surface
    typedef std::vector<const Trk::TrackStateOnSurface*> tsos_vec_t;
    std::unique_ptr<const tsos_vec_t> v
      (m_extrapolator->extrapolateM (
          Gaudi::Hive::currentContext(),
          parameters,
          *surface,
          Trk::alongMomentum,
          true,
          Trk::muon));
    if (!v)
    {
      ATH_MSG_DEBUG("Extrapolation failed");
      return StatusCode::SUCCESS;
    } 
    
    // Sum the amount of material traversed
    for (const Trk::TrackStateOnSurface* tsos : *v) {
      if (tsos->materialEffectsOnTrack())
      {
        //ATH_MSG_INFO("Material effects");
        //tsos->materialEffectsOnTrack()->dump( msg(MSG::INFO) );
        *m_materialTraversed += tsos->materialEffectsOnTrack()->thicknessInX0();
      }
      delete tsos;
    }
    return StatusCode::SUCCESS;
  }


} // namespace D3PD
