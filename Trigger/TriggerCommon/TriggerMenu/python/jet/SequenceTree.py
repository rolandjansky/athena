# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from collections import defaultdict
from Sequence import Sequence

class SequenceTree(object):
    """create a list of sequeneces interconnected to for a directed
    acyclic graph.

    (1/6/2014) Currently sequence trees are simple linear chains of
    sequences. More complex trees were present in 2012."""

    def __init__(self, start_te, alglists, chain_name):
        """Create a DAG of sequences. The first sequence of the list
        is taken to be the root"""

        root = alglists[0]
        self.alias_table = defaultdict(list)
        self.sequences = []

        self.chain_name = chain_name

        # set up a parent-child label of Alglist objects
        for a in alglists:
            self.alias_table[a.attach_to].append(a)

        self._make_sequences(root, start_te)
        self._check()

    def _make_sequences(self, alglist, te_in):
        """Convert alglist objects to sequence objects"""

        # use te_in and the seuquence alias to form te_out.
        # the formeer defines the input, the latter defines
        # the sequence state (alg parameters).Both are needed to
        # specify what the sequence calculates.
        if te_in:
            te_out = te_in + '__' + alglist.alias
        else:
            te_out = alglist.alias

        # te_out = alglist.alias

        #if not te_out.startswith('HLT_'):
        #    te_out = 'HLT_' + te_out
        if not te_out.startswith('EF_'):
            te_out = 'EF_' + te_out
        self.sequences.append(Sequence(te_in, alglist.alg_list, te_out))

        # having appended the new sequence to the sequence list,
        # the sequence to which it is connected is attached. This
        # will be found elsewhere in the alias table.
        # Note that this code is not protected against cycles - that
        # is if the inputs do not form a tree, this procedure could go
        # into an infinite loop.
        for c in self.alias_table[alglist.alias]:
            self._make_sequences(c, te_out)
        
        
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
        
