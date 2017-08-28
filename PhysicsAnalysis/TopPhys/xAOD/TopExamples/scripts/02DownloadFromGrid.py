#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import TopExamples.grid

scope          = 'user.aknue'
datasetPattern = '*test_02-03-45a_output.root'
directory      = '/tmp/YOURUSERNAME/DownloadFolder/'

TopExamples.grid.download(scope, datasetPattern, directory)
