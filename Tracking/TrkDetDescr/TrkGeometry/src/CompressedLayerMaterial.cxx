/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CompressedLayerMaterial.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkGeometry/CompressedLayerMaterial.h"

#include "TrkGeometry/MaterialProperties.h"

Trk::CompressedLayerMaterial::CompressedLayerMaterial()
    : Trk::LayerMaterialProperties(), m_binUtility(nullptr) {}

Trk::CompressedLayerMaterial::CompressedLayerMaterial(
    Trk::BinUtility& binutility)
    : Trk::LayerMaterialProperties(), m_binUtility(binutility.clone()) {}

Trk::CompressedLayerMaterial::CompressedLayerMaterial(
    const Trk::BinUtility& binutility,
    const MaterialPropertiesVector& fullProperties,
    const std::vector<unsigned short int>& materialIndices, double splitFactor)
    : Trk::LayerMaterialProperties(splitFactor),
      m_binUtility(binutility.clone()),
      m_fullMaterial(fullProperties),
      m_materialBins(materialIndices) {}

Trk::CompressedLayerMaterial::CompressedLayerMaterial(
    std::unique_ptr<Trk::BinUtility> binutility,
    const MaterialPropertiesVector&& fullProperties,
    const std::vector<unsigned short int>& materialIndices, double splitFactor)
    : Trk::LayerMaterialProperties(splitFactor),
      m_binUtility(binutility.release()),
      m_fullMaterial(fullProperties),
      m_materialBins(materialIndices) {}

Trk::CompressedLayerMaterial::~CompressedLayerMaterial() {
  delete m_binUtility;
  clearMaterial();
}

Trk::CompressedLayerMaterial::CompressedLayerMaterial(
    const Trk::CompressedLayerMaterial& lmp)
    : Trk::LayerMaterialProperties(lmp),
      m_binUtility(lmp.m_binUtility->clone()),
      m_materialBins(lmp.m_materialBins) {
  // clear the material
  clearMaterial();
  // and fill the material
  fillMaterial(lmp.m_fullMaterial);
}

Trk::CompressedLayerMaterial& Trk::CompressedLayerMaterial::operator=(
    const Trk::CompressedLayerMaterial& lmp) {
  if (this != &lmp) {
    Trk::LayerMaterialProperties::operator=(lmp);
    // first delete everything
    delete m_binUtility;
    clearMaterial();
    // reassign the material
    fillMaterial(lmp.m_fullMaterial);
    // reassign the binutility and the material bins
    m_binUtility = lmp.binUtility()->clone();
    m_materialBins = lmp.m_materialBins;
  }
  return (*this);
}

Trk::CompressedLayerMaterial* Trk::CompressedLayerMaterial::clone() const {
  return new Trk::CompressedLayerMaterial(*this);
}

void Trk::CompressedLayerMaterial::clearMaterial() {
  // loop over the matrix
  for (auto& matIter : m_fullMaterial) delete matIter;
  m_fullMaterial.clear();
}

void Trk::CompressedLayerMaterial::fillMaterial(
    const Trk::MaterialPropertiesVector& matVector) {
  // reassign
  m_fullMaterial.clear();
  m_fullMaterial.reserve(matVector.size());
  for (const auto& matIter : matVector)
    m_fullMaterial.push_back(matIter ? matIter->clone() : nullptr);
}

Trk::CompressedLayerMaterial& Trk::CompressedLayerMaterial::operator*=(
    double scale) {
  size_t imat1 = 0;
  // the vector iterator
  for (auto& matIter : m_fullMaterial) {
    if (matIter) {
      // clone needed for const correctness
      Trk::MaterialProperties* mprop = (matIter)->clone();
      (*mprop) *= scale;
      delete matIter;
      m_fullMaterial[imat1] = mprop;
    }
  }

  // scale the reference material
  return (*this);
}

const Trk::MaterialProperties* Trk::CompressedLayerMaterial::fullMaterial(
    const Amg::Vector3D& gp) const {
  if (m_fullMaterial.empty() || !m_binUtility) return nullptr;
  // first bin
  size_t ibin1 = m_binUtility->bin(gp, 0);
  // second bin
  size_t ibin2 = (m_binUtility->max(1) == 0) ? 0 : m_binUtility->bin(gp, 1);
  // out of bounds check and return
  return (ibin2 > m_binUtility->max(1)) ? nullptr : material(ibin1, ibin2);
}

MsgStream& Trk::CompressedLayerMaterial::dump(MsgStream& sl) const {
  sl << "Trk::CompressedLayerMaterial : " << std::endl;
  sl << "   - Number of Material bins (1/2) : " << m_binUtility->max(0) + 1
     << " / " << m_binUtility->max(1) + 1 << std::endl;
  sl << "   - Parse full update material    : " << std::endl;
  // -------------------------
  size_t imat1 = 0;
  for (const auto& matIter : m_fullMaterial) {
    if (matIter)
      sl << " Bin [" << imat1 << "] - " << matIter << std::endl;
    else
      sl << " Bin [" << imat1 << "] -  empty " << std::endl;
    ++imat1;
  }
  return sl;
}

std::ostream& Trk::CompressedLayerMaterial::dump(std::ostream& sl) const {
  sl << "Trk::CompressedLayerMaterial : " << std::endl;
  sl << "   - Number of Material bins (1/2) : " << m_binUtility->max(0) + 1
     << " / " << m_binUtility->max(1) + 1 << std::endl;
  sl << "   - Parse full update material    : " << std::endl;  //
  size_t imat1 = 0;
  for (const auto& matIter : m_fullMaterial) {
    if (matIter)
      sl << " Bin [" << imat1 << "] - " << matIter << std::endl;
    else
      sl << " Bin [" << imat1 << "] -  empty " << std::endl;
    ++imat1;
  }
  return sl;
}
