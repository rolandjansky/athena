// Dear Emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef MVA_CALIB_TOOL_H_
#define MVA_CALIB_TOOL_H_

#include <string>
#include <set>

#include "AsgTools/AsgTool.h"
#include "egammaMVACalib/IegammaMVATool.h"
#include "xAODTracking/TrackParticleFwd.h"

class egammaMVACalib;
class egammaMVATreeElectron;
class egammaMVATreePhoton;

class egammaMVATool : virtual public IegammaMVATool,
		      public asg::AsgTool
{
  // Create a proper constructor for Athena
  ASG_TOOL_CLASS( egammaMVATool, IegammaMVATool )
public:
  /** Constructor */
  egammaMVATool( const std::string& name );
  /** Generic destructor */
  ~egammaMVATool() {};

  /** @brief initialize method*/
  StatusCode initialize();

  /** @brief finalize method*/
  StatusCode finalize();

  /** Main execute. We need to calibrate the cluster.
      Use full egamma object instead of Type
      As we employ further variables than the ones present in the cluster
      This method needs to be valid also for reconstruction
  */

  StatusCode execute(xAOD::CaloCluster* cluster,const xAOD::Egamma* eg);
  StatusCode hltexecute(xAOD::CaloCluster* cluster, const std::string& egType);

  float getEnergy(const xAOD::CaloCluster* cluster, const xAOD::Egamma*);
  float getEnergy(const xAOD::CaloCluster* cluster, const xAOD::Electron*);
  float getEnergy(const xAOD::CaloCluster* cluster, const xAOD::Photon*);
  float getEnergy(const xAOD::CaloCluster* cluster, const std::string&);

private:
  std::set<std::string> guess_variables(const std::string& filename);
  bool getClusterVariables(const xAOD::CaloCluster*);
  bool getConversionVariables(const xAOD::Vertex*);

  //@brief Return the Pt at the first measurement point or at the perigee if not available
  float getPtAtFirstMeasurement(const xAOD::TrackParticle*) const;

private:

  egammaMVACalib *m_mvaElectron; /// MVA tool for electron
  egammaMVACalib *m_mvaPhoton; /// MVA tool for photon
  egammaMVATreeElectron* m_MVATreeElectron;  //!
  egammaMVATreePhoton* m_MVATreePhoton;    //!

  // here will go variables and stuff
  std::string m_folder; /// string with folder for weight files

  bool m_new_version;
  bool m_use_layer_corrected;

  ////////////////////////////////////////////////////////////
  // fields for internal tree
  double m_rawcl_Es0;
  double m_rawcl_Es1;
  double m_rawcl_Es2;
  double m_rawcl_Es3;
  double m_cl_eta;
  double m_cl_E;
  double m_ptconv;
  double m_pt1conv;
  double m_pt2conv;
  double m_cl_etaCalo;
  double m_cl_phiCalo;
  int m_convtrk1nPixHits;
  int m_convtrk1nSCTHits;
  int m_convtrk2nPixHits;
  int m_convtrk2nSCTHits;
  double m_Rconv;

  ////////////////////////////////////////////////////////////

};

#endif
