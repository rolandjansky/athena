################################################################################
# Package: TriggerMenuMT
################################################################################

# Declare the package name:
atlas_subdir( TriggerMenuMT )

# Install files from the package:
atlas_install_python_modules( python/*.py 
			      python/LVL1MenuConfig
			      python/HLTMenuConfig
			      )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( scripts/generate*Menu.py scripts/generateMenuMT.py)
atlas_install_xmls( data/*.dtd data/*.xml )

atlas_add_test( generateMenuMT SCRIPT scripts/testMenuMT.sh 
                PROPERTIES TIMEOUT 500 
                POST_EXEC_SCRIPT "check_log.pl --config checklogTriggerTest.conf generateMenuMT.log"
              )


