# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
# This module is used to set up the environment for Powheg
# 
#

# Set the environment variable(s):
#find_package( Powheg )

#if( POWHEG_FOUND )
#  get_filename_component( _powhegPath "${POWHEG_INCLUDE_DIR}" DIRECTORY )
  set( POWHEGENVIRONMENT_ENVIRONMENT 
     FORCESET POWHEGPATH "/afs/cern.ch/atlas/offline/external/powhegbox/ATLASOTF-00-04-00" )
#  unset( _powhegPath )
#endif()

# Silently declare the module found:
set( POWHEGENVIRONMENT_FOUND TRUE )

