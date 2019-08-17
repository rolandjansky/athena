# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from constants import lchars

import re

from ToolSetter import ToolSetter

def defaultParameters(parameter, default=''):  # default if parameter unknown
    defaults = {'etalo': '0',
                'etahi': '320',
                'petalo': '0',  # +ve eta
                'petahi': '320',
                'netalo': '-320',  # -ve eta
                'netahi': '0',
                'etlo':   '0',
                'ethi':   'inf',
                'EtThreshold': '0.',
                'eta_mins': '0.',
                'eta_maxs': '3.2',
                'asymmetricEtas': '0',
                'mass_mins': '0.0',
                'mass_maxs': 'inf',
                'deta_mins': '0.',
                'deta_maxs': 'inf',
                'dphi_mins': '0.',
                'dphi_maxs': 'inf',
                'masslo': '0.',
                'masshi': 'inf',
    }

    if parameter not in  defaults:
        print 'defaultParameters: unknown parameter ', parameter
    return defaults.get(parameter, default)

def scaleFactors(parameter):
    defaults = {
        'eta': 0.01,
        'neta': 0.01,
        'peta': 0.01,
        'et': 1000.,
        'smc': 1000.,
        'deta': 0.1,
        'mass': 1000.,
        'dphi': 0.1,
    }
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


class TreeToBooleanExpression(object):
    """visit a hypo tree. If boolean scenarios are present, build a
    boolean expression string."""
    
    def __init__(self):
        self.stack = []

    def mod(self, node):
        if node.scenario == 'not':
            self.stack.append(' ! ')
            return

        if node.scenario == 'and':
            self.stack.append(' x ')
            return

        if node.scenario == 'or':
            self.stack.append(' + ')
            return

        self.stack.append(' %s ' %node.tool.name())

    def report(self):
        s = '%s: ' % self.__class__.__name__
        while self.stack: s += self.stack.pop()
        return s.strip()


class ConditionsDictMakerBase(object):

    window_re = re.compile(
        r'^(?P<lo>\d*)(?P<attr>[%s]+)(?P<hi>\d*)' % lchars)

    labelToAttribute_low = {
        'eta': 'eta_mins',
        'peta': 'eta_mins',
        'neta': 'eta_mins',
        'et': 'EtThresholds',
        'mass': 'mass_mins',
        'dphi': 'dphi_mins',
        }

    
    labelToAttribute_high = {
        'eta': 'eta_maxs',
        'peta': 'eta_maxs',
        'neta': 'eta_maxs',
        'mass': 'mass_maxs',
        'dphi': 'dhi_maxs',
        }
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

        conditions = self.get_conditions(params)


        attributes = self.get_attributes()  # derived classes provide.

        result = {}
        msgs = []
        for a in attributes: result[a] = []

        for c in conditions:  # there is a parameter string for each condition
            toks = c.split(',')  # parameters seperated by ','
            toks = [t.strip() for t in toks]

            # copy attributes...
            # copy used to check condition parameter not set > 1 times
            attributes2 = attributes[:]  
            for t in toks:
                m = self.window_re.match(t)
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
                
                attr_label = group_dict['attr']  # attribute name in label
                lo = group_dict['lo']  # string: low value or ''
                hi = group_dict['hi']  # string high value or ''

                if lo == '':
                    lo = defaultParameters(attr_label+'lo')
                if hi == '':
                    hi = defaultParameters(attr_label+'hi')
                    
                sf = scaleFactors(attr_label)
                if attr_label in ('eta', 'peta', 'neta'):
                    asym = 0 if attr == 'eta' else 1
                    result['asymmetricEtas'].append(asym)
                    attributes2.remove('asymmetricEtas')

                if lo:
                    # find the python proxy class  name
                    attr = self.labelToAttribute_low.get(attr_label, None)
                    print attr, attr_label
                    result[attr].append(str(sf * float(lo)))
                        
                    if attr_label == 'neta':
                        result[attr][-1] *= -1.  # negative eta range
                        
                    try:
                        attributes2.remove(attr)
                    except ValueError, e:
                        print attr, 'appears twice in Conditions string?'
                        raise e
                    
                if hi:
                    attr = self.labelToAttribute_high.get(attr_label, None)
                    if attr is None: continue  # eg et dows not have upper bound
                    result[attr].append(str(sf * float(hi)))
                    if attr == 'neta':
                        result[attr][-1] *= -1.  # negative eta range
                        
                    try:
                        attributes2.remove(attr)
                    except ValueError, e:
                        print attr, 'appears twice in Conditions string?'
                        raise e

            # it maybe that an attribute was not present in the chain label.
            # in this case, default values should be used.
            for attr in attributes2: # whatever has not been removed...
                result[attr].append(defaultParameters(attr))

        msgs = ['SimpleConditionsDict OK']
        error = False
        return result, error, msgs

class SimpleConditionsDictMaker(ConditionsDictMakerBase):
    """Convert parameter string into dictionary holding low, high window
    cut vals. Specialistion for the 'simple' scenario

    Example:  scdm.makeDict('(10et,0eta320)') 
              returns the tuple dict, error, msgs :
              (
              {'eta_maxs': [3.2], 'EtThresholds': [10000.0], 'eta_mins': [0.0],               'asymmetricEtas': [0]}, 
    
              False, 
              ['SimpleConditionsDict OK']
              )
    """

    def get_attributes(self):
        return ['EtThresholds',
                'eta_mins',
                'eta_maxs',
                'asymmetricEtas',]

class DijetConditionsDictMaker(ConditionsDictMakerBase):
    """Convert parameter string into dictionary holding low, high window
    cut vals. Specialistion for the 'dijet' scenario

    parameter strings looks like '40mass, 0dphi20'
    """

    def get_attributes(self):
        return [
            'mass_mins', 'deta_mins', 'dphi_mins',
            'mass_maxs', 'deta_maxs', 'dphi_maxs',
        ]

    # def makeDict(self, parameters):

    #      conditions = self.get_conditions(parameters)

    #      attributes = [
    #         'mass_mins', 'deta_mins', 'dphi_mins',
    #         'mass_maxs', 'deta_maxs', 'dphi_maxs',
    #     ]

    #      result = {}
    #     msgs = []
    #     error = False
    #     
    #     for a in attributes: result[a] = []

    #      for c in conditions:
    #         toks = c.split(',')
    #         toks = [t.strip() for t in toks]


    #           processed_attrs = []
    #         for t in toks:
    #             m = self.window_re.match(t)
    #             if m is None:
    #                 self.msgs.append('match failed for parameter %s' % t)
    #                 error = True
    #                 return {}, error, msgs
    #             
    #             group_dict = m.groupdict()
    #             attr = group_dict['attr']
    #             lo = group_dict['lo']
    #             hi = group_dict['hi']
    #             if lo == '':
    #                 lo = defaultParameters(attr+'_min')
    #             if hi == '':
    #                 hi = defaultParameters(attr+'_max')

    #              sf = scaleFactors(attr)
    #             if lo:
    #                 if attr == 'mass':
    #                     result['mass_mins'].append(str(sf*float(lo)))
    #                 elif attr == 'deta':
    #                     result['deta_mins'].append(str(sf*float(lo)))
    #                 elif attr == 'dphi':
    #                     result['dphi_mins'].append(str(sf*float(lo)))
    #                     
    #                 processed_attrs.append(attr+'_mins')
    #             if hi:
    #                 if attr == 'mass':
    #                     result['mass_maxs'].append(str(sf*float(lo)))
    #                 elif attr == 'deta':
    #                     result['deta_maxs'].append(str(sf*float(lo)))
    #                 elif attr == 'dphi':
    #                     result['dphi_maxs'].append(str(sf*float(lo)))
    #                 
    #                 processed_attrs.append(attr+'_maxs')



    #            for a in attributes:
    #             if a not in processed_attrs:
    #                 result[a].append(defaultParameters(a))

    #          for a in processed_attrs:
    #             if a not in attributes:
    #                 self.msgs.append('Unknown attribute: %s' % a)
    #                 error = True
    
    #      if error:
    #         self.msgs.append('Error')
    #     else:
    #         self.msgs = ['DijetConditionsDict OK']
    
    #      return result, error, msgs


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

        cdm = SimpleConditionsDictMaker()
        d, error, msgs = cdm.makeDict(node.parameters)
        self.msgs.extend(msgs)
        node.conf_attrs.update(d)

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

        cdm = DijetConditionsDictMaker()
        d, error, msgs = cdm.makeDict(node.parameters)
        self.msgs.extend(msgs)
        node.conf_attrs.update(d)

    def report(self):
        return '%s: ' % self.__class__.__name__ + '\n'.join(self.msgs) 


class TreeParameterExpander_combgen(object):
    """Convert parameter string into a dictionary holding low, high window
    cut vals. Specialistaion for the combgen Tool

    parameter strings look like '40m,100deta200, 50dphi300'
    """
    
    def __init__(self):
        self.msgs = []

    def mod(self, node):

        ok = True # status flag
        # the group size must be the first attribute, then the conditions.
        # size_re = re.compile(r'^\((\d+)\)')
        parameters = node.parameters[:]
        # m = size_re.match(parameters)
        # if m is None:
        #     self.msgs.append('Error')
        #     return

        # node.conf_attrs = {'groupSize':int(m.groups()[0])}
        # remove goup info + 2 parentheses
        # parameters = parameters[len(m.groups()[0])+2:]

        cdm = SimpleConditionsDictMaker()
        d, error, msgs = cdm.makeDict(parameters)
        self.msgs.extend(msgs)
        node.conf_attrs.update(d)
        

        if ok:
            self.msgs = ['All OK']
        else:
            self.msgs.append('Error')

        
    def report(self):
        return '%s: ' % self.__class__.__name__ + '\n'.join(self.msgs) 


class TreeParameterExpander_partgen(object):
    """Convert parameter string into a dictionary holding low, high window
    cut vals. Specialistaion for the combgen Tool

    parameter strings look like '40m,100deta200, 50dphi300'
    """
    
    def __init__(self):
        self.msgs = []

    def mod(self, node):

        parameters = node.parameters[:]
 
        cdm = SimpleConditionsDictMaker()

        d, error, msgs = cdm.makeDict(parameters)

        self.msgs.extend(msgs)
        node.conf_attrs = d
        

        if not error:
            self.msgs = ['All OK']
        else:
            self.msgs.append('Error')

        return d, error, msgs
    
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
        'simple': TreeParameterExpander_simple,
        'simplepartition': TreeParameterExpander_simple,
        'dijet': TreeParameterExpander_dijet,
        'not': TreeParameterExpander_null,
        'and': TreeParameterExpander_null,
        'or': TreeParameterExpander_null,
        'combgen': TreeParameterExpander_combgen,
        'partgen': TreeParameterExpander_partgen,
        'agree': TreeParameterExpander_null,
    }

    def __init__(self):
        self.expander = None

    def mod(self, node):
        self.expander = self.router[node.scenario]()
        self.expander.mod(node)
        print self.expander.report()
    def report(self):
        return self.expander.report()
        

def _test(s):

    from ChainLabelParser import ChainLabelParser
    parser = ChainLabelParser(s)

    parser.debug = True

    tree = parser.parse()
    print tree.dump()
    # exapnd the window cuts (strings) obtained from the chain label
    # to attributes and floating point numbers, set defaults
    # for unspecified vallues
    visitor = TreeParameterExpander()
    tree.accept(visitor)

    tree.set_ids(0, 0)
    tree.accept(visitor)
    print visitor.report()
    print tree.dump()

    # set the node attribute node.tool to be the hypo  Al\gTool.
    print 'sending in the ToolSetter visitor'
    ts_visitor = ToolSetter(s)
    tree.accept_cf(ts_visitor)
    print ts_visitor.report()


    # print tree.dump()
    print tree.tool  # printing a Gaudi tool prints its nested tools


def test(index):
    from test_cases import test_strings
    import sys
    if index not in range(len(test_strings)):
        print 'expected int in [0,%d) ] on comand line, got %s' % (
            len(test_strings), c)
        sys.exit()

    print 'index', index
    print '========== Test %d ==============' % index
    s = test_strings[index]
    print s
    _test(s)


if __name__ == '__main__':
    import sys

    c = ''.join(sys.argv[1:])
    ic = -1
    try:
        ic = int(c)
    except Exception:
        print 'expected int on command line, got ',c
        sys.exit()
    test(ic)
