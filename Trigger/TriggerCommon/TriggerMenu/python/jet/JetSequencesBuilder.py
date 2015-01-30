# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" This module sets up the algorithm lists for sequences, and supplies
the list with a name (alias) which will be used later in the construction
of the out trigger element name for the sequence.

The entrance point is make_alglists().

Lists of algorithms are represented by the AlgList class.


Alg instances are created by the alg_factory attribute which is of type
AlgFactory.

Alg instances can themselves be used to instantiate ATLAS
python confuiguration classes using an appropriate Instantiator object,
or remain as they are to provide convenient visualization of Algorthm
factory functions and their arguments.

make_alglists determines which sequences to create based on information
in the chain_config (type ChainConfig) attribute, and uses the alg_factory
to supply the required Alg instances that make up the sequences.

An AlgList objects containing the Algorithm instances of a single
sequence is are created.

make_alglist finishes by connecting the AlgList objects together
in a tree structure. Currently ll such tree structures reduce to a linear
sequence of sequences.
"""


class AlgList(object):
    def __init__(self, alg_list, alias, attach_to=''):
        self.alg_list = alg_list  # a list of Algorithm objects
        self.alias = alias  # a string used to give the trigger element name

        # allow sequence trees by specifing the sequence to follow
        self.attach_to = attach_to


class JetSequencesBuilder(object):
    """Route incoming ChainConfig to the appropriate sequence
    creation functions"""

    def __init__(self, alg_factory, chain_config):
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
        # ds: data scouting sequence requested
        #
        # rd: RoI diagnistics (prints RoI data)
        # ced: cell diagnostics (for studies)
        # cld: cluster diagnostics (for studies)
        # jrd: jet rec diagnostics (for studies, looks at all jets)
        # jhd: jet hypo diagnostics (for studies, looks at hypo-selected jets)
        #
        # REMINDER: (1/6/2014)
        # in the future handling the trigger towers may expand
        # to including a hypo and analysis sequences
        #

        # set up a sequence key (str) to function to make an Alglist
        # table
        self.router = {'fs': self.make_fs,  # full scan
                       'cmfs': self.make_cmfs,  # cell maker full scan
                       'jr': self.make_jr_clusters,  # jet rec
                       'rc': self.make_jr_recluster,  # recluster jets
                       'jh': self.make_jh,  # jet hypo
                       'ps': self.make_ps,  # partial scan Roi maker
                       'cm': self.make_cm,  # cell and cluster maker
                       'tt': self.make_tt,  # trigger towers
                       'jhd': self.make_jhd, # jet hypo diagnostics
                       'fexd': self.make_fexd,  # all pre-hypo diagnostics
                       'ds': self.make_datascouting,
                       }

        # object that produces Alg objects
        self.alg_factory = alg_factory

        #chain_config contains "Algorithm-ready" menu data
        self.chain_config = chain_config

    def make_alglists(self):
        """Entrance point to the module. From the chain_config argument,
        determine which sequences are to be run, and in which order.
        The Alforithms assigned to each sequence are specified
        in the functions after this class."""

        # find the sequences to build
        key = self.make_sequence_list()

        # build the list of algorithms for each sequence.
        fns = [self.router[label] for label in key]
        alg_lists = [fn() for fn in fns]

        # set up the inter-sequence connections
        for i in range(1, len(alg_lists)):
            alg_lists[i].attach_to = alg_lists[i-1].alias

        return alg_lists


    def make_sequence_list(self):
        """Determine which sequences are to be constructed. Each sequence
        is represented by a string label"""

        # return ('fs', 'cmfs', 'jr')  # DEBUG
        menu_data = self.chain_config.menu_data
        data_type = menu_data.data_type
        scan_type = menu_data.scan_type
        seq_order = {
            ('tc', 'FS'): ['fs', 'cmfs', 'jr'],
            ('tc', 'PS'): ['ps', 'cm', 'jr'],
            ('TT', 'FS'): ['tt', 'jr']}.get((data_type, scan_type), [])

        if not seq_order:
            msg = '%s._make_sequence_list: cannot determine sequence'\
                  ' order: %s %s'
            msg = msg % (str(menu_data), str(scan_type))
            raise RuntimeError(msg)

        # add jet reclustering sequence if requested
        if menu_data.recluster_params is not None:
            seq_order.append('rc')

        # fex diagnostics are run before the hypo,
        # otherwise they will not see
        # features before cuts.
        if self.chain_config.test:
            # append the diagnostic sequences for the various fexes
            # seq_order.extend(['rd', 'ced', 'cld', 'jrd'])
            seq_order.append('fexd')

        # check that running the hypo has been requested
        if self.chain_config.run_hypo:
            seq_order.append('jh')
            if self.chain_config.test:
                # run the jet hypo doagnostic after the jet hypo - so we can
                # see which jets are cut.
                seq_order.append('jhd')

        if self.chain_config.data_scouting:
            seq_order.append('ds')

        seq_order = tuple(seq_order)
        return seq_order


    def make_fs(self):
        """make full scan Alglist"""

        return AlgList(alg_list=[self.alg_factory.fullscan_roi()],
                       alias='fullroi')


    def make_ps(self):
        """make partial scan Alglist"""

        return AlgList(alg_list=[self.alg_factory.superRoIMaker()],
                       alias='superroi')


    def make_cmfs(self):
        """Return cellmaker optimized for full scan and cluster maker"""

        cluster_params = self.chain_config.menu_data.cluster_params
        alias = 'cluster_%s' % cluster_params.cluster_label

        # the EnergyDensity tool is placed in this sequence.
        # it is expected that it will always be run with
        # a radius of 0.4 (it does its own jet finding).
        # if this is ever varied, it should go into ists own
        # sequence.
        return AlgList(
            alg_list=[self.alg_factory.cellMaker_fullcalo_topo(),
                      self.alg_factory.topoClusterMaker(),
                      self.alg_factory.energyDensityAlg(),
                  ],
            alias=alias)


    def make_cm(self):
        """Return cellmaker for non partial scan running.
        CellMaker does not use certain read-out optimization,
        and the EnergDensity Algorithm, which is used for pileup
        correction, and which needs the full cell collection, is not
        run."""

        # flag to do local cluster calibration
        cluster_params = self.chain_config.menu_data.cluster_params
        alias = 'cluster_%s' % cluster_params.cluster_label
        return AlgList(
            alg_list=[self.alg_factory.cellMaker_superPS_topo(),
                      self.alg_factory.topoClusterMaker(),
                  ],
            alias=alias)

    
    def make_jr_clusters(self):
        """Make jetrec sequence"""

        menu_data = self.chain_config.menu_data
        fex_params = menu_data.fex_params
        cluster_params = menu_data.cluster_params

        # set jes label according to whether the JES corrections will
        # be done by JetRecTool

        alias = 'jetrec_%s' % fex_params.fex_label

        return AlgList([self.alg_factory.jetrec_cluster()], alias)

        # return AlgList([self.alg_factory.jetrec_cluster(),
        #                self.alg_factory.jetrec_jet()], alias)


    
    def make_jr_recluster(self):
        """Make jetrec sequence"""

        menu_data = self.chain_config.menu_data
        recluster_params = menu_data.recluster_params
        cluster_params = menu_data.cluster_params

        # set jes label according to whether the JES corrections will
        # be done by JetRecTool

        alias = 'jetrec_%s' % recluster_params.fex_label

        return AlgList([self.alg_factory.jetrec_recluster()], alias)

        # return AlgList([self.alg_factory.jetrec_cluster(),
        #                self.alg_factory.jetrec_jet()], alias)



    def make_jh(self):
        """Create an alg_list for 2015 JetRec hypo sequence"""

        menu_data = self.chain_config.menu_data
        hypo = menu_data.hypo_params
        single_jet = len(hypo.jet_attributes) == 1
        if single_jet:
            hypo_key = 'single'
        else:
            hypo_key = 'multi'

        hypo_key = 'jr_hypo_%s' % hypo_key

        alias = 'hypo_' + hypo.jet_attributes_tostring()
        
        # return a list of algs that form the jr sequence
        if single_jet:
            return AlgList([self.alg_factory.jr_hypo_single()], alias)
        else:
            return AlgList([self.alg_factory.jr_hypo_multi()], alias)


    def make_tt(self):
        """Create an alg_list for the trigger tower unpacker"""

        unpacker = self.alg_factory.tt_unpacker()
        return AlgList(alg_list=[unpacker], alias='tt_unpacker')
        

    def make_jhd(self):
        diag_alg = self.alg_factory.jetHypoDiagnostics()
        return AlgList(alg_list=[diag_alg], alias='jethypo_diag')


    def make_fexd(self):
        """make an Alglist of fex diagnostics -
        to be run before the hypo alg"""

        rd = self.alg_factory.roiDiagnostics()
        ced = self.alg_factory.cellDiagnostics()
        cld = self.alg_factory.clusterDiagnostics()
        jrd = self.alg_factory.jetRecDiagnostics()
        
        return AlgList(alg_list=[rd,
                                 ced,
                                 cld,
                                 jrd],
                       alias='fexd_%s' % self.chain_config.chain_name)


    def make_datascouting(self):

        # get the factory fn according to the data scouting flag
        ff = {'ds1': self.alg_factory.dataScoutingAlg1,
              'ds2': self.alg_factory.dataScoutingAlg2}.get(
                  self.chain_config.data_scouting)

        alias='data_scouting_%s' % self.chain_config.chain_name
        return AlgList(alg_list=[ff()], alias=alias)
