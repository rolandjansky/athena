# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( EventIndexProducer )

# External dependencies:
find_package( Protobuf )
find_package( boto3 )
find_package( botocore )
find_package( jmespath )
find_package( s3transfer )
find_package( stomppy )

# Set up a custom script that could run protoc in an environment not yet set
# up for running it. And then update the protobuf::protoc imported target
# (created by FindProtobuf.cmake) to use this script instead of the executable
# itself.
if( Protobuf_FOUND )
   set( _customProtocScript
      "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/atlas_protoc.sh" )
   configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/cmake/atlas_protoc.sh.in"
      "${_customProtocScript}" @ONLY )
   set_target_properties( protobuf::protoc PROPERTIES
      IMPORTED_LOCATION "${_customProtocScript}" )
   unset( _customProtocScript )
endif()

# Generate and install the EventIndex proto file(s).
if( Protobuf_FOUND )
   protobuf_generate_python( PROTO_PYS "protocolB/EventIndex.proto" )
   set( _eiPythonFile
      "${CMAKE_PYTHON_OUTPUT_DIRECTORY}/EventIndexProducer/EventIndex_pb2.py" )
   add_custom_command(
      OUTPUT "${_eiPythonFile}"
      COMMAND "${CMAKE_COMMAND}" -E make_directory
              "${CMAKE_PYTHON_OUTPUT_DIRECTORY}/EventIndexProducer"
      COMMAND "${CMAKE_COMMAND}" -E copy "${PROTO_PYS}" "${_eiPythonFile}"
      DEPENDS ${PROTO_PYS} )
   add_custom_target( EventIndexProducerProtobufGen ALL
      SOURCES "${_eiPythonFile}" )
   add_dependencies( Package_EventIndexProducer EventIndexProducerProtobufGen )
   install( FILES "${_eiPythonFile}"
      DESTINATION "${CMAKE_INSTALL_PYTHONDIR}/EventIndexProducer" )
endif()


# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-ignore=F401,E402,F821 )
atlas_install_runtime( scripts/*.py )
