# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from constants import lchars

import re

from ToolSetter import ToolSetter
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

class SimpleConditionsDictMaker(object):
    """Convert parameter string into duction holding low, high window
    cut vals. Specialistaion for the 'simple' scenario

    parameter strings look like '40et, 0eta320, nosmc'
    """
    
    window_re = re.compile(
        r'^(?P<lo>\d*)(?P<attr>[%s]+)(?P<hi>\d*)' % lchars)

    defaults = {'eta_mins': 0.0,
                'eta_maxs': 3.2,
                'EtThresholds': 0.,
                'asymmetricEtas': 0,
    }

    scale_factors = {'eta': 0.01,
                     'et': 1000.,
                     'smc': 1000.,
    }

    def makeDict(self, params):

        
        def get_conditions():
            """Split conditions string into list of condition strings
            Condition string looks like
            '(10et,0eta320)(20et,0eta320)(40et,0eta320)'
            returned is ['10et,0eta320', '20et,0eta320', '40et,0eta320']
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


        conditions = get_conditions()


        attributes = ['EtThresholds',
                      'eta_mins',
                      'eta_maxs',
                      'asymmetricEtas',]

        result = {}
        msgs = []
        for a in attributes: result[a] = []

        for c in conditions:
            toks = c.split(',')
            toks = [t.strip() for t in toks]

            attributes2 = attributes[:]  # copy...
            for t in toks:
                m = self.window_re.match(t)
                if m is None:
                    msgs.append('match failed for parameter %s' % t)
                    error = True
                    return {}, error, msgs
                group_dict = m.groupdict()
                attr = group_dict['attr']
                lo = group_dict['lo']
                hi = group_dict['hi']
                if lo == '':
                    lo = self.defaults.get(attr+'lo', '')
                if hi == '':
                    hi = self.defaults.get(attr+'hi', '')

                sf = self.scale_factors[attr]
                if lo:
                    if attr == 'eta':
                        attr_lo = 'eta_mins'
                        result[attr_lo].append(sf * float(lo))
                        try:
                            attributes2.remove(attr_lo)
                        except ValueError, e:
                            print attr_lo, 'appears twice in Conditions string?'
                            raise e
                    elif attr == 'et':
                        attr = 'EtThresholds'
                        result[attr].append(sf * float(lo))
                        try:
                            attributes2.remove(attr)
                        except ValueError, e:
                            print 'et appears twice in Conditions string?'
                            raise e
                            
                if hi:
                    if attr == 'eta':
                        attr = 'eta_maxs'

                        attr_hi = 'eta_maxs'
                        result[attr_hi].append(sf * float(hi))
                        try:
                            attributes2.remove(attr_hi)
                        except ValueError, e:
                            print attr_hi, 'appears twice in Conditions string?'
                            raise e

            # fill in unmentioned attributes with defaults:
            for a in attributes2:
                result[a].append(self.defaults[a])

        msgs = ['ConditionsDict OK']
        error = False
        return result, error, msgs

class TreeParameterExpander_simple(object):
    """Convert parameter string into duction holding low, high window
    cut vals. Specialistaion for the 'simple' scenario

    parameter strings look like '40et, 0eta320, nosmc'
    """
    
    window_re = re.compile(
        r'^(?P<lo>\d*)(?P<attr>[%s]+)(?P<hi>\d*)' % lchars)

    defaults = {'eta_mins': 0.0,
                'eta_maxs': 3.2,
                'EtThresholds': 0.,
                'asymmetricEtas': 0,
    }

    scale_factors = {'eta': 0.01,
                     'et': 1000.,
                     'smc': 1000.,
    }

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

    
    scale_factors = {'deta': 0.11,
                     'mass': 1000.,
                     'dphi': 0.1,
    }

    defaults = {'mass_mins': '0.0',
                'mass_maxs': 'inf',
                'deta_mins': '0.',
                'deta_maxs': 'inf',
                'dphi_mins': '0.',
                'dphi_maxs': 'inf',
    }

    def __init__(self):
        self.msgs = []

    def mod(self, node):

        def get_conditions(params):
            """Split conditions string into list of condition strings
            Condition string looks like
            '(75m85,100dEta200, 50dphi200)(80m90, 110dEta210)'
            returned is ['10et,0eta320', '20et,0eta320', '40et,0eta320']
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

        ok = True # status flag
        conditions = get_conditions(node.parameters)

        bare_attrs = ['mass', 'deta', 'dphi']
        all_attrs = []
        for attr in bare_attrs:
            all_attrs.extend([attr+'_mins', attr+'_maxs'])

        for attr in all_attrs:
            node.conf_attrs[attr] = []

        for c in conditions:
            toks = c.split(',')
            toks = [t.strip() for t in toks]


            processed_attrs = []
            for t in toks:
                m = self.window_re.match(t)
                if m is None:
                    self.msgs.append('match failed for parameter %s' % t)
                    return
                group_dict = m.groupdict()
                attr = group_dict['attr']
                lo = group_dict['lo']
                hi = group_dict['hi']
                if lo == '':
                    lo = self.defaults.get(attr+'_min', '')
                if hi == '':
                    hi = self.defaults.get(attr+'_max', '')

                sf = self.scale_factors[attr]
                if lo:
                    if attr == 'mass':
                        node.conf_attrs['mass_mins'].append(str(sf*float(lo)))
                    elif attr == 'deta':
                        node.conf_attrs['deta_mins'].append(str(sf*float(lo)))
                    elif attr == 'dphi':
                        node.conf_attrs['dphi_mins'].append(str(sf*float(lo)))

                    processed_attrs.append(attr+'_mins')
                if hi:
                    if attr == 'mass':
                        node.conf_attrs['mass_maxs'].append(str(sf*float(lo)))
                    elif attr == 'deta':
                        node.conf_attrs['deta_maxs'].append(str(sf*float(lo)))
                    elif attr == 'dphi':
                        node.conf_attrs['dphi_maxs'].append(str(sf*float(lo)))

                    processed_attrs.append(attr+'_maxs')



            for a in all_attrs:
                if a not in processed_attrs:
                    node.conf_attrs[a].append(self.defaults[a])

            for a in processed_attrs:
                if a not in all_attrs:
                    self.msgs.append('Unknown attribute: %s' % a)
                    ok = False

        if ok:
            self.msgs = ['All OK']
        else:
            self.msgs.append('Error')

        
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
        size_re = re.compile(r'^\((\d+)\)')
        parameters = node.parameters[:]
        m = size_re.match(parameters)
        if m is None:
            self.msgs.append('Error')
            return

        node.conf_attrs = {'groupSize':int(m.groups()[0])}
        # remove goup info + 2 parentheses
        parameters = parameters[len(m.groups()[0])+2:]

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
    """Class to exapnder node.paramters string. Delegates to
    specialised exanders."""
    
    router = {
        'simple': TreeParameterExpander_simple,
        'simplepartition': TreeParameterExpander_simple,
        'dijet': TreeParameterExpander_dijet,
        'not': TreeParameterExpander_null,
        'and': TreeParameterExpander_null,
        'or': TreeParameterExpander_null,
        'combgen': TreeParameterExpander_combgen,
        'partgen': TreeParameterExpander_partgen,
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
        print 'expected int in [1,%d] ]on comand line, got %s' % (
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
