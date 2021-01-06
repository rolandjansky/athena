/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "ReadoutGeometryBase/SolidStateDetectorElementBase.h"
#include "ReadoutGeometryBase/SiReadoutCellId.h"
#include "GeoModelUtilities/GeoAlignmentStore.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Vector/ThreeVector.h"

namespace InDetDD {
using Trk::distPhi;
using Trk::distEta;
using Trk::distDepth;

// Constructor with parameters:
  SolidStateDetectorElementBase::SolidStateDetectorElementBase(const Identifier& id,
                                       const DetectorDesign* design,
                                       const GeoVFullPhysVol* geophysvol,
                                       const SiCommonItems* commonItems,
                                       const GeoAlignmentStore* geoAlignStore) :
    TrkDetElementBase(geophysvol),
    m_id(id),
    m_design(design),
    m_commonItems(commonItems),
    m_surface(),
    m_geoAlignStore(geoAlignStore)
  {
    m_hitEta = m_design->etaAxis();
    m_hitPhi = m_design->phiAxis();
    m_hitDepth = m_design->depthAxis();

    commonConstructor();
  }

  // Destructor:
  SolidStateDetectorElementBase::~SolidStateDetectorElementBase()
  {
  }


  
  Trk::Surface&
  SolidStateDetectorElementBase::surface()
  {
    return *m_surface;
  }

  const Trk::Surface&
  SolidStateDetectorElementBase::surface() const
  {
    return *m_surface;
  }

  const GeoTrf::Transform3D&
  SolidStateDetectorElementBase::transformHit() const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }
    
      return m_transformHit;
  }

  const HepGeom::Transform3D&
  SolidStateDetectorElementBase::transformCLHEP() const
  {
    //stuff to get the CLHEP version of the local to global transform.
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    return m_transformCLHEP;
  }

  const Amg::Transform3D&
  SolidStateDetectorElementBase::transform() const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    return m_transform;
  }

  const HepGeom::Transform3D
  SolidStateDetectorElementBase::defTransformCLHEP() const
  {
    if (m_geoAlignStore) {
      const GeoTrf::Transform3D* ptrXf = m_geoAlignStore->getDefAbsPosition(getMaterialGeom());
      if (ptrXf) return Amg::EigenTransformToCLHEP(*ptrXf) * recoToHitTransform();
    }
    return Amg::EigenTransformToCLHEP(getMaterialGeom()->getDefAbsoluteTransform()) * recoToHitTransform();
  }  
   
  const Amg::Transform3D
  SolidStateDetectorElementBase::defTransform() const
  {
    HepGeom::Transform3D tmpTransform = defTransformCLHEP();
    return Amg::CLHEPTransformToEigen(tmpTransform);
  }

  const HepGeom::Transform3D
  SolidStateDetectorElementBase::recoToHitTransform() const
  {
    // Determine the reconstruction local (LocalPosition) to global transform.
    if (m_firstTime) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (m_firstTime) updateCache();
    }

    return recoToHitTransformImpl();
  }

  const HepGeom::Vector3D<double>&
  SolidStateDetectorElementBase::phiAxisCLHEP() const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    return m_phiAxisCLHEP;
  }

  const Amg::Vector3D&
  SolidStateDetectorElementBase::phiAxis() const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    return m_phiAxis;
  }

  const HepGeom::Vector3D<double>&
  SolidStateDetectorElementBase::etaAxisCLHEP() const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    return m_etaAxisCLHEP;
  }

  const Amg::Vector3D&
  SolidStateDetectorElementBase::etaAxis() const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    return m_etaAxis;
  }

  const Amg::Vector3D&
  SolidStateDetectorElementBase::normal() const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    return m_normal;
  }

  const Amg::Vector3D&
  SolidStateDetectorElementBase::center() const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    return m_center;
  }

  Amg::Vector2D
  SolidStateDetectorElementBase::hitLocalToLocal(double xEta, double xPhi) const  // Will change order to phi, eta
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    if (!m_etaDirection) xEta = -xEta;
    if (!m_phiDirection) xPhi = -xPhi;
    return Amg::Vector2D(xPhi, xEta);
  }

  HepGeom::Point3D<double>
  SolidStateDetectorElementBase::hitLocalToLocal3D(const HepGeom::Point3D<double>& hitPosition) const
  {
    // Equiv to transform().inverse * transformHit() * hitPosition
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

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
  void
  SolidStateDetectorElementBase::getEtaPhiRegion(double deltaZ, double& etaMin, double& etaMax, double& phiMin,
                                     double& phiMax, double& rz) const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    HepGeom::Point3D<double> corners[4];
    getCorners(corners);

    bool first = true;

    double phiOffset = 0.;

    for (int i = 0; i < 4; i++) {
      double etaMinPoint = 0.;
      double etaMaxPoint = 0.;
      double phiPoint = 0.;

      // Get the eta phi value for this corner.
      getEtaPhiPoint(corners[i], deltaZ, etaMinPoint, etaMaxPoint, phiPoint);

      if (first) { // Use the first point to initialize the min/max values.

        // Put phi in a range so that we are not near -180/+180 division.
        // Do this by adding an offset if phi > 90 CLHEP::deg or < -90 CLHEP::deg. 
        // This offset is later removed.
        if (phiPoint < -0.5 * M_PI) {
          phiOffset = -0.5 * M_PI;
        } else if (phiPoint > 0.5 * M_PI) {
          phiOffset = 0.5 * M_PI;
        }
        phiMin = phiMax = phiPoint - phiOffset;
        etaMin = etaMinPoint;
        etaMax = etaMaxPoint;
      } else {
        phiPoint -= phiOffset;
        // put phi back in -M_PI < phi < +M_PI range
        if (phiPoint < -M_PI) phiPoint += 2. * M_PI;
        if (phiPoint >  M_PI) phiPoint -= 2. * M_PI;
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
    if (phiMin >  M_PI) phiMin -= 2. * M_PI;
    if (phiMax < -M_PI) phiMax += 2. * M_PI;
    if (phiMax >  M_PI) phiMax -= 2. * M_PI;
    //does it make sense having this if the below is pure virtual for this base class?
    //Should this just be postponed to the derived classes?
    rz = get_rz();

  }

  const Trk::SurfaceBounds&
  SolidStateDetectorElementBase::bounds() const
  {
    return m_design->bounds();
  }

  SiIntersect
  SolidStateDetectorElementBase::inDetector(const Amg::Vector2D& localPosition,
                                double phiTol, double etaTol) const
  {
    return m_design->inDetector(localPosition, phiTol, etaTol);
  }

  SiIntersect
  SolidStateDetectorElementBase::inDetector(const HepGeom::Point3D<double>& globalPosition, double phiTol, double etaTol) const
  {
    return m_design->inDetector(localPosition(globalPosition), phiTol, etaTol);
  }

  Identifier
  SolidStateDetectorElementBase::identifierOfPosition(const Amg::Vector2D& localPosition) const
  {
    SiCellId cellId = m_design->cellIdOfPosition(localPosition);
    return identifierFromCellId(cellId);
  }

  SiCellId
  SolidStateDetectorElementBase::cellIdOfPosition(const Amg::Vector2D& localPosition) const
  {
    return m_design->cellIdOfPosition(localPosition);
  }

  Amg::Vector2D
  SolidStateDetectorElementBase::rawLocalPositionOfCell(const SiCellId& cellId) const
  {
    return m_design->localPositionOfCell(cellId);
  }

  Amg::Vector2D
  SolidStateDetectorElementBase::rawLocalPositionOfCell(const Identifier& id) const
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

  void
  SolidStateDetectorElementBase::commonConstructor()
  {
    //Do we need this at all in base class, or just in derived?
    if (!m_id.is_valid()) throw std::runtime_error("SolidStateDetectorElementBase: Invalid identifier");

    // Set surface
    m_surface = std::make_unique<Trk::PlaneSurface>(*this);
  }

  // update cache
  // This is supposed to be called inside a block like
  //
  // if (!m_cacheValid) {
  //   std::lock_guard<std::mutex> lock(m_mutex);
  //   if (!m_cacheValid) updateCache();
  // }
  //
  void
  SolidStateDetectorElementBase::updateCache() const
  {
    
    // use aligned transform if available
    const GeoTrf::Transform3D* ptrXf;
    GeoTrf::Transform3D geotrf;

    if (m_geoAlignStore){ 
      ptrXf = m_geoAlignStore->getAbsPosition(getMaterialGeom());
      if (ptrXf) {
	m_transformHit = (*ptrXf) * m_design->SiHitToGeoModel(); //need .linear()?
	geotrf = (*ptrXf);
      }
    }
    else{
      m_transformHit  = (getMaterialGeom()->getAbsoluteTransform() * m_design->SiHitToGeoModel()); //need .linear()?
      geotrf = getMaterialGeom()->getAbsoluteTransform();
    }
    
    const GeoTrf::Transform3D& geoTransform = geotrf;
    m_baseCacheValid = true;
    
    bool firstTimeBaseTmp = m_firstTimeBase;
    m_firstTimeBase = false;
    
    m_center = geoTransform * m_design->sensorCenter();
    
    //Is this needed outside e.g. ReadSiDetElements? Maybe candidate for future removal?
    m_centerCLHEP = HepGeom::Point3D<double>(m_center[0],m_center[1],m_center[2]);

    Amg::Vector3D centerGeoModel(0., 0., 0.);
    m_origin = geoTransform * centerGeoModel;
   
    //
    // Determine directions depth, eta and phi axis in reconstruction local frame
    // ie depth away from interaction point
    //    phi in direction of increasing phi
    //    eta in direction of increasing z in barrel, and increasing r in endcap
    //

    // depthAxis, phiAxis, and etaAxis are defined to be x,y,z respectively for all detectors for hit local frame.
    // depthAxis, phiAxis, and etaAxis are defined to be z,x,y respectively for all detectors for reco local frame.
    static const Amg::Vector3D localAxes[3] = {
        Amg::Vector3D(1.,0.,0.),
        Amg::Vector3D(0.,1.,0.),
        Amg::Vector3D(0.,0.,1.)
    };

    static const Amg::Vector3D & localRecoPhiAxis = localAxes[distPhi];     // Defined to be same as x axis
    static const Amg::Vector3D & localRecoEtaAxis = localAxes[distEta];     // Defined to be same as y axis
    static const Amg::Vector3D & localRecoDepthAxis = localAxes[distDepth]; // Defined to be same as z axis

    // We only need to calculate the rough orientation once.
    //For it to change would require extreme unrealistic misalignment changes.
    if (firstTimeBaseTmp) {
        // Determine the unit vectors in global frame
   
        const Amg::Vector3D &geoModelPhiAxis = localAxes[m_hitPhi];
        const Amg::Vector3D &geoModelEtaAxis = localAxes[m_hitEta];
        const Amg::Vector3D &geoModelDepthAxis = localAxes[m_hitDepth];
 
        Amg::Vector3D globalDepthAxis(geoTransform.linear() * geoModelDepthAxis);
        Amg::Vector3D globalPhiAxis(geoTransform.linear() * geoModelPhiAxis);
        Amg::Vector3D globalEtaAxis(geoTransform.linear() * geoModelEtaAxis);

        // unit radial vector
        Amg::Vector3D unitR(m_center.x(), m_center.y(), 0.);
        unitR.normalize();

        Amg::Vector3D nominalEta(0.0,0.0,1.0);
        Amg::Vector3D nominalNormal(0.0,0.0,0.0);
        Amg::Vector3D nominalPhi(-unitR.y(), unitR.x(), 0.0);

        // In Barrel like geometry, the etaAxis is along increasing z, and normal is in increasing radial direction.
        // In Endcap like geometry, the etaAxis is along increasing r, and normal is in decreasing z direction,
        // We base whether it is barrel like or endcap like by the orientation of the local z axis of the 
        // the element. This allows the use of endcap identifiers in a TB setup.


        nominalEta(2) = 1.0;
	m_barrelLike = true;

        if (std::abs(globalEtaAxis.dot(nominalEta)) < 0.5) { // Check that it is in roughly the right direction. Allowed not to be for ITK inclined/barrel ring modules
            m_barrelLike = false;
        }

        if (m_barrelLike) {
          nominalEta(2) = 1.0;
          nominalNormal =  unitR;
        } else { // endcap like
          nominalNormal(2) = -1.0;
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
	      ATH_MSG_DEBUG( "Unable to swap local depth axis.");
            }
        }
        if (std::abs(depthDir) < 0.5) { // Check that it is in roughly the right direction.
	  ATH_MSG_ERROR( "Orientation of local depth axis does not follow correct convention.");
            m_depthDirection = true; // Don't swap.
        }
    
        //
        // Phi axis
        //
        double phiDir = globalPhiAxis.dot(nominalPhi);
        m_phiDirection = true;
        if (phiDir < 0) {
            if (m_design->phiSymmetric()) {
                m_phiDirection = false;
            } else {
	      ATH_MSG_DEBUG("Unable to swap local xPhi axis.");
            }
        }
    
        if (std::abs(phiDir) < 0.5) { // Check that it is in roughly the right direction.
	  ATH_MSG_ERROR( "Orientation of local xPhi axis does not follow correct convention.");
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
	      ATH_MSG_DEBUG("Unable to swap local xEta axis.");
            }
        }
        if (std::abs(etaDir) < 0.5) { // Check that it is in roughly the right direction.
	  ATH_MSG_ERROR( "Orientation of local xEta axis does not follow correct convention.");
            m_etaDirection = true; // Don't swap
        }

    } // end if (firstTimeBaseTemp)
    
    m_transformCLHEP = Amg::EigenTransformToCLHEP(geoTransform) * recoToHitTransformImpl();
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
	  ATH_MSG_DEBUG( "Local frame is left-handed. (hitEtaDirection, hitPhiDirection, hitDepthDirection) = ("
			 << m_etaDirection <<", "
			 << m_phiDirection <<", "
			 << m_depthDirection <<")");
        }
    }
    #endif

    // Initialize various cached members, needs to be done here otherwise the necessary transforms are not yet initialized
    // The unit vectors
    m_normal = m_transform.linear() * localRecoDepthAxis;
  
    m_phiAxis = m_transform.linear() * localRecoPhiAxis;
    m_etaAxis = m_transform.linear() * localRecoEtaAxis;
    
    //Check where these are actually needed - candidates for removal?
    m_phiAxisCLHEP = HepGeom::Vector3D<double>(m_phiAxis[0],m_phiAxis[1],m_phiAxis[2]);
    m_etaAxisCLHEP = HepGeom::Vector3D<double>(m_etaAxis[0],m_etaAxis[1],m_etaAxis[2]);

    getExtent(m_minR, m_maxR, m_minZ, m_maxZ, m_minPhi, m_maxPhi);

    //Install the cache at the end
    m_cacheValid.store(true);
    if (m_firstTime) m_firstTime.store(false);
  }

  // Get min/max or r, z,and phi
  // helper method only to be used for the cache construction
  // i.e inside updateCache
  void
  SolidStateDetectorElementBase::getExtent(double& rMin, double& rMax,
                               double& zMin, double& zMax,
                               double& phiMin, double& phiMax) const
  {

     double radialShift = 0.;
     /*
    Deprecated method for specialized ITk DetElement with different global frame - to be replaced!
    const InDetDD::StripStereoAnnulusDesign* testDesign = dynamic_cast<const InDetDD::StripStereoAnnulusDesign*>(m_design);
    if (testDesign) radialShift = testDesign->localModuleCentreRadius();//additional radial shift for sensors centred on beamline
     */

     //NS this probably needs updates to get a shift from the m_design!!!!

    HepGeom::Point3D<double> corners[4];
    getCorners(corners);

    bool first = true;

    double phiOffset = 0.;

   
    const HepGeom::Transform3D rShift = HepGeom::TranslateX3D(radialShift);//in local frame, radius is x

    for (int i = 0; i < 4; i++) {

      //if (testDesign) corners[i].transform(rShift); see comment re ITk...

      // m_tranform is already there as  part of the cache construction
      // This method seems to be used only as a helper for updateCache
      HepGeom::Point3D<double> globalPoint = m_transformCLHEP * corners[i];

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
        } else if (phiPoint > 0.5 * M_PI) {
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
    if (phiMin >  M_PI) phiMin -= 2. * M_PI;
    if (phiMax < -M_PI) phiMax += 2. * M_PI;
    if (phiMax >  M_PI) phiMax -= 2. * M_PI;

  }

  void
  SolidStateDetectorElementBase::getCorners(HepGeom::Point3D<double>* corners) const
  {
    // This makes the assumption that the forward SCT detectors are orientated such that 
    // the positive etaAxis corresponds to the top of the detector where the width is largest.
    // This is currently always the case.
    // For the SCT barrel and pixel detectors minWidth and maxWidth are the same and so should 
    // work for all orientations.

    double tmpMinWidth = minWidth();
    double tmpMaxWidth = maxWidth();
    double tmpLength   = length();
  
    // Lower left
    corners[0][distPhi] = -0.5 * tmpMinWidth;
    corners[0][distEta] = -0.5 * tmpLength;
    corners[0][distDepth] = 0.;

    // Lower right
    corners[1][distPhi] =  0.5 * tmpMinWidth;
    corners[1][distEta] = -0.5 * tmpLength;
    corners[1][distDepth] = 0.;

    // Upper Right
    corners[2][distPhi] = 0.5 * tmpMaxWidth;
    corners[2][distEta] = 0.5 * tmpLength;
    corners[2][distDepth] = 0.;

    // Upper left
    corners[3][distPhi] = -0.5 * tmpMaxWidth;
    corners[3][distEta] =  0.5 * tmpLength;
    corners[3][distDepth] = 0.;
  }

  // Gets eta phi for a point given in local coordinates. deltaZ is specified to
  // account for the vertex spread. phi is independent of this vertex
  // spread. etaMax will correspond to zMin (-deltaZ) and etaMin will
  // correspond to zMax (+deltaZ).
  void
  SolidStateDetectorElementBase::getEtaPhiPoint(const HepGeom::Point3D<double>& point, double deltaZ,
                                    double& etaMin, double& etaMax, double& phi) const
  {
    // Get the point in global coordinates.
    HepGeom::Point3D<double> globalPoint = globalPosition(point);

    double r = globalPoint.perp();
    double z = globalPoint.z();
  
    double thetaMin = std::atan2(r,(z + deltaZ));
    etaMax = -std::log(tan(0.5 * thetaMin));
    double thetaMax = std::atan2(r,(z - deltaZ));
    etaMin = -std::log(tan(0.5 * thetaMax));

    phi = globalPoint.phi();
  }

  const HepGeom::Transform3D
  SolidStateDetectorElementBase::recoToHitTransformImpl() const
  {
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
      HepGeom::Vector3D<double>(1., 0., 0.),
      HepGeom::Vector3D<double>(0., 1., 0.),
      HepGeom::Vector3D<double>(0., 0., 1.)
    };

    const HepGeom::Transform3D recoToHit(HepGeom::Point3D<double>(0., 0., 0.),
                                         localAxes[distPhi],
                                         localAxes[distEta],
                                         HepGeom::Point3D<double>(0., 0., 0.),
                                         localAxes[m_hitPhi],
                                         localAxes[m_hitEta]);

    // Swap direction of axis as appropriate
    CLHEP::Hep3Vector scale(1., 1., 1.);
    if (!m_phiDirection)   scale[distPhi]   = -1.;
    if (!m_etaDirection)   scale[distEta]   = -1.;
    if (!m_depthDirection) scale[distDepth] = -1.;
    return recoToHit * HepGeom::Scale3D(scale[0], scale[1], scale[2]);
  }


} // namespace InDetDD
