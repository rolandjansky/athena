# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import xml.etree.cElementTree as etree

from AthenaCommon.Logging import logging  # loads logger

def logger():
    return logging.getLogger( "TriggerMenu.menu.HLTObjects" )

#########################################################################################
# HLT Chain classes
#
class HLTChain:
    """ the HLT Chain definition class """
    class HLTSignature:
        """ HLT Signature, internal class of HLT Chain """
        def __init__(self, telist, sigcounter, logic='1'):
            self.sigcounter = sigcounter
            if type(telist) != type([]):
                self.tes    = [ telist ]
            else:
                self.tes    = telist
            self.logic      = logic


        def __eq__(self, other):
            if self.tes == other.tes:
                return True
            return False

        def multiplicity(self):
            return len(self.tes)

        def uniqueTEs(self):
            return len(self.tes)

        def xml(self, xlist):
            if len(self.tes) != 0:
                xSignature = etree.SubElement(xlist,'SIGNATURE',
                                              logic='1', signature_counter=str(self.sigcounter))
                for te in self.tes:
                    if type(te) != type(''): # check if this is a string
                        raise Exception("The trigger element: " + str(te) + " in the signature: " + self.sigcounter + "is not a plain string" )
                    #xTriggerElement = etree.SubElement(xSignature, 'TRIGGERELEMENT', te_name=str(te))
                    etree.SubElement(xSignature, 'TRIGGERELEMENT', te_name=str(te))
                    
    # construction
    def __init__(self, chain_name, chain_counter,
                 lower_chain_name, level, prescale='1', pass_through='0', rerun_prescale='-1',stream_tag=None, groups=None, EBstep="-1"):
        self.chain_name       = chain_name
        self.chain_counter  = chain_counter
        self.level          = level
        self.siglist        = []
        self.prescale       = prescale
        self.rerun_prescale = rerun_prescale
        self.pass_through   = pass_through
        self.lower_chain_name = lower_chain_name
        self.stream_tag     = []
        if stream_tag is not None: self.stream_tag += stream_tag
        self.trigger_type_bits = []
        self.groups           = []
        if groups is not None: self.addGroup(groups)
        self.chainBindings = []
        self.chains_to_merge   = []
        self.sigs_n_before_merge   = 0
        self.EBstep = EBstep



        
    def addHLTSignature(self, telist, sigcounter=None, logic='1'):
        if sigcounter is None:       # figure out sigcounter if not specified
            if len(self.siglist) == 0:
                sigcounter = 1
            else:
                sigcounter =  self.siglist[-1].sigcounter+1
        else:
            if  len(self.siglist) != 0:
                if sigcounter >= self.siglist[-1].sigcounter:
                    sigcounter =  self.siglist[-1].sigcounter+1
                    
        self.siglist.append( HLTChain.HLTSignature(telist, sigcounter, logic) )
        return self

    def getHLTSignatureOfStep(self, step):
        """ Returns signature at given step

        If no signature at given step is present then method recursively searches back.
        If step 0 is reached and no signature is found then empty one is returned.
        """
        for sig in self.siglist:
            if sig.sigcounter == step:
                return sig
        if step == 0:
            return HLTChain.HLTSignature([], step)
        else:
            return self.getHLTSignatureOfStep(step -1)

    def mergeAndAppendChains_Order(self, chains_list):
        """ Helps definig complex chains made out of other chains.

        All chains from the chains_list are scanned and thier signatures are merged and appended to the list of existing signatures.
        Note, this is appending one chain to another, e.g. runs first all xe and then egamma, not in parallel
        """
        self.chains_to_merge = chains_list
        self.sigs_n_before_merge = len(self.siglist)
        self.doChainsMerging_Order()

    def doChainsMerging_Order(self):
        if type(self.chains_to_merge) != type([]):
            logger().error( "Not a chains list given to mergeAndAppendChains for chain: "+ self.chain_name+ " it is: "+ str(type(self.chains_to_merge)) )
            raise Exception("Incorrect usage of: mergeAndAppendChains_Order")

        logger().debug("merging chains seq:"+str([ c.chain_name for c in self.chains_to_merge]))

        copysigs = self.siglist
        self.siglist = copysigs[0:self.sigs_n_before_merge]

        # find out counter of last signature
        last_counter = 1
        if len(self.siglist) != 0:
            last_counter = self.siglist[-1].sigcounter
            logger().debug( "The chain: " + self.chain_name + " with counter " + last_counter + " already contains signatures. Will align sinatures to it." )


        total_counter = 0
        for chain in self.chains_to_merge:
            if len(chain.siglist) != 0:
                total_counter += chain.siglist[-1].sigcounter
                logger().debug("Total counter in merged chains "+str(total_counter))
            else:
                logger().debug("Chain: "+ chain.chain_name + " is having empty signature list wil lbe ignored in merging")
                del self.chains_to_merge[self.chains_to_merge.index(chain)]

        current_counter = 0        
        for chain in self.chains_to_merge:
            if len(chain.siglist) != 0:
                for te in [chain.getHLTSignatureOfStep(step).tes for step in range(1,chain.siglist[-1].sigcounter+1)]:
                    current_counter += 1
                    self.addHLTSignature(te, sigcounter=current_counter)                    
                    
        # now append all signatures which were added after merging
        for sig in copysigs[self.sigs_n_before_merge:]:
            current_counter = current_counter+1
            self.addHLTSignature(sig.tes, sigcounter=current_counter)

    def mergeAndAppendChains(self, chains_list):
        """ Helps definig complex chains made out of other chains.

        All chains from the chains_list are scanned and thier signatures are merged and appended to the list of existing signatures.
        """
        self.chains_to_merge = chains_list
        self.sigs_n_before_merge = len(self.siglist)
        self.doChainsMerging() # TB this needs to be cut short as we do noeed alignements anymore (TODO remove alignChain(s) etc.)

    def doChainsMerging(self):
        if type(self.chains_to_merge) != type([]):
            logger().error( "Not a chains list given to mergeAndAppendChains for chain: "+ self.chain_name+ " it is: "+ str(type(self.chains_to_merge)) )
            raise Exception("Incorrect usage of: mergeAndAppendChains")

        logger().debug("merging chains:"+str([ c.chain_name for c in self.chains_to_merge]))

        copysigs = self.siglist
        self.siglist = copysigs[0:self.sigs_n_before_merge]

        # find out counter of last signature
        last_counter = 1
        if len(self.siglist) != 0:
            last_counter = self.siglist[-1].sigcounter
            logger().debug( "The chain: " + self.chain_name + " with counter " + last_counter + "already contains signatures. Will align sinatures to it." )


        max_counter = 0;
        for chain in self.chains_to_merge:
            if len(chain.siglist) != 0:
                max_counter = max(chain.siglist[-1].sigcounter, max_counter)
                logger().debug("Max counter in merged chains "+str(max_counter))
            else:
                logger().debug("Chain: "+ chain.chain_name + " is having empty signature list wil lbe ignored in merging")
                del self.chains_to_merge[self.chains_to_merge.index(chain)]


        # make dictionary with TE sigantures
        for i in range(1, max_counter+1):
            telist = []
            [ telist.extend(x) for x in [chain.getHLTSignatureOfStep(i).tes for chain in self.chains_to_merge]] # repack TEs from other chain signatures
            self.addHLTSignature(telist, sigcounter=i)

        # now append all signatures which were added after merging
        for sig in copysigs[self.sigs_n_before_merge:]:
            max_counter = max_counter+1
            self.addHLTSignature(sig.tes, sigcounter=max_counter)

    def continueOn(self, chain):
        self.siglist = chain.siglist
        return self

    def evaluateSignatureAfter(self, mySig, otherChain, otherChainSig):
        """ Adds chain binding. Note that at this point action is only scheduled. Real work is done be alignChain method """
        if type(otherChain) == type(''):
            self.chainBindings.append( (mySig, otherChain, otherChainSig) )
        else:
            self.chainBindings.append( (mySig, otherChain.chain_name, otherChainSig) )
#        self.alignChain(mySig, otherChain, otherChainSig)

    def alignChain(self, mySig, otherChain, otherChainSigName ):
        """ Changes internals of this chains and makes sure that selected signatures of other chain are evaluated first """
        # check if any binding is required
        if len(self.chainBindings) == 0:
            return False

        # find this chain signature
        thisChainSig = filter(lambda x: x.tes == HLTChain.HLTSignature(mySig, 0).tes, self.siglist)
        if len(thisChainSig) == 0:
            logger().debug( "Chains binding ineffective (1), can't find signature: " + mySig + " in chain " + self.chain_name )
            return False
        thisChainSig = thisChainSig[0] # unpack it from list

        # find sigcounter of the otherChainSig
        otherChainSig = filter(lambda x: x.tes == HLTChain.HLTSignature(otherChainSigName, 0).tes, otherChain.siglist)
        if len(otherChainSig) == 0:
            logger().debug( "Chains binding ineffective (2), can't find signature: " + str(otherChainSigName) + " in chain " + otherChain.chain_name )
            return False

        otherChainSig = otherChainSig[0]

        # fianlly we have both, check if we need to do anything
        if thisChainSig.sigcounter > otherChainSig.sigcounter:
            logger().debug( "Chains binding ineffective (3), signatures steps already ordered" )
            return False

        # and real work to be done here
        counterShift = otherChainSig.sigcounter - thisChainSig.sigcounter + 1
        for i in range (self.siglist.index(thisChainSig), len(self.siglist)):
            self.siglist[i].sigcounter = self.siglist[i].sigcounter + counterShift
            logger().debug( "Chains binding effective: signature: " + str(self.siglist[i].tes) + " will be shifted to step: "+ str(self.siglist[i].sigcounter) )
        # true means that there was something to do
        return True

    def getOutputTEs(self):
        tes = []
        for sig in self.siglist:
            tes.extend(sig.tes)
        return tes

    def isL2(self):
        if self.level == "L2":
            return True
        else:
            return False

    def isEF(self):
        if self.level == "EF":
            return True
        else:
            return False

    def isHLT(self):
        if self.level == "HLT":
            return True
        else:
            return False

    def addTriggerTypeBit(self, bit):
        self.trigger_type_bits.append(bit)

    def addStreamTag(self, stream, type="physics", obeyLB="yes", prescale='1'):
        self.stream_tag.append( (stream, type, obeyLB, prescale) )

    def addGroup(self, name ):
        if type(name) == type(''):
            self.groups += [name]
        if type(name) == type([]):
            self.groups += name

    def xml(self, xChainList):
        """ Returns XML representation of this chain """
        xChain = etree.SubElement(xChainList, 'CHAIN',                                
                                  chain_name = self.chain_name,
                                  chain_counter = str(self.chain_counter),
                                  lower_chain_name = self.lower_chain_name,
                                  level = str(self.level),
                                  prescale = str(self.prescale),
                                  pass_through = str(self.pass_through),
                                  rerun_prescale = str(self.rerun_prescale),
                                  EBstep = str(self.EBstep))


        xTriggerTypeList = etree.SubElement(xChain, 'TRIGGERTYPE_LIST')
        for bit in self.trigger_type_bits:
            #xTriggerType = etree.SubElement(xTriggerTypeList, 'TRIGGERTYPE', bit = str(bit))
            etree.SubElement(xTriggerTypeList, 'TRIGGERTYPE', bit = str(bit))

        xStreamTagList = etree.SubElement(xChain, 'STREAMTAG_LIST')
        for stream in self.stream_tag:
            #xStreamTag = etree.SubElement(xStreamTagList, 'STREAMTAG',
            etree.SubElement(xStreamTagList, 'STREAMTAG',
                                          stream = stream[0],
                                          type = stream[1],
                                          obeyLB = stream[2],
                                          prescale = str(stream[3]))
            
#         ## remove the CPS group from the EF chain    
#         if self.chain_name.startwith("EF_"):
#             for g in self.groups:
#                 if "CPS" in g:
#                     self.groups.remove(g)

        xGroupList = etree.SubElement(xChain, 'GROUP_LIST')
        for g in self.groups:
            #xGroup = etree.SubElement(xGroupList, 'GROUP', name = g)
            etree.SubElement(xGroupList, 'GROUP', name = g)
            
        xSignatureList = etree.SubElement(xChain, 'SIGNATURE_LIST')
        for sig in self.siglist:
            sig.xml(xSignatureList)


#
# EF Chain classes
#########################################################################################


#########################################################################################
# HLT Sequence
#
class HLTSequence:
    def __init__(self, inputTEs, algos, outputTE, topo_start_from=None):
        """HLT Steering Sequence class constructor
        inputTEs argument is a one or list of names TEs for the sequence,
        if empty string is passed then sequence is considered as UNSEEDED
        algos is one or list of algo instances
        """

        if type(inputTEs) == type(""):
            self.input  = [ inputTEs ]
        else:
            self.input  = inputTEs

        # make algos always to be alist of ... something ...
        if type(algos) != type([]):
            algos = [ algos ]
        from AthenaCommon.Configurable import ConfigurableAlgorithm
        self.algs = []
        self.algInstances = [] # needed such that configurables don't go out of scope
        for alg in algos:
            if type(alg) == type(""): # plain string
                self.algs.append( alg );
                logger().warning("Algorithm named %s has no instance. It will not appear in the list of configurables and thus can not be run." % (alg))
            elif issubclass(type(alg), ConfigurableAlgorithm): # configurable given to us
                self.algs.append(alg.getFullName())
                self.algInstances.append(alg)

        self.output = outputTE
        self.topo_start_from = topo_start_from

        self.__used = None

    def used(self):
        return self.__used

    def use(self, level):
        if level == 'L2' or level == 'EF' or level == 'HLT':
            self.__used = level
        else:
            raise Exception('setting for the sequence level to incorrect value: '+str(level))

    def __str__(self):
        return str(self.input)+" -> "+ str(self.algs)+" -> "+self.output

    def xml(self, xlist):
        """Generates piece of XML used to configure HLT Steering"""
        if not self.used():
            return
        
        xSequence = etree.SubElement(xlist, 'SEQUENCE',
                                     input = reduce(lambda x,y: x+' '+y, self.input),
                                     output = self.output,
                                     algorithm = reduce(lambda x,y: x+' '+y, self.algs))
                                     
        if self.topo_start_from!= None:
            xSequence.set('topo_start_from', self.topo_start_from)

    def dot(self, algs=True):
        if not self.used():
            return ''
        #example EM10_1 -> em10t2calo_1  [label="T2Calo,\n T2Hypo10"]        
        line = ''
        for i in self.input:
            if i == '':
                line += '"UNSEEDED"  [style=filled, fillcolor=blue]\n'
                line += 'UNSEEDED'
            else:
                line += '"'+i+'"'
            line += ' -> ' + '"' + self.output + '"'
            if algs:
                line += ' [label="' + reduce(lambda x,y: x+'\\n '+y, self.algs) + '"]'
            line += '\n'
        return line
    
#
# EOF Sequences
#########################################################################################
