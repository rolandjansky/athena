################################################################################
# Package: TrigMissingETHypo
################################################################################

# Declare the package name:
atlas_subdir( TrigMissingETHypo )

atlas_add_component( TrigMissingETHypo
                     src/METHypo.cxx
                     src/TEHypo.cxx
                     src/XSHypo.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES TrigInterfacesLib TrigTimeAlgsLib
                     xAODTrigMissingET GaudiKernel TrigMissingEtEvent
                     TrigEFMissingETLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/TriggerConfig_*.py )

