# $Id: CMakeLists.txt 726441 2016-02-26 14:57:39Z krasznaa $
################################################################################
# Package: MuonConditionsAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( MuonConditionsAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( MuonConditionsAthenaPoolPoolCnv src/*.cxx
   FILES MuonByteStreamErrors/RpcByteStreamErrorContainer.h
   TYPES_WITH_NAMESPACE Muon::RpcByteStreamErrorContainer
   LINK_LIBRARIES AthContainers AthenaPoolUtilities Identifier MuonByteStreamErrors
   AthenaPoolCnvSvcLib )

atlas_add_dictionary( MuonConditionsAthenaPoolDict
   MuonConditionsAthenaPool/MuonConditionsAthenaPoolDict.h
   MuonConditionsAthenaPool/selection.xml
   LINK_LIBRARIES AthContainers AthenaPoolUtilities Identifier MuonByteStreamErrors
   AthenaPoolCnvSvcLib )

# Declare the test(s) of the package:
atlas_add_test( RpcByteStreamErrorContainerCnv_p1_test
   SOURCES test/RpcByteStreamErrorContainerCnv_p1_test.cxx
   src/RpcByteStreamErrorContainerCnv_p1.cxx
   LINK_LIBRARIES GaudiKernel AthContainers Identifier MuonByteStreamErrors
   TestTools AthenaPoolCnvSvcLib )

# Install files from the package:
atlas_install_headers( MuonConditionsAthenaPool )
