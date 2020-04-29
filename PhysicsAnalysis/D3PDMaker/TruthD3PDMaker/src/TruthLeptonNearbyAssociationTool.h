/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TruthD3PDMaker/src/TruthLeptonNearbyAssociationTool.h
 * @author Zach Marshall <zach.marshall@cern.ch>
 * @date Feb, 2014
 * @brief Provides significant info for truth leptons
 */

#ifndef TRUTHD3PDMAKER_TRUTHLEPTONNEARBYASSOCIATIONTOOL_H
#define TRUTHD3PDMAKER_TRUTHLEPTONNEARBYASSOCIATIONTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "McParticleEvent/TruthParticle.h"
#include <vector>


#include "AtlasHepMC/GenParticle_fwd.h"

namespace D3PD {

class TruthLeptonNearbyAssociationTool : public BlockFillerTool<TruthParticle>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TruthLeptonNearbyAssociationTool (const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent);

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
  virtual StatusCode fill (const TruthParticle& p);

private:
  float m_dressing_dr;
  std::string m_truthCollectionName;

  float *m_dressed_pt;
  float *m_dressed_eta;
  float *m_dressed_phi;
  float *m_dressed_m;
  float *m_iso02;
  float *m_iso03;

  int get_real_parent( HepMC::GenParticle * , int depth=0 ) const;

};


} // namespace D3PD

#endif // not TRUTHD3PDMAKER_TRUTHLEPTONNEARBYASSOCIATIONTOOL_H
