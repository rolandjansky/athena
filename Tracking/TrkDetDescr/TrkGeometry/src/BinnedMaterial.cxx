/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkGeometry/BinnedMaterial.h"

/** Constructor with averaged material and binning in 1D*/
Trk::BinnedMaterial::BinnedMaterial(const Trk::Material*& mat, Trk::BinUtility*& bu, const std::vector<size_t>& index, 
				    const std::vector< const Trk::IdentifiedMaterial* >& detailedMat ) : 
  Trk::Material(*mat),
  m_matBins(0)
{
  Trk::CompactBinnedArray1D<const Trk::IdentifiedMaterial >* bm = 
    new Trk::CompactBinnedArray1D<const Trk::IdentifiedMaterial >(detailedMat,index,bu);
  m_matBins = bm;
}    

/** Constructor with averaged material and binning in 2D*/
Trk::BinnedMaterial::BinnedMaterial(const Trk::Material*& mat, Trk::BinUtility*& bu, std::vector< Trk::BinUtility*>& bVec,
				    const std::vector<std::vector<size_t> >& index, 
				    const std::vector<const Trk::IdentifiedMaterial* >& detailedMat ) :
  Trk::Material(*mat),
  m_matBins(0)
{
  Trk::CompactBinnedArray2D<const Trk::IdentifiedMaterial >* bm = 
    new Trk::CompactBinnedArray2D<const Trk::IdentifiedMaterial >(detailedMat,index,bu,bVec);
  m_matBins = bm;  
}    

/** access to binned material */
const Trk::IdentifiedMaterial* Trk::BinnedMaterial::material(const Amg::Vector3D& position ) const
{ 
  const Trk::IdentifiedMaterial* mat =  m_matBins ? m_matBins->object(position) : 0 ;
  return mat ;        
}
  
/** access to binned material */
const Trk::IdentifiedMaterial* Trk::BinnedMaterial::materialNext(const Amg::Vector3D& position,const Amg::Vector3D& direction, bool layOnly ) const
{ 
  const Trk::IdentifiedMaterial* mat =  m_matBins ? m_matBins->nextObject(position,direction,layOnly) : 0 ;
  return mat ;        
}
  
