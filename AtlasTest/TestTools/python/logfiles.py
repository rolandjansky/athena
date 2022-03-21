# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

"""Utilitites for log file parsing and printing"""

import collections
import itertools
import re
import sys

def grep_with_context(f, pattern, lines = 100, print_head = True, print_tail = True):
    """Print errors in file `f` with the given number of context lines before/after
    the `pattern` (regex). In addition print head or tail of file if requested.
    """
    # See: https://stackoverflow.com/questions/36027227

    # Print first N lines:
    if print_head:
       sys.stdout.writelines(itertools.islice(f, lines))

    before = collections.deque(maxlen=lines)
    skipped = 0  # number of skipped lines

    def printBefore():
        '''Print the `before` buffer'''
        nonlocal skipped
        skipped -= len(before)
        if skipped > 0:
            sys.stdout.write('\n[Skipped %d line%s]\n\n' % (skipped, 's' if skipped>1 else ''))
        sys.stdout.writelines(before)
        before.clear()
        skipped = 0

    regex = re.compile(pattern)
    for line in f:
        if re.search(regex, line):
            printBefore()
            sys.stdout.write(line)
            sys.stdout.writelines(itertools.islice(f, lines))
        else:
            skipped += 1
            before.append(line)

    # Print the last N lines:
    if print_tail:
       printBefore()
