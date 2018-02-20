################################################################################
# Package: AFP_EventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( AFP_EventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          ForwardDetectors/AFP/AFP_DigiEv
                          ForwardDetectors/AFP/AFP_RecoEv/AFP_LocRecoEv
                          ForwardDetectors/AFP/AFP_SimEv
                          ForwardDetectors/AFP/AFP_RawEv
                          PRIVATE
                          DetectorDescription/Identifier
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( AFP_EventTPCnv
                   src/*.cxx
                   PUBLIC_HEADERS AFP_EventTPCnv
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolUtilities AFP_DigiEv AFP_LocRecoEv AFP_SimEv AFP_RawEv AthenaPoolCnvSvcLib
                   PRIVATE_LINK_LIBRARIES Identifier GaudiKernel )

atlas_add_dictionary( AFP_EventTPCnvDict
                      AFP_EventTPCnv/AFP_EventTPCnvDict.h
                      AFP_EventTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities AFP_DigiEv AFP_LocRecoEv AFP_SimEv AFP_RawEv Identifier GaudiKernel AFP_EventTPCnv )

