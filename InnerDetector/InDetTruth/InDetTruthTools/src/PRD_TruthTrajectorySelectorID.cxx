/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////
// PRD_TruthTrajectorySelectorID.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "InDetTruthTools/PRD_TruthTrajectorySelectorID.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "InDetPrepRawData/PixelCluster.h"
// DetectorDescription
#include "AtlasDetDescr/AtlasDetectorID.h"
// HepMC
#include "AtlasHepMC/GenParticle.h"


InDet::PRD_TruthTrajectorySelectorID::PRD_TruthTrajectorySelectorID(const std::string& t, const std::string& n, const IInterface* p) :
AthAlgTool(t,n,p)
{
    declareInterface<Trk::IPRD_TruthTrajectorySelector>(this);
}

StatusCode InDet::PRD_TruthTrajectorySelectorID::initialize() {
  ATH_MSG_VERBOSE("Initializing ...");
  StatusCode sc = detStore()->retrieve(m_atlasId, "AtlasID");
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not get AtlasID helper !" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;

}

StatusCode InDet::PRD_TruthTrajectorySelectorID::finalize() {
  ATH_MSG_VERBOSE("Finalizing ...");
  return StatusCode::SUCCESS;
}

ThreePointCircle::ThreePointCircle(const std::vector<Amg::Vector3D>& posVec) :
	m_translation(0), m_d0(0.), m_z0(0.), m_phi0(0.), m_eta(0.), m_pt(0.)
{
	if (posVec.size() <3)
		std::cout << "[WARNING] not enough points" << std::endl;
	else
		constructCircle(posVec[0],posVec[1],posVec[2]);	
}

ThreePointCircle::~ThreePointCircle()
{
    delete m_translation;
}

void ThreePointCircle::constructCircle(	const Amg::Vector3D& p1, 
					const Amg::Vector3D& p2,
					const Amg::Vector3D& p3) {
	
	// get the (x/y) coordinates of all points
  double translationX = m_translation ? -(m_translation->translation()).x() : 0.;
  double translationY = m_translation ? -(m_translation->translation()).y() : 0.;
	
  double bx = p1.x()+translationX; 
	double by = p1.y()+translationY;
  double cx = p2.x()+translationX;
	double cy = p2.y()+translationY;
  double dx = p3.x()+translationX;
	double dy = p3.y()+translationY;
	
  double temp = cx*cx+cy*cy;

	// prepare the matrix for the linear equation
  double bc = (bx*bx + by*by - temp)/2.0;
  double cd = (temp - dx*dx - dy*dy)/2.0;
  double det = (bx-cx)*(cy-dy)-(cx-dx)*(by-cy);
  //if (fabs(det) < 1.0e-6) {
  //std::cout << "[WARNING] Determinant is close to 0 " << std::endl;
  //}

	// invert the determinant
  det = 1./det;

	// x / y coordinate of the center
  double cRx= (bc*(cy-dy)-cd*(by-cy))*det;
  double cRy = ((bx-cx)*cd-(cx-dx)*bc)*det;

	// get the center, radius and momentum
  m_center = Amg::Vector2D(cRx,cRy); 
  m_radius = sqrt((cRx-bx)*(cRx-bx)+(cRy-by)*(cRy-by));
	m_pt = m_radius*(0.3*2.);

	// transverse parameters
	m_d0 = sqrt(cRx*cRx+cRy*cRy)-m_radius;

	// longitudinal parameters
	double theta = 1./3*(p1.theta() + p2.theta() + p3.theta());
	double r1 = p1.perp();
	double z1 = p1.z();
	double deltaz = r1/tan(theta);
	m_z0 = z1 - deltaz;
	// eta
	m_eta = 1./3*(p1.eta() + p2.eta() + p3.eta());
	
  // get the point of closest approach
	Amg::Vector2D poca = -m_d0*m_center.unit();

  // find the closest measurement
	Amg::Vector2D closestMeasurement = p1.perp() < p2.perp() ? 
	  Amg::Vector2D(p1.x(),p1.y()) : Amg::Vector2D(p2.x(),p2.y());
	if (p3.perp() < p1.perp()) closestMeasurement = Amg::Vector2D(p3.x(),p3.y());

  // phi0 is the most difficult one, two solution possible
	Amg::Vector2D directionOne(-cRy,cRx);
	Amg::Vector2D directionTwo(cRy,-cRx);

	// build difference vector closest measurement, poca
	Amg::Vector2D toFirstMeas(closestMeasurement-poca);

	// check with scalar product
	m_phi0 = directionOne.dot(toFirstMeas) > 0. ? directionOne.phi() : directionTwo.phi();

	// now sign d0
	Amg::Vector3D pocaDir(poca.x(),poca.y(),0.);
	Amg::Vector3D momDir(cos(m_phi0),sin(m_phi0),0.);
  double signD0 = pocaDir.cross(momDir).z() > 0. ? 1. : -1.;    
	m_d0 = signD0 * fabs(m_d0);
	    
	// update the center for the translation
	m_center += Amg::Vector2D(translationX,translationY);
	
}

bool InDet::PRD_TruthTrajectorySelectorID::pass( const Trk::PRD_TruthTrajectory & prdvec) const {

  std::vector<Amg::Vector3D> pos;
  std::vector<const Trk::PrepRawData* >::const_iterator prdIter  = prdvec.prds.begin();
  std::vector<const Trk::PrepRawData* >::const_iterator prdIterE = prdvec.prds.end();

  // look for 3 globalPositions for ThreePointCircle approximation
  for ( int i = 0; i  < 3 &&  prdIter != prdIterE; ++ prdIter ){
    if( m_atlasId->is_pixel((*prdIter)->identify()) ){
      const InDet::PixelCluster* pixclus=dynamic_cast<const InDet::PixelCluster*>(*prdIter);
      if (pixclus) {
        if(!pos.empty() && (pos.back()-pixclus->globalPosition()).squaredNorm() < 9)
          continue;
        pos.push_back (pixclus->globalPosition());
      }
    }
    else if( m_atlasId->is_sct((*prdIter)->identify()) ){
      const InDet::SCT_Cluster* sctclus=dynamic_cast<const InDet::SCT_Cluster*>(*prdIter);
      if (sctclus) { 
        if(!pos.empty() && (pos.back()-sctclus->globalPosition()).squaredNorm() < 9)
          continue;
        pos.push_back (sctclus->globalPosition());
      }
    }
    else if( m_atlasId->is_trt((*prdIter)->identify()) ){
      continue;
    }
    i++;
  }

  // only take trajectory if enough hits
  if( pos.size() != 3 ) return false;
  
  ThreePointCircle circle(pos);
  // trajectory only selected when within cuts
  if( ( abs(circle.d0()) <= 15 && (*prdvec.genParticle).barcode() <= 100000 )  )  return true;
  
  return false;
}
