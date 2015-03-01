# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Module defines Sequence class"""


class Sequence(object):
    """Provide input and out trigger names and algorthm sequence lists.
    Provide a flag signaling wither Algorithm procy objects have been
    converted to ATLAS python configuration classes.

    The use of Algorithm proxies decouples the menu code from the
    Pythion configuration code. Historically, this coupling has made
    development of the slice menu code difficult due to the additional
    complexity of determing the appropriate ATLAS environment variables,failing
    imports, and time to instantiate the python objects."""

    def __init__(self, te_in, alg_list, alias, te_out):
        self.te_in = te_in  # input trigger elenment name

        # list of Algorithms objects. The may be proxies (which
        # can be converted to an ATLAS python configuration object),
        # or the python configuration object itself.
        self.alg_list = alg_list
        self.te_out = te_out    # output trigger elenment name
        self.alias = alias  # for debugging

        # flag signalling whether Algorithm proxies have been
        # converted to an ATLAS python configuration object.
        self.instantiated = False

    def instantiateAlgs(self, instantiator):
        """Convert Algoritm proxies to ATLAS python configuration classes."""

        if self.instantiated:
            return
        self.instantiated = True

        self.alg_list = [instantiator(a) for a in self.alg_list]

    def __str__(self):

        s = ['te_in: %s   te_out: %s' % (self.te_in, self.te_out)]
        if self.instantiated:
            s.extend(['    %s' % str(a) for a in self.alg_list])
        else:
            s.extend(['    %s' % str(a) for a in self.alg_list])
        return '\n'.join(s)
