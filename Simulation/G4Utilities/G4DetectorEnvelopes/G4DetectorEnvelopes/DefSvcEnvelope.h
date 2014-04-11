/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DefSvcEnvelope_H
#define DefSvcEnvelope_H

// Framework includes
#include "AthenaBaseComps/AthMessaging.h"

// STL includes
#include <string>

// FADS
#include "FadsGeometry/DetectorFacility.h"

// forward declarations
class G4LogicalVolume;
class IEnvelopeDefSvc;

class DefSvcEnvelope: public FADS::DetectorFacility {

  public:
    DefSvcEnvelope(G4String);
    void SetEnvelopeSvcName(std::string name);
    void SetEnvelopeName( std::string name);
    void BuildGeometry();
    void SetMaterial(std::string &mat);

  private:
    /** Athena Messaging methods (to be used by AthMsgStreamMacros) */
    MsgStream                          &msg() const;
    MsgStream                          &msg(const MSG::Level lvl) const;
    bool                               msgLvl(const MSG::Level lvl) const;

    bool                               setupServices();

    MsgStream                         *m_msgStream;

    std::string                        m_envelopeSvcName;
    IEnvelopeDefSvc                   *m_envelopeSvc;

    std::string                        m_envelopeName;
    G4LogicalVolume                   *m_volume;
    std::string                        m_material;
};

#endif // DefSvcEnvelope_H

