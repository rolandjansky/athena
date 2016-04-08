# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#---------------------------------------------------------------
# Pool Persistency
#---------------------------------------------------------------
include( "AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py" )

theApp.Dlls   += [ "GeneratorObjectsAthenaPoolPoolCnv" ]
Stream1 = Algorithm( "Stream1" )
Stream1.ItemList += ["2101#*", "133273#*" ]
#PoolSvc = Service( "PoolSvc" )
#PoolSvc.Output = "McEvent.root"
