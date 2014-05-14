/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BinnedLayerMaterial.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkGeometry/BinnedLayerMaterial.h"
#include "TrkGeometry/MaterialProperties.h"

Trk::BinnedLayerMaterial::BinnedLayerMaterial() :
  Trk::LayerMaterialProperties(),
  m_binUtility(0)
{}

Trk::BinnedLayerMaterial::BinnedLayerMaterial(Trk::BinUtility& binutility) :
  Trk::LayerMaterialProperties(),
  m_binUtility(binutility.clone())
{
 // reserve
 m_fullMaterial.reserve(binutility.max(1)+1);
 for (unsigned int ibin2 = 0; ibin2 < (unsigned int)binutility.max(1)+1; ++ibin2){
     // create the vector for the push_back
     Trk::MaterialPropertiesVector matVec;
     matVec.reserve(binutility.max(0)+1);
     for (unsigned int ibin = 0; ibin < (unsigned int)binutility.max(0)+1; ++ibin)
         matVec.push_back(0);
    m_fullMaterial.push_back(matVec);
 }
}

Trk::BinnedLayerMaterial::BinnedLayerMaterial(const Trk::BinUtility& binutility,
                                              const MaterialPropertiesVector& fullProperties,
                                              double splitFactor) :
  Trk::LayerMaterialProperties(splitFactor),
  m_binUtility(binutility.clone())
{
 // constructor from a single vector
 clearMaterial();
 // fill the material with deep copy
 m_fullMaterial.push_back(fullProperties);
}

Trk::BinnedLayerMaterial::BinnedLayerMaterial(const Trk::BinUtility& binutility,
                                              const MaterialPropertiesMatrix& fullProperties,
                                              double splitFactor) :
  Trk::LayerMaterialProperties(splitFactor),
  m_binUtility(binutility.clone()),
  m_fullMaterial(fullProperties)
{}

Trk::BinnedLayerMaterial::~BinnedLayerMaterial()
{
  delete m_binUtility;
  clearMaterial();      
}


Trk::BinnedLayerMaterial::BinnedLayerMaterial( const Trk::BinnedLayerMaterial& lmp) :
 Trk::LayerMaterialProperties(lmp),
 m_binUtility(lmp.m_binUtility->clone())
{
  // clear the material
  clearMaterial();
  // and fill the material
  fillMaterial(lmp.m_fullMaterial);
}

Trk::BinnedLayerMaterial& Trk::BinnedLayerMaterial::operator=(const Trk::BinnedLayerMaterial& lmp)
{
  if (this!=&lmp) {
     Trk::LayerMaterialProperties::operator=(lmp);         
    // first delete everything
    delete m_binUtility;
    // reassign
    m_binUtility = lmp.binUtility()->clone();
    clearMaterial();
    // reassign teh material
    fillMaterial(lmp.m_fullMaterial);
  }
  return (*this);
}

Trk::BinnedLayerMaterial* Trk::BinnedLayerMaterial::clone() const
{
  return new Trk::BinnedLayerMaterial(*this);
}

void Trk::BinnedLayerMaterial::clearMaterial()
{
    // clear the full material
    for (auto& matMatrixIter : m_fullMaterial){
        for (auto& matIter : matMatrixIter)
           delete matIter;
    }

    m_fullMaterial.clear();
}


void Trk::BinnedLayerMaterial::fillMaterial(const Trk::MaterialPropertiesMatrix& matMatrix)
{
    m_fullMaterial.reserve(m_binUtility->max(1)+1);
    for (auto& matMatrixIter : matMatrix){
      // the vector to be copied
      Trk::MaterialPropertiesVector matVector;
      matVector.reserve(m_binUtility->max(0)+1);
      // reassign
      for (auto& matIter : matMatrixIter)
            matVector.push_back( matIter ? matIter->clone() : 0);
      m_fullMaterial.push_back(matVector);
    }
}

Trk::BinnedLayerMaterial& Trk::BinnedLayerMaterial::operator*=(double scale)
{
    // scale the full material
    unsigned int imat2 = 0;
    for (auto& matMatrixIter : m_fullMaterial){
      unsigned int imat1 = 0;
      // the vector iterator
      for (auto& matIter : matMatrixIter){
        if (matIter){
           Trk::MaterialProperties* mprop = matIter->clone();
           (*mprop) *= scale;
           delete matIter;
           m_fullMaterial[imat2][imat1] = mprop;
        }
        ++imat1;
      }
      ++imat2;
    }
   return (*this);
}


const Trk::MaterialProperties* Trk::BinnedLayerMaterial::fullMaterial(const Amg::Vector3D& gp) const
{ 
  if (!m_fullMaterial.size() || !m_binUtility ) return 0;
  // the first bin
  size_t ibin1 = m_binUtility->bin(gp,0);
  size_t ibin2 = m_binUtility->max(1) ? m_binUtility->bin(gp,1) : 0;
  return m_fullMaterial[ibin2][ibin1];
}

MsgStream& Trk::BinnedLayerMaterial::dump( MsgStream& sl) const
{
  sl << "Trk::BinnedLayerMaterial : " << std::endl;
  sl << "   - Number of Material bins (1/2) : " << m_binUtility->max(0)+1 << " / " << m_binUtility->max(1)+1 << std::endl;
  sl << "   - Parse full update material    : " << std::endl;
  // output  the full material
  unsigned int imat2 = 0;
  for (auto& matMatrixIter : m_fullMaterial){
    unsigned int imat1 = 0;
    // the vector iterator
    for (auto& matIter : matMatrixIter){
      if (matIter)  sl << " Bin [" << imat2 << "][" << imat1 << "] - " << (*matIter) << std::endl;
      else  sl << " Bin [" << imat2 << "][" << imat1 << "] -  empty "  << std::endl;
      ++imat1;
    }
    ++imat2;
  }
  return sl;
}

std::ostream& Trk::BinnedLayerMaterial::dump( std::ostream& sl) const
{
  sl << "Trk::BinnedLayerMaterial : " << std::endl;
  sl << "   - Number of Material bins (1/2) : " << m_binUtility->max(0)+1 << " / " << m_binUtility->max(1)+1  << std::endl;
  sl << "   - Parse full update material    : " << std::endl;  // 
  // output  the full material
  unsigned int imat2 = 0;
  for (auto& matMatrixIter : m_fullMaterial){
    unsigned int imat1 = 0;
    // the vector iterator
    for (auto& matIter : matMatrixIter){
      if (matIter)  sl << " Bin [" << imat2 << "][" << imat1 << "] - " << (*matIter) << std::endl;
      else  sl << " Bin [" << imat2 << "][" << imat1 << "] -  empty "  << std::endl;
      ++imat1;
    }
    ++imat2;
  }
  return sl;
}
