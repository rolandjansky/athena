# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from node import Node
from constants import (lchars,
                       digits,
                       delims)



def get_char(s):
    """character generator"""

    while s:
        c = s[0]
        s = s[1:]
        yield c

def _check_parens(s, pars):
    pl = pars[0]
    pr = pars[1]
    np = 0
    for c in s:
        if c == pl: np += 1
        if c == pr: np -= 1
        if np < 0:
            raise RuntimeError('Paren mismatch for parens %s, %s' % (pars, s))

def check_parens(s):
    _check_parens(s, '()')
    _check_parens(s, '[]')
    
            

def preprocess(s):
    llines  = s.split('\n')

    lines  = [l.split('#')[0].strip() for l in llines]
    s = ''.join(lines)
    ss = ''
    reject = ' \n'
    for c in s:
        # keep white space whn within square brackets, reject otherwise
        if c not in reject:
            ss += c
    s = ss      

    check_parens(s)
    print s
    from constants import alphabet
    for c in s:
        if not c in alphabet:
            raise RuntimeError('bad character %s in string %s' % (c, s))
    print 'end of preprocess: ', s
    return s


class ChainLabelParser(object):
    def __init__(self, label, debug=False):
        self.label = label
        self.state = 'start'
        pp = preprocess(label)
        print 'preprocessd string', pp, 'length', len(pp)
        self.gc = get_char(pp)
        self.state_history = []
        self.states = {
            'start': self.start,
            'scenario': self.scen,
            'start_params_0': self.start_params_0,
            'start_params_1': self.start_params_1,
            'params': self.params,
            'end_params_0': self.end_params_0,
            'end_params': self.end_params,
            'end_scenario': self.end_scenario,
            'error': self.error,
        }
        self.debug = debug

    def paramAppend(self, c):
        self.parameters += c
        if self.debug:
            print 'parameters', self.parameters

    def scenAppend(self, c):
        self.scenario += c
        if self.debug:
            print 'scenario', self.scenario
            
    def start(self):
        "initialise"

        self.state = 'scenario'
        self.scenario = ''
        self.parameters = ''
        self.tree = [Node('dummy')]
        self.msg = ''
        

    def scen(self):
        """accumulate scenario name string into self.scenario"""
        
        c = self.gc.next()
    
        if c in lchars: 
            self.scenAppend(c)
            return
    
        if c == '(':
            self.state = 'start_params_0'

            return
    
        self.msg = 'state %s, bad character %s in string %s' % (self.state,
                                                                c,
                                                                self.label)
        self.state = 'error'

    def start_params_0(self):
        """accumulate parameter string into self.parameter"""

        self.tree.append(Node(self.scenario))
        self.scenario = ''

        c = self.gc.next()
    
        if c == '[':
            self.state = 'start_params_1'
            return

        self.msg = 'state %s, bad character %s in string %s' % (self.state,
                                                                c,
                                                                self.label)
        self.state = 'error'

    def start_params_1(self):
        """accumulate parameter string into self.parameter"""
        
        c = self.gc.next()
    
        if c == '(':
            self.paramAppend(c)
            self.state = 'params'
            return

        if c == ']':
            self.state = 'end_params'
            return


        self.msg = 'state %s, bad character %s in string %s' % (self.state,
                                                                c,
                                                                self.label)
        self.state = 'error'

    def params(self):
        """accumulate parameter string into self.parameter"""
        
        c = self.gc.next()
    
        if c in lchars or c in digits or c ==',':
            self.paramAppend(c)
            return

        if c == ')':
            self.paramAppend(c)
            self.state = 'end_params_0'
            return


        self.msg = 'state %s, bad character %s in string %s' % (self.state,
                                                                c,
                                                                self.label)
        self.state = 'error'
    

    def end_params_0(self):
        """Check whether there are more prameters to accumulate"""

        c = self.gc.next()

        # more parameters
        if c == '(':
            self.paramAppend(c)
            self.state = 'params'
            return

        # end of paramter lists
        if c == ']':
            self.tree[-1].parameters = self.parameters.strip()
            self.parameters = ''
            self.state = 'end_params'
            return

        self.msg = 'state %s, bad character %s in string %s' % (self.state,
                                                                c,
                                                                self.label)
        self.state = 'error'
    

    def end_params(self):
        """after accumulating params, drop white space, then add node to tree
        or process next scenario"""
        
        c = self.gc.next()
        
        if c == ')':
            self.state = 'end_scenario'
            return
        
        if c in lchars:
            self.scenAppend(c)
            self.state = 'scenario'
            return
        
        self.msg = 'state %s, bad character %s in string %s' % (self.state,
                                                                c,
                                                                self.label)
        self.state = 'error'
        
    def end_scenario(self):
        """Add current node to its parent"""
        
        n = self.tree.pop()
        self.tree[-1].add_child(n)

        while True:
            c = self.gc.next()
            if c == ')':
                n = self.tree.pop()
                self.tree[-1].add_child(n)
            else:

                # more input means a new scenario is starting
                self.scenario = c
                self.parameters = ''
                self.msg = ''
                self.state = 'scenario'
        
                return    

        
    def error(self):
        """From error state, dump error report and raise exception"""
        
        print '---error state report ---'
        print ' state', self.state
        print ' scenario', self.scenario
        print ' parameters', self.parameters
        print ' msg', self.msg
        print 'state history', self.state_history
        print ' tree dump:'
        print self.tree[0].dump()
        print '--end error state report---'
        raise RuntimeError('error state')
    
    def get_state(self):
        self.state_history.append(self.state)
        return self.states[self.state]
    

    
    def parse(self):
        "parse a chain label"
        
        error = False
        terminated = False

        self.start()  # re-initialise
        
        try:   # parsing ends with an exception
            while True:   # continue until exception

                # print 'current state', self.state
                # print 'current scenario', self.scenario
                # print 'current parameters', self.parameters
                
                # self.states[self.state]()  # process the current state
                self.get_state()()
                
        except StopIteration:  # generator has reached the end of the string
            print 'parse terminated'
            terminated = True
        except AssertionError, e:
            print 'assertion err'
            print e
            error = True
        except RuntimeError, e:
            print e
            error = True
            
    
        if not terminated:
            s = ''
            try:
                while True:
                    s += self.gc.next()
            except StopIteration:
                if s:
                    print 'error: remaining characters:', s
     
        if len(self.tree) != 1:
            error = True
            print 'error, stack size', len(self.tree), 'expected 2'
            print self.state_history
            
        if len(self.tree[0].children) != 1:
            error = True
            print 'error, top node has %d cdildren, expected 1' % (
                len(self.tree[0].children))

        final_state = 'end_scenario'
        if self.state != final_state:
            error = True
            print 'error: final state is %s, expected %s' % (self.state,
                                                             final_state)
        # print 'tree dump:'
        # print self.tree[0].dump()
        print 'parse',
        if not error:
            print 'succeeded'
        else:
            print 'state: %s scenario: %s parameters: %s stack len %d' % (
                self.state, self.scenario, self.parameters, len(self.tree))
            print 'failed'

        # Kludge: mark the tops of the trees. The visitor which
        # creates Tool instances with give the Tool for this node
        # the name of the chain
    
        for c in self.tree[0].children:
            c.tree_top = True

        # for now (02/01/2019), no reco. First tree is only tree is hypo
        return self.tree[0].children[0]

def _test(s):
    from ChainLabelParser import ChainLabelParser
    parser = ChainLabelParser(s, debug=True)
    tree = parser.parse()
    print tree.dump()


def test(index):
    from test_cases import test_strings
    c = sys.argv[1]
    index = -1
    try:
        index = int(c)
    except:
        print 'expected int in [1,%d] ]on comand line, got %s' % (
            len(test_strins), c)
        sys.exit()

    print 'index', index
    print '========== Test %d ==============' % index
    s = test_strings[index]
    print s
    _test(s)


def usage(options):
    print 'usage: ChainLabelPaers -[%s]' % options

  
if __name__ == '__main__':

    import getopt, sys
    from test_cases import test_strings
    ncases = len(test_strings)
    try:
        options = "1234567"
        opts, args = getopt.getopt(sys.argv[1:], options, [])
    except getopt.GetoptError as err:
        # print help information and exit:
        print str(err) # will print something like "option -a not recognized"
        usage(options)
        sys.exit(2)

    assert len(args) == 1
    o = args[0]
    try:
        index = int(o)
    except:
        print 'Supply an test case integer index  on the command line '
        sys.exit(0)
    
    if index < 0 or index >= ncases:
        print 'no such test case ind %d, expect val in [0, %d]'  %(index,
                                                                   ncases-1)
        sys.exit(0)

    test(int(o))
    
