# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

""" create a PassThroughFilter configurer"""

from TrigHLTJetHypo.FastReductionAlgToolFactory import toolfactory

def makePassThroughFilterConfigurer():

    toolclass, name =  toolfactory('PassThroughFilterConfigTool')
    vals = {'name' : name}

    # return a PassThroughFilter configuration AlgTool
    return toolclass(**vals)
