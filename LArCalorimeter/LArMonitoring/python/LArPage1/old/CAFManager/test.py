# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from CAFManager import CAFManager
from JobInputSelectors import FullRunsWithLimit
mgr = CAFManager()
jSel = FullRunsWithLimit()
jSel.configure(mgr.db(), mgr.db().getJobTypeByName('samples2009'), mgr.stream)
l2 = jSel.getFreeInputs()
for f in l2[0] : print f.name
