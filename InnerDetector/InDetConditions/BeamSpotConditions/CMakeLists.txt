## automatically generated CMakeLists.txt file

# Declare the package
atlas_subdir( BeamSpotConditions )

# Declare external dependencies ... default here is to include ROOT
find_package( ROOT COMPONENTS MathCore RIO Core Tree Hist )

# An example is included
atlas_depends_on_subdirs(
    PUBLIC

    PRIVATE
    Control/AthenaBaseComps
    Control/AthenaKernel
    Control/StoreGate
    AthenaPoolUtilities
)

# if you add components (tools, algorithms) to this package
# uncomment the next lines
atlas_add_component( BeamSpotConditionsComps
		     src/*.cxx 
		     src/components/*.cxx
                     LINK_LIBRARIES AthenaPoolUtilities AthenaBaseComps AthenaKernel StoreGateLib
                   )

# if you add an application (exe) to this package
# declare it like this (note convention that apps live in util dir)
# atlas_add_executable( MyApp util/myApp.cxx
#                       LINK_LIBRARIES BeamSpotConditionsLib
# )

# Install python modules, joboptions, and share content
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_data( data/* )
# You can access your data from code using path resolver, e.g.
# PathResolverFindCalibFile("BeamSpot_ConditionsAlgs/file.txt")

