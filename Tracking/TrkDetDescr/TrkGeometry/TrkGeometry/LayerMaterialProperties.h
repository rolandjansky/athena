/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LayerMaterialProperties.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETY_LAYERMATERIALPROPERTIES_H
#define TRKGEOMETY_LAYERMATERIALPROPERTIES_H

//Amg
#include "GeoPrimitives/GeoPrimitives.h"
//Trk
#include "TrkGeometry/MaterialProperties.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkEventPrimitives/PropDirection.h"
//Gaudi
#include "GaudiKernel/MsgStream.h"
//STD
#include <vector>

class BinnedLayerMaterialCnv_p1;

namespace Trk {

  class BinUtility;
  class ElementTable;
    
 /**
   @enum MaterialConcentration

   Simple enum to identify when a material update on a non-structured layer should be done,
   options are alongPre and oppositePre.

  */
  
  enum MaterialConcentration { alongPre  = 1, split =  0, oppositePre = -1 };

  /** 
   @class LayerMaterialProperties

   This virtual base class encapsulates the logics to build pre/post/full update material 
   for Layer structures, it hosts 5 different MaterialProperties in the following order:
   
    - fullUpdate          (crossing full Layer in both directions)
    - alongPreUpdate      (passing material to sensitive part in direction of the normal vector)
    - alongPostUpdate     (passing material after sensitive part in direction of the normal vector)
    - oppositePreUpdate   (passing material to sensitive part in opposite direction to the normal vector)
    - poopsitePostUpdate  (passing material after sensitive part in opposite direction to the normal vector)

   for obvious reasons: alongPre == oppositePost && oppositePre == alongPost
      
   @author Andreas.Salzburger@cern.ch 
   */

  class LayerMaterialProperties {
    
    public:
      /**Constructor*/
      LayerMaterialProperties() :
       m_splitFactor(1.)
     {}

      /**Constructor*/
      LayerMaterialProperties(double splitFactor) :
       m_splitFactor(splitFactor)
      {}

      /**Destructor*/
      virtual ~LayerMaterialProperties(){}
      
      /**Pseudo-Constructor clone()*/ 
      virtual LayerMaterialProperties* clone() const = 0;
      
      /** Scale operator */
      virtual LayerMaterialProperties& operator*=(double scale) = 0;
      
      /**Return method for full material description of the Layer */
      virtual const MaterialProperties* fullMaterial(const Amg::Vector3D& gp) const = 0;
      
      /**Direct access via bins to the MaterialProperties */
      virtual const MaterialProperties* material(size_t ib0, size_t ib1) const = 0;
      
      /** Update the ElementTable */
      void updateElementTable(const SharedObject<const ElementTable>&) const { return; }
      
      /** Get the ElementTable */
      const ElementTable* elementTable() const { return 0; }
            
      /** Update pre factor */
      double factor(PropDirection pDir, MaterialUpdateStage mStage) const;

      /**Return method for pre update 
      material description of the Layer along normalvector */
      double alongPreFactor() const;
     
      /**Return method for post update
      material description of the Layer along normalvector */
      double alongPostFactor() const;
     
      /**Return method for pre update 
      material description of the Layer along normalvector */
      double  oppositePreFactor() const;
     
      /**Return method for post update
      material description of the Layer along normalvector */
      double oppositePostFactor() const;

      /** Return the BinUtility */
      virtual const BinUtility* binUtility() const = 0;
            
      /** Update the BinUtility if necessary - passing ownership of the utility class*/
      virtual void updateBinning(BinUtility* bu) const = 0;

      /** Output Method for MsgStream, to be overloaded by child classes */
      virtual MsgStream& dump(MsgStream& sl) const = 0;
      
      /** Output Method for std::ostream, to be overloaded by child classes */
      virtual std::ostream& dump(std::ostream& sl) const = 0;
                                            
    protected :
      friend class ::BinnedLayerMaterialCnv_p1;
      double               m_splitFactor;     //!< the split factor in favour of oppositePre

  };

/** inline return methods for the pre/post factors */  
inline double LayerMaterialProperties::factor(PropDirection pDir, MaterialUpdateStage mStage ) const 
{ 
    if (mStage == Trk::fullUpdate) return 1.;
    return ( pDir*mStage > 0 ? m_splitFactor : 1.-m_splitFactor ); 
}   

/** inline return methods for the pre/post factors */ 
inline double LayerMaterialProperties::alongPreFactor() const { return (1.-m_splitFactor); }

inline double LayerMaterialProperties::alongPostFactor() const { return m_splitFactor; }

inline double LayerMaterialProperties::oppositePreFactor() const { return m_splitFactor; }

inline double LayerMaterialProperties::oppositePostFactor() const { return (1.-m_splitFactor); }

//**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 

MsgStream& operator<<( MsgStream& sl, const LayerMaterialProperties& mprop);

std::ostream& operator<<( std::ostream& sl, const LayerMaterialProperties& mprop);
    
} // end of namespace

#endif // TRKGEOMETY_LAYERMATERIALPROPERTIES_H

