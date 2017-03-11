# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Authors: Ben Smart (ben.smart@cern.ch), Xanthe Hoad (xanthe.hoad@cern.ch)
# See https://twiki.cern.ch/twiki/bin/view/Atlas/MaDQM for more information

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

        
        print "Now setting up all trigger-monitoring tools."
        print ""
        print "#"*10
        print ""

        # load all tools in PackagesToInterrogate
        for key, value in self.packages_to_interrogate.PackagesToInterrogate.iteritems():

            # first import the tool from the package
            exec "from %s import %s" % (value['PackageName'],value['ToolName'])

            # then initialise the tool
            exec "%s()" % (value['ToolName'])

        
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

            # OLD:
            # find if Trig and Monitoring are in the tool Dll name
            if "Trig" in tool.getDlls() and "Monitoring" in tool.getDlls():
                mon_tools.append(tool.getName())
            #
            # NEW:
            # find if Trig and Monitoring are in the tool Dll name, or if the tool name contains both 'Mon' and either 'HLT' or 'Trig'
            # (This change was made to catch TrigEgammaMonTool, which has 'TrigEgammaAnalysisTools' as its Dll name)
            # FURTHER EDIT: extra if statemend added below (7 lines below here) to catch all TrigEgammaAnalysisTools, so we can just revert back to the previous 'OLD' if statement above
            #if ( "Trig" in tool.getDlls() and "Monitoring" in tool.getDlls() ) or ( ("HLT" in tool.getName() or "Trig" in tool.getName()) and "Mon" in tool.getName() ):
            #    mon_tools.append(tool.getName())

            # of if the tool Dll name is TrigInDetAnalysisExample, as is the case for the IDtrk tools
            if 'TrigInDetAnalysisExample' == tool.getDlls():
                mon_tools.append(tool.getName())
            # of if the tool Dll name is TrigEgammaAnalysisTools, as is the case for the Egamma tools
            if 'TrigEgammaAnalysisTools' == tool.getDlls():
                mon_tools.append(tool.getName())


        # return the list of monitoring tools
        print "mon_tools =",mon_tools
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

        # we first need to check whether this is the HLTEgammaMon or HLTEgammaDump, or TrigEgammaMonTool tool
        if tool_ToolSvc_name == 'HLTEgammaMon':
            slice_name = 'Egamma'
        elif tool_ToolSvc_name == 'HLTEgammaDump':
            slice_name = 'Egamma_Dump'
        elif tool_ToolSvc_name == 'TrigEgammaMonTool':
            slice_name = 'Egamma'
        else:
            for key, value in self.packages_to_interrogate.PackagesToInterrogate.iteritems():
                if value['Dll'] == tool_Dll:
                    slice_name = key

        # check if slice has been found or not
        if slice_name == '':

            #if not, return -1
            print ""
            print "Additional info not found for ToolSvc tool ", tool_ToolSvc_name
            return -1

        else:

            # fill the smck_config
            smck_config['ToolSvcName'] = tool_ToolSvc_name
            smck_config['PackageName'] = self.packages_to_interrogate.PackagesToInterrogate[slice_name]['PackageName']
            smck_config['ToolName'] =    self.packages_to_interrogate.PackagesToInterrogate[slice_name]['ToolName']
            smck_config['ToolInfo'] =    self.interrogate(tool_ToolSvc_name)
            smck_config['SliceType'] =   slice_name
            smck_config['MonitCategoryName'] = self.packages_to_interrogate.PackagesToInterrogate[slice_name]['MonitCategoryName']
            smck_config['MonitCategoryInfo'] = self.interrogate_MonitCategory(self.packages_to_interrogate.PackagesToInterrogate[slice_name]['MonitCategoryName'])

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

            # GaudiKernel.GaudiHandles.PublicToolHandleArray objects are not storable, so skip them
            # Little hack because MaM doesn't know what GaudiKernel is
            if 'GaudiKernel.GaudiHandles.PublicToolHandleArray' in str(type(value)):
                continue

            # test if this value is JSON serializable
            try:

                # json dump and load,
                # if this is sucessful, then the value can be stored in the SQL database CLOB
                # if it fails, then the try except block will catch this, and the value will not be included in data be be stored
                #throwaway_test_object = self.__unicode_to_str__(json.loads( json.dumps(value, ensure_ascii=True, sort_keys=True) ) )
                throwaway_test_object = json.loads( json.dumps(value, ensure_ascii=True, sort_keys=True) )

                # # now we check that the json packing and unpacking hasn't garbled the data
                # # is the object equal to itself after packing and unpacking (it should be)
                # if throwaway_test_object != value:

                #     
                #     print "In",tool_ToolSvc_name,", the object",prop,"has the value",value
                #     print "However, after json dumping and loading, this object has the value",throwaway_test_object
                #     print "As these values are not equal, the object can not be stored and accurately retrieved from the database, and thus will not be stored."

                #     # do nothing, as we shall not store this item
                #     continue

                # if we've made it this far
                # then this information is good, and should be stored in our dictionary
                tool_info[prop] = value

            # if this value is not JSON serializable
            except:

                # do nothing, as we shall not store this value
                continue

        # return the info we have collected
        return tool_info


    def interrogate_MonitCategory(self,monitCategoryName=''):
        "Extract the relevant MonitCategory info for a locally running trigger-monitoring tool."

        # if the input is empty, then simply return an empty dict
        if monitCategoryName == '':
            return {}

        # now we import the MonitCategory, and set it as monitCategoryObject so that we have an easy handle on it
        monitCategoryObject = ""
        exec "import %s as monitCategoryObject" % (monitCategoryName)

        # next we need to find out what this monitCategoryObject contains
        monitCategoryObject_properties = ""
        exec "monitCategoryObject_properties = dir(monitCategoryObject)"

        # only some of the things in monitCategoryObject_properties are of interest to us
        # we store those items in the dict monitCategoryObject_interesting_properties
        monitCategoryObject_interesting_properties = {}
        for item_name in monitCategoryObject_properties:

            # if the item name doesn't start with __ then it is an object that we are interested in
            if not item_name.startswith('__'):

                # get the value of this item
                item_value = ""
                exec "item_value = monitCategoryObject.%s" % (item_name)

                # test if this value is JSON serializable
                try:

                    # json dump and load,
                    # if this is sucessful, then the item can be stored in the SQL database CLOB
                    # if it fails, then the try except block will catch this, and the item will not be included in data be be stored
                    #throwaway_test_object = self.__unicode_to_str__(json.loads( json.dumps(item_value, ensure_ascii=True, sort_keys=True) ) )
                    throwaway_test_object = json.loads( json.dumps(item_value, ensure_ascii=True, sort_keys=True) )
                    ### HERE BE DRAGONS! ### Something strange lives here. If you make a typo in the line above (as I did) and have 'value' instead of 'item_value', then the code does some very very weird stuff. Really, try it out for yourself, I dare you. And if you work out why it does what it does, please let me know.

                    # # now we check that the json packing and unpacking hasn't garbled the data
                    # # is the object equal to itself after packing and unpacking (it should be)
                    # if throwaway_test_object != item_value:

                    #     
                    #     print "In",monitCategoryName,", the object",item_name,"has the value",item_value
                    #     print "However, after json dumping and loading, this object has the value",throwaway_test_object
                    #     print "As these values are not equal, the object can not be stored and accurately retrieved from the database, and thus will not be stored."

                    #     # do nothing, as we shall not store this item
                    #     continue

                    # if we've made it this far
                    # then this information is good, and should be stored in our dictionary
                    # so add item to monitCategoryObject_interesting_properties
                    monitCategoryObject_interesting_properties[item_name] = item_value

                # if this value is not JSON serializable
                except:

                    # do nothing, as we shall not store this item
                    continue

        # now we can return the results
        return monitCategoryObject_interesting_properties


    def __unicode_to_str__(self,input1=""):
        "Input a unicode string, list, or dict, and convert all unicode to str."

        # test for the type of input1
        # if need be, recursively call this function
        # where unicode is found, convert it to str
        if type(input1) is str:
            return input1
        elif type(input1) is unicode:
            return str(input1)
        elif type(input1) is list:
            for n, item in enumerate(input1):
                input1[n] = self.__unicode_to_str__(item)
            return input1
        elif type(input1) is dict:
            return_dict = {}
            for key, value in input1.iteritems():
                return_dict[str(key)] = self.__unicode_to_str__(value)
            return return_dict
        else:
            return input1
