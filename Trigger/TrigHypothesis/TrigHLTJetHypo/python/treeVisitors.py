# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function
from __future__ import absolute_import

from .constants import lchars

import re
from collections import defaultdict

def defaultParameters(parameter, default=''):  # default if parameter unknown
    defaults = {'etalo': '0',
                'etahi': '320',
                'petalo': '0',  # +ve eta
                'petahi': '320',
                'netalo': '320',  # -ve eta
                'netahi': '0',
                'etlo':   '0',
                'ethi':   'inf',
                'EtThreshold': '0.',
                'eta_mins': '0.',
                'eta_maxs': '3.2',
                'asymmetricEtas': 0,  # exception: not a string
                'djmasslo': '0.0',
                'djmasshi': 'inf',
                'djdetalo': '0.',
                'djdetahi': 'inf',
                'djdphilo': '0.',
                'djdphihi': 'inf',
                'qjmasslo': '0.0',
                'qjmasshi': 'inf',
                'momCutslo': '-inf',
                'momCutshi': 'inf',
                'smclo': '0',
                'smchi': 'inf',                
                'jvtlo': '0',
                'jvthi': 'inf',
                'htlo' : '1000.',
                'hthi' : 'inf',
    }

    if parameter.startswith('mom'):
        parameter = 'momCuts'

    if parameter not in  defaults:
        print ('defaultParameters: unknown parameter, returning default ',
               parameter)

    return defaults.get(parameter, default)



def scaleFactors(parameter):
    defaults = {
        'eta': 0.01,
        'neta': -0.01,
        'peta': 0.01,
        'et': 1000.,
        'ht': 1000.,
        'smc': 1000.,
        'djdeta': 0.1,
        'djmass': 1000.,
        'djdphi': 0.1,
        'qjmass': 1000.,
        'momCuts': 0.01,
        'jvt': 0.01,
    }
    if parameter.startswith('mom'):
        parameter = 'momCuts'
    return defaults[parameter]
        
class Checker(object):
    def __init__(self):
        self.known = {
            'simple': ('Et'),
            'cascade': ('m')
        }

        self.msgs = []
        self.nchecked = 0

    def mod(self, node):
        self.nchecked += 1
        if node.scenario not in self.known:
            self.msgs.append('unknown scenario %s' % node.scenario)
    
    def report(self):
        s = ['Checker: %d nodes checked' % self.nchecked]
        s.extend(self.msgs)
        return '\n'.join(s)


class TreeBuilder(object):
    def __init__(self):
        self.tree = '('
        
    def add(self, s):
        self.tree += s

    def report(self):
        return self.tree


class ConditionsDictMaker(object):

    """Convert parameter string into dictionary holding low, high window
    cut vals. 

    Example:  makeDict('(10et,0eta320)') 
              returns the tuple:  ([dict0, dict1,...] , error, msgs]:
              (
                [
                 {'eta_maxs': [3.2], 
                 'EtThresholds': [10000.0], 
                 'eta_mins': [0.0],
                 'asymmetricEtas': [0]}
               ],
               False, 
               ['OK']
              )

    dijets:     parameter strings looks like '40mass, 0dphi20'

    """

    window_re = re.compile(
        r'^(?P<lo>\d*)(?P<attr>[%s]+)(?P<hi>\d*)' % lchars)


    # key: substring from chain label. value: attribute of python
    # component proxy
    
    def get_conditions(self, params):
        """Split conditions string into list of condition strings

        simple example:
          get_conditions('(10et,0eta320)(20et,0eta320)(40et,0eta320)')
          ['10et,0eta320', '20et,0eta320', '40et,0eta320']

        dijet example: 
           get_conditions('(900mass,26dphi)')
           ['900mass,26dphi']
        """
        
        alphabet = 'abcdefghijklmnopqrstuvwxyz0123456789,'
        pat = re.compile(r'(^\([%s]+\))'% alphabet )
        s = params
        m = True
        conditions = []
        while m:
            m = pat.match(s)
            if m is not None:
                conditions.append(m.group(0))
                s = s[len(conditions[-1]):]
        assert params == ''.join(conditions)
        conditions = [c[1:-1] for c in conditions]  # strip parens
        return conditions


    def makeDict(self, params):

        # conditions example: ['10et,0eta320', '20et']
        conditions = self.get_conditions(params)

        # keep track of identical conditions (mult > 1)
        
        mult_conditions = defaultdict(int)
        for c in conditions: mult_conditions[c] += 1
        
        result = []
        chainpartinds = []
        msgs = []


        # process each parameter string once.
        for c, mult in mult_conditions.items(): # c is condition string
            cdict = defaultdict(dict)
            
            toks = c.split(',')  # parameters in par string are separated by ','
            toks = [t.strip() for t in toks]
            cpis = [t for t in toks if t.startswith('leg')]
            assert len(cpis) < 2
            if cpis:
                chainpartinds.append((cpis[0], mult))
                toks.remove(chainpartinds[-1][0])

            for t in toks:
                m = self.window_re.match(t)
                limits_dict = {}
                if m is None:
                    msgs.append('match failed for parameter %s' % t)
                    error = True
                    return {}, error, msgs
                group_dict = m.groupdict()

                # example: t = '10et,0eta320', group_dict =
                # {'lo': '10', 'hi': '', 'attr': 'et'}

                # There are two naming schemes
                # 1. comes from chain label. Attributes and values are strings.
                #    eg 0eta: The default value for etahi will be looked up and
                #       found to be the strinbg '320'. This will be converted
                #       to a float, and multiplied by the scale factor for
                #       'eta', namely 0.01. The values will be added
                #       to  a list. The name of the variable set is
                #        adjusted to that used to intiialize the C++ class.
                #
                # 2. attributes not present in the chain label, but which
                #    must be set. The  attribute name is used directly,
                #    the defaults do not require multiplying by a scale factor.
                #   
                
                attr = group_dict['attr']  # attribute name in label
                lo = group_dict['lo']  # string: low value or ''
                hi = group_dict['hi']  # string high value or ''

                def scale_limit(limit, sf):

                    try:
                        fl = float(limit)
                    except TypeError: # limit = 'inf' or similar
                        return limit

                    if fl != 0:  # avoid '-0'
                        fl = fl * sf

                    return str(fl) 

                if lo == '':
                    lo = defaultParameters(attr+'lo')
                if hi == '':

                    hi = defaultParameters(attr+'hi')
                    
                sf = scaleFactors(attr)
                
                if lo:
                    # find the python proxy class  name
                    limits_dict['min'] = scale_limit(lo, sf)
                        
                if hi:
                    limits_dict['max'] = scale_limit(hi, sf)
                        
                cdict[attr] = limits_dict
            
            result.append((cdict, mult)) # append dictionary and mult.


        # Example: input condition string:
        #
        # 260et,320eta490
        # 
        # result:
        #
        # [
        #  defaultdict(<class 'dict'>, {'et': {'min': '260000.0', 'max': 'inf'},
        #                               'eta': {'min': '3.2', 'max': '4.9'}})
        # ]

        msgs = ['ConditionsDict OK']
        error = False
        return result, chainpartinds, error, msgs


class TreeParameterExpander_simple(object):
    """Convert parameter string into duction holding low, high window
    cut vals. Specialistaion for the 'simple' scenario

    parameter strings look like '40et, 0eta320, nosmc'
    """
    
    window_re = re.compile(
        r'^(?P<lo>\d*)(?P<attr>[%s]+)(?P<hi>\d*)' % lchars)

    def __init__(self):
        self.msgs = []

    def mod(self, node):

        cdm = ConditionsDictMaker()
        d, chainpartinds, error, msgs = cdm.makeDict(node.parameters)
        self.msgs.extend(msgs)
        node.conf_attrs = d
        node.chainpartinds = chainpartinds
        
    def report(self):
        return '%s: ' % self.__class__.__name__ + '\n'.join(self.msgs) 


class TreeParameterExpander_agg(object):
    """Convert parameter string into duction holding low, high window
        cut vals, as for the  'simple' scenario. Then place conditions
        not in the agg list in the filters dict. These conditions wil be used
        to select the subset of the jet collection to be presented to the agg
        conditions."""

    agg_conditions = ('ht',)
    
    def __init__(self):
        self.msgs = []

    def mod(self, node):

        simple_expander = TreeParameterExpander_simple()
        simple_expander.mod(node)

        # example conf_attrs:
        # conf_attrs [3]:
        # (defaultdict(<class 'dict'>,
        #              {'ht': {'min': '1000000.0', 'max': 'inf'}}), 1)
        # (defaultdict(<class 'dict'>,
        #              {'et': {'min': '30000.0', 'max': 'inf'}}), 1)
        # (defaultdict(<class 'dict'>,
        #             {'eta': {'min': '0.0', 'max': '3.2'}}), 1)


        for ca in node.conf_attrs:
            assert len(ca) == 2  # (dict, mult)
            assert len(ca[0]) == 1  # one entry in dict
            ca_keys = ca[0].keys()
            cond_name = list(ca_keys)[0]
            if cond_name not in self.agg_conditions:
                node.filter_conditions.append(ca)
        for fc in node.filter_conditions:
            node.conf_attrs.remove(fc)

    def report(self):
        return '%s: ' % self.__class__.__name__ + '\n'.join(self.msgs) 


class TreeParameterExpander_dijet(object):
    """Convert parameter string into tuples holding low, high window
    cut vals. Specialistaion for the dijet scenario

    parameter strings look like '40m,100deta200, 50dphi300'

    outputs values are strings which are passed to the C++ components
    which will convert numeric values, and symbolic values such as 'inf'
    """
    
    window_re = re.compile(
        r'^(?P<lo>\d*)(?P<attr>[%s]+)(?P<hi>\d*)' % lchars)


    def __init__(self):
        self.msgs = []

    def mod(self, node):

        cdm = ConditionsDictMaker()
        d, chainpartinds, error, msgs = cdm.makeDict(node.parameters)
        self.msgs.extend(msgs)
        node.conf_attrs = d
        node.chainpartinds = chainpartinds

    def report(self):
        return '%s: ' % self.__class__.__name__ + '\n'.join(self.msgs)


class  TreeParameterExpander_all(object):
    """Convert parameter string into a dictionary holding low, high window
    cut vals. Specialistaion for the "all" node

    parameter strings look like '40m,100deta200, 50dphi300'
    """
    
    def __init__(self):
        self.msgs = []

    def mod(self, node):

        if node.parameters != '' :
            self.msgs.append(
                'Error, all node with parameters ' + node.parameters)
            return

        node.conf_attrs = ''

        self.msgs = ['All OK']

        
    def report(self):
        return '%s: ' % self.__class__.__name__ + '\n'.join(self.msgs)


class TreeParameterExpander_null(object):
    """Does nothing except check the parameter string is empty"""

    def __init__(self):
        self.msgs = ['Do nothing paramter expander']

    def mod(self, node):
        assert node.parameters == ''
        
    def report(self):
        return '%s: ' % self.__class__.__name__ + '\n'.join(self.msgs) 
    

class TreeParameterExpander(object):
    """Class to expand node.parameters string. Delegates to
    specialised expanders."""
    
    router = {
        'z': TreeParameterExpander_null,
        'root': TreeParameterExpander_null,
        'simple': TreeParameterExpander_simple,
        'agg': TreeParameterExpander_agg,
        'dijet': TreeParameterExpander_dijet,
        'qjet': TreeParameterExpander_simple,
        'all': TreeParameterExpander_all,
        'agree': TreeParameterExpander_null,
    }

    def __init__(self):
        self.expander = None

    def mod(self, node):

        self.expander = self.router[node.scenario]()
        self.expander.mod(node)

    def report(self):
        return self.expander.report()
        

