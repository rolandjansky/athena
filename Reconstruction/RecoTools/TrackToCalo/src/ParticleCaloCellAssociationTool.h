/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
ParticleCaloCellAssociationTool.h  -  Description
-------------------
begin   : Summer 2014
authors : Niels van Eldik (CERN PH-ATC)
***************************************************************************/
#ifndef TRKPARTICLECREATOR_PARTICLECALOCELLASSOCIATION_H
#define TRKPARTICLECREATOR_PARTICLECALOCELLASSOCIATION_H

#include "RecoToolInterfaces/IParticleCaloCellAssociationTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "ParticleCaloExtension/ParticleCellAssociation.h"
#include "TrackToCalo/CaloCellSelectorLayerdR.h"

#include "PathLengthUtils.h"



namespace Trk {
  class IParticleCaloExtensionTool;
  class CaloExtension;
}

namespace Rec {

  class ParticleCaloCellAssociationTool : virtual public IParticleCaloCellAssociationTool, public AthAlgTool {
  public:
    
    ParticleCaloCellAssociationTool(const std::string&,const std::string&,const IInterface*);

    virtual ~ParticleCaloCellAssociationTool();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    /** Method to get the ParticleCellAssociation of a given TrackParticle
        @param trackParticle  input track particle
        @param extension      reference to a pointer to a ParticleCellAssociation, will be updated if call is successfull
                              NEVER delete the pointer, you will cause a crash! 
        @param dr             cone size used for the association
                              If caching is enabled, the cells associated to the association contain at least all cells
                              in dr but could contain more. Users ALWAYS have to recalculate the associated cells in their cone.
        @param container      cell container to be used if provided 
        @param useCaching     configure whether the tool caches the result on the track particle
                              The default behavior is 'true' to ensure optimal performance
                              If caching is enabled, the code will perform a consistency check on the container pointer
                              If the function is called twice on the same particle with different containers, the call will fail.
                              The same is true if the function is called once without container and once with on the same particle.
        @return true if the call was successful
    */
    bool particleCellAssociation( const xAOD::IParticle& particle,  const ParticleCellAssociation*& association, float dr, 
                                  const CaloCellContainer* container = 0, bool useCaching = true ) const final;
    
  private:

    void getCellIntersections( const Trk::CaloExtension& caloExtension,
                               const std::vector<const CaloCell*>& cells,
                               ParticleCellAssociation::CellIntersections& cellIntersections) const;

    const CaloCellContainer* getCellContainer() const;
    void associateCells( const CaloCellContainer& container, const Trk::CaloExtension& caloExtension, float dr,
                         std::vector<const CaloCell*>& cells ) const;

    ToolHandle< Trk::IParticleCaloExtensionTool >  m_caloExtensionTool;
    std::string m_cellContainerName;
    double      m_coneSize;
    mutable Trk::CaloCellSelectorLayerdR m_defaultSelector;

    mutable PathLengthUtils m_pathLenUtil;


  };


}

#endif
