# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function
from __future__ import absolute_import

from .constants import (lchars, seps, digits)

import re
from collections import defaultdict

win_alphabet = lchars + seps
window_re = re.compile(
    r'^(?P<lo>\d*)(?P<attr>[%s]+)(?P<hi>\d*)' % win_alphabet)

def defaultParameters(parameter, default=''):  # default if parameter unknown
    explicit_defaults = {
        'etahi': '320',
        'j1etahi': '320',
        'j2etahi': '320',
        'eta_maxs': '3.2',
        'EtThreshold': '0.',
        'eta_mins': '0.',
        'asymmetricEtas': 0,  # exception: not a string
        'momCutslo': '-inf',
    }

    parameter = parameter.split(':')[-1]  # fltr:eta....
    if parameter.startswith('mom'):
        parameter = 'momCuts'


    if parameter in explicit_defaults: return explicit_defaults[parameter]
    
    if not default:
        if parameter.endswith('lo'):
            default = '0'
        elif parameter.endswith('hi'):
            default = 'inf'
        else:
            raise RuntimeError(
                'cannot find default for parameter ' + str(parameter))

    return default


def scaleFactors(parameter):

    defaults = {  # only positive values here. sign done elsewhere
        'eta': 0.01,
        'neta': 0.01,
        'ceta': 0.01,
        'peta': 0.01,
        'nphi': 0.01,
        'cphi': 0.01,
        'pphi': 0.01,
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

    parameter = parameter.split(':')[-1]  # fltr:eta....

    if parameter.startswith('mom'):
        parameter = 'momCuts'

    return defaults[parameter]
        
class TreeChecker(object):
    def __init__(self):
        self.node_count = 0
        self.root_count = 0
        self.bad_condition_count = 0
        self.msgs = []
        
    def mod(self, node):
        self.node_count += 1
        if node.scenario == 'root':
            self.root_count += 1
            if node.conf_attrs:
                self.bad_condition_count += 1

        else:
            if len(node.conf_attrs) != 1:
                self.bad_condition_count += 1

    def error(self):
        good  = self.root_count == 1 and self.bad_condition_count == 0
        return not good
    
    
    def report(self):
        cn = self.__class__.__name__
        s = ['%s: %d nodes checked' % (cn, self.node_count),
             '%s: %d root node(s)' % (cn, self.root_count),
             '%s: error - ' % cn + str(self.error())]
        
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
        
        alphabet = lchars + seps + digits
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
        
        result = []
        chainpartinds = []
        msgs = []


        # process each parameter string once.
        for c in conditions: # c is condition string
            cdict = defaultdict(dict)
            
            toks = c.split(',')  # parameters in par string are separated by ','
            toks = [t.strip() for t in toks]

            # cpis: shain part ind
            cpis = [t for t in toks if t.startswith('leg')]
            assert len(cpis) < 2
            if cpis:
                chainpartinds.append(cpis[0])
                toks.remove(chainpartinds[-1])

            for t in toks:
                m = window_re.match(t)
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

                    # note on the value inf in python
                    # the value of float('inf') is inf.
                    # the value of float('-inf') is -inf.
                    # the value of 10 * inf is inf.
                    #
                    # this is case independent:
                    #
                    # the value of float('iNf') is inf.

                    fl = float(limit)

                    if fl != 0:  # avoid '-0'
                        fl = fl * sf

                    return str(fl) 

                if lo == '':
                    lo = defaultParameters(attr+'lo')
                if hi == '':

                    hi = defaultParameters(attr+'hi')
                    
                sf = scaleFactors(attr)
                
                # scale fctors and negative limts
                # Scaling taking the string version of the limit found in
                # the jet labelm and
                #
                # converts it to a float, and scales it. eg for eta,
                # '320' -> '3.2'
                # 
                # Some limits are negative. eg the values assocated with
                # neta are bot negative. Eg the  '320neta100' represent
                # cut values of -3.2 and -1.0. 
                #
                # Currently (1/2021), dor attributes neta and nphi the 
                # both the low and high limits are negative, while for
                # ceta, cphi (c = crossing zero) the lower limits are
                # negative.

                neg_low = ('neta', 'nphi', 'ceta', 'cphi')
                neg_high = ('neta', 'nphi')
                
                limits_dict = {}
                if lo:
                    # find the python proxy class  name
                    ssf = -sf if  attr in neg_low else sf
                    limits_dict['min'] = scale_limit(lo, ssf)
                    
                if hi:
                    ssf = -sf if  attr in neg_high else sf
                    limits_dict['max'] = scale_limit(hi, ssf)
                        
                cdict[attr] = limits_dict

            result.append(cdict) # append dictionary


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


class TreeParameterExpander_dijet(object):
    """Convert parameter string into tuples holding low, high window
    cut vals. Specialistaion for the dijet scenario

    parameter strings look like '40m,100deta200, 50dphi300'

    outputs values are strings which are passed to the C++ components
    which will convert numeric values, and symbolic values such as 'inf'
    """
    
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

        node.conf_attrs = []

        self.msgs = ['All OK']

        
    def report(self):
        return '%s: ' % self.__class__.__name__ + '\n'.join(self.msgs)


class FilterConditionsMover:
    def mod(self, node):
        """Move dictionary used to construct the filter for a Condition from
        node.conf_attrs to node.filter_dicts"""


        filter_dicts = []
        for ca in node.conf_attrs:
            ca_keys = ca.keys()
            n_filtkey = 0

            for ca_key in ca_keys:
                if 'fltr' in ca_key: n_filtkey += 1
            assert n_filtkey == 0 or n_filtkey == len(ca_keys)

            if n_filtkey > 0:
                filter_dicts.append(ca)

        [node.conf_attrs.remove(ca) for ca in filter_dicts]
        node.filter_dicts = filter_dicts


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
        'agg': TreeParameterExpander_simple,
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
        

class IdenticalNodeCompressor(object):
    """Visitor Class to group nodes represnting the same conditions into
    a single node """

    def __init__(self):
        self.messages = []

    def mod(self, node):
        """identify idental nodes/ If > 1, set the multiplicity of the first 
        to the number of such nodes, then remove the remainder"""
        
        param_nodes = defaultdict(list)
        [param_nodes[c.parameters].append(c) for c in node.children]

        for cns in param_nodes.values():
            if len(cns) > 1:
                self. messages.append('Coelescing %d nodes' % len(cns))
                cns[0].multiplicity = len(cns)
                [node.children.remove(c) for c in cns[1:]]


    def report(self):
        
        if self.messages:
            return '%s: %s' % (self.__class__.__name__,
                               '\n'.join(self.messages))
        else:
            return ''
