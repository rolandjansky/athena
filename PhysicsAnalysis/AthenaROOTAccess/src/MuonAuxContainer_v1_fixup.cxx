/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaROOTAccess/src/MuonAuxContainer_v1_fixup.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2015
 * @brief Run fixup for MuonAuxContainer_v1.
 */


#include "xAODMuon/versions/MuonAuxContainer_v1.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "AthLinks/ElementLink.h"
#include "AthenaROOTAccess/MuonAuxContainer_v1_fixup.h"
#include <vector>


namespace {


const ElementLink< xAOD::TrackParticleContainer >*
tryTrackParticleLink (xAOD::MuonAuxContainer_v1* aux,
                      SG::auxid_t auxid,
                      size_t i)
{
  typedef std::vector<ElementLink<xAOD::TrackParticleContainer> > vec_t;
  const vec_t* cont = reinterpret_cast<const vec_t*> (aux->getIOData (auxid));
  if (!cont) return nullptr;
  if (cont->size() <= i || !(*cont)[i].isValid()) return nullptr;
  return &(*cont)[i];
}

const ElementLink< xAOD::TrackParticleContainer >*
getPrimaryTrackParticleLink(xAOD::MuonAuxContainer_v1* aux,
                            size_t i)
{
  typedef ElementLink< xAOD::TrackParticleContainer> el_t;
  static SG::AuxElement::Accessor<el_t> combinedTrackParticleLink_ac
    ("combinedTrackParticleLink");
  static SG::AuxElement::Accessor<el_t> inDetTrackParticleLink_ac
    ("inDetTrackParticleLink");
  static SG::AuxElement::Accessor<el_t> muonSpectrometerTrackParticleLink_ac
    ("muonSpectrometerTrackParticleLink");

  const el_t* ret = 0;

  if ((ret = tryTrackParticleLink (aux, combinedTrackParticleLink_ac.auxid(), i)))
    return ret;
  if ((ret = tryTrackParticleLink (aux, inDetTrackParticleLink_ac.auxid(), i)))
    return ret;
  if ((ret = tryTrackParticleLink (aux, muonSpectrometerTrackParticleLink_ac.auxid(), i)))
    return ret;

  return nullptr;
}


} // anonymous namespace


namespace AthenaROOTAccess {


void MuonAuxContainer_v1_fixup::fixupAfterAux (void* data, long long /*entry*/)
{
  xAOD::MuonAuxContainer_v1* aux =
    reinterpret_cast<xAOD::MuonAuxContainer_v1*> (data);

  static SG::AuxElement::Accessor<float> charge_ac ("charge");
  const std::vector<float>* charge =
    reinterpret_cast<const std::vector<float>* > (aux->getIOData (charge_ac.auxid()));
  if (!charge) return;
  std::vector<float>& charge_nc = *const_cast<std::vector<float>*> (charge);
  charge_nc.clear();
  size_t sz = aux->size();
  /*if( charge->size() != sz )*/ {
    charge_nc.resize (sz);
    for( size_t i = 0; i < sz; ++i ) {
      // Figure out which link to take:
      const ElementLink< xAOD::TrackParticleContainer >* link = getPrimaryTrackParticleLink(aux, i);
      // Fill the charge:
      if (link)
        charge_nc[i] = ( **link )->charge();
    }
  }
}


} // namespace AthenaROOTAccess

