# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

dsname      = "T1_McAtNLO_top"
dsid        = 4100
inputstage  = "recov10"
outputstage = "merge"
process     = "rome.00"
dir         = "rfio:/castor/cern.ch/grid/atlas/datafiles/rome/recov10"
catalog     = "file:PoolFileCatalog.xml"
nfiles      = 20
dbseed      = 3124
prefix      = process + str(dsid) + '.' + inputstage  + '.' + dsname
postfix     = process + str(dsid) + '.' + outputstage + '.' + dsname
