# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################
# User interface for the configuration, nothing beneath this class need to be studied
#
import re
import string
from collections import defaultdict
import xml.dom.minidom as minidom
import xml.etree.cElementTree as etree

from TriggerMenu.menu.HLTObjects import HLTSequence

from AthenaCommon.Logging        import logging
log = logging.getLogger( 'TriggerPythonConfig' )

class TriggerPythonConfig:
    sCurrentTriggerConfig = None
    def currentTriggerConfig():
        return TriggerPythonConfig.sCurrentTriggerConfig
    currentTriggerConfig = staticmethod(currentTriggerConfig)

    def __init__(self, hltfile=None, signaturesOverwritten=False):
        self.menuName = 'TestMenu'
        self.__HLTFile = hltfile
        self.allThresholds     = {}
        self.allItems          = {}
        self.allChains         = {}

        self.theHLTChains      = []
        self.theSeqLists       = []
        self.theSeqDict        = {} # dict by Seq output TE
        self.theTopoStartFrom    = []

        # Sanity check since the second argument in __init__ used to be the l1 xml file name
        if type(signaturesOverwritten)!=bool:
            log.error('Wrong type for signaturesOverwritten. Received %s but expected bool', signaturesOverwritten)

        self.signaturesOverwritten = signaturesOverwritten

        TriggerPythonConfig.sCurrentTriggerConfig = self



    def printIt(self):
        log.debug("self.theSeqLists: %s",  self.theSeqLists)
        log.debug("self.theSeqDict: %s", self.theSeqDict) 
        log.debug("self.theTopoStartFrom: %s", self.theTopoStartFrom) 
        


    # Work out smk and psk names to put inside xml files, for TT to use on upload

    # Please don't modify this function without consulting with the trigger configuration group
    def get_smk_psk_Name(self, menuName):
        smk_psk_Name = {}
        if "mc_prescale" in menuName:
            form = "(.*)_(.*)_mc_prescale"
            m = re.match(form, menuName)
            (smkName, pskName) = m.groups()
            pskName = pskName+"_mc"       
        elif "prescale" in menuName:
            #eg lumi1e31_simpleL1Calib_no_prescale
            form = "(.*)_(.*)_prescale"
            m = re.match(form,menuName)
            (smkName, pskName) = m.groups()
        else:
            #eg lumi1e31 ps set name can be "default"
            smkName = menuName
            pskName = "default"

        smk_psk_Name["smkName"] = str(smkName)
        smk_psk_Name["pskName"] = str(smkName+"_"+pskName+"_prescale")
        
        return smk_psk_Name

    # Please don't modify this function without consulting with the trigger configuration group
    @classmethod
    def getMenuNameFromHltpsName(cls,hltpsName):
        m = re.match( "(.*)_default_prescale", hltpsName)
        if m: return m.group(1)
        else: return hltpsName


    def addHLTChain(self, chain):
        """ Adds HLT Trigger chain to the menu """
        if chain.isHLT():
            self.theHLTChains.append(chain)                        
        else:
            raise Exception( 'ERROR: Chain: %s has incorrectly defined level' % chain.chain_name)


    def checkChainUniqueness(self, teRenaming):
        """Check for chains defined multiple times but have differently"""
        def renameTE(te, teRemap):
            """returns teRemap[te] if existent, otherwise te itself"""
            if te in teRemap:
                return teRemap[te]
            return te
        chain_TEsigs = {} # [chain][TElist] -> list of signatures
        for sig, chains in self.allChains.iteritems():
            keys = chain_TEsigs.keys()
            for c in chains:
                #tes = string.join(c.getOutputTEs(), ',')
                tes  = string.join([ renameTE(te, teRenaming) for te in c.getOutputTEs() ], ',')                
                if c.chain_name in keys: # chain already defined before
                    TEsigs = chain_TEsigs[c.chain_name]
                    log.debug('Uniqueness check: TEs in sig=%s: %s', sig, tes)
                    log.debug(' TE list already found TEs: %s', TEsigs.keys())
                    if tes in TEsigs.keys():
                        TEsigs[tes].append(sig)
                    else:
                        TEsigs[tes] = [sig]
                else: # chain found for the first time
                    chain_TEsigs[c.chain_name] = { tes: [sig] }
        for c, TEsigs in chain_TEsigs.iteritems():
            if len(TEsigs) <= 1: continue
            n = sum(map(lambda x: len(x[1]), TEsigs.iteritems() ) )
            m = len(TEsigs)
            log.error('Chain %s defined %d times with %d variants' % (c, n, m))
            i = 0
            for TE, sigs in TEsigs.iteritems():
                log.warning('  [%d] TElist=%s : %s' % (i, TE, sigs))
                i += 1
        del(chain_TEsigs)
        pass

    
    def registerHLTChain(self, logical_name, chain):
        """ Adds chain to the set of chains which are registered for further use """
        if logical_name in self.allChains:
            self.allChains[logical_name].append( chain )
        else:
            self.allChains[logical_name] = [ chain ]
        
    def getHLTChain(self, name):
        """ Gets chain by name. None returned inf nothing found"""
        found=[]
        for  cl in self.allChains.values():
            for chain in cl:
                if chain.chain_name == name:
                    found.append(chain)
        if len(found)>1:
            idx=0
            for i in range(1,len(found)):
                if len(found[idx].siglist)<len(found[i].siglist):
                    idx=i
            return found[idx]
        if found:
            return found[0]
        return None

    def addSequence(self,  inTE, algolist, outTE, topo_start_from=None):
        """ Adds HLT sequences to the menu """

        if outTE in self.theSeqDict:
            log.error("There exists already sequence providing TE: "+outTE)
            log.error("Which is: "+str(self.theSeqDict[outTE]))
            log.error('Tried to produce it from '+inTE+' ('+str([a.name() for a in algolist]))
            
        seq = HLTSequence(inTE, algolist, outTE, topo_start_from)
        self.theSeqLists.append(seq)
        self.theSeqDict[outTE] = seq
        return outTE

    def addHLTSequence(self, theHLTSequence):
        """ Adds HLT sequences to TPC """
        outTE = theHLTSequence.output
        
        if outTE in self.theSeqDict:
            if not (str(self.theSeqDict[outTE]) == str(theHLTSequence)):
                log.error("Tried to add sequence %s but there exists already a DIFFERENT sequence with same TEout: %s " % (str(self.theSeqDict[outTE]),str(theHLTSequence)))
            else:
                log.debug("Tried to add sequence %s but there exists already the following sequence: %s ", self.theSeqDict[outTE], theHLTSequence)
            
        self.theSeqLists.append(theHLTSequence)
        self.theSeqDict[outTE] = theHLTSequence


    def seqDict(self):
        return self.theSeqDict
    def allSequences(self):
        return self.theSeqLists
    def allHLTChains(self):
        return self.theHLTChains

    def checkMenuConsistency(self):
        """ Checks menu consistency


        Basic checks are done like chain continuity etc.
        Some of the checks are considered as a serious and therefore Exception is thrown.
        Other are just messaged as a warnings.
        """
        
        return self.checkChains() and self.checkSequences()


    def checkChainCounters(self, theChains):
        correct=True
        counters = map(lambda x: x.chain_counter, theChains) 
        for counter in counters: 
            if (int(counter) > 8191):
                log.error("IN CHAIN: Chain counter " +str(counter) + " is too high (13 bist = 8192 allowed for serialisation)")
                correct = False

        repcounters = filter(lambda x: counters.count(x) > 1, counters)
        for counter in repcounters:
            log.error("IN CHAIN: Chain counter "+ str(counter) + " used " + str(counters.count(counter)) + " times while can only once, will print them all")
            log.error( str(map( lambda x: x.chain_name,  filter(lambda x: x.chain_counter == counter, theChains))) )
            correct=False
        return correct



    def checkChains(self):
        log.info("TriggerPythonConfig: checkChains...")
        correct=True
        # repeating definitions
        ids = map(lambda x: x.chain_name, self.theHLTChains)      
        repid = filter(lambda x: ids.count(x) > 1, ids)
        for id in repid: 
            log.error("Chain of chain_name "+ str(id) +" used " + str(ids.count(id))+ " times, while can only once") 
            correct=False                     
        
        if not self.checkChainCounters(self.theHLTChains):
            # Print list of available counters if check failed
            all_counters = [c.chain_counter for c in self.theHLTChains]

            log.info("Number of used counters: %s", len(all_counters))
            if log.isEnabledFor(logging.DEBUG):
                log.debug("Used HLT counter: %s", ",".join(map(str,all_counters)))
                
            free_counters = set(range(0,1000))-set(all_counters)
            log.info("Available HLT counter: %s", ",".join(map(str,free_counters)))

            correct=False

        # sufficiently defined ?
        for chain in self.theHLTChains:
            if len(chain.trigger_type_bits) == 0:
                log.debug("TRIGGERTYPE bit undefined for chain: %s will use chain_counter for it: %s",
                          chain.chain_name, chain.chain_counter)

                from string import atoi
                chain.trigger_type_bits = [atoi(chain.chain_counter)]
                
            if len(chain.stream_tag) == 0:                
                log.error("STREAMTAG undefined for chain: %s", chain.chain_name)
                correct=False
                    
        # are TEs mentioned in sequences
        tesInSeq = set([seq.output for seq in self.theSeqLists])

        for chain in self.theHLTChains:
            for te in chain.getOutputTEs():
                if te not in tesInSeq:
                    log.error('TE %s used in the chain %s but not found in the sequences definitions', te, chain.chain_name )
                    log.debug('Available are: ', tesInSeq)
                    correct=False
        
        return correct

    def checkSequences(self):
        """ Check sequences consistency """
        correct=True
        return correct


    def resolveDuplicateTEs(self):
        def renameTE(te, teRemap):
            """returns teRemap[te] if existent, otherwise te itself"""
            if (type(te)==list): te = te[0]
            if te in teRemap:
                return teRemap[te]
            return te
        def renameKey(key, teRemap):
            # print 'key=',key
            keycount = key.count("#")
            if (keycount == 2):
                tes, algos, tsf = key.split('#')
            else:
                tes, algos = key.split('#')

            tes = tes.split(',')
            key1 = ''
            for te in tes:
                key1 += '%s,' % renameTE(te, teRemap)
            if len(key1)>0: key1 = key1[:-1]
            if keycount == 1:
                return '%s#%s' % (key1, algos)
            elif keycount ==2:
                return '%s#%s#%s' % (key1, algos,tsf)

        def chooseName(v):
            """pick the name of the output TE from a list of output TE's (which have the same definition)
            v: list of TE names of length >= 2
            returned is the shortest TE name, or in case of ambiguity the first in the alphabet
            """
            goodname = v[0]
            for x in v[1:]:
                if len(x)<len(goodname) or ( len(x)==len(goodname) and x < goodname ): goodname = x
            return goodname
        def createKey(seq):
            """turns a sequence into a 'key' for use in a dictionary
            concatenates input TEs and algs
            """
            inTEs = ','.join([str(te) for te in seq.input])
            algs  = ','.join([str(a) for a in seq.algs])
            if (seq.topo_start_from):
                tsf = ','.join([str(a) for a in seq.topo_start_from])
                return '%s#%s#%s' % (inTEs, algs, tsf)
            else:
                return '%s#%s' % (inTEs, algs)

        seqs = self.allSequences()
        seq_list1 = set() # updated set of sequences after renaming TEs
        seq_list2 = set() # temporary set of sequences
        seq_to_outTEs = defaultdict(list)
        for seq in seqs:
            key = createKey(seq)
            outTE = seq.output
            seq_to_outTEs[key].append(outTE)
            seq_list1.add( (key, outTE) )
        
        # Update the renaming rules
        teRenaming = {}
        iterationCount = 0

        #---------------------------------------------------------------------
        # Change outTE names which use the same inputTEs and algorithms
        # Once the outTEs are changed, modify the inputTEs which use them
        # Do it recursively, until there's no more changes needed
        #---------------------------------------------------------------------
        while True:
            # First update the renaming rules based on the previous iteration
            for outTEs in seq_to_outTEs.itervalues():
                if len(outTEs) <= 1: continue                                       
                name = chooseName(outTEs)
                for x in outTEs:
                    teRenaming[x] = name

            # Changing the mapping defined earlier
            # A->B(old), B->C(new) should become A->C
            for a, b in teRenaming.iteritems():
                if b in teRenaming:
                    teRenaming[a] = teRenaming[b]
            
            # Update sequences (remove duplicates)
            n1 = len(seq_list1)
            seq_list2 = set()
            seq_to_outTEs = defaultdict(list)
            for key,outTE in seq_list1:
                key = renameKey(key, teRenaming)
                # This is actually slightly faster than dict.get(outTE,outTE)
                outTE2 = outTE if not outTE in teRenaming else teRenaming[outTE]

                seq_to_outTEs[key].append(outTE2)
                seq_list2.add( (key, outTE2) )
        
            seq_list1 = seq_list2
            iterationCount += 1
            if log.isEnabledFor(logging.DEBUG):
                log.debug('N TE remapping after iteration %d: %d', iterationCount, len(teRenaming))
                
            if len(seq_list1) == n1: break

        # teRenaming still contains mappings x->x now, removing it
        teRenaming = dict( [(o,t) for o,t in teRenaming.items() if o!=t] )

        #--------------------------------------------
        # Apply the TE renaming to the real sequences
        #--------------------------------------------
        for seq in seqs:
            seq.input  = [ renameTE(inTE, teRenaming) for inTE in seq.input ]
            seq.output = renameTE(seq.output, teRenaming)
            
        #--------------------------------------------
        # Apply the TE renaming to the real chains
        #--------------------------------------------
        for chain in self.allHLTChains():
            for sig in chain.siglist:
                oldtes = sig.tes
                sig.tes  = [ renameTE(te, teRenaming) for te in sig.tes ]
                if oldtes != sig.tes:
                    log.debug('Remapping applied to chain %s and TEs %r to TEs %r', chain.chain_name, oldtes, sig.tes)

        if log.isEnabledFor(logging.DEBUG):
            for k in sorted(teRenaming.keys()):
                log.debug('  %-40s -> %s', k, teRenaming[k])
        
        return teRenaming
            

    def findSequencesRecursively(self, seqDict, te):
        """ finds recursively sequences
        
        seqDict is dictionary {sequence outout te: sequence obj}
        """
        seq = []
        if not seqDict.has_key(te):
            return seq
        this = seqDict[te]
        seq += [this]
        for i in this.input:
            if i != '':
                seq += self.findSequencesRecursively(seqDict, i)		
        return seq	

    def getChainTEs(self, seqDict, chain, alsorecursive=False):
        """ gets TEs used by chain (also recursively searching sequences)

        seqDict is dictionary {sequence outout te: sequence obj}
        """
        neededtes = reduce(lambda x,y: x+y, [s.tes for s in chain.siglist], [])
        if alsorecursive == True:
            seqs = []
            for te in neededtes:
                seqs += self.findSequencesRecursively(seqDict, te)
                for s in seqs:
                    # if s.used is set then it needs 
                    if (s.used() and s.used() == chain.level) or not s.used():
                        if s.output not in neededtes:
                            neededtes.append(s.output)    
        return neededtes


    def markUsedSequences(self):
        seqDict = {}
        seqDict.update([ (s.output, s) for s in self.theSeqLists])

        # mark sequences as the one which will be run at HLT
        for c in self.theHLTChains:
            tes = reduce(lambda x,y: x+y, [s.tes for s in c.siglist], [])
            for te in tes:
                seqs = self.findSequencesRecursively(seqDict, te)
                for seq in seqs:
                    if seq.used() == 'HLT':
                        continue
                    else:
                        seq.use('HLT')

    def getHLTConfigFile(self):
        return self.__HLTFile

    def writeHLTConfigFile(self,smk_psk_Name):
        """ Writes HLT config file"""
        if self.__HLTFile is None:
            return

        # HLT XML
        xHLTMenu = etree.Element("HLT_MENU",
                                 menu_name = smk_psk_Name["smkName"],
                                 prescale_set_name = smk_psk_Name["pskName"])

        # comments
        xHLTMenu.append(etree.Comment('File is auto-generated by python config class, may be overwritten'))
        xHLTMenu.append(etree.Comment('No. of HLT chains:' + str(len(self.theHLTChains))))

        self.markUsedSequences()
        
        # make xml sequences
        xSequenceList = etree.SubElement(xHLTMenu,'SEQUENCE_LIST')
        for s in self.theSeqLists:
            s.xml( xSequenceList )

        # make xml chains
        xChainList = etree.SubElement(xHLTMenu,'CHAIN_LIST')                        
        for c in self.theHLTChains:
            c.xml( xChainList )            

        # write out the xml as a file
        file = open( self.__HLTFile, mode="wt" )
        # etree does not support pretty printing, use minidom for this
        file.write(minidom.parseString(etree.tostring(xHLTMenu)).toprettyxml('  '))
        file.close()
        log.info("Wrote %s",self.__HLTFile)

                    
    def writeConfigFiles(self,level="all"):
        """ Writes all config files, and checks consistency"""

        teRenaming = self.resolveDuplicateTEs()
        self.checkChainUniqueness(teRenaming)
        
        #self.checkStreamConsistency()
        
        # disable python tracing
        from AthenaCommon.Include import excludeTracePattern
        excludeTracePattern.append("*/dom/*")

        smk_psk_Name = self.get_smk_psk_Name(self.menuName)

        # HLT XML
        if level=="all" or level=="hlt":
            self.writeHLTConfigFile(smk_psk_Name)

        # L1 XML no longer supported (now done by Lvl1Menu.py)
        if level=="l1":
            log.error("Lvl1 menu writing no longer supported here")

        if self.checkMenuConsistency() is False:
            raise Exception( "Trigger menu inconsistent, aborting...." )


    def dot(self, algs=True):
        file = open( self.__HLTFile[0:-4]+'_sequenceses.dot', mode="wt" )
        file.write( 'digraph Menu  {\n'\
                    +'\n'\
                    +'node [ shape=polygon, fontname=Helvetica ]\n'\
                    +'edge [ fontname=Helvetica ]\n' )
        for s in self.theSeqLists:
            file.write( s.dot(algs) )
        file.write( '}')
        file.close()

        





