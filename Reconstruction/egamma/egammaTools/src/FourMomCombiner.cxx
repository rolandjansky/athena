/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FourMomCombiner.h"

#include "FourMom/EigenP5Jacobiand0z0PhiThetaqOverP2d0z0PhiEtaP.h"
#include "FourMom/ErrorMatrixEEtaPhiM.h"

#include "egammaUtils/ParameterDownWeighter.h"
#include "egammaUtils/WeightedMeanCalc.h"
#include "CaloUtils/CaloVertexedCluster.h"

//NEW xAOD INCLUDES.
#include "xAODEgamma/EgammaDefs.h"
#include "xAODEgamma/EgammaEnums.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODTracking/TrackParticle.h"

#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Electron.h"
#include "xAODCaloEvent/CaloCluster.h"

using namespace CLHEP;
using namespace CombinationFlags;
using namespace CombinationTypes;

using CLHEP::HepVector;

//Constructor.
FourMomCombiner::FourMomCombiner(const std::string& type,
				 const std::string& name,
				 const IInterface* parent) :
  AthAlgTool(type, name, parent),
  m_trkVector(5),
  m_trkMatrix(5,5),
  m_clusVector(5),
  m_clusMatrix(5,5),
  m_combVector(5),
  m_combMatrix(5,5),
  m_par1(0), 
  m_par2(0), 
  m_par3(0),
  m_combFlag(TRACK),
  m_DEFAULT_QOVERP_ERROR(1), 
  m_DEFAULT_MOMENTUM(0)
{
  m_trkVector.setZero();
  m_trkMatrix.setZero();
  m_clusVector.setZero();
  m_clusMatrix.setZero();
  m_combVector.setZero();
  m_combMatrix.setZero();
    
  // Declare interface.
  declareInterface<IFourMomCombiner>(this);

}

//Destructor.
FourMomCombiner::~FourMomCombiner() {}

StatusCode FourMomCombiner::initialize() {

  ATH_MSG_DEBUG(" Initializing FourMomCombiner");


  return StatusCode::SUCCESS;

}

StatusCode FourMomCombiner::ClusterExecute(xAOD::Egamma *eg,
					   const Amg::Vector3D& vtx)

{

  //A few security checks.                                                                                                                                                         
  if (eg == 0) {
    ATH_MSG_DEBUG("No Egamma Object");
    return StatusCode::FAILURE;
  }

  //Fill only cluster, since there are no track parameters.
  fillClusterParameters(eg, vtx);

  return StatusCode::SUCCESS;

}

StatusCode FourMomCombiner::TrackClusterExecute(xAOD::Egamma *eg,
						unsigned int index, 
						bool hasSi)

{

  // This should only be called if it's a conversion or an electron;
  // the other 

  //A few security checks.
  if (!eg) {
    ATH_MSG_ERROR("No Egamma Object");
    return StatusCode::FAILURE;
  }

  //Load track parameters from egamma object.
  fillTrackVectorElements(eg,index);
  
  int charge(0);
  if (isElectron(eg)) {
    xAOD::Electron* el =static_cast<xAOD::Electron*> (eg);
    charge = el->trackParticle(index)->charge();
  }
  
  //Fill matrix here.
  fillTrackMatrixElements(eg,index,charge);

  //Decide how to combine first.
  MakeCombinationDecisionElectron(eg, index, hasSi);

  //Perform combination.
  StatusCode sc = execute();

  if (sc.isFailure()) {
    ATH_MSG_WARNING("FourMomCombiner: execute did not return success!");
    return sc; 
  }

  return StatusCode::SUCCESS;

}

StatusCode FourMomCombiner::execute() {

  if (m_combType==KALMAN) {
    ATH_MSG_DEBUG("Performing E-p combination using Kalman update");
    setFromKalmanUpdate();
  } else {

    ATH_MSG_DEBUG("Performing E-p combination using full covariance matrix method");

    //enum parameters.
    int is1  = m_par1;
    int is2  = m_par2;
    int is3  = m_par3;
    
    int flag = m_combFlag;
    
    //Get cluster and track vectors & matrices
    EigenVectorAndMatrix clusVM(m_clusVector, m_clusMatrix);
    EigenVectorAndMatrix trkVM (m_trkVector,  m_trkMatrix);
    
    //Downweight parameter(s): 
    switch (flag) {
    case TRACK:
      ParameterDownWeighter::getDownWeightedVM(clusVM, trkVM, is1);
      ParameterDownWeighter::getDownWeightedVM(clusVM, trkVM, is2);
      break;
    case TRACKONLY:
      ParameterDownWeighter::getDownWeightedVM(clusVM, trkVM, is1);
      ParameterDownWeighter::getDownWeightedVM(clusVM, trkVM, is2);
      ParameterDownWeighter::getDownWeightedVM(clusVM, trkVM, is3);
      break;
    case TRACK_TRT:
      ParameterDownWeighter::getDownWeightedVM(clusVM, trkVM, is1);
      ParameterDownWeighter::getDownWeightedVM(trkVM, clusVM, is2);
      break;
    case CLUSTER:
      ParameterDownWeighter::getDownWeightedVM(clusVM, trkVM, is1);
      ParameterDownWeighter::getDownWeightedVM(clusVM, trkVM, is2);
      ParameterDownWeighter::getDownWeightedVM(trkVM, clusVM, is3);
      break;
    case CLUSTER_TRT:
      ParameterDownWeighter::getDownWeightedVM(clusVM, trkVM, is1);
      ParameterDownWeighter::getDownWeightedVM(trkVM, clusVM, is2);
      ParameterDownWeighter::getDownWeightedVM(trkVM, clusVM, is3);
      break;
    }
    
    EigenVectorAndMatrix wVM = WeightedMeanCalc::getWeightedMean(clusVM, trkVM);  
    
    //Set the combined parameters in the object.
    m_combVector = wVM.getVector();
    m_combMatrix = wVM.getMatrix();

  }
  return StatusCode::SUCCESS;
}

StatusCode FourMomCombiner::finalize() {

  return StatusCode::SUCCESS;
  
}

void FourMomCombiner::setDownweightParameters(DownWeightPar s1, DownWeightPar s2, DownWeightPar s3, CombinationFlag flag) {        

  //Set the enum parameters.
  m_par1 = s1; m_par2 = s2; m_par3 = s3;
  m_combFlag = flag;

}

//Alternate E-p combination method: Kalman update of track parameters
//with cluster energy measurement.
//(method provided by A. Morley).
void FourMomCombiner::setFromKalmanUpdate() {

  //Define dimensions for ease of use.
  int first(1), second(5);

  EigenVectorAndMatrix clusVM(m_clusVector,m_clusMatrix);
  EigenVectorAndMatrix trkVM(m_trkVector, m_trkMatrix);

  //Define track parameters, covariance matrix.
  Amg::VectorX    pj(m_trkVector);
  Amg::MatrixX    Cj(m_trkMatrix);

  //Set a (first) size vector for cluster parameters to use in update, and (first) x (first) size matrix.
  //Currently just using cluster energy to update. Could also use eta, phi, in principle.
  Amg::VectorX m(first); 
  m.setZero();
  m[0] = (m_clusVector)[4];
  Amg::MatrixX V(first,first); 
  V.setZero();
  V(0,0) = m_clusMatrix(4,4);

  //Define observation model matrix, H.
  Amg::MatrixX H(first,second);
  H.setZero();
  H(0,second) = 1.;

  //Build measurement residual.
  Amg::VectorX tmp1(m); tmp1 += H*pj;

  //Define results objects.
  Amg::VectorX    p(second);
  p.setZero();
  Amg::MatrixX    C(second,second);
  C.setZero();

  //Intermediate results.
  Amg::MatrixX Rinv(first,first);
  Rinv.setZero();
  Amg::MatrixX Rtmp(first,first);
  Rtmp.setZero();
  Amg::MatrixX K(second,first);
  K.setZero();
  Amg::MatrixX Ctmp(second,second);
  Ctmp.setZero();
  Amg::MatrixX tmp2(second,first);
  tmp2.setZero();
  Amg::MatrixX HT(second,first);
  HT.setZero();

  //Perform the update.
  tmp2 = Cj * H.transpose();
  Rtmp = H * tmp2;
  Rinv = (V + Rtmp).inverse();

  K = tmp2 * Rinv;

  //Pass updated measurement & covariance.
  p = pj; p += K * tmp1;

  Ctmp = K * tmp2.transpose();
  //C.assign(Cj - Ctmp);
  C = Cj - Ctmp;

  // If track momentum is > 2sigma from the calo energy measurment, 
  //use simple model instead.
  if( tmp1[0]*tmp1[0] > 4.0 * ( Cj(4,4) + V(0,0)) ) {
    
    C(0,4) = 0;
    C(1,4) = 0;
    C(2,4) = 0;
    C(3,4) = 0;
    C(4,0) = 0;
    C(4,1) = 0;
    C(4,2) = 0;
    C(4,3) = 0;    
    C(4,4) = V(0,0);
    
    p[4]  = m[0];
    
  }

  //Set the updated parameters, matrix.
  m_combVector = p; m_combMatrix = C;

}

//NOTE: All we need to do here is rearrange the elements of the matrix we already
//have in the (d0, z0, phi, eta, P) representation to make the (E, eta, phi, M)
//representation.
const AmgMatrix(4,4) FourMomCombiner::get4x4CombinedErrorMatrix() const{

  AmgMatrix(4,4) combmatrix;
  combmatrix.setZero();

  combmatrix(0,0) = m_combMatrix(4,4);
  combmatrix(1,1) = m_combMatrix(3,3);
  combmatrix(2,2) = m_combMatrix(2,2);

  combmatrix.fillSymmetric(0,1,m_combMatrix(4,3));
  combmatrix.fillSymmetric(0,2,m_combMatrix(4,2));
  combmatrix.fillSymmetric(1,2,m_combMatrix(3,2));


  return combmatrix;

}

//Returns the track matrix, minus any correlations to energy, and sigma(Eclus,Eclus)
//in place of momentum.

const AmgMatrix(4,4) FourMomCombiner::get4x4ClusterErrorMatrix() const{

  AmgMatrix(4,4) clustermatrix;
  clustermatrix.setZero();

  clustermatrix(0,0) = m_clusMatrix(4,4);
  clustermatrix(1,1) = m_clusMatrix(3,3);
  clustermatrix(2,2) = m_clusMatrix(2,2);

  return clustermatrix;

}

//============================================
//Routines to fill cluster, track parameters.


//bool FourMomCombiner::fillTrackVectorElements(const Trk::Perigee* perigee) {
bool FourMomCombiner::fillTrackVectorElements(xAOD::Egamma *eg, int index)
{

  if (eg == 0) return false;

  xAOD::DefiningParameters_t perigee;
  perigee.setZero();

  if (isElectron(eg)) {
    xAOD::Electron* el = dynamic_cast<xAOD::Electron*> (eg);
    if (el) {
      perigee            = el->trackParticle(index)->definingParameters();
      m_trkVector[3]     = el->trackParticle(index)->eta();
    }
  } else {
    return false;
  }

  //Load track parameters from egamma object.
  double momentum(0.);
  m_trkVector[0] = perigee(0);
  m_trkVector[1] = perigee(1);
  m_trkVector[2] = perigee(2);
  //m_trkVector[3] = perigee->eta();
  const double qOverP = perigee(4);
  if (qOverP != 0) {
    momentum = fabs(1./ qOverP);
  } else {
    momentum = m_DEFAULT_MOMENTUM;
  }
  m_trkVector[4] = momentum;

  return true;
  
}

//First make typical covariance matrix, then use Jacobian
//to transform to the relevant representation (d0, z0, phi, eta, P).
//bool FourMomCombiner::fillTrackMatrixElements(const Trk::Perigee* perigee, int charge) {
bool FourMomCombiner::fillTrackMatrixElements(xAOD::Egamma *eg, int index, int charge)
{

  ATH_MSG_DEBUG("fillTrackMatrixElements: Getting covariance matrix ...");

  if (eg == 0) return false;

  xAOD::ParametersCovMatrix_t covmat;
  covmat.setZero();

  xAOD::DefiningParameters_t  perigee;
  perigee.setZero();

  if (isElectron(eg)) {
    xAOD::Electron* el = dynamic_cast<xAOD::Electron*> (eg);
    covmat  = el->trackParticle(index)->definingParametersCovMatrix();
    perigee = el->trackParticle(index)->definingParameters();
  } else {
    return false;
  }

  //Get initial matrix in (theta, q/P) representation.
  //const AmgSymMatrix(5) *covmat(perigee->covariance());
  AmgSymMatrix(5)  emnew;
  emnew.setZero();

  ATH_MSG_DEBUG("fillTrackMatrixElements: Got covariance matrix ...");

  //Case of old single conversion.
  if ( covmat.rows() == 3) {
    emnew(0,0) = emnew(1,1) = 10e10;
    emnew(2,2) = covmat(0,0);
    emnew(3,3) = covmat(1,1);
    emnew(4,4) = covmat(2,2);

    emnew.fillSymmetric(2,3,covmat(0,1));
    emnew.fillSymmetric(2,4,covmat(0,2));
    emnew.fillSymmetric(3,4,covmat(1,2));
  } else {
    emnew = covmat;
  }

  //Transform it to the one we want.
  AmgMatrix(5,5) m;
  m.setZero();
  ATH_MSG_DEBUG("fillTrackMatrixElements: Creating Jacobian ...");

  EigenP5Jacobiand0z0PhiThetaqOverP2d0z0PhiEtaP J(perigee(3),
						  charge, 
						  m_trkVector[4]);

  ATH_MSG_DEBUG("fillTrackMatrixElements: Got Jacobian, performing similarity transform ...");

  //m = emnew.similarity(J);
  m = J * (emnew * J.transpose());
  m_trkMatrix = m;

  ATH_MSG_DEBUG("From fillTrackMatrixElements: " << m_trkMatrix);

  return true;
}

bool FourMomCombiner::fillClusterParameters(const xAOD::Egamma* eg)
{

  if (!eg) return false;

  const xAOD::CaloCluster* aCluster = eg->caloCluster();
  if (aCluster == 0) return false;

  //if (forcePhoton) ATH_MSG_DEBUG("Treating all objects as photons");
  
  //Set cluster parameter elements.
  const double clusterPhi    = aCluster->phi();
  const double clusterEta    = aCluster->eta();
  const double clusterEnergy = aCluster->e();

  //ATH_MSG_DEBUG("Cluster pointing eta:" << eg->detailValue(egammaParameters::etap));
  
  m_clusVector[2] = clusterPhi;
  m_clusVector[3] = caloEta(eg, clusterEta);
  m_clusVector[4] = clusterEnergy;
  
  ATH_MSG_DEBUG("From fillClusterParameters: " << m_clusVector);

  //Set relevant cluster matrix elements.
  const double clusterEtaError    = getClusterEtaError(eg);
  const double clusterPhiError    = getClusterPhiError(eg);
  const double clusterEnergyError = getClusterEnergyError(eg);
  
  m_clusMatrix(0,0) = m_clusMatrix(1,1) = 1E11;
  m_clusMatrix(2,2) = clusterPhiError * clusterPhiError;
  m_clusMatrix(3,3) = clusterEtaError * clusterEtaError;
  m_clusMatrix(4,4) = clusterEnergyError * clusterEnergyError;

  ATH_MSG_DEBUG("From fillClusterParameters: " << m_clusMatrix);
   
  return true;
   
}


bool FourMomCombiner::fillClusterParameters(const xAOD::Egamma* eg, 
					    const Amg::Vector3D& vtx)
{

  if (!eg) return false;

  const xAOD::CaloCluster* aCluster = eg->caloCluster();
  if (aCluster == 0) return false;

  //if (forcePhoton) ATH_MSG_DEBUG("Treating all objects as photons");

  const Amg::Vector3D vect(vtx);

  //CaloClusterChangeVertexState mychange(aCluster, &vect);
  const xAOD::CaloVertexedCluster vtxCluster(*aCluster,xAOD::CaloCluster::CALIBRATED,vtx);
  
  //Set cluster parameter elements.
  const double clusterPhi    = vtxCluster.phi();
  const double clusterEta    = vtxCluster.eta();
  const double clusterEnergy = vtxCluster.e();

  //ATH_MSG_DEBUG("Cluster pointing eta:" << eg->detailValue(egammaParameters::etap));
  
  m_clusVector[2] = clusterPhi;
  m_clusVector[3] = caloEta(eg, clusterEta);
  m_clusVector[4] = clusterEnergy;
  
  ATH_MSG_DEBUG("From fillClusterParameters: " << m_clusVector);

  //Set relevant cluster matrix elements.
  const double clusterEtaError    = getClusterEtaError(eg);
  const double clusterPhiError    = getClusterPhiError(eg);
  const double clusterEnergyError = getClusterEnergyError(eg);
  
  m_clusMatrix(0,0) = m_clusMatrix(1,1) = 1E11;
  m_clusMatrix(2,2) = clusterPhiError * clusterPhiError;
  m_clusMatrix(3,3) = clusterEtaError * clusterEtaError;
  m_clusMatrix(4,4) = clusterEnergyError * clusterEnergyError;

  ATH_MSG_DEBUG("From fillClusterParameters: " << m_clusMatrix);
   
  return true;
   
}

//============================================
//Routines to calculate cluster errors.
double FourMomCombiner::getClusterPhiError(const xAOD::Egamma* eg) const{

  if (!eg) return 0.;
  
  const xAOD::CaloCluster* aCluster = eg->caloCluster();
  if (aCluster == 0 /*|| pars == 0*/) return 1E11;
  return 1e-3;
  //return pars->getPhiMatrix(tp).getError(caloEta(eg, aCluster->eta()), aCluster->e());
}

double FourMomCombiner::getClusterEtaError(const xAOD::Egamma* eg) const {
  if (!eg) return 0.;

  const xAOD::CaloCluster* aCluster = eg->caloCluster();
  double eta = (aCluster) ?  caloEta(eg, aCluster->eta()) : 0;
  if (fabs(eta) > 8) eta = 8.0;
  if (1 /*pars == 0*/) {

    const double clusterEnergyGeV = 100.0; //GeV - just use 100 as the default
    const double sigma_theta = 0.07 /sqrt(clusterEnergyGeV);
    const double theta = 2.*atan(exp(eta));
    return ( fabs(sigma_theta/sin(theta)) );
  }
  return 0.;
}


double FourMomCombiner::getClusterEnergyError(const xAOD::Egamma* eg) const{

  if (!eg) return 0.;
  //if (forcePhoton) ATH_MSG_DEBUG("Treating all objects as photons");

  const xAOD::CaloCluster* aCluster = eg->caloCluster();

  if (aCluster == 0 /* || pars == 0*/) return 1e-3; // In merging, use the cluster energy.
  
  double cl_err = 1E10;

  return cl_err;
} 

double FourMomCombiner::caloEta(const xAOD::Egamma* eg, double clusterEta) const {

  if (!eg) return clusterEta;

  //NOTE: Currently don't have access to pointing information in xAOD???
  const double etaPointing(100.);// = eg->detailValue(egammaParameters::etap);
  if ( fabs(etaPointing - clusterEta ) < 0.15 ) {
    return etaPointing;
  } else {
    return clusterEta;
  }
}

bool FourMomCombiner::isElectron(const xAOD::Egamma* eg) const {
  return (xAOD::EgammaHelpers::isElectron(eg));
}

//Adaptation of routine from EMFourMomBuilder.cxx.
void FourMomCombiner::MakeCombinationDecisionElectron(xAOD::Egamma *eg, 
						      unsigned int index, 
						      bool hasSi){

  xAOD::Electron* el = dynamic_cast<xAOD::Electron*> (eg);
  const xAOD::TrackParticle *trParticle = el->trackParticle(index);
  float trkParPt=trParticle->pt();
  ATH_MSG_DEBUG("Using CLUSTER flag (no combination) for now ");
  if (hasSi && trkParPt > 0){
    setDownweightParameters(PHI, ETA, PMOM, CLUSTER);
  }
  else{
    setDownweightParameters(PHI, ETA, PMOM, CLUSTER_TRT);
  }

  return;
}
