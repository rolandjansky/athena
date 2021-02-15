/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Material.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_BINNEDMATERIAL_H
#define TRKGEOMETRY_BINNEDMATERIAL_H

#include <utility>
#include <vector>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <climits>
#include <memory>
#include "TrkGeometry/Material.h"
#include "TrkDetDescrUtils/CompactBinnedArray.h"
#include "TrkDetDescrUtils/CompactBinnedArray1D.h"
#include "TrkDetDescrUtils/CompactBinnedArray2D.h"

namespace Trk {

  typedef std::pair< const Material*, int>   IdentifiedMaterial;

  class BinUtility;

   /** @class BinnedMaterial 
   
    for description of non-homogenous dense volumes
    
    @author sarka.todorova@cern.ch 
  */    
  class BinnedMaterial : public Material {

    public:

      /** Default Constructor needed for POOL */
      BinnedMaterial() = default;

      /** Constructor with arguments */
      BinnedMaterial(float iX0, 
		    float iL0,
		    float iA,
		    float iZ,
		    float iRho,
		    const CompactBinnedArray<const IdentifiedMaterial>* binMat = nullptr):
      Material(iX0,iL0,iA,iZ,iRho),
      m_matBins(binMat)
      {}    

      /** Constructor with averaged material and binning in 1D*/
      BinnedMaterial(const Material* mat, BinUtility*& bu, const std::vector<size_t>& index,
		     const std::vector<IdentifiedMaterial>& detailedMat); 

      /** Constructor with averaged material and binning in 2D*/
      BinnedMaterial(const Material* mat, BinUtility*& bu, std::vector< Trk::BinUtility*>& bVec,
		     const std::vector<std::vector<size_t> >& index,
		     const std::vector<IdentifiedMaterial>& detailedMat);
      
      /** Copy Constructor */
      BinnedMaterial(const BinnedMaterial& amc);

      /** Destructor - delete the composition if there */
      ~BinnedMaterial() = default;

      /** Assignment operator */
      BinnedMaterial& operator=(const BinnedMaterial& amc);

      /** access to layer bin utility */
      const Trk::BinUtility*  layerBinUtility(const Amg::Vector3D& position) const;

      /** layer bin */
      size_t  layerBin(const Amg::Vector3D& position) const;

      /** access to material/id per bin */
      const IdentifiedMaterial* material(const Amg::Vector3D& position ) const;
      const IdentifiedMaterial* materialNext(const Amg::Vector3D& pos,const Amg::Vector3D& dir, bool layOnly ) const;
  
  private:
      std::vector<const Trk::IdentifiedMaterial* > ptrs() const;
      std::vector<IdentifiedMaterial> m_matVec;
      using binsPtr_t = std::unique_ptr<const CompactBinnedArray< const IdentifiedMaterial > >;
      binsPtr_t m_matBins;  
  };


}

inline const Trk::BinUtility* Trk::BinnedMaterial::layerBinUtility(const Amg::Vector3D& position) const
 {
      return m_matBins ? m_matBins->layerBinUtility(position) : nullptr;
 }

inline size_t Trk::BinnedMaterial::layerBin(const Amg::Vector3D& position) const
 {
      return m_matBins ? m_matBins->layerBin(position) : 0;
 }
  
#endif
