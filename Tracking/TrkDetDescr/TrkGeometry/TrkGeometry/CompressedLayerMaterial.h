/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CompressedLayerMaterial.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_COMPRESSEDLAYERMATERIAL_H
#define TRKGEOMETRY_COMPRESSEDLAYERMATERIAL_H

//Trk
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkDetDescrUtils/BinUtility.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"
//Gaudi
#include "GaudiKernel/MsgStream.h"

namespace Trk {


  /** 
   @class CompressedLayerMaterial

   It extends the LayerMaterialProperties base class

   @author Andreas.Salzburger@cern.ch 
   */

  class CompressedLayerMaterial : public LayerMaterialProperties {
    
    public:
      /** Default Constructor - needed by POOL*/    
      CompressedLayerMaterial();
  
      /** Default Constructor for emptly material */    
      CompressedLayerMaterial(BinUtility& binutility);

      /**Explizit constructor with only full MaterialProperties, 
         and split factors:
          - 1. : oppositePre
          - 0. : alongPre

        ===> 1 Dimensional array, 1 dimensional indices ( can represent 2-dim indices)

        ATTENTION: Ownership of MaterialProperties objects is given!
       */
      CompressedLayerMaterial(const Trk::BinUtility& binutility,
                              const MaterialPropertiesVector& fullProperties,
                              const std::vector< unsigned short int >& materialIndices,
                              double splitFactor=0.);

      CompressedLayerMaterial(std::unique_ptr<Trk::BinUtility> binutility,
                              const MaterialPropertiesVector&& fullProperties,
                              const std::vector< unsigned short int >& materialIndices,
                              double splitFactor=0.);

      /**Copy Constructor */  
      CompressedLayerMaterial(const CompressedLayerMaterial& mprop);
      
      /**Destructor*/
      virtual ~CompressedLayerMaterial();
      
      /**Pseudo-Constructor clone()*/ 
      CompressedLayerMaterial* clone() const override;
      
      /** Assignment operator */
      CompressedLayerMaterial& operator=(const CompressedLayerMaterial& lmp);

      /** Scale operator */
      virtual CompressedLayerMaterial& operator*=(double scale) override;

      /** Return the BinUtility */
      const BinUtility* binUtility() const override;
      
      /** Update the BinUtility if necessary - passing ownership of the utility class*/
      virtual void updateBinning(BinUtility* bu) const override;
       
      /**Return method for full material description of the Layer - for all bins*/
      const MaterialPropertiesVector& fullMaterial() const;
      
      /**Return method for index vector, needs BinUtility to interpret this*/
      const std::vector<unsigned short int>& materialBins() const;
 
      /**Return method for full material description of the Layer */
      const MaterialProperties* fullMaterial(const Amg::Vector3D& gp) const override;
      
       /** Access the single bin */
      const MaterialProperties* material(size_t bin0, size_t bin1) const override;
            
      /** Output Method for MsgStream, to be overloaded by child classes */
      MsgStream& dump(MsgStream& sl) const override;
      /** Output Method for std::ostream, to be overloaded by child classes */
      std::ostream& dump(std::ostream& sl) const override;      

    private:
      mutable BinUtility*                             m_binUtility; //!< the helper for the bin finding 

      /** The five different MaterialProperties */
      MaterialPropertiesVector                        m_fullMaterial;
      std::vector <unsigned short int>                m_materialBins;

      /** helper method - to clear the material*/
      void clearMaterial();
                                            
      /** helper method - to refill the material  */
      void fillMaterial(const MaterialPropertiesVector& matVector);   

  };


  inline const BinUtility* CompressedLayerMaterial::binUtility() const
  { return m_binUtility; }
  
  inline const MaterialPropertiesVector& CompressedLayerMaterial::fullMaterial() const
  { return m_fullMaterial; }
  
  inline const std::vector< unsigned short int >& CompressedLayerMaterial::materialBins() const
  { return m_materialBins; }
  
  inline const MaterialProperties* CompressedLayerMaterial::material(size_t bin0, size_t bin1) const 
  {
      int accessBin = bin1*(m_binUtility->max(0)+1)+bin0;
      // safety check
      if (bin0 <= (unsigned int)m_binUtility->max(0) && bin1 <= (unsigned int)m_binUtility->max(1) 
         && m_fullMaterial.size() && accessBin < int(m_materialBins.size()))
          return m_fullMaterial[m_materialBins[accessBin]];
     return 0;
  }
  
  inline void CompressedLayerMaterial::updateBinning(BinUtility* bu) const {
      if (bu){
          delete m_binUtility;
          m_binUtility = bu;
      }
  }

}

#endif
