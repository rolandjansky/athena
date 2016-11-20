# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Reformat input dictionary. Dictionary structure at
https://svnweb.cern.ch/trac/atlasoff/browser/Trigger/TriggerCommon/ \
TriggerMenu/trunk/python/menu/SignatureDicts.py#L144
"""

import re
from fexparams_factory import fexparams_factory
from hypo_factory import hypo_factory
from clusterparams_factory import clusterparams_factory
from MenuData import MenuData
from ChainConfig import ChainConfig
from eta_string_conversions import eta_string_to_floats

class JetAttributes(object):
    """Per jet attributes. Used by  hypo algorithms."""

    def __init__(self, threshold, eta_range):
        self.threshold = threshold
        self.eta_range = eta_range  # string like '0eta320'
        # eta_min, eta_max are floats
        self.eta_min, self.eta_max = eta_string_to_floats(eta_range)

        self.asymmetricEta = 1 if (eta_range.startswith('n') or
                                   eta_range.startswith('p')) else 0
                                      
    def __str__(self):
        return 'thresh: %s eta_min: %s eta_max: %s' % (str(self.threshold),
                                                       str(self.eta_min),
                                                       str(self.eta_max))


class ChainConfigMaker(object):
    """Process input dictionary to create a ChainConfig object.
    A Chainfig obeject carries "algorithm-ready"" arguments (derived
    from the incoming dictionary) arranged by algorithm type.

    
    ChainConfigMaker.__init__ method acquires the incoming dictionary from
    the menu code, performs consistancy checks (eg the chainParts of the input
    dictionary are highly redundant. By affirmation of the menu code author
    many entries in the chain part dictionary will have the same values.
    If this is not the case, there is an error, or the chainParts has acquired
    new, currently unknown, semantics), and transforms its values into values
    directly usable by the Algoritm classes. These variables are attributes
    of this class.

    When the  function method (__call__) is invoked, these atrributes
    are transferred to a lighter weight ChainConfig object."""

    reco_alg_re = re.compile(r'^a\d+$')
    recluster_alg_re = re.compile(r'^a\d+r$')
    trimming_alg_re = re.compile(r'^a\d+t$')
    tla_re = \
        re.compile(r'^(?P<indexlo>\d+)i(?P<indexhi>\d+)c(?P<mass_min>\d+)m(?P<mass_max>\d+)TLA$')
    invm_re = re.compile(r'^invm(?P<mass_min>\d+)$')
    deta_re = re.compile(r'^deta(?P<dEta_min>\d+)$')

    # conerverter for cleaner names used in chain anmes, and those
    # used to instantiate the C++ converters (cleanerFactory keys)
    
    cleaner_names = {
        'cleanL': 'looseCleaning',
        'cleanLA': 'antiLooseCleaning',
        'cleanT': 'tightCleaning',
        'cleanTA': 'antiTightCleaning',
        'cleanLLP': 'llpCleaning',
        'cleanLLPA':'antLlpCleaning',
        'noCleaning': 'noCleaning',
        }

    def __init__(self, d):
            
        self.err_hdr = '%s._process_part: ' % self.__class__.__name__
        self.n_parts = 0

        self.data_type = ''
        self.scan_type = ''
        self.multi_eta = len(set([cp['etaRange'] for cp in d['chainParts']])) > 1
        self.jet_attributes = []
        self.run_rtt_diags = d['run_rtt_diags']
        self.chain_name = d['chainName']
        self.seed = d['L1item']
        self.deta = None
        self.invm = None
        # error check: count the number of hypos requested. Should
        # only be one.
        self.hypo_types = []
        [self._process_part(p) for p in d['chainParts']]

        # self._ensure_single_hypo()
        
    # def _ensure_single_hypo(self):
    #     """hypo specification is spread over > 1 chain parts
    #     (multi Et levels for the Eta-pt hypo) or specified
    #     in a single chainpart (eg TLA hypo). Make sure that
    #     not more than one hypo has been specied."""

    #      htypes = set(self.hypo_types)
    #     counter = {}
    #     for ht in htypes: counter[ht] = 0
    #     for ht in self.hypo_types:
    #         if ht in ('HLThypo',
    #                   'HLThypo2_etaet',
    #                   'HLThypo2_dimass_deta',
    #                   'HLThypo2_ht'
    #                   'HLThypo2_tla'
    #                   'HLTSRhypo', 'run1hypo'):
    #             counter[ht] = 1
    #         else:
    #             counter[ht] += 1

    #      nhypos = sum(counter.values())

    #      if nhypos > 1:
    #         raise RuntimeError('%s  %d hypo types calculated: %s' % (
    #             self.err_hdr, nhypos, str(counter)))
            
    def _process_part(self, part):
        """Process chain parts. If there is more than one chain part,
        the fex data must always be the same: multiple chain parts
        convey information about different thresholds in for the
        jet hypo"""

        self._check_part(part)

        # ----- test chain parameter - if present, sill be  be in the 'extra' field
        extra = part['extra']
        if extra.startswith('test'):
            self.check_and_set('test_flag', extra)
        else:
            self.check_and_set('test_flag', '')

        # -------------- cluster parameters -------------

        # the following dictionary translates the incoming value of
        # the calibration string to arguments relevant for
        # python class instantiation. The first is used by TrigCaloClusterMaker
        # while the second affects how JetRecTool is set up by
        # TrigHLTJetRec.

        # calib = {
        #    'em': {'cluster_dolc': False, 'do_jes': False},
        #    'lcw': {'cluster_dolc': True, 'do_jes': False},
        #    'had': {'cluster_dolc': False, 'do_jes': True}
        # }

        cluster_calib = part['calib']
        assert cluster_calib in ('em', 'lcw')
        
        # 25/02/2016 cluster will aways be made with local calibration on.
        # cluster_calib will be used only by the TrigJetReco to
        # decide if the the clusters should be switched to uncalibrated
        self.check_and_set('cluster_calib', cluster_calib)

        # the cluster algorithm is always run with local calibration on
        cluster_label = reduce(lambda x, y: x + y,
                               (part['dataType'],
                                'lcw',
                                part["scan"]))
        self.check_and_set('cluster_label', cluster_label)


        # the input data changes meaning depending on the chain,
        # all in the interest of having chain names that are alledgedly
        # easy for analysts to understand...
        # 13/01/2015... jet reclustering changes the meaning ofthe
        # dictionary contents. Straighten this out here to allow
        # systematic use of the dictionary.
        
        match = self.recluster_alg_re.search(part['recoAlg'])
        if match:
            # reclustering has been requested - fix dict:
            self.check_and_set('do_recluster', True)

            # ------- recluster parameters ----------
            # - the Et cut on input jets is hardwired.
            # - the eta cut on input jets is hardwired.
            # - the algo is calculated from the input dictionary value

            self.check_and_set('recl_fex_name', 'antikt')
            self.check_and_set('recl_ptMinCut', 15.)
            self.check_and_set('recl_etaMaxCut', 10.0)  # no effect, to be removed
            self.check_and_set('recl_merge_param', part['recoAlg'][1:-1])
            self.check_and_set('recl_jet_calib', 'nojcalib')
            self.check_and_set('recl_fex_alg_name', part["recoAlg"])

            fex_label = reduce(lambda x, y: x + y,
                           (part["recoAlg"],
                            '_',
                            part["dataType"],
                            self.cluster_calib,
                            part["jetCalib"],
                            part["scan"]))

            self.check_and_set('recl_fex_label', fex_label)
            # - input jets are made from akt4
            part['recoAlg'] = 'a4'
        else:
            self.check_and_set('do_recluster', False)


	# Check for trimming regular expression match
        match = self.trimming_alg_re.search(part['recoAlg'])
        if match:
            self.check_and_set('do_trim',True)
            print 'doTrimming is True' #Nima!
            # ----- trimming parameters -----
            #self.check_and_set('trim_fex_name', 'antikt')
            self.check_and_set('trim_rclus',0.2)
            self.check_and_set('trim_ptfrac',0.05)
            #self.check_and_set('trim_merge_param', part['recoAlg'][1:-1])
            #self.check_and_set('trim_jet_calib', part['jetCalib'])
            #self.check_and_set('trim_fex_alg_name', part["recoAlg"])

            fex_label = reduce(lambda x, y: x + y,
                               (part["recoAlg"],
                                '_',
                                part["dataType"],
                                self.cluster_calib,
                                part["jetCalib"],
                                part["scan"]))
            self.check_and_set('trim_fex_label', fex_label)
            # Input jets are aktX ungroomed (change from 'aXt' to 'aX', typically X=10)
            part['recoAlg'] = part['recoAlg'][0:-1]
        else:
            self.check_and_set('do_trim',False)
            print 'doTrimming is false' #Nima!

        jet_calib = part['jetCalib']
        self.check_and_set('jet_calib', jet_calib)
        
        # find the type of data the clustering will act on: topoclusters
        # or trigger towers are the possibilities for now.
        data_type = part['dataType']
        self.check_and_set('data_type', data_type)

        # -------------- fex parameters ----------------

        reco_alg = part['recoAlg']
        m = self.reco_alg_re.search(reco_alg)
        if not m:
            msg = '%s unknown reco alg: %s' % (self.err_hdr, reco_alg)
            raise RuntimeError(msg)

        merge_param = int(reco_alg[1:])
        self.check_and_set('fex_name', 'antikt')
        self.check_and_set('merge_param', merge_param)
        self.check_and_set('dataType', part['dataType'])
        self.check_and_set('fex_alg_name', part["recoAlg"])
        # generate a string that will be used to label the fex
        # instance and associated entities.
        fex_label = reduce(lambda x, y: x + y,
                           (self.fex_alg_name,
                            self.data_type,
                            self.cluster_calib,
                            part["jetCalib"],
                            part["scan"]))

        self.check_and_set('fex_label', fex_label)


        # ------- data scouting parameters ----------
        
        ds = part.get('dataScouting', '')
        ds1 = 'ds1' if 'ds1' in ds  else ''
        ds2 = 'ds2' if 'ds2' in ds  else ''

        if ds1 and ds2:
            msg = '%s both "ds1" and "ds2" present in data scouting' \
            'string: %s' % (self.err_hdr, ds)
            raise RuntimeError(msg)

        self.check_and_set('data_scouting', ds1 + ds2)

        # --------  check scan type consistency ----------------
        scan_type = part['scan']
        scan_types = ('', 'PS', 'FS')
        if scan_type not in scan_types:
            msg = '%s unknown scanType %s allowed: %s' % (self.err_hdr,
                                                          scan_type,
                                                          str(scan_types))
            raise RuntimeError(msg)
        
        # -----------------------------------------

        if scan_type != 'FS' and self.data_type == 'TT':
            msg = '%s: scanType %s set for Trigger tower scan' % (
                self.err_hdr, scan_type)
            raise RuntimeError(msg)

        self.check_and_set('scan_type', scan_type)

        # check whether to run the hypo
        run_hypo =  'perf' not in part['addInfo'] and not self.data_scouting
        self.check_and_set('run_hypo', run_hypo)

        # --------  hypo parameters ----------------
        self.check_and_set('tla_string',  part['TLA'])

        def getTopo(topos, target):
            targets = [x for x in part['topo'] if x.startswith(target)]
            if len(targets) > 1:
                msg = '%s %s mass specied more than once %s' % (
                    self.err_hdr, target, str(topos))
                raise RuntimeError(msg)

            result = ''
            if targets: result = targets[0]
            return result

        # do not use check_and_set for invm_string or deta_Str
        # as these values cab be different in different chainParts
        self.invm_string =  getTopo(part['topo'], 'invm')
        self.deta_string = getTopo(part['topo'], 'deta')

        self.dimass_deta = bool(self.invm_string) or bool(self.deta_string)

        # 16/04/10 switch to using TrigHLTJetHypo2 for non-test4 chains
        # and TrigHLTJetHypo for test4 chains (reverses previous order)
        hypo_type = {('j', '', False, False): 'HLThypo2_etaet',
                     ('j', 'test1', False, False): 'HLThypo2_etaet',
                     ('j', 'test2', False, False): 'HLTSRhypo',
                     ('j', 'test4', False, False): 'HLThypo',
                     # set up jets normally, dijet hypo appended:
                     # requires corresponding change to generateJetChainDefs.py
                     ('j', 'test4', False, True): 'HLThypo', 
                     ('j', '', False, True): 'HLThypo2_dimass_deta',
                     ('ht','', False, False): 'ht',
                     ('ht', 'test4', False, False):'HLThypo2_ht',
                     ('j', 'test4', True, False): 'tla',
                     ('j', '', True, False): 'HLThypo2_tla',}.get(
                         (part['trigType'],
                          self.test_flag,
                          bool(self.tla_string),
                          self.dimass_deta), None)
       
        # hypo_type = {('j', '', False, False): 'HLThypo',
        #              ('j', 'test1', False, False): 'run1hypo',
        #              ('j', 'test2', False, False): 'HLTSRhypo',
        #              ('j', 'test4', False, False): 'HLThypo2_etaet',
        #              ('j', 'test4', False, True): 'HLThypo2_dimass_deta',
        #              ('j', '', False, True): 'HLThypo',
        #              ('ht','test4', False, False): 'HLThypo2_ht',
        #              ('j', 'test4', True, False): 'HLThypo2_tla',
        #              ('ht', '', False, False):'ht',
        #              ('j', '', True, False): 'tla'}.get(
        #                  (part['trigType'],
        #                   self.test_flag,
        #                   bool(self.tla_string),
        #                   self.dimass_deta), None)

        if self.multi_eta and not hypo_type.startswith('HLThypo'):
            hypo_type = 'HLThypo'

        # maximum  bipartite is now "standard"
        # if self.multi_eta: hypo_type = 'maximum_bipartite'
                     
        if not hypo_type:
            msg = '%s: cannot determine hypo type '\
                  'from trigger type: %s test flag: %s' \
                  'TLA: %s dimass_eta %s' %  (
                      self.err_hdr, part['trigType'],
                      self.test_flag,
                      str(bool(self.tla_string)),
                      str(self.dimass_deta))
            raise RuntimeError(msg)

        self.hypo_types.append(hypo_type)
        print self.hypo_types
        self.check_and_set('hypo_type', hypo_type)

        # convert the cleaner names obtained from the chain name to 
        # the one which will be used to key the C++ cleaner factory
        cleaner = part['cleaning']
        cleaner = self.cleaner_names.get(cleaner, None)
        if cleaner is None:
            m = 'Cannot convert chain name  cleaner name "%s" to C++ cleaner '\
                'name ' % part['cleaning']
            raise RuntimeError(m)

        self.check_and_set('cleaner', cleaner) 

        hypo_setup_fn = {
            'HLThypo': self._setup_etaet_vars,
            'HLThypo2_etaet': self._setup_etaet_vars,
            'HLThypo2_dimass_deta': self._setup_dimass_deta_vars,
            'HLThypo2_tla': self._setup_tla_vars,
            'HLThypo2_ht': self._setup_ht_vars,
            'run1hypo': self._setup_run1_vars,
            'HLTSRhypo': self._setup_etaet_vars,
            'tla': self._setup_tla_vars,
            'ht': self._setup_ht_vars}.get(hypo_type, None)

        if hypo_setup_fn is None:
            msg = '%s: unknown hypo type (JetDef bug) %s' % (
                self.err_hdr,
                str(self.hypo_type))
            raise RuntimeError(msg)

        hypo_setup_fn(part)

        self.n_parts += 1


    def check_and_set(self, attr, val):
        """Set the value of attribute attr to val if the first chain part
        is being processed. For subsequent chain parts, ensure their
        attribute values are the same (chain parts have much redundant
        information)."""

        if self.n_parts == 0:
            setattr(self, attr, val)
            return

        
            
        # handle special cases, where chainParts differ for understood
        # reasons.
        #
        # - hypo_type. if
        # old == HLThypo2_dimass_deta, new == HLThypo2_etaet, keep old
        # new == HLThypo2_dimass_deta, old == HLThypo2_etaet, use new

        my_val = None
        try:
            my_val = getattr(self, attr)
        except AttributeError:
            # second chain parts include new inforamtion
            # of the dijet hypo
            if self.hypo_type == 'HLThypo2_dimass_deta':
                if attr in  ('mass_min','dEta_min'):
                    setattr(self, attr, val)
                    my_val = getattr(self, attr)
                    
        if attr == 'hypo_type':
            if my_val == 'HLThypo2_etaet' and val == 'HLThypo2_dimass_deta':
                setattr(self, attr, val)
                my_val = getattr(self, attr)

            if val == 'HLThypo2_etaet' and my_val == 'HLThypo2_dimass_deta':
                val = my_val

        if my_val != val:
            msg = '%s attribute clash %s %s %s' % (self.err_hdr,
                                                   attr,
                                                   my_val,
                                                   val)
            raise RuntimeError(msg)

    def __call__(self):

        # obtain objects which state which the
        # algorithm to use, and the corresponding algorithm dependent
        # parameters

        cluster_args = {'label': self.cluster_label}
        
        cluster_params = clusterparams_factory(cluster_args)

        fex_args = {'merge_param': self.merge_param,
                    'jet_calib': self.jet_calib,
                    'cluster_calib': self.cluster_calib,
                    'fex_label': self.fex_label,
                    'data_type': self.data_type,
                    'fex_alg_name': self.fex_alg_name,  # for labelling
                    }

        fex_params = fexparams_factory(self.fex_name, fex_args)

        # various fexes maybe present in the same chain (a4, a10r eg)
        # the last set of fex parameters is used to name the hypo instance
        last_fex_params = fex_params  

        recluster_params = None
        if self.do_recluster:
            recl_args = dict(fex_args)
            recl_args.update({'ptMinCut': self.recl_ptMinCut,
                              'etaMaxCut': self.recl_etaMaxCut,
                              'fex_label': self.recl_fex_label,
                              'merge_param': self.recl_merge_param,
                              'jet_calib': self.recl_jet_calib,
                              'fex_alg_name': self.recl_fex_alg_name,
                          })

            recluster_params = fexparams_factory(
                'jetrec_recluster', recl_args)

            # overwrite last_fex_params
            # this is used to name the hypo instance
            last_fex_params = recluster_params  

        trim_params = None
        if self.do_trim:
            trim_args = dict(fex_args)
            trim_args.update({'rclus': self.trim_rclus,
                              'ptfrac': self.trim_ptfrac,
                              'fex_label': self.trim_fex_label,
                              })
            trim_params = fexparams_factory('jetrec_trimming',trim_args)
	    
  	    # overwrite last_fex_params
	    # this is used to name the hypo instance
            last_fex_params = trim_params
          
            
        if self.hypo_type in ('HLThypo', 'HLThypo2_etaet'):
            hypo_args = {
                'chain_name': self.chain_name,
                'jet_attributes': self.jet_attributes,
                'cleaner': self.cleaner,
                'matcher': 'maximumBipartite', 
                'isCaloFullScan': self.scan_type == 'FS',
                'triggertower': self.data_type == 'TT',
            }


        elif self.hypo_type == 'HLTSRhypo':
            hypo_args = {
                'chain_name': self.chain_name,
                'jet_attributes': self.jet_attributes,
                'cleaner': self.cleaner,
                'matcher': 'orderedCollections', 
                'isCaloFullScan': self.scan_type == 'FS',
                'triggertower': self.data_type == 'TT',
            }

        elif self.hypo_type == 'run1hypo':
            hypo_args = {
                'chain_name': self.chain_name,
                'jet_attributes': self.jet_attributes,
                'isCaloFullScan': self.scan_type == 'FS',
                'triggertower': self.data_type == 'TT',
                'cleaner': 'noClean',
            }

        elif self.hypo_type in ('ht', 'HLThypo2_ht'):
            hypo_args = {
                'chain_name': self.chain_name,
                'eta_range': self.eta_range,
                'ht_threshold': self.ht_threshold,
                'jet_et_threshold': self.jet_et_threshold,
                'matcher': 'maximumBipartite', 
                }
            if self.hypo_type == 'ht': hypo_args['matcher'] = None

        elif self.hypo_type in ('tla', 'HLThypo2_tla'):
            hypo_args = {
                'chain_name': self.chain_name,
                'indexlo': int(self.indexlo),
                'indexhi': int(self.indexhi),
                'mass_min': float(self.mass_min),
                'mass_max': float(self.mass_max),
                'matcher': 'maximumBipartite', 
                'tla_string': self.tla_string}

        elif self.hypo_type in ('HLThypo2_dimass_deta',):

            try:
                mass_min = float(self.mass_min)
            except TypeError:
                mass_min = None
                
            try:
                dEta_min = float(self.dEta_min)/10.
            except TypeError:
                dEta_min = None

            hypo_args = {
            'chain_name': self.chain_name,
                'jet_attributes': self.jet_attributes,
                'cleaner': self.cleaner,
                'matcher': 'maximumBipartite', 
                'dEta_min': dEta_min,
                'mass_min': mass_min}
            
        else:
            msg = '%s unknown hypo_type %s' % (self.err_hdr, self.hypo_type)
            raise RuntimeError(msg)

        hypo_params = hypo_factory(self.hypo_type, hypo_args)

        menu_data = MenuData(self.scan_type,
                             self.data_type,
                             fex_params=fex_params,
                             hypo_params=hypo_params,
                             cluster_params=cluster_params,
                             recluster_params=recluster_params,
			     trim_params=trim_params,
                             last_fex_params= last_fex_params
                         )


        return ChainConfig(chain_name=self.chain_name,
                           seed=self.seed,
                           run_hypo=self.run_hypo,
                           run_rtt_diags=self.run_rtt_diags,
                           data_scouting=self.data_scouting,
                           menu_data=menu_data,)

    def _check_part(self, part):
        """Check chain part for errors"""

        # Attempting to do partial scan with pileup subtraction
        # produces nonsense results
        if part['scan'] == 'PS' and 'sub' in part['jetCalib']:
            msg = '%s partial scan is incompatible with pileup subtraction' %(
                self.err_hdr, )
            raise RuntimeError(msg)


            hypo_setup_fn = {'standard': self.setup_standard_hypo,
                             'ht': self.setup_ht_hypo}.get(hypo_type, None)

    def _setup_jet_vars(self, part):
        """make hypo parameteters an attribute of this instance so 
        they can be checked with check_and_set. This method handles
        common standard jet hypo paramters"""

        mult = int(part['multiplicity'])
        threshold = int(part['threshold'])
        eta_range = part['etaRange']

        self.jet_attributes.extend(
            [(JetAttributes(threshold, eta_range)) for i in range(mult)])

    def _setup_run1_vars(self, part):
        """Ensure the Run1 hypo is not being asked to do things it cannot do"""

        if self.cleaner != 'noCleaning':
            m ='Jet cleaning requested %s, '\
                'but is incomaptable with the standard hypo' % self.cleaner
            raise RuntimeError(m)
        
        if self.multi_eta:
            m ='Hypo with > 1 eta refions requested , '\
                'but is incomaptable with the standard hypo'
            raise RuntimeError(m)

        self._setup_jet_vars(part)

    def _setup_etaet_vars(self, part):
        """Delegate setting up the hypo parameters"""
        self._setup_jet_vars(part)

    def _setup_dimass_deta_vars(self, part):
        self._setup_jet_vars(part)

        m = self.invm_re.search(self.invm_string)
        if m:
            # set dimass min limit obtained from regex matching
            for k, v in m.groupdict().items():
                self.check_and_set(k, v)
        else:
            self.check_and_set('mass_min', None)

        m = self.deta_re.search(self.deta_string)
        if m:
            # set ystar min limit obtained from regex matching
            for k, v in m.groupdict().items():
                self.check_and_set(k, v)
        else:
            self.check_and_set('dEta_min', None)
        
    def _setup_ht_vars(self, part):
        """make the HT hypo paramters attributes of this instance to 
        allow consistency checking with check_and_set"""

        eta_range = part['etaRange']
        self.check_and_set('eta_range', eta_range)
        ht_threshold = int(part['threshold'])
        self.check_and_set('ht_threshold',  ht_threshold)

        # set the default cuts on the jets contributing to the Et
        # sum to be 30 if not specified. Otherwise the expected form is
        #j\d+

        jet_et_threshold = 30.
        extra = part['extra']
        if (not (extra == '' or extra.startswith('test'))):
            try:
                jet_et_threshold = float(part['extra'][1:])
            except:
                m = '%s unrecognized value for HT jet cut %s' % (
                    self.err_hdr,
                    str(part['extra'][1:]))
            
                raise RuntimeError(m)
            
        self.check_and_set('jet_et_threshold', jet_et_threshold)
            
    def _setup_tla_vars(self, part):

        m = self.tla_re.search(self.tla_string)
        if m == None:
            m = 'ChainConfigMaker:_setup_tla_vars unmatched ' \
                'tla string: %s regex: %s'  % (tla_string, 
                                               self.tla_re.pattern)
            raise RuntimeError(m)

        # set indices and mass limits obtained from regex matching
        for k, v in m.groupdict().items():
            self.check_and_set(k, v)
 
