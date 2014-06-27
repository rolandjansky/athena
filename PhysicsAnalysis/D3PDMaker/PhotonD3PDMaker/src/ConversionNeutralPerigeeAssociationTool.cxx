/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ConversionNeutralPerigeeAssociationTool.cxx
/**
 * @file PhotonD3PDMaker/src/ConversionNeutralPerigeeAssociationTool.cxx
 * @author Jean-Francois Marchand <jean-francois.marchand@cern.ch>
 * @date November 2012
 * @brief Associate MeasuredNeutralPerigee to conversion vertex
 */


#include "ConversionNeutralPerigeeAssociationTool.h"
#include "EventKernel/INavigable4Momentum.h"
#include "AthenaKernel/errorcheck.h"

#include "VxVertex/VxContainer.h"
#include "VxVertex/ExtendedVxCandidate.h"
//#include "InDetSecVxFinderTool/InDetJetFitterUtils.h"
#include "egammaUtils/EMConversionUtils.h"

namespace D3PD {

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  ConversionNeutralPerigeeAssociationTool::ConversionNeutralPerigeeAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent)//,
      //m_convUtils ("InDet::InDetJetFitterUtils/InDetJetFitterUtils")
  {
    //declareProperty("ConversionUtils", m_convUtils, "Tool for creating neutral perigee.");
  }
  
  StatusCode ConversionNeutralPerigeeAssociationTool::initialize()
  {
    CHECK( Base::initialize() );
    //CHECK( m_convUtils.retrieve() );
    return StatusCode::SUCCESS;
  }

  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  const Trk::NeutralPerigee *ConversionNeutralPerigeeAssociationTool::get (const Trk::VxCandidate& VxConv)
  {
    const Trk::NeutralPerigee* perigee = 0;

    //if (dynamic_cast<const Trk::ExtendedVxCandidate*>(&VxConv)) {
      // This method is gone in current devval.
      //perigee = m_convUtils->createNeutralPerigee(VxConv);
    //} else
    {
      perigee = EMConversionUtils::createPhotonParameters(&VxConv);
    }
    if (!perigee) return 0;

    const Trk::NeutralPerigee* nperigee = dynamic_cast<const Trk::NeutralPerigee*>(perigee);

    return nperigee;
  }
  
} // namespace D3PD



