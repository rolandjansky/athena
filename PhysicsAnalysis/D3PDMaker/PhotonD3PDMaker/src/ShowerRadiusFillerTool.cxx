/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file PhotonD3PDMaker/src/ShowerRadiusFillerTool.cxx
 * @author Jean-Francois Marchadn <jean-francois.marchand@cern.ch>
 * @date May, 2013
 * @brief Block filler tool for shower radius in layer 2
 */


#include "ShowerRadiusFillerTool.h"
#include "CaloEvent/CaloCluster.h"
#include "AthenaKernel/errorcheck.h"
#include <typeinfo>
#include "CaloDetDescr/CaloDetDescrManager.h"


namespace D3PD {


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  ShowerRadiusFillerTool::ShowerRadiusFillerTool (const std::string& type,
					const std::string& name,
					const IInterface* parent)
    : BlockFillerTool<CaloCluster> (type, name, parent)
  {
    book().ignore(); // Avoid coverity warnings
  }


  /**
   * @brief Book variables for this block.
   */
  StatusCode ShowerRadiusFillerTool::book()
  {
    CHECK( addVariable ("radius_s1",     m_radius_s1)  );
    CHECK( addVariable ("radius_s2",     m_radius_s2)  );

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
  StatusCode ShowerRadiusFillerTool::fill (const CaloCluster& p)
  {
    const CaloDetDescrManager* calodet = CaloDetDescrManager::instance();
    CaloCell_ID::CaloSample layer1, layer2;
    if (p.inBarrel() && ! p.inEndcap()) {
      layer1 = CaloCell_ID::EMB1;
      layer2 = CaloCell_ID::EMB2;
    } else if (p.inEndcap() && !p.inBarrel()) {
      layer1 = CaloCell_ID::EME1;
      layer2 = CaloCell_ID::EME2;
    } else {
      double eBarrel = p.getVariable(CaloVariableType::ENERGY, CaloSampling::EMB2,true);
      double eEndCap = p.getVariable(CaloVariableType::ENERGY, CaloSampling::EME2,true);
      if (eEndCap>= eBarrel) {
	layer1 = CaloCell_ID::EME1;
	layer2 = CaloCell_ID::EME2;
      } else {
	layer1 = CaloCell_ID::EMB1;
	layer2 = CaloCell_ID::EMB2;
      }
    }

    double radius_s2 = 0;
    const CaloDetDescrElement* element2 = calodet->get_element(layer2,p.eta(),p.phi());
    if (element2) {
      if (layer2==CaloCell_ID::EMB2) radius_s2 = element2->r()-element2->dr();
      else
	if (std::fabs(p.eta())>0.)
	  radius_s2 = (std::fabs(element2->z())-std::fabs(element2->dz()))/std::fabs(sinh(p.eta()));
    }
    *m_radius_s2 = radius_s2;

    double radiusEntrance_s1 = 0;
    const CaloDetDescrElement* element1 = calodet->get_element(layer1,p.etaBE(1),p.phiBE(2));
    if (element1) {
      if ( layer2 == CaloCell_ID::EME2 )
	radiusEntrance_s1 = element1->z() - (element1->z()<0 ? -element1->dz() : element1->dz());
      else
	radiusEntrance_s1 = (element1->r()-element1->dr());
    }
    if ( std::abs (radiusEntrance_s1) < 10.) {
      float aeta = static_cast<float> (std::abs(p.etaBE(1)));
      if ( layer1 == CaloCell_ID::EMB1 ) {
	if (aeta < 0.8)
	  radiusEntrance_s1 = (1567.8 - 18.975*aeta - 17.668*aeta*aeta);
	else
	  radiusEntrance_s1 = (1503.2 + 71.716*aeta - 41.008*aeta*aeta);
      } else {
	radiusEntrance_s1 = 3760.;
	if (p.etaBE(1) < 0.) radiusEntrance_s1 = -radiusEntrance_s1;
      }
    }
    *m_radius_s1 = radiusEntrance_s1;


    return StatusCode::SUCCESS;
  }


} // namespace D3PD
