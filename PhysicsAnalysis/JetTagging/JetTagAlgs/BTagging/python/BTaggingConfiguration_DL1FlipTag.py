# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for DL1FlipTag
# Author: Marie Lanfermann (September 2015)
from BTagging.BTaggingFlags import BTaggingFlags
from BTagging.BTaggingConfiguration_DL1Tag import buildDL1

toolDL1muFlipTag, metaDL1muFlipTag = buildDL1("DL1muFlip","DL1mu")
toolDL1FlipTag, metaDL1FlipTag = buildDL1("DL1Flip", "DL1")
toolDL1rnnFlipTag, metaDL1rnnFlipTag = buildDL1("DL1rnnFlip", "DL1rnn")
