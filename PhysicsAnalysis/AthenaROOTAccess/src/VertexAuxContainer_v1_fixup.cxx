/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaROOTAccess/src/VertexAuxContainer_v1_fixup.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2015
 * @brief Run fixup for VertexAuxContainer_v1.
 */

#include "xAODTracking/versions/VertexAuxContainer_v1.h"
#include "AthenaROOTAccess/VertexAuxContainer_v1_fixup.h"
#include "AthContainers/AuxTypeRegistry.h"
#include <vector>


namespace AthenaROOTAccess {


void VertexAuxContainer_v1_fixup::fixupAfterAux (void* data,
                                                 long long /*entry*/)
{
  xAOD::VertexAuxContainer_v1* aux =
    reinterpret_cast<xAOD::VertexAuxContainer_v1*> (data);

  SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();

  // nb. Don't rely on aux->size() --- it's possible that one of
  // neutralParticleLinks or neutralWeights could get the lowest auxid,
  // in which case we'll get the size wrong!
  size_t sz;
  {
    SG::auxid_t auxid = reg.getAuxID<float> ("chiSquared");
    void* data1 = const_cast<void*> (aux->getIOData (auxid));
    std::vector<float>* vec = reinterpret_cast<std::vector<float>*> (data1);
    sz = vec->size();
  }

  {
    typedef std::vector< ElementLink< xAOD::NeutralParticleContainer > >
      NeutralLink_t;
    SG::auxid_t auxid = reg.getAuxID<NeutralLink_t> ("neutralParticleLinks");
    void* data1 = const_cast<void*> (aux->getIOData (auxid));
    std::vector<NeutralLink_t>* vec = reinterpret_cast<std::vector<NeutralLink_t>*> (data1);
    if (vec->size() != sz) vec->resize(sz);
  }

  {
    SG::auxid_t auxid = reg.getAuxID<std::vector<float> > ("neutralWeights");
    void* data2 = const_cast<void*> (aux->getIOData (auxid));
    std::vector<std::vector<float> >* vec = reinterpret_cast<std::vector<std::vector<float> >*> (data2);
    if (vec->size() != sz) vec->resize(sz);
  }
}


} // namespace AthenaROOTAccess

