// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: NeutralPerigeeCovarianceAssociationTool.h
/**
 * @file PhotonD3PDMaker/src/PerigeeCovarianceAssociationTool.h
 * @author Jean-Francois Marchadn <jean-francois.marchand@cern.ch>
 * @date Nov, 2012
 * @brief Associate from a NeutralPerigee to its ErrorMatrix.
 */
#ifndef PHOTOND3PDMAKER_NEUTRALPERIGEECOVARIANCEASSOCIATIONTOOL_H
#define PHOTOND3PDMAKER_NEUTRALPERIGEECOVARIANCEASSOCIATIONTOOL_H

#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "TrkNeutralParameters/NeutralParameters.h"


namespace D3PD {

  /**
   * @brief Associate from a Perigee to its ErrorMatrix.
   */
  class NeutralPerigeeCovarianceAssociationTool
    : public SingleAssociationTool<Trk::NeutralPerigee, AmgSymMatrix(5)>
    {
    public:
      typedef SingleAssociationTool<Trk::NeutralPerigee, AmgSymMatrix(5)> Base;
      
      /**
       * @brief Standard Gaudi tool constructor.
       * @param type The name of the tool type.
       * @param name The tool name.
       * @param parent The tool's Gaudi parent.
       */
      NeutralPerigeeCovarianceAssociationTool (const std::string& type,
					       const std::string& name,
					       const IInterface* parent);
      
      
      /**
       * @brief Return the target object.
       * @param p The source object for the association.
       *
       * Return the target of the association, or 0.
       */
      virtual const AmgSymMatrix(5)* get (const Trk::NeutralPerigee& p);
    };
  
  
} // namespace D3PD



#endif // not PHOTOND3PDMAKER_NEUTRALPERIGEECOVARIANCEASSOCIATIONTOOL_H
