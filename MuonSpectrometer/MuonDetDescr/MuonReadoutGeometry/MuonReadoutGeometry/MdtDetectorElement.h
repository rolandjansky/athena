/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Common Mdt Detector Element properties
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: MdtDetectorElement.h 
//<version>	$Name: not supported by cvs2svn $

//!  A MdtDetectorElement Class
/*!
  MdtDetectorElements are as granular as offline mdt data collections 
  (prepRawData); hence they can be retrieved using the collection hash 
  identifier.  MdtDetectorElement will hold an array of MdtReadoutElements 
  which will actually implement the real functionality; MdtDetectorElement 
  methods will delegate the job to the appropriate MdtReadoutElement.  
*/

#ifndef MDTGEOMODEL_MDTDETECTORELEMENT_H
# define MDTGEOMODEL_MDTDETECTORELEMENT_H

#include <string>
#include "MuonReadoutGeometry/MuonDetectorElement.h"

#define maxMdtREinDE 2


namespace MuonGM {

class MuonDetectorManager;
class MdtReadoutElement;    
class MuonStation;

/**
   Base class for the XxxDetectorelement, with Xxx = Mdt, Rpc, Tgc, Csc. 
   It's a Trk::TrkDetElementBase, therefore it must implement the generic 
   tracking interfaces requested to the geometry: center, normal, 
   surfaceBound, transform.
   No link to raw geometry. 
*/

class MdtDetectorElement: public MuonDetectorElement 
{

public:    

   MdtDetectorElement(GeoVFullPhysVol* pv, MuonDetectorManager* mgr, Identifier id, IdentifierHash idHash);
   virtual ~MdtDetectorElement(){};
    
    //Declaring the Message method for further use
    MsgStream& msg( MSG::Level lvl ) const ;
    //Declaring the Method providing Verbosity Level
    bool msgLevel( MSG::Level lvl ) ;

   virtual int getStationEta() const {return 0;}; //!< returns stationEta 
   virtual int getStationPhi() const {return 0;}; //!< returns stationPhi

   //  DetectorElement content
   double getRsize() const;//<! size of the DetectorElement (collection of readout elements)
   double getZsize() const;//<! size of the DetectorElement (collection of readout elements)
   double getLongSsize() const;//<! size of the DetectorElement (collection of readout elements)
   double getLongRsize() const;//<! size of the DetectorElement (collection of readout elements)
   double getLongZsize() const;//<! size of the DetectorElement (collection of readout elements)
    
   // Common tracking generic interfaces
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

   unsigned int nMDTinStation() const {return nReadoutElements();} 
   unsigned int nCSCinStation() const {return 0;}
   unsigned int nTGCinStation() const {return 0;}
   unsigned int nRPCinStation() const {return 0;}

   // Add a XxxReadoutElement to the Collection
   void addMdtReadoutElement (const MdtReadoutElement* x, Identifier  id);
   //!< store the MdtReadoutElement using as "key" the identifier

   void addMdtReadoutElement (const MdtReadoutElement* x, int multilayer);
   //!< store the MdtReadoutElement using as "key" the multilayer

   // access to Readout Elements
   const MdtReadoutElement* getMdtReadoutElement(Identifier) const;
   //!< access via extended identifier (requires unpacking)

   const MdtReadoutElement* getMdtReadoutElement(int multilayer) const;
   //!< access via multilayer index   
    
protected:    

private:
   const MdtReadoutElement *m_mdtRE[maxMdtREinDE];
   unsigned int m_nRE;

   mutable std::vector<const Trk::Surface*> m_detectorSurfaces;

};

} // namespace MuonGM

#endif // MDTGEOMODEL_MDTDETECTORELEMENT_H
