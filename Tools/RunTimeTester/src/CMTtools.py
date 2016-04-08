# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def decodeCMTCONFIG(cfg):
    tokens = cfg.split('-')
    nbits = 32
    if cfg.startswith('x86_64'): nbits = 64
    assert (len(tokens) == 4)
    dict = {'chip':tokens[0], 'os':tokens[1], 'compiler':tokens[2], 'build': tokens[3], 'nbits': nbits}
    return dict
