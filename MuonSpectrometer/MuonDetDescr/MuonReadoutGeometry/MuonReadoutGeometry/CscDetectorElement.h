/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Collect CSC readout elements - granularity is same as for EDM (hash ids)
 -------------------------------------------------------
 ***************************************************************************/

#ifndef MUONGEOMODEL_CSCDETECTORELEMENT_H
#define MUONGEOMODEL_CSCDETECTORELEMENT_H

#include "Identifier/IdentifierHash.h"
#include "Identifier/Identifier.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorElement.h"

class CscIdHelper;

namespace MuonGM {
    
class MuonStation;


    
class CscDetectorElement : public MuonDetectorElement
{

public:

  
   CscDetectorElement(GeoVFullPhysVol* pv, MuonDetectorManager* mgr, Identifier id, IdentifierHash idhash);
   
   virtual int getStationEta() const {return 0;}; //!< returns stationEta 
   virtual int getStationPhi() const {return 0;}; //!< returns stationPhi

   
   unsigned int nMDTinStation() const {return 0;} 
   unsigned int nCSCinStation() const {return 1;}
   unsigned int nTGCinStation() const {return 0;}
   unsigned int nRPCinStation() const {return 0;}


   // access to the readout-elements in this DetectorElement
   const CscReadoutElement* readoutElement() const {return m_cscre;}
   
   void setReadoutElement(const CscReadoutElement *re) {m_cscre=re; ++m_nREinDetectorElement;}
   // access to the MuonStation this DetectorElement belongs to
   MuonStation* parentMuonStation() const;

   unsigned int NreadoutElements() const {return 1;}

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

private:
   const CscReadoutElement *m_cscre;


};

} // namespace MuonGM

#endif // MUONGEOMODEL_CSCDETECTORELEMENT_H
