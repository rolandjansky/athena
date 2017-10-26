/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 sTgc Readout Element properties
 -----------------------------------------
***************************************************************************/

//<doc><file>	$Id: sTgcReadoutElement.h,v 1.3 2009-03-03 00:27:38 dwright Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef MUONGEOMODEL_STGCREADOUTELEMENT_H
# define MUONGEOMODEL_STGCREADOUTELEMENT_H

#include "MuonReadoutGeometry/MuonClusterReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonChannelDesign.h"
#include "MuonReadoutGeometry/MuonPadDesign.h"

#include "MuonIdHelpers/sTgcIdHelper.h"


namespace Trk{
  class PlaneSurface;
}


namespace MuonGM {
  /**
     An sTgcReadoutElement corresponds to a single STGC module; therefore 
     typicaly a barrel muon station contains:
  */

  class MuonDetectorManager;
 
  class sTgcReadoutElement: public MuonClusterReadoutElement {
  public:
    /** constructor */
    
    sTgcReadoutElement(GeoVFullPhysVol* pv, std::string stName,
		       int zi, int fi, int mL, bool is_mirrored,
		       MuonDetectorManager* mgr);
    
    /** destructor */
    ~sTgcReadoutElement();                  

    /** function to be used to check whether a given Identifier is contained in the readout element */
    bool containsId(Identifier id) const;

    /** distance to readout. 
	If the local position is outside the active volume, the function first shift the position back into the active volume */
    double distanceToReadout( const Amg::Vector2D& pos, const Identifier& id ) const;

    /** strip number corresponding to local position. 
	Should be renamed to channelNumber : the only public access for all hit types */
    int stripNumber( const Amg::Vector2D& pos, const Identifier& id ) const;
     
    /** strip position - should be renamed to channel position
	If the strip number is outside the range of valid strips, the function will return false */
    bool stripPosition( const Identifier& id, Amg::Vector2D& pos ) const;

    /** pad number corresponding to local position */
    int padNumber( const Amg::Vector2D& pos, const Identifier& id) const;

    /** pad position */
    bool padPosition( const Identifier& id, Amg::Vector2D& pos) const;

    /** pad corners */
    bool padCorners ( const Identifier& id, std::vector<Amg::Vector2D> &corners) const;

    /** number of layers in phi/eta projection */
    int numberOfLayers( bool ) const;

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

    /** simHit local (SD) To Global position - to be used by MuonGeoAdaprors only      */
    Amg::Vector3D localToGlobalCoords(Amg::Vector3D locPos, Identifier id) const;

    /** @brief function to fill tracking cache */
    void         fillCache() const;
    void         refreshCache() const {clearCache(); fillCache();}

    /** @brief returns the hash to be used to look up the surface and transform in the MuonClusterReadoutElement tracking cache */
    int surfaceHash( const Identifier& id ) const;

    /** @brief returns the hash to be used to look up the surface and transform in the MuonClusterReadoutElement tracking cache */
    int surfaceHash( int gasGap, int channelType) const;

    /** @brief returns the hash to be used to look up the normal and center in the MuonClusterReadoutElement tracking cache */
    int layerHash( const Identifier& id ) const;

    /** @brief returns the hash to be used to look up the normal and center in the MuonClusterReadoutElement tracking cache */
    // int layerHash( int gasGap) const;        // does not fit in the scheme ( layer hash needs to follow surface hash )
  
    /** returns the hash function to be used to look up the surface boundary for a given identifier */
    int  boundaryHash(const Identifier& id) const;  

    /** @brief returns whether the current identifier corresponds to a phi measurement */
    bool measuresPhi(const Identifier& id) const;

    /** @brief initialize the design classes for this readout element */
    void initDesign(double largeX, double smallX, double lengthY, double stripPitch, double wirePitch,
                    double stripWidth, double wireWidth, double thickness);
 
    /** returns the MuonChannelDesign class for the given identifier */
    const MuonChannelDesign* getDesign( const Identifier& id ) const;

    /** returns the MuonChannelDesign class  */
    const MuonChannelDesign* getDesign( int gasGap, int channelType ) const;

    /** returns the MuonChannelDesign class for the given identifier */
    const MuonPadDesign* getPadDesign( const Identifier& id ) const;

    /** returns the MuonChannelDesign */
    const MuonPadDesign* getPadDesign( int gasGap ) const;

    /** set methods only to be used by MuonGeoModel */
    void setIdentifier(Identifier id);
    
    /** set methods only to be used by MuonGeoModel */
    void setChamberLayer(int ml) {m_ml=ml;}

    //double getSectorOpeningAngle(bool isLargeSector);

  private:

    std::vector<MuonChannelDesign> m_phiDesign;
    std::vector<MuonChannelDesign> m_etaDesign;
    std::vector<MuonPadDesign>     m_padDesign;

    std::vector<int> m_nStrips;
    std::vector<int> m_nWires;
    std::vector<int> m_nPads;
    int m_nlayers;
    
    int m_ml;

    int sTGC_type;

    //const double m_largeSectorOpeningAngle = 28.0;
    //const double m_smallSectorOpeningAngle = 17.0;

    // surface dimensions for strips
    std::vector<double> m_halfX;
    std::vector<double> m_minHalfY;
    std::vector<double> m_maxHalfY;
    // surface dimensions for pads and wires
    std::vector<double> m_PadhalfX;
    std::vector<double> m_PadminHalfY;
    std::vector<double> m_PadmaxHalfY;

    // transforms (RE->layer)
    Amg::Transform3D m_Xlg[4];
  };

  inline int sTgcReadoutElement::surfaceHash( const Identifier& id ) const {
    return surfaceHash(manager()->stgcIdHelper()->gasGap(id),manager()->stgcIdHelper()->channelType(id));
  }

  inline int sTgcReadoutElement::surfaceHash( int gasGap, int channelType) const {
    return (gasGap-1)*3+(2-channelType);  // assumes channelType=2 (wires), 1(strips), 0(pads)
  }

  inline int sTgcReadoutElement::layerHash( const Identifier& id ) const {
    //return layerHash(manager()->stgcIdHelper()->gasGap(id));
    return surfaceHash(id);                                      // don't have a choice here : rewrite MuonClusterReadoutElement first  
  }

  //inline int sTgcReadoutElement::layerHash( int gasGap ) const {
  //  return gasGap-1;
  //}

  inline int sTgcReadoutElement::boundaryHash( const Identifier& id ) const {
    int iphi = manager()->stgcIdHelper()->channelType(id)!=1 ? 1:0 ;      // wires and pads have locX oriented along phi
    if (abs(getStationEta())<3) iphi += 2*(manager()->stgcIdHelper()->gasGap(id)-1);
    return iphi; 
  }

  inline bool sTgcReadoutElement::measuresPhi( const Identifier& id ) const {
    return (manager()->stgcIdHelper()->channelType(id)!=1);
  }

  inline const MuonChannelDesign* sTgcReadoutElement::getDesign( const Identifier& id ) const {
    if (manager()->stgcIdHelper()->channelType(id)==1) return &(m_etaDesign[manager()->stgcIdHelper()->gasGap(id)-1]);
    if (manager()->stgcIdHelper()->channelType(id)==2) return &(m_phiDesign[manager()->stgcIdHelper()->gasGap(id)-1]);
    return 0;
  }

  inline const MuonPadDesign* sTgcReadoutElement::getPadDesign( const Identifier& id ) const {
    if (manager()->stgcIdHelper()->channelType(id)==0) return &(m_padDesign[manager()->stgcIdHelper()->gasGap(id)-1]);
    return 0;
  }

  inline const MuonChannelDesign* sTgcReadoutElement::getDesign( int gasGap, int channelType ) const {
    if (channelType==1) return &(m_etaDesign[gasGap-1]);
    if (channelType==2) return &(m_phiDesign[gasGap-1]);
    return 0;
  }

  inline const MuonPadDesign* sTgcReadoutElement::getPadDesign( int gasGap ) const {
    return &(m_padDesign[gasGap-1]);
  }

  inline double sTgcReadoutElement::distanceToReadout( const Amg::Vector2D& pos, const Identifier& id ) const {
    
    const MuonChannelDesign* design = getDesign(id);
    if( !design ) return -10000.;
    return design->distanceToReadout(pos);
 
  }

  inline int sTgcReadoutElement::stripNumber( const Amg::Vector2D& pos, const Identifier& id ) const {

    if (manager()->stgcIdHelper()->channelType(id)==0) return padNumber(pos,id);

    const MuonChannelDesign* design = getDesign(id);
    if( !design ) return -1;
    return design->channelNumber(pos);

  }

  inline int sTgcReadoutElement::padNumber( const Amg::Vector2D& pos, const Identifier& id) const {

    const MuonPadDesign* design = getPadDesign(id);
    if( !design ) {
      *m_MsgStream << MSG::WARNING << "no pad Design" << endmsg;
      return -1;
    }
    std::pair<int,int> pad(design->channelNumber(pos));
    *m_MsgStream << MSG::DEBUG << "pad numbers from MuonPadDesign " <<pad.first <<"  " << pad.second << "  "<<endmsg;

    if (pad.first>0 && pad.second>0) {

      Identifier padID=manager()->stgcIdHelper()->padID( manager()->stgcIdHelper()->stationName(id),
							 manager()->stgcIdHelper()->stationEta(id),
							 manager()->stgcIdHelper()->stationPhi(id),
							 manager()->stgcIdHelper()->multilayer(id),
							 manager()->stgcIdHelper()->gasGap(id),
							 0, pad.first, pad.second, true );     
      int channel = manager()->stgcIdHelper()->channel(padID);
      int padEta = manager()->stgcIdHelper()->padEta(padID);
      int padPhi = manager()->stgcIdHelper()->padPhi(padID);
      if( padEta != pad.first || padPhi != pad.second ){
	*m_MsgStream << MSG::WARNING << " bad pad indices: input " << pad.first << " " << pad.second << " from ID " << padEta << " " << padPhi << endmsg;
	return -1;
      }
      return channel;
    } 
    *m_MsgStream << MSG::WARNING << "bad channelNumber" << endmsg;

    return -1; 
  }

  inline bool sTgcReadoutElement::stripPosition( const Identifier& id, Amg::Vector2D& pos ) const {

    if (manager()->stgcIdHelper()->channelType(id)==0) return padPosition(id,pos);

    const MuonChannelDesign* design = getDesign(id);
    if( !design ) return 0;
    return design->channelPosition(manager()->stgcIdHelper()->channel(id),pos);

  }

  inline bool sTgcReadoutElement::padPosition( const Identifier& id, Amg::Vector2D& pos ) const {

    const MuonPadDesign* design = getPadDesign(id);
    if( !design ) return false;
    
    int padEta = manager()->stgcIdHelper()->padEta(id);
    int padPhi = manager()->stgcIdHelper()->padPhi(id);

    return design->channelPosition(std::pair<int,int>(padEta,padPhi),pos);

  }

  inline bool sTgcReadoutElement::padCorners( const Identifier& id, std::vector<Amg::Vector2D> &corners ) const {

    const MuonPadDesign* design = getPadDesign(id);
    if( !design ) return false;
    
    int padEta = manager()->stgcIdHelper()->padEta(id);
    int padPhi = manager()->stgcIdHelper()->padPhi(id);

    return design->channelCorners(std::pair<int,int>(padEta,padPhi),corners);

  }

  inline int sTgcReadoutElement::numberOfLayers( bool ) const { return m_nlayers; }

  inline int sTgcReadoutElement::numberOfStrips( const Identifier& layerId )   const {
    return numberOfStrips(manager()->stgcIdHelper()->gasGap(layerId)-1,manager()->stgcIdHelper()->channelType(layerId)==2);
  }

  inline int sTgcReadoutElement::numberOfStrips( int lay , bool measPhi ) const {
    if (lay>-1 && lay<m_nlayers) {
      return measPhi ? m_nWires[lay]: m_nStrips[lay]; 
    }
    return -1; 
  }

  inline bool sTgcReadoutElement::spacePointPosition( const Identifier& phiId, const Identifier& etaId, Amg::Vector2D& pos ) const {
    Amg::Vector2D phiPos;
    Amg::Vector2D etaPos;
    if( !stripPosition(phiId,phiPos) || !stripPosition(etaId,etaPos) ) return false;
    spacePointPosition(phiPos,etaPos,pos);
    return true;
  }

  inline bool sTgcReadoutElement::spacePointPosition( const Identifier& phiId, const Identifier& etaId, Amg::Vector3D& pos ) const {
    Amg::Vector2D lpos;
    spacePointPosition(phiId,etaId,lpos);
    surface(phiId).localToGlobal(lpos,pos,pos);
    return true;
  }

  inline void sTgcReadoutElement::spacePointPosition( const Amg::Vector2D& phiPos, const Amg::Vector2D& etaPos, Amg::Vector2D& pos ) const {
    pos[0] = phiPos.x();
    pos[1] = etaPos.x();
  }


} // namespace MuonGM

#endif // MUONGEOMODEL_STGCREADOUTELEMENT_H
