// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file PhotonD3PDMaker/src/PAUDiPhotonAssociationTool.h
 * @author Mike Hance <hance@hep.upenn.edu>
 * @date July, 2010
 * @brief Associate PAU diphoton to photon objects
 */


#ifndef PHOTOND3PDMAKER_PAUDIPHOTONASSOCIATIONTOOL_H
#define PHOTOND3PDMAKER_PAUDIPHOTONASSOCIATIONTOOL_H


#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace PAU {
  class egamma;
  class DiPhoton;
}

namespace Analysis {
  class Photon;
}

namespace D3PD {


/**
 * @brief Associate a PAU::DiPhoton to Analysis::Photons
 *
 * This is a multi-association tool.
 * Given a @c PAU::DiPhoton object,
 * return the single photon objects
 *
 * Parameters:
 *  none
 *
 */
class PAUDiPhotonAssociationTool
  : public MultiAssociationTool<PAU::DiPhoton, Analysis::Photon>
{
public:
  typedef MultiAssociationTool<PAU::DiPhoton, Analysis::Photon> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  PAUDiPhotonAssociationTool (const std::string& type,
			      const std::string& name,
			      const IInterface* parent);


  /**
   * @brief Start the iteration for a new association
   */
  virtual StatusCode reset(const PAU::DiPhoton& dp);


  /**
   * @brief Return a pointer to the next associated object
   *
   * Return the target of the association, or 0 if exhausted.
   */
  virtual const Analysis::Photon* next ();


private:
  unsigned int m_photonCounter;
  const PAU::egamma* m_photon_1;
  const PAU::egamma* m_photon_2;
};


} // namespace D3PD

#endif // not PHOTOND3PDMAKER_PAUDIPHOTONASSOCIATIONTOOL_H
