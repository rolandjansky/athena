/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// LayerMaterialRecord.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_LAYERMATERIALRECORD_H
#define TRKGEOMETRY_LAYERMATERIALRECORD_H

#include <climits>

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrUtils/MaterialAssociationType.h"
#include "TrkGeometry/MaterialProperties.h"

#ifndef UCHARCONV
#define UCHARCONV
#define ucharbin 0.00392157  // 1./double(1.*UCHAR_MAX)
// int to unsigned char and vv
#define uchar2uint(uchar) static_cast<unsigned int>(uchar)
#define uint2uchar(unint) static_cast<unsigned char>(unint)
// double to unsigned char and vv
#define uchar2dfrac(uchar) double(uchar * ucharbin)
#define dfrac2uchar(dfrac) lrint(dfrac* UCHAR_MAX)
#endif

namespace Trk {

/** @class LayerMaterialRecord

 Helper Class to record the material during the GeantinoNtupleMappingProcess

 @author Andreas.Salzburger@cern.ch
 */

class Layer;
class AssociatedMaterial;

class LayerMaterialRecord {
 public:
  /** Default Constructor */
  LayerMaterialRecord();

  /** Constructor */
  LayerMaterialRecord(double tlayer, const BinUtility* binutils,
                      double minFraction,
                      MaterialAssociationType assoc = EffectiveNumAtoms);

  /** Constructor */
  LayerMaterialRecord(const LayerMaterialRecord& lmr);

  /** Destructor */
  ~LayerMaterialRecord();

  /** Assignment operator */
  LayerMaterialRecord& operator=(const LayerMaterialRecord& lmr);

  /** adding the information about the Geantino hit */
  void associateGeantinoHit(const Amg::Vector3D& pos, double s,
                            const Trk::Material& mat);

  /** adding the information about an empty hit scaling- particle crossed layer,
   * but no mapping information */
  void associateEmptyHit(const Amg::Vector3D& pos);

  /** finalize the Event */
  AssociatedMaterial* finalizeEvent(const Trk::Layer& lay,
                                    bool fullHit = false);

  /** finalize the Run */
  void finalizeRun(bool recordElements = true);

  /** return method for the LayerMaterial */
  const MaterialPropertiesMatrix& associatedLayerMaterial() const;

  /** return method for the events used for this */
  const std::vector<std::vector<unsigned int> >& binCounts() const;

  /** return the BinUtility */
  const Trk::BinUtility* binUtility() const;

 private:
  double m_layerThickness;   //!< record the layerThickness
  BinUtility* m_binUtility;  //!< record the BinnedArray
  int m_bins0;               //!< number of bins in coordinate 1
  int m_bins1;               //!< number of bins in coordinate 2
  double m_minFraction;      //!< minimum element fraction to be recorded

  //!< event normalizers
  int m_steps;
  Amg::Vector3D m_pos;

  //!< event related information
  bool m_emptyHitCase;
  double m_s;
  double m_s_in_x0;
  double m_s_in_l0;
  double m_a;
  double m_z;
  double m_rho;
  std::map<unsigned int, double> m_elements;
  MaterialAssociationType m_assoc;  //!< type of hit association

  //!< run related information - normalized per event
  std::vector<std::vector<unsigned int> > m_run_events;
  std::vector<std::vector<Amg::Vector3D> > m_run_pos;
  std::vector<std::vector<double> > m_run_s;
  std::vector<std::vector<double> > m_run_s_in_x0;
  std::vector<std::vector<double> > m_run_s_in_l0;
  std::vector<std::vector<double> > m_run_a;
  std::vector<std::vector<double> > m_run_z;
  std::vector<std::vector<double> > m_run_rho;

  std::vector<std::vector<std::map<unsigned int, double> > > m_run_elements;

  //!< the final material properties
  MaterialPropertiesMatrix m_associatedLayerMaterial;

  //!< clear the material -> calls delete
  void clearMaterial();
  //!< copy from another vector
  void copyMaterial(const MaterialPropertiesMatrix& mat);
};

inline const MaterialPropertiesMatrix&
LayerMaterialRecord::associatedLayerMaterial() const {
  return m_associatedLayerMaterial;
}

inline const std::vector<std::vector<unsigned int> >&
LayerMaterialRecord::binCounts() const {
  return m_run_events;
}

inline const Trk::BinUtility* LayerMaterialRecord::binUtility() const {
  return m_binUtility;
}

}  // namespace Trk

#endif  // TRKGEOMETRY_LAYERMATERIALRECORD_H

