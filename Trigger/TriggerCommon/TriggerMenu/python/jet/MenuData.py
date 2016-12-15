# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


class MenuData(object):
    """Container for menu data organized in a manner useful for jet chain
    configuration."""

    def __init__(self,
                 scan_type,
                 data_type,
                 fex_params,
                 hypo_params,
                 cluster_params,
                 recluster_params,
                 trim_params,
                 last_fex_params  # parameters of the last fex rn (eg a10r?)
             ):

        self.scan_type = scan_type
        self.data_type = data_type
        self.fex_params = fex_params
        self.hypo_params = hypo_params
        self.cluster_params = cluster_params
        self.recluster_params = recluster_params
        self.trim_params = trim_params
        self.last_fex_params = last_fex_params

    def __str__(self):
        s = ['%s\n%s\n' % (k, str(v)) for k, v in self.__dict__.items()]
        return '\n'.join(s)


