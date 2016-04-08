/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CompoundLayerMaterial.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_COMPOUNDLAYERMATERIAL_H
#define TRKGEOMETRY_COMPOUNDLAYERMATERIAL_H

//Trk
#include "TrkGeometry/ElementTable.h"
#include "TrkGeometry/Material.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrUtils/SharedObject.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"
//Gaudi
#include "GaudiKernel/MsgStream.h"

class CompoundLayerMaterialCnv_p1;

namespace Trk {

    typedef std::vector<unsigned char> ValueVector;
    typedef std::vector< ValueVector > ValueMatrix;

    struct ValueStore {
        float        valueMin;
        float        valueStep;
        ValueMatrix  valueBinMatrix;
        
        double value(unsigned char ibin0, unsigned char ibin1) {
            // allows for single entry
            if (!valueBinMatrix.size()) return valueMin;
            // get the entry from the matrix
            unsigned int ibin = static_cast<unsigned int>(valueBinMatrix[static_cast<unsigned int>(ibin1)][static_cast<unsigned int>(ibin0)]);
            if (!ibin) return 0.;
            double value = valueMin + (double(ibin)-0.5)*valueStep;
            return value;
        }
    };
      
    /** 
     @class CompoundLayerMaterial
    
     It extends the LayerMaterialProperties base class,
     and has a intrinsic accuracy of about 0.5 % on the dynamic range from using unsigned char
    
     @author Andreas.Salzburger@cern.ch 
     */
    
    class CompoundLayerMaterial : public LayerMaterialProperties {
      
      public:
        /** Default Constructor - needed by POOL*/    
        CompoundLayerMaterial();
    
        /**Explizit constructor */
        CompoundLayerMaterial(const Trk::BinUtility& binutility,
                              const ValueStore&  thicknessBins,
                              const ValueStore&  x0Bins,
                              const ValueStore&  l0Bins,
                              const ValueStore&  aBins,
                              const ValueStore&  zBins,
                              const ValueStore&  rhoBins,
                              const std::vector< std::vector< MaterialComposition > >& composition,
                              bool fComposition = false);
    
        /**Copy Constructor */  
        CompoundLayerMaterial(const CompoundLayerMaterial& mprop);
        
        /**Destructor*/
        virtual ~CompoundLayerMaterial();
        
        /**Pseudo-Constructor clone()*/ 
        virtual CompoundLayerMaterial* clone() const override;
        
        /** Assignment operator */
        CompoundLayerMaterial& operator=(const CompoundLayerMaterial& lmp);
    
        /** Scale operator */
        virtual CompoundLayerMaterial& operator*=(double scale) override;
    
        /** Return the BinUtility */
        const BinUtility* binUtility() const override;
        
        /** Update the BinUtility if necessary - passing ownership of the utility class*/
        virtual void updateBinning(BinUtility* bu) const override;
        
        /**Return method for full material description of the Layer */
        virtual const MaterialProperties* fullMaterial(const Amg::Vector3D& gp) const override;
        
        /** Access the single bin */
        const MaterialProperties* material(size_t bin0, size_t bin1) const override;
        
        /** Update the ElementTable */
        void updateElementTable(const SharedObject<const ElementTable>& set) const;
              
        /** Get the ElementTable */
        const ElementTable* elementTable() const;
              
        /** Output Method for MsgStream, to be overloaded by child classes */
        MsgStream& dump(MsgStream& sl) const override;
        
        /** Output Method for std::ostream, to be overloaded by child classes */
        std::ostream& dump(std::ostream& sl) const override;      
    
      private:
        friend class ::CompoundLayerMaterialCnv_p1;

        mutable MaterialProperties*                                 m_materialProperties; //!< the ones you return
          
        mutable BinUtility*                                         m_binUtility;         //!< the helper for the bin finding 
        mutable ValueStore                                          m_thicknessBins;      //!< thickness parameter
        mutable ValueStore                                          m_x0Bins;             //!< x0 parameter
        mutable ValueStore                                          m_l0Bins;             //!< l0 parameter
        mutable ValueStore                                          m_aBins;              //!< A parameters (averaged)
        mutable ValueStore                                          m_zBins;              //!< Z parameter (averaged)
        mutable ValueStore                                          m_rhoBins;            //!< rho parameter (averaged)
        mutable std::vector< std::vector< MaterialComposition > >   m_composition;        //!< composition matrix
        bool                                                        m_fullComposition;    //!< full composition calculation
        mutable SharedObject<const ElementTable>                    m_elementTable;       // the ElementTable (filled/synchronised)
    
    
    
    };
    
    
    inline const BinUtility* CompoundLayerMaterial::binUtility() const
    { return m_binUtility; }
    
    inline void CompoundLayerMaterial::updateElementTable(const SharedObject<const ElementTable>& set) const
    { m_elementTable = set; }
  
    inline void CompoundLayerMaterial::updateBinning(BinUtility* bu) const
    {  
        if (bu){
            delete m_binUtility;
            m_binUtility = bu;
        }
    }
    
    inline const ElementTable* CompoundLayerMaterial::elementTable() const 
    { return m_elementTable.getPtr(); }
  
}

#endif
