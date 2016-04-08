################################################################################
# Package: TriggerMenu
################################################################################

# Declare the package name:
atlas_subdir( TriggerMenu )

# Install files from the package:
atlas_install_python_modules( python/*.py python/menu python/l1 python/l1menu python/l1topo python/l1topomenu python/egamma python/muon python/jet python/bjet python/met python/tau python/minbias python/heavyion python/bphysics python/calibcosmicmon python/test python/combined )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( scripts/generate*Menu.py scripts/menuTestTMC.sh )
atlas_install_xmls( data/*.dtd data/*.xml )

