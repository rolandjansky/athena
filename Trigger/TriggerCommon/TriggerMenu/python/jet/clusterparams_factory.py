# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" factory function to return the objects that carry cluster maker args
according to the cluster maker  class. """

def clusterparams_factory(kargs):
    # no choice yet
    return _ClusterParams(**kargs)


class _ClusterParams(object):
    """ Argument checking class that holds the  parameters for the
    TrigClusterMaker creation python alg."""

    cluster_type = 'topo_clusters'

    def __init__(self, label, **kargs):
        self.cluster_label = label
