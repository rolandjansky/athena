/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// LayerMaterialRecord.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkGeometry/LayerMaterialRecord.h"

#include <climits>

#include "EventPrimitives/EventPrimitivesToStringConverter.h"
#include "TrkGeometry/AssociatedMaterial.h"
#include "TrkGeometry/CylinderLayer.h"
#include "TrkGeometry/DiscLayer.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkSurfaces/CylinderBounds.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/SurfaceBounds.h"

Trk::LayerMaterialRecord::LayerMaterialRecord()
    : m_layerThickness(0.),
      m_binUtility(nullptr),
      m_bins0(0),
      m_bins1(0),
      m_minFraction(0.),
      m_steps(0),
      m_pos(Amg::Vector3D(0., 0., 0.)),
      m_emptyHitCase(false),
      m_s(0.),
      m_s_in_x0(0.),
      m_s_in_l0(0.),
      m_a(0.),
      m_z(0.),
      m_rho(0.),
      m_assoc(Trk::EffectiveNumAtoms) {
  // m_pos = Amg::Vector3D(0.,0.,0.);
}

Trk::LayerMaterialRecord::LayerMaterialRecord(
    double thickness, const BinUtility* binutils, double minfraction,
    Trk::MaterialAssociationType assoc)
    : m_layerThickness(thickness),
      m_binUtility(binutils ? binutils->clone() : nullptr),
      m_bins0(binutils ? (binutils->max(0) + 1) : 1),
      m_bins1(binutils && binutils->dimensions() > 1 ? (binutils->max(1) + 1)
                                                     : 1),
      m_minFraction(minfraction),
      m_steps(0),
      m_pos(Amg::Vector3D(0., 0., 0.)),
      m_emptyHitCase(false),
      m_s(0.),
      m_s_in_x0(0.),
      m_s_in_l0(0.),
      m_a(0.),
      m_z(0.),
      m_rho(0.),
      m_assoc(assoc) {
  // initialize for the run
  const auto zeroedVectorDbl = std::vector<double>(m_bins0, 0.);
  const auto zeroedVectorUInt = std::vector<unsigned int>(m_bins0, 0);
  const auto zeroedVectorVector3D = std::vector<Amg::Vector3D>(m_bins0, m_pos);
  using Element_t = std::map<unsigned int, double>;
  const auto zeroedVectorElements =
      std::vector<Element_t>(m_bins0, Element_t());
  for (int ibin = 0; ibin < m_bins1; ++ibin) {
    // run-related parameters
    m_run_pos.push_back(zeroedVectorVector3D);
    m_run_events.push_back(zeroedVectorUInt);
    m_run_s.push_back(zeroedVectorDbl);
    m_run_s_in_x0.push_back(zeroedVectorDbl);
    m_run_s_in_l0.push_back(zeroedVectorDbl);
    m_run_a.push_back(zeroedVectorDbl);
    m_run_z.push_back(zeroedVectorDbl);
    m_run_rho.push_back(zeroedVectorDbl);
    m_run_elements.push_back(zeroedVectorElements);
  }
}

Trk::LayerMaterialRecord::LayerMaterialRecord(
    const Trk::LayerMaterialRecord& lmr)
    : m_layerThickness(lmr.m_layerThickness),
      m_binUtility(lmr.m_binUtility ? lmr.m_binUtility->clone() : nullptr),
      m_bins0(lmr.m_bins0),
      m_bins1(lmr.m_bins1),
      m_minFraction(lmr.m_minFraction),
      m_steps(lmr.m_steps),
      m_pos(Amg::Vector3D(0., 0., 0.)),
      m_emptyHitCase(lmr.m_emptyHitCase),
      m_s(lmr.m_s),
      m_s_in_x0(lmr.m_s_in_x0),
      m_s_in_l0(lmr.m_s_in_l0),
      m_a(lmr.m_a),
      m_z(lmr.m_z),
      m_rho(lmr.m_rho),
      m_elements(lmr.m_elements),
      m_assoc(lmr.m_assoc),
      m_run_events(lmr.m_run_events),
      m_run_pos(lmr.m_run_pos),
      m_run_s(lmr.m_run_s),
      m_run_s_in_x0(lmr.m_run_s_in_x0),
      m_run_s_in_l0(lmr.m_run_s_in_l0),
      m_run_a(lmr.m_run_a),
      m_run_z(lmr.m_run_z),
      m_run_rho(lmr.m_run_rho),
      m_run_elements(lmr.m_run_elements) {
  copyMaterial(lmr.m_associatedLayerMaterial);
}

Trk::LayerMaterialRecord& Trk::LayerMaterialRecord::operator=(
    const Trk::LayerMaterialRecord& lmr) {
  if (this != &lmr) {
    m_layerThickness = lmr.m_layerThickness;
    delete m_binUtility;
    m_binUtility = lmr.m_binUtility ? lmr.m_binUtility->clone() : nullptr;
    m_bins0 = lmr.m_bins0;
    m_bins1 = lmr.m_bins1;
    m_minFraction = lmr.m_minFraction;
    m_assoc = lmr.m_assoc;
    m_steps = lmr.m_steps;
    m_pos = lmr.m_pos;
    m_emptyHitCase = lmr.m_emptyHitCase;
    m_s = lmr.m_s;
    m_s_in_x0 = lmr.m_s_in_x0;
    m_s_in_l0 = lmr.m_s_in_l0;
    m_a = lmr.m_a;
    m_z = lmr.m_z;
    m_rho = lmr.m_rho;
    m_elements = lmr.m_elements;
    m_run_events = lmr.m_run_events;
    m_run_s = lmr.m_run_s;
    m_run_s_in_x0 = lmr.m_run_s_in_x0;
    m_run_s_in_l0 = lmr.m_run_s_in_l0;
    m_run_pos = lmr.m_run_pos;
    m_run_a = lmr.m_run_a;
    m_run_z = lmr.m_run_z;
    m_run_rho = lmr.m_run_rho;
    m_run_elements = lmr.m_run_elements;
    // deal with the material
    clearMaterial();
    copyMaterial(lmr.m_associatedLayerMaterial);
  }
  return (*this);
}

Trk::LayerMaterialRecord::~LayerMaterialRecord() {
  // don't delete the material -> its given to the outside world
  delete m_binUtility;
}

void Trk::LayerMaterialRecord::associateGeantinoHit(const Amg::Vector3D& pos,
                                                    double s,
                                                    const Trk::Material& mat) {
  m_steps++;

  m_s += s;
  // path association method
  m_pos += pos;
  // path lenght updates
  m_s_in_x0 += s / mat.X0;
  m_s_in_l0 += s / mat.L0;
  // effective rho, A, Z weithed by pathlength through it
  m_rho += mat.rho * s;
  m_a += mat.A * s * mat.rho;
  m_z += mat.Z * s * mat.rho;

  // record the composition - since this is mainly for hadronic interactions :
  // weight by s/L0
  MaterialComposition* mComposition = mat.composition;
  if (mComposition) {
    for (auto& it : (*mComposition)) {
      // element identification
      unsigned int Z = uchar2uint(it.first);
      double fraction = uchar2dfrac(it.second);
      // record the elements, let's weight the fractions in s/mat.L0
      auto eIter = m_elements.find(Z);
      if (eIter == m_elements.end())
        m_elements[Z] = fraction * s / mat.L0;
      else
        m_elements[Z] += fraction * s / mat.L0;
    }
  }
}

void Trk::LayerMaterialRecord::associateEmptyHit(const Amg::Vector3D& pos) {
  // just remember that you had an empty hit
  m_emptyHitCase = true;
  // take the position to increase the event counter by one
  m_pos = pos;
}

Trk::AssociatedMaterial* Trk::LayerMaterialRecord::finalizeEvent(
    const Trk::Layer& lay, bool fullHit) {
  Trk::AssociatedMaterial* fullHitMaterial = nullptr;
  // empty hit scaling
  if (m_emptyHitCase) {
    // averge the hit positions
    int rBin0 = m_binUtility ? m_binUtility->bin(m_pos, 0) : 0;
    int rBin1 = (m_binUtility && m_binUtility->dimensions() > 1)
                    ? m_binUtility->bin(m_pos, 1)
                    : 0;
    // simply increas the event counter
    m_run_events[rBin1][rBin0]++;
    // material hit collection
  } else if (m_steps) {
    // only if there was a single step in the event
    Amg::Vector3D hitPosition(m_pos * 1. / (m_steps));
    // get the correction factor depending on the layer type
    double corrFactorInv = fabs(1. / lay.surfaceRepresentation().pathCorrection(
                                         hitPosition, hitPosition));
    // averge the hit positions
    int rBin0 = m_binUtility ? m_binUtility->bin(hitPosition, 0) : 0;
    int rBin1 = (m_binUtility && m_binUtility->dimensions() > 1)
                    ? m_binUtility->bin(hitPosition, 1)
                    : 0;

    // event averaging
    m_run_events[rBin1][rBin0]++;
    m_run_s[rBin1][rBin0] += m_s;
    m_run_pos[rBin1][rBin0] += hitPosition;
    m_run_s_in_x0[rBin1][rBin0] += m_s_in_x0 * corrFactorInv;
    m_run_s_in_l0[rBin1][rBin0] += m_s_in_l0 * corrFactorInv;
    // a & z are normalised to rho
    m_run_a[rBin1][rBin0] += m_a / m_rho;
    m_run_z[rBin1][rBin0] += m_z / m_rho;
    // rho is normalised to the layer thickness times corection factor
    m_run_rho[rBin1][rBin0] += m_rho * corrFactorInv / m_layerThickness;  // ST

    // add to the run element table
    for (auto& eIter : m_elements) {
      // first normalize the element fraction
      double nef = eIter.second * corrFactorInv / m_s_in_l0;
      if (m_run_elements[rBin1][rBin0].find(eIter.first) ==
          m_run_elements[rBin1][rBin0].end())
        m_run_elements[rBin1][rBin0][eIter.first] = nef;
      else
        m_run_elements[rBin1][rBin0][eIter.first] += nef;
    }

    // just for validation purpose
    if (fullHit) {
      // average the material for the per event validation
      double eventNorm = 1. / double(m_run_events[rBin1][rBin0]);

      // normalize the value by number of recorded events - each event counts
      // the same
      double a = m_run_a[rBin1][rBin0] * eventNorm;
      double z = m_run_z[rBin1][rBin0] * eventNorm;
      double rho = m_run_rho[rBin1][rBin0] * eventNorm;

      // the average s/X0 and s/L0 is the average over all events corrected by
      // the incident angle
      double s_in_x0 = m_run_s_in_x0[rBin1][rBin0] * eventNorm;
      double s_in_l0 = m_run_s_in_l0[rBin1][rBin0] * eventNorm;

      // condense to the layer thickness
      double x0 = m_layerThickness / s_in_x0;
      double l0 = m_layerThickness / s_in_l0;

      /** Constructor with explicit arguments  */
      fullHitMaterial = new Trk::AssociatedMaterial(
          hitPosition, m_run_s[rBin1][rBin0] * eventNorm, x0, l0, a, z, rho,
          corrFactorInv, lay.enclosingTrackingVolume(), &lay);
    }
  }

  // reset event variables
  m_emptyHitCase = false;
  m_steps = 0;
  m_pos = Amg::Vector3D(0., 0., 0.);
  m_s = 0.;
  m_s_in_x0 = 0.;
  m_s_in_l0 = 0.;
  m_a = 0.;
  m_z = 0.;
  m_rho = 0.;
  m_elements.clear();

  return fullHitMaterial;
}

void Trk::LayerMaterialRecord::finalizeRun(bool recordElements) {
  m_associatedLayerMaterial.reserve(m_bins1);

  for (int ibin1 = 0; ibin1 < m_bins1; ++ibin1) {
    // create the vector first
    Trk::MaterialPropertiesVector matVector;
    matVector.reserve(m_bins0);
    // loop over local 1 bins
    for (int ibin0 = 0; ibin0 < m_bins0; ++ibin0) {
      Trk::MaterialProperties* binMaterial = nullptr;
      if (m_run_events[ibin1][ibin0]) {
        // The event norm
        double eventNorm = 1. / double(m_run_events[ibin1][ibin0]);

        // normalize the value by number of recorded events - each event counts
        // the same
        m_run_a[ibin1][ibin0] *= eventNorm;
        m_run_z[ibin1][ibin0] *= eventNorm;
        m_run_rho[ibin1][ibin0] *= eventNorm;

        // the average s/X0 and s/L0 is the average over all events corrected by
        // the incident angle
        m_run_s_in_x0[ibin1][ibin0] *= eventNorm;
        m_run_s_in_l0[ibin1][ibin0] *= eventNorm;

        // condense to the layer thickness
        double x0 = m_layerThickness / m_run_s_in_x0[ibin1][ibin0];
        double l0 = m_layerThickness / m_run_s_in_l0[ibin1][ibin0];

        // prepare the material composition
        Trk::MaterialComposition* matComposition = nullptr;
        if (recordElements) {
          // pre-loop to get a sample
          double preTotalFraction = 0.;
          std::map<unsigned int, double> binElements =
              m_run_elements[ibin1][ibin0];
          for (auto& peIter : binElements) {
            // normalize the fraction to the number of events
            peIter.second *= eventNorm;
            preTotalFraction += peIter.second;
          }
          // first loop to sort rescale
          std::map<double, unsigned int> probabilityOrdered;
          double totalFraction = 0.;
          for (auto& eIter : binElements) {
            // get the fraction
            double eFraction = eIter.second / preTotalFraction;
            if (eFraction < m_minFraction) continue;
            probabilityOrdered[eIter.second] = eIter.first;
            totalFraction += eIter.second;
          }
          // second loop to fill the element fractions
          std::vector<Trk::ElementFraction> elementFractions;
          elementFractions.reserve(binElements.size());
          for (auto& poEl : probabilityOrdered) {
            double fracEl = poEl.first / totalFraction;
            unsigned int fracEluChar = fracEl * UCHAR_MAX;
            elementFractions.emplace_back(poEl.second, fracEluChar);
          }
          // reverse the order to have the one with the highest fraction first
          std::reverse(elementFractions.begin(), elementFractions.end());
          matComposition = new Trk::MaterialComposition(elementFractions);
        }

        Trk::Material material(x0, l0, m_run_a[ibin1][ibin0],
                               m_run_z[ibin1][ibin0], m_run_rho[ibin1][ibin0],
                               0., matComposition);

        binMaterial = new Trk::MaterialProperties(material, m_layerThickness);
      }
      matVector.push_back(binMaterial);
    }
    m_associatedLayerMaterial.push_back(matVector);
  }
}

void Trk::LayerMaterialRecord::clearMaterial() {
  Trk::MaterialPropertiesMatrix::iterator matMatrixIter =
      m_associatedLayerMaterial.begin();
  Trk::MaterialPropertiesMatrix::iterator matMatrixIterEnd =
      m_associatedLayerMaterial.end();
  for (; matMatrixIter != matMatrixIterEnd; ++matMatrixIter) {
    // loop over the subsets
    std::vector<const Trk::MaterialProperties*>::iterator matIter =
        (*matMatrixIter).begin();
    std::vector<const Trk::MaterialProperties*>::iterator matIterEnd =
        (*matMatrixIter).end();
    for (; matIter != matIterEnd; ++matIter) delete (*matIter);
  }
  m_associatedLayerMaterial.clear();
}

void Trk::LayerMaterialRecord::copyMaterial(
    const MaterialPropertiesMatrix& materialMatrix) {
  // clear the vector
  m_associatedLayerMaterial.clear();

  Trk::MaterialPropertiesMatrix::const_iterator matMatrixIter =
      materialMatrix.begin();
  Trk::MaterialPropertiesMatrix::const_iterator matMatrixIterEnd =
      materialMatrix.end();
  for (; matMatrixIter != matMatrixIterEnd; ++matMatrixIter) {
    Trk::MaterialPropertiesVector matProp;
    // loop over the subsets
    std::vector<const Trk::MaterialProperties*>::const_iterator matIter =
        (*matMatrixIter).begin();
    std::vector<const Trk::MaterialProperties*>::const_iterator matIterEnd =
        (*matMatrixIter).end();
    for (; matIter != matIterEnd; ++matIter) {
      // test it
      matProp.push_back(((*matIter) ? (*matIter)->clone() : nullptr));
    }
    // and now push back the vector
    m_associatedLayerMaterial.push_back(matProp);
  }
}
