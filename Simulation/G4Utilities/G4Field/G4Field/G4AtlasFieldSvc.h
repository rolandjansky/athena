/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AtlasFieldSvc_H
#define G4AtlasFieldSvc_H

#include "FadsField/MagneticFieldMap.h"
#include "GaudiKernel/ServiceHandle.h"


// forward declarations
namespace MagField {
  class IMagFieldSvc;
}

class G4AtlasFieldSvc : public FADS::MagneticFieldMap {
  public:

    G4AtlasFieldSvc();
    G4AtlasFieldSvc(const std::string& n);
    ~G4AtlasFieldSvc() { }
    
    void FieldValue(const double *xyzPos, double *xyzField) const;
    void Initialize();
    
  private:
    /** status flag for field initialization status */
    static bool s_fieldinitialized;
    
    /** the MagneticField service */
    ServiceHandle<MagField::IMagFieldSvc>   m_magFieldSvc;       //!< athena service handle
    MagField::IMagFieldSvc                 *m_magFieldSvcQuick;  //!< fast access, removing gaudi overhead
};

#endif
