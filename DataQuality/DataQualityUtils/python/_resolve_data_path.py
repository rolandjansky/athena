# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
import os
import logging
logging.basicConfig()

def resolve_data_path(fin):
    if 'DATAPATH' not in os.environ:
        logging.error('DATAPATH not found! Have you actually set up a release?')
        return None
    for possible_dir in os.environ['DATAPATH'].split(':'):
        hypo = os.path.join(possible_dir, fin)
        if os.path.exists(hypo):
            return hypo
    return None
