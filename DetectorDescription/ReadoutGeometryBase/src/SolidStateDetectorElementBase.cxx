/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "ReadoutGeometryBase/SolidStateDetectorElementBase.h"

#include "TrkSurfaces/PlaneSurface.h"

namespace InDetDD {
using Trk::distPhi;
using Trk::distEta;
using Trk::distDepth;

SolidStateDetectorElementBase::SolidStateDetectorElementBase(const Identifier &id,
                                                             const DetectorDesign *design,
                                                             const GeoVFullPhysVol *geophysvol,
                                                             SiCommonItems *commonItems):
    TrkDetElementBase(geophysvol),
    m_id(id),
    m_design(design),
    m_commonItems(commonItems),
    m_baseCacheValid(false),
    m_conditionsCacheValid(false),
    m_firstTimeBase(true),
    m_barrelLike(true),
    m_tanLorentzAnglePhi(0),
    m_tanLorentzAngleEta(0),
    m_lorentzCorrection(0),
    m_surface(0)
{
    //The following are fixes for coverity bug 11955, uninitialized scalars:
    const bool boolDefault(true);
    m_depthDirection=boolDefault;
    m_phiDirection=boolDefault;
    m_etaDirection=boolDefault;
    const double defaultMin(std::numeric_limits<double>::max());
    const double defaultMax(std::numeric_limits<double>::lowest());
    m_minZ=defaultMin;
    m_maxZ=defaultMax;
    m_minR=defaultMin;
    m_maxR=defaultMax;
    m_minPhi=defaultMin;
    m_maxPhi=defaultMax;

    m_hitEta = m_design->etaAxis();
    m_hitPhi = m_design->phiAxis();
    m_hitDepth = m_design->depthAxis();

    if (!m_id.is_valid()) throw std::runtime_error("SolidStateDetectorElementBase: Invalid identifier");

    // Increase the reference count of the DetectorDesign and SiCommonItems objects.
    m_design->ref();
    m_commonItems->ref();

    // Should we reference count the geophysvol as well?

}

SolidStateDetectorElementBase::~SolidStateDetectorElementBase()
{

    // The design and SiCommonItems are reference counted so that it will not be deleted until the last element is deleted.
    m_design->unref();
    m_commonItems->unref();

}

void
SolidStateDetectorElementBase::updateCache() const
{
    m_baseCacheValid = true;

    bool firstTimeBaseTmp = m_firstTimeBase;
    m_firstTimeBase = false;

    const HepGeom::Transform3D &geoTransform = getMaterialGeom()->getAbsoluteTransform() * m_design->moduleShift();

    m_centerCLHEP = geoTransform * m_design->sensorCenter();
    m_center = Amg::Vector3D(m_centerCLHEP[0],m_centerCLHEP[1],m_centerCLHEP[2]);

    HepGeom::Point3D<double> centerGeoModel(0., 0., 0.);
    m_originCLHEP = geoTransform * centerGeoModel;
    m_origin = Amg::Vector3D(m_originCLHEP[0],m_originCLHEP[1],m_originCLHEP[2]);
    //
    // Determine directions depth, eta and phi axis in reconstruction local frame
    // ie depth away from interaction point
    //    phi in direction of increasing phi
    //    eta in direction of increasing z in barrel, and increasing r in endcap
    //

    // depthAxis, phiAxis, and etaAxis are defined to be x,y,z respectively for all detectors for hit local frame.
    // depthAxis, phiAxis, and etaAxis are defined to be z,x,y respectively for all detectors for reco local frame.
    static const HepGeom::Vector3D<double> localAxes[3] = {
        HepGeom::Vector3D<double>(1,0,0),
        HepGeom::Vector3D<double>(0,1,0),
        HepGeom::Vector3D<double>(0,0,1)
    };

    static const HepGeom::Vector3D<double> & localRecoPhiAxis = localAxes[distPhi];     // Defined to be same as x axis
    static const HepGeom::Vector3D<double> & localRecoEtaAxis = localAxes[distEta];     // Defined to be same as y axis
    static const HepGeom::Vector3D<double> & localRecoDepthAxis = localAxes[distDepth]; // Defined to be same as z axis

    // We only need to calculate the rough orientation once.
    //For it to change would require extreme unrealistic misalignment changes.
    if (firstTimeBaseTmp) {
        // Determine the unit vectors in global frame
   
        const HepGeom::Vector3D<double> &geoModelPhiAxis = localAxes[m_hitPhi];
        const HepGeom::Vector3D<double> &geoModelEtaAxis = localAxes[m_hitEta];
        const HepGeom::Vector3D<double> &geoModelDepthAxis = localAxes[m_hitDepth];
 
        HepGeom::Vector3D<double> globalDepthAxis(geoTransform * geoModelDepthAxis);
        HepGeom::Vector3D<double> globalPhiAxis(geoTransform * geoModelPhiAxis);
        HepGeom::Vector3D<double> globalEtaAxis(geoTransform * geoModelEtaAxis);

        // unit radial vector
        HepGeom::Vector3D<double> unitR(m_center.x(), m_center.y(), 0.);
        unitR.setMag(1.);

        HepGeom::Vector3D<double> nominalEta;
        HepGeom::Vector3D<double> nominalNormal;
        HepGeom::Vector3D<double> nominalPhi(-unitR.y(), unitR.x(), 0);

        // In Barrel like geometry, the etaAxis is along increasing z, and normal is in increasing radial direction.
        // In Endcap like geometry, the etaAxis is along increasing r, and normal is in decreasing z direction,
        // We base whether it is barrel like or endcap like by the orientation of the local z axis of the 
        // the element. This allows the use of endcap identifiers in a TB setup.

        nominalEta.setZ(1);
        if (std::abs(globalEtaAxis.dot(nominalEta)) < 0.5) { // Check that it is in roughly the right direction. Allowed not to be for ITK inclined/barrel ring modules
            m_barrelLike = false;
        }

        if (m_barrelLike) {
          nominalEta.setZ(1);
          nominalNormal =  unitR;
        } else { // endcap like
          nominalNormal.setZ(-1);
          nominalEta = unitR;
        }

        // Determine if axes are to have there directions swapped.

        //
        // Depth axis.
        //
        double depthDir = globalDepthAxis.dot(nominalNormal);
        m_depthDirection = true;
        if (depthDir < 0) {
            if (m_design->depthSymmetric()) {
            m_depthDirection = false;
            } else {
                if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Unable to swap local depth axis." << endreq;
            }
        }
        if (std::abs(depthDir) < 0.5) { // Check that it is in roughly the right direction.
            msg(MSG::ERROR) << "Orientation of local depth axis does not follow correct convention." << endreq;
            // throw std::runtime_error("Orientation of local depth axis does not follow correct convention.");
            m_depthDirection = true; // Don't swap.
        }
        
        // for HGTD modules, the check on phi and eta directions don't make sense
        // as the modules do not respect the conventional position for endcap discs:
        // - the local eta axis is never parallel to the radial direction
        // - the local phi axis is never perpendicular to the radial direction
        // hence, removing errors and allowing swap of the axis when needed
        bool isHGTD = this->getIdHelper()->is_hgtd(m_id);

        //
        // Phi axis
        //
        double phiDir = globalPhiAxis.dot(nominalPhi);
        m_phiDirection = true;
        if (phiDir < 0) {
            if (m_design->phiSymmetric()) {
                m_phiDirection = false;
            } else {
                if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Unable to swap local xPhi axis." << endreq;
            }
        }
    
        if (not isHGTD and std::abs(phiDir) < 0.5) { // Check that it is in roughly the right direction.
            msg(MSG::ERROR) << "Orientation of local xPhi axis does not follow correct convention." << endreq;
            // throw std::runtime_error("Orientation of local xPhi axis does not follow correct convention.");
            m_phiDirection = true; // Don't swap.
        }
    
        //
        // Eta axis
        //
        double etaDir = globalEtaAxis.dot(nominalEta);
        m_etaDirection = true;
        if (etaDir < 0) {
            if (m_design->etaSymmetric()) {
                m_etaDirection = false;
            } else {
                if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Unable to swap local xEta axis." << endreq;
            }
        }
        if (not isHGTD and std::abs(etaDir) < 0.5) { // Check that it is in roughly the right direction.
            msg(MSG::ERROR) << "Orientation of local xEta axis does not follow correct convention." << endreq;
            // throw std::runtime_error("Orientation of local xEta axis does not follow correct convention.");
            m_etaDirection = true; // Don't swap
        }

    } // end if (firstTimeBaseTemp)

    m_transformHit   = geoTransform * m_design->SiHitToGeoModel();
    m_transformCLHEP = geoTransform * recoToHitTransform();
    m_transform = Amg::CLHEPTransformToEigen(m_transformCLHEP);

    #ifndef NDEBUG
    // Check that local frame is right-handed. (ie transform has no reflection)
    // This can be done by checking that the determinant is >0.
    if (firstTimeBaseTmp) { // Only need to check this once.
        HepGeom::Transform3D & t = m_transformCLHEP;
        double det = t(0,0) * (t(1,1)*t(2,2) - t(1,2)*t(2,1)) -
                     t(0,1) * (t(1,0)*t(2,2) - t(1,2)*t(2,0)) +
                     t(0,2) * (t(1,0)*t(2,1) - t(1,1)*t(2,0));
        if (det < 0) {
            if(msgLvl(MSG::DEBUG)) {
                msg(MSG::DEBUG) << "Local frame is left-handed. (hitEtaDirection, hitPhiDirection, hitDepthDirection) = ("
                                << m_etaDirection <<", "
                                << m_phiDirection <<", "
                                << m_depthDirection <<")"
                                << endreq;
            }
        }
    }
    #endif

    // Initialize various cached members, needs to be done here otherwise the necessary transforms are not yet initialized
    // The unit vectors
    m_normalCLHEP = m_transformCLHEP * localRecoDepthAxis;
    m_normal = Amg::Vector3D( m_normalCLHEP[0], m_normalCLHEP[1], m_normalCLHEP[2]);
  
  
    m_phiAxisCLHEP = m_transformCLHEP * localRecoPhiAxis;
    m_etaAxisCLHEP = m_transformCLHEP * localRecoEtaAxis;
  
    m_phiAxis = Amg::Vector3D(m_phiAxisCLHEP[0],m_phiAxisCLHEP[1],m_phiAxisCLHEP[2]);
    m_etaAxis = Amg::Vector3D(m_etaAxisCLHEP[0],m_etaAxisCLHEP[1],m_etaAxisCLHEP[2]);

    getExtent(m_minR, m_maxR, m_minZ, m_maxZ, m_minPhi, m_maxPhi);

}

void 
SolidStateDetectorElementBase::updateConditionsCache() const
{
    m_conditionsCacheValid = true;

    // 
    // Lorentz Angle related stuff
    // 

    if (m_commonItems->lorentzAngleSvc()) {
        m_tanLorentzAnglePhi = m_commonItems->lorentzAngleSvc()->getTanLorentzAngle(m_idHash);
        m_tanLorentzAngleEta = m_commonItems->lorentzAngleSvc()->getTanLorentzAngleEta(m_idHash);
        m_lorentzCorrection = m_commonItems->lorentzAngleSvc()->getLorentzShift(m_idHash);
    } else {
        // Set to zero
        m_tanLorentzAnglePhi = 0;
        m_tanLorentzAngleEta = 0;
        m_lorentzCorrection =  0;
    }
}

const Trk::Surface &
SolidStateDetectorElementBase::surface() const
{
    if (!m_surface){
        m_surface = new Trk::PlaneSurface(*this);
    }
    return *m_surface;
}

const HepGeom::Transform3D &
SolidStateDetectorElementBase::transformHit() const
{
  if (!m_baseCacheValid) {
      updateCache();
  }
  return m_transformHit;
}

const Amg::Transform3D &
SolidStateDetectorElementBase::transform() const
{
  if (!m_baseCacheValid) updateCache();
  return m_transform;
}

const HepGeom::Transform3D &
SolidStateDetectorElementBase::transformCLHEP() const
{
  //stuff to get the CLHEP version of the local to global transform.
  if (!m_baseCacheValid) updateCache();
  return m_transformCLHEP;

}
const HepGeom::Transform3D
SolidStateDetectorElementBase::defTransformCLHEP() const
{
  return getMaterialGeom()->getDefAbsoluteTransform() * recoToHitTransform();
}  
   
const Amg::Transform3D
SolidStateDetectorElementBase::defTransform() const
{
  HepGeom::Transform3D tmpTransform =  getMaterialGeom()->getDefAbsoluteTransform() * recoToHitTransform();
  return Amg::CLHEPTransformToEigen(tmpTransform);
}

const HepGeom::Transform3D
SolidStateDetectorElementBase::recoToHitTransform() const
{

    // Determine the reconstruction local (LocalPosition) to global transform.

    if (!m_baseCacheValid) updateCache();

    // global = transform * recoLocal
    //        = transfromHit * hitLocal
    //        = transformHit * recoToHitTransform * recoLocal
    // recoToHitTransform takes recoLocal to hitLocal
    // x,y,z -> y,z,x 
    // equiv to a rotation around Y of 90 deg followed by a rotation around X of 90deg
    // 
    // recoToHit is static as it needs to be calculated once only.
    // We use the HepGeom::Transform3D constructor which takes one coordinates system to another where the
    // coordinate system is defined by it center and two axes.
    // distPhi, distEta are the reco local axes and hitPhi and hitEta are the hit local axes.
    // It assume phi, eta, depth makes a right handed system which is the case.
    static const HepGeom::Vector3D<double> localAxes[3] = {
        HepGeom::Vector3D<double>(1,0,0),
        HepGeom::Vector3D<double>(0,1,0),
        HepGeom::Vector3D<double>(0,0,1)
    };
    //static 

    int signPhi = m_phiDirection? +1: -1;
    int signEta = m_etaDirection? +1: -1;

    const HepGeom::Transform3D recoToHit(HepGeom::Point3D<double>(0, 0, 0),
    signPhi * localAxes[distPhi], signEta * localAxes[distEta],
    HepGeom::Point3D<double>(0, 0, 0), localAxes[m_hitPhi], localAxes[m_hitEta]);

    return recoToHit;
}

Amg::Vector2D SolidStateDetectorElementBase::correctLocalPosition(const Amg::Vector2D &position) const
{
    Amg::Vector2D correctedPosition(position);
    correctedPosition[distPhi] += getLorentzCorrection();
    return correctedPosition;
}

const Amg::Vector3D & 
SolidStateDetectorElementBase::center() const
{
  if (!m_baseCacheValid) updateCache();
  return m_center;
}

const Amg::Vector3D & 
SolidStateDetectorElementBase::origin() const
{
  if (!m_baseCacheValid) updateCache();
  return m_origin;
}

const Amg::Vector3D & 
SolidStateDetectorElementBase::normal() const
{
  if (!m_baseCacheValid) updateCache();
  return m_normal;
}

const Trk::SurfaceBounds &
SolidStateDetectorElementBase::bounds() const
{
  return m_design->bounds();
}

const HepGeom::Vector3D<double> & 
SolidStateDetectorElementBase::etaAxisCLHEP() const
{
  if (!m_baseCacheValid) updateCache();
  return m_etaAxisCLHEP;
}

const HepGeom::Vector3D<double> & 
SolidStateDetectorElementBase::phiAxisCLHEP() const
{
  if (!m_baseCacheValid) updateCache();
  return m_phiAxisCLHEP;
}

const Amg::Vector3D & 
SolidStateDetectorElementBase::etaAxis() const
{
  if (!m_baseCacheValid) updateCache();
  return m_etaAxis;
}

const Amg::Vector3D & 
SolidStateDetectorElementBase::phiAxis() const
{
  if (!m_baseCacheValid) updateCache();
  return m_phiAxis;
}

// Get min/max or r, z,and phi
void SolidStateDetectorElementBase::getExtent(double &rMin, double &rMax,
           double &zMin, double &zMax,
           double &phiMin, double &phiMax) const
{
    HepGeom::Point3D<double> corners[4];
    getCorners(corners);

    bool first = true;

    double phiOffset = 0.;

    HepGeom::Point3D<double> sensorCenter = m_design->sensorCenter();
    double radialShift = sensorCenter[0];

    const HepGeom::Transform3D rShift = HepGeom::TranslateY3D(radialShift);//in local frame, radius is y=distEta

    for (int i = 0; i < 4; i++) {
        corners[i].transform(rShift);
        HepGeom::Point3D<double> globalPoint = globalPosition(corners[i]);

        double rPoint = globalPoint.perp();
        double zPoint = globalPoint.z();
        double phiPoint = globalPoint.phi();
    
        // Use first point to initializa min/max values.
        if (first) {

            // Put phi in a range so that we are not near -180/+180 division.
            // Do this by adding an offset if phi > 90 CLHEP::deg or < -90 CLHEP::deg.
            // This offset is later removed.
            if (phiPoint < -0.5 * M_PI) {
                phiOffset = -0.5 * M_PI;
            } else if  (phiPoint > 0.5 * M_PI) {
                phiOffset = 0.5 * M_PI;
            }
            phiMin = phiMax = phiPoint - phiOffset;
            rMin = rMax = rPoint;
            zMin = zMax = zPoint;

        } else {
            phiPoint -= phiOffset;
            // put phi back in -M_PI < phi < +M_PI range
            if (phiPoint < -M_PI) phiPoint += 2. * M_PI;
            if (phiPoint > M_PI)  phiPoint -= 2. * M_PI;
            phiMin = std::min(phiMin, phiPoint);
            phiMax = std::max(phiMax, phiPoint);
            rMin = std::min(rMin, rPoint);
            rMax = std::max(rMax, rPoint);
            zMin = std::min(zMin, zPoint);
            zMax = std::max(zMax, zPoint);
        }
            first = false;
    }

    // put phi back in -M_PI < phi < +M_PI range
    phiMin += phiOffset;
    phiMax += phiOffset;
    if (phiMin < -M_PI) phiMin += 2. * M_PI;
    if (phiMin > M_PI)  phiMin -= 2. * M_PI;
    if (phiMax < -M_PI) phiMax += 2. * M_PI;
    if (phiMax > M_PI)  phiMax -= 2. * M_PI;

}



void SolidStateDetectorElementBase::getCorners(HepGeom::Point3D<double> *corners) const
{
    if (!m_baseCacheValid) updateCache();

    // This makes the assumption that the forward SCT detectors are orientated such that 
    // the positive etaAxis corresponds to the top of the detector where the width is largest.
    // This is currently always the case.
    // For the SCT barrel and pixel detectors minWidth and maxWidth are the same and so should 
    // work for all orientations.

    double minWidth = m_design->minWidth();
    double maxWidth = m_design->maxWidth();
    double length   = m_design->length();
  
    // Lower left
    corners[0][distPhi] = -0.5 * minWidth;
    corners[0][distEta] = -0.5 * length;
    corners[0][distDepth] = 0.;

    // Lower right
    corners[1][distPhi] =  0.5 * minWidth;
    corners[1][distEta] = -0.5 * length;
    corners[1][distDepth] = 0.;

    // Upper Right
    corners[2][distPhi] = 0.5 * maxWidth;
    corners[2][distEta] = 0.5 * length;
    corners[2][distDepth] = 0.;

    // Upper left
    corners[3][distPhi] = -0.5 * maxWidth;
    corners[3][distEta] =  0.5 * length;
    corners[3][distDepth] = 0.;
}

Amg::Vector2D
SolidStateDetectorElementBase::hitLocalToLocal(double xEta, double xPhi) const  // Will change order to phi,eta
{
    if (!m_baseCacheValid) updateCache();
    if (!m_etaDirection) xEta = -xEta;
    if (!m_phiDirection) xPhi = -xPhi;
    return Amg::Vector2D(xPhi, xEta);
}

HepGeom::Point3D<double>
SolidStateDetectorElementBase::hitLocalToLocal3D(const HepGeom::Point3D<double> & hitPosition) const
{
    // Equiv to transform().inverse * transformHit() * hitPosition
    if (!m_baseCacheValid) updateCache();
    double xDepth = hitPosition[m_hitDepth];
    double xPhi = hitPosition[m_hitPhi];
    double xEta = hitPosition[m_hitEta];
    if (!m_depthDirection) xDepth = -xDepth;
    if (!m_phiDirection) xPhi = -xPhi;
    if (!m_etaDirection) xEta = -xEta;
    return HepGeom::Point3D<double>(xPhi, xEta, xDepth);
}


// Get eta/phi extent. Returns min/max eta and phi and r (for barrel)
// or z (for endcap) Takes as input the vertex spread in z (+-deltaZ).
// Gets 4 corners of the sensor and calculates eta phi for each corner
// for both +/- vertex spread.  The returned phi is between -M_PI and M_PI
// with the direction minPhi to maxPhi always in the positive sense,
// so if the element extends across the -180/180 boundary then minPhi will
// be greater than maxPhi.
void SolidStateDetectorElementBase::getEtaPhiRegion(double deltaZ, double &etaMin, double &etaMax, double &phiMin, 
                                                    double &phiMax, double &rz) const
{
    if (!m_baseCacheValid) updateCache();

    HepGeom::Point3D<double> corners[4];
    getCorners(corners);

    bool first = true; 

    double phiOffset = 0.;

    for (int i = 0; i < 4; i++) {
        double etaMinPoint;
        double etaMaxPoint;
        double phiPoint;

        // Get the eta phi value for this corner.
        getEtaPhiPoint(corners[i], deltaZ, etaMinPoint, etaMaxPoint, phiPoint);

        if (first) { // Use the first point to initialize the min/max values.

            // Put phi in a range so that we are not near -180/+180 division.
            // Do this by adding an offset if phi > 90 CLHEP::deg or < -90 CLHEP::deg. 
            // This offset is later removed.
            if (phiPoint < -0.5 * M_PI) {
                phiOffset = -0.5 * M_PI;
            } else if  (phiPoint > 0.5 * M_PI) {
                phiOffset = 0.5 * M_PI;
            }
            phiMin = phiMax = phiPoint - phiOffset;
            etaMin = etaMinPoint;
            etaMax = etaMaxPoint;
        } else {
            phiPoint -= phiOffset;
            // put phi back in -M_PI < phi < +M_PI range
            if (phiPoint < -M_PI) phiPoint += 2. * M_PI; 
            if (phiPoint > M_PI)  phiPoint -= 2. * M_PI; 
            phiMin = std::min(phiMin, phiPoint);
            phiMax = std::max(phiMax, phiPoint);
            etaMin = std::min(etaMin, etaMinPoint);
            etaMax = std::max(etaMax, etaMaxPoint);
        }
        first = false;
    }

    // put phi back in -M_PI < phi < +M_PI range    
    phiMin += phiOffset;
    phiMax += phiOffset;
    if (phiMin < -M_PI) phiMin += 2. * M_PI; 
    if (phiMin > M_PI)  phiMin -= 2. * M_PI; 
    if (phiMax < -M_PI) phiMax += 2. * M_PI; 
    if (phiMax > M_PI)  phiMax -= 2. * M_PI; 

    rz = get_rz();

}


// Gets eta phi for a point given in local coordinates. deltaZ is specified to
// account for the vertex spread. phi is independent of this vertex
// spread. etaMax will correspond to zMin (-deltaZ) and etaMin will
// correspond to zMax (+deltaZ).
void SolidStateDetectorElementBase::getEtaPhiPoint(const HepGeom::Point3D<double> & point, double deltaZ, 
                       double &etaMin, double &etaMax, double &phi) const
{
    // Get the point in global coordinates.
    HepGeom::Point3D<double> globalPoint = globalPosition(point);

    double r = globalPoint.perp();
    double z = globalPoint.z();
  
    double thetaMin = atan2(r,(z + deltaZ));
    etaMax = -log(tan(0.5 * thetaMin));
    double thetaMax = atan2(r,(z - deltaZ));
    etaMin = -log(tan(0.5 * thetaMax));
  
    phi = globalPoint.phi();
}

Amg::Vector2D
SolidStateDetectorElementBase::correctedLocalPositionOfCell(const SiCellId &cellId) const
{
  Amg::Vector2D pos(m_design->localPositionOfCell(cellId));
  return correctLocalPosition(pos);
}

Amg::Vector2D
SolidStateDetectorElementBase::correctedLocalPositionOfCell(const Identifier & id) const
{
  SiCellId cellId = cellIdFromIdentifier(id);
  SiLocalPosition silp = m_design->localPositionOfCell(cellId);
  Amg::Vector2D pos = hitLocalToLocal(silp.xEta(), silp.xPhi());
  return correctLocalPosition(pos);
}

Amg::Vector2D
SolidStateDetectorElementBase::localPositionOfCell(const SiCellId &cellId) const
{
    return m_design->localPositionOfCell(cellId);
}

Amg::Vector2D
SolidStateDetectorElementBase::localPositionOfCell(const Identifier & id) const
{
    SiCellId cellId = cellIdFromIdentifier(id);
    return m_design->localPositionOfCell(cellId);
}

int 
SolidStateDetectorElementBase::numberOfConnectedCells(const SiCellId cellId) const
{
    SiReadoutCellId readoutId = m_design->readoutIdOfCell(cellId);
    return m_design->numberOfConnectedCells(readoutId);
}

SiCellId 
SolidStateDetectorElementBase::connectedCell(const SiCellId cellId, int number) const
{
    SiReadoutCellId readoutId = m_design->readoutIdOfCell(cellId);
    return m_design->connectedCell(readoutId, number);
}

SiIntersect
SolidStateDetectorElementBase::inDetector(const Amg::Vector2D & localPosition,
                      double phiTol, double etaTol) const
{
    return m_design->inDetector(localPosition, phiTol, etaTol);
}

  
SiIntersect
SolidStateDetectorElementBase::inDetector(const HepGeom::Point3D<double> & globalPosition, double phiTol, double etaTol) const
{
    return m_design->inDetector(localPosition(globalPosition), phiTol, etaTol);
}

SiCellId
SolidStateDetectorElementBase::cellIdOfPosition(const Amg::Vector2D &localPosition) const
{
    return m_design->cellIdOfPosition(localPosition);
}

Identifier
SolidStateDetectorElementBase::identifierOfPosition(const Amg::Vector2D &localPosition) const
{
    SiCellId cellId = m_design->cellIdOfPosition(localPosition);
    return identifierFromCellId(cellId);
}

} // namespace InDetDD
