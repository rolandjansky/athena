# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# Author: Ben Smart (bsmart@cern.ch)
#

import sys
# needed for getting package names etc. of tools
from TrigHLTMonitoring.PackagesToInterrogate import PackagesToInterrogate
# needed to access the tools in ToolSvc
from AthenaCommon.AppMgr import ToolSvc
#import json to dump and load each smck_config, so that any odd characters are consistently encoded in the output smck_config dictionary
import json

# put all local-package interrogation tools in a single class
class ToolInterrogator:
    """A class to find locally running trigger-monitoring tools, and extract their configurations from them.
    Also provides functionality to set up all trigger-monitoring tools, such that they can be interrogated."""


    def __init__(self):
        """Set up ToolInterrogator.
        Get dictionary of trigger-monitoring tools from PackagesToInterrogate"""

        # Packages to interrogate
        self.packages_to_interrogate = PackagesToInterrogate()


    def load_all_tools(self):
        "Set up all trigger-monitoring tools, such that they can be interrogated."

        # info for user
        print "Now setting up all trigger-monitoring tools."
        print ""
        print "#"*10
        print ""

        # nasty hacks to fix problems in HLTMonTriggerList.py in Athena 20.1.5.8
        #
        # line 60, in config
        # self.primary_single_ele_iso = egamma.primary_single_ele_iso
        # AttributeError: 'module' object has no attribute 'primary_single_ele_iso'
        #
        # line 84, in config
        # self.monitoring_muonEFFS = muon.monitoring_muonEFFS
        # AttributeError: 'module' object has no attribute 'monitoring_muonEFFS'
        #
        # so we give these objects the offending attributes...
        # import TrigEgammaMonitoring.TrigEgammaMonitCategory as egamma
        # egamma.primary_single_ele_iso = ['e24_lhmedium_iloose_L1EM18VH','e24_lhmedium_iloose_L1EM20VH','e24_lhtight_iloose_L1EM20VH','e24_lhtight_iloose','e26_lhtight_iloose']
        # egamma.primary_single_ele_cutbased_iso = ['e24_medium_iloose_L1EM18VH','e24_medium_iloose_L1EM20VH','e24_tight_iloose_L1EM20VH','e24_tight_iloose','e26_tight_iloose']
        # import TrigMuonMonitoring.TrigMuonMonitCategory as muon
        # muon.monitoring_muonEFFS = ['HLT_mu18_mu8noL1']

        # load all tools in PackagesToInterrogate
        for key, value in self.packages_to_interrogate.PackagesToInterrogate.iteritems():

            # first import the tool from the package
            exec "from %s import %s" % (value['PackageName'],value['ToolName'])

            # then initialise the tool
            exec "%s()" % (value['ToolName'])

        # info for user
        print ""
        print "#"*10
        print ""
        print "All trigger-monitoring tools have now been set up."


    def get_available_trigger_monitoring_tools(self):
        "Get list of currently running trigger-monitoring tools"

        # get list of all tools in ToolSvc
        tool_list = ToolSvc.getAllChildren()

        # list of monitoring tools
        mon_tools = []

        # loop over tools
        for tool in tool_list:

            # skip tools with names that will garble our commands
            # (no TrigMonTools do this)
            # specifically skip tools with scope resolution operators in their names (why do they have these here!?)
            if "::" in tool.getName():
                print "Skipping",tool.getName(),"as its name contains a scope resolution operator '::'."
                continue

            # find if Trig and Monitoring are in the tool Dll name
            if "Trig" in tool.getDlls() and "Monitoring" in tool.getDlls():
                mon_tools.append(tool.getName())

            # of if the tool Dll name is TrigInDetAnalysisExample, as is the case for the IDtrk tools
            if 'TrigInDetAnalysisExample' == tool.getDlls():
                mon_tools.append(tool.getName())

        # return the list of monitoring tools
        return mon_tools


    def check_if_valid_tool(self,tool_ToolSvc_name):
        "Is this tool a locally running tool (is it in ToolSvc)."

        # check that we have been given a valid name
        if not hasattr(ToolSvc,tool_ToolSvc_name):
            print "ToolSvc does not contain a tool named",tool_ToolSvc_name
            return -1


    def get_smck_config_from_ToolSvc_tool_name(self,tool_ToolSvc_name):
        "Get the configuration and all other pertinent information related to a locally running trigger-monitoring tool."

        # check if this is a valid tool
        if self.check_if_valid_tool(tool_ToolSvc_name) == -1:
            return -1

        # get tool Dll
        tool_Dll = ""
        exec "tool_Dll = ToolSvc.%s.getDlls()" % (tool_ToolSvc_name)

        # the slice_name, the key in packages_to_interrogate.PackagesToInterrogate
        slice_name = ''

        # the smck_config to fill and return
        smck_config = {}

        # we first need to check whether this is the HLTEgammaMon or HLTEgammaDump tool
        if tool_ToolSvc_name == 'HLTEgammaMon':
            slice_name = 'Egamma'
        elif tool_ToolSvc_name == 'HLTEgammaDump':
            slice_name = 'Egamma_Dump'
        else:
            for key, value in self.packages_to_interrogate.PackagesToInterrogate.iteritems():
                if value['Dll'] == tool_Dll:
                    slice_name = key

        # check if slice has been found or not
        if slice_name == '':

            #if not, return -1
            print "Additional info not found for ToolSvc tool ", tool_ToolSvc_name
            return -1

        else:

            # fill the smck_config
            smck_config['ToolSvcName'] = tool_ToolSvc_name
            smck_config['PackageName'] = self.packages_to_interrogate.PackagesToInterrogate[slice_name]['PackageName']
            smck_config['ToolName'] =    self.packages_to_interrogate.PackagesToInterrogate[slice_name]['ToolName']
            smck_config['ToolInfo'] =    self.interrogate(tool_ToolSvc_name)
            smck_config['SliceType'] =   slice_name

            # json dump and load, 
            # so that any odd characters are consistently encoded in the output smck_config dictionary
            smck_config = json.loads( json.dumps(smck_config, ensure_ascii=True, sort_keys=True) )

            # return filled smck_config
            return smck_config


    def interrogate(self,tool_ToolSvc_name):
        "Extract the configuration from a locally running trigger-monitoring tool."

        # check if this is a valid tool
        if self.check_if_valid_tool(tool_ToolSvc_name) == -1:
            return -1

        # now we begin interrogating the tool...
            
        # This is the dictionary where we will store all our good gathered information
        tool_info = {}
    
        # first we get its properties
        tool_properties = ""
        exec "tool_properties = ToolSvc.%s.properties()" % (tool_ToolSvc_name)
    
        # we also get the property 'no value' string for this tool (to use later)
        tool_novalue = ""
        exec "tool_novalue = ToolSvc.%s.propertyNoValue" % (tool_ToolSvc_name)

        # and we get the default property values
        tool_default_properties = ""
        exec "tool_default_properties = ToolSvc.%s.getDefaultProperties()" % (tool_ToolSvc_name)
    
        # then we check these properties
        for prop,value in tool_properties.iteritems():
        
            # does the tool really have this property?
            tool_property_truth = False
            exec "tool_property_truth = hasattr(ToolSvc.%s,prop)" % (tool_ToolSvc_name)
        
            # if it does not...
            # then the value has likely not been set
            # ie. the default value is to be used
            if tool_property_truth == False:
            
                # does this property hold a 'no value' value?
                if value == tool_novalue:
                
                    # if so, use the default value
                    if tool_default_properties.__contains__(prop):
                        value = tool_default_properties[prop]
                    else:
                        continue

                # else if this tool does not contain this property
                # and this property is also not just currently set to 'no value'
                # then skip this property
                else:
                    continue
    
            # test if this value is JSON serializable
            try:

                # json dump and load,
                # if this is sucessful, then the value can be stored in the SQL database CLOB
                # if it fails, then the try except block will catch this, and the value will not be included in data be be stored
                throwaway_test_object = json.loads( json.dumps(value, ensure_ascii=True, sort_keys=True) )
                                
                # if we've made it this far
                # then this information is good, and should be stored in our dictionary
                tool_info[prop] = value

            # if this value is not JSON serializable
            except:

                # do nothing, as we shall not store this value
                continue

        # return the info we have collected
        return tool_info

