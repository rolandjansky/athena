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
        # ed: EnergyDensity
        # jr: jet rec
        # jh: EFJetHypo
        # jh_ht: HT hypo
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
                       'cmfs': self.make_cmfs,  # cell/cluster maker full scan
                       'ed': self.make_ed,  # energy density
                       'jr': self.make_jr_clusters,  # jet rec
                       'hijr': self.make_hijr,  # hi jet rec
                       'rc': self.make_jr_recluster,  # recluster jets
                       'jh': self.make_jh,  # jet hypo
                       'jh_ht': self.make_jh_ht,  # HT hypo
                       'jh_tla': self.make_jh_tla,  # TLA hypo
                       'ps': self.make_ps,  # partial scan Roi maker
                       'cm': self.make_cm,  # cell and cluster maker
                       'tt': self.make_tt,  # construct trigger tower objects
                       'hicm': self.make_hicm,  # hi cluster maker
                       # jets from trigger towers:
                       'jt': self.make_jr_triggertowers,
                       'jhd': self.make_jhd, # jet hypo diagnostics
                       'fexd': self.make_fexd,  # all pre-hypo diagnostics
                       'ds': self.make_datascouting,
                       }

        # object that produces Alg objects
        self.alg_factory = alg_factory

        #chain_config contains "Algorithm-ready" menu data
        self.chain_config = chain_config
        self.chain_name_esc = self.chain_config.chain_name.replace('.', '_')

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
            ('tc', 'FS'): ['fs', 'cmfs', 'ed', 'jr'],
            # ('tc', 'FS'): ['fs', 'cmfs', 'jr'],
            ('tc', 'PS'): ['ps', 'cm', 'jr'],
            ('ion', 'FS'): ['fs','hicm','hijr'],
            ('TT', 'FS'): ['tt', 'jt']}.get((data_type, scan_type), [])

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
        if self.chain_config.run_rtt_diags:
            # append the diagnostic sequences for the various fexes
            # seq_order.extend(['rd', 'ced', 'cld', 'jrd'])
            seq_order.append('fexd')

        # check that running the hypo has been requested
        if self.chain_config.run_hypo:
            hypo_type = self.chain_config.menu_data.hypo_params.hypo_type
            if hypo_type in ('HLThypo',
                             'HLTSRhypo',
                             'run1hypo',):
                seq_order.append('jh')
                if self.chain_config.run_rtt_diags:
                    # run the jet hypo doagnostic after the jet hypo - so we can
                    # see which jets are cut.
                    seq_order.append('jhd')
            elif hypo_type == 'HT':
                seq_order.append('jh_ht')
            elif hypo_type == 'tla':
                seq_order.append('jh_tla')
            else:
                
                msg = '%s._make_sequence_list: unknown hypo type %s ' % (
                    self.__class__.__name__, str(hypo_type))
                raise RuntimeError(msg)

        if self.chain_config.data_scouting:
            seq_order.append('ds')

        seq_order = tuple(seq_order)
        return seq_order


    def make_fs(self):
        """make full scan Alglist"""

        return AlgList(alg_list=self.alg_factory.fullscan_roi(),
                       alias='fullroi')


    def make_ps(self):
        """make partial scan Alglist"""

        alias='superroi_%s' % self.chain_config.seed
        return AlgList(alg_list=self.alg_factory.superRoIMaker(),
                       alias=alias)


    def make_cmfs(self):
        """Return cellmaker optimized for full scan and cluster maker"""

        cluster_params = self.chain_config.menu_data.cluster_params
        alias = 'cluster_%s' % cluster_params.cluster_label

        algs = []
        [algs.extend(f()) for f in (self.alg_factory.cellMaker_fullcalo_topo,
                                    self.alg_factory.topoClusterMaker,)
                                    # self.alg_factory.energyDensityAlg,)
        ]

        return AlgList(algs, alias=alias)


    def make_ed(self):
        """Return Energy Density Alg"""

        fex_params = self.chain_config.menu_data.fex_params
        alias = 'rho_04_%s' % fex_params.cluster_calib

        algs = []
        [algs.extend(f()) for f in (self.alg_factory.energyDensityAlg,)]

        return AlgList(algs, alias=alias)


    def make_cm(self):
        """Return cellmaker for non partial scan running.
        CellMaker does not use certain read-out optimization,
        and the EnergDensity Algorithm, which is used for pileup
        correction, and which needs the full cell collection, is not
        run."""

        # flag to do local cluster calibration
        cluster_params = self.chain_config.menu_data.cluster_params
        alias = 'cluster_%s' % cluster_params.cluster_label

        algs = []
        [algs.extend(f()) for f in (self.alg_factory.cellMaker_superPS_topo,
                                    self.alg_factory.topoClusterMaker)]
        return AlgList(alg_list=algs, alias=alias)
    
    #HI
    def make_hicm(self):
        cluster_params = self.chain_config.menu_data.cluster_params
        alias = 'cluster_%s' % cluster_params.cluster_label

        algs = []
        [algs.extend(f()) for f in (self.alg_factory.cellMaker_fullcalo_topo,
                                    self.alg_factory.hiCombinedTowerMaker,
                                    self.alg_factory.hiClusterMaker,
                                    self.alg_factory.hiEventShapeMaker,
                                   )]

        return AlgList(algs, alias=alias)


    #HI
    def make_hijr(self): #@@@
        """Make hijetrec sequence"""

        menu_data = self.chain_config.menu_data
        fex_params = menu_data.fex_params
        cluster_params = menu_data.cluster_params

        alias = 'hijetrec_%s' % fex_params.fex_label

        return AlgList(self.alg_factory.hijetrec_hic(), alias)

    
    def make_jr_triggertowers(self):
        """Make jetrec sequence suing trigger towers as input"""

        menu_data = self.chain_config.menu_data
        fex_params = menu_data.fex_params
        cluster_params = menu_data.cluster_params

        # set jes label according to whether the JES corrections will
        # be done by JetRecTool

        alias = 'jetrec_%s' % fex_params.fex_label

        return AlgList(self.alg_factory.jetrec_triggertowers(), alias)

    
    def make_jr_clusters(self):
        """Make jetrec sequence"""

        menu_data = self.chain_config.menu_data
        fex_params = menu_data.fex_params
        cluster_params = menu_data.cluster_params

        # set jes label according to whether the JES corrections will
        # be done by JetRecTool

        alias = 'jetrec_%s' % fex_params.fex_label

        return AlgList(self.alg_factory.jetrec_clusters(), alias)

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

        return AlgList(self.alg_factory.jetrec_recluster(), alias)

        # return AlgList([self.alg_factory.jetrec_cluster(),
        #                self.alg_factory.jetrec_jet()], alias)



    def make_jh(self):
        """Create an alg_list for 2015 JetRec hypo sequence"""

        menu_data = self.chain_config.menu_data
        hypo = menu_data.hypo_params

        alias_base = 'hypo_' + hypo.jet_attributes_tostring()
        alias_base += '_' + hypo.cleaner

        alias = alias_base  # copy
        
        function_map  = {
            'HLThypo': self.alg_factory.hlt_hypo,  # maxbipartitite matcher
            'HLTSRhypo': self.alg_factory.hlt_hypo,  # singleEtaRegion matcher
            'run1hypo': self.alg_factory.jr_hypo,
            }

        f = function_map.get(hypo.hypo_type, None)

        if f is None:
            msg = '%s._make_jh: unknown hypo_type: %s' % (
                self.__class__.__name__, str(hypo.hypo_type))
            raise RuntimeError(msg)

        alias += '_' + hypo.hypo_type

        return AlgList(f(), alias)


    def make_jh_ht(self):
        """Create an alg_list for 2015 JetRec hypo sequence"""

        menu_data = self.chain_config.menu_data
        hypo = menu_data.hypo_params
        alias = 'hypo_%s' % str(hypo.attributes_to_string())

        return AlgList(self.alg_factory.ht_hypo(), alias)


    def make_jh_tla(self):
        """Create an alg_list for the TLA hypo"""

        menu_data = self.chain_config.menu_data
        hypo = menu_data.hypo_params
        alias = 'hypo_%s' % str(hypo.tla_string)

        return AlgList(self.alg_factory.hlt_hypo_tla(), alias)


    def make_tt(self):
        """Create an alg_list for the trigger tower unpacker"""

        algs = self.alg_factory.tt_maker()
        return AlgList(alg_list=algs, alias='tt_maker')
        

    def make_jhd(self):
        diag_alg = self.alg_factory.jetHypoDiagnostics()

        # ensure diagnostics have te names according to the chain_name
        # else collosions.
        alias='jethypo_diagnostics_%s' % self.chain_name_esc

        return AlgList(alg_list=diag_alg,
                       alias=alias)


    def make_fexd(self):
        """make an Alglist of fex diagnostics -
        to be run before the hypo alg"""

        algs = []
        [algs.extend(f()) for f in (self.alg_factory.roiDiagnostics,
                                    self.alg_factory.cellDiagnostics,
                                    self.alg_factory.clusterDiagnostics,
                                    self.alg_factory.jetRecDiagnostics,
                                    self.alg_factory.jetRecDebug
                                )]

        # ensure diagnostics have te names according to the chain_name
        # else collosions.
        alias='jetfex_diagnostics_%s' % self.chain_name_esc

        return AlgList(alg_list=algs, alias=alias)


    def make_datascouting(self):

        # get the factory fn according to the data scouting flag
        ff = {'ds1': self.alg_factory.getDataScoutingAlgs1,
              'ds2': self.alg_factory.getDataScoutingAlgs2}.get(
                  self.chain_config.data_scouting)

        alias='data_scouting_%s' % self.chain_config.chain_name
        return AlgList(alg_list=ff(), alias=alias)
