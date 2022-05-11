/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CompoundLayerMaterial.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkGeometry/CompoundLayerMaterial.h"

#include "TrkGeometry/MaterialProperties.h"

Trk::CompoundLayerMaterial::CompoundLayerMaterial()
    : Trk::LayerMaterialProperties(),
      m_materialProperties(),
      m_binUtility(nullptr),
      m_fullComposition(false),
      m_elementTable(Trk::SharedObject<const Trk::ElementTable>(nullptr)) {}

Trk::CompoundLayerMaterial::CompoundLayerMaterial(
    const Trk::BinUtility& binutility, const ValueStore& thicknessBins,
    const ValueStore& x0Bins, const ValueStore& l0Bins, const ValueStore& aBins,
    const ValueStore& zBins, const ValueStore& rhoBins,
    const std::vector<std::vector<MaterialComposition> >& composition,
    bool fComposition)
    : m_binUtility(binutility.clone()),
      m_thicknessBins(thicknessBins),
      m_x0Bins(x0Bins),
      m_l0Bins(l0Bins),
      m_aBins(aBins),
      m_zBins(zBins),
      m_rhoBins(rhoBins),
      m_composition(composition),
      m_fullComposition(fComposition),
      m_elementTable(Trk::SharedObject<const Trk::ElementTable>(nullptr))
{
  resizeMaterialProperties();
}

Trk::CompoundLayerMaterial::CompoundLayerMaterial(
    const Trk::CompoundLayerMaterial& clm)
    : Trk::LayerMaterialProperties(clm),
      m_binUtility(clm.m_binUtility->clone()),
      m_thicknessBins(clm.m_thicknessBins),
      m_x0Bins(clm.m_x0Bins),
      m_l0Bins(clm.m_l0Bins),
      m_aBins(clm.m_aBins),
      m_zBins(clm.m_zBins),
      m_rhoBins(clm.m_rhoBins),
      m_composition(clm.m_composition),
      m_fullComposition(clm.m_fullComposition),
      m_elementTable(clm.m_elementTable)
{
  resizeMaterialProperties();
}

Trk::CompoundLayerMaterial& Trk::CompoundLayerMaterial::operator=(
    const Trk::CompoundLayerMaterial& clm) {
  if (this != &clm) {
    delete m_binUtility;
    m_materialProperties.clear();

    m_binUtility = clm.m_binUtility->clone();
    m_thicknessBins = clm.m_thicknessBins;
    m_x0Bins = clm.m_x0Bins;
    m_l0Bins = clm.m_l0Bins;
    m_aBins = clm.m_aBins;
    m_zBins = clm.m_zBins;
    m_rhoBins = clm.m_rhoBins;
    m_composition = clm.m_composition;
    m_fullComposition = clm.m_fullComposition;
    m_elementTable = clm.m_elementTable;

    resizeMaterialProperties();
  }
  return (*this);
}

Trk::CompoundLayerMaterial::~CompoundLayerMaterial() {
  delete m_binUtility;
}

Trk::CompoundLayerMaterial* Trk::CompoundLayerMaterial::clone() const {
  return new Trk::CompoundLayerMaterial(*this);
}

Trk::CompoundLayerMaterial& Trk::CompoundLayerMaterial::operator*=(double) {
  //!< @todo implement
  // scale the reference material
  return (*this);
}

const Trk::MaterialProperties* Trk::CompoundLayerMaterial::fullMaterial(
    const Amg::Vector3D& gp) const {
  // first bin
  size_t bin0 = m_binUtility->bin(gp, 0);
  // second bin
  size_t bin1 = (m_binUtility->max(1) == 0) ? 0 : m_binUtility->bin(gp, 1);
  return material(bin0, bin1);
}

const Trk::MaterialProperties* Trk::CompoundLayerMaterial::material(
    size_t bin0, size_t bin1) const
{
  const MaterialPropertiesCUP& p = m_materialProperties[bin1][bin0];
  if (!p) {
    // get the size
    const double thickness = m_thicknessBins.value(bin0, bin1);
    // no thickness or no x0 - return a null pointer
    if (thickness == 0.) return nullptr;
    double x0 = 0.;
    double l0 = 0.;
    double a = 0.;
    double z = 0.;
    double rho = 0.;
    // the full composition calculation
    if (m_fullComposition && m_elementTable.get()) {
      //!< @todo measure if this is slow
      for (const auto& eFraction : m_composition[bin1][bin0]) {
        double fraction = eFraction.fraction();
        const Trk::Material* material =
          m_elementTable->element(eFraction.element());
        if (material) {
          x0 += material->X0 * fraction;
          l0 += material->L0 * fraction;
          a += material->A * fraction;
          z += material->Z * fraction;
          rho += material->rho * fraction;
        }
      }
    } else {
      x0 = m_x0Bins.value(bin0, bin1);
      l0 = m_l0Bins.value(bin0, bin1);
      a = m_aBins.value(bin0, bin1);
      z = m_zBins.value(bin0, bin1);
      rho = m_rhoBins.value(bin0, bin1);
    }
    
    // check for 0 material
    if (x0 == 0.) {
      return nullptr;
    }

    // record the material composition
    Trk::MaterialComposition* mComposition =
      new Trk::MaterialComposition(m_composition[bin1][bin0]);
    p.set (std::make_unique<Trk::MaterialProperties>
            (Trk::Material(x0, l0, a, z, rho, 0., mComposition), thickness));
  }
  return p.get();
}

MsgStream& Trk::CompoundLayerMaterial::dump(MsgStream& sl) const {
  sl << "Trk::CompoundLayerMaterial : \n";
  sl << "   - Number of Material bins (1/2) : " << m_binUtility->max(0) + 1
     << " / " << m_binUtility->max(1) + 1 << "\n";
  return sl;
}

std::ostream& Trk::CompoundLayerMaterial::dump(std::ostream& sl) const {
  sl << "Trk::CompoundLayerMaterial : " << std::endl;
  sl << "   - Number of Material bins (1/2) : " << m_binUtility->max(0) + 1
     << " / " << m_binUtility->max(1) + 1 << std::endl;
  return sl;
}


void Trk::CompoundLayerMaterial::resizeMaterialProperties()
{
  m_materialProperties.clear();
  m_materialProperties.resize (m_thicknessBins.valueBinMatrix.size());
  for (size_t i = 0; i < m_materialProperties.size(); i++) {
    m_materialProperties[i].resize (m_thicknessBins.valueBinMatrix[i].size());
  }
}

