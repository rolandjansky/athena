/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IJETJVTEFFICIENCY_H_
#define IJETJVTEFFICIENCY_H_

// EDM include(s):
#include "xAODJet/Jet.h"

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
    virtual CorrectionCode applyAllEfficiencyScaleFactor(const xAOD::IParticleContainer *jets,float& sf,bool doJvtSelection) = 0;

    //Adds a random dropping decoration to single jet.
    virtual CorrectionCode applyRandomDropping( const xAOD::Jet& jet ) = 0;

    //Adds a random dropping decoration to every jet.
    virtual CorrectionCode applyAllRandomDropping( const xAOD::IParticleContainer *jets) = 0;

    //Checks if the jet passes the jvt cut threshold. 
    //Uses maximum validity range by default, but there is a configurable parameter
    //to lower the max pt value (if one only wants to use jvt up to 50 GeV, for example.
    virtual bool passesJvtCut(const xAOD::Jet& jet) = 0;

    //Checks if the jet lies in the pt and eta range where Jvt is valid.
    //0<|eta|<2.4 and 20<pt<60 GeV
    virtual bool isInRange(const xAOD::Jet& jet) = 0;

    //returns actual jvtthreshold
    virtual float getJvtThresh() const = 0;

    //returns user-defined max pt if applicable (by default is the same as the max pt range of the histograms)
    virtual float getUserPtMax() const = 0;

    //Sets random seed for random generator, for debugging
    virtual void setRandomSeed(int seed) = 0;

}; // class IJetJvtEfficiencyTool

} // namespace CP

#endif /* IJETJVTEFFICIENCY_H_ */
