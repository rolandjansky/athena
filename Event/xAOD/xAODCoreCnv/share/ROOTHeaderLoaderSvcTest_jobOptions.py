# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Small/simple job testing the behaviour of xAODMaker::ROOTHeaderLoaderSvc.
#

# Set up the service.
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr += CfgMgr.xAODMaker__ROOTHeaderLoaderSvc( 'ROOTHeaderLoaderSvc',
                              HeaderNames = [
                                 'xAODEgamma/PhotonContainer.h',
                                 'xAODTrigEgamma/TrigPhotonContainer.h' ] )
theApp.CreateSvc += [ 'xAODMaker::ROOTHeaderLoaderSvc/ROOTHeaderLoaderSvc' ]

# Some final tweaking:
theApp.EvtMax = 1
