# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

"""
Configuration database for ISF_Services
Elmar Ritsch, 16/12/2014
"""

from AthenaCommon.CfgGetter import addService

# Common tools, services and algorithms used by jobs
addService("ISF_Services.ISF_ServicesConfigLegacy.getISFEnvelopeDefSvc", "ISF_ISFEnvelopeDefSvc")
addService("ISF_Services.ISF_ServicesConfigLegacy.getAFIIEnvelopeDefSvc", "ISF_AFIIEnvelopeDefSvc")
addService("ISF_Services.ISF_ServicesConfigLegacy.getGeoIDSvc", "ISF_GeoIDSvc")
addService("ISF_Services.ISF_ServicesConfigLegacy.getAFIIGeoIDSvc", "ISF_AFIIGeoIDSvc")
addService("ISF_Services.ISF_ServicesConfigLegacy.getParticleBrokerSvc", "ISF_ParticleBrokerSvc")
addService("ISF_Services.ISF_ServicesConfigLegacy.getParticleBrokerSvcNoOrdering", "ISF_ParticleBrokerSvcNoOrdering")
addService("ISF_Services.ISF_ServicesConfigLegacy.getAFIIParticleBrokerSvc", "ISF_AFIIParticleBrokerSvc")
addService("ISF_Services.ISF_ServicesConfigLegacy.getAFIIEnergyOrderedParticleBrokerSvc", "ISF_AFIIEnergyOrderedParticleBrokerSvc")
addService("ISF_Services.ISF_ServicesConfigLegacy.getInputConverter", "ISF_InputConverter")
addService("ISF_Services.ISF_ServicesConfigLegacy.getLongLivedInputConverter", "ISF_LongLivedInputConverter")
addService("ISF_Services.ISF_ServicesConfigLegacy.getTruthService", "ISF_TruthService")
addService("ISF_Services.ISF_ServicesConfigLegacy.getMC12TruthService", "ISF_MC12TruthService")
addService("ISF_Services.ISF_ServicesConfigLegacy.getMC12PlusTruthService", "ISF_MC12PlusTruthService")
addService("ISF_Services.ISF_ServicesConfigLegacy.getMC12LLPTruthService", "ISF_MC12LLPTruthService")
addService("ISF_Services.ISF_ServicesConfigLegacy.getMC15aTruthService", "ISF_MC15aTruthService")
addService("ISF_Services.ISF_ServicesConfigLegacy.getMC15aPlusTruthService", "ISF_MC15aPlusTruthService")
addService("ISF_Services.ISF_ServicesConfigLegacy.getMC15aPlusLLPTruthService", "ISF_MC15aPlusLLPTruthService")
addService("ISF_Services.ISF_ServicesConfigLegacy.getMC15TruthService", "ISF_MC15TruthService")
addService("ISF_Services.ISF_ServicesConfigLegacy.getMC16TruthService", "ISF_MC16TruthService")
addService("ISF_Services.ISF_ServicesConfigLegacy.getMC16LLPTruthService", "ISF_MC16LLPTruthService")
addService("ISF_Services.ISF_ServicesConfigLegacy.getMC18TruthService", "ISF_MC18TruthService")
addService("ISF_Services.ISF_ServicesConfigLegacy.getMC18LLPTruthService", "ISF_MC18LLPTruthService")
addService("ISF_Services.ISF_ServicesConfigLegacy.getValidationTruthService", "ISF_ValidationTruthService")
addService("ISF_Services.LegacySimServicesConfigLegacy.getParticleKillerSvc", "ISF_ParticleKillerSvc")
