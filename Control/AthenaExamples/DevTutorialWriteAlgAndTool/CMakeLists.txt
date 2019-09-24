atlas_subdir( DevTutorialWriteAlgAndTool )

atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Event/xAOD/xAODTracking )

atlas_add_component( DevTutorialWriteAlgAndTool
                     src/*.cxx
                     src/components/*.cxx 
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps xAODTracking )

atlas_install_python_modules( python/*.py )
