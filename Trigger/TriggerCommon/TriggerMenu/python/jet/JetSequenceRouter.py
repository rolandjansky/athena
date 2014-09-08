# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" This module sets up the algorithm lists for sequences, and supplies
the list with a name (alias) which will be used in the construction if the
out trigger element name for the sequence.

The entrance point is make_alglists().

List of algorithms are represented by the AlgList class.

A set of string keys is into a dispatch table is made according to the
chain request (input data). The value side of the dispatch table are the
functions which create the Algorithm instances of a sequence.

The keys are looped over, and the correponding ***REMOVED***ions are called. These
return an AlgList objects containing the Algorithm instances of a single
sequence.

make_alglist finishes by connecting the AlgList objects together
in a tree structure. Currently ll such tree structures reduce to a linear
sequence of sequences.
"""

from collections import defaultdict
from algFactories import alg_dispatcher


class AlgList(object):
    def __init__(self, alg_list, alias, attach_to=''):
        self.alg_list = alg_list  # a list of Algorithm objects
        self.alias = alias  # a string used to give the trigger element name

        # allow sequence trees by specifing the sequence to follow
        self.attach_to = attach_to


class JetSequenceRouter(object):
    """Route incoming ChainConfig to the appropriate sequence
    creation functions"""

    def __init__(self):
        """Set up tables that determine how sequences are connected,
        and the dunctions used to construct sequences"""

        #
        # table giving legal sequence connections
        # fs: full scan
        # cmfs: cell/cluster maker using full scan tools in CellMaker
        # jr: jet rec
        # jh: EFJetHypo
        # ps: partial scan
        # cm: cell/cluster maker using non full scan tools in CellMaker
        # roi: use l1 RoI
        #
        # rd: RoI diagnistics (prints RoI data)
        # ced: cell diagnostics (for studies)
        # cld: cluster diagnostics (for studies)
        # jrd: jet rec diagnostics (for studies, looks at all jets)
        # jhd: jet hypo diagnostics (for studies, looks at hypo-selected jets)
        #
        # REMINDER: (1/6/2014)
        # in the future handling the trigger towers may exapnd
        # to including a hypo and analysis sequences
        #

        # set up a sequence key (str) to function to make an Alglist
        # table
        self.router = {'fs': _make_fs,  # full scan
                       'cmfs': _make_cmfs,  # cell maker full scan
                       'jr': _make_jr,  # jet rec
                       'jh': _make_jh,  # jet hypo
                       'ps': _make_ps,  # partial scan Roi maker
                       'cm': _make_cm,  # cell and cluster maker
                       'tt': _make_tt,  # trigger towers
                       'jhd': _make_jhd, # jet hypo diagnostics
                       'fexd': _make_fexd  # make all the pre-hypo diagnostics
                       }

    def make_alglists(self, chain_config):
        """Use the router table to send the incoming ChainConfig object
        to by the central menu to set up trigger sequences."""

        # find the sequences to build
        key = _make_sequence_list(chain_config)

        # build the list of algorithms for each sequence.
        fns = [self.router[label] for label in key]
        alg_lists = [fn(chain_config) for fn in fns]

        # set up the inter-sequence connections
        for i in range(1, len(alg_lists)):
            alg_lists[i].attach_to = alg_lists[i-1].alias

        return alg_lists


def _make_sequence_list(chain_config):
    """Determine which sequences are to be constructed. Each sequence
    is represented by a string label"""

    # return ('fs', 'cmfs', 'jr')  # DEBUG
    scan_type = chain_config.jr_menudata.scan_type

    if (chain_config.tt_menudata):
        seq_order = ['tt', 'cm', 'jr']
    elif scan_type == 'FS':
        seq_order = ['fs', 'cmfs', 'jr']
    elif scan_type == 'PS':
        seq_order = ['ps', 'cm', 'jr']
    elif scan_type == '':
        seq_order = ['roi', 'jr']
    else:
        msg = '%s._make_sequence_list: cannot determine sequence order: %s %s'
        msg = msg % (str(chain_config.tt_menudata), str(scan_type))
        raise RuntimeError(msg)

    # fex diagnostics are run before the hypo, otherwise they will not see
    # features before cuts.
    if chain_config.test:
        # append the diagnostic sequences for the various fexes
        # seq_order.extend(['rd', 'ced', 'cld', 'jrd'])
        seq_order.append('fexd')

    # check that running the hypo has been requested
    if chain_config.run_hypo:
        seq_order.append('jh')
        if chain_config.test:
            # run the jet hypo doagnostic after the jet hypo - so we can
            # see which jets are cut.
            seq_order.append('jhd')

    seq_order = tuple(seq_order)
    return seq_order


def _make_fs(chain_config):
    """make full scan Alglist"""

    return AlgList(alg_list=[alg_dispatcher['roi']()], alias='full')


def _make_ps(chain_config):
    """make partial scan Alglist"""

    return AlgList(alg_list=[alg_dispatcher['superRoIMaker']()], alias='super')


def _make_cmfs(chain_config):
    """Return cellmaker optimized for full scan and cluster maker"""

    return AlgList(alg_list=[alg_dispatcher['cellMaker_fullcalo_topo'](),
                             alg_dispatcher['topoClusterMaker_fullcalo']()],
                   alias='cluster')


def _make_cm(chain_config):
    """Return cellmaker for non full scan running"""

    return AlgList(alg_list=[alg_dispatcher['cellMaker_superPS_topo'](),
                             alg_dispatcher['topoClusterMaker_partial']()],
                   alias='ps_cluster')

    
def _make_jr(chain_config):
    """Make jetrec sequence"""

    fex_key = chain_config.jr_menudata.fex.fex_key
    return AlgList([alg_dispatcher[fex_key]()], fex_key)


def _make_jh(chain_config):
    """Create an alg_list for 2015 JetRec hypo sequence"""

    menudata = chain_config.jr_menudata
    hypo = menudata.hypo
    single_jet = len(hypo.jet_attributes) == 1
    if single_jet:
        hypo_key = 'single'
    else:
        hypo_key = 'multi'

    hypo_key = 'jr_hypo_%s' % hypo_key

    alias = hypo.jet_attributes_tostring()
    # return a list of algs that form the jr sequence
    return AlgList([alg_dispatcher[hypo_key](menudata)], alias)


def _make_tt(chain_config):
    """Create an alg_list for 2012/2015 trigger tower scan fex sequence"""

    menudata = chain_config.tt_menudata
    fex = menudata.fex

    tt_unpacker_key = 'l15_unpacking_%s' % (fex.fex_input)
    tt_fex_key = fex.fex_key

    unpacker = alg_dispatcher[tt_unpacker_key]()
    fex = alg_dispatcher[tt_fex_key]()

    # no hypos yet
    # hypo_key, hypo = _make_tt_hypo(menudata)
    # if hypo:
    #    return [AlgList(alg_list=[unpacker, fex, hypo], alias=hypo_key)]

    return AlgList(alg_list=[unpacker, fex], alias=tt_fex_key)


def _make_jhd(chain_config):
    diag_alg = alg_dispatcher['jetHypoDiagnostics'](chain_config)
    return AlgList(alg_list=[diag_alg], alias='jethypo_diag')


def _make_fexd(chain_config):
    """make an Alglist of fex diagnostics - to be run before the hypo alg"""
    rd = alg_dispatcher['roiDiagnostics'](chain_config)
    ced = alg_dispatcher['cellDiagnostics'](chain_config)
    cld = alg_dispatcher['clusterDiagnostics'](chain_config)
    jrd = alg_dispatcher['jetRecDiagnostics'](chain_config)
    return AlgList(alg_list=[rd,
                             ced,
                             cld,
                             jrd], alias='fexd_%s' % chain_config.name)
