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
  ~egammaMVATool();

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
  StatusCode execute(xAOD::CaloCluster* cluster, const xAOD::EgammaParameters::EgammaType egType);
  StatusCode hltexecute(xAOD::CaloCluster* cluster, const std::string& egType);

  float getEnergy(const xAOD::Egamma* eg);
  float getEnergy(const xAOD::CaloCluster* cluster, const xAOD::Egamma*);
  float getEnergy(const xAOD::CaloCluster* cluster, const xAOD::EgammaParameters::EgammaType egType);

  float getEnergy(const xAOD::CaloCluster* cluster, const xAOD::Electron*);
  float getEnergy(const xAOD::CaloCluster* cluster, const xAOD::Photon*);
  float getEnergy(const xAOD::CaloCluster* cluster, const std::string&);

private:
  std::set<std::string> guess_variables(const std::string& filename);

  std::unique_ptr<egammaMVACalib> m_mvaElectron; /// MVA tool for electron
  std::unique_ptr<egammaMVACalib> m_mvaPhoton; /// MVA tool for photon
  std::unique_ptr<egammaMVATreeElectron> m_MVATreeElectron;  //!
  std::unique_ptr<egammaMVATreePhoton> m_MVATreePhoton;    //!

  std::string m_folder; /// string with folder for weight files

  bool m_use_layer_corrected;

};

#endif
