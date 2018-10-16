# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

"""
Configuration database for ISF
Elmar Ritsch, 10/11/2014
"""

from AthenaCommon.CfgGetter import addTool, addService

addTool("ISF_FatrasServices.TrackingToolsConfig.getInDetTrackingGeometryBuilder",            "ISF_InDetTrackingGeometryBuilder")
addTool("ISF_FatrasServices.TrackingToolsConfig.getFatrasCaloTrackingGeometryBuilder",       "ISF_FatrasCaloTrackingGeometryBuilder")
addTool("ISF_FatrasServices.TrackingToolsConfig.getFatrasMuonTrackingGeometryBuilder",       "ISF_FatrasMuonTrackingGeometryBuilder")
addTool("ISF_FatrasServices.TrackingToolsConfig.getFatrasGeometryBuilder",                   "ISF_FatrasGeometryBuilder")
addTool("ISF_FatrasServices.TrackingToolsConfig.getFatrasNavigator",                         "ISF_FatrasNavigator")
addTool("ISF_FatrasServices.TrackingToolsConfig.getFatrasChargedPropagator",                 "ISF_FatrasChargedPropagator")
addTool("ISF_FatrasServices.TrackingToolsConfig.getFatrasSTEP_Propagator",                   "ISF_FatrasSTEP_Propagator")
addTool("ISF_FatrasServices.TrackingToolsConfig.getFatrasPropagator",                        "ISF_FatrasPropagator")
addTool("ISF_FatrasServices.TrackingToolsConfig.getFatrasStaticPropagator",                  "ISF_FatrasStaticPropagator")
addTool("ISF_FatrasServices.TrackingToolsConfig.getFatrasStaticNavigationEngine",            "ISF_FatrasStaticNavigationEngine")
addTool("ISF_FatrasServices.TrackingToolsConfig.getFatrasBetheHeitlerEnergyLossUpdator",     "ISF_FatrasBetheHeitlerEnergyLossUpdator")
addTool("ISF_FatrasServices.TrackingToolsConfig.getFatrasEnergyLossSamplerBetheBloch",       "ISF_FatrasEnergyLossSamplerBetheBloch")
addTool("ISF_FatrasServices.TrackingToolsConfig.getFatrasMultipleScatteringUpdator",         "ISF_FatrasMultipleScatteringUpdator")
addTool("ISF_FatrasServices.TrackingToolsConfig.getFatrasExtrapolator",                      "ISF_FatrasExtrapolator")
addTool("ISF_FatrasServices.TrackingToolsConfig.getFatrasStaticExtrapolator",                "ISF_FatrasStaticExEngine")
addTool("ISF_FatrasServices.TrackingToolsConfig.getFatrasExEngine",                          "ISF_FatrasExEngine")
addService("ISF_FatrasServices.TrackingToolsConfig.getFatrasTrackingGeometrySvc",            "ISF_FatrasTrackingGeometrySvc")

addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasKinematicFilter",                   "ISF_FatrasKinematicFilter")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getG4RunManagerHelper",                      "ISF_G4RunManagerHelper")
addService("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasSimServiceID",                   "ISF_FatrasSimSvc")
addService("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasNewExtrapolationSimServiceID",   "ISF_FatrasNewExtrapolationSimSvc")
addService("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasPileupSimServiceID",             "ISF_FatrasPileupSimSvc")
