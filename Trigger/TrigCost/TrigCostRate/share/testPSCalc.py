#!/usr/bin/env python

from optparse import OptionParser

p = OptionParser(usage="usage: -n <number of events>", version="0.1")

p.add_option('-n', "--nevents",   type="int",          default=10000,  dest="nevent",    help="number of events")
p.add_option('-r', "--randseed",  type="int",          default=25879,  dest="randseed",  help="random seed")
p.add_option('--nl1',             type='int',          default=1,      dest="nl1",       help="number of L1 triggers")
p.add_option('--nl2',             type='int',          default=2,      dest="nl2",       help="number of L2 triggers")
p.add_option('--nl3',             type='int',          default=2,      dest="nl3",       help="number of L3 triggers")
p.add_option('-d', '--debug',     action='store_true', default=False,  dest="debug",     help="turn on debug option")
p.add_option('--corrl1',          action='store_true', default=False,  dest="corrl1",    help="include L1 correlations")

(opts, args) = p.parse_args()

import os, string, sys, random,math

random.seed(opts.randseed)

#-----------------------------------------------------------------------------
# Class for single trigger: contains decisions for one event and global counts
#
class Trigger:

    def __init__(self):
        pass

    level   = 0
    tbp     = False # trigger pass before PS
    tap     = False # trigger pass after  PS
    id      = 0
    id_prev = 0
    ps      = 1.0
    ps_inv  = 0.0
    eff     = 0.0
    
    count_tbp = 0
    count_tap = 0

    def headerString(self):
        res = 'L%d'%self.level+'_T%02d'%self.id
        if   self.level == 2: res += ' <--L1_T%02d'%self.id_prev
        elif self.level == 3: res += ' <--L2_T%02d'%self.id_prev        
        else:                 res += "          "
        return res

    def configString(self, event):
        res  = self.headerString()
        res += '  eff=%1.3f'%self.eff+' ps=%2d'%self.ps

        if self.level == 2:
            p = event.l1[self.id_prev]
            res += '  Teff=%1.3f'%(self.eff*p.eff)+' Tps=%2d'%(self.ps*p.ps)
        elif self.level == 3:
            p2 = event.l2[self.id_prev]
            p1 = event.l1[  p2.id_prev]
            res += '  Teff=%1.3f'%(self.eff*p1.eff*p2.eff)+' Tps=%2d'%(self.ps*p1.ps*p2.ps)
            
        return res

    def stateString(self):
        return self.headerString()+'  tbp %d'%int(self.tbp)+', tap %d'%int(self.tap)

    def countString(self):
        return self.headerString()+'  tbp %6d'%self.count_tbp+', tap %6d'%self.count_tap

    def generateDecision(self, inputTrigger):
        self.tbp = False
        self.tap = False

        if self.level == 1:
            if random.random() < self.eff:
                self.tbp = True
            if self.tbp and self.ps > 0.0 and random.random() < self.ps_inv:
                self.tap = True
        else:
            if inputTrigger.id != self.id_prev:
                print 'Logic error for previous trigger'
                return
            if not inputTrigger.tbp:
                return

            # compute decision w/o PS if input trigger passsed
            if inputTrigger.tbp and random.random() < self.eff:
                self.tbp = True

            # ser decision with PS if input trigger passed after PS
            if inputTrigger.tap and self.ps > 0.0 and random.random() < self.ps_inv:
                self.tap = self.tbp


        self.count_tap += int(self.tap)
        self.count_tbp += int(self.tbp)

#-----------------------------------------------------------------------------
# Fill correlation matrix for triggers
#
def makeCorr(size):
    corr = size*[size*[0.0]]
    for i in range(0, size):
        for j in range(0, size):
            if i + j % 2 == 0 and i + j > 0:
                corr[i][j] = random.uniform(0.0, 1.0/float(i+j))
    print corr
    return corr

if opts.corrl1:
    l1_corr = makeCorr(opts.nl1)
    l2_corr = makeCorr(opts.nl2)
    l3_corr = makeCorr(opts.nl3)


#-----------------------------------------------------------------------------
# Class for one event: contains decisions for all triggers and global counts
#
class Event:

    def __init__(self):
        pass
    
    l1 = []
    l2 = []
    l3 = []    
    
    l1_tbp = 0
    l1_tap = 0
    l2_tbp = 0
    l2_tap = 0
    l3_tbp = 0
    l3_tap = 0
    
    l1_sum  = 0.0
    l1_err  = 0.0    
    l2_sum  = 0.0
    l2_err  = 0.0
    l2_sum2 = 0.0
    l2_err2 = 0.0    
    l3_sum  = 0.0
    l3_err  = 0.0
    l3_sum2 = 0.0
    l3_err2 = 0.0      
    
    def configure(self, nl1, nl2, nl3):

        for i in range(0, nl1):
            t = Trigger()
            t.level  = 1
            t.id     = i
            t.ps     = random.randint(1, 20)
            t.ps_inv = 1.0/t.ps
            t.eff    = random.uniform(0.05, 0.4)
            self.l1 += [t]

        for i in range(0, nl2):
            t = Trigger()
            t.level   = 2
            t.id      = i
            t.ps      = random.randint(1, 10)
            t.ps_inv  = 1.0/t.ps
            t.id_prev = random.randint(0, nl1-1)
            t.eff     = random.uniform(0.2, 0.6)
            self.l2 += [t]

        for i in range(0, nl3):
            t = Trigger()
            t.level   = 3
            t.id      = i
            t.ps      = random.randint(1, 5)
            t.ps_inv  = 1.0/t.ps
            t.id_prev = random.randint(0, nl2-1)
            t.eff     = random.uniform(0.3, 0.9)            
            
            self.l3 += [t]

    def generate(self):
        l1_pass_tbp = False
        l1_pass_tap = False
        l2_pass_tbp = False
        l2_pass_tap = False
        l3_pass_tbp = False
        l3_pass_tap = False
        
        l1_weight  = 0.0
        l2_weight  = 0.0
        l2_weight2 = 0.0
        l3_weight  = 0.0
        l3_weight2 = 0.0

        l2w  = len(self.l1)*[0.0] # array of weights for L2 triggers with common L1 seed
        l3w1 = len(self.l1)*[0.0] # array of weights for L3 triggers with common L1 seed
        l3w2 = len(self.l2)*[0.0] # array of weights for L3 triggers with common L2 seed        

        # Compute decisions
        for t in self.l1: t.generateDecision(None)

        # Compute L1 correlations
        if opts.corrl1:
            for i in range(0, len(self.l1)):
                for j in range(0, len(self.l1)):
                    if i != j and self.l1[i].tbp and not self.l1[j].tbp and random.random() < l1_corr[i][j]:
                        self.l1[j].tbp = True
                        self.l1[j].tap = random.random() < self.l1[j].ps_inv

        # Collect L1 decisioons
        for t in self.l1:
            l1_pass_tbp = l1_pass_tbp or t.tbp
            l1_pass_tap = l1_pass_tap or t.tap
            
            if t.tbp:
                l1_weight = t.ps_inv + l1_weight*(1.0-t.ps_inv)

        # Collect L2 decisions
        for t in self.l2:
            p = self.l1[t.id_prev]
            t.generateDecision(p)
            l2_pass_tbp = l2_pass_tbp or t.tbp
            l2_pass_tap = l2_pass_tap or t.tap

            # Collect weights for L2 triggers which share L1 seed
            if t.tbp:
                w = p.ps_inv*t.ps_inv
                l2_weight      = w        + l2_weight     *(1.0-w)
                l2w[t.id_prev] = t.ps_inv + l2w[t.id_prev]*(1.0-t.ps_inv)

        # Reweight L2 weights by PS of common L1 seed
        for i in range(0, len(l2w)):
            l2w[i] *= self.l1[i].ps_inv

        # Compute total L2 weight
        for x in l2w:
            l2_weight2 = x + l2_weight2*(1.0-x)

        # Collect L2 decisions
        for t in self.l3:
            p2 = self.l2[ t.id_prev]
            p1 = self.l1[p2.id_prev]
            
            t.generateDecision(p2)
            l3_pass_tbp = l3_pass_tbp or t.tbp
            l3_pass_tap = l3_pass_tap or t.tap

            # Collect weights for L3 triggers which share L2 seed
            if t.tbp:
                w = p1.ps_inv*p2.ps_inv*t.ps_inv
                l3_weight       = w        + l3_weight      *(1.0-w)
                l3w2[t.id_prev] = t.ps_inv + l3w2[t.id_prev]*(1.0-t.ps_inv)

        # Collect weights for L2 seeds to L2 trigger and which share L1 seed 
        for i in range(0, len(l3w2)):
            l2 = self.l2[i]
            if l2.tbp:
                w = l2.ps_inv*l3w2[i]
                l3w1[l2.id_prev] = w + l3w1[l2.id_prev]*(1.0-w)

        # Reweight L2 weights by PS of common L1 seed
        for i in range(0, len(l3w1)):
            l3w1[i] *= self.l1[i].ps_inv

        # Compute total L3 weight
        for x in l3w1:
            l3_weight2 = x + l3_weight2*(1.0-x)
                
        if l1_pass_tbp: self.l1_tbp += 1
        if l1_pass_tap: self.l1_tap += 1

        if l2_pass_tbp: self.l2_tbp += 1
        if l2_pass_tap: self.l2_tap += 1        

        if l3_pass_tbp: self.l3_tbp += 1
        if l3_pass_tap: self.l3_tap += 1   

        self.l1_sum  += l1_weight
        self.l1_err  += l1_weight*l1_weight
        
        self.l2_sum  += l2_weight
        self.l2_err  += l2_weight*l2_weight
        self.l2_sum2 += l2_weight2
        self.l2_err2 += l2_weight2*l2_weight2

        self.l3_sum  += l3_weight
        self.l3_err  += l3_weight*l3_weight
        self.l3_sum2 += l3_weight2
        self.l3_err2 += l3_weight2*l3_weight2
        
    def finalize(self):
        self.l1_err  = math.sqrt(self.l1_err)
        self.l2_err  = math.sqrt(self.l2_err)
        self.l2_err2 = math.sqrt(self.l2_err2)        
        self.l3_err  = math.sqrt(self.l3_err)
        self.l3_err2 = math.sqrt(self.l3_err2)
        
    def printConfig(self):

        print 'Print %d'%len(self.l1),'L1 triggers:'
        for t in self.l1:
            print t.configString(self)
        print '\nPrint %d'%len(self.l2),'L2 triggers:'
        for t in self.l2:
            print t.configString(self)
        print '\nPrint %d'%len(self.l3),'L3 triggers:'
        for t in self.l3:
            print t.configString(self)
        print '-----------------------------------------------------\n'

    def printState(self):
        for t in self.l1: print t.stateString()
        for t in self.l2: print t.stateString()

    def printCount(self):
        for t in self.l1: print t.countString()
        print
        for t in self.l2: print t.countString()
        print
        for t in self.l3: print t.countString()
        print
        
        print 'L1 tbp = %6d'%self.l1_tbp,'+/- %4d'%math.sqrt(self.l1_tbp),' - pass before PS'
        print 'L1 tap = %6d'%self.l1_tap,'+/- %4d'%math.sqrt(self.l1_tap),' - pass after  PS'
        print 'L2 tbp = %6d'%self.l2_tbp,'+/- %4d'%math.sqrt(self.l2_tbp),' - pass before PS'
        print 'L2 tap = %6d'%self.l2_tap,'+/- %4d'%math.sqrt(self.l2_tap),' - pass after  PS'
        print 'L3 tbp = %6d'%self.l3_tbp,'+/- %4d'%math.sqrt(self.l3_tbp),' - pass before PS'
        print 'L3 tap = %6d'%self.l3_tap,'+/- %4d'%math.sqrt(self.l3_tap),' - pass after  PS'
        
        print
        print 'L1 sum  = %6d'%self.l1_sum, '+/ %4.1f'%self.l1_err, ' - usual weighted event count'
        print 'L2 sum  = %6d'%self.l2_sum, '+/ %4.1f'%self.l2_err, ' - usual weighted event count'
        print 'L2 sum2 = %6d'%self.l2_sum2,'+/ %4.1f'%self.l2_err2,' - new   weighted event count'
        print 'L3 sum  = %6d'%self.l3_sum, '+/ %4.1f'%self.l3_err, ' - usual weighted event count'
        print 'L3 sum2 = %6d'%self.l3_sum2,'+/ %4.1f'%self.l3_err2,' - new   weighted event count'


if __name__ == "__main__":
    #
    # Configure trigger and execute events
    #
    
    evt = Event()
    evt.configure(opts.nl1, opts.nl2, opts.nl3)
    evt.printConfig()

    for ievt in range(0, opts.nevent):
        evt.generate()

        if opts.debug:
            print 'Event %d'%ievt        
            evt.printState()
            
    evt.finalize()
    evt.printCount()
