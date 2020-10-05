/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETANALYSISINTERFACES_IJETJVTEFFICIENCY_H_
#define JETANALYSISINTERFACES_IJETJVTEFFICIENCY_H_

// EDM include(s):
#include "xAODJet/JetContainer.h"

// Local include(s):
#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/ISystematicsTool.h"

// STL include(s):
#include <string>
#include <vector>
#include <map>

namespace CP {

const static SystematicVariation JvtEfficiencyUp("JET_JvtEfficiency__1up");
const static SystematicVariation JvtEfficiencyDown("JET_JvtEfficiency__1down");
const static SystematicVariation fJvtEfficiencyUp("JET_fJvtEfficiency__1up");
const static SystematicVariation fJvtEfficiencyDown("JET_fJvtEfficiency__1down");
const static SystematicVariation MVfJvtEfficiencyUp("JET_MVfJvtEfficiency__1up");
const static SystematicVariation MVfJvtEfficiencyDown("JET_MVfJvtEfficiency__1down");

class IJetJvtEfficiency : public virtual CP::ISystematicsTool {

    //Declare the interface that the class provides
    ASG_TOOL_INTERFACE( CP::IJetJvtEfficiency )

public:

    virtual ~IJetJvtEfficiency() {}

    //Fills sf with the Jvt efficiency scale factor. If the jet is out of the defined jet pt
    //range, or if the jet fails passesJvtCut, a value of 1 is returned.
    virtual CorrectionCode getEfficiencyScaleFactor(const xAOD::Jet& jet,float& sf ) = 0;

    //Fills sf with the Jvt inefficiency scale factor. If the jet is out of the defined jet pt
    //range, or if the jet fails passesJvtCut, a value of 1 is returned.
    virtual CorrectionCode getInefficiencyScaleFactor(const xAOD::Jet& jet,float& sf ) = 0;

    //Returns the Jvt scale factor as a decoration on the jet.
    virtual CorrectionCode applyEfficiencyScaleFactor( const xAOD::Jet& jet ) = 0;

    //Returns the Jvt scale factor as a decoration on the jet.
    virtual CorrectionCode applyInefficiencyScaleFactor( const xAOD::Jet& jet ) = 0;

    //Returns the jvt scale factors as decorations on the input jets and fills in the combined scale factor into sf
    virtual CorrectionCode applyAllEfficiencyScaleFactor(const xAOD::IParticleContainer *jets,float& sf) = 0;

    //Checks if the jet passes the jvt cut threshold. 
    //Uses maximum validity range by default, but there is a configurable parameter
    virtual bool passesJvtCut(const xAOD::Jet& jet) const = 0;

    //Checks if the jet lies in the pt and eta range where Jvt is valid.
    virtual bool isInRange(const xAOD::Jet& jet) const = 0;

    //returns actual jvtthreshold
    virtual float getJvtThresh() const = 0;

    //returns user-defined max pt if applicable (by default is the same as the max pt range of the histograms)
    virtual float getUserPtMax() const = 0;

    //Applies truth matching decoration to determine appropriate scale factor type -- call before extracting SFs
    virtual StatusCode tagTruth(const xAOD::IParticleContainer *jets,const xAOD::IParticleContainer *truthJets) = 0;

}; // class IJetJvtEfficiencyTool

} // namespace CP

#endif /* JETANALYSISINTERFACES_IJETJVTEFFICIENCY_H_ */
