/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CompetingSCT_ClustersOnTrack.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkSurfaces/Surface.h"
#include "GaudiKernel/MsgStream.h"
// InDet
#include "InDetCompetingRIOsOnTrack/CompetingSCT_ClustersOnTrack.h"
// std
#include <cmath>
#include <ostream>

// default constructor
InDet::CompetingSCT_ClustersOnTrack::CompetingSCT_ClustersOnTrack():
        Trk::CompetingRIOsOnTrack(),
        m_globalPosition{},
        m_containedChildRots(0)
        //
        {}

// copy constructor
InDet::CompetingSCT_ClustersOnTrack::CompetingSCT_ClustersOnTrack(const InDet::CompetingSCT_ClustersOnTrack& compROT) :
        Trk::CompetingRIOsOnTrack(compROT),
        m_globalPosition{},
        m_containedChildRots(0) {
    m_containedChildRots = new std::vector< const InDet::SCT_ClusterOnTrack* >;
    std::vector< const InDet::SCT_ClusterOnTrack* >::const_iterator rotIter = compROT.m_containedChildRots->begin();
    for (; rotIter!=compROT.m_containedChildRots->end(); ++rotIter) {
        m_containedChildRots->push_back((*rotIter)->clone());
    }
    if (compROT.m_globalPosition) {
        m_globalPosition.set(std::make_unique<const Amg::Vector3D>(*compROT.m_globalPosition));
    }
}

// explicit constructor
InDet::CompetingSCT_ClustersOnTrack::CompetingSCT_ClustersOnTrack(
    //const Trk::Surface* sf,
    std::vector<const InDet::SCT_ClusterOnTrack*>* childrots,
    std::vector<AssignmentProb>* assgnProb
    
):
Trk::CompetingRIOsOnTrack(assgnProb),
m_globalPosition{},
m_containedChildRots(childrots)
{
  // initialize local position and error matrix
  setLocalParametersAndErrorMatrix();

}

InDet::CompetingSCT_ClustersOnTrack& InDet::CompetingSCT_ClustersOnTrack::operator=(const InDet::CompetingSCT_ClustersOnTrack& compROT) {
    if (this!=&compROT) {
        // assingment operator of base class
        Trk::CompetingRIOsOnTrack::operator=(compROT);
        // clear rots
        clearChildRotVector();
        delete m_containedChildRots;
        if (m_globalPosition) delete m_globalPosition.release().get();
        m_containedChildRots = new std::vector<const InDet::SCT_ClusterOnTrack*>;

        std::vector<const InDet::SCT_ClusterOnTrack*>::const_iterator rotIter = compROT.m_containedChildRots->begin();
        for (; rotIter!=compROT.m_containedChildRots->end(); ++rotIter)
            m_containedChildRots->push_back((*rotIter)->clone());

        if (compROT.m_globalPosition) {
            m_globalPosition.set(std::make_unique<const Amg::Vector3D>(*compROT.m_globalPosition));
        }
    }
    return (*this);
}

InDet::CompetingSCT_ClustersOnTrack::~CompetingSCT_ClustersOnTrack() {
    clearChildRotVector();
    delete m_containedChildRots;
}

void InDet::CompetingSCT_ClustersOnTrack::clearChildRotVector() {
    std::vector<const InDet::SCT_ClusterOnTrack*>::const_iterator rotIter = m_containedChildRots->begin();
    for (; rotIter!=m_containedChildRots->end(); ++rotIter)
        delete (*rotIter);
}

MsgStream& InDet::CompetingSCT_ClustersOnTrack::dump( MsgStream& out ) const {
  using std::ios;
  out << "Trk::CompetingSCT_ClustersOnTrack with [" << numberOfContainedROTs() 
      << "] competing RIO_OnTrack objects" << std::endl;
  out << "  - " << (this->ROTsHaveCommonSurface(true)?"on common surface":
                    "over different surfaces") << "  (given prob>cut)" << std::endl;
  Trk::CompetingRIOsOnTrack::dump(out);
  out << "  - GlobalPosition        : ";
  if (not m_globalPosition) out << "null pointer"<<endmsg;
  else out << *m_globalPosition << endmsg;
  return out;
}

std::ostream& InDet::CompetingSCT_ClustersOnTrack::dump( std::ostream& out ) const {
  using std::ios;
  out << "Trk::CompetingSCT_ClustersOnTrack with [" << numberOfContainedROTs() 
      << "] competing RIO_OnTrack objects" << std::endl;
  out << "  - " << (this->ROTsHaveCommonSurface(true)?"on common surface":
                    "over different surfaces") << "  (given prob>cut)" << std::endl;
  Trk::CompetingRIOsOnTrack::dump(out);
  out << "  - GlobalPosition        : ";
  if (not m_globalPosition) out << "null pointer"<<std::endl;
  else out << *m_globalPosition << std::endl;
  return out;
}

// Have all the contained ROTs a common associated surface?
bool InDet::CompetingSCT_ClustersOnTrack::ROTsHaveCommonSurface(const bool) const {
    return true;
}

void InDet::CompetingSCT_ClustersOnTrack::setLocalParametersAndErrorMatrix() {

  if (ROTsHaveCommonSurface()) {
    int nNonVanishingROTs = ((assignmentProbability(0) > 1.e-10)? 1 : 0);
    for (unsigned int i=1; i<numberOfContainedROTs(); i++) {
      if (assignmentProbability(i) > 1.e-10) {
        nNonVanishingROTs+=1;
      }
    }
    const int& paramKey = rioOnTrack(0).localParameters().parameterKey();
    Amg::MatrixX meanWeightMatrix;
    meanWeightMatrix.setZero();
    const unsigned int& maxProbIndex = indexOfMaxAssignProb();
    if (nNonVanishingROTs > 1) {
      // more than one non-vanishing ROT: do the more complicated calculation
      if (paramKey==1) {
        Amg::MatrixX weight  = rioOnTrack(0).localCovariance().inverse();
        Amg::VectorX meanParams = assignmentProbability(0) * weight * rioOnTrack(0).localParameters();
	meanWeightMatrix = assignmentProbability(0) * weight;
	for (unsigned int i=1; i<numberOfContainedROTs(); i++) {
          weight     = rioOnTrack(i).localCovariance().inverse();
	  meanParams = meanParams + assignmentProbability(i) * weight * rioOnTrack(i).localParameters();
	  meanWeightMatrix += assignmentProbability(i) * weight;
	}
	
	meanParams =  meanWeightMatrix.inverse() * meanParams;
	Trk::DefinedParameter Par1(meanParams[Trk::loc1], Trk::loc1);
	m_localParams = Trk::LocalParameters(Par1);
      } else if (paramKey==3) {
	double meanTheta = 0.;
	double meanEigen1 = 0.;
	double meanEigen2 = 0.;
	double sumAssignProb = 0.;
	double meanMeasX = 0.;
	double meanMeasY = 0.;
            for (unsigned int i=0; i<numberOfContainedROTs(); i++) {
	      const Amg::MatrixX& covMat = rioOnTrack(i).localCovariance();
                //rho[i] = covMat[0][1]/sqrt(covMat[0][0]*covMat[1][1]);
                const double& assignProb = assignmentProbability(i);
                sumAssignProb += assignProb;
                const double trace = covMat(0,0) + covMat(1,1);
                const double det = covMat(0,0)*covMat(1,1)-covMat(0,1)*covMat(1,0);
                const double lambda1 = trace/2. - sqrt(trace*trace/4. - det);
                const double lambda2 = trace/2. + sqrt(trace*trace/4. - det);
                meanTheta += assignProb * 0.5 * atan(2.*covMat(0,1)/( covMat(0,0)-covMat(1,1)));
                meanEigen1 += assignProb / lambda1;
                meanEigen2 += assignProb / lambda2;
                // x coord has smaller uncert (use smaller eigenvalue):
                meanMeasX += assignProb/lambda1 * rioOnTrack(i).localParameters()[Trk::locX];
                meanMeasY += assignProb/lambda2 * rioOnTrack(i).localParameters()[Trk::locY];
            }
            meanTheta /= sumAssignProb;
            meanMeasX /= meanEigen1;
            meanMeasY /= meanEigen2;
            
            const double cosTheta = cos(meanTheta);
            const double sinTheta = sin(meanTheta);
            meanWeightMatrix = Amg::MatrixX(2,2);
            meanWeightMatrix.setZero(); 
            meanWeightMatrix(0,0) = cosTheta*cosTheta*meanEigen1 + sinTheta*sinTheta*meanEigen2;
            meanWeightMatrix(1,1) = cosTheta*cosTheta*meanEigen2 + sinTheta*sinTheta*meanEigen1;
            meanWeightMatrix(0,1) = cosTheta*sinTheta*meanEigen1 - cosTheta*sinTheta*meanEigen2;
            meanWeightMatrix(1,0) = meanWeightMatrix(0,1);
            
	          Amg::MatrixX weightMatrix_maxIndex = rioOnTrack(maxProbIndex).localCovariance().inverse();
            int orderInput = (weightMatrix_maxIndex(0,0) > weightMatrix_maxIndex(1,1)) ? -1 : 1;
            int orderOutput = (meanWeightMatrix(0,0) > meanWeightMatrix(1,1)) ? -1 : 1;
            if (orderInput*orderOutput < 0) {
                std::cout << "Trk::CompetingSCT_ClustersOnTrack: order of dimensions does not match!!!";
            }
            
            Trk::DefinedParameter Par1(meanMeasX, Trk::loc1);
            Trk::DefinedParameter Par2(meanMeasY, Trk::loc2);
            m_localParams = Trk::LocalParameters(Par1, Par2);
        } else {
            std::cout << "Trk::CompetingSCT_ClustersOnTrack: can not handle parameter key " << paramKey << std::endl;
        }
    } else {
        // not more than one non-vanishing ROT: use the ROT with highest prob
      meanWeightMatrix = assignmentProbability(maxProbIndex) * rioOnTrack(maxProbIndex).localCovariance().inverse();
      m_localParams = rioOnTrack(maxProbIndex).localParameters();
    }
    // limit weight values against values too close to 0, otherwise inversion will fail!
    if (meanWeightMatrix.trace() <= 1.0e-15) {
      for (int i=0; i<meanWeightMatrix.cols(); ++i)
        meanWeightMatrix(i,i) = 1.0e-10;
    }
    m_localCovariance = meanWeightMatrix.inverse();

  } else {
    
    std::cout << "Trk::CompetingRIOsOnTrack: can not handle ROTs in different surfaces without detector specific knowledge " << std::endl;
  }
}
