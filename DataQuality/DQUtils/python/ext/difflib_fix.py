# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# See http://bugs.python.org/issue1711800

def patch_get_opcodes():
    import difflib
    
    difflib.SequenceMatcher.get_opcodes = get_opcodes

def get_opcodes(self):
    """Return list of 5-tuples describing how to turn a into b.

    Each tuple is of the form (tag, i1, i2, j1, j2).  The first tuple
    has i1 == j1 == 0, and remaining tuples have i1 == the i2 from the
    tuple preceding it, and likewise for j1 == the previous j2.

    The tags are strings, with these meanings:

    'replace':  a[i1:i2] should be replaced by b[j1:j2]
    'delete':   a[i1:i2] should be deleted.
                Note that j1==j2 in this case.
    'insert':   b[j1:j2] should be inserted at a[i1:i1].
                Note that i1==i2 in this case.
    'equal':    a[i1:i2] == b[j1:j2]

    >>> a = "qabxcd"
    >>> b = "abycdf"
    >>> s = SequenceMatcher(None, a, b)
    >>> for tag, i1, i2, j1, j2 in s.get_opcodes():
    ...    print ("%7s a[%d:%d] (%s) b[%d:%d] (%s)" %
    ...           (tag, i1, i2, a[i1:i2], j1, j2, b[j1:j2]))
     delete a[0:1] (q) b[0:0] ()
      equal a[1:3] (ab) b[0:2] (ab)
    replace a[3:4] (x) b[2:3] (y)
      equal a[4:6] (cd) b[3:5] (cd)
     insert a[6:6] () b[5:6] (f)
    """

    if self.opcodes is not None:
        return self.opcodes
    i = j = 0
    self.opcodes = answer = []
    for ai, bj, size in self.get_matching_blocks():
        # invariant:  we've pumped out correct diffs to change
        # a[:i] into b[:j], and the next matching block is
        # a[ai:ai+size] == b[bj:bj+size].  So we need to pump
        # out a diff to change a[i:ai] into b[j:bj], pump out
        # the matching block, and move (i,j) beyond the match
        tag = ''

        if i < ai and j < bj:
            tag = 'replace'
            
            sizea, sizeb = ai - i, bj - j
            sizediff = abs(sizeb - sizea)
            
            if sizea < sizeb:
                answer.append( (tag,      i, i+sizea, j, j+sizea) )
                i, j = i + sizea, j + sizea
                answer.append( ("insert", i, i,       j, j+sizediff) )  
                j += sizediff
                tag = ''
                
            elif sizea > sizeb:
                answer.append( (tag,      i, i+sizeb,    j, j+sizeb) )
                i, j = i + sizeb, j + sizeb
                answer.append( ("delete", i, i+sizediff, j, j) )    
                i += sizediff
                tag = ''
            
        elif i < ai:
            tag = 'delete'
            
        elif j < bj:
            tag = 'insert'
            
        if tag:
            answer.append( (tag, i, ai, j, bj) )
        i, j = ai+size, bj+size
        # the list of matching blocks is terminated by a
        # sentinel with size 0
        if size:
            answer.append( ('equal', ai, i, bj, j) )
    return answer
