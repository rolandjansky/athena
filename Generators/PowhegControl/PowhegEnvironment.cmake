# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
# This module is used to set up the environment for Powheg
# 
#

# Set the environment variable(s):
#find_package( Powheg )
MESSAGE (STATUS "Powheg version  " ${POWHEG_VERSION})
MESSAGE (STATUS "Powheg root  " ${POWHEG_ROOT})

#if( POWHEG_FOUND )
get_filename_component( _powhegPath "${POWHEG_ROOT}" DIRECTORY )
  set( POWHEGENVIRONMENT_ENVIRONMENT 
     FORCESET POWHEGPATH ${_powhegPath}/${POWHEG_VERSION} )
unset( _powhegPath )
#endif()

MESSAGE (STATUS "POWHEGPATH  " ${POWHEGPATH})

# Silently declare the module found:
set( POWHEGENVIRONMENT_FOUND TRUE )

