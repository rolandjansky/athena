# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Configuration database for ISF_Services
Elmar Ritsch, 16/12/2014
"""

from AthenaCommon.CfgGetter import addService

# Common tools, services and algorithms used by jobs
addService("ISF_Services.ISF_ServicesConfig.getISFEnvelopeDefSvc", "ISF_ISFEnvelopeDefSvc")
addService("ISF_Services.ISF_ServicesConfig.getAFIIEnvelopeDefSvc", "ISF_AFIIEnvelopeDefSvc")
addService("ISF_Services.ISF_ServicesConfig.getGeoIDSvc", "ISF_GeoIDSvc")
addService("ISF_Services.ISF_ServicesConfig.getAFIIGeoIDSvc", "ISF_AFIIGeoIDSvc")
addService("ISF_Services.ISF_ServicesConfig.getParticleBrokerSvc", "ISF_ParticleBrokerSvc")
addService("ISF_Services.ISF_ServicesConfig.getParticleBrokerSvcNoOrdering", "ISF_ParticleBrokerSvcNoOrdering")
addService("ISF_Services.ISF_ServicesConfig.getAFIIParticleBrokerSvc", "ISF_AFIIParticleBrokerSvc")
addService("ISF_Services.ISF_ServicesConfig.getSimHitService", "ISF_SimHitService")
addService("ISF_Services.ISF_ServicesConfig.getNoG4SimHitService", "ISF_NoG4SimHitService")
addService("ISF_Services.ISF_ServicesConfig.getPileupSimHitService", "ISF_PileupSimHitService")
addService("ISF_Services.ISF_ServicesConfig.getInputConverter", "ISF_InputConverter")
addService("ISF_Services.ISF_ServicesConfig.getLongLivedInputConverter", "ISF_LongLivedInputConverter")
addService("ISF_Services.ISF_ServicesConfig.getTruthService", "ISF_TruthService")
addService("ISF_Services.ISF_ServicesConfig.getMC12TruthService", "ISF_MC12TruthService")
addService("ISF_Services.ISF_ServicesConfig.getMC12PlusTruthService", "ISF_MC12PlusTruthService")
addService("ISF_Services.ISF_ServicesConfig.getMC12LLPTruthService", "ISF_MC12LLPTruthService")
addService("ISF_Services.ISF_ServicesConfig.getMC15aTruthService", "ISF_MC15aTruthService")
addService("ISF_Services.ISF_ServicesConfig.getMC15aPlusTruthService", "ISF_MC15aPlusTruthService")
addService("ISF_Services.ISF_ServicesConfig.getMC15aPlusLLPTruthService", "ISF_MC15aPlusLLPTruthService")
addService("ISF_Services.ISF_ServicesConfig.getMC15TruthService", "ISF_MC15TruthService")
addService("ISF_Services.ISF_ServicesConfig.getMC16TruthService", "ISF_MC16TruthService")
addService("ISF_Services.ISF_ServicesConfig.getMC16LLPTruthService", "ISF_MC16LLPTruthService")
addService("ISF_Services.ISF_ServicesConfig.getMC18TruthService", "ISF_MC18TruthService")
addService("ISF_Services.ISF_ServicesConfig.getMC18LLPTruthService", "ISF_MC18LLPTruthService")
addService("ISF_Services.ISF_ServicesConfig.getValidationTruthService", "ISF_ValidationTruthService")
addService("ISF_Services.ISF_ServicesConfig.getParticleKillerSvc", "ISF_ParticleKillerSvc")
