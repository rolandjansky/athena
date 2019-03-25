# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from constants import lchars

import re
import math

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


class TreeParameterExpander_simple(object):
    """Convert parameter string into duction holding low, high window
    cut vals. Specialistaion for the 'simple' scenario

    parameter strings look like '40et, 0eta320, nosmc'
    """
    
    window_re = re.compile(
        r'^(?P<lo>\d*)(?P<attr>[%s]+)(?P<hi>\d*)' % lchars)

    defaults = {'etalo': 0.0,
                'etahi': 3.2,
    }

    scale_factors = {'eta': 0.01,
                     'et': 1000.,
                     'smc': 1000.,
    }

    def __init__(self):
        self.msgs = []

    def mod(self, node):

        def get_conditions(params):
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


        conditions = get_conditions(node.parameters)

        
        node.conf_attrs['EtThresholds'] = []
        node.conf_attrs['eta_mins'] = []
        node.conf_attrs['eta_maxs'] = []
        node.conf_attrs['asymmetricEtas'] = []

        for c in conditions:
            toks = c.split(',')
            toks = [t.strip() for t in toks]


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
                    lo = self.defaults.get(attr+'lo', '')
                if hi == '':
                    hi = self.defaults.get(attr+'hi', '')

                sf = self.scale_factors[attr]
                if lo:
                    if attr == 'eta':
                        node.conf_attrs['eta_mins'].append(sf * float(lo))
                    elif attr == 'et':
                        node.conf_attrs['EtThresholds'].append(sf * float(lo))
                if hi:
                    if attr == 'eta':
                        node.conf_attrs['eta_maxs'].append(sf * float(hi))
                
        #01/01/2019 PS  KLUDGE !! FIX ME!!!  asymmetric eta hardwired to 0.

        [node.conf_attrs['asymmetricEtas'].append(0) for i in range(
                len(conditions))]
        self.msgs = ['All OK']

        
    def report(self):
        return '%s: ' % self.__class__.__name__ + '\n'.join(self.msgs) 

    


class TreeParameterExpander_dijet(object):
    """Convert parameter string into duction holding low, high window
    cut vals. Specialistaion for the dijet scenario

    parameter strings look like '40m,100deta200, 50dphi300'
    """
    
    window_re = re.compile(
        r'^(?P<lo>\d*)(?P<attr>[%s]+)(?P<hi>\d*)' % lchars)

    defaults = {'mass_mins': 0.0,
                'mass_maxs': 100000.,
                'deta_mins': 0.,
                'deta_maxs': 20.,
                'dphi_mins': 0.,
                'dphi_maxs': math.pi,
    }

    scale_factors = {'deta': 0.01,
                     'mass': 1000.,
                     'dphi': 0.01,
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
                        node.conf_attrs['mass_mins'].append(sf * float(lo))
                    elif attr == 'deta':
                        node.conf_attrs['deta_mins'].append(sf * float(lo))
                    elif attr == 'dphi':
                        node.conf_attrs['dphi_mins'].append(sf * float(lo))

                    processed_attrs.append(attr+'_mins')
                if hi:
                    if attr == 'mass':
                        node.conf_attrs['mass_maxs'].append(sf * float(lo))
                    elif attr == 'deta':
                        node.conf_attrs['deta_maxs'].append(sf * float(lo))
                    elif attr == 'dphi':
                        node.conf_attrs['dphi_maxs'].append(sf * float(lo))

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
        'dijet': TreeParameterExpander_dijet,
        'not': TreeParameterExpander_null,
        'and': TreeParameterExpander_null,
        'or': TreeParameterExpander_null,
    }

    def __init__(self):
        self.expander = None

    def mod(self, node):
        self.expander = self.router[node.scenario]()
        self.expander.mod(node)
        print self.expander.report()
    def report(self):
        return self.expander.report()
        


class TreeToCascade(object):
    def __init__(self):
        self.stack = []

    def mod(self, node):
        
        self.stack.append(
            '(%s < (%s))' %(node.tool.name(), 
                            ' '.join([c.tool.name  for c in node.children])))

    def report(self):
        s = ''
        while self.stack:
            s += self.stack.pop()
        return s.strip()


def _test(s):

    from ChainLabelParser import ChainLabelParser
    parser = ChainLabelParser(s)
    tree = parser.parse()
    print tree.dump()
    # exapnd the window cuts (strings) obtained from the chain label
    # to attributes and floating point numbers, set defaults
    # for unspecified vallues
    visitor = TreeParameterExpander()
    tree.accept(visitor)
    print visitor.report()
    print tree.dump()


    # set the node attribute node.tool to be the hypo  Al\gTool.
    print 'sending in the ToolSetter visitor'
    ts_visitor = ToolSetter(s)
    tree.accept_cf(ts_visitor)
    print ts_visitor.report()


    print tree.dump()


def test(index):
    from test_cases import test_strings
    import sys
    c = sys.argv[1]
    index = -1
    try:
        index = int(c)
    except:
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
    c = sys.argv[1]
    ic = -1
    try:
        ic = int(c)
    except:
        print 'expected int on command line, got ',c
        sys.exit()
    test(ic)
