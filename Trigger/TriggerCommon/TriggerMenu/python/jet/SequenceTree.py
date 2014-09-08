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
        self.teout_namechecker = defaultdict(set)

        # set up a parent-child label of Alglist objects
        for a in alglists:
            self.alias_table[a.attach_to].append(a)

        self._make_sequences(root, start_te)
        self._check_for_duplicate_teouts()

    def _make_sequences(self, alglist, te_in):
        """Convert alglist objects to sequence objects"""

        # ensure te_out names do not upset people
        if te_in:
            te_out = te_in + '__' + alglist.alias
        else:
            te_out = alglist.alias

        #if not te_out.startswith('HLT_'):
        #    te_out = 'HLT_' + te_out
        if not te_out.startswith('EF_'):
            te_out = 'EF_' + te_out
        self.sequences.append(Sequence(te_in, alglist.alg_list, te_out))

        for c in self.alias_table[alglist.alias]:
            self._make_sequences(c, te_out)
        
        
    def _check_for_duplicate_teouts(self):
        """check no te out label is used for more than sequence"""

        def _make_identifier(seq):
            s_id = [seq.te_in]
            s_id.extend([a.asString() for a in seq.alg_list])

        [self.teout_namechecker[seq.te_out].add(_make_identifier(seq)) for
         seq in self.sequences]

        for a, s in self.teout_namechecker.items():
            if len(s) > 1:
                msg = '%s._make_sequences, te_out: %s collision %s'\
                    'chain: %s'
                msg = msg % (self.__class__.__name__,
                             te_out, ' '.join(s),
                             self.chain_name)
                raise RuntimeError(msg)

        
