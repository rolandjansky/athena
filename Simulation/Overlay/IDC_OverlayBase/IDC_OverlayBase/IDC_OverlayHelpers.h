/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @brief Helpers for overlaying Identifiable Containers
/// @author Tadej Novak

#ifndef IDC_OVERLAYHELPERS_H
#define IDC_OVERLAYHELPERS_H

#include <string>

namespace Overlay
{

/// @brief Diagnostic output of Identifiable Containers
template <class IDC_Container>
std::string debugPrint(const IDC_Container *container,
                       unsigned numprint = 25);

} // namespace Overlay

#include "IDC_OverlayHelpers.icc"

#endif
