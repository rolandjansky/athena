# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
################################################################################
##
#@file DiTauAODList.py
#
#@brief List AOD output containers. 
################################################################################

#------------------------------------------------------------------------------
# AOD output list
#------------------------------------------------------------------------------
DiTauAODList = []

#------------------------------------------------------------------------------
# DiTauRec main xAOD containers
#------------------------------------------------------------------------------
DiTauAODList += [ "xAOD::DiTauJetContainer#DiTauJets" ]
DiTauAODList += [ "xAOD::DiTauJetAuxContainer#DiTauJetsAux." ]


