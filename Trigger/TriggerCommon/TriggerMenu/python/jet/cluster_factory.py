# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def cluster_factory(doLC):
    # no choice yet
    return ClusterAlg(doLC)


class ClusterAlg(object):
    """ Argument checking class that holds the  parameters for an
    Cluster creation python alg."""

    def __init__(self, do_lc):
        self.do_lc = do_lc
