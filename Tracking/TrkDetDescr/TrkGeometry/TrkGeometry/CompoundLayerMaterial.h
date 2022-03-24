/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CompoundLayerMaterial.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_COMPOUNDLAYERMATERIAL_H
#define TRKGEOMETRY_COMPOUNDLAYERMATERIAL_H

// Trk
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkGeometry/ElementTable.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/Material.h"
#include "TrkGeometry/MaterialProperties.h"
#include "CxxUtils/CachedUniquePtr.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"

class CompoundLayerMaterialCnv_p1;

namespace Trk {

typedef std::vector<unsigned char> ValueVector;
typedef std::vector<ValueVector> ValueMatrix;

struct ValueStore {
  float valueMin;
  float valueStep;
  ValueMatrix valueBinMatrix;

  double value(unsigned char ibin0, unsigned char ibin1) const {
    // allows for single entry
    if (valueBinMatrix.empty()) return valueMin;
    // get the entry from the matrix
    unsigned int ibin = static_cast<unsigned int>(
        valueBinMatrix[static_cast<unsigned int>(ibin1)]
                      [static_cast<unsigned int>(ibin0)]);
    if (!ibin) return 0.;
    double value = valueMin + (double(ibin) - 0.5) * valueStep;
    return value;
  }
};

/**
 @class CompoundLayerMaterial

 It extends the LayerMaterialProperties base class,
 and has a intrinsic accuracy of about 0.5 % on the dynamic range from using
 unsigned char

 @author Andreas.Salzburger@cern.ch
 */

class CompoundLayerMaterial final: public LayerMaterialProperties {
 public:
  /** Default Constructor - needed by POOL*/
  CompoundLayerMaterial();

  /**Explizit constructor */
  CompoundLayerMaterial(
      const Trk::BinUtility& binutility, const ValueStore& thicknessBins,
      const ValueStore& x0Bins, const ValueStore& l0Bins,
      const ValueStore& aBins, const ValueStore& zBins,
      const ValueStore& rhoBins,
      const std::vector<std::vector<MaterialComposition> >& composition,
      bool fComposition = false);

  /**Copy Constructor */
  CompoundLayerMaterial(const CompoundLayerMaterial& mprop);

  /**Destructor*/
  virtual ~CompoundLayerMaterial() override;

  /**Pseudo-Constructor clone()*/
  virtual CompoundLayerMaterial* clone() const override final;

  /** Assignment operator */
  CompoundLayerMaterial& operator=(const CompoundLayerMaterial& lmp);

  /** Scale operator */
  virtual CompoundLayerMaterial& operator*=(double scale) override final;

  /** Return the BinUtility */
  virtual const BinUtility* binUtility() const override final;

  /** Update the BinUtility if necessary - passing ownership of the utility
   * class*/
  virtual void updateBinning(BinUtility* bu) override final;

  /**Return method for full material description of the Layer */
  virtual const MaterialProperties* fullMaterial(
      const Amg::Vector3D& gp) const override final;

  /** Access the single bin */
  virtual const MaterialProperties* material(size_t bin0,
                                             size_t bin1) const override final;

  /** Update the ElementTable */
  void updateElementTable(const SharedObject<const ElementTable>& set);

  /** Get the ElementTable */
  const ElementTable* elementTable() const;

  /** Output Method for MsgStream, to be overloaded by child classes */
  virtual MsgStream& dump(MsgStream& sl) const override final;

  /** Output Method for std::ostream, to be overloaded by child classes */
  virtual std::ostream& dump(std::ostream& sl) const override final;

 private:
  friend class ::CompoundLayerMaterialCnv_p1;

  void resizeMaterialProperties();

  using MaterialPropertiesCUP = CxxUtils::CachedUniquePtr<MaterialProperties>;
  std::vector<std::vector<MaterialPropertiesCUP> >
    m_materialProperties;  //!< the ones you return

  BinUtility* m_binUtility;    //!< the helper for the bin finding
  ValueStore m_thicknessBins;  //!< thickness parameter
  ValueStore m_x0Bins;         //!< x0 parameter
  ValueStore m_l0Bins;         //!< l0 parameter
  ValueStore m_aBins;          //!< A parameters (averaged)
  ValueStore m_zBins;          //!< Z parameter (averaged)
  ValueStore m_rhoBins;        //!< rho parameter (averaged)
  std::vector<std::vector<MaterialComposition> >
      m_composition;       //!< composition matrix
  bool m_fullComposition;  //!< full composition calculation
  SharedObject<const ElementTable>
      m_elementTable;  // the ElementTable (filled/synchronised)
};

inline const BinUtility* CompoundLayerMaterial::binUtility() const {
  return m_binUtility;
}

inline void CompoundLayerMaterial::updateElementTable(
    const SharedObject<const ElementTable>& set) {
  m_elementTable = set;
}

inline void CompoundLayerMaterial::updateBinning(BinUtility* bu) {
  if (bu) {
    delete m_binUtility;
    m_binUtility = bu;
  }
}

inline const ElementTable* CompoundLayerMaterial::elementTable() const {
  return m_elementTable.get();
}

}  // namespace Trk

#endif
