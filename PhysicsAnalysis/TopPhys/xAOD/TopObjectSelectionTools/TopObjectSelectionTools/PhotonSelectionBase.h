/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHOTONSELECTIONBASE_H_
#define PHOTONSELECTIONBASE_H_

#include "xAODEgamma/Photon.h"

#include <memory>

namespace top {

/**
 * @brief Base class for implementing an Photon selection.
 *
 * If you want to build a new photon selection, you should inherit from this.
 *
 */

class PhotonSelectionBase {
public:
    PhotonSelectionBase();
    virtual ~PhotonSelectionBase();

    ///No copy construction
    PhotonSelectionBase(const PhotonSelectionBase& rhs) = delete;

    ///No moving 
    PhotonSelectionBase(PhotonSelectionBase&& rhs) = delete;

    ///No assignment
    PhotonSelectionBase& operator=(const PhotonSelectionBase& rhs) = delete;

    ///For the main analysis object selection
    virtual bool passSelection(const xAOD::Photon&) const = 0;

    ///For the loose object selection
    virtual bool passSelectionLoose(const xAOD::Photon&) const = 0;

    ///Because everybody likes to know what object definitions they ran with
    virtual void print(std::ostream&) const = 0;
};

}

std::ostream& operator<<(std::ostream& os, const top::PhotonSelectionBase& selection);

#endif
