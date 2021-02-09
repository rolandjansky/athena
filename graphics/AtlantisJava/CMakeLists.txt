################################################################################
# Package: AtlantisJava
################################################################################

# Declare the package name:
atlas_subdir( AtlantisJava )

# Install files from the package:
atlas_install_joboptions( share/InteractiveServer.py )
atlas_install_scripts( share/atlantis share/atlantis_online )
atlas_install_generic( lib/*.jar
                       DESTINATION "share/AtlantisJava/lib"
                       EXECUTABLE )
atlas_install_generic( img/*.gif img/*.png img/*.jpg
                       DESTINATION "share/AtlantisJava/img"
                       EXECUTABLE )
atlas_install_generic( configuration/*.xml configuration/*.csv configuration/PSProlog.txt
                       DESTINATION "share/AtlantisJava/configuration"
                       EXECUTABLE )
atlas_install_generic( events/event.dtd events/*.zip
                       DESTINATION "share/AtlantisJava/events"
                       EXECUTABLE )
atlas_install_generic( geometry/*.xml
                       DESTINATION "share/AtlantisJava/geometry"
                       EXECUTABLE )
atlas_install_generic( help/help.jar help/online.xml help/*.png
                       DESTINATION "share/AtlantisJava/help"
                       EXECUTABLE )
