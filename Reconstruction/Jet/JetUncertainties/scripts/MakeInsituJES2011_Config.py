#!/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

for nInSitu in [6,9] :
  
  nTotal = nInSitu+5
  configFile = open("InsituJES2011_"+str(nTotal)+"NP_Preliminary.config","w")

  configFile.write("""
#######################################
#
#  JESProvider Input Settings
#    Reduced list of nuisance parameters from third iteration of 
#    the in-situ JES
#    Several small eta intercalibration components still missing
#    May 3, 2012
#
#  D. Gillberg, C. Doglioni, B. Malaescu
# 
#######################################

JESUncertaintyRootFile:      JESUncertainty_Preliminary.root

#######################################
# 
#   Settings for JES Components
#
#######################################
  """)

  for inSituComp in xrange(1,nInSitu) :
    
    configFile.write("""
JESComponent."""+str(inSituComp)+""".Desc:		Effective JES uncertainty component """+str(inSituComp)+"""
JESComponent."""+str(inSituComp)+""".Name:		EffectiveNP_"""+str(inSituComp))

  configFile.write("""\n# rest term:
JESComponent."""+str(nInSitu)+""".Desc:		Effective JES uncertainty component """+str(nInSitu)+"""
JESComponent."""+str(nInSitu)+""".Name:		EffectiveNP_"""+str(nInSitu)+"""restTerm

###################################

  # Eta intercalibration: stat uncertainties (bundled)
JESComponent.56.Desc:           Eta intercalibration: stat uncertainties
JESComponent.56.Name:           EtaIntercalibration_TotalUnc
JESComponent.56.Subcomponents:           EtaIntercalibration_TotalStat EtaIntercalibration_Modelling

   # Pileup: Mu term
JESComponent.57.Desc:           Pileup: Offset, mu term
JESComponent.57.Name:           Pileup_OffsetMu

#   # Pileup: NPV term
JESComponent.58.Desc:           Pileup: Offset, NPV term
JESComponent.58.Name:           Pileup_OffsetNPV

  # Pileup: High pT term
JESComponent.59.Desc:           High Pt term (temporary, 2010 uncertainty)
JESComponent.59.Name:           SingleParticle_HighPt

  # Calibration closure
JESComponent.60.Desc:           Closure of the calibration, relative to MC11b
JESComponent.60.Name:           RelativeNonClosure_MCTYPE
# NOTE: MCTYPE in the name above will be replaced by the MC type used when
# initiating the provider

#
######################################
  """)