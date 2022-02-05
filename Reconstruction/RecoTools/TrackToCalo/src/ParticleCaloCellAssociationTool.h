/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/***************************************************************************
  ParticleCaloCellAssociationTool.h
begin   : Summer 2014
authors : Niels van Eldik (CERN PH-ATC),Christos (MT)
 ***************************************************************************/
#ifndef TRKPARTICLECREATOR_PARTICLECALOCELLASSOCIATION_H
#define TRKPARTICLECREATOR_PARTICLECALOCELLASSOCIATION_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "ParticleCaloExtension/ParticleCellAssociation.h"
#include "PathLengthUtils.h"
#include "RecoToolInterfaces/IParticleCaloCellAssociationTool.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "TrackToCalo/CaloCellSelectorLayerdR.h"

namespace Trk {
class IParticleCaloExtensionTool;
class CaloExtension;
} // namespace Trk

class CaloCellContainer;

namespace Rec {

class ParticleCaloCellAssociationTool
  : virtual public IParticleCaloCellAssociationTool
  , public AthAlgTool
{
public:
  ParticleCaloCellAssociationTool(const std::string&,
                                  const std::string&,
                                  const IInterface*);

  virtual ~ParticleCaloCellAssociationTool() override;

  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;
  /** Method to get the ParticleCellAssociation for a given Particle
   * @param particle       input particle
   * @param dr             cone size used for the association
   * @param container      cell container to be used if provided
   * @return std::unique_ptr<ParticleCellAssociation>
   */
  virtual std::unique_ptr<ParticleCellAssociation> particleCellAssociation(
    const xAOD::IParticle& particle,
    float dr,
    const CaloCellContainer* container = nullptr,
    const CaloExtensionCollection* extensionCache =
      nullptr) const override final;

  /** Method to get the ParticleCellAssociation for a given Particle
   * @param particle       input particle
   * @param dr             cone size used for the association
   * @ param cache         cache for keeping previous results
   * @param container      cell container to be used if provided
   * @return ParticleCellAssociation* (plain ptr cache has ownership)
   *
   * An alg looping over a single collection of IParticles
   * re-using them multiple times can use a local  cache of
   * the form
   * std::unordered_map<size_t,std::unique_ptr<ParticleCellAssociation >>.
   * where the key is the  value of IParticle::index()
   *
   * This method adds the ParticleCellAssociation to the cache look-up table
   * which retains ownership.
   */

  virtual ParticleCellAssociation* particleCellAssociation(
    const xAOD::IParticle& particle,
    float dr,
    IParticleCaloCellAssociationTool::Cache& cache,
    const CaloCellContainer* container = nullptr,
    const CaloExtensionCollection* extensionCache =
      nullptr) const override final;

private:
  void getCellIntersections(
    const Trk::CaloExtension& caloExtension,
    const std::vector<const CaloCell*>& cells,
    ParticleCellAssociation::CellIntersections& cellIntersections) const;

  void associateCells(const CaloCellContainer& container,
                      const Trk::CaloExtension& caloExtension,
                      float dr,
                      std::vector<const CaloCell*>& cells) const;

  ToolHandle<Trk::IParticleCaloExtensionTool> m_caloExtensionTool{
    this,
    "ParticleCaloExtensionTool",
    ""
  };

  SG::ReadHandleKey<CaloCellContainer> m_cellContainerName{ this,
                                                            "CaloCellContainer",
                                                            "AllCalo" };

  Gaudi::Property<double> m_coneSize{ this, "ConeSize", 0.2 };

  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey{this,"CaloDetDescrManager", "CaloDetDescrManager"};

  Trk::CaloCellSelectorLayerdR m_defaultSelector;

  PathLengthUtils m_pathLenUtil;
};

} // namespace Rec

#endif
