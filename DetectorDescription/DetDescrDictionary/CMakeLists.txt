# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DetDescrDictionary )

# Component(s) in the package:
atlas_add_dictionary( DetDescrDict
                      DetDescrDictionary/DetDescrDictionaryDict.h
                      DetDescrDictionary/selection.xml
                      LINK_LIBRARIES AthContainers Identifier )
