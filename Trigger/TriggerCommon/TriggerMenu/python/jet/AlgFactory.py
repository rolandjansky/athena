# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Provide a  look up tables that connects logical algortihm names to
factory functions and arguments.

The table is used to determine the function to used to return an Algorithm
proxy object.

The factory functions are static methods of the _AlgFactory class.
Static methods: methods of a class that make no use of instance variables.

"""

try:
    from AthenaCommon.SystemOfUnits import GeV
except ImportError:
    GeV = 1000.


class Alg(object):
    """Proxy class for ATLAS python configuration class (APCC). Can
    by converted to an APCC using an instantiator."""

    def __init__(self, factory, args, kargs, manual_attrs={}):
        """Hold the information needed to construct a string that can be
        passed to eval() to instantiate a python object to control an
        ATLAS Athena algorithm.
            
        This decouples assemblying the information to instantiate
        the python object from thre instantiation.
        
        This indirection allows the configuration code to be run in a mode
        where no such objects are created, which allows, e.g.,
        for development to be done when if instantiation is not possible,
        or for debug purposes
        where the creation string can be examined.

        The following information is stored:
        - the factory function (usally the class name) 
        used to obtain an instance of the Athena Alg python class

        - arguments which can be passed to teh construtor as
        *list or **kwds

        - "manual_attributes " which are set post instantiation using
        the dot operator (ie a.sttr_name = attr_value). The attribute
        values themselves will be passed to eval before setting them in
        the newly instantiated object."""

        # factory function used to instantiate the APCC. Usually the class
        # name of the APCC
        self.factory = factory

        # arguments and keyword arguments for the factory function
        n_args = []
        for a in args:
            try:
                n_args.append(int(a))
            except ValueError:
                try:
                    n_args.append(float(a))
                except ValueError:
                    n_args.append(a)

        self.args = tuple(n_args)

        self.kargs = []
        for k in sorted(kargs.keys()):
            self.kargs.append('%s=%s' % (k, kargs[k]))

        self.manual_attrs = manual_attrs

    def getName(self):
        return self.factory

    def __repr__(self):
        return self.factory

    def __str__(self):
        text = [self.factory]
        text.extend(['arg: %s' % a for a in self.args])
        text.extend(['karg: %s' % k for k in self.kargs])
        text.extend(['manual attr: %s' % k for k in self.manual_attrs])
        return '\n'.join(text)

    def asString(self):
        args = list(self.args)
        args.extend(self.kargs)
        args = [str(a) for a in args]
        args = ', '.join(args)
        # import pdb; pdb.set_trace()
        s = '%s(%s)' % (self.factory, args)
        return s


class AlgFactory(object):
    def __init__(self, chain_config):
        
        #chain_config contains "Algorithm-ready" menu data
        self.chain_config = chain_config
        self.menu_data = chain_config.menu_data
        self.cluster_params = self.menu_data.cluster_params
        self.fex_params = self.menu_data.fex_params
        self.recluster_params = self.menu_data.recluster_params

    def tt_unpacker(self):
        return Alg('T2L1Unpacking_TT', (), {})

    def fullscan_roi(self):
        return Alg('DummyAlgo', ('"RoiCreator"',), {})

    def jetrec(self):
        """Instantiate a python object for TrigHLTJetRec. Incoming
        fex_data is of type MenuData"""

        merge_param_str = str(self.fex_params.merge_param).zfill(2)
    
        cluster_calib = "'LC'" if self.cluster_params.do_lc else "'EM'"

        # assign a name which identifies the fex sequence and
        # the python class to be instantiated.
        name = "'TrigHLTJetRec_param_%s'" % (self.fex_params.fex_label)

        kwds = {
            'name': name,
            'merge_param': "'%s'" % merge_param_str,
            'jet_calib': "'%s'" % self.fex_params.jet_calib,
            'cluster_calib': cluster_calib,
            'output_collection_label': "'%s'" % (
            self.fex_params.fex_label)
        }

        return Alg('TrigHLTJetRec_param', (), kwds)

    def jetrec_cluster(self):
        """Instantiate a python object for TrigHLTJetRec that will
        use xAOD::CaloCluster s as input."""

        merge_param_str = str(self.fex_params.merge_param).zfill(2)
    
        cluster_calib = "'LC'" if self.cluster_params.do_lc else "'EM'"

        factory = 'TrigHLTJetRecFromCluster'
        # add factory to instance label to facliltate log file searches
        name = '"%s_%s"' %(factory, self.fex_params.fex_label)

        kwds = {
            'name': name,  # instance label
            'merge_param': "'%s'" % merge_param_str,
            'jet_calib': "'%s'" % self.fex_params.jet_calib,
            'cluster_calib': cluster_calib,
            'output_collection_label': "'%s'" % (
            self.fex_params.fex_label)
        }

        return Alg(factory, (), kwds)


    def jetrec_recluster(self):
        """Instantiate a python object for TrigHLTJetRec that will
        use xAOD::Jet s as input."""

        merge_param_str = str(self.recluster_params.merge_param).zfill(2)
    
        cluster_calib = "'LC'" if self.cluster_params.do_lc else "'EM'"

        factory = 'TrigHLTJetRecFromJet'
        # add factory to instance label to facliltate log file searches
        name = '"%s_%s"' %(factory, self.recluster_params.fex_label)

        kwds = {
            'name': name,  # instance label
            'merge_param': "'%s'" % merge_param_str,
            'jet_calib': "'%s'" % self.recluster_params.jet_calib,
            'cluster_calib': cluster_calib,
            'output_collection_label': "'%s'" % (
                self.recluster_params.fex_label),
            # ptMinCut and etaMaxCut are cuts applied to the
            # input jets before the reclustering jet finding is done.
            'ptMinCut': self.recluster_params.ptMinCut * GeV,
            'etaMaxCut': self.recluster_params.etaMaxCut,
        }

        return Alg(factory, (), kwds)

    def jr_hypo_single(self):

        hd = self.chain_config.menu_data.hypo_params
        assert len(hd.jet_attributes) == 1
        ja = hd.jet_attributes[0]

        chain_name = hd.chain_name
        chain_name = chain_name.replace("_boffperf","")
        chain_name = chain_name.replace("_bperf","")
        chain_name = chain_name.replace("_split","")
        chain_name = chain_name.replace("_EFID","")

        # make a new hypo instance by specifying a unique name
        name = '"EFJetHypo_ef_%s"' % chain_name

        # Gaudi does not allow '.' in instance names
        name = name.replace('.', '_')

        etaMin = str(ja.eta_min)
        etaMax = str(ja.eta_max)
        kargs = {}
    
        args = [name, str(GeV * ja.threshold), etaMin, etaMax]
    
        return Alg('EFJetHypo', args, kargs)

    def jr_hypo_multi(self):

        hypo = self.menu_data.hypo_params

        etaMin = hypo.jet_attributes[0].eta_min
        etaMax = hypo.jet_attributes[0].eta_max
    
        kargs = {'multiplicity': len(hypo.jet_attributes),
                 'ef_thrs': [j.threshold * GeV for j in hypo.jet_attributes],
                 'etaMin': etaMin,
                 'etaMax': etaMax,
             }

        str_mult = hypo.jet_attributes_tostring()

        # make a new hypo instance by specifying a unique name
        name = '"EFCentFullScanMultiJetHypo_ef_%s"' % (
            self.chain_config.chain_name)
        

        # Gaudi does not allow '.' in instance names
        name = name.replace('.', '_')

        return Alg(
            'EFCentFullScanMultiJetHypo',
            (name,),
            kargs)

    def superRoIMaker(self):
        factory = 'SeededAlgo'
        return Alg(factory,
                   (),
                   {'UseRoiSizes':False,
                    'EtaHalfWidth':0.5,
                    'PhiHalfWidth':0.5})

    def cellMaker_superPS_topo(self):
        """assign a name which identifies the fex sequence and
        the python class to be instantiated. The cell maker
        python class used here differs from the full scan case due
        to the absence of a read-out optimization present in the
        full scan case."""
        
        class_name = 'TrigCaloCellMaker_jet_super'
        instance_name = '"%s"' % 'PS'

        return Alg(class_name,
                   (instance_name,
                    'doNoise=0',
                    'AbsE=True'),
                   {})

    def cellMaker_fullcalo_topo(self):
        # assign a name which identifies the fex sequence and
        # the python class to be instantiated.

        class_name = 'TrigCaloCellMaker_jet_fullcalo'
        instance_name = '"%s"' % 'FS'

        return Alg(class_name,
                   (instance_name,
                    'doNoise=0',
                    'AbsE=True',
                    'doPers=True'),
                   {})

    def topoClusterMaker(self):
        
        # assign a name which identifies the sequence the Algorithm
        # will be placed in and the python class to be instantiated.

        class_name = 'TrigCaloClusterMaker_topo'
        instance_name = '"%s_%s"' % (class_name,
                                     self.cluster_params.cluster_label)

        return Alg(class_name,
                   (instance_name,),
                   {'doMoments': True,
                    'doLC': self.cluster_params.do_lc})
        
    def roiDiagnostics(self):
        factory = 'TrigHLTRoIDiagnostics'
        return Alg(factory, (), {})

    def jetRecDiagnostics(self):
        chain_name = self.chain_config.chain_name
        factory = 'TrigHLTJetDiagnostics_named'
        kwds = {'name': "'TrigHLTJetDiagnostics_%s'" % chain_name,
                'chain_name': "'%s'" % chain_name}
        return Alg(factory, (), kwds)

    def jetHypoDiagnostics(self):
        chain_name = self.chain_config.chain_name
        factory = 'TrigHLTHypoDiagnostics_named'
        kwds = {'name': "'TrigHLTHypoDiagnostics_%s'" % chain_name,
                'chain_name': "'%s'" % chain_name}
        return Alg(factory, (), kwds)


    def clusterDiagnostics(self):
        chain_name = self.chain_config.chain_name
        factory = 'TrigHLTClusterDiagnostics_named'
        kwds = {'name': "'TrigHLTClusterDiagnostics_%s'" % chain_name,
                'chain_name': "'%s'" % chain_name}
        return Alg(factory, (), kwds)


    def cellDiagnostics(self):
        chain_name = self.chain_config.chain_name
        factory = 'TrigHLTCellDiagnostics_named'
        kwds = {'name': "'TrigHLTCellDiagnostics_%s'" % chain_name,
                'chain_name': "'%s'" % chain_name}
        return Alg(factory, (), kwds)


    def energyDensityAlg(self):

        factory = 'TrigHLTEnergyDensity'

        # cluster_calib is a string that will be used to label
        # the PseudoJetGetter which is used by the offline software.
        # The name of the PSG will be '%sTopo' % cluster_calib.
        # The possible names are in JetContainerInfo.h (maybe in the JetOnput
        # enum?). And thus limited to a finite set of values. "LC" and "EM"
        # are strings in this set.

        cluster_calib = {
            True: "'LC'",
            False: "'EM'"}.get(self.cluster_params.do_lc)

        # assign a name which identifies the fex sequence and
        # the python class to be instantiated.
        ed_merge_param = 0.4


        name = '"%s_%s%s"' % (
            factory, 
            str(int(10 * ed_merge_param)).zfill(2),
            self.cluster_params.cluster_label,
        )

        # we do not carry the energy density calculation merge_param
        # so hard wire it here (to be fixed). This is not the fex
        # merge param!
        kwds = {'name':  name,
                'cluster_calib': cluster_calib,
                'ed_merge_param': ed_merge_param
            }
    
        return Alg(factory, (), kwds)


    def dataScoutingAlg1(self):
        factory = 'ScoutingStreamWriter'
        manual_attrs ={
            'CollectionTypeName':
            str(['xAOD::JetContainer_v1#TrigHLTJetRec', 
                 'xAOD::JetTrigAuxContainer_v1#TrigHLTJetRecAux'])} 

        return Alg(factory, ("'JetDataScouting'",), {}, manual_attrs)


    def dataScoutingAlg2(self):
        factory = 'ScoutingStreamWriter'
        manual_attrs ={
            'CollectionTypeName': 
            str(['xAOD::JetContainer_v1#TrigHLTJetRec', 
                 'xAOD::JetTrigAuxContainer_v1#TrigHLTJetRecAux'])} 

        return Alg(factory, ("'JetDataScouting'",), {}, manual_attrs)




# old code....
 # NEEDS UPDATING FOR ETA RANGE HANDLING
 # def jr_hypo_testCleaning(params):
 # 
 #     assert len(params.hypo_data) == 1
 #     hd = params.hypo_data[0]
 # 
 #     if hd.eta_region == 'j':
 #         factory = 'EFJetHypo_doBasicCleaning'
 #         arg0 = '"EFJetHypo_doBasicCleaning_%s"' % hd.sig
 #     elif hd.eta_region == 'fj':
 #         factory = 'EFFwdJetHypo_doBasicCleaning'
 #         arg0 = '"EFFwdJetHypo_doBasicCleaning_%s"' % hd.sig
 #     else:
 #         assert False
 # 
 #     return Alg(factory, (arg0, str(GeV * hd.threshold)), {})
 # 
 # 
