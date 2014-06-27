// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ConversionNeutralPerigeeAssociationTool.h
/**
 * @file PhotonD3PDMaker/src/ConversionNeutralPerigeeAssociationTool.h
 * @author Jean-Francois Marchand <jean-francois.marchand@cern.ch>
 * @date November 2012
 * @brief Associate MeasuredNeutralPerigee to conversion vertex
 */

#ifndef PHOTOND3PDMAKER_CONVERSIONNEUTRALPERIGEEASSOCIATIONTOOL_H
#define PHOTOND3PDMAKER_CONVERSIONNEUTRALPERIGEEASSOCIATIONTOOL_H

#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "TrkNeutralParameters/NeutralParameters.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk {
  class VxCandidate;
  class ExtendedVxCandidate;
}

namespace InDet{
//class InDetJetFitterUtils;
}


namespace D3PD {

  /**
   * @brief Associate from a VxTrackAtVertex to a MeasuredPerigee.
   */
  class ConversionNeutralPerigeeAssociationTool
    : public SingleAssociationTool<Trk::VxCandidate, Trk::NeutralPerigee>
  {
  public:
    typedef SingleAssociationTool<Trk::VxCandidate, Trk::NeutralPerigee> Base;
    
    /**
     * @brief Standard Gaudi tool constructor.
     * @param type The name of the tool type.
     * @param name The tool name.
     * @param parent The tool's Gaudi parent.
     */
    ConversionNeutralPerigeeAssociationTool (const std::string& type,
				      const std::string& name,
				      const IInterface* parent);

    StatusCode initialize();

    /**
     * @brief Return the target object.
     * @param p The source object for the association.
     *
     * Return the target of the association, or 0.
     */
    virtual const Trk::NeutralPerigee* get (const Trk::VxCandidate& p);

  private:
    //ToolHandle<InDet::InDetJetFitterUtils> m_convUtils;

  };
  
  
} // namespace D3PD



#endif // not PHOTOND3PDMAKER_CONVERSIONNEUTRALPERIGEEASSOCIATIONTOOL_H
