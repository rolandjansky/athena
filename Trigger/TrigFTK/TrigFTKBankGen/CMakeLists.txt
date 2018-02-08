################################################################################
# Package: TrigFTKBankGen
################################################################################

# Declare the package name:
atlas_subdir( TrigFTKBankGen )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          AtlasPolicy
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          GaudiKernel
                          Trigger/TrigFTK/TrigFTKSim )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( Eigen )
find_package( HepPDT )
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( TBB )

add_definitions(-DHAVE_BOOST_IOSTREAMS)

# Component(s) in the package:
atlas_add_component( TrigFTKBankGen
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS}
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} ${TBB_LIBRARIES} ${Boost_LIBRARIES} GaudiKernel TrigFTKSimLib )

# Install files from the package:
atlas_install_headers( TrigFTKBankGen )
atlas_install_joboptions( share/*job[Oo]ptions.py share/MC12.184002.ParticleGenerator_mu_Pt1to1000_Eta30_x10_FTK.py share/MC12.184002.ParticleGenerator_pi_Pt1to1000_Eta30_x10_FTK.py share/MC12.184002.ParticleGenerator_mu_Pt1to1000_Eta30_FTK.py share/MC12.184002.ParticleGenerator_pi_Pt1to1000_Eta30_FTK.py )
atlas_install_joboptions( share/skeleton.*.py )
atlas_install_runtime( scripts/*_tf.py )

