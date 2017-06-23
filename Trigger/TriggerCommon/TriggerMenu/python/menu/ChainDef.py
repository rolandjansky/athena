# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""ChainDef is a simple object with state but no behaviour.
It serves to vehical information from trigger slice menu software,
which is often maintained by a slice domain expertto the central
trigger menu code."""

import pprint
pp = pprint.PrettyPrinter(indent=4, depth=8)

class ChainDef:
 
    def __init__(self, chain_name, level, lower_chain_name, chain_counter=-1):
        """chain_name: string. Identifies the chain within
        the slice.

        level: string. can take the value L2 or EF or HLT

        lower_chain_name: string. the name of the chain that will be run
        immediately prior to the curent chain. Can be the empty string.

        For a given slice, the combination of level and chain_name is
        unique.
        
        signature list: a list of 2-entry dicionaries. Each dictionary
        gives a signature counter which is used by the central trigger
        for optimization. The 'normal' values an integer index which
        is incremented each time a signature is added. The second
        entry is the name of an output trigger element at which the
        chain is tested for success or failure. If the chain fails, its
        actions are aborted.

        sequence list. a 3-element dictionary containing the names of the
        input and output trigger elements of the sequence, and a list
        of Algorithms (AthenaConfigurables) which are to be run. A chain
        may have more than one sequence.

        """
    
        self.chain_name = chain_name
        self.level = level
        self.lower_chain_name = lower_chain_name
        self.chain_counter = chain_counter
        self.signatureList = []
        self.sequenceList = []
 
    def addSignature(self, signature_counter, listOfTriggerElements):
        """add signature data to the chain"""

        self.signatureList.append(
            {'signature_counter': signature_counter,
             'listOfTriggerElements': listOfTriggerElements})


    def insertSignature(self, signature_counter, listOfTriggerElements):
        """insert signature data to the chain"""

        #safety: if asked to insert with sig_counter -1, use addSignature
        if signature_counter == -1:
            self.addSignature(signature_counter,listOfTriggerElements)
        else:
            pos_newSeq = -1
            
            for pos, sig in enumerate(self.signatureList):
                if sig['signature_counter'] == signature_counter:
                    pos_newSeq = pos
                    
            self.signatureList.insert(pos_newSeq,
                                      {'signature_counter': signature_counter,
                                       'listOfTriggerElements': listOfTriggerElements})
            
            for pos, sig in enumerate(self.signatureList):
                if pos <= pos_newSeq: continue
                sig['signature_counter'] = sig['signature_counter']+1
                

    def addSignatureL2(self, listOfTriggerElements):
        """add new signature after L2 signatures"""

        maxL2SignatureIndex = -1
        for signatureIndex,signature in enumerate(self.signatureList):
            if signature['listOfTriggerElements'][0][0:2] == "L2":
                maxL2SignatureIndex = max(maxL2SignatureIndex,signatureIndex)
        
        # insert new signature after last L2 signature
        self.signatureList.insert(maxL2SignatureIndex+1,{'signature_counter': -1, 'listOfTriggerElements': listOfTriggerElements})

        # fix counters
        incrementSignatureCounter = False
        for signatureIndex,signature in enumerate(self.signatureList):
            if signature['signature_counter']==-1:
                incrementSignatureCounter = True
                self.signatureList[signatureIndex]['signature_counter']=self.signatureList[signatureIndex-1]['signature_counter']+1
            elif incrementSignatureCounter:
                self.signatureList[signatureIndex]['signature_counter']+=1

    def appendSignature(self, listOfTriggerElements):
        """add new signature to the end of the signature list and increment chain counter automatically"""

        for s in self.signatureList:
            if( s["listOfTriggerElements"] == listOfTriggerElements ):
                return

        self.signatureList.append(
            {'signature_counter': self.signatureList[-1]["signature_counter"]+1,
             'listOfTriggerElements': listOfTriggerElements})

 
    def addSequence(self, listOfAlgorithmInstances, te_in, te_out, topo_start_from=None):
        """add sequence data to the chain"""

        self.sequenceList.append(
            {'algorithm': listOfAlgorithmInstances,
             'input': te_in,
             'output': te_out,
             'topo_start_from': topo_start_from})


    def __str__(self):
        """Provide a string representation of the chain"""

        s = ['chain_name:       %s ' % self.chain_name,
             'lower chain name: %s ' % self.lower_chain_name,
             'no signatures     %d ' % len(self.signatureList),
             'no sequences      %d ' % len(self.sequenceList),
             '\n',
             'sequences:']

        s.extend([pp.pformat(ss) for ss in self.sequenceList])
        s.extend(['\n',
                  'signatures:'])
        s.extend([pp.pformat(ss) for ss in self.signatureList])

        return '\n'.join(s)


class ErrorChainDef(object):
    """Error object. Created when ChainDef creation fails."""

    def __init__(self, err_msg, chain_name='unknown'):
        self.chain_name = chain_name
        self.err_msg = err_msg

    def __str__(self):
        return '\n'.join(['ErrorChainDef:',
                          'chain name: ' + self.chain_name,
                          'err msg: ' + self.err_msg])

    def str_no_cr(self):
        return str(self).replace('\n', '; ')
