/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "TrkGeometry/Material.h"
#include "TrkDetDescrUtils/BinMap.h"
#include "TrkDetDescrUtils/CompactBinnedArray.h"
#include "TrkDetDescrUtils/CompactBinnedArray1D.h"
#include "TrkDetDescrUtils/CompactBinnedArray2D.h"

namespace Trk {

  typedef std::pair< const Material*, int>   IdentifiedMaterial;

  typedef BinMap<const IdentifiedMaterial>   MatBinMap;

  class BinUtility;

   /** @class BinnedMaterial 
   
    for description of non-homogenous dense volumes 
    
    @author sarka.todorova@cern.ch 
  */    
  class BinnedMaterial : public Material {

    public:

      /** Default Constructor needed for POOL */
      BinnedMaterial():
      Material()
      {}

      /** Constructor with arguments */
      BinnedMaterial(float iX0, 
		    float iL0,
		    float iA,
		    float iZ,
		    float iRho,
		    const CompactBinnedArray<const IdentifiedMaterial>* binMat = 0):
      Material(iX0,iL0,iA,iZ,iRho),
	m_matBins(binMat),
        m_compactBins(0)
	{}    

      /** Constructor with averaged material and binning in 1D*/
      BinnedMaterial(const Material* mat, BinUtility* bu, std::vector<size_t>& index,
		     std::vector< const IdentifiedMaterial*>& detailedMat); 

      /** Constructor with averaged material and binning in 2D*/
      BinnedMaterial(const Material* mat, BinUtility* bu, std::vector< Trk::BinUtility*>& bVec,
		     const std::vector<std::vector<size_t> >& index,
		     const std::vector< const IdentifiedMaterial* >& detailedMat);

      /** Constructor with averaged material and binning in 1D x 2D*/
      BinnedMaterial(const Material* mat, BinUtility* bu,
		     std::vector<size_t>& index,
		     std::vector< const CompactBinnedArray<const IdentifiedMaterial>* >& detailedMat);
      
      /** Copy Constructor */
      BinnedMaterial(const BinnedMaterial& amc) :
      Material(amc),
	m_matBins(0),
        m_compactBins(0)
	  { if (amc.m_matBins) m_matBins=amc.m_matBins->clone(); }

      /** Desctructor - delete the composition if there */
      ~BinnedMaterial() { delete m_matBins; delete m_compactBins;  }

      /** Assignment operator */
      BinnedMaterial& operator=(const BinnedMaterial& amc) {
          if (this != &amc){
	    X0          = amc.X0;
	    L0          = amc.L0;
	    A           = amc.A;
	    Z           = amc.Z;
	    rho         = amc.rho;  
	    dEdX        = amc.dEdX;  
	    zOaTr       = amc.zOaTr;  
	    m_matBins   = amc.m_matBins ? amc.m_matBins->clone() : 0; 
	    m_compactBins  = amc.m_compactBins ? amc.m_compactBins->clone() : 0; 
          }
          return (*this);
      }

      /** access to content in global mode (transport of neutral particle) */
      Trk::MatBinMap  binMap(const Amg::Vector3D& position,const Amg::Vector3D& direction, float min=-1.e05,float max=1.e05) const;

      /** access to layer bin utility */
      const Trk::BinUtility*  layerBinUtility(const Amg::Vector3D& position) const;
      const Trk::BinUtility*  layerBinUtility() const;

      /** layer bin */
      size_t  layerBin(const Amg::Vector3D& position) const;

      /** access to material/id per bin */
      const IdentifiedMaterial* material(const Amg::Vector3D& position ) const;
      const IdentifiedMaterial* materialNext(const Amg::Vector3D& pos,const Amg::Vector3D& dir, bool layOnly ) const;
      const IdentifiedMaterial* layerMaterial(const Amg::Vector3D& position ) const;
      const IdentifiedMaterial* layerMaterial(size_t bin ) const;

      
  
  private:
      const CompactBinnedArray< const IdentifiedMaterial >*  m_matBins;  
      const CompactBinnedArray< const CompactBinnedArray<const IdentifiedMaterial > >* m_compactBins;
  };


}

inline const Trk::BinUtility* Trk::BinnedMaterial::layerBinUtility(const Amg::Vector3D& position) const
 {
   if (m_matBins) return m_matBins->layerBinUtility(position);
   if (m_compactBins) return m_compactBins->layerBinUtility(position);
   return 0;
 }

inline const Trk::BinUtility* Trk::BinnedMaterial::layerBinUtility() const
 {
   if (m_matBins) return m_matBins->binUtility();
   if (m_compactBins) return m_compactBins->binUtility();
   return 0;
 }

inline size_t Trk::BinnedMaterial::layerBin(const Amg::Vector3D& position) const
 {
   if (m_matBins) return m_matBins->layerBin(position);
   if (m_compactBins) return m_compactBins->layerBin(position);
   return 0;
 }
  
#endif
