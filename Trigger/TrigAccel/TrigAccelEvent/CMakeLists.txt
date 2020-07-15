################################################################################
# Package: TrigAccelEvent
################################################################################

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigAccelEvent )

# Component(s) in the package:
atlas_add_library( TrigAccelEvent
                   TrigAccelEvent/*.h
                   INTERFACE
                   PUBLIC_HEADERS TrigAccelEvent)
