# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Done as direct AddItems
CompulsoryContent = [
'xAOD::EventInfo#*',
'xAOD::EventAuxInfo#*',
'xAOD::TrigDecision#*',
'xAOD::TrigDecisionAuxInfo#*',
'xAOD::TrigConfKeys#*']

CompulsoryTriggerNavigation = [
'xAOD::TrigNavigation#*',
'xAOD::TrigNavigationAuxInfo#*',
# Run 3 navigation:
'xAOD::TrigCompositeContainer#HLTNav*',
'xAOD::TrigCompositeAuxContainer#HLTNav*'
]
