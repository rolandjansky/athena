# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# Configuration functions for DL1FlipTag
# Author: Marie Lanfermann (September 2015)
from BTagging.BTaggingFlags import BTaggingFlags
from BTagging.BTaggingConfiguration_DL1Tag import buildDL1

toolDL1rmuFlipTag, metaDL1rmuFlipTag = buildDL1("DL1rmuFlip","DL1rmu")
toolDL1FlipTag, metaDL1FlipTag = buildDL1("DL1Flip", "DL1")
toolDL1rFlipTag, metaDL1rFlipTag = buildDL1("DL1rFlip", "DL1r")
