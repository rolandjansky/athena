/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Collect TGC readout elements - granularity is same as for EDM (hash ids)
 -------------------------------------------------------
 ***************************************************************************/

#ifndef MUONGEOMODEL_TGCDETECTORELEMENT_H
#define MUONGEOMODEL_TGCDETECTORELEMENT_H

#include "Identifier/IdentifierHash.h"
#include "Identifier/Identifier.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorElement.h"

class TgcIdHelper;

namespace MuonGM {
    
class MuonStation;


    
class TgcDetectorElement : public MuonDetectorElement
{

public:

  
   TgcDetectorElement(GeoVFullPhysVol* pv, MuonDetectorManager* mgr, Identifier id, IdentifierHash idhash);
   
   virtual int getStationEta() const {return 0;}; //!< returns stationEta 
   virtual int getStationPhi() const {return 0;}; //!< returns stationPhi

   
   unsigned int nMDTinStation() const {return 0;} 
   unsigned int nCSCinStation() const {return 0;}
   unsigned int nTGCinStation() const {return 1;}
   unsigned int nRPCinStation() const {return 0;}


    const Amg::Transform3D& transform() const;

    const Trk::Surface& surface() const;
  
    const Trk::SurfaceBounds& bounds() const;

    const Amg::Vector3D& center() const;
  
    const Amg::Vector3D& normal() const;
  
    const Amg::Vector3D& normal(const Identifier& id) const;
  
    const Trk::Surface& surface(const Identifier& id) const;
  
    const Trk::SurfaceBounds& bounds(const Identifier& id) const;
  
    const Amg::Transform3D& transform(const Identifier& id) const;
  
    const Amg::Vector3D& center(const Identifier& id) const;

    const std::vector<const Trk::Surface*>& surfaces() const;

   // access to the readout-elements in this DetectorElement
   const TgcReadoutElement* readoutElement() const {return m_tgcre;}
  
   void setReadoutElement(const TgcReadoutElement *re) {m_tgcre=re;}
   // access to the MuonStation this DetectorElement belongs to
   MuonStation* parentMuonStation() const;

   unsigned int NreadoutElements() const {return 1;}

private:
   const TgcReadoutElement *m_tgcre;


};

} // namespace MuonGM

#endif // MUONGEOMODEL_TGCDETECTORELEMENT_H
