/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "FlavorTagDiscriminants/customGetter.h"
#include "FlavorTagDiscriminants/BTagTrackIpAccessor.h"

#include "xAODJet/Jet.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODBTagging/BTagging.h"

#include <optional>

namespace {
  // ______________________________________________________________________
  // Custom getters for jet-wise quantities
  //
  // this function is not at all optimized, but then it doesn't have
  // to be since it should only be called in the initialization stage.
  //
  std::function<double(const xAOD::BTagging&)> customGetter(
    const std::string& name)
  {
    using JL = ElementLink<xAOD::JetContainer>;
    SG::AuxElement::ConstAccessor<JL> jl("jetLink");
    auto jg = [jl](const xAOD::BTagging& btag) -> const xAOD::Jet& {
      auto link = jl(btag);
      if (!link.isValid()) {
        throw std::runtime_error("invalid jetLink");
      }
      return **link;
    };
    if (name == "pt") {
      return [jg](const xAOD::BTagging& b) {return jg(b).pt();};
    }
    if (name == "abs_eta") {
      return [jg](const xAOD::BTagging& b) {return std::abs(jg(b).eta());};
    }
    throw std::logic_error("no match for custom getter " + name);
  }


  // _______________________________________________________________________
  // Custom getters for track variables

  template <typename T>
  class TJGetter
  {
  private:
    T m_getter;
  public:
    TJGetter(T getter):
      m_getter(getter)
      {}
    std::vector<double> operator()(
      const xAOD::Jet& jet,
      const std::vector<const xAOD::TrackParticle*>& tracks) const {
      std::vector<double> sequence;
      for (const auto* track: tracks) {
        sequence.push_back(m_getter(*track, jet));
      }
      return sequence;
    }
  };

  std::optional<FlavorTagDiscriminants::SequenceFromTracks>
  sequenceWithIpDep(
    const std::string& name,
    const std::string& prefix)
  {

    using Tp = xAOD::TrackParticle;
    using Jet = xAOD::Jet;

    BTagTrackIpAccessor a(prefix);
    if (name == "IP3D_signed_d0_significance") {
      return TJGetter([a](const Tp& t, const Jet& j){
          return a.getSignedIp(t, j).ip3d_signed_d0_significance;
      });
    }
    if (name == "IP3D_signed_z0_significance") {
      return TJGetter([a](const Tp& t, const Jet& j){
        return a.getSignedIp(t, j).ip3d_signed_z0_significance;
      });
    }
    if (name == "IP2D_signed_d0") {
      return TJGetter([a](const Tp& t, const Jet& j){
        return a.getSignedIp(t, j).ip2d_signed_d0;
      });
    }
    if (name == "IP3D_signed_d0") {
      return TJGetter([a](const Tp& t, const Jet& j){
        return a.getSignedIp(t, j).ip3d_signed_d0;
      });
    }
    if (name == "IP3D_signed_z0") {
      return TJGetter([a](const Tp& t, const Jet& j){
        return a.getSignedIp(t, j).ip3d_signed_z0;
      });
    }
    if (name == "d0" || name == "btagIp_d0") {
      return TJGetter([a](const Tp& t, const Jet&){
        return a.d0(t);
      });
    }
    if (name == "z0SinTheta" || name == "btagIp_z0SinTheta") {
      return TJGetter([a](const Tp& t, const Jet&){
        return a.z0SinTheta(t);
      });
    }
    if (name == "d0Uncertainty") {
      return TJGetter([a](const Tp& t, const Jet&){
        return a.d0Uncertainty(t);
      });
    }
    if (name == "z0SinThetaUncertainty") {
      return TJGetter([a](const Tp& t, const Jet&){
        return a.z0SinThetaUncertainty(t);
      });
    }
    return std::nullopt;

  }

  std::optional<FlavorTagDiscriminants::SequenceFromTracks>
  sequenceNoDep(const std::string& name)
  {

    using Tp = xAOD::TrackParticle;
    using Jet = xAOD::Jet;
    typedef std::vector<const xAOD::TrackParticle*> Tracks;

    if (name == "log_ptfrac") {
      return TJGetter([](const Tp& t, const Jet& j) {
        return std::log(t.pt() / j.pt());
      });
    }
    if (name == "log_dr") {
      return TJGetter([](const Tp& t, const Jet& j) {
        return std::log(t.p4().DeltaR(j.p4()));
      });
    }
    if (name == "log_dr_nansafe") {
      return TJGetter([](const Tp& t, const Jet& j) {
        return std::log(t.p4().DeltaR(j.p4()) + 1e-7);
      });
    }
    if (name == "dphi") {
      return TJGetter([](const Tp& t, const Jet& j) {
        return t.p4().DeltaPhi(j.p4());
      });
    }
    if (name == "deta") {
      return TJGetter([](const Tp& t, const Jet& j) {
        return t.eta() - j.eta();
      });
    }
    if (name == "pt") {
      return [](const Jet&, const Tracks& t) {
        std::vector<double> tracks;
        for (auto* trk: t) tracks.push_back(trk->pt());
        return tracks;
      };
    }
    if (name == "eta") {
      return [](const Jet&, const Tracks& t) {
        std::vector<double> tracks;
        for (auto* trk: t) tracks.push_back(trk->eta());
        return tracks;
      };
    }
    return std::nullopt;
  }

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
    std::function<std::pair<std::string, double>(const xAOD::BTagging&)>
    customGetterAndName(const std::string& name) {
      auto getter = customGetter(name);
      return [name, getter](const xAOD::BTagging& j) {
               return std::make_pair(name, getter(j));
             };
    }

    // Case for track variables
    std::pair<std::function<std::pair<std::string, std::vector<double>>(
      const xAOD::Jet&,
      const std::vector<const xAOD::TrackParticle*>&)>,
      std::set<std::string>>
    customNamedSeqGetterWithDeps(const std::string& name,
                                 const std::string& prefix) {
      auto [getter, deps] = customSequenceGetterWithDeps(name, prefix);
      return {
        [n=name, g=getter](const xAOD::Jet& j,
                       const std::vector<const xAOD::TrackParticle*>& t) {
          return std::make_pair(n, g(j, t));
        },
        deps
      };
    }
  }
  // ________________________________________________________________________
  // Master track getter list
  //
  // These functions are wrapped by the customNamedSeqGetter function
  // below to become the ones that are actually used in DL2.
  //
  std::pair<SequenceFromTracks, std::set<std::string>>
  customSequenceGetterWithDeps(const std::string& name,
                               const std::string& prefix) {

    if (auto getter = sequenceWithIpDep(name, prefix)) {
      auto deps = BTagTrackIpAccessor(prefix).getTrackIpDataDependencyNames();
      return {*getter, deps};
    }

    if (auto getter = sequenceNoDep(name)) {
      return {*getter, {}};
    }
    throw std::logic_error("no match for custom getter " + name);
  }

}
