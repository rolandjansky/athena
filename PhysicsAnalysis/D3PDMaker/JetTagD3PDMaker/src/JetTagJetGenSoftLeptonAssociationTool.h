/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file D3PDMaker/JetTagD3PDMaker/src/JetTagJetGenSoftLeptonAssociationTool.h
 * @author Georges Aad
 * @date Oct, 2010
 * @brief association from jets to soft leptons
 * 
 */

#ifndef JetTagD3PDMaker_JetTagJetGenSoftLeptonAssociationTool_H
#define JetTagD3PDMaker_JetTagJetGenSoftLeptonAssociationTool_H

#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "D3PDMakerUtils/SGKeyResolver.h"

class Jet;

#include "AtlasHepMC/GenParticle_fwd.h"
#include "AtlasHepMC/GenEvent_fwd.h"

namespace  Analysis{
  class SoftLeptonTruthInfo;
}

namespace D3PD {


/**
 * @brief Associate Gen soft leptons inside jets
 * using the barcode (only first gen event
 * consider switch to jet->extended barcode->GenParticles for more general cases
 *
 */

  
class JetTagJetGenSoftLeptonAssociationTool
  : public MultiAssociationTool<Jet,HepMC::GenParticle>
{
public:

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  JetTagJetGenSoftLeptonAssociationTool (const std::string& type,
                         const std::string& name,
                         const IInterface* parent);

  typedef MultiAssociationTool<Jet,HepMC::GenParticle> base;

  /// Standard Gaudi initialize method.
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  /**
   * @brief Start the iteration for a new association.
   * @param p The object from which to associate.
   */
  virtual StatusCode reset (const Jet& p);


  /**
   * @brief Return a pointer to the next element in the association.
   * Return 0 when the association has been exhausted.
   */
  const HepMC::GenParticle* next();

  /**
   * @brief Create any needed tuple variables.
   *
   * This is called at the start of the first event.
   */

  virtual StatusCode book();


private:

  SGKeyResolver m_resolver;
  std::string m_keys;
  bool m_fillVariables;

  short *m_origin;
  int *m_barcode;

  const Analysis::SoftLeptonTruthInfo* m_slmcinfo;
  const HepMC::GenEvent* m_genEvent;

  /// dummy particle to return if no association is found
  mutable HepMC::GenParticle* m_dummyParticle; 

  int m_lepItr;
  int m_lepEnd;

};


} // namespace D3PD


#endif // JetTagD3PDMaker_JetTagJetGenSoftLeptonAssociationTool_H
