// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ConversionVertexAssociationTool.h 605545 2014-07-09 04:51:09Z ssnyder $
/**
 * @file ConversionD3PDMaker/src/ConversionVertexAssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Associate from a conversion to its vertex.
 */


#ifndef PHOTOND3PDMAKER_CONVERSIONVERTEXASSOCIATIONTOOL_H
#define PHOTOND3PDMAKER_CONVERSIONVERTEXASSOCIATIONTOOL_H


#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "xAODEgamma/Photon.h"
#include "xAODTracking/Vertex.h"



namespace D3PD {


/**
 * @brief Associate from a conversion to its vertex.
 */
class ConversionVertexAssociationTool
  : public MultiAssociationTool<xAOD::Photon, xAOD::Vertex>
{
public:
  typedef MultiAssociationTool<xAOD::Photon, xAOD::Vertex> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  ConversionVertexAssociationTool (const std::string& type,
				   const std::string& name,
				   const IInterface* parent);


  /**
   * @brief Start the iteration for a new association
   */
  virtual StatusCode reset(const xAOD::Photon& ph);


  /**
   * @brief Return a pointer to the next associated object
   *
   * Return the target of the association, or 0 if exhausted.
   */
  virtual const xAOD::Vertex* next ();


private:
  unsigned int m_vxCounter;
  const xAOD::Photon* m_ph;
};


} // namespace D3PD



#endif // not PHOTOND3PDMAKER_CONVERSIONVERTEXASSOCIATIONTOOL_H


