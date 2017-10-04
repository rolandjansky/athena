/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Rpc Readout Element properties
 -----------------------------------------
***************************************************************************/

//<doc><file>	$Id: RpcReadoutElement.h,v 1.3 2009-03-03 00:27:38 dwright Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef MUONGEOMODEL_RPCREADOUTELEMENT_H
# define MUONGEOMODEL_RPCREADOUTELEMENT_H

#include "MuonReadoutGeometry/MuonClusterReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonStripDesign.h"
#include "MuonIdHelpers/RpcIdHelper.h"

//<<<<<< PUBLIC DEFINES >>>>>>
#define maxphipanels  2
#define maxetapanels  2

namespace Trk{
  class RectangleBounds;
  class PlaneSurface;
}

namespace Muon {
  class RpcAlignModule;
  class CombinedMuonAlignModule;
}

namespace MuonGM {
  /**
     An RpcReadoutElement corresponds to a single RPC module; therefore 
     typicaly a barrel muon station contains:
     - 4 RpcReadoutElements in the middle layer, two adjacent to each 
     other (in z) on the inner side (low-pt plane) and two on the 
     outer side (pivot plane);
     - 2 RpcReadoutElements in the outer layer (inner or outer side 
     depends on the Large or Small sector type). 
     RpcReadoutElements are identified by StationName, StationEta, StationPhi,
     Technology=2, doubletR, doubletZ and doubletPhi (the latter being 
     actually needed only in a few special cases). Therefore the granularity 
     of the data-collections is less fine-grained than the granularity of the 
     geometry description (1 collection -> 2 RpcReadoutElements, typically). 
   
     Pointers to all RpcReadoutElements are created in the build() method of
     the MuonChamber class, and are held in arrays by the MuonDetectorManager,
     which is responsible for storing, deleting and providing access to these 
     objects. 

     An RpcReadoutElement holds properties related to its internal structure
     (i.e. number of strip panels) and general geometrical properties (size); 
     it implements tracking interfaces and provide access to typical 
     readout-geometry information: i.e. number of strips, strip positions, etc.

     The globalToLocalCoords and globalToLocalTransform methods (+ their 
     opposite) define the link between the ATLAS global reference frame and 
     the internal (geo-model defined) local reference frame of any gas gap
     volume (which is the frame where local coordinates of SimHits, in output
     from G4, are expressed).
  */

  class MuonDetectorManager;
 
  class RpcReadoutElement: public MuonClusterReadoutElement 
  {
    friend class Muon::RpcAlignModule;
    friend class Muon::CombinedMuonAlignModule;
    friend class MuonChamber;

  public:
    /** constructor */
    RpcReadoutElement(GeoVFullPhysVol* pv, std::string stName,
		      int zi, int fi, bool is_mirrored,
		      MuonDetectorManager* mgr);
    
    /** destructor */
    ~RpcReadoutElement();                  
                      
    int getDoubletR() const; //!< return DoubletR value for the given readout element 
    int getDoubletZ() const; //!< return DoubletZ value for the given readout element 
    int getDoubletPhi() const; //!< return DoubletPhi value for the given readout element, be aware that one RE can contain two DoubletPhis!!!! 
    bool hasDEDontop() const; //!< return whether the RPC is 'up-side-down' 

    /** function to be used to check whether a given Identifier is contained in the readout element */
    bool containsId(Identifier id) const;

    /** returns whether the RE is in the ribs of the muon spectrometer */
    bool inTheRibs() const;
    
    int Nphigasgaps() const; //!< returns the number of phi gas gaps 
    int NphiStripPanels() const; //!< returns the number of phi strip panels (1 or 2) 
    int NetaStripPanels() const; //!< returns the number of eta strip panels (should always be 1) 
    int NphiStrips() const; //!< returns the number of phi strips 
    int NetaStrips() const; //!< returns the number of eta strips 
    int Nstrips(int measphi) const; //!< returns the number of strips for the phi or eta plane 
    double StripWidth(int measphi) const; //!< returns the strip width for the phi or eta plane 
    double StripLength(int measphi) const; //!< returns the strip length for the phi or eta plane 
    double StripPitch(int measphi) const; //!< returns the strip pitch for the phi or eta plane 
    double StripPanelDead(int measphi) const; //!< returns strip panel dead area for the phi or eta plane 
    double stripPanelSsize(int measphi) const; //!< returns strip panel S size for the phi or eta plane 
    double stripPanelZsize(int measphi) const; //!< returns strip panel Z size for the phi or eta plane 
    double gasGapSsize() const; //!< returns the gas gap S size 
    double gasGapZsize() const; //!< returns the gas gap Z size 

    /** distance to readout. 
	If the local position is outside the active volume, the function first shift the position back into the active volume */
    double distanceToReadout( const Amg::Vector2D& pos, const Identifier& id ) const;

    /** strip number corresponding to local position. 
	If the local position is outside the active volume, the function first shift the position back into the active volume */
    int stripNumber( const Amg::Vector2D& pos, const Identifier& id ) const;

    /** strip position 
	If the strip number is outside the range of valid strips, the function will return false */
    bool stripPosition( const Identifier& id, Amg::Vector2D& pos ) const;

    /** number of layers in phi/eta projection, same for eta/phi planes */
    int numberOfLayers( bool measPhi = true ) const;

    /** number of strips per layer */
    int numberOfStrips( const Identifier& layerId )   const;
    int numberOfStrips( int , bool measuresPhi ) const;

    /** space point position for a given pair of phi and eta identifiers 
	The LocalPosition is expressed in the reference frame of the phi surface.
	If one of the identifiers is outside the valid range, the function will return false */
    bool spacePointPosition( const Identifier& phiId, const Identifier& etaId, Amg::Vector2D& pos ) const;


    /** Global space point position for a given pair of phi and eta identifiers 
	If one of the identifiers is outside the valid range, the function will return false */
    bool spacePointPosition( const Identifier& phiId, const Identifier& etaId, Amg::Vector3D& pos ) const;

    /** space point position for a pair of phi and eta local positions and a layer identifier 
	The LocalPosition is expressed in the reference frame of the phi projection.
    */
    void spacePointPosition( const Amg::Vector2D& phiPos, const Amg::Vector2D& etaPos, Amg::Vector2D& pos ) const;

    /** @brief center of an RPC RE is not trivially the center of the first surface, overloading MuonClusterReadoutElement */
    const Amg::Vector3D  REcenter() const;

    /** @brief function to fill tracking cache */
    void         fillCache() const;
    void         refreshCache() const {clearCache(); fillCache();}

    /** @brief returns the hash to be used to look up the surface and transform in the MuonClusterReadoutElement tracking cache */
    int surfaceHash( const Identifier& id ) const;

    /** @brief returns the hash to be used to look up the surface and transform in the MuonClusterReadoutElement tracking cache */
    int surfaceHash( int dbPhi, int gasGap, int measPhi) const;

    /** @brief returns the hash to be used to look up the normal and center in the MuonClusterReadoutElement tracking cache */
    int layerHash( const Identifier& id ) const;
    /** @brief returns the hash to be used to look up the normal and center in the MuonClusterReadoutElement tracking cache */
    int layerHash( int dbPhi, int gasGap) const;
  
    /** returns the hash function to be used to look up the surface boundary for a given identifier */
    int  boundaryHash(const Identifier& id) const;  

    /** @brief returns whether the current identifier corresponds to a phi measurement */
    bool measuresPhi(const Identifier& id) const;

    /** @brief initialize the design classes for this readout element */
    void initDesign();

    /** set methods only to be used by MuonGeoModel */
    void setIdentifier(Identifier id);
    void setDoubletR(int ) ;
    void setDoubletZ(int ) ;
    void setDoubletPhi(int ) ;
    void set_nphigasgaps(int );
    void set_nphistrippanels(int );
    void set_netastrippanels(int );
    void set_nphistripsperpanel(int );
    void set_netastripsperpanel(int );
    void set_phistripwidth(double );
    void set_etastripwidth(double );
    void set_phistriplength(double );
    void set_etastriplength(double );
    void set_first_phistrip_s(double* );
    void set_first_etastrip_z(double* );
    
    
    /** local MuonGeoModel to global transforms and positions, only to be used by digitization */
    const Amg::Vector3D stripPanelPos(Identifier id) const;
    const Amg::Vector3D stripPanelPos(IdentifierHash id) const;
    const Amg::Vector3D stripPanelPos(int doubletR, int doubletZ,
						 int DoubletPhi, int gasGap, int measPhi) const;
    const Amg::Vector3D localStripPanelPos(Identifier id) const;
    const Amg::Vector3D localStripPanelPos(IdentifierHash id) const;
    const Amg::Vector3D localStripPanelPos(int doubletR, int doubletZ, int DoubletPhi,
						      int gasGap, int measPhi) const;
    // local to global
    const Amg::Vector3D SDtoModuleCoords(Amg::Vector3D x, Identifier id) const;
    const Amg::Vector3D localToGlobalCoords(Amg::Vector3D x, Identifier id) const;
    const Amg::Transform3D localToGlobalTransf(Identifier id) const;
    const Amg::Transform3D localToGlobalTransf(int dbZ, int dbPhi, int gasGap) const;
    // global to local
    const Amg::Vector3D globalToLocalCoords(Amg::Vector3D x, Identifier id) const;
    const Amg::Transform3D globalToLocalTransf(Identifier id) const;

    const Amg::Vector3D stripPos(Identifier id) const;
    const Amg::Vector3D stripPos(IdentifierHash id) const;
    const Amg::Vector3D stripPos(int doubletR, int doubletZ,
					    int DoubletPhi, int gasGap, int measPhi, int strip) const;
    const Amg::Vector3D localStripPos(Identifier id) const;
    const Amg::Vector3D localStripPos(IdentifierHash id) const;
    const Amg::Vector3D localStripPos(int doubletR, int doubletZ, int DoubletPhi,
						 int gasGap, int measPhi, int strip) const;
    double localStripSCoord(int doubletZ, int doubletPhi, int measphi, int strip) const;
    double localStripZCoord(int doubletZ, int doubletPhi, int measphi, int strip) const;
    double localGasGapDepth(int gasGap) const;
    const Amg::Vector3D localGasGapPos(Identifier id) const; 
    const Amg::Vector3D localGasGapPos(int dbZ, int dbP, int gg) const; 
    const Amg::Vector3D gasGapPos(Identifier id) const; 
    const Amg::Vector3D gasGapPos(int dbZ, int dbP, int gg) const; 
    bool rotatedRpcModule() const; 
    bool localTopGasGap(Identifier id) const;
    bool localTopGasGap(int gasGap) const;
    bool rotatedGasGap(Identifier id) const;
    bool rotatedGasGap(int gasGap) const;

    // Readout side infos
    // P is a point in the global reference frame
    // we want to have the distance from the side of the phi readout (length travelled along a phi strip) from a signal produced at P)
    double distanceToPhiReadout(const Amg::Vector3D& P) const;
    // P is a point in the global reference frame
    // we want to have the distance from the side of the eta readout (length travelled along a eta strip) from a signal produced at P)
    double distanceToEtaReadout(const Amg::Vector3D& P) const;
    // for backward compatibility of the interfaces keep the following 2 methods
    double distanceToPhiReadout(const Amg::Vector3D& x, const Identifier& id) const;
    double distanceToEtaReadout(const Amg::Vector3D& x, const Identifier& id) const;


  private:
    
    /** returns the MuonStripDesign class for the given identifier */
    const MuonStripDesign* getDesign( const Identifier& id ) const;

    int m_dbR, m_dbZ, m_dbPhi;
    bool m_hasDEDontop;

    int m_nphigasgaps;
    int m_netagasgaps;
    double m_gasgapssize;
    double m_gasgapzsize;

    int m_nphistrippanels;                
    int m_netastrippanels;                
    int m_nphistripsperpanel;             
    int m_netastripsperpanel;
    double m_phistripwidth;               
    double m_etastripwidth;               
    double m_phistrippitch;               
    double m_etastrippitch;               
    double m_phistriplength;              
    double m_etastriplength;
    double m_phipaneldead,  m_etapaneldead;
    double m_exthonthick;
    double m_first_phistrip_s[maxphipanels];
    double m_first_etastrip_z[maxetapanels];
    double m_etastrip_s[maxphipanels];
    double m_phistrip_z[maxetapanels];    
    Amg::Transform3D m_Xlg[2][2];

    const Amg::Transform3D localToGlobalStripPanelTransf(int dbZ, int dbPhi, int gasGap) const;
    const Amg::Vector3D localStripPanelPos(int dbZ, int dbP, int gg) const; 

    std::vector<MuonStripDesign> m_phiDesigns;
    std::vector<MuonStripDesign> m_etaDesigns;
    
  };
  
  inline int RpcReadoutElement::getDoubletR() const   {return m_dbR;}
  inline int RpcReadoutElement::getDoubletZ() const   {return m_dbZ;}
  inline int RpcReadoutElement::getDoubletPhi() const {return m_dbPhi;}
  inline void RpcReadoutElement::setDoubletR(int dbr)       {m_dbR = dbr;}
  inline void RpcReadoutElement::setDoubletZ(int dbz)       {m_dbZ = dbz;}
  inline void RpcReadoutElement::setDoubletPhi(int dbp)     {m_dbPhi = dbp;}
  inline bool RpcReadoutElement::hasDEDontop() const  {return m_hasDEDontop;}

  inline int RpcReadoutElement::Nphigasgaps() const      {return m_nphigasgaps;}
  inline int RpcReadoutElement::NphiStripPanels() const  {return m_nphistrippanels;}
  inline int RpcReadoutElement::NetaStripPanels() const  {return m_netastrippanels;}
  inline int RpcReadoutElement::NphiStrips() const       {return m_nphistripsperpanel;}
  inline int RpcReadoutElement::NetaStrips() const       {return m_netastripsperpanel;}

  inline int RpcReadoutElement::Nstrips(int measphi) const  { return measphi == 0 ? m_netastripsperpanel : m_nphistripsperpanel;  }
  inline double RpcReadoutElement::StripWidth(int measphi) const { return measphi == 1 ? m_phistripwidth :  m_etastripwidth;   }
  inline double RpcReadoutElement::StripLength(int measphi) const  { return measphi == 1 ?  m_phistriplength :  m_etastriplength;   }
  inline double RpcReadoutElement::StripPitch(int measphi) const   { return measphi == 1 ? m_phistrippitch : m_etastrippitch;   }
  inline double RpcReadoutElement::StripPanelDead(int measphi) const { return measphi == 1 ? m_phipaneldead : m_etapaneldead; }
  inline double RpcReadoutElement::stripPanelSsize(int measphi) const   {
    if (measphi == 1) return Nstrips(measphi)*StripPitch(measphi) - (StripPitch(measphi)-StripWidth(measphi));
    else return StripLength(measphi);
  }

  inline double RpcReadoutElement::stripPanelZsize(int measphi) const
  {
    if (measphi == 0)
      return Nstrips(measphi)*StripPitch(measphi) - (StripPitch(measphi)-StripWidth(measphi));
    else return StripLength(measphi);
  }

  inline double RpcReadoutElement::gasGapSsize() const  {return m_gasgapssize;}

  inline double RpcReadoutElement::gasGapZsize() const  {return m_gasgapzsize;}

  inline int RpcReadoutElement::surfaceHash( const Identifier& id ) const {
    return surfaceHash(manager()->rpcIdHelper()->doubletPhi(id),manager()->rpcIdHelper()->gasGap(id),manager()->rpcIdHelper()->measuresPhi(id));
  }

  inline int RpcReadoutElement::surfaceHash( int dbPhi, int gasGap, int measPhi) const {
    
    // if there is only one doublet phi we should always use one in the hash calculation
    if ( m_nphistrippanels == 1 ) dbPhi = 1;
    if( dbPhi > NphiStripPanels() || gasGap > numberOfLayers(true) ) {
      reLog() << MSG::WARNING << " surfaceHash: identifier out of range dbphi " << dbPhi << " max " << NphiStripPanels() 
	      << " ch dbphi " << getDoubletPhi() << " gp " << gasGap << " max " << numberOfLayers() << endmsg;
      return -1;
    }
    return  (dbPhi-1)*(2*NphiStripPanels()) + 2*(gasGap-1) + (measPhi ? 0 : 1);
  }

  inline const Amg::Vector3D  RpcReadoutElement::REcenter()    const { 
    if (NphiStripPanels() == 1) return MuonClusterReadoutElement::center(0);
    return 0.5*(MuonClusterReadoutElement::center(0)+MuonClusterReadoutElement::center(2));
  }

  inline int RpcReadoutElement::layerHash( const Identifier& id ) const {
    return layerHash(manager()->rpcIdHelper()->doubletPhi(id),manager()->rpcIdHelper()->gasGap(id));
  }

  inline int RpcReadoutElement::layerHash( int dbPhi, int gasGap) const {

    if ( m_nphistrippanels == 1 ) dbPhi = 1;

    if( dbPhi > NphiStripPanels() || gasGap > numberOfLayers(true) ) {
      reLog() << MSG::WARNING << " layerHash: identifier out of range dbphi " << dbPhi << " max " << NphiStripPanels() 
	      << " ch dbphi " << getDoubletPhi() << " gp " << gasGap << " max " << numberOfLayers() << endmsg;
      return -1;
    }
    return (dbPhi-1)*(NphiStripPanels()) + (gasGap-1);
  }

  inline int  RpcReadoutElement::boundaryHash(const Identifier& id) const { 
    return ( measuresPhi(id)? 0:1 ); 
  }

  inline bool RpcReadoutElement::measuresPhi( const Identifier& id ) const {
    return manager()->rpcIdHelper()->measuresPhi(id);
  }


  inline const MuonStripDesign* RpcReadoutElement::getDesign( const Identifier& id ) const {
    int phipanel = m_nphistrippanels == 1 ? 1 : manager()->rpcIdHelper()->doubletPhi(id);
    if( phipanel > (int)m_phiDesigns.size() ) {
      reLog() << MSG::WARNING << " bad identifier, no MuonStripDesign found " << endmsg;
      return 0;
    }
    return manager()->rpcIdHelper()->measuresPhi(id) ? &m_phiDesigns[phipanel-1] : &m_etaDesigns[phipanel-1];
  }
  inline double RpcReadoutElement::distanceToReadout( const Amg::Vector2D& pos, const Identifier& id ) const {
    const MuonStripDesign* design = getDesign(id);
    if( !design ) return 0;
    return design->distanceToReadout(pos);
  }

  inline int RpcReadoutElement::stripNumber( const Amg::Vector2D& pos, const Identifier& id ) const {
    const MuonStripDesign* design = getDesign(id);
    if( !design ) return 1;
    return design->stripNumber(pos);
  }

  inline bool RpcReadoutElement::stripPosition( const Identifier& id, Amg::Vector2D& pos ) const {
    const MuonStripDesign* design = getDesign(id);
    if( !design ) return 0;
    return design->stripPosition(manager()->rpcIdHelper()->strip(id),pos);
  }

  inline int RpcReadoutElement::numberOfLayers( bool ) const { return 2; }

  inline int RpcReadoutElement::numberOfStrips( const Identifier& layerId )   const {
    return numberOfStrips(1,manager()->rpcIdHelper()->measuresPhi(layerId));
  }
  inline int RpcReadoutElement::numberOfStrips( int , bool measuresPhi ) const {
    return Nstrips(measuresPhi); 
  }

  inline bool RpcReadoutElement::spacePointPosition( const Identifier& phiId, const Identifier& etaId, Amg::Vector2D& pos ) const {
    Amg::Vector2D phiPos;
    Amg::Vector2D etaPos;
    if( !stripPosition(phiId,phiPos) || !stripPosition(etaId,etaPos) ) return false;
    spacePointPosition(phiPos,etaPos,pos);
    return true;
  }

  inline bool RpcReadoutElement::spacePointPosition( const Identifier& phiId, const Identifier& etaId, Amg::Vector3D& pos ) const {
    Amg::Vector2D lpos;
    spacePointPosition(phiId,etaId,lpos);
    surface(phiId).localToGlobal(lpos,pos,pos);
    return true;
  }

  inline void RpcReadoutElement::spacePointPosition( const Amg::Vector2D& phiPos, const Amg::Vector2D& etaPos, Amg::Vector2D& pos ) const {
    pos[0] = phiPos.x();
    pos[1] = etaPos.x();
  }


} // namespace MuonGM

#endif // MUONGEOMODEL_RPCREADOUTELEMENT_H
