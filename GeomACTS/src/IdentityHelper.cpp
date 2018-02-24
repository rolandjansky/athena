// This file is part of the ACTS project.
//
// Copyright (C) 2016 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"

#include "GeomACTS/IdentityHelper.hpp"

Acts::IdentityHelper::IdentityHelper(const InDetDD::SiDetectorElement *elem)
  : m_elem(elem)
{}
  

const PixelID* 
Acts::IdentityHelper::getPixelIDHelper() const 
{
  return dynamic_cast<const PixelID*>(m_elem->getIdHelper());
}
const SCT_ID* 
Acts::IdentityHelper::getSCTIDHelper() const
{
  return dynamic_cast<const SCT_ID*>(m_elem->getIdHelper());
}

int 
Acts::IdentityHelper::bec() const 
{
  auto id = m_elem->identify();
  if (m_elem->isPixel()) {
    return getPixelIDHelper()->barrel_ec(id);
  }
  else {
    return getSCTIDHelper()->barrel_ec(id);
  }
}

int 
Acts::IdentityHelper::layer_disk() const
{
  auto id = m_elem->identify();
  if (m_elem->isPixel()) {
    return getPixelIDHelper()->layer_disk(id);
  }
  else {
    return getSCTIDHelper()->layer_disk(id);
  }
}

int 
Acts::IdentityHelper::phi_module() const
{
  auto id = m_elem->identify();
  if (m_elem->isPixel()) {
    return getPixelIDHelper()->phi_module(id);
  }
  else {
    return getSCTIDHelper()->phi_module(id);
  }
}

int 
Acts::IdentityHelper::eta_module() const
{
  auto id = m_elem->identify();
  if (m_elem->isPixel()) {
    return getPixelIDHelper()->eta_module(id);
  }
  else {
    return getSCTIDHelper()->eta_module(id);
  }
}

int 
Acts::IdentityHelper::side() const
{
  auto id = m_elem->identify();
  if (m_elem->isPixel()) {
    return 0;
  }
  else {
    return getSCTIDHelper()->side(id);
  }
}

