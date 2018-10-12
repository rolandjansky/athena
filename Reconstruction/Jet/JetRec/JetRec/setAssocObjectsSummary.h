#ifndef JetRecTool_setAssocObjectsSummary_H
#define JetRecTool_setAssocObjectsSummary_H
#include "xAODBase/IParticle.h"
#include "xAODJet/Jet.h"
#include <vector>
#include <string>

void setAssocObjectsSummary(const std::vector<const xAOD::IParticle*>&,
                            const std::string& label,
                            xAOD::Jet& jet);
#endif
