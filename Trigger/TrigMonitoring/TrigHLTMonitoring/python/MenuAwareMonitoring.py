# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# Authors: Ben Smart (Ben.Smart@cern.ch), Xanthe Hoad (Xanthe.Hoad@cern.ch) 
#

import sys,os
# import Athena-less MaM
from TrigHLTMonitoring.MenuAwareMonitoringStandalone import MenuAwareMonitoringStandalone
# import oracle interaction class
from TrigHLTMonitoring.OracleInterface import OracleInterface
# import tool interrogator
from TrigHLTMonitoring.ToolInterrogator import ToolInterrogator
# needed to access the tools in ToolSvc
from AthenaCommon.AppMgr import ToolSvc
# use rec to find out if we are making ESDs and/or AODs
from RecExConfig.RecFlags import rec
# import subprocess. Required to get Athena version (there must be a better way!)
import subprocess
# import hash library for generating smck config hashes
import hashlib
# import json for converting configuration dictionaries into strings for hashing
import json
# for getting connection details
from xml.dom import minidom

# all Menu-Aware Monitoring stuff in one class
# it uses OracleInterface to talk to the Oracle database
# and ToolInterrogator to talk to the local tools
class MenuAwareMonitoring:
    """Menu-aware Monitoring class.
    Provides functionality for the upload/download of trigger-monitoring tool configurations to an Oracle database,
    as well as reading in of configurations from locally running tools, and application of configurations to these tools."""


    def __init__(self,database_username="",database_password="",database_name="",database_directory=""):
        """Setup Menu-aware Monitoring,
        find locally running trigger-monitoring tools,
        connect to the Oracle database,
        and get the current default from the database (if it exists)."""

        # flag for setting whether to print out anything to screen or not
        self.print_output = True

        # flag so that diff instruction are only printed once
        self.firstdiff = True
        
        # create MaMStandalone interaction object (includes connecting to oracle)
        if not database_username and not database_password and not database_name and not database_directory:
            self.ms = MenuAwareMonitoringStandalone("TRIGGERDB",database_username,database_password,database_name)
        else:
            self.ms = MenuAwareMonitoringStandalone("",database_username,database_password,database_name)
    
        # holder for current local Athena version
        self.current_athena_version = ""
        self.__get_athena_version__()
        
        # holder for stream (bulk or express)
        self.stream = ""
        self.__get_stream__()

        # print guide for user if this is an interactive session
        if self.ms.__is_session_interactive__():
            
            # print guide for user
            # TODO
            #print ""
            #print "Authors of this code: Ben Smart (Ben.Smart@cern.ch), Xanthe Hoad (Xanthe.Hoad@cern.ch)"
            #print "This is Menu-aware monitoring version",self.version
            #print "You are",self.ms.current_user
            print "Running in Athena release",self.current_athena_version
            print "Stream detected:",self.stream
            print ""
    
        # create oracle interaction object
        #self.oi = OracleInterface()

        # create tool interrogator object
        self.ti = ToolInterrogator()

        #need to grab ms.local_global_info and make all methods act on that

        # pointer to local tool info
        self.local = self.ms.local_global_info['MONITORING_TOOL_DICT']

        # automatically fill current local tool info
        self.get_current_local_info()

        # flag to record if we have connected to Oracle
        #self.ms.connected_to_oracle = False
        

        # fill default global info (if available)
        if self.ms.connected_to_oracle == True:
            self.ms.get_default_from_db(self.current_athena_version)


    def __quiet_output__(self):
        """Print no output.
        This can be overwritten for a single function by passing print_output_here=True to that function."""

        # print nothing to screen
        self.print_output = False


    def __verbose_output__(self):
        """Print all output (default).
        This can be overwritten for a single function by passing print_output_here=False to that function."""

        # print output to screen by default
        self.print_output = True


    def __get_stream__(self):
        """Determine stream (express or bulk) currently being used.
        The local variable <ThisVariable>.stream is set to the result."""

        # set self.stream to the output of rec.triggerStream()
        # this will equal 'EXPRESS' in the case of express stream, and 'BULK' in the case of the bulk stream. 
        # rec.triggerStream() seems to be able to take on many other values.
        # 
        # if extra options are desired as valid user input to match this variable, 
        # then thy just need to be added to the list of valid inputs in the function
        # ms.__ask_for_processing_stream__()
        # in the list valid_input
        #
        self.stream = str(rec.triggerStream()).upper()


    def __get_tag__(self,package=""):
        """Get the tag of the input package.
        If the trunk/HEAD of this package is being used, the svn revision number will also be returned."""

        # if the input is empty, return an empty result
        if package == "":
            return ""

        # if the package is checked out locally, then find it
        bash_command = "echo $(if [ -e $TestArea/InstallArea/include/"+package+"/"+package+"/ ] ; then cat $(cat $TestArea/InstallArea/include/"+package+"/"+package+".cmtref )/../.svn/entries | grep -m 1 -B 1 \"svn+ssh\"; fi) | sed \"s@ @ URL: @\" "
        local_version = subprocess.check_output( bash_command , shell=True).replace("\n","")

        # get the tag (only valid if a non-local package is being used!)
        bash_command = "for d in $(echo $JOBOPTSEARCHPATH | sed \"s@:@ @g\"); do if [ -e $d/../include/"+package+"/"+package+"/ ]; then cat $d/../../Trigger/TrigMonitoring/"+package+"/cmt/version.cmt; fi; done | grep -m 1 \""+package+"\" "
        package_tag = subprocess.check_output( bash_command , shell=True).replace("\n","")

        # if something local has been found and it is the trunk, then return that, otherwise return the package_tag
        if local_version != "":
            if local_version.__contains__("trunk"):
                # string to return
                return_str = "Revision: "+local_version
                return return_str
        if package_tag != "":
            return package_tag
        else:
            return ""


    def __get_athena_version__(self):
        "Get the current Athena version."

        # get the current local Athena version (there must be a better way!)
        self.current_athena_version = subprocess.check_output("echo $AtlasVersion", shell=True).replace("\n","")


    def __update_local_pointer__(self):
        """update self.local to point to self.ms.local_global_info['MONITORING_TOOL_DICT']
        Needed if self.ms.local_global_info has been overwritten."""
        
        self.ms.__update_local_pointer__()
        self.local = self.ms.local


    def list_local_tools_read_in(self):
        """List local running trigger-monitoring tools that have been discovered by the Tool Interrogator.
        These tool configurations are available in the <ThisVariable>.local dictionary."""

        # info for user
        print "The following local tools have had their configurations extracted by the ToolInterrogator:"

        # list the tools in self.ms.local_global_info['MONITORING_TOOL_DICT']
        for tool in self.ms.local_global_info['MONITORING_TOOL_DICT'].keys():
            print tool


    def __is_input_a_valid_current_processing_step_to_use__(self,input1="",print_output_here=""):
        "Is input1 ('ALL', 'ESD', or 'AOD') valid for the current Athena running stage."

        # check for empty print_output_here
        # if it is found, use self.print_output
        if print_output_here == "":
            print_output_here = self.print_output

        # returns True or False depending on whether the input is valid to be used in the current stage of Athena running

        # if no input, then return false
        if input1 == "":
            return False

        # if input is ALL, then return true
        if input1 == "ALL":
            return True

        # if input is ESD, and doESD is true, then return true
        if input1 == "ESD":
            return rec.doESD

        # if input is AOD, and doAOD is true, then return true
        if input1 == "AOD":
            return rec.doAOD

        # if we have reached this far, then the input has not been recognised
        if print_output_here:
            print "The processing step",input1,"has not been recognised. Valid options are ALL, ESD, and AOD."


    def __is_input_a_valid_current_processing_stream_to_use__(self,input1="",print_output_here=""):
        "Is input1 ('ALL', 'BULK', 'EXPRESS', or other) valid for the current stream."

        # if extra options are desired, then thy just need to be added to the list of valid inputs in the function
        # ms.__ask_for_processing_stream__()
        # in the list valid_input

        # check for empty print_output_here
        # if it is found, use self.print_output
        if print_output_here == "":
            print_output_here = self.print_output

        # returns True or False depending on whether the input is valid to be used in the current stream

        # if no input, then return false
        if input1 == "":
            return False

        # if input is ALL, then return true
        if input1 == "ALL":
            return True

        # if input equals self.stream, then return true
        if input1 == self.stream:
            return True

        # if we have reached this far, then the input is not valid for the current stream
        return False


    def get_current_local_info(self,print_output_here=""):
        """Use the Tool Interrogator to find locally running trigger-monitoring tools.
        These tool configurations are made available in the <ThisVariable>.local dictionary."""

        # check for empty print_output_here
        # if it is found, use self.print_output
        if print_output_here == "":
            print_output_here = self.print_output

        # info for user
        if print_output_here:
            print "Attempting to get configurations of all locally running trigger-monitoring tools"

        # get list of locally running monitoring tools
        mon_tools = self.ti.get_available_trigger_monitoring_tools()

        # print info about available local tools
        if len(mon_tools) > 0:
            if print_output_here:
                print "The following trigger-monitoring tools have been found locally:"
                print ""

        # get smck_config for each of these tools
        for tool in mon_tools:

            # get smck_config
            smck_config = self.ti.get_smck_config_from_ToolSvc_tool_name(tool)

            # check that smck_config has been found
            # if not (Athena oddities can cause this) then skip to the next tool
            # only correct, valid trig mon tools should survive this process
            if smck_config == -1:
                continue

            # construct smck_info for this tool
            smck_info = {}
            smck_info['SMCK_CONFIG'] = smck_config 
            smck_info['SMCK_CONFIG_HASH'] = self.ms.__get_config_hash__(smck_config)
            smck_info['SMCK_SLICE_TYPE'] = smck_config['SliceType']
            smck_info['SMCK_TOOL_TYPE'] = tool
            smck_info['SMCK_ATHENA_VERSION'] = self.current_athena_version
            smck_info['SMCK_SVN_TAG'] = self.__get_tag__(str(smck_config['PackageName']).split(".")[0])

            # add this info to the local_global_info
            self.ms.local_global_info['MONITORING_TOOL_DICT'][tool] = smck_info

            # print info about this tool
            if print_output_here:
                print "ToolSvc."+tool
                print "The extracted data of this tool is stored in <ThisVariable>.local['"+tool+"']"
                print "This can be passed to MaM methods with the string '"+tool+"'"
                print ""


        # update self.local
        self.__update_local_pointer__()

        # add nice spacing if we have been printing tool info
        if len(mon_tools) > 0:
            if print_output_here:
                print "The extracted data of all local trigger-monitoring tools is stored in <ThisVariable>.local"
                print "All local trigger-monitoring tools can be passed together as an 'MCK' to MaM diff and search methods with the string 'LOCAL'"
                print ""


    def setup_all_local_tools(self):
        "Setup all local trigger-monitoring tools and runs get_current_local_info() to read them in using the Tool Interrogator."

        # setup all local packages listed in PackagesToInterrogate via ToolInterrogator
        self.ti.load_all_tools()

        # we probably want to read in these tools with the Tool Interrogator
        self.get_current_local_info()
                    

    def upload_smck(self,input1="",processing_step="",comment="",print_output_here=""):
        """Upload local configuration for tool 'input1' as an SMCK.
        If input1=='local', then all local configuration changes wrt the default will be uploaded.
        Optional processing step and comment can be provided."""

        if self.ms.connected_to_oracle == False:
            print "You are not connected to the database, so this function is not available."
            return

        if self.replica_db_connection == True:
            print "You are connected to the replica database and your connection is read only, so this function is not available to you."
            return
        
        # check for empty print_output_here
        # if it is found, use self.print_output
        if print_output_here == "":
            print_output_here = self.print_output

        # search for default mck
        default_mck = self.ms.get_default_mck_id_from_db()

        # if the default does not exist
        if default_mck < 0:

            # info for user
            if print_output_here:
                print "No default for this Athena version ("+self.current_athena_version+") has been uploaded"
                print "If you are not running with any local changes to the default, then consider running the command \"<ThisVariable>.ms.upload_default()\""
            return

        # if input is local, then run ms.upload_all_local_changes_as_smck()
        if input1 == 'local':

            if print_output_here:
                print "You have provided the input 'local'. All local changes wrt the default will be uploaded."
            self.ms.upload_all_local_changes_as_smck(processing_step,comment)

        # get all local tool info 
        #self.get_current_local_info()

        # check if input1 is running (has been read in by the above line)
        if not self.ms.local_global_info['MONITORING_TOOL_DICT'].__contains__(input1):

            # this tool has not been loaded
            # exit and suggest to the user how to start the tool, if they so wish
            if print_output_here:
                print "Tool",input1,"is not currently set up locally, so can not have its local configuration uploaded as an SMCK."
                print "To list all local tools currently set up and read in, please run \"<ThisVariable>.list_local_tools_read_in()\""
                print "To set up and read in all trigger monitoring tools locally, please run \"<ThisVariable>.setup_all_local_tools()\""
            return

        # get local smck_info for tool input1
        local_smck_info = self.ms.local_global_info['MONITORING_TOOL_DICT'][input1]

        # get default from database
        # (should already have been done during __init__, 
        # but in case the default has only been uploaded in this session then we check again)
        self.ms.get_default_from_db()

        # get default smck_info
        default_smck_info = self.ms.default_global_info['MONITORING_TOOL_DICT'][input1]
        
        # create diff of smck_info
        # we want diffed_smck_info2, 
        # which is the 'patch' to apply to the default to get the current local configuration
        diffed_smck_info1, diffed_smck_info2 = self.ms.__calculate_diff__(default_smck_info,local_smck_info,False)

        # if there are no local differences wrt the default, then we upload nothing and exit
        if diffed_smck_info2 == {}:
            
            # info for user
            if print_output_here:
                print "No local differences have been found with respect to the default SMCK (SMCK_ID="+str(default_smck_info['SMCK_ID'])+") for this tool ("+str(input1)+"), for this Athena version ("+self.current_athena_version+")."
                print "Nothing shall be uploaded to the Oracle database as a result."
            return

        # check if this SMCK already exists

        # if no processing_step is provided, then ask for one
        if processing_step=="":
            processing_step = self.ms.__ask_for_processing_step__()

        # if no processing_stream is provided, then ask for one
        if processing_stream=="":
            processing_stream = self.ms.__ask_for_processing_stream__()

        # if no comment is provided, then ask for one
        if comment=="":
            comment = self.ms.__ask_for_comment__()

        # fill extra smck_info
        diffed_smck_info2['SMCK_PROCESSING_STEP'] = processing_step
        diffed_smck_info2['SMCK_PROCESSING_STREAM'] = processing_stream
        if not diffed_smck_info2.__contains__('SMCK_CONFIG'):
            diffed_smck_info2['SMCK_CONFIG'] = {}
        diffed_smck_info2['SMCK_CONFIG']['PackageName'] = local_smck_info['SMCK_CONFIG']['PackageName']
        diffed_smck_info2['SMCK_CONFIG']['ToolName'] = local_smck_info['SMCK_CONFIG']['ToolName']
        diffed_smck_info2['SMCK_CONFIG']['ToolSvcName'] = local_smck_info['SMCK_CONFIG']['ToolSvcName']
        if not diffed_smck_info2['SMCK_CONFIG'].__contains__('ToolInfo'):
            diffed_smck_info2['SMCK_CONFIG']['ToolInfo'] = {}
        diffed_smck_info2['SMCK_CONFIG']['SliceType'] = local_smck_info['SMCK_CONFIG']['SliceType']
        diffed_smck_info2['SMCK_CONFIG']['MonitCategoryName'] = local_smck_info['SMCK_CONFIG']['MonitCategoryName']
        if not diffed_smck_info2['SMCK_CONFIG'].__contains__('MonitCategoryInfo'):
            diffed_smck_info2['SMCK_CONFIG']['MonitCategoryInfo'] = {}
        diffed_smck_info2['SMCK_CONFIG_HASH'] = self.ms.__get_config_hash__(diffed_smck_info2['SMCK_CONFIG'])
        diffed_smck_info2['SMCK_TOOL_TYPE'] = input1
        diffed_smck_info2['SMCK_SLICE_TYPE'] = local_smck_info['SMCK_SLICE_TYPE']
        diffed_smck_info2['SMCK_DEFAULT'] = 0
        diffed_smck_info2['SMCK_ATHENA_VERSION'] = self.current_athena_version
        diffed_smck_info2['SMCK_SVN_TAG'] = local_smck_info['SMCK_SVN_TAG']
        diffed_smck_info2['SMCK_CREATOR'] = self.ms.current_user
        diffed_smck_info2['SMCK_COMMENT'] = comment

        # if tool_value['SMCK_CONFIG']['ToolInfo'] and tool_value['SMCK_CONFIG']['MonitCategoryInfo'] are both empty, then we don't want to include this as a new SMCK
        if diffed_smck_info2['SMCK_CONFIG']['ToolInfo'] == {} and diffed_smck_info2['SMCK_CONFIG']['MonitCategoryInfo'] == {}:

            # info for user
            if print_output_here:
                print "No local differences have been found with respect to the default SMCK (SMCK_ID="+str(default_smck_info['SMCK_ID'])+") for this tool ("+str(input1)+"), for this Athena version ("+self.current_athena_version+")."
                print "Nothing shall be uploaded to the Oracle database as a result."
            return

        # upload smck_info (diffed_smck_info2)
        new_smck_id = self.ms.oi.upload_smck(diffed_smck_info2)

        # info for user
        if print_output_here:

            # print new smck_id and smck_tool_patch_version
            print "This is SMCK (SMCK_ID)",new_smck_id
            print "with SMCK_TOOL_PATCH_VERSION",diffed_smck_info2['SMCK_TOOL_PATCH_VERSION']

        # if we are running silently, still return the smck_id and smck_tool_patch_version 
        # (ie. in case this function has been called by another function, which might like to know the smck_id and smck_tool_patch_version)
        else:
            return new_smck_id, diffed_smck_info2['SMCK_TOOL_PATCH_VERSION']
                

    def __ask_for_default__(self):
        """If running interactively, ask user whether this upload is to be a default for an Athena release, or a patch."""
        
        # is this session interactive? If not, return "ALL"
        if self.ms.__is_session_interactive__():

            # info for user
            print "Please specify whether this upload is to be a default for an Athena release, or a patch."
            print "valid inputs are 'default' or 'patch'."

            # now get user input
            user_input = raw_input("default or patch?: ").upper()

            # if input is blank, interpret this as 'PATCH'
            if user_input == "":
                user_input = "PATCH"

                # confirmation to user
                print "You have selected",user_input

            # valid input
            # if need be, then this list can be extended at a later date
            valid_input = ['DEFAULT','PATCH']

            # check for valid input
            # if input is not valid, ask for it again
            while not valid_input.__contains__(user_input):

                # warning to user that input was not understood
                print "The input",user_input,"was not recognised. Please specify a valid option."

                # get user input
                user_input = raw_input("default or patch?: ").upper()

                # if input is blank, interpret this as 'PATCH'
                if user_input == "":
                    user_input = "PATCH"

                    # confirmation to user
                    print "You have selected",user_input

            # return selected user choice
            if user_input == 'DEFAULT':
                return 1
            else:
                return 0

        else:

            # this is not an interactive session, so we can not ask for input
            return 0


    def __print_two_dicts__(self,dict1={},dict2={},tab_space="",input1="",input2=""):
        """Print two dictionaries in a neatly formatted manner.
        Designed to be used with diff."""

        # make sure inputs are strings
        input1 = str(input1)
        input2 = str(input2)

        # make input names of equal length, for nice printout spacing
        # if the inputs are already of the same length
        if len(input1) == len(input2):

            # set the names to print
            name1 = input1
            name2 = input2

        # else if input 1 is longer than input 2
        elif len(input1) > len(input2):

            # set the names to print
            # pad out the shorter one with spaces before it
            name1 = input1
            name2 = ((len(input1)-len(input2))*" ") + input2

        # else if input 2 is longer than input 1
        elif len(input1) < len(input2):

            # set the names to print
            # pad out the shorter one with spaces before it
            name1 = ((len(input2)-len(input1))*" ") + input1
            name2 = input2

        # loop over the keys (and values) in dict1
        for key, value1 in dict1.iteritems():

            # if this key is in the second dictionary
            if dict2.__contains__(key):

                # value from dict2
                value2 = dict2[key]

                # if the value is a dict
                if type(value1) is dict:

                    # first print this dict key
                    print tab_space+name1+"['"+key+"']:"
                    print tab_space+name2+"['"+key+"']:"

                    # recursively call this function
                    # add some space to tab_space, to indent the sub-dictionary
                    self.__print_two_dicts__(value1,value2,"   "+tab_space,name1,name2)

                # if the value is a list
                elif type(value1) is list:

                    # print the items nicely (no unicode u' features)
                    print tab_space+name1+"['"+key+"'] = [",
                    for n, item in enumerate(value1):
                        if type(item) is unicode or type(item) is str:
                            print "'"+item+"'",
                        else:
                            print item,
                        if n != len(value1)-1:
                            print ",",
                    print "]"
                    print tab_space+name2+"['"+key+"'] = [",
                    for n, item in enumerate(value2):
                        if type(item) is unicode or type(item) is str:
                            print "'"+item+"'",
                        else:
                            print item,
                        if n != len(value2)-1:
                            print ",",
                    print "]"

                # else if this value is not a dict or a list, then we should print
                else:
                    print tab_space+name1+"['"+key+"'] =",value1
                    print tab_space+name2+"['"+key+"'] =",value2

            # else if this key is only in dict1, then we print it anyway
            else:

                # if this is a dict
                if type(value1) is dict:

                    # then use ms.__print_one_dict__ to print it
                    self.ms.__print_one_dict__(value1,tab_space,name1)

                # else just print it
                else:
                    print tab_space+name1+"['"+key+"'] =",value1

        # loop over the keys (and values) in dict2
        for key, value2 in dict2.iteritems():

            # if this key is not in the first dictionary
            if not dict1.__contains__(key):

                # if this is a dict
                if type(value2) is dict:

                    # then use ms.__print_one_dict__ to print it
                    self.ms.__print_one_dict__(value2,tab_space,name2)

                # else just print it
                else:
                    print tab_space+name2+"['"+key+"'] =",value2


    def __is_input_a_local_tool__(self,input1=""):
        "Is input1 a locally running trigger-monitoring tool that has been read by the Tool Interrogator."

        # is the input a key in self.ms.local_global_info['MONITORING_TOOL_DICT']
        return self.ms.local_global_info['MONITORING_TOOL_DICT'].keys().__contains__(input1)


    def __is_input_an_mck__(self,input1=""):
        "Is input1 a valid MCK_ID."

        if self.ms.connected_to_oracle == False:
            print "You are not connected to the database, so this function is not available."
            return

        # if the input is blank, then no, it's not an mck
        if input1 == "":
            return False

        # search for this mck
        mck_info = self.ms.oi.read_mck_info_from_db(input1)

        # if mck_info == -1 then we've not found an mck, so return False
        if mck_info == -1:
            return False
        
        # else this is a valid mck
        else:
            return True


    def __is_input_an_smck__(self,input1="",print_output_here=""):
        "Is input1 a valid SMCK_ID or SMCK_TOOL_PATCH_VERSION."

        if self.ms.connected_to_oracle == False:
            print "You are not connected to the database, so this function is not available."
            return

        # check for empty print_output_here
        # if it is found, use self.print_output
        if print_output_here == "":
            print_output_here = self.print_output

        # if the input is blank, then no, it's not an smck
        if input1 == "":
            return False

        # search for this smck
        smck_id = self.ms.__get_smck_id_from_smck_identifier__(input1,print_output_here)

        # if smck_id == -1 then we've not found an smck, so return False
        if smck_id == -1:
            return False
        
        # else this is a valid smck
        else:
            return True


    def diff(self,input1="",flag1="",input2="",flag2="",diff_all=False,print_output_here=""):
        """Perform a diff between two inputs, and print the results.
        If diff_all==True, all items that are not identical in both inputs will be returned."
        If diff_all==False (default), only items that are in both inputs, and different, will be returned.
        The inputs specify what things are to be diffed.
        The flags specify what kind of inputs the inputs are."""

        if self.ms.connected_to_oracle == False and ( input1.upper() != "LOCAL" or input2.upper() != "LOCAL" ):
            print "You are not connected to the database, so this function is only available for use on local tools."
            return

        # check for empty print_output_here
        # if it is found, use self.print_output
        if print_output_here == "":
            print_output_here = self.print_output

        #info for user
        if print_output_here:
            if self.firstdiff:
                print ""
                print "diff takes four arguments: input1, flag1, input2, flag2."
                print "An optional fifth argument can be provided, either True or False (False by default)."
                print "If True, all items that are not identical in both inputs will be returned."
                print "If False (default), only items that are in both inputs, and different, will be returned."
                print ""
                self.firstdiff = False

        # input flags must be specified
        # acceptable tags are SMCK and MCK

        # valid flags
        valid_flags = ['MCK','SMCK']

        # if any of the inputs or flags are missing
        if input1 == "" or flag1 == "" or input2 == "" or flag2 == "":

            #info for user
            if print_output_here:
                print "diff takes four arguments: input1, flag1, input2, flag2."
                print "The inputs specify what things are to be diffed."
                print "The flags specify what the inputs are. Recognised flags are MCK and SMCK"
                print "Locally running tools can be specified with their tool name, and the flag SMCK."
                print "All locally running tools can be specified together with the input LOCAL, and the flag MCK."
                print "Valid arguments therefore include:"
                print "1,'MCK','LOCAL','MCK'"
                print "1,'MCK',2,'MCK'"
                print "1,'MCK','HLTMuonMon','SMCK'"
                print "17,'SMCK','HLTMuonMon','SMCK'"

            # if input1 has not been provided
            if input1 == "":

                #info for user
                if print_output_here:
                    print "The first input has not been provided. Please provide the first input."

            # if flag1 has not been provided
            if flag1 == "":

                #info for user
                if print_output_here:
                    print "The first flag has not been provided. Please provide the first flag. Recognised flags are MCK and SMCK"

            # if input2 has not been provided
            if input2 == "":

                #info for user
                if print_output_here:
                    print "The second input has not been provided. Please provide the second input."

            # if flag2 has not been provided
            if flag2 == "":

                #info for user
                if print_output_here:
                    print "The second flag has not been provided. Please provide the second flag. Recognised flags are MCK and SMCK"

            # since one or more inputs are missing, we exit
            return

        # if flag 1 is not a string
        if type(flag1) is not str:

            #info for user
            if print_output_here:
                print "The flag '"+flag1+"' is not a string. Recognised flags are MCK and SMCK"
            return

        # if input 1 does not have a valid flag
        if not flag1.upper() in valid_flags:

            #info for user
            if print_output_here:
                print "The flag '"+flag1+"' is not a recognised flag. Recognised flags are MCK and SMCK"
            return

        # check if input 1 is a valid mck
        if flag1 == "MCK":

            # if it is not a valid mck or local
            is_1_local = False
            if type(input1) is str:
                if input1.upper() == "LOCAL":
                    is_1_local = True
            if is_1_local == False and not self.__is_input_an_mck__(input1):

                #info for user
                if print_output_here:
                    print input1,"is not a valid MCK, or 'LOCAL' has not been selected."
                return

        # check if input 1 is a valid smck
        elif flag1 == "SMCK":

            # if it is not a valid smck or local tool
            if not self.__is_input_an_smck__(input1,False) and not self.__is_input_a_local_tool__(input1):

                #info for user
                if print_output_here:
                    print input1,"is not a valid SMCK or local tool."
                return

        # if flag 2 is not a string
        if type(flag2) is not str:

            #info for user
            if print_output_here:
                print "The flag '"+flag2+"' is not a string. Recognised flags are MCK and SMCK"
            return

        # if input 2 does not have a valid flag
        if not flag2.upper() in valid_flags:

            #info for user
            if print_output_here:
                print "The flag '"+flag2+"' is not a recognised flag. Recognised flags are MCK and SMCK"
            return

        # check if input 2 is a valid mck
        if flag2 == "MCK":

            # if it is not a valid mck or local
            is_2_local = False
            if type(input2) is str:
                if input2.upper() == "LOCAL":
                    is_2_local = True
            if is_2_local == False and not self.__is_input_an_mck__(input2):

                #info for user
                if print_output_here:
                    print input2,"is not a valid MCK, or 'LOCAL' has not been selected."
                return

        # check if input 2 is a valid smck
        elif flag2 == "SMCK":

            # if it is not a valid smck or local tool
            if not self.__is_input_an_smck__(input2,False) and not self.__is_input_a_local_tool__(input2):

                #info for user
                if print_output_here:
                    print input2,"is not a valid SMCK or local tool."
                return

        # if we've reached this far then the inputs have been parsed
        # now we deal with the combinatorics of the options

        # these are the dicts we will diff
        diff_input1_dict = {}
        diff_input2_dict = {}

        # if input 1 is an mck
        if flag1 == "MCK":

            # if input 1 is local
            if is_1_local:

                # then use currently stored local global info
                diff_input1_dict = self.ms.local_global_info

            # else input 1 is an mck
            # (we know this because we checked if input 1 was a valid mck or 'local' above)
            else:

                # get the global info from the database
                diff_input1_dict = self.ms.get_global_info_from_db(input1)

        # else input 1 is an smck
        elif flag1 == "SMCK":

            # we must either get the smck from the database, or locally if it is a local tool
            # (it will be one or the other, as we have confirmed above)
            # we then need to construct a temporary global info for the diff

            # construct the temporary global info for the diff
            diff_input1_dict['MONITORING_TOOL_DICT'] = {}

            # is input 1 a local tool
            if self.__is_input_a_local_tool__(input1):

                # set the tool name
                tool_name1 = input1

                # get the tool from the locally stored config
                diff_input1_dict['MONITORING_TOOL_DICT'][tool_name1] = self.ms.local_global_info['MONITORING_TOOL_DICT'][tool_name1]

            # else the input is an smck in the database
            else:

                # get the tool from the database
                smck_info1 = self.ms.get_smck_info_from_db(input1)

                # set the tool name
                tool_name1 = smck_info1['SMCK_TOOL_TYPE']

                # get the tool from the database
                diff_input1_dict['MONITORING_TOOL_DICT'][tool_name1] = smck_info1

        # if input 2 is an mck
        if flag2 == "MCK":

            # if input 2 is local
            if is_2_local:

                # then use currently stored local global info
                diff_input2_dict = self.ms.local_global_info

            # else input 2 is an mck
            # (we know this because we checked if input 2 was a valid mck or 'local' above)
            else:

                # get the global info from the database
                diff_input2_dict = self.ms.get_global_info_from_db(input2)

        # else input 2 is an smck
        elif flag2 == "SMCK":

            # we must either get the smck from the database, or locally if it is a local tool
            # (it will be one or the other, as we have confirmed above)
            # we then need to construct a temporary global info for the diff

            # construct the temporary global info for the diff
            diff_input2_dict['MONITORING_TOOL_DICT'] = {}

            # is input 2 a local tool
            if self.__is_input_a_local_tool__(input2):

                # set the tool name
                tool_name2 = input2

                # get the tool from the locally stored config
                diff_input2_dict['MONITORING_TOOL_DICT'][tool_name2] = self.ms.local_global_info['MONITORING_TOOL_DICT'][tool_name2]

            # else the input is an smck in the database
            else:

                # get the tool from the database
                smck_info2 = self.ms.get_smck_info_from_db(input2)

                # set the tool name
                tool_name2 = smck_info2['SMCK_TOOL_TYPE']

                # get the tool from the database
                diff_input2_dict['MONITORING_TOOL_DICT'][tool_name2] = smck_info2

        # we now have our dictionaries for the diff
        # lets do the diff
        if flag1 == "MCK" and flag2 == "MCK":
            diff_output1_dict, diff_output2_dict = self.ms.__calculate_diff__(diff_input1_dict,diff_input2_dict,diff_all)
        elif flag1 == "SMCK" and flag2 == "SMCK":
            diff_output1_dict, diff_output2_dict = self.ms.__calculate_diff__(diff_input1_dict['MONITORING_TOOL_DICT'][tool_name1],diff_input2_dict['MONITORING_TOOL_DICT'][tool_name2],diff_all)
        elif flag1 == "MCK" and flag2 == "SMCK":
            diff_output1_dict, diff_output2_dict = self.ms.__calculate_diff__(diff_input1_dict,diff_input2_dict,diff_all)
        elif flag1 == "SMCK" and flag2 == "MCK":
            diff_output1_dict, diff_output2_dict = self.ms.__calculate_diff__(diff_input1_dict,diff_input2_dict,diff_all)

        # now we need to print this in a neat manner
        if print_output_here:

            # recursively print the diffs
            self.__print_two_dicts__(diff_output1_dict,diff_output2_dict,"",input1,input2)

        # else we assume the user wants the diffs returned
        # (ie if this function has been called from within another function)
        else:

            # return the diffed dictionaries
            return diff_output1_dict, diff_output2_dict


    def print_database_schema(self):
        "Print the tables and entries in the current Oracle database."

        if self.ms.connected_to_oracle == False:
            print "You are not connected to the database, so this function is not available."
            return

        # get SQL table and column names
        database_column_list = self.ms.oi.get_db_tables_and_columns()

        # list of tables
        table_list = []

        # construct list of tables
        for line in database_column_list:

            # if this table is not in the list
            if not line['TABLE_NAME'].upper() in table_list:

                # add it to the list
                table_list.append(line['TABLE_NAME'].upper())

        # info for user
        print ""
        print "The Menu-aware Monitoring Oracle database currently has the following format:"
        print ""

        # for each table
        for table in table_list:

            # print the table name
            print "Table",table

            # loop over the database_column_list
            for column in database_column_list:

                # if this column is in this table
                if column['TABLE_NAME'].upper() == table:

                    # print this column info
                    print "     ",column['COLUMN_NAME']

            # nice spacing for user
            print ""


    def apply_mck(self,input1="",print_output_here=""):
        "Apply an MCK to locally running tools."

        if self.ms.connected_to_oracle == False:
            print "You are not connected to the database, so this function is not available."
            return

        # check for empty print_output_here
        # if it is found, use self.print_output
        if print_output_here == "":
            print_output_here = self.print_output

        # is the input a valid mck?
        if not self.__is_input_an_mck__(input1):

            # info for user
            if print_output_here:
                print "MCK",input1,"has not been recognised as a valid MCK."
            return

        # get list of smck_id that this mck links to
        smck_ids = self.ms.oi.read_mck_links_from_db(input1)

        # for each smck_id, apply the smck config
        for smck_id in smck_ids:

            # apply the smck config
            self.apply_smck(smck_id)


    def apply_smck(self,input1="",print_output_here=""):
        "Apply an SMCK to a locally running tool."

        if self.ms.connected_to_oracle == False:
            print "You are not connected to the database, so this function is not available."
            return

        # check for empty print_output_here
        # if it is found, use self.print_output
        if print_output_here == "":
            print_output_here = self.print_output

        # make sure we have the smck_id
        smck_id = self.ms.__get_smck_id_from_smck_identifier__(input1)
        
        # if we don't
        if smck_id == -1:

            # info for user
            if print_output_here:
                print "SMCK",input1,"has not been recognised as a valid SMCK."
            return

        # get the smck_info
        smck_info = self.ms.oi.read_smck_info_from_db(smck_id)

        # get the processing step this smck should be used for
        processing_step = smck_info['SMCK_PROCESSING_STEP']

        # are we running in an appropriate processing step?
        if not self.__is_input_a_valid_current_processing_step_to_use__(processing_step):

            # info for user
            if print_output_here:
                print "SMCK",input1,"is for the Athena processing stage '"+processing_step+"', which we are not currently in. This SMCK will therefore not be applied as a config patch at this time."
            return

        # get the processing stream this smck should be used for
        processing_stream = smck_info['SMCK_PROCESSING_STREAM']

        # are we running in an appropriate processing stream?
        if not self.__is_input_a_valid_current_processing_stream_to_use__(processing_stream):

            # info for user
            if print_output_here:
                print "SMCK",input1,"is for the Athena processing stream '"+processing_stream+"', which we are not currently using. This SMCK will therefore not be applied as a config patch at this time."
            return

        # get the ToolSvc_tool_name
        ToolSvc_tool_name = smck_info['SMCK_CONFIG']['ToolSvcName']

        # is the input a locally running tool
        tool_is_running = False
        tool_is_running = hasattr(ToolSvc,ToolSvc_tool_name)
        #exec "tool_is_running = hasattr(ToolSvc,%s)" % (ToolSvc_tool_name)
        if not tool_is_running:

            # info for user
            if print_output_here:
                print "SMCK",input1," corresponds to the tool",ToolSvc_tool_name,"which is not running locally, so can not be configured with this SMCK."
            return

        # get the patch config
        # make sure there is no unicode in it # TODO Fix code so that this is not needed
        patch_config = self.ms.oi.__unicode_to_str__(smck_info['SMCK_CONFIG']['ToolInfo'] )

        # now we apply the config patch of the smck
        # for each variable in the patch
        for tool_key, tool_value in patch_config.iteritems():

            # test if the tool has this variable
            tool_contains_variable = False
            exec "tool_contains_variable = hasattr(ToolSvc.%s,tool_key)" % (ToolSvc_tool_name)
            
            # if the tool has this variable
            if tool_contains_variable:

                # get the type of the value
                type_to_set_to = type
                exec "type_to_set_to = type(ToolSvc.%s.%s)" % (ToolSvc_tool_name,tool_key)

                # test that the value to be applied is of the same type as the existing value
                if type_to_set_to is type(tool_value):

                    # if this is a list, make it blank first
                    # this is to avoid overzealous Athena type limiting
                    # TODO Write this in a more generic way, ie. not just for lists
                    if type_to_set_to is list:
                        exec "ToolSvc.%s.%s = []" % (ToolSvc_tool_name,tool_key)

                    # apply the config for this variable
                    exec "ToolSvc.%s.%s = tool_value" % (ToolSvc_tool_name,tool_key)

                # if they are not the same type
                else:

                    # info for user
                    if print_output_here:
                        print "ToolSvc."+str(ToolSvc_tool_name)+"."+str(tool_key)+" is of type "+str(type_to_set_to)+", however in SMCK",input1,str(ToolSvc_tool_name)+"."+str(tool_key),"is of type",type(tool_value)
                        #print "Athena will only allow ToolSvc."+str(ToolSvc_tool_name)+"."+str(tool_key)+" to be set to values of type",type_to_set_to
                        print "Attempting conversion...",

                    # get the type_to_set_to as a str
                    type_to_set_to_str = str(type_to_set_to).split("'")[1]

                    # try to convert
                    try:

                        # try the conversion
                        exec "new_value = %s(tool_value)" % (type_to_set_to_str)

                        # if we've got this far then the conversion was a success
                        # info for user
                        if print_output_here:
                            print "conversion successful.",

                        # check that the values are equal
                        if new_value == tool_value:
                            
                            # apply the config for this variable
                            exec "ToolSvc.%s.%s = new_value" % (ToolSvc_tool_name,tool_key)

                            # nice formatting for user
                            if print_output_here:
                                print ""

                        # if the values are not equal
                        else:

                            # info for user
                            if print_output_here:
                                print "However the values do not match. This tool variable will not be set."
                                print "(",str(new_value),"!=",str(tool_value),")"

                    # if the conversion has failed
                    except:

                        # info for user
                        if print_output_here:
                            print "conversion failed. This tool variable will not be set."

        # get the MonitCategory patch
        monitCategory_patch_config = self.ms.oi.__unicode_to_str__(smck_info['SMCK_CONFIG']['MonitCategoryInfo'] )

        # if monitCategory_patch_config is not empty, then we must apply this patch
        if not monitCategory_patch_config == {}:

            # get the MonitCategory name
            monitCategory_name = self.ms.oi.__unicode_to_str__(smck_info['SMCK_CONFIG']['MonitCategoryName'] )

            # first we must import the relevant MonitCategory
            # we import it as monitCategory_object so that we have a handle on it
            monitCategory_object = ""
            exec "import %s as monitCategory_object" % (monitCategory_name)

            # we then need to apply each variable in monitCategory_patch_config to monitCategory_object
            for key, value in monitCategory_patch_config.iteritems():

                # test if the monitCategory_object has this variable
                monitCategory_object_contains_variable = False
                monitCategory_object_contains_variable = hasattr(monitCategory_object,key)
            
                # if the monitCategory_object has this variable
                if monitCategory_object_contains_variable:            

                    # get the type of the value
                    type_to_set_to = type
                    exec "type_to_set_to = type(monitCategory_object.%s)" % (key)

                    # test that the value to be applied is of the same type as the existing value
                    if type_to_set_to is type(value):

                        # if this is a list, make it blank first
                        # this is to avoid overzealous Athena type limiting
                        # TODO Write this in a more generic way, ie. not just for lists
                        if type_to_set_to is list:
                            exec "monitCategory_object.%s = []" % (key)

                        # apply the config for this variable
                        exec "monitCategory_object.%s = value" % (key)

                    # if they are not the same type
                    else:

                        # info for user
                        if print_output_here:
                            print str(monitCategory_name)+"."+str(key)+" is of type "+str(type_to_set_to)+", however in SMCK",input1,str(monitCategory_name)+"."+str(key),"is of type",type(value)
                            #print "Athena will only allow ToolSvc."+str(ToolSvc_tool_name)+"."+str(tool_key)+" to be set to values of type",type_to_set_to
                            print "Attempting conversion...",

                        # get the type_to_set_to as a str
                        type_to_set_to_str = str(type_to_set_to).split("'")[1]

                        # try to convert
                        try:

                            # try the conversion
                            exec "new_value = %s(tool_value)" % (type_to_set_to_str)

                            # if we've got this far then the conversion was a success
                            # info for user
                            if print_output_here:
                                print "conversion successful.",

                            # check that the values are equal
                            if new_value == tool_value:
                            
                                # apply the config for this variable
                                exec "ToolSvc.%s.%s = new_value" % (ToolSvc_tool_name,tool_key)

                                # nice formatting for user
                                if print_output_here:
                                    print ""

                            # if the values are not equal
                            else:

                                # info for user
                                if print_output_here:
                                    print "However the values do not match. This MonitCategory variable will not be set."
                                    print "(",str(new_value),"!=",str(tool_value),")"

                        # if the conversion has failed
                        except:

                            # info for user
                            if print_output_here:
                                print "conversion failed. This MonitCategory variable will not be set."

            # finally we should refresh HLTMonTriggerList
            self.__refresh_HLTMonTriggerList__()

        # info for user
        if print_output_here:
            print "SMCK",input1,"has been applied as a config patch to tool",ToolSvc_tool_name


    def __refresh_HLTMonTriggerList__(self):
        """If a slice MonitCategory has been updated,
        then this function can be used to update HLTMonTriggerList."""

        # import HLTMonTriggerList
        from TrigHLTMonitoring.HLTMonTriggerList import hltmonList

        # now we rerun hltmonList.config() to update it with all the latest info from all slices
        hltmonList.config()


    def get_mck_id_from_smk(self,input_smk):
        """Input an SMK, and get an MCK_ID back.
        If no MCK is found, -1 is returned.
        If an MCK of 0 is returned, this is intended to signify 
        that the default tool configurations should be used."""

        if self.ms.connected_to_oracle == False:
            print "You are not connected to the database, so this function is not available."
            return

        # get list of all mck_to_smk links
        mck_to_smk_links = []
        mck_to_smk_links = self.ms.oi.get_all_mck_to_smk_links()

        # loop over the list
        for link in mck_to_smk_links:

            # only consider active links
            if link['ACTIVE'] == '1':

                # the list should be ordered in smk, from largest to smallest
                # check if this link's smk is equal to or less than the input_smk
                if link['SMK'] <= input_smk:
            
                    # then this is the link we want, so return the mck
                    return link['MCK']

        # if we've made it this far, then an mck has not been found, so return -1
        return -1

    def dump_local_config_to_json(self,output_json_filename="mam_configs.json",processing_step="",processing_stream="",comment="",default="",print_output_here=""):
        "All locally read-in trigger monitoring tool configurations are output to a json file."
        
        # create a file-like-object to write the json to
        output_file = open( output_json_filename , "w" )

        # if no default option is provided, then ask for one
        if default=="":
            default = self.__ask_for_default__()

        # if this is a default...
        if default==1:

            # then set the processing_step and processing_stream accordingly
            processing_step="ALL"
            processing_stream="ALL"

        # else if it's a patch...
        else:

            # if no processing_step is provided, then ask for one
            if processing_step=="":
                processing_step = self.ms.__ask_for_processing_step__()

            # if no processing_step is provided, then ask for one
            if processing_stream=="":
                processing_stream = self.ms.__ask_for_processing_stream__()

        # if no comment is provided, then ask for one
        if comment=="":
            comment = self.ms.__ask_for_comment__()

        # fill extra mck_info
        self.ms.local_global_info['MCK'] = {}
        self.ms.local_global_info['MCK']['MCK_DEFAULT'] = default
        self.ms.local_global_info['MCK']['MCK_ATHENA_VERSION'] = self.current_athena_version
        self.ms.local_global_info['MCK']['MCK_CREATOR'] = self.ms.current_user
        self.ms.local_global_info['MCK']['MCK_COMMENT'] = comment

        # fill extra smck_info for all tools
        for tool_key, tool_value in self.ms.local_global_info['MONITORING_TOOL_DICT'].iteritems():

            # fill extra smck_info
            tool_value['SMCK_PROCESSING_STEP'] = processing_step
            tool_value['SMCK_PROCESSING_STREAM'] = processing_stream
            tool_value['SMCK_DEFAULT'] = default
            tool_value['SMCK_ATHENA_VERSION'] = self.current_athena_version
            tool_value['SMCK_CREATOR'] = self.ms.current_user
            tool_value['SMCK_COMMENT'] = comment

        # json encode the local global info, and dump it to the output file
        json.dump(self.ms.local_global_info, output_file, ensure_ascii=True, sort_keys=True) 

        # close the output file
        output_file.close()


    def diff_json_files(self,input_default_config_file="mam_default_configs.json",input_config_file="mam_configs.json",output_json_filename="mam_patch_configs.json"):
        """Input a default config, and an alternative config. A diff will be performed. 
        The parts of the alternative config that are different to the default config 
        will be returned as a new json file."""

        # open all file-like-objects
        input_default_file = open( input_default_config_file , "r" )
        input_alt_file = open( input_config_file , "r" )
        output_file = open( output_json_filename , "w" )

        # get the input dictionaries
        input_default_dict = self.ms.oi.__unicode_to_str__( json.load(input_default_file) )
        input_alt_dict = self.ms.oi.__unicode_to_str__( json.load(input_alt_file) )

        # perform a diff of these dicts
        # we want diffed_info2, 
        # which is the 'patch' to apply to the default to get the current local configuration
        diffed_info1, diffed_info2 = self.ms.__calculate_diff__(input_default_dict,input_alt_dict,False)

        # json encode the diffed 'patch' info, and dump it to the output file
        json.dump(diffed_info2, output_file, ensure_ascii=True, sort_keys=True)

        # close all files
        input_default_file.close()
        input_alt_file.close()
        output_file.close()
