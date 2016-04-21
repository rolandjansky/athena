/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkGeometry/BinnedMaterial.h"

/** Constructor with averaged material and binning in 1D*/
Trk::BinnedMaterial::BinnedMaterial(const Trk::Material* mat, Trk::BinUtility* bu, std::vector<size_t>& index, 
				    std::vector< const Trk::IdentifiedMaterial* >& detailedMat ) : 
  Trk::Material(*mat),
  m_matBins(0),
  m_compactBins(0)
{
  Trk::CompactBinnedArray1D<const Trk::IdentifiedMaterial >* bm = 
    new Trk::CompactBinnedArray1D<const Trk::IdentifiedMaterial >(detailedMat,index,bu);
  m_matBins=bm;
}    

/** Constructor with averaged material and binning in 2D*/
Trk::BinnedMaterial::BinnedMaterial(const Trk::Material* mat, Trk::BinUtility* bu, std::vector< Trk::BinUtility*>& bVec,
				    const std::vector<std::vector<size_t> >& index, 
				    const std::vector<const Trk::IdentifiedMaterial* >& detailedMat ) :
  Trk::Material(*mat),
  m_matBins(0),
  m_compactBins(0)
{
  Trk::CompactBinnedArray2D<const Trk::IdentifiedMaterial >* bm = 
    new Trk::CompactBinnedArray2D<const Trk::IdentifiedMaterial >(detailedMat,index,bu,bVec);
  m_matBins=bm;  
}    

/** Constructor with averaged material and binning in 1D x XD*/
Trk::BinnedMaterial::BinnedMaterial(const Trk::Material* mat, Trk::BinUtility* bu, 
				    std::vector<size_t>& index,
				    std::vector< const Trk::CompactBinnedArray<const Trk::IdentifiedMaterial>* >& detailedMat) :
  Trk::Material(*mat),
  m_matBins(0),
  m_compactBins(0)
{
  Trk::CompactBinnedArray1D< const Trk::CompactBinnedArray<const Trk::IdentifiedMaterial> >* cm = 
    new Trk::CompactBinnedArray1D< const Trk::CompactBinnedArray<const Trk::IdentifiedMaterial> >(detailedMat,index,bu);
  m_compactBins=cm;  
}

/** access to binned material  TODO : return simultaneously: distance to bin entry and exit, previous and next material*/
const Trk::IdentifiedMaterial* Trk::BinnedMaterial::material(const Amg::Vector3D& position ) const
{ 
  if (m_matBins) return m_matBins->object(position);

  if (m_compactBins) {
    const Trk::CompactBinnedArray<const Trk::IdentifiedMaterial >* mb = m_compactBins->object(position);
    if (mb)  return mb->object(position);
  }
  
  return 0;
}
  
/** access to binned material */
const Trk::IdentifiedMaterial* Trk::BinnedMaterial::materialNext(const Amg::Vector3D& position,const Amg::Vector3D& direction, bool layOnly ) const
{ 
  if (m_matBins) return m_matBins->nextObject(position, direction, layOnly);

  if (m_compactBins) {
    const Trk::CompactBinnedArray<const Trk::IdentifiedMaterial >* mb = m_compactBins->object(position);
    if (mb)  return mb->nextObject(position, direction, layOnly);
  }
  
  return 0;
}

const Trk::IdentifiedMaterial* Trk::BinnedMaterial::layerMaterial(const Amg::Vector3D& position) const
{
  if (m_matBins) return material(position);

  if (m_compactBins) {
    const Trk::CompactBinnedArray<const Trk::IdentifiedMaterial >* mb = m_compactBins->object(position);
    if (mb)  return mb->arrayObjects().back();
  }
  return 0;
}

const Trk::IdentifiedMaterial* Trk::BinnedMaterial::layerMaterial(size_t bin) const
{
  if (m_matBins && bin<m_matBins->arrayObjects().size()) return m_matBins->arrayObjects()[bin];

  // dummy call ?
  if (m_compactBins && bin<m_compactBins->arrayObjects().size()) { 
    const Trk::CompactBinnedArray<const Trk::IdentifiedMaterial >* mb = m_compactBins->arrayObjects()[bin];
    if (mb)  return mb->arrayObjects().back();
  }

  return 0;
}

/** access to bin content along path */
Trk::MatBinMap  Trk::BinnedMaterial::binMap(const Amg::Vector3D& position,const Amg::Vector3D& direction, float min, float max) const
{
  // bin navigation : bounds defined by distance
  if (m_matBins) {
    Trk::MatBinMap map = m_matBins->binMap(position,direction,min,max); 
    return map; 
  } else if (m_compactBins) {          
    Trk::BinMap<const Trk::CompactBinnedArray<const Trk::IdentifiedMaterial> > cmap = m_compactBins->binMap(position,direction,min,max);
    // merge local maps
    Trk::BinMap<const Trk::IdentifiedMaterial> merged(min,max);
    float shift = 0.;
    Trk::BinPath topPath = cmap.map;
    
    for (unsigned int ibu = 0; ibu<topPath.steps.size()-1; ibu++) {
      if (ibu >= cmap.objects.size() ) break;    // should be checked at map creation
      if (topPath.steps[ibu].first>0.) shift = topPath.steps[ibu].first; 
      if (topPath.steps[ibu].second<0) continue; 
      const Trk::CompactBinnedArray<const Trk::IdentifiedMaterial>* ba=cmap.objects[ibu];
      Trk::BinMap<const Trk::IdentifiedMaterial> mapBin =  ba->binMap(position+shift*direction,direction,
								      topPath.steps[ibu].first-shift,topPath.steps[ibu+1].first-shift);
      merged.mergeIn(mapBin,shift);
    }
    return merged;
  }

  // dummy
  std::vector<const IdentifiedMaterial*> dummyMaterial;
  dummyMaterial.push_back(new std::pair<const Material*, int>(this,0));
  return Trk::MatBinMap(dummyMaterial,Trk::BinPath(Trk::BinIntersection(0.,0)));
}
