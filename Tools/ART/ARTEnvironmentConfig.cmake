# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# This module is used to set up the environment for ART inputs
#
# author :  Attila Krasznahorkay <attila.krasznahorkay@cern.ch>, Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>

# Set the environment variable(s):
set( ARTENVIRONMENT_ENVIRONMENT
   APPEND DATAPATH "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art" )

# Silently declare the module found:
set( ARTENVIRONMENT_FOUND TRUE )
