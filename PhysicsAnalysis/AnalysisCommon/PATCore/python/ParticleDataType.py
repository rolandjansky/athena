# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# ParticleDataType.py
# Author: Karsten Koeneke <karsten.koeneke@cern.ch>
# 
# Purpose: This file defines the values of the enum PATCore::ParticleDataType::DataType
#          so that they can be used in Python in a fairly transparent way
#
# Usage: 1) import 2) use
#  1) import PATCore.ParticleDataType
#  2) myTool.dataType = PATCore.ParticleDataType.Full

Data       = 0
Full       = 1
FastShower = 2
Fast       = 3
True       = 4  # This is actually a very bad idea! Be very careful with it!

