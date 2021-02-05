# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function
from __future__ import absolute_import

from TrigHLTJetHypo.node import Node
from TrigHLTJetHypo.constants import lchars, param_alphabet

from AthenaCommon.Logging import logging
from AthenaCommon.Constants import DEBUG
logger = logging.getLogger( __name__)

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
    nl = s.count(pl)
    nr = s.count(pr)
    if nl != nr:
        raise RuntimeError(
            'Paren mismatch for parens %s, %s, %d %d ' % (pars, s, nl, nr))

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
    from TrigHLTJetHypo.constants import alphabet
    for c in s:
        if c not in alphabet:
            raise RuntimeError('bad character %s in string %s' % (c, s))
    check_parens(s)
    return s


class ChainLabelParser(object):
    def __init__(self, label, debug=False):

        self.label = label
        self.state = 'start'
        pp = preprocess(label)
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

        if debug:
            logger.setLevel(DEBUG)

    def paramAppend(self, c):
        self.parameters += c
        logger.verbose('parameters %s' % self.parameters)

    def scenAppend(self, c):
        self.scenario += c
        logger.verbose('scenario %s' % self.scenario)
            
    def start(self):
        "initialise"

        self.state = 'scenario'
        self.scenario = ''
        self.parameters = ''
        self.tree = [Node('dummy')]
        self.msg = ''
        

    def scen(self):
        """accumulate scenario name string into self.scenario"""
        
        c = next(self.gc)
    
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

        c = next(self.gc)
    
        if c == '[':
            self.state = 'start_params_1'
            return

        self.msg = 'state %s, bad character %s in string %s' % (self.state,
                                                                c,
                                                                self.label)
        self.state = 'error'

    def start_params_1(self):
        """accumulate parameter string into self.parameter"""
        
        c = next(self.gc)
    
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
        
        c = next(self.gc)
    
        if c in param_alphabet:
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

        c = next(self.gc)

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
        
        c = next(self.gc)
        
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
            c = next(self.gc)
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

        msg = '\n'.join(
            ['---error state report ---',
             'state ' + self.state,
             'scenario ' +  self.scenario,
             'parameters ' + str(self.parameters),
             'msg ' +  self.msg,
             'state history ' + str(self.state_history),
             'tree dump:',
             self.tree[0].dump(),
             '--end error state report---',
             '\n'])
        
        logger.error(msg)
        raise RuntimeError(msg)
    
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

                self.get_state()()
                
        except StopIteration:  # generator has reached the end of the string
            logger.debug('parse terminated')
            terminated = True
        except AssertionError as e:
            logger.error(e)
            error = True
        except RuntimeError as e:
            logger.error(e)
            error = True
            
    
        if not terminated:
            s = ''
            try:
                while True:
                    s += next(self.gc)
            except StopIteration:
                if s:
                    logger.error('error: remaining characters:', s)
     
        if len(self.tree) != 1:
            error = True

            logger.error('stack size ', len(self.tree),
                         'expected 2\n',
                         str(self.state_history))
            

        final_state = 'end_scenario'
        if self.state != final_state:
            error = True

            logger.error('final state is ', self.state,
                         'expected %s', final_state)
            
        if not error:
            logger.debug('parse succeeded')
        else:

            logger.error(
                'parse failed ',
                'state: %s scenario: %s parameters: %s stack len %d' % (
                    self.state, self.scenario, self.parameters, len(self.tree)
                )
            )
            

        # Kludge: mark the tops of the trees. The visitor which
        # creates Tool instances with give the Tool for this node
        # the name of the chain
    
        for c in self.tree[0].children:
            c.tree_top = True

        # return hypo forest (>= 1 trees)
        return self.tree[0].children

def _test(s):
    from TrigHLTJetHypo.ChainLabelParser import ChainLabelParser
    parser = ChainLabelParser(s, debug=True)
    trees = parser.parse()

    logger.debug('JetLabelParser _test: No of trees produced: ' +
                 str(len(trees)))
    
    for t in trees:
        logger.debug(t.dump())


def test():
    import sys
    logger.setLevel(DEBUG)
    
    from TrigHLTJetHypo.test_cases import test_strings
    c = sys.argv[1]
    index = -1

    try:
        index = int(c)
    except Exception:
        logger.error(
            'JetLabelParser.test() expected int in [1,', len(test_strings),
            '] ]on comand line, got ', c)
        sys.exit()
        
    if(index < 0 or index > len(test_strings) - 1):
        logger.error('JetLabelParser.test() index ', index,
                     ' not in range [0,', len(test_strings) -1, ']')
        sys.exit()
                                                 
            
    logger.debug('JetLabelParser.test() index ', str(index))
    label = test_strings[index]
    logger.debug('========== Test ', str(index), ' ==============')
    logger.debug('========== label ', label, '% ==============')
    _test(label)

  
if __name__ == '__main__':

    test()
