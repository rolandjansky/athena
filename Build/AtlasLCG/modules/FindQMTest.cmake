# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindQMTest.cmake 718732 2016-01-20 12:30:12Z krasznaa $
#
# - Find QMTest.
#
# This module will define the following variables:
#  QMTEST_EXECUTABLE  - the qmtest main script
#  QMTEST_PYTHON_PATH - directory containing the Python module 'qm'
#  QMTEST_BINARY_PATH - directory with the qmtest executable
#  QMTEST_PREFIX_PATH - directory holding QMTest on the whole
#
# Can be steered by QMTEST_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Find the executable:
find_program( QMTEST_EXECUTABLE
  NAMES qmtest
  PATH_SUFFIXES bin Scripts PATHS ${QMTEST_ROOT} )
get_filename_component( QMTEST_BINARY_PATH ${QMTEST_EXECUTABLE}
  PATH )

# Set an environment variable:
get_filename_component( QMTEST_PREFIX_PATH ${QMTEST_BINARY_PATH}
  PATH )
set( QMTEST_ENVIRONMENT SET QM_home ${QMTEST_PREFIX_PATH} )

# Find the python path:
find_path( QMTEST_PYTHON_PATH
  NAMES qm/__init__.py
  PATH_SUFFIXES Lib/site-packages
  lib/python2.7/site-packages
  PATHS ${QMTEST_ROOT} )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( QMTest DEFAULT_MSG QMTEST_EXECUTABLE
  QMTEST_PYTHON_PATH )
mark_as_advanced( QMTEST_FOUND QMTEST_EXECUTABLE QMTEST_BINARY_PATH
  QMTEST_PREFIX_PATH QMTEST_PYTHON_PATH )

# Set up the RPM dependency:
lcg_need_rpm( QMtest )
