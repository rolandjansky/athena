# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from collections import defaultdict
from .Sequence import Sequence
import binascii

# def _update_if_diagnostic_sequence(sequence):
#     # diagnostic sequences monitor sequences. Ensure diagnostic
#     # Algorithm names are labled by the sequence they are monitoring.
#     # The 'chain_name attribute' - a misnomer, it is realy part
#     # of the histogram file name is also set here.
#     # This is the earliest this information is known
# 
#     if 'jetfex_diagnostics' not in sequence.alias:
#         return
#         
#     for a in sequence.alg_list:
# 
#         def select(k):
#             if k.startswith('name'):
#                 return True
#             if k.startswith('chain_name'):
#                 return True
#             return False
#             
#         new_kargs = [k for k in a.kargs if not select(k)]
#         to_modify = [k for k in a.kargs if select(k)]
# 
#         def modify(k):
#             t  = k.split('=')
#             return  '%s="%s%s"' % (t[0],
#                                    t[1][1:-1],
#                                    sequence.te_in)
#         new_kargs.extend([modify(k) for k in to_modify])
#         a.kargs = new_kargs


class SequenceTree(object):
    """create a list of sequeneces interconnected to for a directed
    acyclic graph.

    (1/6/2014) Currently sequence trees are simple linear chains of
    sequences. More complex trees were present in 2012."""

    # impose a limit on Sequence aliases
    # to limit the length of trigger element names
    
    max_alias_length = 50  
    
    def __init__(self, start_te, alglists, chain_name):
        """Create a DAG of sequences. The first sequence of the list
        is taken to be the root"""

        root = alglists[0]
        self.alias_table = defaultdict(list)
        self.sequences = []

        self.chain_name = chain_name  # used only for log messages

        # set up a parent-child label of Alglist objects
        for a in alglists:
            self.alias_table[a.attach_to].append(a)

        self._make_sequences(root, start_te)
        self._check()

    def _make_teout_name(self, te_in, alias):
        
        # use te_in and the sequence alias to form te_out.
        # the former defines the input, the latter defines
        # the sequence state (alg parameters).Both are needed to
        # specify what the sequence calculates.
        #
        # 17/5/2016 the te names have grown in length as more
        # complicated chains have been created. Ensure that the
        # name is not excessivey long.

        # te_in is the name of the input trigger element (type string)
        # alias is a string that describes the content of the sequence.
        # this is created in JetSeqeunceBuilder.

        # 22/09/2016. Some sequences are shared by other slices.
        # The code to set these up is not handled ny the jet slice.
        # use the hard coded te_out name which is transported in the sequence
        # alias
        #

        if alias == 'EF_FSTopoClusters': return alias

        
        assert self.max_alias_length
        if len(alias) > self.max_alias_length:

            # the 0th token gives generic information about the
            # sequence. Keep this for human readers
            toks = alias.split('_')
            # hash the rest to get a shorter name
            # Don't use the builtin hash().  With py3, that changes run-to-run,
            # which ultimately implies that the HLTResult objects
            # we write will also vary from run to run.
            ah = binascii.crc32 (alias.encode())
            alias = '%s_%d' % (toks[0], ah)
            
            
        if te_in:
            # te_out = te_in + '__' + alglist.alias
            # Similarly, don't use builtin hash() here.
            h = binascii.crc32 (te_in.encode())
            hash_in = ('%s' % h).replace('-', '_')
            te_out = '%s_%s' % (hash_in, alias)
        else:
            te_out = alias

        # te_out = alglist.alias

        #if not te_out.startswith('HLT_'):
        #    te_out = 'HLT_' + te_out
        if not te_out.startswith('EF_'):
            te_out = 'EF_' + te_out

        return te_out
    
    def _make_sequences(self, alglist, te_in):
        """Convert alglist objects to sequence objects"""

        te_out = self._make_teout_name(te_in, alglist.alias)
        
        new_sequence = Sequence(te_in,
                                alglist.alg_list,
                                alglist.alias,  # for debugging
                                te_out)

        # _update_if_diagnostic_sequence(new_sequence)
        self.sequences.append(new_sequence)

        # having appended the new sequence to the sequence list,
        # the sequence to which it is connected is attached. This
        # will be found elsewhere in the alias table.
        # Note that this code is not protected against cycles - that
        # is if the inputs do not form a tree, this procedure could go
        # into an infinite loop.
        for c in self.alias_table[alglist.alias]:
            self._make_sequences(c, te_out)


        # import pdb;pdb.set_trace()
        
    def _check_for_duplicates(self, l):
        """check no te out label is used for more than sequence"""
        return len(l) != len(set(l))

        
    def _check(self):
        te_outs = [s.te_out for s in self.sequences]
        if self._check_for_duplicates(te_outs):

            msg = '%s._check  chain: %s,te_out  duplicates found %s' % (
                self.__class__.__name__,
                self.chain_name,
                str(te_outs))
            raise RuntimeError(msg)


class SequenceLinear(SequenceTree):
    """Construct a linear sequence of sequnces, such occurs in most
    chains (in 2012 trees occured). A sequence of sequences is a tree
    of sequences with the extra condition that a te_out of one sequence
    can be used at most once as a te_in for another sequence.

    For linear sequences of sequences, the last sequence is well defined.
    Its TE takes the chain name as its name."""

    def __init__(self, start_te, alglists, chain_name):
        SequenceTree.__init__(self, start_te, alglists, chain_name)

    def _make_sequences(self, alglist, te_in):
        SequenceTree._make_sequences(self, alglist, te_in)

        # 22/01/2015 PS by request of the menu group
        # do not rename the last te to the chain name
        # self._rename_last_te()

    def _check(self):
        SequenceTree._check(self)

        te_ins = [s.te_in for s in self.sequences]

        if self._check_for_duplicates(te_ins):
            msg = '%s._check  chain: %s, te_in duplicates found %s' % (
                self.__class__.__name__,
                self.chain_name,
                str(te_ins))
            raise RuntimeError(msg)

    def _rename_last_te(self):
        """identify the last TE of a linear chain, and rename
        it to 'EF_<chainname>' """

        te_ins = [s.te_in for s in self.sequences]
        last_te = [s.te_out for s in self.sequences if s.te_out not in te_ins]
        if len (last_te) == 0:
            msg = '%s._check  chain: %s, no last te' % (
                self.__class__.__name__,
                self.chain_name)
            raise RuntimeError(msg)
        
        if len (last_te) > 1:
            msg = '%s._check  chain: %s, > 1 unconected te+out %s' % (
                self.__class__.__name__,
                self.chain_name,
                str(last_te))
            raise RuntimeError(msg)

        # last_te is now a single element list
        last_te = last_te[0]

        # only rename the output te to the chain name if the name
        # of the sequence alias is marked as being a hypo sequence.
        if '__hypo_' not in last_te:
            return

        last_sequence = [s for s in self.sequences if s.te_out == last_te][0]

        if self.chain_name.startswith('EF_'):
            last_sequence.te_out = self.chain_name
        else:
            last_sequence.te_out = 'EF_jets_' + self.chain_name
        
