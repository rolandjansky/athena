/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRK_CALOEXTENSION_H
#define TRK_CALOEXTENSION_H

#include "TrkParameters/TrackParameters.h"
#include <vector>

namespace Trk {
  
  /** 
      Tracking class to hold the extrapolation from a particle from the ID to the muon system (or the other way around)
      Both the caloEntryLayerIntersection and the muonEntryLayerIntersection can return NULL if the particle 
      failed to reach the layer
   */
  class CaloExtension {
  public:
    /** constructor taking result of the intersection as arguments.
        Both calo and muon Entry layer intersections are allowed to be NULL.
        The class takes ownership of all pointers.
     */
    CaloExtension(const TrackParameters* caloEntry,
                  const TrackParameters* muonEntry,
                  std::vector<CurvilinearParameters>&& caloLayers);

    /** destructor */
    ~CaloExtension();

    /** access to intersection with the calorimeter entry layer 
        return NULL if the intersection failed
     */
    const TrackParameters* caloEntryLayerIntersection() const;

    /** access to intersection with the muon entry layer 
        return NULL if the intersection failed
     */
    const TrackParameters* muonEntryLayerIntersection() const;
    
    /** access to the intersections with the calorimeter layers.
        The intersections are stored as curvilinear parameters, 
        the layers are identified by their cIdentifier() */
    const std::vector<CurvilinearParameters>& caloLayerIntersections() const;

  private:
    /** no copying nor assignment allow, use shared pointers instead */
    CaloExtension& operator=(const CaloExtension&) = delete;
    CaloExtension(const CaloExtension&) = delete;
    
    /** no default constructor */
    CaloExtension() = delete;
    
    /** data members */
    /// parameters at the calorimeter entrance
    const TrackParameters*                      m_caloEntryLayerIntersection; 
     /// parameters at the muon entrance
    const TrackParameters*                      m_muonEntryLayerIntersection; 
    /// parameters at the different calorimeter layers
    std::vector<CurvilinearParameters>   m_caloLayerIntersections;
  };

  inline const TrackParameters*
  CaloExtension::caloEntryLayerIntersection() const
  {
    return m_caloEntryLayerIntersection;
  }

  inline const TrackParameters*
  CaloExtension::muonEntryLayerIntersection() const
  {
    return m_muonEntryLayerIntersection;
  }

  inline const std::vector<CurvilinearParameters>&
  CaloExtension::caloLayerIntersections() const
  {
    return m_caloLayerIntersections;
  }

  }

#endif
