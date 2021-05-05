#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from argparse import ArgumentParser
from TruthUtils.WeightHelpers import weightNameCleanup, weightNameWithPrefix

parser = ArgumentParser(prog='generator-weights')
parser.add_argument('weights', metavar='weights', type=str, nargs="+",
                    help="specify the list of weights")
parser.add_argument('-p', '--prefixed', default=False, action="store_true",
                    help="add prefix to cleaned weights")
args = parser.parse_args()

for weight in args.weights:
  if args.prefixed:
    print(weightNameWithPrefix(weight))
  else:
    print(weightNameCleanup(weight))
