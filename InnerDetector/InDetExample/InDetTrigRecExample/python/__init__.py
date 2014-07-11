# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

if not 'InDetTrigFlags' in dir():
  from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
  InDetTrigFlags.setupDefaults()
