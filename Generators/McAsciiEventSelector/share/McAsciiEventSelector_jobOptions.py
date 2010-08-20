## @file McAsciiEventSelector_jobOptions.py
## @brief EventSelector job options file to load an ASCII based event selector
## @author Sebastien Binet <binet@cern.ch>
## $Id: McAsciiEventSelector_jobOptions.py,v 1.1.1.1 2007-07-25 02:00:33 binet Exp $

# load the McAsciiEventSelector
theApp.Dlls += [ "McAsciiEventSelector" ]
theApp.ExtSvc += [ "McAsciiEventSelector/EventSelector" ]

EventSelector = Service( "EventSelector" )

## list of input ASCII file names
EventSelector.InputCollections = [ "hepmc.ascii" ]


## load and create the McAsciiCnvSvc service
theApp.ExtSvc += [ "McAsciiCnvSvc" ]

## configure the core services
EventPersistencySvc = Service( "EventPersistencySvc" )
EventPersistencySvc.CnvServices += [ "McAsciiCnvSvc" ]

## make our event selector an AddressProvider for the ProxyProviderSvc
ProxyProviderSvc = Service( "ProxyProviderSvc" )
ProxyProviderSvc.ProviderNames += [ "EventSelector" ]
