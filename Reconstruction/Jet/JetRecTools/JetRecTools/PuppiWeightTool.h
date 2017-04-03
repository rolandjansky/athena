#ifndef PUPPIWeightTool_h
#define PUPPIWeightTool_h

#include "JetRecTools/JetConstituentModifierBase.h"
#include "xAODBase/IParticleContainer.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODPFlow/PFOContainer.h"
#include <string>

class PuppiWeightTool: public JetConstituentModifierBase {
	ASG_TOOL_CLASS(PuppiWeightTool, IJetConstituentModifier)

	public:

	PuppiWeightTool(const std::string& name);
	StatusCode process(xAOD::IParticleContainer* cont) const;
	StatusCode process(xAOD::PFOContainer* cont) const; 

 private:

	// puppi parameters
	double m_R0;
	double m_Rmin;
	double m_beta;
	double m_centralPTCutOffset;
	double m_centralPTCutSlope;
	double m_forwardPTCutOffset;
	double m_forwardPTCutSlope;
	double m_etaBoundary;
	bool m_PUPenalty;

	//experimental
	bool m_includeLowPTTracks;
	bool m_includeCentralNeutralsInAlpha;

	bool m_applyWeight;
	//bool m_limitRemoval;
};

#endif
