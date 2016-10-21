#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# 2016 Ryan Mackenzie White <ryan.white@cern.ch>
#

"""
Configuration of TrigDecisionTool for running analysis jobs in athena
"""
# Gaudi is very smart, all the tools declared 
# in the initialisation list of the algorithm
# will be initialised with their default properties
# no need to do the configuration yourself, unless updating properties
# Since the defaults for TDT need some updating, we need to do the following 

# The default for TDT is to use the xAODConfigTool
# No need to configure yourself
# import AthenaCommon.CfgMgr as CfgMgr
from AthenaCommon.AppMgr import ToolSvc

#cfgtool = CfgMgr.TrigConf__xAODConfigTool("xAODConfigTool")
#ToolSvc += cfgtool

# TDT needs a little help
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
from TrigEDMConfig.TriggerEDM import EDMLibraries
ToolSvc.TrigDecisionTool.Navigation.Dlls = [e for e in  EDMLibraries if 'TPCnv' not in e]
# The following should become the default for reading AODs
#ToolSvc.TrigDecisionTool.Navigation.ReadonlyHolders = True
