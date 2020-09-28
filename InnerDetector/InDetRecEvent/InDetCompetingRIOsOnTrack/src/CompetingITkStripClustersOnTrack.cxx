/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CompetingITkStripClustersOnTrack.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkSurfaces/Surface.h"
#include "GaudiKernel/MsgStream.h"
// InDet
#include "InDetCompetingRIOsOnTrack/CompetingITkStripClustersOnTrack.h"
// std
#include <math.h>

//#include <iostream.h>
// default constructor
InDet::CompetingITkStripClustersOnTrack::CompetingITkStripClustersOnTrack():
        Trk::CompetingRIOsOnTrack(),
        //m_associatedSurface(0),
        m_globalPosition(0),
        m_containedChildRots(0)
        //        m_assumedTrackParameters(0),
        //
        {}

// copy constructor
InDet::CompetingITkStripClustersOnTrack::CompetingITkStripClustersOnTrack(const InDet::CompetingITkStripClustersOnTrack& compROT) :
        Trk::CompetingRIOsOnTrack(compROT),
        m_globalPosition(compROT.m_globalPosition ? new Amg::Vector3D(*compROT.m_globalPosition) : 0),
        m_containedChildRots(0) {
    //        cout << "CompetingITkStripClustersOnTrack: in copy constructor" << endl;
    m_containedChildRots = new std::vector< const InDet::ITk_Strip_ClusterOnTrack* >;
    std::vector< const InDet::ITk_Strip_ClusterOnTrack* >::const_iterator rotIter = compROT.m_containedChildRots->begin();
    for (; rotIter!=compROT.m_containedChildRots->end(); ++rotIter) {
        m_containedChildRots->push_back((*rotIter)->clone());
        //        cout << "pushed back cloned ROT" << endl;
    }
}

// explicit constructor
InDet::CompetingITkStripClustersOnTrack::CompetingITkStripClustersOnTrack(
    //const Trk::Surface* sf,
    std::vector<const InDet::ITk_Strip_ClusterOnTrack*>* childrots,
    std::vector<AssignmentProb>* assgnProb
    //        const Trk::TrackParameters*  assumedTrkPars,
    //const Trk::LocalParameters* effecLocalPars,
    //const Trk::ErrorMatrix* effecLocalErrMat
    //int ROTsHaveComSrfc // meaning of the values are described in the definition of ROTsHaveCommonSurface()
):
Trk::CompetingRIOsOnTrack(assgnProb),
//m_associatedSurface(sf),
//m_localParameters(effecLocalPars);
//m_localErrorMatrix(effecLocalErrMat);
m_globalPosition(0),
m_containedChildRots(childrots)
//    m_assumedTrackParameters(assumedTrkPars),
{
  // initialize local position and error matrix
  setLocalParametersAndErrorMatrix();

}

InDet::CompetingITkStripClustersOnTrack& InDet::CompetingITkStripClustersOnTrack::operator=(const InDet::CompetingITkStripClustersOnTrack& compROT) {
    if (this!=&compROT) {
        // assingment operator of base class
        Trk::CompetingRIOsOnTrack::operator=(compROT);
        // clear rots
        clearChildRotVector();
        delete m_containedChildRots;
        delete m_globalPosition;
        //        delete m_assumedTrackParameters;
        // delete surface if not owned by detElement
        //if (m_associatedSurface && !m_associatedSurface->associatedDetectorElement())
        //    delete m_associatedSurface;
        m_containedChildRots = new std::vector<const InDet::ITk_Strip_ClusterOnTrack*>;
//         if (compROT.m_associatedSurface) {
//             // copy only if surface is not one owned by a detector Element
//             m_associatedSurface = (!compROT.m_associatedSurface->associatedDetectorElement()) ? compROT.m_associatedSurface->clone() : compROT.m_associatedSurface;
//         } else {
//             m_associatedSurface = 0;
//         }
        m_globalPosition     = compROT.m_globalPosition ? new Amg::Vector3D(*compROT.m_globalPosition) : 0;
        //        m_assumedTrackParameters     = compROT.m_assumedTrackParameters ? compROT.m_assumedTrackParameters->clone() : 0;
        //m_ROTsHaveCommonSurface     = compROT.m_ROTsHaveCommonSurface;
        std::vector<const InDet::ITk_Strip_ClusterOnTrack*>::const_iterator rotIter = compROT.m_containedChildRots->begin();
        for (; rotIter!=compROT.m_containedChildRots->end(); ++rotIter)
            m_containedChildRots->push_back((*rotIter)->clone());
    }
    return (*this);
}

InDet::CompetingITkStripClustersOnTrack::~CompetingITkStripClustersOnTrack() {
    // delete surface if not owned by detElement
//    if (m_associatedSurface && !m_associatedSurface->associatedDetectorElement())
//         delete m_associatedSurface;
    delete m_globalPosition;
    clearChildRotVector();
    delete m_containedChildRots;
    //    delete m_assumedTrackParameters;
}

void InDet::CompetingITkStripClustersOnTrack::clearChildRotVector() {
    std::vector<const InDet::ITk_Strip_ClusterOnTrack*>::const_iterator rotIter = m_containedChildRots->begin();
    for (; rotIter!=m_containedChildRots->end(); ++rotIter)
        delete (*rotIter);
}

MsgStream& InDet::CompetingITkStripClustersOnTrack::dump( MsgStream& out ) const {
  using std::ios;
  out << "Trk::CompetingITkStripClustersOnTrack with [" << numberOfContainedROTs() 
      << "] competing RIO_OnTrack objects" << std::endl;
  out << "  - " << (this->ROTsHaveCommonSurface(true)?"on common surface":
                    "over different surfaces") << "  (given prob>cut)" << std::endl;
  Trk::CompetingRIOsOnTrack::dump(out);
  out << "  - GlobalPosition        : ";
  if (m_globalPosition==NULL) out << "null pointer"<<endreq;
  else out << *m_globalPosition<<endreq;
  return out;
}

std::ostream& InDet::CompetingITkStripClustersOnTrack::dump( std::ostream& out ) const {
  using std::ios;
  out << "Trk::CompetingITkStripClustersOnTrack with [" << numberOfContainedROTs() 
      << "] competing RIO_OnTrack objects" << std::endl;
  out << "  - " << (this->ROTsHaveCommonSurface(true)?"on common surface":
                    "over different surfaces") << "  (given prob>cut)" << std::endl;
  Trk::CompetingRIOsOnTrack::dump(out);
  out << "  - GlobalPosition        : ";
  if (m_globalPosition==NULL) out << "null pointer"<<std::endl;
  else out << *m_globalPosition<<std::endl;
  return out;
}

// Have all the contained ROTs a common associated surface?
bool InDet::CompetingITkStripClustersOnTrack::ROTsHaveCommonSurface(const bool) const {
    return true;
}

void InDet::CompetingITkStripClustersOnTrack::setLocalParametersAndErrorMatrix() {

  if (ROTsHaveCommonSurface()) {
    //std::cout << "weight matrices: " << std::endl;
    //std::cout << "[0] " << rioOnTrack(0)->localErrorMatrix().weight() << std::endl;
    //CLHEP::HepSymMatrix meanWeightMatrix = assignmentProbability(0) * rioOnTrack(0)->localErrorMatrix().weight();
    int nNonVanishingROTs = ((assignmentProbability(0) > 1.e-10)? 1 : 0);
    for (unsigned int i=1; i<numberOfContainedROTs(); i++) {
      //meanWeightMatrix += assignmentProbability(i) * rioOnTrack(i)->localErrorMatrix().weight();
      //std::cout << "["<< i << "] " << rioOnTrack(i)->localErrorMatrix().weight() << std::endl;
      if (assignmentProbability(i) > 1.e-10) {
        nNonVanishingROTs+=1;
      }
    }
    const int& paramKey = rioOnTrack(0).localParameters().parameterKey();
    Amg::MatrixX meanWeightMatrix;
    meanWeightMatrix.setZero();
    //= AmgSymMatrix(rioOnTrack(0)->localErrorMatrix().weight().num_col());
    //CLHEP::HepSymMatrix meanWeightMatrix = CLHEP::HepSymMatrix(2, 0);
    const unsigned int& maxProbIndex = indexOfMaxAssignProb();
    // std::cout << "CompStrip - nNonVanishingROTs: " << nNonVanishingROTs << std::endl;
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
	
	//             std::cout << "sum params: " << meanParams << std::endl;
	meanParams =  meanWeightMatrix.inverse() * meanParams;
	//             std::cout << "mean params: " << meanParams << std::endl;
	Trk::DefinedParameter Par1(meanParams[Trk::loc1], Trk::loc1);
	m_localParams = Trk::LocalParameters(Par1);
      } else if (paramKey==3) {
	//double meanRho = 0.;
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
//                 std::cout << "["<<i<<"]: l1= " << lambda1 << " l2="<< lambda2 << " theta="<< atan2(2.*covMat[0][1], covMat[0][0]-covMat[1][1])/2. << std::endl;
                //meanTheta += assignProb * 0.5 * atan2(2.*covMat[0][1], covMat[0][0]-covMat[1][1]);
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
//             std::cout << "[mean]: l1= " << meanEigen1 << " l2="<< meanEigen2 << " theta="<< meanTheta << std::endl;
            
            //CLHEP::HepSymMatrix meanWeightMatrix = CLHEP::HepSymMatrix(2, 0);
            meanWeightMatrix(0,0) = cosTheta*cosTheta*meanEigen1 + sinTheta*sinTheta*meanEigen2;
            meanWeightMatrix(1,1) = cosTheta*cosTheta*meanEigen2 + sinTheta*sinTheta*meanEigen1;
            meanWeightMatrix(0,1) = cosTheta*sinTheta*meanEigen1 - cosTheta*sinTheta*meanEigen2;
            meanWeightMatrix(1,0) = meanWeightMatrix(0,1);
            
	    Amg::MatrixX weightMatrix_maxIndex = rioOnTrack(maxProbIndex).localCovariance().inverse();
            int orderInput = (weightMatrix_maxIndex(0,0) > weightMatrix_maxIndex(1,1)) ? -1 : 1;
            int orderOutput = (meanWeightMatrix(0,0) > meanWeightMatrix(1,1)) ? -1 : 1;
            if (orderInput*orderOutput < 0) {
                std::cout << "Trk::CompetingITkStripClustersOnTrack: order of dimensions does not match!!!";
            }
            
            //m_localParams[Trk::locX] = meanMeasX;
            //m_localParams[Trk::locY] = meanMeasY;
            Trk::DefinedParameter Par1(meanMeasX, Trk::loc1);
            Trk::DefinedParameter Par2(meanMeasY, Trk::loc2);
            m_localParams = Trk::LocalParameters(Par1, Par2);
        } else {
            std::cout << "Trk::CompetingITkStripClustersOnTrack: can not handle parameter key " << paramKey << std::endl;
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
//     std::cout << "mean weight: " << meanWeightMatrix << std::endl;
    m_localCovariance = meanWeightMatrix.inverse();
     //Trk::WeightMatrix(new Trk::WeightMatrix(meanWeightMatrix));
//     std::cout << "mean covariance: " << localErrorMatrix().covariance() << std::endl;

  } else {
    // --------------------------------------------------
    // Warning: effective localParams cannot be calculated when ROTs don't lie on the
    //          associated surface without detector specific knowledge.
    // --------------------------------------------------
    //return 0;
    std::cout << "Trk::CompetingRIOsOnTrack: can not handle ROTs in different surfaces without detector specific knowledge " << std::endl;
  }
}
