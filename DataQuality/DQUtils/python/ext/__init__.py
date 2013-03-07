# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from collections import defaultdict
def tally(stuff):
    """
    From python thread http://mail.python.org/pipermail/python-dev/2006-April/063462.html
    Subject: [Python-Dev] tally (and other accumulators)
    """
    accumulator = defaultdict(int)
    for element in stuff:
        accumulator[element] += 1
    return sorted(accumulator.iteritems())

