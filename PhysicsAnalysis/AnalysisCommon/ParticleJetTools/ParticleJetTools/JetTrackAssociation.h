/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// author: cpollard@cern.ch

#ifndef JETTRACKTOOLS_JETTRACKASSOCIATION_H
#define JETTRACKTOOLS_JETTRACKASSOCIATION_H

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetModifier.h"
#include "xAODTracking/TrackParticle.h"

#include <vector>
#include <string>

namespace Analysis {

    class JetTrackAssociation : public asg::AsgTool, public IJetModifier {
        ASG_TOOL_INTERFACE(JetTrackAssociation)
        ASG_TOOL_CLASS(JetTrackAssociation, IJetModifier)
        public:

            JetTrackAssociation(const std::string& name);

            int modify(xAOD::JetContainer& jets) const;

            // obvs to be provided by the deriving class
            virtual const std::vector<std::vector<const xAOD::TrackParticle*> >*
                match(xAOD::JetContainer&) const = 0;


        private:
            std::string m_outputCollectionName;
    };

}
#endif
