/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRK_TRACKPARAMETERSIDHELPER_H
#define TRK_TRACKPARAMETERSIDHELPER_H

#include "TrkParametersIdentificationHelpers/BitField.h"

#include "CaloGeoHelpers/CaloSampling.h"
#include "AtlasDetDescr/AtlasDetTechnology.h"

typedef unsigned int TrackParametersIdentifier;

namespace Trk {

  /** helper class to encode and decode a TrackParametersIdentifier */
  class TrackParametersIdHelper {
  public:

    /** Encodes a TrackParametersIdentifier from AtlasDetDescr::AtlasDetTechnology and CaloSampling::CaloSample 
        and a bool indicating whether this is an entry or exit layer.
        Checks consistency of technology and calo sample and will create an invalid identifier is any of the inputs
        is inconsistent        
     */
    TrackParametersIdentifier          encode( AtlasDetDescr::AtlasDetTechnology, CaloSampling::CaloSample,
                                               bool isEntryToVolume ) const;

    /** create invalid identifier */
    TrackParametersIdentifier          encodeInValidIdentifier() const;

    /** returns true if the id is valid */
    bool                               isValid( TrackParametersIdentifier id ) const;

    /** returns true if the id belongs to the volume entrance */
    bool                               isEntryToVolume( TrackParametersIdentifier id ) const;

    /** AtlasDetTechnology encoded in id, returns AtlasDetDescr::fUndefined if id is not valid */
    AtlasDetDescr::AtlasDetTechnology  technology( TrackParametersIdentifier id ) const;

    /** CaloSample encoded in id, returns CaloSampling::Unknown if id is not valid  */
    CaloSampling::CaloSample           caloSample( TrackParametersIdentifier id ) const;

  private:
    /** set version bits */
    void setVersion( TrackParametersIdentifier& id ) const;

    static unsigned int           m_version;
    static BitField<unsigned int> m_validField;
    static BitField<unsigned int> m_versionField;
    static BitField<unsigned int> m_technologyField;

    static BitField<unsigned int> m_isEntryToLayerField;
    static BitField<unsigned int> m_caloSamplingField;

  };

 inline TrackParametersIdentifier TrackParametersIdHelper::encode( AtlasDetDescr::AtlasDetTechnology tech, 
                                                                   CaloSampling::CaloSample sample,
                                                                   bool isEntryToVolume ) const {
    TrackParametersIdentifier id = 0;
    // check whether techology matches calo
    if( tech < AtlasDetDescr::fFirstAtlasCaloTechnology || tech > AtlasDetDescr::fLastAtlasCaloTechnology ){
      m_validField.encode(1,id);
    }else{
      m_technologyField.encode(tech,id);
      unsigned int ientry = isEntryToVolume ? 1 : 0;
      m_isEntryToLayerField.encode(ientry,id);
      m_caloSamplingField.encode(sample,id);
      setVersion(id);
    }
    return id;
  }

  inline bool TrackParametersIdHelper::isValid( TrackParametersIdentifier id ) const {
    return !m_validField.decode(id);
  }

  inline bool TrackParametersIdHelper::isEntryToVolume( TrackParametersIdentifier id ) const {
    return m_isEntryToLayerField.decode(id);
  }

  inline AtlasDetDescr::AtlasDetTechnology TrackParametersIdHelper::technology( TrackParametersIdentifier id ) const {
    // check if identifier is valid
    if( !isValid(id) ) return AtlasDetDescr::fUndefined;
    return static_cast<AtlasDetDescr::AtlasDetTechnology>(m_technologyField.decode(id));
  }
  
  inline CaloSampling::CaloSample TrackParametersIdHelper::caloSample( TrackParametersIdentifier id ) const {
    // check if identifier is valid
    if( !isValid(id) ) return CaloSampling::Unknown;
    return static_cast<CaloSampling::CaloSample>(m_caloSamplingField.decode(id));
  }

  inline void TrackParametersIdHelper::setVersion( TrackParametersIdentifier& id ) const {
     m_versionField.encode(m_version,id);
  }

  inline TrackParametersIdentifier TrackParametersIdHelper::encodeInValidIdentifier() const {
    TrackParametersIdentifier id(0);
    m_validField.encode(1,id);
    return id;
  }

}

#endif
