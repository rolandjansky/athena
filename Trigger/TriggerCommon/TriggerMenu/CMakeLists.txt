################################################################################
# Package: TriggerMenu
################################################################################

# Declare the package name:
atlas_subdir( TriggerMenu )

# Install files from the package:
atlas_install_python_modules( python/*.py python/menu python/l1
python/l1menu python/l1topo python/l1topomenu python/egamma
python/muon python/jet python/bjet python/met python/tau python/afp
python/minbias python/heavyion python/bphysics python/calibcosmicmon
python/test python/combined python/commonUtils )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( scripts/generate*Menu.py scripts/menuTestTMC.sh )
atlas_install_xmls( data/*.dtd data/*.xml )

atlas_add_test( generateMenu SCRIPT scripts/testMenu.sh 
                PROPERTIES TIMEOUT 500 
                POST_EXEC_SCRIPT "check_log.pl --config checklogTriggerTest.conf generateMenu.log" 
              )

atlas_add_test ( checkL1Menu SCRIPT scripts/checkL1Menu.sh
   POST_EXEC_SCRIPT "check_log.pl --config checklogTriggerTest.conf checkL1Menu.log"
)
