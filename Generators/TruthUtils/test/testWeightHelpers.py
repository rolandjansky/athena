#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TruthUtils.WeightHelpers import generatorWeightsPrefix, weightNameCleanup, weightNameWithPrefix

test_weights = [
    ('PDF set = ABC', 'PDF_ABC', 'GEN_PDF_ABC'),
    ('foo = bar', 'foo_bar', 'GEN_foo_bar'),
    ('a=b, c.d, #1', 'ab_cd_num1', 'GEN_ab_cd_num1'),
    ('foo: bar', 'foo_bar', 'GEN_foo_bar'),
    ('a\nb', 'a_b', 'GEN_a_b'),
    ('q/p', 'qoverp', 'GEN_qoverp'),
]

assert generatorWeightsPrefix == 'GEN_'

for w, cleaned, prefixed in test_weights:
    print(weightNameCleanup(w), cleaned, weightNameCleanup(w) == cleaned)
    assert weightNameCleanup(w) == cleaned
    assert weightNameWithPrefix(w) == prefixed
