#! /usr/bin/env sh
###################################################################
#   ATLAS setup environment script
#   Wrapper script to submit MuonDQA jobs to LXBATCH
#   - using Rel. = AtlasPoint1  
###################################################################
#   (C) Nektarios.Chr.Benekos-Illinois
################################################################### 
#--- the following variables must (!!! TO BE ADJUSTED !!!) 
export Mn=M5 
export numevents=2000
export skip=0
export dstnum=29576
export rel=1320Y

#---  Do not add flags or variables below this line
#--- the following variables must not be changed (!!! TO BE ADJUSTED !!!) 
export process=muonoffdqa
export datatype=MuonMon$Mn
export prefix=daq.muon_combined_$Mn
export type=physics.cosmics
export sample=$prefix.00$dstnum.$type
