/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "FlavorTagDiscriminants/customGetter.h"
#include "FlavorTagDiscriminants/BTagTrackAugmenter.h"

namespace {
  // ______________________________________________________________________
  // Custom getters for jet-wise quantities
  //
  // this function is not at all optimized, but then it doesn't have
  // to be since it should only be called in the initialization stage.
  //
  std::function<double(const xAOD::Jet&)> customGetter(const std::string& name)
  {
    if (name == "pt") {
      return [](const xAOD::Jet& j) {return j.pt();};
    }
    if (name == "abs_eta") {
      return [](const xAOD::Jet& j) {return std::abs(j.eta());};
    }
    throw std::logic_error("no match for custom getter " + name);
  }


  // _______________________________________________________________________
  // Custom getters for track variables
  //
  // TODO: only use a subset of the signed_ip functions from the
  // augmenter below
  class SignedD0SequenceGetter
  {
  private:
    BTagTrackAugmenter m_augmenter;
  public:
    SignedD0SequenceGetter():
      m_augmenter()
      {}
    std::vector<double> operator()(
      const xAOD::Jet& jet,
      const std::vector<const xAOD::TrackParticle*>& tracks) const {
      std::vector<double> signed_d0;
      for (const auto* track: tracks) {
        signed_d0.push_back(
          m_augmenter.get_signed_ip(*track, jet).ip3d_signed_d0_significance);
      }
      return signed_d0;
    }
  };
  class SignedZ0SequenceGetter
  {
  private:
    BTagTrackAugmenter m_augmenter;
  public:
    SignedZ0SequenceGetter():
      m_augmenter()
      {}
    std::vector<double> operator()(
      const xAOD::Jet& jet,
      const std::vector<const xAOD::TrackParticle*>& tracks) const {
      std::vector<double> signed_z0;
      for (const auto* track: tracks) {
        signed_z0.push_back(
          m_augmenter.get_signed_ip(*track, jet).ip3d_signed_z0_significance);
      }
      return signed_z0;
    }
  };


}

namespace FlavorTagDiscriminants {
  namespace internal {

    // ________________________________________________________________
    // Interface functions
    //
    // As long as we're giving lwtnn pair<name, double> objects, we
    // can't use the raw getter functions above (which only return a
    // double). Instead we'll wrap those functions in another function,
    // which returns the pair we wanted.
    //
    // Case for jet variables
    std::function<std::pair<std::string, double>(const xAOD::Jet&)>
    customGetterAndName(const std::string& name) {
      auto getter = customGetter(name);
      return [name, getter](const xAOD::Jet& j) {
               return std::make_pair(name, getter(j));
             };
    }

    // Case for track variables
    std::function<std::pair<std::string, std::vector<double>>(
      const xAOD::Jet&,
      const std::vector<const xAOD::TrackParticle*>&)>
    customNamedSeqGetter(const std::string& name) {
      auto getter = customSequenceGetter(name);
      return [name, getter](const xAOD::Jet& j,
                            const std::vector<const xAOD::TrackParticle*>& t) {
               return std::make_pair(name, getter(j, t));
             };
    }
  }
  // ________________________________________________________________________
  // Master track getter list
  //
  // These functions are wrapped by the customNamedSeqGetter function
  // below to become the ones that are actually used in DL2.
  //
  std::function<std::vector<double>(
    const xAOD::Jet&,
    const std::vector<const xAOD::TrackParticle*>&)>
  customSequenceGetter(const std::string& name) {
    typedef std::vector<const xAOD::TrackParticle*> Tracks;
    if (name == "IP3D_signed_d0_significance") {
      return SignedD0SequenceGetter();
    }
    if (name == "IP3D_signed_z0_significance") {
      return SignedZ0SequenceGetter();
    }
    if (name == "log_ptfrac") {
      return [](const xAOD::Jet& j, const Tracks& t) {
               double jpt = j.pt();
               std::vector<double> pfc;
               for (const auto& trk: t) {
                 pfc.push_back(std::log(trk->pt() / jpt));
               }
               return pfc;
             };
    }
    if (name == "log_dr") {
      return [](const xAOD::Jet& j, const Tracks& t) {
               const auto jp4 = j.p4();
               std::vector<double> log_dr;
               for (const auto& trk: t) {
                 log_dr.push_back(std::log(trk->p4().DeltaR(jp4)));
               }
               return log_dr;
             };
    }
    if (name == "log_dr_nansafe") {
      return [](const xAOD::Jet& j, const Tracks& t) {
               const auto jp4 = j.p4();
               std::vector<double> log_dr;
               for (const auto& trk: t) {
                 double dr = trk->p4().DeltaR(jp4) + 1E-7;
                 log_dr.push_back(std::log( dr ) );
               }
               return log_dr;
             };
    }
    if (name == "pt") {
      return [](const xAOD::Jet&, const Tracks& t) {
               std::vector<double> tracks;
               for (auto* trk: t) tracks.push_back(trk->pt());
               return tracks;
             };
    }
    if (name == "eta") {
      return [](const xAOD::Jet&, const Tracks& t) {
               std::vector<double> tracks;
               for (auto* trk: t) tracks.push_back(trk->eta());
               return tracks;
             };
    }
    throw std::logic_error("no match for custom getter " + name);
  }

}
