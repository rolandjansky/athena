/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Csc Readout Element properties
 -----------------------------------------
 Copyright (C) 2004 by ATLAS Collaboration
***************************************************************************/

//<doc><file>	$Id: CscReadoutElement.h,v 1.3 2009-03-03 00:27:38 dwright Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef MUONGEOMODEL_CSCREADOUTELEMENT_H
# define MUONGEOMODEL_CSCREADOUTELEMENT_H

#include "MuonReadoutGeometry/MuonClusterReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonIdHelpers/CscIdHelper.h"

//<<<<<< PUBLIC DEFINES >>>>>>
#define maxwlay   4

namespace Trk{
  class TrapezoidBounds;
  class RotatedTrapezoidBounds;
}

namespace Muon {
  class CscAlignModule;
  class CombinedMuonAlignModule;
}

class CscInternalAlignmentPar;

namespace MuonGM {

  /*
    A CscReadoutElement corresponds to a single CSC chamber; therefore typically 
    (in the final MS layout, i.e. if no detector stagein is considered) a CSC 
    station contains 2 CscReadoutElements. 
    CscReadoutElements are identified by StationName, StationEta, StationPhi, 
    Technology=1 and ChamberLayer. Therefore the granularity of the data
    collections is not equal to the granularity of the geometry description 
    (1 collection -> 2 CscReadoutElement [for final layout]; ->1 CscReadoutElement 
    in the initial layout). 
   
    Pointers to all CscReadoutElements are created in the build() method of the 
    MuonChamber class, and are held in arrays by the MuonDetectorManager, which 
    is responsible for storing, deleting and provide access to these objects. 

    A CscReadoutElement holds properties related to its internal structure 
    (i.e. number of strip panels) and general geometrical properties (size); 
    it implements tracking interfaces and provides access to typical 
    readout-geometry information: i.e. number of strips, strip  positions, etc.

    The globalToLocalCoords and globalToLocalTransform methods (+ their opposite) 
    define the link between the ATLAS global reference frame and the internal 
    (geo-model defined) local reference frame of any gas gap volume (which is the 
    frame where local coordinates of SimHits, in output from G4, are expressed).
  */

  class MuonDetectorManager;

    
  class CscReadoutElement: public MuonClusterReadoutElement 
  {
    friend class Muon::CscAlignModule;
    friend class Muon::CombinedMuonAlignModule;
    friend class MuonChamber;

  public:
    
    CscReadoutElement(GeoVFullPhysVol* pv, std::string stName,
		      int zi, int fi, bool is_mirrored, MuonDetectorManager* mgr);
    
    virtual ~CscReadoutElement();

    /** distance to readout. 
	If the local position is outside the active volume, the function first shift the position back into the active volume */
    inline double distanceToReadout( const Amg::Vector2D& pos, const Identifier& id ) const;

    /** strip number corresponding to local position. 
	If the local position is outside the active volume, the function first shift the position back into the active volume */
    inline int stripNumber( const Amg::Vector2D& pos, const Identifier& id ) const;

    /** strip position 
	If the strip number is outside the range of valid strips, the function will return false */
    inline bool stripPosition( const Identifier& id, Amg::Vector2D& pos ) const;

    /** returns the hash function to be used to look up the center and the normal of the tracking surface for a given identifier */
    inline int  layerHash(const Identifier& id)   const; 

    /** returns the hash function to be used to look up the surface and surface transform for a given identifier */
    inline int  surfaceHash(const Identifier& id) const;  
  
    /** returns the hash function to be used to look up the surface boundary for a given identifier */
    inline int  boundaryHash(const Identifier& id) const;  
  
    /** returns whether the given identifier measures phi or not */
    inline bool measuresPhi(const Identifier& id) const; 

    /** number of layers in phi/eta projection */
    inline int numberOfLayers( bool ) const;

    /** number of strips per layer */
    inline int numberOfStrips( const Identifier& layerId )   const;
    inline int numberOfStrips( int layer, bool ) const;

    /** space point position for a given pair of phi and eta identifiers 
	The LocalPosition is expressed in the reference frame of the phi projection.
	If one of the identifiers is outside the valid range, the function will return false */
    inline bool spacePointPosition( const Identifier& phiId, const Identifier& etaId, Amg::Vector2D& pos ) const;

    /** Global space point position for a given pair of phi and eta identifiers 
	If one of the identifiers is outside the valid range, the function will return false */
    inline bool spacePointPosition( const Identifier& phiId, const Identifier& etaId, Amg::Vector3D& pos ) const;

    /** space point position for a pair of phi and eta local positions and a layer identifier 
	The LocalPosition is expressed in the reference frame of the phi projection.
    */
    inline void spacePointPosition( const Amg::Vector2D& phiPos, const Amg::Vector2D& etaPos, Amg::Vector2D& pos ) const;

    // Id set/get methods
    inline int ChamberLayer() const;
    inline void setChamberLayer(int cl);

    inline int Ngasgaps() const;
    inline int NphiStripLayers() const;
    inline int NetaStripLayers() const;
    inline int NphiStrips(int gasgaplayer) const;
    inline int NetaStrips(int gasgaplayer) const;
    // in the following methods the chlayer argument is irrelevant
    // the first 3 will disappear 
    double StripWidth(int chlayer, int measphi) const;
    double StripPitch(int chlayer, int measphi) const;
    double cathodeReadoutPitch(int chLayer, int measuresPhi) const;
    double StripWidth(int measphi) const;
    double StripPitch(int measphi) const;
    double cathodeReadoutPitch(int measuresPhi) const;

    inline double shortWidth() const;
    inline double length() const;
    inline double longWidth() const;
    int maxNumberOfStrips(int measuresPhi) const;
    int maxNumberOfStrips(int measuresPhi, double width) const;
    double activeWidth(int measuresPhi) const;
    double z0() const;
    inline double roxacellWidth() const;
    inline double lengthUpToMaxWidth() const;
    inline double excent() const;
    
    const Amg::Vector3D stripLayerPos(Identifier id) const;
    const Amg::Vector3D stripLayerPos(IdentifierHash id) const;
    const Amg::Vector3D stripLayerPos(int chamberLayer, int wireLayer,
						 int measPhi, int channel) const;
    const Amg::Vector3D localStripLayerPos(Identifier id) const;
    const Amg::Vector3D localStripLayerPos(IdentifierHash id) const;
    const Amg::Vector3D localStripLayerPos(int chamberLayer, int wireLayer,
						      int measPhi, int channel) const;
    //
    const Amg::Vector3D localWireLayerPos(Identifier id) const; 
    const Amg::Vector3D localWireLayerPos(int gg) const; 
    const Amg::Vector3D wireLayerPos(Identifier id) const; 
    const Amg::Vector3D wireLayerPos(int gg) const; 
    
    //
    const Amg::Vector3D nominalStripPos(Identifier id) const; /**< ignores internal alignment parameters, hence gives generally incorrect answer */
    const Amg::Vector3D nominalStripPos(int eta, int chamberLayer, int wireLayer,
						   int measPhi, int channel) const; /**< ignores internal alignment parameters, hence gives generally incorrect answer */
    const Amg::Vector3D stripPos(Identifier id) const;       /**< takes into account internal alignment parameters, hence gives accurate answer */
    const Amg::Vector3D stripPos(int eta, int chamberLayer, int wireLayer,
					    int measPhi, int channel) const;  /**< takes into account internal alignment parameters, hence gives accurate answer */
    const Amg::Vector3D nominalGlobalPos(Amg::Vector3D localP) const; /**<  ignores internal alignment parameters, hence gives generally incorrect answer */
    const Amg::Vector3D globalPos(Amg::Vector3D localP) const;        /**<  station-level method: does not depend on the strip view/layer, hence it cannot account for internal alignment parameters */
    const Amg::Vector3D localPos(Amg::Vector3D globalP) const;        /**<  station-level method: does not depend on the strip view/layer, hence it cannot account for internal alignment parameters  */
    const Amg::Vector3D nominalLocalStripPos(Identifier id) const; /**< ignores internal alignment parameters, hence gives generally incorrect answer (local here is the station frame, coherent with the gas gas frames) */
    //const Amg::Vector3D nominalLocalStripPos(IdentifierHash id) const;/**< ignores internal alignment parameters, hence gives generally incorrect answer (local here is the station frame, coherent with the gas gas frames) */
    const Amg::Vector3D nominalLocalStripPos(int eta, int chamberLayer, int wireLayer,
							int measPhi, int channel) const;/**< ignores internal alignment parameters, hence gives generally incorrect answer (local here is the station frame, coherent with the gas gas frames) */
    const Amg::Vector3D nominalLocalClusterPos(int eta, int wireLayer, 
							  int measPhi, double x0) const;/**< ignores internal alignment parameters, hence gives generally incorrect answer (local here is the station frame, coherent with the gas gas frames) */
    const Amg::Vector3D localStripPos(Identifier id) const;/**< takes into account internal alignment parameters, hence gives generally accurate answer (local here is the station frame, coherent with the gas gas frames) - digitization should use this method to emulate internal alignment parameters */
    const Amg::Vector3D localStripPos(int eta, int chamberLayer, int wireLayer,
						 int measPhi, int channel) const;/**< takes into account internal alignment parameters, hence gives generally accurate answer (local here is the station frame, coherent with the gas gas frames) - digitization should use this method to emulate internal alignment parameters */
    const Amg::Vector3D localClusterPos(int eta, int wireLayer, 
						   int measPhi, double x0) const;/**< takes into account internal alignment parameters, hence gives generally  answer (local here is the station frame, coherent with the gas gas frames) */
    const Amg::Vector3D stripPosOnTrackingSurface(Identifier id) const;  /**< nominal strip pos in the tracking local frame of the measurement surface*/
    const Amg::Vector3D stripPosOnTrackingSurface(int eta, int chamberLayer, int wireLayer,
							     int measPhi, int channel) const; /**< nominal strip pos in the tracking local frame of the measurement surface*/
    const Amg::Transform3D nominalTransform(const Identifier& id) const; /**< like tracking Transform but nominal - returns a transform not a reference to it */ 
    const Amg::Transform3D nominalTransform(int gasGap, int measPhi) const; /**< like tracking Transform but nominal - returns a transform not a reference to it */
    const Amg::Vector3D nominalCenter(int gasGap) const;/**< like tracking center but nominal - returns a Amg::Vector3D not a reference to it */
    const Amg::Vector3D originForInternalALines(int gasGap) const;/**< like tracking center but nominal - returns a Amg::Vector3D not a reference to it */

    

    double stripLength(int chamberLayer, int measuresPhi, 
		       int stripNumber, double& epsilon) const;
    double stripLength(const Identifier& id) const;
    double lengthCorrection (int measuresPhi,double stripPos) const;

    inline double anodeCathodeDistance() const;
    
    /*!
      localToGlobalCoords and Transf connect the Gas Gap Frame (defined as a Sensitive Detector) to the Global Frame; 
      Notice that internal CSC alignment affects the strip plane orientation and position in the CSC station frame, but 
      does not impact on the gas gap geometry within the station; therefore the following methods do not depen on internal alignment parameters. 
      Otherwise stated, the internal alignment parameters in MC can be emulated at the digitization stage. 
      @param[out] localToGlobalCoords the coordinates in the Global Atlas Frame of point at x in the local frame
      @param[in]  id CSC gas gap identifier - view and strip field must be 0 
      @param[in]  x  local coordinates of the point in the gas gap = sensitive volume
    */
    //local to global and viceversa
    const Amg::Vector3D localToGlobalCoords(Amg::Vector3D x, Identifier id) const; //**< localToGlobalCoords and Transf relates gas-gap frame (SensitiveDetectors) to the Global Frame  */
    const Amg::Transform3D localToGlobalTransf(Identifier id) const; //**< localToGlobalCoords and Transf relates gas-gap frame (SensitiveDetectors) to the Global Frame  */
    const Amg::Transform3D localToGlobalTransf(int gasGap) const;    //**< localToGlobalCoords and Transf relates gas-gap frame (SensitiveDetectors) to the Global Frame  */
    // global to local
    const Amg::Vector3D globalToLocalCoords(Amg::Vector3D x, Identifier id) const; //**< localToGlobalCoords and Transf relates gas-gap frame (SensitiveDetectors) to the Global Frame  */
    const Amg::Transform3D globalToLocalTransf(Identifier id) const;           //**< localToGlobalCoords and Transf relates gas-gap frame (SensitiveDetectors) to the Global Frame  */
    
    // modifiers
    void  setIdentifier(Identifier id);
    void set_ngasgaps            (int );
    void set_nstriplayers        (int );
    void set_nwirelayers         (int );
    void set_nstripsperlayer     (int );
    void set_nwiresperlayer      (int );
    void set_stripwidth          (double );
    void set_strippitch          (double );
    void set_wirepitch           (double );
    void set_first_strip_localcoo(double );
    void set_first_wire_localcoo (double );
    void setCscInternalAlignmentPar(CscInternalAlignmentPar* );
    void setCscInternalAlignmentParams();
    //    // internal alignment
    //    double m_cscIntTransl[4][3]; // first index is wireLayer, second = 0,1,2 for    s,z,t
    //    double m_cscIntRot[4][3];    // first index is wireLayer, second = 0,1,2 for rots,z,t    
    const CscInternalAlignmentPar getCscInternalAlignmentPar(int gasGap) const;
    double getGasGapIntAlign_s(int gasGap) const;
    double getGasGapIntAlign_z(int gasGap) const;
    double getGasGapIntAlign_t(int gasGap) const;
    double getGasGapIntAlign_rots(int gasGap) const;
    double getGasGapIntAlign_rotz(int gasGap) const;
    double getGasGapIntAlign_rott(int gasGap) const;
  

    virtual bool containsId(Identifier id) const;
    
    // compute sin(stereo angle) at a given position:
    double sinStereo(const Identifier & stripId) const;

    void fillCache() const;    
    void    refreshCache() const {clearCache(); fillCache();}
    double  xCoordinateInTrackingFrame( const Identifier& id ) const;

  private:
    void doTests();
      
  private:

    double m_excent;
    double m_roxacellwidth;
    double m_RlengthUpToMaxWidth;
    double m_anodecathode_distance;
    int m_chamberlayer;
    int m_ngasgaps;
    int m_nstriplayers;  
    int m_nwirelayers;  
    int m_nPhistripsperlayer;
    int m_nEtastripsperlayer;
    int m_nwiresperlayer;
    double m_Phistripwidth;
    double m_Phistrippitch;
    double m_Etastripwidth;
    double m_Etastrippitch;
    double m_wirepitch;
    double m_first_strip_localcoo;
    double m_first_wire_localcoo;
    double m_wireplanez[maxwlay];

    // internal alignment
    double m_cscIntTransl[4][3]; // first index is wireLayer, second = 0,1,2 for    s,z,t
    double m_cscIntRot[4][3];    // first index is wireLayer, second = 0,1,2 for rots,z,t    
  };

  int CscReadoutElement::ChamberLayer() const 
  {return m_chamberlayer;}

  void CscReadoutElement::setChamberLayer(int cl) 
  {m_chamberlayer = cl;}

  double CscReadoutElement::shortWidth() const 
  {return getSsize();}

  double CscReadoutElement::length() const 
  {return getRsize();}

  double CscReadoutElement::longWidth() const 
  {return getLongSsize();}

  double CscReadoutElement::roxacellWidth() const 
  {return m_roxacellwidth;}

  double CscReadoutElement::lengthUpToMaxWidth() const 
  {return m_RlengthUpToMaxWidth;}

  double CscReadoutElement::excent() const 
  {return m_excent;}

  double CscReadoutElement::anodeCathodeDistance() const 
  {return m_anodecathode_distance;}

  int CscReadoutElement::Ngasgaps() const 
  {return m_ngasgaps;}

  int CscReadoutElement::NphiStripLayers() const 
  {return m_ngasgaps;}

  int CscReadoutElement::NetaStripLayers() const 
  {return m_ngasgaps;}

  int CscReadoutElement::NphiStrips(int) const 
  {return m_nPhistripsperlayer;}

  int CscReadoutElement::NetaStrips(int) const 
  {return m_nEtastripsperlayer;}
  
  int  CscReadoutElement::layerHash(const Identifier& id)   const { return manager()->cscIdHelper()->wireLayer(id)-1; }
  
  int  CscReadoutElement::surfaceHash(const Identifier& id) const { return 2*(manager()->cscIdHelper()->wireLayer(id)-1)+ (manager()->cscIdHelper()->measuresPhi(id)?0:1); }
  
  int  CscReadoutElement::boundaryHash(const Identifier& id) const { 
    return ( measuresPhi(id)? 0:1 ); 
  }
  
  bool CscReadoutElement::measuresPhi(const Identifier& id) const { return manager()->cscIdHelper()->measuresPhi(id); } 

  double CscReadoutElement::distanceToReadout( const Amg::Vector2D& , const Identifier& ) const {
    reLog() << MSG::WARNING << " distanceToReadout::dummy routine " << endmsg;
    return 0.;
  }

  int CscReadoutElement::stripNumber( const Amg::Vector2D& , const Identifier&  ) const { 
    reLog() << MSG::WARNING << " stripNumber::dummy routine " << endmsg;
    return 1;
  }

  bool CscReadoutElement::stripPosition( const Identifier& id, Amg::Vector2D& pos ) const {
    /** please don't copy the inefficient code below!! Look at the RpcReadoutElement for a proper implementation */
    Amg::Vector3D gpos = stripPos(id);  
    if( !surface(id).globalToLocal(gpos,gpos,pos) ){
      reLog() << MSG::WARNING << " stripPosition:: globalToLocal failed " << surface(id).transform().inverse()*gpos << std::endl;
      return false;
    }
    return true;
  }

  int CscReadoutElement::numberOfLayers( bool ) const { return Ngasgaps(); }
  int CscReadoutElement::numberOfStrips( const Identifier&  )   const { return NphiStrips(1);  }
  int CscReadoutElement::numberOfStrips( int , bool ) const { return NphiStrips(1);  }

  inline bool CscReadoutElement::spacePointPosition( const Identifier& phiId, const Identifier& etaId, Amg::Vector2D& pos ) const {
    Amg::Vector2D phiPos;
    Amg::Vector2D etaPos;
    if( !stripPosition(phiId,phiPos) || !stripPosition(etaId,etaPos) ) return false;
    spacePointPosition(phiPos,etaPos,pos);
    return true;
  }

  inline bool CscReadoutElement::spacePointPosition( const Identifier& phiId, const Identifier& etaId, Amg::Vector3D& pos ) const {
    Amg::Vector2D lpos;
    spacePointPosition(phiId,etaId,lpos);
    surface(phiId).localToGlobal(lpos,pos,pos);
    return true;
  }

  inline void CscReadoutElement::spacePointPosition( const Amg::Vector2D& phiPos, const Amg::Vector2D& etaPos, Amg::Vector2D& pos ) const {
    pos[0] = phiPos.x();
    pos[1] = etaPos.x();
  }

} // namespace MuonGM


#endif // MUONGEOMODEL_CSCREADOUTELEMENT_H
