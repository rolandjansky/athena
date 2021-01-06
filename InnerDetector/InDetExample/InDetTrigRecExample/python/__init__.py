# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

if 'InDetTrigFlags' not in dir():
  from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
  InDetTrigFlags.setupDefaults()
