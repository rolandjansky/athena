# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory


def  TrigEgammaTLAPhotonHypoToolFromDict(chain_dict):
    return  CompFactory.TrigEgammaTLAPhotonHypoTool(chain_dict['chainName'])
