atlas_subdir(PileUpMT)

find_package(CLHEP)
find_package(range-v3)

atlas_add_library(
  PileUpMTLib
  INTERFACE
  PUBLIC_HEADERS PileUpMT
)

atlas_add_component(
  PileUpMT src/*.h src/*.cxx src/components/*.cxx
  INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
  LINK_LIBRARIES PileUpMTLib ${CLHEP_LIBRARIES} range-v3::range-v3 AthenaBaseComps EventInfo xAODCnvInterfaces PileUpToolsLib AthenaKernel SGTools StoreGateLib GaudiKernel TRT_PAI_ProcessLib
)
