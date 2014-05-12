#!/bin/env python
#
# atn basepath
# /afs/cern.ch/atlas/software/builds/nightlies/devval/AtlasTrigger/$rel/NICOS_area/NICOS_qmtest15X0VAL32BS4TrgOpt/triggertest_testconfiguration_work/$test/$ref


import glob
from itertools import chain
import ROOT
import os
from datetime import datetime
import logging as log
import xml.etree.cElementTree as ET
import re

log.basicConfig(format='%(levelname)-15s  %(message)s', level=log.ERROR)

#exec (file("/afs/cern.ch/atlas/software/dist/AtlasLogin/python/atlconfiguration.py"))



class CountsSummaryException:
    pass
#atlconf = Configuration()
class Utils:
    @staticmethod
    def today():
        return datetime.today().isoweekday()%7

    @staticmethod
    def testdate(file):
        """
        Returnd date when the test was run.        
        """
        return datetime.fromtimestamp(os.path.getmtime(file))

    @staticmethod
    def wildcarding(list_of_tests):
        def __can_be_wildcarded(str1, str2):
            if len(str1) == len(str2):
                different = ''.join([ (x[0], '*')[x[0] != x[1]] for x in zip(str1, str2)])
                if different.count('*') == 1:
                    return different
            return None
        
        tests = sorted(list(list_of_tests), key=lambda x: str(len(x))+x)
        #print tests

        possible_wildcards = set()
        for pair in zip(tests[:-1], tests[1:]):
            pattern = __can_be_wildcarded(pair[0], pair[1])
            #print pattern, pair
            if pattern:
                possible_wildcards.add(pattern)
        tests.extend(possible_wildcards)
        return tests



class CountsCheck:
    """
    Counts checkers.
    """
    def __init__(self, failed, text):
        self.failed = bool(failed)
        self.text = text

    @staticmethod
    def any_different(counts):
            return CountsCheck(len(set(counts)) != 1, 'diff')

    @staticmethod
    def two_perecnt(counts):
        day_to_day = zip(counts[:-1], counts[1:])
        diffs = map(lambda x: 1.0*abs(x[0]-x[1]) > 0.02*max(x), day_to_day)
        return CountsCheck(bool(diffs.count(True)), "day-to-day>2%" )

    @staticmethod
    def five_perecnt(counts):
        day_to_day = zip(counts[:-1], counts[1:])
        diffs = map(lambda x: 1.0*abs(x[0]-x[1]) > 0.05*max(x), day_to_day)
        return CountsCheck(bool(diffs.count(True)), "day-to-day>5%" )

    @staticmethod
    def went_to_zero(counts):
        return CountsCheck( CountsCheck.any_different(counts).failed and counts.count(0) , "hit zero")

    @staticmethod
    def disabled(counts):
        return CountsCheck( counts.count(-1) , "disabled")

    @staticmethod
    def rtt_robust_check(counts):
        day_to_day = zip(counts[:-1], counts[1:])
        diffs = map(lambda x: 1.*abs(x[0]-x[1])>5, day_to_day) 
        return CountsCheck(bool(diffs.count(True)) and CountsCheck.five_perecnt(counts), "rtt check" )

class Atn:

    def location(self, build, test, rel):
        """ Finds the location of the ATN test

        It needs to know build, test name and release. It searches 4 possible projects
        AtlasTrigger, AtlasHLT, AtlasP1HLt, AtlasCAFHLT (this is this nasty piece with square brackets ..
           I could not make glow working with (Atlas|HLT|...).

        Returned value is list of matching paths, and list of actual test names
        """
        tog = '/afs/cern.ch/atlas/software/builds/nightlies/'+build+'/Atlas[TPCH][r1AL]*/'+\
              rel+'/NICOS_area/NICOS_atntest*32BS5G4*Opt/*/'+test
        #print tog
        g = glob.glob(tog)
        return g, [x.split('/')[-1] for x in g]



    def available_tests(self, build='*'):
        #tog='/afs/cern.ch/atlas/software/builds/nightlies/*/Atlas[TPCH][r1AL]*/rel_*/'
        def basic_from_path(path):
            return path.split('/')[7:10] 

        def build_test_from_path(path):
            return tuple(path.split('/')[7:14:6]) # the build is at position 7 and at 13 is the test dir (7+6 == 13)
        
        tog='/afs/cern.ch/atlas/software/builds/nightlies/'+build+'/Atlas[TPCH][r1AL]*/rel_*/'
        stage1 = glob.glob(tog)
        #print stage1

        tests = {}
        for path in stage1:
            details = basic_from_path(path)
            #print details
            tog = '/afs/cern.ch/atlas/software/builds/nightlies/'+details[0]+'/'+details[1]+'/'+details[2]+'/NICOS_area/NICOS_atntest*32BS5G4*Opt/trig*configuration_work/*/expert-monitoring.root'
            # print '.... scanning ', tog
            stage2 = glob.glob(tog)
            for zz in stage2:
                build, test_name = build_test_from_path(zz)
                # print build, testdir
                tests.setdefault(build, set()).add(test_name)
        return tests

    def checks(self):
        return [ CountsCheck.any_different,                              
                 CountsCheck.went_to_zero,
                 CountsCheck.disabled ]

                        
   
class Rtt:
    __domain='*'
    def __init__(self, domain):
        self.__memoized_tests = {} # keyed by the test_name
        self.__memoized_test_names = []
        Rtt.__domain=domain

    @staticmethod
    def __test_name(path):
        br = path.split('/')
        f = '/'.join(br[:-1])+'/rttjobinfo.xml'
        name = None
        try:
            et = ET.parse(f)
            name = et.getroot().findall('.//jobDisplayName')[0].text.strip('\n\t ')
            if name == '':                
                name = et.getroot().findall('.//jobName')[0].text.strip('\n\t ')
            del et
        except:
            log.warning("... problem finding jobDisplayName in: %s, ignoring this test" % f)
        return name

    @staticmethod
    def __details(path):
        br = path.split('/')
        # get info from the path                    
        rel     = br[9] 
        build   = br[10]
        return rel, build

    @staticmethod
    def __path(build, rel='*'):
        #p = '/afs/cern.ch/atlas/project/RTT/prod/Results/rtt/%s/%s/build/i686-slc5-gcc43-opt/%s/Trig*Test/*/*/*/expert-monitoring.root' % (rel, build, Rtt.__domain)
        p = '/afs/cern.ch/atlas/project/RTT/prod/Results/rtt/%s/%s/build/i686-slc5-gcc43-opt/%s/Trig*Test/*/expert-monitoring.root' % (rel, build, Rtt.__domain)
        log.debug('rtt path: %s' % p)
        return  p
    
    def location(self, build, test, rel, domain='*'):
        """
        Return locations and actual test names (the test param can be wildcard).
        
        """
        key = test+build
        if key not in self.__memoized_test_names:
            #print ".... memoizing test name", test, build, rel
            self.__memoized_test_names.append(key)
            tog = Rtt.__path(build)
            dirs = glob.glob(tog)
            for path in dirs:
                test_name = Rtt.__test_name(path)
                if test_name != None:
                    self.__memoized_tests.setdefault(test_name, []).append(path)

        cre = re.compile(test)
        paths = []
        real_names = []
        
        for test_name, allp in self.__memoized_tests.iteritems():
            for p in allp:
                r, b = Rtt.__details(p)
                if cre.match(test_name) and rel == r and build == b:
                    log.debug(".... test matching keyword: %s -  (%s) (%s) (%s) (%s) " %  (test, test_name, build, rel, p) )
                    paths.append('/'.join(p.split('/')[:-1]))
                    real_names.append(test_name)

        return paths, real_names
    
    def available_tests(self, build='*'):
        tog = Rtt.__path(build)
        log.debug('scanning RTT dir: ' + tog )
        dirs = glob.glob(tog)
        log.debug('found '+str(len(dirs))+ ' directories')
        tests = {}
        for path in dirs:
            test_name = Rtt.__test_name(path)
            if test_name != None and test_name != '':
                tests.setdefault(build, set()).add(test_name)
        return tests

    def checks(self):
        return [ CountsCheck.rtt_robust_check,
                 CountsCheck.disabled ]

        
        
        
class TestSystem:
    def __init__(self):
        self.__system = None

    def set(self, system):
        self.__system = system
        
    def __call__(self):
        return self.__system

TestSystem = TestSystem()

            
class Counts:
    def __init__(self, build, test_name, rel):
        """ crates access point to L2 and EF counts given build  (i.e. dev), test name (i.e. AthenaTrigRDO), and release/nighly (rel_1)
        
        """        
        self.__build = build
        self.__rel   = rel
        self.__test_name  = test_name

        self.__dirnames, self.__tests = TestSystem().location(build, test_name, rel)
        if len(self.__dirnames) == 0:
            log.warning("No data for the "+build+" "+test_name+" "+rel)
            raise CountsSummaryException
        
        log.debug('... Counts dirctories seems to be ok: %s' % str(self.__dirnames))

        self.__date          = max([Utils.testdate(d) for d in self.__dirnames])
        

        self.__counts = {'L2': None, 'EF': None}
        self.__events = {'L2': None, 'EF': None}

    def datetime(self):
        return self.__date

    def build(self):
        return self.__build
    
    def rel(self):
        return self.__rel

    def tests(self):
        return  self.__tests

    def reduce_pool_of_tests(self, test_names):
        self.__dirnames,  self.__tests = zip(*[ t for t in zip(self.__dirnames,  self.__tests) if t[1] in test_names ])
        self.__counts = {'L2': None, 'EF': None} # scratch so far summed up counts counts
        self.__events = {'L2': None, 'EF': None} 

    def scale(self, level, factor):

        if self.events(level) == 0.:
            return            
        scale_by = factor/self.events(level)
        log.info("scalling up by:%f", scale_by)        
        for k,v in self.counts(level).iteritems():
            self.__counts[level][k] = int(v*scale_by)

    def events(self, level):
        counts = self.counts(level)
        return self.__events[level]

    def counts(self, level='L2'):
        """
        Gives back dictionary where key is chain name and value is count of passed events.

        Counts are teken from L2Chain.txt or EFChain.txt if they exist. Else stright from the expert-monitoring.root file.
        If none exists ... empty dict is returned.

        It is expensive call for the first time. Then it uses it's own cache.
        """
        if self.__counts[level] != None:
            return self.__counts[level]

        
        self.__counts[level] = {}
        
        rootnames  = [ d+'/expert-monitoring.root' for d in self.__dirnames]
        #print '... rootfiles ', rootnames
        
        for counts_file in rootnames:
            ROOT.gErrorIgnoreLevel =7000;
            r = ROOT.TFile(counts_file, "OLD")
            ROOT.gErrorIgnoreLevel =0;
            hist = r.Get('TrigSteer_'+level+'/ChainAcceptance');
            if hist:
                #print hist
                this_test_counts = [[hist.GetXaxis().GetBinLabel(bin), int(hist.GetBinContent(bin))]  for bin in range(1, 1+hist.GetNbinsX()) ]
                for cnt in this_test_counts:
                    self.__counts[level].setdefault(cnt[0], 0)
                    self.__counts[level][cnt[0]] += cnt[1]
            log.debug("looking for the SignatureAcceptance hist in: %s" % counts_file)            
            hist = r.Get('TrigSteer_'+level+'/SignatureAcceptance_runsummary')
            if hist:
                if self.__events[level] == None:
                    self.__events[level] = 0.                
                self.__events[level] += hist.GetBinContent(1,2) # there we have total number of input events
                    
            r.Close()

        return self.__counts[level]


class Overview:
    def __init__(self, builds, test_name, options):
        whole_week = []
        for build in builds:
            for day in range(7):
                try:
                    c = Counts(build=build, test_name=test_name, rel='rel_%d' % day)
                    whole_week.append(c)
                except CountsSummaryException :
                    log.debug( '.. missing test for: %d test %s build %s' % (day, test_name, build) )
                except:
                    raise
                
        # and assure that the tests intersect
        if options.intersect:
            tests_across_week = [set(d.tests()) for d in whole_week]
            tests_running_every_day  = tests_across_week[0]
            tests_running_any_day    = tests_across_week[0]            

            for t in tests_across_week:
                tests_running_every_day = tests_running_every_day.intersection(t)
                tests_running_any_day.update(t)                

            log.debug('tests running whole week          : '+' '.join(tests_running_every_day))
            log.debug('tests running at least once a week: '+' '.join(tests_running_any_day))
            [ d.reduce_pool_of_tests(list(tests_running_every_day)) for d in whole_week ] 
            
        # eliminate days when test gave no counts        
        self.__week = [d for d in whole_week if d.counts('L2') != {} or d.counts('EF') != {} ]
        del whole_week

        # normalize counts to the best day
        if options.normalize:
            log.info('Will normalize the counts')
            ev_max = max([d.events('L2') for d in self.__week ])
            [d.scale('L2', ev_max)  for d in self.__week ]
            ev_max = max([d.events('EF') for d in self.__week ])
            [d.scale('EF', ev_max)  for d in self.__week ]        

        self.sort(options)


    def chains(self, level):
        ch = list(set(reduce( lambda x,y: x+y, [ d.counts(level).keys() for d in self.__week ], [])))
        #print ch
        return ch
    #return [set( chain.from_iterable([ d.counts(level).keys() for d in self.__week ]))]

    def sort(self, options):
        def __by_date_or_rel( d ):
            return "%05d" % ( ((d.datetime().month*31)+d.datetime().day)*24+d.datetime().hour)
        
        if 'rel' in options.ordering:
            log.info("Will sort the counts by nightly")
            __by_date_or_rel = lambda d: d.rel() 

        __ordering = __by_date_or_rel
        
        if options.ordering == 'build':
            log.info("Will sort the counts by build")
            __ordering = lambda d: d.build()+__by_date_or_rel(d)

        self.__week.sort(key = __ordering )

    def __difference(self, counts):
        """
        Returns list of results for all registered checks which failed.
        """
        return [ c for c in map( lambda x: x(counts), TestSystem().checks() ) ]

    def ascii(self, print_all):
        """
        Print on the terminal.
        
        """
        week = self.__week
            
        print 'counts evolution for: ', opt.builds, opt.test
        print 'overall sum of tests: ', ' '.join(list(set(chain.from_iterable([w.tests() for w in  self.__week]))))
        # figure out all tests which are summed up
        if len(set([len(w.tests()) for w in  self.__week])) != 1:
            log.warning( '. number of summed up tests not equal for each week, consider using option -i' )
        
        print '-'*(40+1+7*len(week))
        format  = '%-40s '+''.join([' %6d']*len(week))
        sformat = '%-40s '+''.join([' %6s']*len(week))
        print sformat  % tuple(["build"] + [d.build().split('.')[-1] for d in week] )
        print format  % tuple(["day of the month"] + [d.datetime().day for d in week] )

        print sformat % tuple(["rel"]+[ d.rel() for d in week ]), '   _checks which failed_'
        try:
            print sformat % tuple(["events L2"]+[ int(d.events('L2')) for d in week ])
            print sformat % tuple(["events EF"]+[ int(d.events('EF')) for d in week ])
        except: pass
 
        def __print_one_level(level):
            print sformat % tuple(['----'+level+'---------'] + ['----']*len(week))
            keys = set( chain.from_iterable([ d.counts(level).keys() for d in week ]))
            for k in sorted(keys): 
                # figure out if there was any change
                # collect counts for this chain for all week
                counts = [ d.counts(level).setdefault(k, -1) for d in week]
                diff_checks = self.__difference(counts)
                diff = map( lambda x: x.failed, diff_checks).count(True)
                
                if print_all or diff:
                    print format % tuple([k]+counts), ' & '.join([ (' '*len(c.text), c.text)[c.failed]  for c in diff_checks ] )

        __print_one_level('L2')
        __print_one_level('EF')

    def dictionarize(self):
        ret = {}

        day   = [d.datetime().day for d in self.__week]        
        today = map(lambda x: [0, 1][Utils.today()==x], [int(d.rel()[-1]) for d in self.__week] )
        build = [d.build() for d in self.__week]
        rel   = [ d.rel() for d in self.__week ]
        l2ev  = [ d.events('L2') for d in self.__week ]
        efev  = [ d.events('EF') for d in self.__week ]
        
        ret['meta'] = zip( day, today, build, rel, l2ev, efev)
        def __dict_counts(level):
            keys = self.chains(level)
            l = {}
            for k in keys:
                counts = [ d.counts(level).setdefault(k, -1) for d in self.__week]
                diff_checks = self.__difference(counts)
                l[k] = (counts, diff_checks)
            return l
        ret['L2'] = __dict_counts('L2')
        ret['EF'] = __dict_counts('EF')
        return ret
    

if __name__ == "__main__":
    import optparse
    parser = optparse.OptionParser('show chain counts for tests')
    
    parser.add_option('-b', '--builds', default='dev',
                      help='builds, coma separated list of  '+' '.join( [ n.split('/')[-1] for n in  glob.glob('/afs/cern.ch/atlas/software/builds/nightlies/*')])  , metavar='BUILD')
    parser.add_option('-d', '--domain', default='*', 
                       help='test suite domain [p1hlt|offline], if not specified test results will be merged')    
    parser.add_option('-t', '--test', default='AthenaTrigRDO',
                      help='test name, look at NICOS test page for test names', metavar='TEST')
    parser.add_option('-a', '--all', default=False,  action='store_true',
                      help='print all chains (not only those changing)')
    parser.add_option('-l', '--list', default=False, action='store_true',
                      help='print all tests in all builds (takes long time to execute, and output is just huge)')    
    parser.add_option('-s', '--sys', default='ATN', 
                      help='Test system, either ATN (default) or RTT')
    parser.add_option('-v', '--verbose', default=0, action='store_true',
                      help='More verbose output')
    parser.add_option('-i', '--intersect', default=0, action='store_true',
                      help='from the set of tests (when wildcard is used) ')
    parser.add_option('-n', '--normalize', default=0, action='store_true',
                      help='normalize (scaling up to the day wiht highest number of processed events) the counts before comparison')
    parser.add_option('-o', '--ordering', default='date', 
                      help='results ordering: date(default), build, rel')    

    (opt,args) = parser.parse_args()
    if opt.verbose:
        log.getLogger().setLevel(log.DEBUG)
        log.debug('... more debug ')
    
    TestSystem.set(Atn())
    if opt.sys == 'RTT':
        TestSystem.set(Rtt(opt.domain))
    
    if opt.list:
        log.debug('... scanning for available tests')
        tests = TestSystem().available_tests(opt.builds)
        for build, tests in tests.iteritems():
            print '-'*80
            print 'Build: ', build        
            print  '\n'.join(sorted(Utils.wildcarding(tests)))
        import sys
        sys.exit(0)
        
    over = Overview(opt.builds.split(','), opt.test, opt)
    over.ascii(opt.all)

    
