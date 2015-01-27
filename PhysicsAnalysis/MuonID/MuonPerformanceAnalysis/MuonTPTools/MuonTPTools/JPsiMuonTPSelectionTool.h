/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * JPsiMuonTPSelectionTool.h
 *
 *  Created on: Sep 01, 2014
 *      Author: Maximiliano Sioli
 */

#ifndef JPSIMUONTPSELECTIONTOOL_H_
#define JPSIMUONTPSELECTIONTOOL_H_

/// This is the implementation of the IMuonTPSelectionTool for the Z->mm tag and probe
/// efficiency measurement.
#include "MuonTPTools/MuonTPSelectionTool.h"
#include "AsgTools/AsgTool.h"

class JPsiMuonTPSelectionTool:
        virtual public IMuonTPSelectionTool,
        virtual public asg::AsgTool,
        public MuonTPSelectionTool{
      ASG_TOOL_CLASS(JPsiMuonTPSelectionTool, IMuonTPSelectionTool)
public:
      JPsiMuonTPSelectionTool(std::string myname);

      virtual StatusCode initialize();

      /// Select Probes
      ProbeContainer* selectProbes(const xAOD::MuonContainer*, const xAOD::IParticleContainer*) const;

      /// Get Efficiency Flag
      std::string efficiencyFlag() {return m_efficiencyFlag;}

      /// Check if Probe and Tag are the same object
      bool isTag(const xAOD::Muon* tag, const xAOD::IParticle* probe) const;

      // helper method to check if the probe is the right type
      bool isRightType(const xAOD::IParticle* probe) const;

    private:

      bool m_accept_sameCharge;
      bool m_accept_oppCharge;

      bool m_probe_ID_hits;

      double m_tagIsolation;
      double m_probeIsolation;

      bool m_IDProbe;
      bool m_CaloProbe;
      bool m_CBProbe;
      bool m_MSProbe;

      double m_deltaPhiCut;

};


#endif /* JPSIMUONTPSELECTIONTOOL_H_ */
