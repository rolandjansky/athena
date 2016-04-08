/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BinnedLayerMaterial.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_BINNEDLAYERMATERIAL_H
#define TRKGEOMETRY_BINNEDLAYERMATERIAL_H

//Trk
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/MaterialProperties.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkDetDescrUtils/BinUtility.h"
//Gaudi
#include "GaudiKernel/MsgStream.h"

class BinnedLayerMaterialCnv_p1;

namespace Trk {


  /** 
   @class BinnedLayerMaterial

   It extends the LayerMaterialProperties base class

   @author Andreas.Salzburger@cern.ch 
   */

  class BinnedLayerMaterial : public LayerMaterialProperties {
    
    public:
      /** Default Constructor - needed by POOL*/    
      BinnedLayerMaterial();
  
      /** Default Constructor for emptly material */    
      BinnedLayerMaterial(BinUtility& binutility);

      /**Explizit constructor with only full MaterialProperties, 
         and split factors:
          - 1. : oppositePre
          - 0. : alongPre

        ===> 1 Dimensional array

        ATTENTION: Ownership of MaterialProperties objects is given!
       */
      BinnedLayerMaterial(const Trk::BinUtility& binutility,
                          const MaterialPropertiesVector& fullProperties,
                          double splitFactor=0.);

      /**Explizit constructor with only full MaterialProperties, 
         and split factors:
          - 1. : oppositePre
          - 0. : alongPre

        ===> 2 Dimensional array

        ATTENTION: Ownership of MaterialProperties objects is given!
       */
      BinnedLayerMaterial(const Trk::BinUtility& binutility,
                          const MaterialPropertiesMatrix& fullProperties,
                          double splitFactor=0.);

      /**Copy Constructor */  
      BinnedLayerMaterial(const BinnedLayerMaterial& mprop);
      
      /**Destructor*/
      virtual ~BinnedLayerMaterial();
      
      /**Pseudo-Constructor clone()*/ 
      BinnedLayerMaterial* clone() const override;
      
      /** Assignment operator */
      BinnedLayerMaterial& operator=(const BinnedLayerMaterial& lmp);

      /** Scale operator */
      virtual BinnedLayerMaterial& operator*=(double scale) override;

      /** Return the BinUtility */
      const BinUtility* binUtility() const override;
       
      /** Update the BinUtility if necessary - passing ownership of the utility class*/
      virtual void updateBinning(BinUtility* bu) const override;
       
      /**Return method for full material description of the Layer - for all bins*/
      const MaterialPropertiesMatrix& fullMaterial() const;
 
      /**Return method for full material description of the Layer */
      virtual const MaterialProperties* fullMaterial(const Amg::Vector3D& gp) const override;
            
      /** Access the single bin */
     const MaterialProperties* material(size_t bin0, size_t bin1 ) const override;
            
      /** Output Method for MsgStream, to be overloaded by child classes */
      MsgStream& dump(MsgStream& sl) const override;
      
      /** Output Method for std::ostream, to be overloaded by child classes */
      std::ostream& dump(std::ostream& sl) const override;      

    private:
      friend class ::BinnedLayerMaterialCnv_p1;

      mutable BinUtility*       m_binUtility; //!< the helper for the bin finding
 
      /** The five different MaterialProperties */
      MaterialPropertiesMatrix m_fullMaterial;

      /** helper method - to clear the material*/
      void clearMaterial();
                                            
      /** helper method - to refill the material  */
      void fillMaterial(const MaterialPropertiesMatrix& matMatrix);

  };


inline const BinUtility* BinnedLayerMaterial::binUtility() const
  { return m_binUtility; }
  
  inline const MaterialPropertiesMatrix& BinnedLayerMaterial::fullMaterial() const
  { return m_fullMaterial; }
  
  inline const MaterialProperties* BinnedLayerMaterial::material(size_t bin0, size_t bin1 ) const 
  {
     return m_fullMaterial[bin1][bin0];
  }
  
  inline void BinnedLayerMaterial::updateBinning(BinUtility* bu) const {
      if (bu){
          delete m_binUtility;
          m_binUtility = bu;
      }
  }


}

#endif
