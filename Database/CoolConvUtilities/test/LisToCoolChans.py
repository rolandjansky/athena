#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# execute LisToCool multiple times for different channels
# used for transferring Tile CTB beam energy data to COOL
# Time offset of 1 hour for fixing lisbon problem

import sys,os,traceback

# Name of Lisbon folder - channel number will be between head and tail
#lisfolderhead="/tile/dcs/Tile_LV_62:beam.val"
lisfolderhead="/tile/dcs/System1:beam.val"
lisfoldertail=":online..value"

# Name of COOL database and folder
cooldb="sqlite://X;schema=testdcs.db;dbname=TBP130"
#coolfolder="/TILE/DCS/TILE_LV_62/BEAM"
coolfolder="/TILE/DCS/SYSTEM1/BEAM"

# first and channels to do
chan1=6
chan2=138
# cool variablename string (if more than one name, separate by commas)
coolvar="val"

for ichan in range (chan1,chan2+1):
    lisfolder=lisfolderhead+str(ichan)+lisfoldertail
    print "Extract data from folder",lisfolder,"into channel",ichan
    os.system("LisToCool.exe \"atlobk02.cern.ch:conditions_ctb_2004:conditions:conditions\" \""+lisfolder+"\" \""+cooldb+"\" \""+coolfolder+"\" 11 1 \""+coolvar+"\" "+str(ichan))
    
