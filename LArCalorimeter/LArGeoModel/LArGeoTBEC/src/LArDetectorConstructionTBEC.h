/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArDetectorConstructionTBEC
// Return an envelope that contains the LAr End-Cap Test Bench.
// Dec-2005 V. Niess
// from CryostatConstructionH62003 and LArG4TBECCryostatConstruction.

#ifndef __LArDetectorConstructionTBEC_H__
#define __LArDetectorConstructionTBEC_H__

#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
class IRDBAccessSvc;
namespace LArGeo {

  class LArDetectorConstructionTBEC 
    {
    public:
    
    LArDetectorConstructionTBEC();
    virtual ~LArDetectorConstructionTBEC();
    
    // Get the envelope containing this detector.
    virtual GeoVPhysVol* GetEnvelope();
    

    // Set a vis limit for the EC
    void setECVisLimit(int /*limit*/) {/*_ecVisLimit=limit;*/}
      
  private:

    double              m_eta_pos;
    double              m_eta_cell;
    double              m_phi_pos;
    double              m_phi_cell;
    
    bool                m_hasLeadCompensator;
    bool		m_hasPresampler;
    double              m_ModuleRotation;
    double              m_YShift;              

    void                getSimulationParameters();

    GeoFullPhysVol*     createEnvelope();

    GeoPhysVol          *m_tbecEnvelopePhysical;

    IRDBAccessSvc       *m_pAccessSvc;
    
  };

} // namespace LArGeo

#endif // __LArDetectorConstructionTBEC_H__
