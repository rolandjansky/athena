# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( IdDictParser )

# Component(s) in the package:
atlas_add_library( IdDictParser
                   src/IdDictParser.cxx
                   PUBLIC_HEADERS IdDictParser
                   LINK_LIBRARIES IdDict XMLCoreParser )

atlas_add_executable( tid
                      test/tid.cxx
                      LINK_LIBRARIES IdDictParser Identifier )

atlas_add_executable( test_det_id
                      test/test_indet_id.cxx
                      LINK_LIBRARIES IdDictParser Identifier )

atlas_add_dictionary( IdDictParserDict
                      IdDictParser/IdDictParserDict.h
                      IdDictParser/selection.xml
                      LINK_LIBRARIES IdDictParser )

# Install files from the package:
atlas_install_xmls( data/*.dtd data/*.xml )

