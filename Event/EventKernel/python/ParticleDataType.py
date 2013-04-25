# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# ParticleDataType.py
# Author: Kyle Cranmer <Kyle.Cranmer@cern.ch>
# 
# Purpose: This file defines the values of the enum ParticleDataType::DataType
#          so that they can be used in Python in a fairly transparent way
#
# Usage: 1) import 2) use
#  1) import EventKernel.ParticleDataType
#  2) muonBuilder.MuonBuilderTool.dataType=EventKernel.ParticleDataType.Full

Data=0
Full=1
FastShower=2
Fast=3
True=4
