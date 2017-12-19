# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


class MenuData(object):
    """Container for menu data organized in a manner useful for jet chain
    configuration.


    scan_type: used in Algorithm instance names
    data_type: used in Algorithm instance names
    fex_params: parameters used to configure the fex.
    hypo_params: parameters used to configure the hypo
    cluster_params: used to label Algorithms. Since moving to the common
                    sequence for cluster building, this is used only by heavy
                    ion Algorithms.
    second_fex_params: parameters for a second fex if the chain requires
                       such a fex. This is the case for jet reclustering
                       chains where the first fex creates jets, which are
                       the inputs if the second fex.

    The types of the _params objects vary according to the typ of the fex
    or hypo they are configuring. A type label is held inside the
    _params object.

    """

    def __init__(self):

        self.scan_type = None
        self.data_type = None
        self.fex_params = None
        self.hypo_params = None
        self.cluster_params = None
        self.second_fex_params = None
        self.trkopt = None

    def __str__(self):
        s = ['%s\n%s\n' % (k, str(v)) for k, v in self.__dict__.items()]
        return '\n'.join(s)


