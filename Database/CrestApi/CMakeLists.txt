atlas_subdir(CrestApi )

atlas_depends_on_subdirs( PRIVATE Database/APR/FileCatalog)

#External dependencies:
find_package(CURL)
find_package( nlohmann_json )

find_package(Boost REQUIRED)

set(CXX_FILESYSTEM_LIBRARIES stdc++fs)

atlas_add_library(CrestApi
   src/CrestApi.cxx CrestApi/CrestApi.h
   PUBLIC_HEADERS CrestApi
   INCLUDE_DIRS nlohmann_json::nlohmann_json
   LINK_LIBRARIES ${CURL_LIBRARIES} ${CXX_FILESYSTEM_LIBRARIES} nlohmann_json::nlohmann_json
   Boost::regex Boost::filesystem Boost::system Boost::thread
)

atlas_add_test(test  
                SOURCES test/test.cxx CrestApi/CrestApi.h
		PUBLIC_HEADERS CrestApi
		INCLUDE_DIRS nlohmann_json::nlohmann_json CrestApi
		LINK_LIBRARIES nlohmann_json::nlohmann_json CrestApi
		POST_EXEC_SCRIPT nopost.sh )
