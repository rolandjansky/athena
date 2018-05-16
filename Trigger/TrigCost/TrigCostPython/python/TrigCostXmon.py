from ROOT import TFile, TTree, TIter
from array import array
import logging
logging.basicConfig(level=logging.INFO)
log = logging.getLogger('TrigCostXmon.py')

class TriggerCollection(object):
    def __init__(self, name, level = "None"):
        self.chain_name = name
        self.rate_list = []
        self.level = level

class LumiBlockCollection:
    hlt_trigger_master = []
    l1_trigger_master = []
    def __init__(self, in_id, lbbeg = 0, lbend = 999):
            self.lb_beg = lbend
            self.lb_end = lbbeg
            self.block_id = in_id
            self.lumi_iovs = []
            self.l1_iovs = []
            self.hlt_iovs = []

            self.trp__lumi = []
            self.trp__mu = []

            self.trp__TEST_l1 = []
            self.trp__TEST_hlt = []
            self.l1_triggers = []
            self.hlt_triggers = []
            self.ratecollection = {}
            self.ratecollection_HLT = {}

    def BookTrigger(self, name, type = "none"):
        if type == "L1" : self.l1_triggers.append(TriggerCollection(name, level = "L1"))
        if type == "HLT": self.hlt_triggers.append(TriggerCollection(name, level = "HLT"))
        log.info ("Booked new %s trigger [%s]" % (type, name))
    def InLBRange(self, lb):
        if lb <= self.lb_end and lb >= self.lb_beg: return True
        else: return False
    def GetL1List(self):
        if self.l1_triggers == [] : raise RunTimeWarning("You should book triggers before calling for list")
        if hasattr(self, "L1stringList"): return self.L1stringList
        else:
            self.L1stringList = []
            for p in self.l1_triggers: self.L1stringList.append(p.chain_name)
            return (self.L1stringList)
    def GetHLTList(self):
        if self.hlt_triggers == [] : raise RunTimeWarning("You should book triggers before calling for list")
        if hasattr(self, "HLTstringList"): return self.HLTstringList
        else:
            self.HLTstringList = []
            for p in self.hlt_triggers: self.HLTstringList.append(p.chain_name)
            return (self.HLTstringList)
    def InitializeRateCollection(self):
        self.ratecollection = dict([ (p.chain_name, p) for p in self.l1_triggers ])
        self.ratecollection_HLT = dict([ (p.chain_name, p) for p in self.hlt_triggers ])

    def SetTriggerMasterList (self, in_hlt, in_l1) :
        log.warning ("Undefined")
    def AddL1TriggerRate  (self, name, rate) :
        self.ratecollection[name].rate_list.append(rate)
        self.trp__TEST_l1.append(rate)
    def AddHLTTriggerRate (self, name, rate) :
        self.ratecollection_HLT[name].rate_list.append(rate)
        self.trp__TEST_hlt.append(rate)
    def AddLumiInfo(self, lumi, mu, time) :
        self.lumi_iovs.append(time)
        self.trp__lumi.append(lumi)
        self.trp__mu.append(mu)
        #print ("Recorded Lumi data at [time,lumi,mu] = [%d,%f,%f] with a total of %d lumi-points recorded" % (time,lumi,mu, len(self.lumi_iovs)))
    def AddL1TimeInfo(self, time) :
        self.l1_iovs.append(time)
        #print ("Recorded L1 TimeStamp data at [time] = [%d], total of %d l1-points recorded" % (time, len(self.l1_iovs)))
    def AddHLTTimeInfo(self, time) :
        self.hlt_iovs.append(time)
        #print ("Recorded HLT TimeStamp data at [time] = [%d], total of %d hlt-points recorded" % (time, len(self.hlt_iovs)))
    def ProcessLumiBlock(self) :
        # Used to match 3 different timestamped values to eachother in order to write a fully flat n-tuple
        # Number of events is driven by lowest total number
        tmp_counts = [len(self.lumi_iovs),len(self.l1_iovs),len(self.hlt_iovs) ]
        self.master_list = ""
        self.write_out_count = min(tmp_counts)

        log.info ("Found %d total events to write out" % (self.write_out_count))
        if self.write_out_count == len(self.lumi_iovs) :
            log.info ("Using Lumi as List")
            self.master_list = "LUMI"
        if self.write_out_count == len(self.l1_iovs) :
            log.info ("Using LVL1 as List")
            self.master_list = "L1"
        if self.write_out_count == len(self.hlt_iovs) :
            log.info ("Using HLT as List")
            self.master_list = "HLT"
        self.match_success = False
        log.info ("Now seeing if all IOVs are given as sorted...")
        if not sorted(self.lumi_iovs) == self.lumi_iovs:
            log.warning ("Please carefully inspect the output from this...")
            log.warning ("A small switch in IOV will not cause problems, but if everything is out of order, then we'll have serious problems")
            for index,value in enumerate(sorted(self.lumi_iovs)):
                if not value == self.lumi_iovs[index]:
                    log.warning ("Mismatch at i = %d, sorted = %f, unsorted = %f" % (index, value, self.lumi_iovs[index]))
            log.warning("Lumi not given in time-order")
        if not sorted(self.l1_iovs) == self.l1_iovs:
            log.warning ("Please carefully inspect the output from this...")
            log.warning ("A small switch in IOV will not cause problems, but if everything is out of order, then we'll have serious problems")
            for index,value in enumerate(sorted(self.l1_iovs)):
                if not value == self.l1_iovs[index]:
                    log.warning ("Mismatch at i = %d, sorted = %f, unsorted = %f" % (index, value, self.l1_iovs[index]))
            log.warning("Lumi not given in time-order")
        if not sorted(self.hlt_iovs) == self.hlt_iovs:
            log.warning ("Please carefully inspect the output from this...")
            log.warning ("A small switch in IOV will not cause problems, but if everything is out of order, then we'll have serious problems")
            for index,value in enumerate(sorted(self.hlt_iovs)):
                if not value == self.hlt_iovs[index]:
                    log.warning ("Mismatch at i = %d, sorted = %f, unsorted = %f" % (index, value, self.hlt_iovs[index]))
            log.warning("Lumi not given in time-order")
        log.info ("IOV list passed checks!!!")
        if self.master_list == "HLT":
            # Generate 3 arrays
            self.hlt_match_rulebook = []
            self.l1_match_rulebook = []
            self.lumi_match_rulebook = []
            log.info ("Now performing closest TimeStamp matching for master point set...")
            self.write_out_count -= 2
            try:
                for i in range (self.write_out_count) :
                    for j, lumi_time in enumerate(self.lumi_iovs) :
                        if (lumi_time >= self.hlt_iovs[i]) :
                            self.lumi_match_rulebook.append(j)
                            break;
                    for j, l1_time in enumerate(self.l1_iovs) :
                        if (l1_time >= self.hlt_iovs[i]) :
                            self.l1_match_rulebook.append(j)
                            break;
                    for j, hlt_time in enumerate(self.hlt_iovs) :
                        if (hlt_time >= self.hlt_iovs[i]) :
                            self.hlt_match_rulebook.append(j)
                            break;
            except:
                log.error("Unexpected error, aborting lumi-block (putting block in the trash)")
                return
            # Validate
            log.info ("Now validating DB rulebooks...")
            if len(self.hlt_match_rulebook) != len(self.l1_match_rulebook) or len(self.l1_match_rulebook) != len(self.lumi_match_rulebook) :
                log.warning (len(self.lumi_match_rulebook))
                log.warning (len(self.l1_match_rulebook))
                log.warning (len(self.hlt_match_rulebook))
                raise  RuntimeWarning ("I missed an IOV, this will cause matching problems, aboring lumi-block")
                return
            self.match_success = True # If we got here, things are probably OK
            log.info ("Rulebooks are valid, matching was a success!!!")
            #for i, val in enumerate(self.hlt_match_rulebook) :
            #    print ("Match rules set for [master,lumi,l1,hlt] --> [%d,%d,%d,%d]" % (i, self.lumi_match_rulebook[i], self.l1_match_rulebook[i], self.hlt_match_rulebook[i]))
        return

    def WriteOutRules(self, output_path) :
        maxn = self.write_out_count
        f = TFile( output_path, 'RECREATE' )
        t = TTree( 'Rules', 'Rules for TRP DB matching' )
        rules__lumi = array( 'i', maxn*[ 0 ] )
        rules__l1 = array( 'i', maxn*[ 0 ] )
        rules__hlt = array('i', maxn*[ 0 ])
        t.Branch( 'rules__lumi', rules__lumi, 'rules__lumi/I' )
        t.Branch( 'rules__l1', rules__l1, 'rules__l1/I' )
        t.Branch( 'rules__hlt', rules__hlt, 'rules__hlt/I' )
        for i in range (maxn):
            log.debug ("Writing rules set for [master,lumi,l1,hlt] --> [%d,%d,%d,%d]" % (i, self.lumi_match_rulebook[i], self.l1_match_rulebook[i], self.hlt_match_rulebook[i]))
            rules__lumi[0] = self.lumi_match_rulebook[i]
            rules__l1[0] = self.l1_match_rulebook[i]
            rules__hlt[0] = self.hlt_match_rulebook[i]
            t.Fill()
        f.Write()
        f.Close()
        log.info("Wrote rulebook --> %s" % output_path)
        return


    def WriteToNtuple(self) :
        maxn = self.write_out_count
        f = TFile( './test.root', 'recreate' )
        t = TTree( 'ntp', 'Xmon Flattened Rate Info' )

        evt__lumi = array( 'f', maxn*[ 0.0 ] )
        evt__mu = array( 'f', maxn*[ 0.0 ] )
        t.Branch( 'evt__lumi', evt__lumi, 'evt__lumi/F' )
        t.Branch( 'evt__mu', evt__mu, 'evt__mu/F' )

        self.ratecollection.update(self.ratecollection_HLT) # add HLT to full rate collection

        for i in range(self.write_out_count) :
            evt__lumi[i] = self.trp__lumi[self.lumi_match_rulebook[i]]
            evt__mu[i] = self.trp__mu[self.lumi_match_rulebook[i]]
            t.Fill()
        for chainname, ratelist in self.ratecollection.items():
            log.info ("Now writing data from trigger [%s]" % (chainname))
            evt__rate = array( 'f', maxn*[ 0.0 ] )
            t.Branch('evt__'+chainname, evt__rate, 'evt__'+chainname+'/F')
            for i in range(self.write_out_count) :
                if i > 2 : continue
                if ratelist.level == "L1" : evt__rate[i] = ratelist.rate_list[self.l1_match_rulebook[i]]
                if ratelist.level == "HLT" : evt__rate[i] = ratelist.rate_list[self.l1_match_rulebook[i]]
                t.Fill()
        f.Write()
        f.Close()
        return
