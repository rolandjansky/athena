/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// author: jie.yu@cern.ch

#ifndef JETTRACKTOOLS_JETTRACKCENTEROFMASSASSOCIATION_H
#define JETTRACKTOOLS_JETTRACKCENTEROFMASSASSOCIATION_H

#include "ParticleJetTools/JetTrackAssociation.h"

#include <vector>
#include <string>

namespace Analysis {

  class JetTrackCenterOfMassAssociation : public JetTrackAssociation {
    ASG_TOOL_CLASS2(JetTrackCenterOfMassAssociation, JetTrackAssociation, IJetModifier)
    public:

      JetTrackCenterOfMassAssociation(const std::string& name);

      const std::vector<std::vector<const xAOD::TrackParticle*> >*
        match(xAOD::JetContainer&) const;

      inline double getAngleSize(const double& par_R) const{ 
        double result = acos(1-par_R*0.5);
        return result;
      }


    private:
      std::string m_inputTrackCollectionName;
      double m_trackMatchCone;
      double m_parentJetCone;
  };

}
#endif

