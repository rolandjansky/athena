# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration



def fex_factory(selector, kargs):

    if selector == 'antikt':
        return AntiKt(**kargs)

    if selector == 'cone':
        return Cone(**kargs)

    if selector == 'jetrec':
        return JetRec(**kargs)

    else:
        raise RuntimeError('fex_factory: unknow fex selector %s' % selector)


class JetFex(object):
    """Common code for Fexes: check sanity of input parameters.
    Provide an alg factory parameter.

    26/2/2014: with the current simple list (in constrast with 2012)
    the fexes are so similar the class heirarchy is not yet useful.
    It may become so if the variety of Fex algorithms increases."""

    def __init__(self, merge_param, calib, fex_input, level):

        # temporary kludge until algFactories names updataed
        level = {
            'TT': 'l15',
            'tc': 'jr'}[level]

        self._check_args(merge_param, calib, fex_input)
        self.fex_input = fex_input
        # make key for the alg dispatcher table
        self.fex_key = '%s_%s%s_%s_%s' % (level,
                                          self.fex_type,
                                          str(merge_param),
                                          fex_input,
                                          calib)

    def __str__(self):
        s = ['%s: %s\n' % (k, str(v)) for k, v in self.__dict__.items()]
        return '\n'.join(s)

    def _check_args(self, merge_param, calib, fex_input):
        """check the constructor args"""

        try:
            merge_param = int(merge_param)
        except ValueError:
            m = '%s._check_args: non integer merge_param %s' % (
                self.__class__.__name__,
                str(merge_param))
            raise RuntimeError(m)

        if not merge_param in (2, 4, 10):
            m = '%s._check_args: unsupported merge_param %d' % (
                self.__class__.__name__,
                merge_param)
            raise RuntimeError(m)

        if not fex_input in ('tc', 'cc', 'TT'):
            m = '%s._check_args: unsupported fex input value %s' % (
                self.__class__.__name__,
                str(fex_input))
            raise RuntimeError(m)

        if not calib in ('em', 'had'):
            m = '%s._check_args: unsupported calibration value %s' % (
                self.__class__.__name__,
                str(calib))
            raise RuntimeError(m)


class AntiKt(JetFex):
    """ Argument checking class that holds the  parameters for a
    AntiKt python alg"""

    fex_type = 'antikt'

    def __init__(self, merge_param, calib, fex_input, level, **kargs):
        JetFex.__init__(self, merge_param, calib, fex_input, level)


class Cone(JetFex):
    """ Argument checking class that holds the  parameters for a
    Cone python alg"""

    fex_type = 'cone'

    def __init__(self, merge_param, calib, fex_input, level, **kargs):
        JetFex.__init__(self, merge_param, calib, fex_input, level)


class JetRec(JetFex):
    """ Argument checking class that holds the  parameters for JetRec"""

    fex_type = 'jetrec'

    def __init__(self, merge_param, calib, fex_input, level, **kargs):
        JetFex.__init__(self, merge_param, calib, fex_input, level)
