atlas_subdir(PileUpMT)

find_package(ROOT COMPONENTS MathCore RIO Core Tree Hist)
find_package(CLHEP)
atlas_add_library(
  PileUpMTLib src/*.cxx
  PUBLIC_HEADERS PileUpMT
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${RANGEV3_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
  LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES}
  AthAnalysisBaseCompsLib AthenaBaseComps EventInfo xAODCnvInterfaces PileUpToolsLib AthenaKernel SGTools StoreGateLib GaudiKernel TRT_PAI_ProcessLib
)

atlas_add_component(
  PileUpMT src/components/*.cxx
  NOCLIDDB
  LINK_LIBRARIES PileUpMTLib
)
