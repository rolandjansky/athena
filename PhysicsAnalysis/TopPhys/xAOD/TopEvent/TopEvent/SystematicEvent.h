/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */
#ifndef ANALYSISTOP_TOPEVENT_SYSTEMATICEVENT_H
#define ANALYSISTOP_TOPEVENT_SYSTEMATICEVENT_H

// EDM include(s).
#include "AthContainers/AuxElement.h"
#include "xAODCore/CLASS_DEF.h"

// System include(s).
#include <vector>

namespace xAOD {

  /**
   * @author John Morris <john.morris@cern.ch>
   *
   * @brief SystematicEvent
   *   A simple xAOD class which we can persist into a mini-xAOD
   *   The xAOD EDM is way too complex, so let's simplify it
   *   It's not like ROOT can do schema evolution......
   *
   *   This class contains only vectors of unsigned int's
   *   It is very light weight and is used to make top::Event objects
   **/
  class SystematicEvent: public SG::AuxElement {

  public:
    /// Default constructor
    SystematicEvent() = default;

    /// get hash value
    std::size_t hashValue() const;
    /// set hash value
    void setHashValue(std::size_t);

    /// get ttreeIndex
    unsigned int ttreeIndex() const;
    /// set ttreeIndex
    void setTtreeIndex(unsigned int);

    /// get isLooseEvent
    char isLooseEvent() const;
    /// set isLooseEvent
    void setIsLooseEvent(char);

    /// get Photons
    const std::vector<unsigned int>& goodPhotons() const;
    /// set Photons
    void setGoodPhotons(const std::vector<unsigned int>&);

    /// get Electrons
    const std::vector<unsigned int>& goodElectrons() const;
    /// set Electrons
    void setGoodElectrons(const std::vector<unsigned int>&);

    /// get FWD Electrons
    const std::vector<unsigned int>& goodFwdElectrons() const;
    /// set FWD Electrons
    void setGoodFwdElectrons(const std::vector<unsigned int>&);

    /// get Muons
    const std::vector<unsigned int>& goodMuons() const;
    /// set Muons
    void setGoodMuons(const std::vector<unsigned int>&);

    /// get Soft Muons
    const std::vector<unsigned int>& goodSoftMuons() const;
    /// set Soft Muons
    void setGoodSoftMuons(const std::vector<unsigned int>&);

    /// get Taus
    const std::vector<unsigned int>& goodTaus() const;
    /// set Taus
    void setGoodTaus(const std::vector<unsigned int>&);

    /// get Jets
    const std::vector<unsigned int>& goodJets() const;
    /// set Jets
    void setGoodJets(const std::vector<unsigned int>&);

    /// get LargeRJets
    const std::vector<unsigned int>& goodLargeRJets() const;
    /// set LargeRJets
    void setGoodLargeRJets(const std::vector<unsigned int>&);

    /// get TrackJets
    const std::vector<unsigned int>& goodTrackJets() const;
    /// set TrackJets
    void setGoodTrackJets(const std::vector<unsigned int>&);

    /// get Tracks
    const std::vector<unsigned int>& goodTracks() const;
    /// set Tracks
    void setGoodTracks(const std::vector<unsigned int>&);

  }; // class SystematicEvent

} // namespace xAOD

// Define a ClassID for the type.
CLASS_DEF(xAOD::SystematicEvent, 173054021, 1)

#endif // not ANALYSISTOP_TOPEVENT_SYSTEMATICEVENT_H
