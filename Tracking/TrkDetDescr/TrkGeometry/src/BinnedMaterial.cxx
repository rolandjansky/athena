/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkGeometry/BinnedMaterial.h"

/** Constructor with averaged material and binning in 1D*/
Trk::BinnedMaterial::BinnedMaterial(const Trk::Material* mat, Trk::BinUtility*& bu, const std::vector<size_t>& index, 
				    const std::vector<Trk::IdentifiedMaterial>& detailedMat ) : 
  Trk::Material(*mat),
  m_matVec (detailedMat),
  m_matBins (std::make_unique<Trk::CompactBinnedArray1D<const Trk::IdentifiedMaterial> >(ptrs(),index,bu))
{
}    

/** Constructor with averaged material and binning in 2D*/
Trk::BinnedMaterial::BinnedMaterial(const Trk::Material* mat, Trk::BinUtility*& bu, std::vector< Trk::BinUtility*>& bVec,
				    const std::vector<std::vector<size_t> >& index, 
				    const std::vector<Trk::IdentifiedMaterial>& detailedMat ) :
  Trk::Material(*mat),
  m_matVec (detailedMat),
  m_matBins (std::make_unique<Trk::CompactBinnedArray2D<const Trk::IdentifiedMaterial> >(ptrs(),index,bu,bVec))
{
}    

Trk::BinnedMaterial::BinnedMaterial(const BinnedMaterial& amc) :
  Material(amc),
  m_matVec (amc.m_matVec)
{
  if (amc.m_matBins) {
    if (!m_matVec.empty()) {
      m_matBins = binsPtr_t (amc.m_matBins->clone(ptrs()));
    }
    else {
      m_matBins = binsPtr_t (amc.m_matBins->clone());
    }
  }
}

/** Assignment operator */
Trk::BinnedMaterial&
Trk::BinnedMaterial::operator=(const BinnedMaterial& amc)
{
  if (this != &amc){
    X0          = amc.X0;
    L0          = amc.L0;
    A           = amc.A;
    Z           = amc.Z;
    rho         = amc.rho;  
    dEdX        = amc.dEdX;  
    zOaTr       = amc.zOaTr;
    m_matVec    = amc.m_matVec;
    m_matBins.reset();
    if (amc.m_matBins) {
      if (!m_matVec.empty()) {
        m_matBins = binsPtr_t (amc.m_matBins->clone(ptrs()));
      }
      else {
        m_matBins =  binsPtr_t (amc.m_matBins->clone());
      }
    }
  }
  return (*this);
}

/** access to binned material */
const Trk::IdentifiedMaterial* Trk::BinnedMaterial::material(const Amg::Vector3D& position ) const
{ 
  const Trk::IdentifiedMaterial* mat =  m_matBins ? m_matBins->object(position) : nullptr ;
  return mat ;        
}
  
/** access to binned material */
const Trk::IdentifiedMaterial* Trk::BinnedMaterial::materialNext(const Amg::Vector3D& position,const Amg::Vector3D& direction, bool layOnly ) const
{ 
  const Trk::IdentifiedMaterial* mat =  m_matBins ? m_matBins->nextObject(position,direction,layOnly) : nullptr ;
  return mat ;        
}
  

std::vector<const Trk::IdentifiedMaterial* >
Trk::BinnedMaterial::ptrs() const
{
  std::vector<const Trk::IdentifiedMaterial* > p;
  p.reserve (m_matVec.size());
  for (const Trk::IdentifiedMaterial& m : m_matVec) {
    p.push_back (&m);
  }
  return p;
}
