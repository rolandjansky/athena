/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Mdt Readout Element properties
 -----------------------------------------

*/

#ifndef MUONGEOMODEL_MDTREADOUTELEMENT_H
# define MUONGEOMODEL_MDTREADOUTELEMENT_H

//<<<<<< INCLUDES                                                       >>>>>>
#include "MuonReadoutGeometry/MuonReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "TrkDistortedSurfaces/SaggedLineSurface.h"
//<<<<<< PUBLIC DEFINES                                                 >>>>>>
#define maxnlayers  4
#define maxnsteps  10


class BLinePar;

namespace Trk{
    class CylinderBounds;
    class SurfaceBounds;
}

namespace Muon {
  class MdtAlignModule;
  class CombinedMuonAlignModule;
}

namespace MuonGM {

/**
   An MdtReadoutElement corresponds to a single MDT multilayer; therefore 
   typicaly a MDT chamber consists of two MdtReadoutElements, each identified 
   by StationName, StationEta, StationPhi, Technology=0, and Multilayer.
   
   Pointers to all MdtReadoutElements are created in the build() method of 
   the MuonChamber class, and are held in arrays by the MuonDetectorManager, 
   which is responsible for storing, deleting and providing access to these 
   objects. 

   An MdtReadoutElement holds properties related to its internal structure 
   (i.e. wire pitch) and general geometrical properties (size);  it 
   implements tracking interfaces and provides access to typical 
   readout-geometry information: i.e. number of tubes, position of the tubes, 
   distance of a point to the readout side, etc.

   The globalToLocalCoords and globalToLocalTransform methods (+ their opposite) 
   define the link between the ATLAS global reference frame and the internal 
   (geo-model defined) local reference frame of any drift tube (which is the 
   frame where local coordinates of SimHits, in output from G4, are expressed).
*/

class MdtReadoutElement: public MuonReadoutElement 
{
  
friend class Muon::MdtAlignModule;
friend class Muon::CombinedMuonAlignModule;
friend class MuonChamber;
  
public:
  
   MdtReadoutElement(GeoVFullPhysVol* pv, std::string stName,
                     int zi, int fi, bool is_mirrored, MuonDetectorManager* mgr);
                      
   virtual ~MdtReadoutElement();

   // Id set/get methods
   inline int getMultilayer() const;
   inline int getNLayers() const;
   inline int getNtubesperlayer() const;
   inline int getNtubesinastep() const;
   inline void setMultilayer(int ml);
   inline void setNLayers(int nl);
   inline bool isInBarrel() const;
   void  setIdentifier(Identifier id);
   bool getWireFirstLocalCoordAlongZ(int tubeLayer, double& coord) const;
   bool getWireFirstLocalCoordAlongR(int tubeLayer, double& coord) const;

   virtual bool containsId(Identifier id) const;
    
    // detector specific 
    inline double getTubeLength(int tubeLayer, int tube) const;
    inline double getActiveTubeLength(int tubeLayer, int tube) const;
    inline double getGasLength(int tubeLayer, int tube) const;
    inline double getWireLength(int tubeLayer, int tube) const;
    double tubeLength(Identifier id) const;
    double innerTubeRadius() const;
    double outerTubeRadius() const;
    inline double tubePitch() const;

    // local(tube frame) to global coord.
    const Amg::Vector3D                    localToGlobalCoords(Amg::Vector3D x, Identifier id) const;
    const Amg::Transform3D                localToGlobalTransf(Identifier id) const;
    const Amg::Transform3D                localToGlobalTransf(int tubeLayer, int tube) const;
    const Amg::Vector3D           nodeform_localToGlobalCoords(Amg::Vector3D x, Identifier id) const;
    const Amg::Transform3D       nodeform_localToGlobalTransf(Identifier id) const;
    // global to local(tube frame) coord.
    const Amg::Vector3D                    globalToLocalCoords(Amg::Vector3D x, Identifier id) const;
    const Amg::Transform3D                globalToLocalTransf(Identifier id) const;
    const Amg::Vector3D           nodeform_globalToLocalCoords(Amg::Vector3D x, Identifier id) const;
    const Amg::Transform3D       nodeform_globalToLocalTransf(Identifier id) const;
    // local(tube frame) to multilayer coord.
    const Amg::Vector3D           tubeToMultilayerCoords(Amg::Vector3D x, Identifier id) const;
    const Amg::Transform3D       tubeToMultilayerTransf(Identifier id) const;
    const Amg::Vector3D     nodeform_tubeToMultilayerCoords(Amg::Vector3D x, Identifier id) const;
    const Amg::Transform3D nodeform_tubeToMultilayerTransf(Identifier id) const;
    // multilayer to local (tube frame) coords
    const Amg::Vector3D           multilayerToTubeCoords(Amg::Vector3D x, Identifier id) const;
    const Amg::Transform3D       multilayerToTubeTransf(Identifier id) const;
    const Amg::Vector3D     nodeform_multilayerToTubeCoords(Amg::Vector3D x, Identifier id) const;
    const Amg::Transform3D nodeform_multilayerToTubeTransf(Identifier id) const;

    // in the native MDT reference system
    const Amg::Vector3D localTubePos(Identifier id) const;
    const Amg::Vector3D localTubePos(IdentifierHash id) const;
    const Amg::Vector3D localTubePos(int multilayer, int tubelayer, int tube) const;
    const Amg::Vector3D nodeform_localTubePos(Identifier id) const;
    const Amg::Vector3D nodeform_localTubePos(IdentifierHash id) const;
    const Amg::Vector3D nodeform_localTubePos(int multilayer, int tubelayer, int tube) const;
    // in the global reference system
    const Amg::Vector3D          tubePos(Identifier id) const;
    const Amg::Vector3D          tubePos(IdentifierHash id) const;
    const Amg::Vector3D          tubePos(int multilayer, int tubelayer, int tube) const;
    const Amg::Vector3D nodeform_tubePos(Identifier id) const;
    const Amg::Vector3D nodeform_tubePos(IdentifierHash id) const;
    const Amg::Vector3D nodeform_tubePos(int multilayer, int tubelayer, int tube) const;
    // in the Amdb local (szt) reference system //////////////// check here ////////////
    const Amg::Vector3D AmdbLRStubePos(Identifier id) const;
    const Amg::Vector3D AmdbLRStubePos(int multilayer, int tubelayer, int tube) const;

    // Readout / HV side
    double signedRODistanceFromTubeCentre(const Identifier& id) const;
    double signedRODistanceFromTubeCentre(int ml, int tl, int tube) const;
    double RODistanceFromTubeCentre(const Identifier& id) const;
    double RODistanceFromTubeCentre(int ml, int tl, int tube) const;
    double distanceFromRO(Amg::Vector3D GlobalHitPosition, Identifier id) const;
    double distanceFromRO(Amg::Vector3D GlobalHitPosition, int multilayer, int tubelayer, int tube) const;
    int isAtReadoutSide(Amg::Vector3D GlobalHitPosition, Identifier id) const;
    int isAtReadoutSide(Amg::Vector3D GlobalHitPosition, int multilayer, int tubelayer, int tube) const;
    const Amg::Vector3D localROPos(int multilayer, int tubelayer, int tube) const;
    const Amg::Vector3D ROPos(int multilayer, int tubelayer, int tube) const;
    const Amg::Vector3D tubeFrame_localROPos(int multilayer, int tubelayer, int tube) const;
    const Amg::Vector3D localROPos(Identifier id) const;
    const Amg::Vector3D ROPos(Identifier id) const;
    const Amg::Vector3D tubeFrame_localROPos(Identifier id) const;

    // defining B-line parameters 
    /*inline*/ void setBLinePar(BLinePar*  bLine) const;
    inline void clearBLinePar() const; 
    inline const BLinePar* getBLinePar() const {return m_BLinePar;} 

    ////////////////////////////////////////////////////////////
    //// Tracking interfaces
    ////////////////////////////////////////////////////////////

    void         clearCache() const;
    void         fillCache() const;
    void         refreshCache() const {clearCache();}
    void         clearBLineCache() const;
    void         fillBLineCache() const;
    //void         fillBLineCache() const;
    virtual const Trk::Surface& surface() const;
    virtual const Trk::SaggedLineSurface& surface(const Identifier& id) const;
    virtual const Trk::SaggedLineSurface& surface(int tubeLayer, int tube) const;
    virtual const Trk::SurfaceBounds& bounds() const;
    virtual const Trk::CylinderBounds& bounds(const Identifier& id) const;
    virtual const Trk::CylinderBounds& bounds(int tubeLayer, int tube) const;
    
    virtual const Amg::Transform3D& transform(const Identifier&) const;    
    virtual const Amg::Transform3D& transform(int tubeLayer, int tube) const;    
    virtual const Amg::Transform3D& transform() const {return absTransform();}
    
    virtual const Amg::Vector3D& center(const Identifier&) const;
    virtual const Amg::Vector3D& center(int tubeLayer, int tube) const;
    virtual const Amg::Vector3D& center() const;
    
    virtual const Amg::Vector3D& normal(const Identifier&) const {return normal();}
    virtual const Amg::Vector3D tubeNormal(const Identifier&) const;
    virtual const Amg::Vector3D& normal(int , int ) const {return normal();}
    virtual const Amg::Vector3D tubeNormal(int, int ) const;
    virtual const Amg::Vector3D& normal() const;
      
    /** returns all the surfaces contained in this detector element */
    virtual const std::vector<const Trk::Surface*>& surfaces() const;

    // methods handling deformations
    const Amg::Transform3D& fromIdealToDeformed(const Identifier&) const;
    const Amg::Transform3D& fromIdealToDeformed(int multilayer, int tubelayer, int tube) const;
    
private:

    double getTubeLengthForCaching(int tubeLayer, int tube) const;
    double getNominalTubeLengthWoCutouts(int tubeLayer, int tube) const;
    Amg::Vector3D localNominalTubePosWoCutouts(int tubelayer, int tube) const;

    Amg::Vector3D posOnDefChamWire(const Amg::Vector3D& locAMDBPos, double, double, double, double, 
	double, double, double, double, double, double, double, double,
	double, double, double, const Amg::Vector3D fixedPoint) const;
    Amg::Vector3D posOnDefChamWire(const Amg::Vector3D& locAMDBPos, const BLinePar* bLine, const Amg::Vector3D fixedPoint) const;
    void wireEndpointsAsBuilt(Amg::Vector3D& locAMDBWireEndP, Amg::Vector3D& locAMDBWireEndN, int multilayer, int tubelayer, int tube) const;

    // methods used only by friend class MdtAlignModule to shift chambers
    void shiftTube(const Identifier& id) const;
    void restoreTubes() const;

    
    int    m_multilayer;
    int    m_nlayers;
    double m_tubepitch;
    double m_tubelayerpitch;
    int    m_ntubesperlayer;
    int    m_nsteps;
    int    m_ntubesinastep;
    double m_tubelenStepSize;
    double m_tubelength[maxnsteps];
    double m_cutoutShift;
    double m_endpluglength;
    double m_deadlength;
    bool   m_inBarrel;
    double m_firstwire_x[maxnlayers];
    double m_firstwire_y[maxnlayers];
    double m_innerRadius;
    double m_tubeWallThickness;
    mutable int m_zsignRO_tubeFrame; // comes from AMDB CRO location in the station

    mutable std::vector<Amg::Transform3D*> * m_deformTransfs;
    mutable BLinePar* m_BLinePar;
    mutable Amg::Vector3D                           * m_elemNormal;        // one
    mutable std::vector<Trk::SaggedLineSurface *> * m_tubeSurfaces;  // one per tube
    mutable std::vector<Amg::Transform3D *>         * m_tubeTransf;    // one per tube
    mutable std::vector<Amg::Vector3D *>             * m_tubeCenter;    // one per tube
    mutable std::vector<Trk::CylinderBounds *>    * m_tubeBounds;    // one per step in tube-length
    mutable std::vector<Amg::Vector3D *>             * m_backupTubeCenter;  // one per tube
    mutable std::vector<Amg::Transform3D *>         * m_backupTubeTransf;  // one per tube
    mutable std::vector<Amg::Transform3D *>         * m_backupDeformTransf;  // one per tube
    
    /** these are all surfaces represented by this detector element : it's for visualization without casting */
    mutable std::vector<const Trk::Surface*>  m_elementSurfaces;

    // the single surface information representing the DetElement
    mutable Trk::Surface*           m_associatedSurface; 
    mutable Trk::SurfaceBounds*     m_associatedBounds;

    
};

int MdtReadoutElement::getMultilayer() const 
  {return m_multilayer;}
int MdtReadoutElement::getNLayers() const
  {return m_nlayers;}
int MdtReadoutElement::getNtubesperlayer() const
  {return m_ntubesperlayer;}
int MdtReadoutElement::getNtubesinastep() const
  {return m_ntubesinastep;}
void MdtReadoutElement::setMultilayer(int ml)
  {m_multilayer = ml;}
void MdtReadoutElement::setNLayers(int nl)
  {m_nlayers = nl;}
bool MdtReadoutElement::isInBarrel() const
  {return  m_inBarrel;}
double MdtReadoutElement::tubePitch() const
  {return m_tubepitch;}
void MdtReadoutElement::clearBLinePar() const
  {m_BLinePar = 0;}
double MdtReadoutElement::getWireLength(int tubeLayer, int tube) const
{
  return getGasLength(tubeLayer, tube);
}
double MdtReadoutElement::getGasLength(int tubeLayer, int tube) const
{
  return getTubeLength(tubeLayer,tube) - 2.*m_endpluglength;
}
double MdtReadoutElement::getTubeLength(int tubeLayer, int tube) const
{
  //std::cout<<" in getTubeLength going to compure getActiveTubeLength (+2.*m_deadlength="<<m_deadlength<<")"<<std::endl; 
  return getActiveTubeLength(tubeLayer, tube) + 2.*m_deadlength;
}
double MdtReadoutElement::getActiveTubeLength(int tubeLayer, int tube) const
{
  //std::cout<<" in getActiveTubeLength going to compute 2*bounds.halflength for tLayer,tube="<<tubeLayer<<" "<<tube<<" "<<std::endl;
  return 2.*(bounds(tubeLayer,tube).halflengthZ());
}

inline const std::vector<const Trk::Surface*>& MdtReadoutElement::surfaces() const {
    // create when first time requested and when possible
    if (!m_elementSurfaces.size() && ( m_associatedSurface || m_tubeSurfaces ) ){
        if (m_associatedSurface) m_elementSurfaces.push_back(m_associatedSurface);
        if (m_tubeSurfaces){
            for (auto& sf : (*m_tubeSurfaces) )
                m_elementSurfaces.push_back(sf);
        }
    }
    // return the element surfaces
    return m_elementSurfaces;
}

} // namespace MuonGM

#endif // MUONGEOMODEL_MDTREADOUTELEMENT_H
