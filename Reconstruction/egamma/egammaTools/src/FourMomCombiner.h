/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATOOLS_FOURMOMCOMBINER_H
#define EGAMMATOOLS_FOURMOMCOMBINER_H

//This class will hold all the variables relevant to the actual combination,
//as well as eventually performing it. This will be the most modular, fundamental element of the
//routine, capable (in principle) of being used outside of EMFourMomBuilder.cxx.

#include "egammaInterfaces/IFourMomCombiner.h"

// XAOD INCLUDES:
#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODEgamma/EgammaFwd.h"
#include "EventPrimitives/EventPrimitives.h"


namespace CombinationFlags {
  enum DownWeightPar   {INVALID=999, D0=0, Z0=1, PHI=2, ETA=3, PMOM=4};
  enum CombinationFlag {TRACK, TRACK_TRT, CLUSTER, CLUSTER_TRT,TRACKONLY};
}


class FourMomCombiner : public AthAlgTool, virtual public IFourMomCombiner {

 public:

  //Constructor/destructor.
  FourMomCombiner(const std::string& type,
		  const std::string& name,
		  const IInterface* parent);

  ~FourMomCombiner();

  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID();

  //Tool standard routines.
  StatusCode initialize();

  StatusCode ClusterExecute     (xAOD::Egamma*, 
				 const Amg::Vector3D& vtx);

  StatusCode TrackClusterExecute(xAOD::Egamma*, 
				 unsigned int index, bool hasSi);
  StatusCode finalize();
  
  const Amg::VectorX &getClusterVector()  const {return m_clusVector;}
  const Amg::VectorX &getCombinedVector() const {return m_combVector;}

  const Amg::MatrixX &getClusterMatrix()  const  {return m_clusMatrix;}
  const Amg::MatrixX &getCombinedMatrix() const  {return m_combMatrix;}

  //Returns matrices in (E, eta, phi, M) representation.
  const AmgMatrix(4,4) get4x4ClusterErrorMatrix() const;
  const AmgMatrix(4,4) get4x4CombinedErrorMatrix() const;

 private:

  //do the combination
  StatusCode execute();

  //Routine to do decision making (ported over from EMFourMomBuilder.cxx).
  void MakeCombinationDecisionElectron(xAOD::Egamma*, unsigned int, bool hasSi);

  //Routines to perform combination.
  void setDownweightParameters(CombinationFlags::DownWeightPar s1, 
			       CombinationFlags::DownWeightPar s2, 
			       CombinationFlags::DownWeightPar s3, 
			       CombinationFlags::CombinationFlag flag);

  bool   fillClusterParameters (const xAOD::Egamma*, const Amg::Vector3D&);
  bool   fillClusterParameters (const xAOD::Egamma*);
  bool   fillTrackVectorElements        (xAOD::Egamma*, int index);
  bool   fillTrackMatrixElements        (xAOD::Egamma*, int index, int charge);

  //Track/cluster parameter vectors and matrices.
  Amg::VectorX    m_trkVector;
  Amg::MatrixX    m_trkMatrix;
  Amg::VectorX    m_clusVector;
  Amg::MatrixX    m_clusMatrix;

  //Combined parameters.
  Amg::VectorX    m_combVector;
  Amg::MatrixX    m_combMatrix;
  
  //Downweighting/combination parameters.
  int m_par1, m_par2, m_par3;
  int m_combFlag;

  Gaudi::Property<int> m_combType {this, 
      "combType", CombinationTypes::COVMAT,
      "Decides whether to do full covariance method, or single-shot Kalman update"};

  double m_DEFAULT_QOVERP_ERROR;
  double m_DEFAULT_MOMENTUM;

  // Get the error on cluster energy, eta and phi.
  double getClusterEnergyError (const xAOD::Egamma*) const;
  double getClusterEtaError    (const xAOD::Egamma*) const;
  double getClusterPhiError    (const xAOD::Egamma*) const;

  //Assorted routines.
  bool   isElectron (const xAOD::Egamma*)  const;
  double caloEta    (const xAOD::Egamma*, double clusterEta) const;

  void setFromKalmanUpdate();

  double computeEPsigma(double energy, double energyError, 
			double momentum, double momentumError) const 
  {
    if (momentumError < 0) {
      // this is the uninitialzed case, the -999999
      return 9999; 
    } else {
      return (sqrt(((energy - momentum)*(energy - momentum))
		   / ((energyError*energyError) + (momentumError*momentumError))));
    }
  }  
};

#endif
