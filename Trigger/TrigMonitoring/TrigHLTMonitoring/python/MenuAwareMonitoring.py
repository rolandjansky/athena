# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# Author: Ben Smart (bsmart@cern.ch)
#

import sys
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

        # MaM code version
        self.version = '1.2.3'

        # flag for setting whether to print out anything to screen or not
        self.print_output = True

        # flag so that diff instruction are only printed once
        self.firstdiff = True

        # create oracle interaction object
        self.oi = OracleInterface()

        # create tool interrogator object
        self.ti = ToolInterrogator()

        # holder for current local Athena version
        self.current_athena_version = ""
        self.__get_athena_version__()

        # holder for current user
        self.current_user = ""
        self.__get_current_user__()

        # holder for stream (bulk or express)
        self.stream = ""
        self.__get_stream__()

        # holder for default global_info
        self.default_global_info = {}

        # holder for current local global_info
        self.local_global_info = {}
        self.local_global_info['MONITORING_TOOL_DICT'] = {}

        # pointer to local tool info
        self.local = self.local_global_info['MONITORING_TOOL_DICT']

        # automatically fill current local tool info
        self.get_current_local_info()

        # flag to record if we have connected to Oracle
        self.connected_to_oracle = False

        # now connect to oracle
        self.__connect_to_oracle__(database_username,database_password,database_name,database_directory)

        # fill default global info (if available)
        self.get_default_from_db()

        # print guide for user if this is an interactive session
        if self.__is_session_interactive__():

            # print guide for user
            # TODO
            print ""
            print "Author of this code: Ben Smart (Ben.Smart@cern.ch)"
            print "This is Menu-aware monitoring version",self.version
            print "Running in Athena release",self.current_athena_version
            print "You are",self.current_user
            print "Stream detected:",self.stream
            print ""


    def __connect_to_oracle__(self,database_username="",database_password="",database_name="",database_directory=""):
        "Connect to the Oracle server."

        # if we are already connected
        if self.connected_to_oracle:

            # info for user
            print "We are already connected to the Oracle database"

        # else if we are not
        else:
        
            # info for user
            print "We are now connecting to the Oracle database"

            # try catch
            try:

                # connect to oracle
                self.oi.connect_to_oracle(database_username,database_password,database_name,database_directory)

                # flag to record that we have connected
                self.connected_to_oracle = True

            except:

                # info for user
                print "Error while connecting to Oracle database. Exiting."
            
                # exit, otherwise the program will crash later when Oracle database features are required
                sys.exit(1)


    def __disconnect_from_oracle__(self):
        "Disconnect from the Oracle server."

        # if we are connected to Oracle
        if self.connected_to_oracle:

            # info for user
            print "We are now disconnecting from the Oracle database"

            # disconnect from oracle
            self.oi.disconnect_from_oracle()


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
        # __ask_for_processing_stream__()
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


    def __get_current_user__(self):
        "Get the current user."

        # get the current user
        self.current_user = subprocess.check_output("echo $USER", shell=True).replace("\n","")


    def __get_athena_version__(self):
        "Get the current Athena version."

        # get the current local Athena version (there must be a better way!)
        self.current_athena_version = subprocess.check_output("echo $AtlasVersion", shell=True).replace("\n","")


    def __update_local_pointer__(self):
        """update self.local to point to self.local_global_info['MONITORING_TOOL_DICT']
        Needed if self.local_global_info has been overwritten."""

        self.local = self.local_global_info['MONITORING_TOOL_DICT']


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


    def __get_config_hash__(self,smck_config=""):
        "Return a sha512 hash of the input SMCK configuration."

        # if the input is not a string, make it one
        if type(smck_config) is not str:

            # use json to turn the config into a string
            smck_config_json = json.dumps(smck_config, ensure_ascii=True, sort_keys=True)

        # else if it is a string
        else:

            # use input string as string to hash
            print "__get_config_hash__ has been passed a string, rather than an smck_config dict. This is unexpected, so you are being notified in case something has gone wrong. If it has, please contact the author: Ben Smart bsmart@cern.ch"
            print "input =",smck_config
            smck_config_json = smck_config

        # calculate hash
        return_hash = hashlib.sha512(smck_config_json.encode()).hexdigest()

        # return hash
        return return_hash


    def list_local_tools_read_in(self):
        """List local running trigger-monitoring tools that have been discovered by the Tool Interrogator.
        These tool configurations are available in the <ThisVariable>.local dictionary."""

        # info for user
        print "The following local tools have had their configurations extracted by the ToolInterrogator:"

        # list the tools in self.local_global_info['MONITORING_TOOL_DICT']
        for tool in self.local_global_info['MONITORING_TOOL_DICT'].keys():
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
        # __ask_for_processing_stream__()
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


    def get_default_mck_id_from_db(self,input_athena_version=""):
        """Get the MCK number (MCK_ID) of the default for this Athena version.
        If input_athena_version=='', the current Athena version is used."""

        # if no input Athena version is provided, then use the current version
        if input_athena_version == "":
            input_athena_version = self.current_athena_version

        # search for default mck
        return self.oi.read_default_mck_id_from_db(input_athena_version)


    def get_default_from_db(self,input_athena_version="",print_output_here=""):
        """Prints default MCK number (MCK_ID) for an Athena version.
        If no Athena version is specified, the current Athena version being run in is used.
        All default information is made available in the <ThisVariable>.default_global_info dictionary."""

        # check for empty print_output_here
        # if it is found, use self.print_output
        if print_output_here == "":
            print_output_here = self.print_output

        # info for user
        if print_output_here:
            print "Attempting to get default tool configuration from database"

        # if no input Athena version is provided, then use the current version
        if input_athena_version == "":
            input_athena_version = self.current_athena_version

        # search for default mck
        default_mck = self.get_default_mck_id_from_db(input_athena_version)

        # if a valid default mck exists
        if default_mck >= 0:

            # info for user
            if print_output_here:
                print "Default mck for Athena version "+input_athena_version+" is",default_mck

            # fill self.default_global_info
            self.default_global_info = self.get_global_info_from_db(default_mck)

        # if there is no default for this Athena version
        else:

            # info for user
            if print_output_here:
                print "No default for Athena version "+self.current_athena_version+" has been uploaded"
                print "If you are not running with any local changes to the default, then consider running the command \"<ThisVariable>.upload_default()\""


    def get_global_info_from_db(self,mck_id):
        "For an input MCK number (MCK_ID), get all related MCK and SMCK info, and return it as a dictionary."

        # get mck_info for this mck
        global_info = {}
        global_info['MONITORING_TOOL_DICT'] = {}
        global_info['MCK'] = self.oi.read_mck_info_from_db(mck_id)

        # get mck_to_smck links (list of smck_id that this mck links to)
        smck_ids = self.oi.read_mck_links_from_db(mck_id)

        # loop over smck_ids and get smck_info
        for smck_id in smck_ids:

            # get smck_info
            smck_info = self.oi.read_smck_info_from_db(smck_id)

            # generate monitoring_tool_dict key, a combination of the tool name, 
            # the processing step (if not ALL),
            # and the processing stream (if not ALL)
            smck_key = smck_info['SMCK_TOOL_TYPE']
            if smck_info['SMCK_PROCESSING_STEP'] != "ALL":
                smck_key += "_"+smck_info['SMCK_PROCESSING_STEP']
            if smck_info['SMCK_PROCESSING_STREAM'] != "ALL":
                smck_key += "_"+smck_info['SMCK_PROCESSING_STREAM']

            # add this smck_info to monitoring_tool_dict
            global_info['MONITORING_TOOL_DICT'][smck_key] = smck_info

        # return global_info
        return global_info


    def __get_smck_id_from_smck_identifier__(self,smck_identifier,print_output_here=""):
        "Input can either be an SMCK_ID or an SMCK_TOOL_PATCH_VERSION. Output will be the the correct SMCK_ID."

        # check for empty print_output_here
        # if it is found, use self.print_output
        if print_output_here == "":
            print_output_here = self.print_output

        # test if smck_identifier can be an int (smck_id)
        try:

            # input could be 1.0 or 1, etc.
            smck_id = int(float(smck_identifier))

            # if we've got this far then we at least have an int
            # but is it a valid smck_id?
            smck_info = self.oi.read_smck_info_from_db(smck_id)

            # if smck_id is not valid, then smck_info will equal -1
            if smck_info == -1:

                # print warning and return -1, as the smck_identifier is not valid
                if print_output_here:
                    print "No SMCK found with",smck_identifier
                return -1

            # else if it has been found, then return smck_id as it is valid smck_id
            else:

                # return smck_id
                return smck_id

        # if smck_identifier is not a number
        except ValueError:

            # get smck_id
            smck_id = self.oi.get_smck_id_from_smck_tool_patch_version(smck_identifier)

            # if no smck_id is found
            if smck_id == -1:

                # print warning and return -1
                if print_output_here:
                    print "No SMCK found with",smck_identifier
                return -1

            # else if it has been found, then return smck_id
            else:

                # return smck_id
                return smck_id


    def get_smck_info_from_db(self,smck_identifier,print_output_here=""):
        "Get an SMCK configuration from the Oracle database."

        # check for empty print_output_here
        # if it is found, use self.print_output
        if print_output_here == "":
            print_output_here = self.print_output

        # translate smck_identifier into an smck_id
        smck_id = self.__get_smck_id_from_smck_identifier__(smck_identifier)

        # if an smck_id has not been found
        if smck_id == -1:

            # return -1, as no smck_info can be obtained
            return -1

        else:

            # get smck_info
            smck_info = self.oi.read_smck_info_from_db(smck_id)

            # return this smck_info
            return smck_info


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
            smck_info['SMCK_CONFIG_HASH'] = self.__get_config_hash__(smck_config)
            smck_info['SMCK_SLICE_TYPE'] = smck_config['SliceType']
            smck_info['SMCK_TOOL_TYPE'] = tool
            smck_info['SMCK_ATHENA_VERSION'] = self.current_athena_version
            smck_info['SMCK_SVN_TAG'] = self.__get_tag__(str(smck_config['PackageName']).split(".")[0])

            # add this info to the local_global_info
            self.local_global_info['MONITORING_TOOL_DICT'][tool] = smck_info

            # print info about this tool
            if print_output_here:
                print ""
                print "ToolSvc."+tool
                print "The extracted data of this tool is stored in <ThisVariable>.local['"+tool+"']"
                print "This can be passed to MaM methods with the string '"+tool+"'"

        # update self.local
        self.__update_local_pointer__()

        # add nice spacing if we have been printing tool info
        if len(mon_tools) > 0:
            if print_output_here:
                print ""
                print "The extracted data of all local trigger-monitoring tools is stored in <ThisVariable>.local"
                print "All local trigger-monitoring tools can be passed together as an 'MCK' to MaM diff and search methods with the string 'LOCAL'"
                print ""


    def setup_all_local_tools(self):
        "Setup all local trigger-monitoring tools and runs get_current_local_info() to read them in using the Tool Interrogator."

        # setup all local packages listed in PackagesToInterrogate via ToolInterrogator
        self.ti.load_all_tools()

        # we probably want to read in these tools with the Tool Interrogator
        self.get_current_local_info()


    def upload_default(self,comment="",athena_version="",user="",print_output_here=""):
        "Upload all current trigger-monitoring tool configurations as default for this Athena version."

        # check for empty print_output_here
        # if it is found, use self.print_output
        if print_output_here == "":
            print_output_here = self.print_output

        # check for empty athena_version
        # if it is found, use self.current_athena_version
        if athena_version == "":
            athena_version=self.current_athena_version

        # check for empty user
        # if it is found, use self.current_user
        if user == "":
            user=self.current_user

        # search for default mck
        default_mck = self.get_default_mck_id_from_db(athena_version)

        # if it already exists
        if default_mck >= 0:

            if print_output_here:
                print "There already exists a default mck for this Athena version:"
                print "Athena version: "+athena_version
                print "Default MCK:",default_mck

        else:

            # ensure that all tools are loaded
            #self.setup_all_local_tools()

            # get current local info
            # this is now done inside self.setup_all_local_tools() so no need to repeat it
            #self.get_current_local_info()

            # if no comment is provided, then ask for one
            if comment=="":
                comment = self.__ask_for_comment__()

            # fill mck_info
            self.local_global_info['MCK'] = {}
            self.local_global_info['MCK']['MCK_DEFAULT'] = 1
            self.local_global_info['MCK']['MCK_ATHENA_VERSION'] = athena_version
            self.local_global_info['MCK']['MCK_CREATOR'] = user
            self.local_global_info['MCK']['MCK_COMMENT'] = comment

            # update self.local
            self.__update_local_pointer__()

            # for each local tool
            for tool, smck_info in self.local_global_info['MONITORING_TOOL_DICT'].iteritems():

                # fill smck_info
                smck_info['SMCK_PROCESSING_STEP'] = "ALL"
                smck_info['SMCK_PROCESSING_STREAM'] = "ALL"
                smck_info['SMCK_DEFAULT'] = 1
                smck_info['SMCK_CREATOR'] = user
                smck_info['SMCK_COMMENT'] = comment

            # upload this self.local_global_info to the database, and get the new mck_id and smck_ids
            mck_id, smck_ids = self.oi.upload_mck_and_smck(self.local_global_info)

            # info for user
            if print_output_here:
                print "The default for this Athena version ("+athena_version+") has been uploaded"
                print "It has been given the MCK",mck_id
                print ""
                print "The following tools have had their current configurations uploaded as defaults:"
                print ""
                for smck_id in smck_ids:
                    smck_info = self.oi.read_smck_info_from_db(smck_id)
                    print "Tool:",smck_info['SMCK_TOOL_TYPE']
                    print "SMCK:",smck_id
                    print "SMCK tool patch version:",smck_info['SMCK_TOOL_PATCH_VERSION']
                    print ""


    def upload_smck(self,input1="",processing_step="",comment="",print_output_here=""):
        """Upload local configuration for tool 'input1' as an SMCK.
        If input1=='local', then all local configuration changes wrt the default will be uploaded.
        Optional processing step and comment can be provided."""

        # check for empty print_output_here
        # if it is found, use self.print_output
        if print_output_here == "":
            print_output_here = self.print_output

        # search for default mck
        default_mck = self.get_default_mck_id_from_db()

        # if the default does not exist
        if default_mck < 0:

            # info for user
            if print_output_here:
                print "No default for this Athena version ("+self.current_athena_version+") has been uploaded"
                print "If you are not running with any local changes to the default, then consider running the command \"<ThisVariable>.upload_default()\""
            return

        # if input is local, then run upload_all_local_changes_as_smck()
        if input1 == 'local':

            if print_output_here:
                print "You have provided the input 'local'. All local changes wrt the default will be uploaded."
            self.upload_all_local_changes_as_smck(processing_step,comment)

        # get all local tool info 
        #self.get_current_local_info()

        # check if input1 is running (has been read in by the above line)
        if not self.local_global_info['MONITORING_TOOL_DICT'].__contains__(input1):

            # this tool has not been loaded
            # exit and suggest to the user how to start the tool, if they so wish
            if print_output_here:
                print "Tool",input1,"is not currently set up locally, so can not have its local configuration uploaded as an SMCK."
                print "To list all local tools currently set up and read in, please run \"<ThisVariable>.list_local_tools_read_in()\""
                print "To set up and read in all trigger monitoring tools locally, please run \"<ThisVariable>.setup_all_local_tools()\""
            return

        # get local smck_info for tool input1
        local_smck_info = self.local_global_info['MONITORING_TOOL_DICT'][input1]

        # get default from database
        # (should already have been done during __init__, 
        # but in case the default has only been uploaded in this session then we check again)
        self.get_default_from_db()

        # get default smck_info
        default_smck_info = self.default_global_info['MONITORING_TOOL_DICT'][input1]
        
        # create diff of smck_info
        # we want diffed_smck_info2, 
        # which is the 'patch' to apply to the default to get the current local configuration
        diffed_smck_info1, diffed_smck_info2 = self.__calculate_diff__(default_smck_info,local_smck_info,False)

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
            processing_step = self.__ask_for_processing_step__()

        # if no processing_stream is provided, then ask for one
        if processing_stream=="":
            processing_stream = self.__ask_for_processing_stream__()

        # if no comment is provided, then ask for one
        if comment=="":
            comment = self.__ask_for_comment__()

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
        diffed_smck_info2['SMCK_CONFIG_HASH'] = self.__get_config_hash__(diffed_smck_info2['SMCK_CONFIG'])
        diffed_smck_info2['SMCK_TOOL_TYPE'] = input1
        diffed_smck_info2['SMCK_SLICE_TYPE'] = local_smck_info['SMCK_SLICE_TYPE']
        diffed_smck_info2['SMCK_DEFAULT'] = 0
        diffed_smck_info2['SMCK_ATHENA_VERSION'] = self.current_athena_version
        diffed_smck_info2['SMCK_SVN_TAG'] = local_smck_info['SMCK_SVN_TAG']
        diffed_smck_info2['SMCK_CREATOR'] = self.current_user
        diffed_smck_info2['SMCK_COMMENT'] = comment

        # if tool_value['SMCK_CONFIG']['ToolInfo'] and tool_value['SMCK_CONFIG']['MonitCategoryInfo'] are both empty, then we don't want to include this as a new SMCK
        if diffed_smck_info2['SMCK_CONFIG']['ToolInfo'] == {} and diffed_smck_info2['SMCK_CONFIG']['MonitCategoryInfo'] == {}:

            # info for user
            if print_output_here:
                print "No local differences have been found with respect to the default SMCK (SMCK_ID="+str(default_smck_info['SMCK_ID'])+") for this tool ("+str(input1)+"), for this Athena version ("+self.current_athena_version+")."
                print "Nothing shall be uploaded to the Oracle database as a result."
            return

        # upload smck_info (diffed_smck_info2)
        new_smck_id = self.oi.upload_smck(diffed_smck_info2)

        # info for user
        if print_output_here:

            # print new smck_id and smck_tool_patch_version
            print "This is SMCK (SMCK_ID)",new_smck_id
            print "with SMCK_TOOL_PATCH_VERSION",diffed_smck_info2['SMCK_TOOL_PATCH_VERSION']

        # if we are running silently, still return the smck_id and smck_tool_patch_version 
        # (ie. in case this function has been called by another function, which might like to know the smck_id and smck_tool_patch_version)
        else:
            return new_smck_id, diffed_smck_info2['SMCK_TOOL_PATCH_VERSION']


    def upload_all_local_changes_as_smck(self,processing_step="",processing_stream="",comment="",athena_version="",user="",print_output_here=""):
        """Upload all local configuration changes wrt the default.
        Optional processing step, stream, and comment can be provided."""

        # check for empty print_output_here
        # if it is found, use self.print_output
        if print_output_here == "":
            print_output_here = self.print_output

        # check for empty athena_version
        # if it is found, use self.current_athena_version
        if athena_version == "":
            athena_version=self.current_athena_version

        # check for empty user
        # if it is found, use self.current_user
        if user == "":
            user=self.current_user

        # search for default mck
        default_mck = self.get_default_mck_id_from_db(athena_version)

        # if the default does not exist
        if default_mck < 0:

            # info for user
            if print_output_here:
                print "No default for Athena version "+athena_version+" has been uploaded"
                print "If you are not running with any local changes to the default, then consider running the command \"<ThisVariable>.upload_default()\""
            return

        # get all local tool info 
        #self.get_current_local_info()

        # if no tools are running locally
        if len(self.local_global_info['MONITORING_TOOL_DICT']) == 0:

            # exit and suggest to the user how to start the tools, if they so wish
            if print_output_here:
                print "No trigger monitoring tools are currently set up locally and read in, so they can not have their local configurations uploaded as SMCK."
                print "To set up and read in all trigger monitoring tools locally, please run \"<ThisVariable>.setup_all_local_tools()\""
                print "To read in all local tools currently set up, please run \"<ThisVariable>.get_current_local_info()\""
                print "To list all local tools currently set up and read in, please run \"<ThisVariable>.list_local_tools_read_in()\""

            return

        # get default from database
        # (should already have been done during __init__, 
        # but in case the default has only been uploaded in this session then we check again)
        self.get_default_from_db(athena_version)

        # create diff of global_info
        # we want diffed_global_info2, 
        # which is the 'patch' to apply to the default to get the current local configuration
        diffed_global_info1, diffed_global_info2 = self.__calculate_diff__(self.default_global_info,self.local_global_info,False)

        # if there are no local differences wrt the default, then we upload nothing and exit
        if diffed_global_info2 == {}:
            # info for user
            if print_output_here:
                print "No local differences have been found with respect to the default MCK ("+str(default_mck)+") for Athena version "+athena_version+"."
                print "Nothing shall be uploaded to the Oracle database as a result."
            return

        # if no processing_step is provided, then ask for one
        if processing_step=="":
            processing_step = self.__ask_for_processing_step__()

        # if no processing_step is provided, then ask for one
        if processing_stream=="":
            processing_stream = self.__ask_for_processing_stream__()

        # if no comment is provided, then ask for one
        if comment=="":
            comment = self.__ask_for_comment__()

        # fill extra mck_info
        diffed_global_info2['MCK'] = {}
        diffed_global_info2['MCK']['MCK_DEFAULT'] = 0
        diffed_global_info2['MCK']['MCK_ATHENA_VERSION'] = athena_version
        diffed_global_info2['MCK']['MCK_CREATOR'] = user
        diffed_global_info2['MCK']['MCK_COMMENT'] = comment

        # in case we want to remove any diffed_global_info2['MONITORING_TOOL_DICT'] items, we must make a list of the keys, 
        # and then delete these keys after we have finished iterating over diffed_global_info2['MONITORING_TOOL_DICT']
        # It is not possible to delete elements while iterating over a list or dict
        keys_to_delete = []
        
        # fill extra smck_info for all tools
        for tool_key, tool_value in diffed_global_info2['MONITORING_TOOL_DICT'].iteritems():

            # fill extra smck_info
            tool_value['SMCK_PROCESSING_STEP'] = processing_step
            tool_value['SMCK_PROCESSING_STREAM'] = processing_stream
            if not tool_value.__contains__('SMCK_CONFIG'):
                tool_value['SMCK_CONFIG'] = {}
            tool_value['SMCK_CONFIG']['PackageName'] = self.local_global_info['MONITORING_TOOL_DICT'][tool_key]['SMCK_CONFIG']['PackageName']
            tool_value['SMCK_CONFIG']['ToolName'] = self.local_global_info['MONITORING_TOOL_DICT'][tool_key]['SMCK_CONFIG']['ToolName']
            tool_value['SMCK_CONFIG']['ToolSvcName'] = self.local_global_info['MONITORING_TOOL_DICT'][tool_key]['SMCK_CONFIG']['ToolSvcName']
            if not tool_value['SMCK_CONFIG'].__contains__('ToolInfo'):
                tool_value['SMCK_CONFIG']['ToolInfo'] = {}
            tool_value['SMCK_CONFIG']['SliceType'] = self.local_global_info['MONITORING_TOOL_DICT'][tool_key]['SMCK_CONFIG']['SliceType']
            tool_value['SMCK_CONFIG']['MonitCategoryName'] = self.local_global_info['MONITORING_TOOL_DICT'][tool_key]['SMCK_CONFIG']['MonitCategoryName']
            if not tool_value['SMCK_CONFIG'].__contains__('MonitCategoryInfo'):
                tool_value['SMCK_CONFIG']['MonitCategoryInfo'] = {}
            tool_value['SMCK_CONFIG_HASH'] = self.__get_config_hash__(tool_value['SMCK_CONFIG'])
            tool_value['SMCK_TOOL_TYPE'] = tool_key
            tool_value['SMCK_SLICE_TYPE'] = self.local_global_info['MONITORING_TOOL_DICT'][tool_key]['SMCK_SLICE_TYPE']
            tool_value['SMCK_DEFAULT'] = 0
            tool_value['SMCK_ATHENA_VERSION'] = athena_version
            tool_value['SMCK_SVN_TAG'] = self.local_global_info['MONITORING_TOOL_DICT'][tool_key]['SMCK_SVN_TAG']
            tool_value['SMCK_CREATOR'] = user
            tool_value['SMCK_COMMENT'] = comment

            # if tool_value['SMCK_CONFIG']['ToolInfo'] and tool_value['SMCK_CONFIG']['MonitCategoryInfo'] are both empty, then we don't want to include this as a new SMCK
            if tool_value['SMCK_CONFIG']['ToolInfo'] == {} and tool_value['SMCK_CONFIG']['MonitCategoryInfo'] == {}:
                keys_to_delete.append(tool_key)

        # if there are any items in keys_to_delete to be deleted from diffed_global_info2['MONITORING_TOOL_DICT'] then delete them now
        for tool_key in keys_to_delete:
            diffed_global_info2['MONITORING_TOOL_DICT'].__delitem__(tool_key)

        # if there are no items in diffed_global_info2['MONITORING_TOOL_DICT'] then we do not want to upload anything
        if len(diffed_global_info2['MONITORING_TOOL_DICT']) == 0:

            # info for user
            if print_output_here:
                print "No local differences have been found with respect to the default MCK ("+str(default_mck)+") for Athena version "+athena_version+"."
                print "Nothing shall be uploaded to the Oracle database as a result."
            return

        # upload global_info (diffed_global_info2)
        new_mck_id, new_smck_ids = self.oi.upload_mck_and_smck(diffed_global_info2)

        # info for user
        if print_output_here:
            
            # print smck info
            print "The following SMCK have been created:"
            print ""

            # loop over filled dictionary diffed_global_info2['MONITORING_TOOL_DICT'] as it contains all the new_smck_ids but also the new smck_tool_patch_version
            # Edit: no, it seems not to. Where did the pointers break?
            #for tool_key, tool_value in diffed_global_info2['MONITORING_TOOL_DICT'].iteritems():
            for smck_id in new_smck_ids:

                # get smck_info
                tool_value = self.get_smck_info_from_db(smck_id)

                # print info for this smck
                print "SMCK_ID =",tool_value['SMCK_ID']
                print "SMCK_TOOL_PATCH_VERSION =",tool_value['SMCK_TOOL_PATCH_VERSION']

            # also print mck info
            print ""
            print "For convenience, an MCK has also been created automatically, linking to the above SMCK."
            print "MCK_ID =",new_mck_id
            print ""

        # if we are running silently, still return the new_mck_id and new_smck_ids
        # (ie. in case this function has been called by another function, which might like to know the new_mck_id and new_smck_ids)
        else:

            return new_mck_id, new_smck_ids


    def upload_mck(self,input_smck_list=[],comment="",print_output_here=""):
        """input_smck_list should be a list of SMCK, identified be either their SMCK_ID or SMCK_TOOL_PATCH_VERSION.
        An MCK will be uploaded, linking to these SMCK.
        Optional comment can be provided."""

        # check for empty print_output_here
        # if it is found, use self.print_output
        if print_output_here == "":
            print_output_here = self.print_output

        # if the input is empty, then we exit
        if input_smck_list == []:

            # info for user
            if print_output_here:
                print "No list of SMCK has been provided."
                print "No MCK upload is possible without a list of SMCK."
            return

        # holder for smck_ids
        smck_ids = []

        # loop over input
        for smck_identifier in input_smck_list:

            # get smck_id
            smck_id = self.__get_smck_id_from_smck_identifier__(smck_identifier)
            
            # if an smck_id has not been found
            if smck_id == -1:

                # info for user
                if print_output_here:
                    print "Problem with requested SMCK",smck_identifier
            
            # add this smck_id to the smck_ids list
            smck_ids.append(smck_id)


        # check if all smck_id have been found
        # TODO - CHECK THAT THE -1 CRITERIA IS CORRECT (ie do we need '-1' instead? I don't think so)
        if smck_ids.__contains__(-1):

            # one or more smck are missing. Abort mck upload
            if print_output_here:
                print "One or more of the SMCK requested are missing. Aborting MCK upload."
            return

        # check that the mck does not already exist
        mck_id = self.oi.check_if_exact_mck_exists(smck_ids)

        # if mck does already exist, then say so and exit
        if mck_id != -1:

            # info for user
            if print_output_here:
                print "This MCK already exists. It is MCK",mck_id
            return
        
        # if no comment is provided, then ask for one
        if comment=="":
            comment = self.__ask_for_comment__()

        # fill mck_info
        mck_info = {}
        mck_info['MCK_DEFAULT'] = 0
        mck_info['MCK_ATHENA_VERSION'] = self.current_athena_version
        mck_info['MCK_CREATOR'] = self.current_user
        mck_info['MCK_COMMENT'] = comment

        # upload mck_info
        mck_id = self.oi.upload_mck(mck_info)

        # link this mck to the smck_ids
        for smck_id in smck_ids:

            # upload this link
            self.oi.upload_mck_smck_link(mck_id,smck_id)

        # info for user
        if print_output_here:
            print "This MCK has been uploaded. It is MCK",mck_id


    def __ask_for_comment__(self):
        "If running interactively, ask user for an upload comment."

        # is this session interactive? If not, return ""
        if self.__is_session_interactive__():

            # info for user
            print "Please enter a comment to be attached to this upload to the database."
            print "The comment is limited in length to 1000 characters"

            # now get user input
            user_input = raw_input("comment: ")

            # check that comment length is valid
            while len(user_input) > 1000:

                # warn user that their input is too long, and give them the change to re-enter a comment
                print "Your comment was too long, (it was",len(user_input),"characters long). Please enter a shorter comment."
                
                # get user input
                user_input = raw_input("comment: ")

            # return valid comment
            return user_input

        else:

            # this is not an interactive session, so we can not ask for input
            return ""


    def __ask_for_default__(self):
        """If running interactively, ask user whether this upload is to be a default for an Athena release, or a patch."""
        
        # is this session interactive? If not, return "ALL"
        if self.__is_session_interactive__():

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


    def __ask_for_processing_step__(self):
        """If running interactively, ask user for the processing step(s) an upload will be valid for.
        'ALL' is the default."""
        
        # is this session interactive? If not, return "ALL"
        if self.__is_session_interactive__():

            # info for user
            print "Please specify which processing step(s) you wish this input to be for."
            print "The default is 'ALL' for all processing steps."
            print "Valid options are 'ESD', 'AOD', and 'ALL'."
            print "Hit <enter> without any input to select the default option (ALL)"

            # now get user input
            user_input = raw_input("processing step: ").upper()

            # if input is blank, interpret this as 'ALL'
            if user_input == "":
                user_input = "ALL"

                # confirmation to user
                print "You have selected",user_input

            # valid input
            # if need be, then this list can be extended at a later date
            valid_input = ['ALL','ESD','AOD']

            # check for valid input
            # if input is not valid, ask for it again
            while not valid_input.__contains__(user_input):

                # warning to user that input was not understood
                print "The input",user_input,"was not recognised. Please specify a valid option."

                # get user input
                user_input = raw_input("processing step: ")

                # if input is blank, interpret this as 'ALL'
                if user_input == "":
                    user_input = "ALL"

                    # confirmation to user
                    print "You have selected",user_input

            # return selected processing step
            return user_input

        else:

            # this is not an interactive session, so we can not ask for input
            return "ALL"


    def __ask_for_processing_stream__(self):
        """If running interactively, ask user for the processing stream(s) an upload will be valid for.
        'ALL' is the default."""
        
        # is this session interactive? If not, return "ALL"
        if self.__is_session_interactive__():

            # info for user
            print "Please specify which processing stream(s) you wish this input to be for."
            print "The default is 'ALL' for all processing streams."
            print "Valid options are 'BULK', 'EXPRESS', and 'ALL'."
            print "Hit <enter> without any input to select the default option (ALL)"

            # now get user input
            user_input = raw_input("processing stream: ").upper()

            # if input is blank, interpret this as 'ALL'
            if user_input == "":
                user_input = "ALL"

                # confirmation to user
                print "You have selected",user_input

            # valid input
            # if need be, then this list can be extended at a later date
            valid_input = ['ALL','BULK','EXPRESS']

            # check for valid input
            # if input is not valid, ask for it again
            while not valid_input.__contains__(user_input):

                # warning to user that input was not understood
                print "The input",user_input,"was not recognised. Please specify a valid option."

                # get user input
                user_input = raw_input("processing stream: ")

                # if input is blank, interpret this as 'ALL'
                if user_input == "":
                    user_input = "ALL"

                    # confirmation to user
                    print "You have selected",user_input

            # return selected processing stream
            return user_input

        else:

            # this is not an interactive session, so we can not ask for input
            return "ALL"


    def __is_session_interactive__(self):
        "Is this an interactive Athena session."

        # check if this is an interactive session
        # (if -i was passed as a command-line argument)
        return sys.argv.__contains__('-i')


    def __print_one_dict__(self,dict1={},tab_space="",name1=""):
        "Print a dictionary in a neatly formatted manner."

        # loop over the keys (and values) in dict1
        for key, value1 in dict1.iteritems():

            # if the value is a dict
            if type(value1) is dict:

                # first print this dict key
                print tab_space+name1+"['"+key+"']:"

                # recursively call this function
                # add some space to tab_space, to indent the sub-dictionary
                self.__print_one_dict__(value1,"   "+tab_space,name1)

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

            # else if this value is not a dict or a list, then we should print
            else:
                print tab_space+name1+"['"+key+"'] =",value1


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

                    # then use __print_one_dict__ to print it
                    self.__print_one_dict__(value1,tab_space,name1)

                # else just print it
                else:
                    print tab_space+name1+"['"+key+"'] =",value1

        # loop over the keys (and values) in dict2
        for key, value2 in dict2.iteritems():

            # if this key is not in the first dictionary
            if not dict1.__contains__(key):

                # if this is a dict
                if type(value2) is dict:

                    # then use __print_one_dict__ to print it
                    self.__print_one_dict__(value2,tab_space,name2)

                # else just print it
                else:
                    print tab_space+name2+"['"+key+"'] =",value2


    def __calculate_diff__(self,dict1={},dict2={},diff_all=False):
        """Calculate and return a diff between two inputs.
        If diff_all==True, all items that are not identical in both inputs will be returned."
        If diff_all==False (default), only items that are in both inputs, and different, will be returned."""

        # only return keys that are in both dictionaries
        # for these keys, return two dictionaries, one of all the values in dict1, one of all the values in dict2

        # dictionaries for filling and returning
        return_dict1 = {}
        return_dict2 = {}

        # loop over the first dictionary
        for key, value1 in dict1.iteritems():
    
            # if this key is in the second dictionary
            if dict2.__contains__(key):

                # get the value in the second dictionary
                value2 = dict2[key]

                # first check if the values are equal 
                # (if they are equal, then we don't want to put anything for this key into the return dictionaries)
                if str(value1) != str(value2):

                    # are value1 and value2 dictionaries? if so, recursively call __calculate_diff__
                    if type(value1) is dict and type(value2) is dict:
                    
                        # recursively call __calculate_diff__ and fill return dictionaries
                        return_dict1_temp, return_dict2_temp = self.__calculate_diff__(value1,value2,diff_all)

                        # check that the output is not identical
                        # this catches the case when diff_all==False
                        # where str(value1) != str(value2) because at least one of the dicts contains a key that the other doesn't
                        # but all the matching keys have identical values
                        # which causes the return_dicts to be equal (blank)
                        if return_dict1_temp != return_dict2_temp:
                        
                            # fill return dictionaries
                            return_dict1[key] = return_dict1_temp
                            return_dict2[key] = return_dict2_temp

                    else:

                        # fill return dictionaries with the different values
                        return_dict1[key] = value1
                        return_dict2[key] = value2

            # else if this key is not in the second dictionary
            else:

                #if we are adding all differences to the return dicts
                if diff_all:

                    # then add this key and value to the first return dict
                    return_dict1[key] = value1

        #if we are adding all differences to the return dicts
        if diff_all:

            # loop over the second dictionary
            for key, value2 in dict2.iteritems():

                # if this key is not in the first dictionary
                if not dict1.__contains__(key):

                    # then add this key and value to the second return dict
                    return_dict2[key] = value2

        # return the dictionaries
        return return_dict1, return_dict2


    def __is_input_a_local_tool__(self,input1=""):
        "Is input1 a locally running trigger-monitoring tool that has been read by the Tool Interrogator."

        # is the input a key in self.local_global_info['MONITORING_TOOL_DICT']
        return self.local_global_info['MONITORING_TOOL_DICT'].keys().__contains__(input1)


    def __is_input_an_mck__(self,input1=""):
        "Is input1 a valid MCK_ID."

        # if the input is blank, then no, it's not an mck
        if input1 == "":
            return False

        # search for this mck
        mck_info = self.oi.read_mck_info_from_db(input1)

        # if mck_info == -1 then we've not found an mck, so return False
        if mck_info == -1:
            return False
        
        # else this is a valid mck
        else:
            return True


    def __is_input_an_smck__(self,input1="",print_output_here=""):
        "Is input1 a valid SMCK_ID or SMCK_TOOL_PATCH_VERSION."

        # check for empty print_output_here
        # if it is found, use self.print_output
        if print_output_here == "":
            print_output_here = self.print_output

        # if the input is blank, then no, it's not an smck
        if input1 == "":
            return False

        # search for this smck
        smck_id = self.__get_smck_id_from_smck_identifier__(input1,print_output_here)

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
                diff_input1_dict = self.local_global_info

            # else input 1 is an mck
            # (we know this because we checked if input 1 was a valid mck or 'local' above)
            else:

                # get the global info from the database
                diff_input1_dict = self.get_global_info_from_db(input1)

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
                diff_input1_dict['MONITORING_TOOL_DICT'][tool_name1] = self.local_global_info['MONITORING_TOOL_DICT'][tool_name1]

            # else the input is an smck in the database
            else:

                # get the tool from the database
                smck_info1 = self.get_smck_info_from_db(input1)

                # set the tool name
                tool_name1 = smck_info1['SMCK_TOOL_TYPE']

                # get the tool from the database
                diff_input1_dict['MONITORING_TOOL_DICT'][tool_name1] = smck_info1

        # if input 2 is an mck
        if flag2 == "MCK":

            # if input 2 is local
            if is_2_local:

                # then use currently stored local global info
                diff_input2_dict = self.local_global_info

            # else input 2 is an mck
            # (we know this because we checked if input 2 was a valid mck or 'local' above)
            else:

                # get the global info from the database
                diff_input2_dict = self.get_global_info_from_db(input2)

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
                diff_input2_dict['MONITORING_TOOL_DICT'][tool_name2] = self.local_global_info['MONITORING_TOOL_DICT'][tool_name2]

            # else the input is an smck in the database
            else:

                # get the tool from the database
                smck_info2 = self.get_smck_info_from_db(input2)

                # set the tool name
                tool_name2 = smck_info2['SMCK_TOOL_TYPE']

                # get the tool from the database
                diff_input2_dict['MONITORING_TOOL_DICT'][tool_name2] = smck_info2

        # we now have our dictionaries for the diff
        # lets do the diff
        if flag1 == "MCK" and flag2 == "MCK":
            diff_output1_dict, diff_output2_dict = self.__calculate_diff__(diff_input1_dict,diff_input2_dict,diff_all)
        elif flag1 == "SMCK" and flag2 == "SMCK":
            diff_output1_dict, diff_output2_dict = self.__calculate_diff__(diff_input1_dict['MONITORING_TOOL_DICT'][tool_name1],diff_input2_dict['MONITORING_TOOL_DICT'][tool_name2],diff_all)
        elif flag1 == "MCK" and flag2 == "SMCK":
            diff_output1_dict, diff_output2_dict = self.__calculate_diff__(diff_input1_dict,diff_input2_dict,diff_all)
        elif flag1 == "SMCK" and flag2 == "MCK":
            diff_output1_dict, diff_output2_dict = self.__calculate_diff__(diff_input1_dict,diff_input2_dict,diff_all)

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

        # get SQL table and column names
        database_column_list = self.oi.get_db_tables_and_columns()

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


    def search(self,input1="",flag1="",print_output_here=""):
        """Search the Oracle database for something.
        input1 is is what is to be searched for.
        flag1 specifies what kind of input input1 is."""

        # check for empty print_output_here
        # if it is found, use self.print_output
        if print_output_here == "":
            print_output_here = self.print_output

        # search for an input
        # there should be an input and a flag
        # the flag can be any one of the columns found in the SQL schema
        # the columns in the SQL schema are found using self.oi.get_db_tables_and_columns()
        # if the flag matches a column, then then that table is searched for rows where input_value=flag_column
        # such rows are then converted into dictionaries, one dict per row
        # these dicts are then printed using self.__print_one_dict__(row_dict,"",row_name)

        # get SQL table and column names
        database_column_list = self.oi.get_db_tables_and_columns()

        # if the input or flag are missing
        if input1 == "" or flag1 == "":

            #info for user
            if print_output_here:
                print "search takes two arguments: input, flag."
                print "The input is what is to be searched for."
                print "The flag specifies what the input is. Recognised flags are listed below"
                print "Valid arguments therefore include:"
                print "1,'MCK_ID'"
                print "17,'SMCK_ID'"
                print "'HLTMuonMon_1','SMCK_TOOL_PATCH_VERSION'"
                print "'HLTMuonMon','SMCK_TOOL_TYPE'"
                print "'bsmart','MCK_CREATOR'"
                print "'Muon','SMCK_SLICE_TYPE'"
                print ""
                print "Recognised flags are:"

                # print available columns
                for row in database_column_list:
                    
                    # print this column name, with a preceding -
                    print row['COLUMN_NAME']

                # nice spacing for the user
                print ""

            # if input1 has not been provided
            if input1 == "":

                #info for user
                if print_output_here:
                    print "The input has not been provided. Please provide the first input."

            # if flag1 has not been provided
            if flag1 == "":

                #info for user
                if print_output_here:
                    print "The flag has not been provided. Please provide the flag. Recognised flags are listed above"

            # if we do not have the input and the flag, then we can not search, and so must exit
            return

        # check if the flag is a valid column
        # flag to indicate that we've matched the flag to a column
        column_match = False

        # loop over columns
        for row in database_column_list:

            # check if this column matches
            if flag1.upper() == row['COLUMN_NAME']:

                # then extract the table name and column name
                table_name = row['TABLE_NAME']
                column_name = row['COLUMN_NAME']

                # and set column_match and break out of this loop
                column_match = True
                break

        # check that we have found the column
        if not column_match:

            #info for user
            if print_output_here:
                print "The flag '"+flag1+"' has not been recognised as a valid flag. Recognised flags are:"

                # print available columns
                for row in database_column_list:
                    
                    # print this column name, with a preceding -
                    print "-"+row['COLUMN_NAME']
                    
            # no valid flag (column) means we can not search, and so must exit
            return

        # if we've got to here then we should have a value input1, and a table_name and column_name
        # if we want to check for naughty input (SQL injection) in input1, then here is the place to do it

        # we can't search for CLOB objects, ie. smck_config
        # so if the user is attempting to search for smck_config, then warn them
        # the input they have provided could be hashed, and the hash could be searched for
        if flag1 == "smck_config":
            
            # info for the user
            if print_output_here:

                # explain problem
                print ""
                print "You have attempted to search for an SMCK_CONFIG."
                print "Unfortunately, due to limitations in Oracle, this is not possible."
                print "This is because the SMCK_CONFIG is stored as a CLOB, and in Oracle it is not possible to perform SQL query comparisons to CLOB objects, (WHERE SMCK_CONFIG = user_input)."

                # explain the alternative
                print ""
                print "To allow for SMCK_CONFIG comparisons, each config is hashed (using sha512) and hashes are compared."
                print "Your input will now be hashed, and that hash will be searched for."
                print "As this may not be exactly what you intended, do take care when interpreting these search results."
                print ""

                # convert the flag
                flag1 = 'smck_config_hash'

                # convert the input to a hash
                input1 = self.__get_config_hash__(input1)
                

        # now lets search
        search_results_list = self.oi.column_search(input1,table_name,column_name)

        # and print our results in a neat manner
        if print_output_here:

            # remind the user what was searched for
            print ""
            print input1,"has been searched for in the column",column_name,"of table",table_name,"in the menu-aware monitoring Oracle database."
            print ""

            # if no results have been found
            if len(search_results_list) == 0:

                # let the user know
                print "No results have been found."
                print ""
                return

            # else if results have been found
            print "The following",len(search_results_list),"results have been found:"

            # loop over the search results
            for n, row_dict in enumerate(search_results_list):

                # create a name for the row
                # this is just "Result_n" where n is the row number
                row_name = "Result_"+str(n)

                # recursively print the dictionary
                print ""
                print row_name+":"
                self.__print_one_dict__(row_dict,"   ",row_name)

                # added bonus for the user
                # if this is an mck table, then we shall find and print out the list of smck that this mck links to
                if table_name == "MCK_TABLE":

                    # get the smck_ids
                    smck_ids = self.oi.read_mck_links_from_db(row_dict['MCK_ID'])

                    # info for user
                    print ""
                    print row_name,"is an MCK, and links to the following SMCK:"

                    # first print the list of SMCK_ID
                    print "SMCK_ID list =",smck_ids

                    # find the longest length of each of the three strings we want to print below
                    id_ljust = 0
                    version_ljust = 0
                    slice_ljust = 0

                    # temporary dict to hold smck_info so that we don't have to get them a second time
                    smck_info_dict = {}

                    # for each smck_id
                    for smck_id in smck_ids:

                        # get the smck_info
                        smck_info = self.oi.read_smck_info_from_db(smck_id)

                        # add it to smck_info_dict
                        smck_info_dict[smck_id] = smck_info

                        # find the longest strings
                        if len(str(smck_id)) > id_ljust:
                            id_ljust = len(str(smck_id))
                        if len(str(smck_info['SMCK_TOOL_PATCH_VERSION'])) > version_ljust:
                            version_ljust = len(str(smck_info['SMCK_TOOL_PATCH_VERSION']))
                        if len(str(smck_info['SMCK_SLICE_TYPE'])) > slice_ljust:
                            slice_ljust = len(str(smck_info['SMCK_SLICE_TYPE']))

                    # now we print stuff
                    # for each smck_id
                    for smck_id in smck_ids:

                        # get the smck_info from the above dictionary
                        smck_info = smck_info_dict[smck_id]

                        # print some formatted info on this smck
                        print "[ SMCK_ID = "+str(smck_id).ljust(id_ljust)+" , SMCK_TOOL_PATCH_VERSION = "+str(smck_info['SMCK_TOOL_PATCH_VERSION']).ljust(version_ljust)+" , SMCK_SLICE_TYPE = "+str(smck_info['SMCK_SLICE_TYPE']).ljust(slice_ljust)+" ]"

            # some nice spacing
            print ""

        # else we assume the user wants the search results returned
        # (ie if this function has been called from within another function)
        else:

            # return the search results
            return search_results_list


    def apply_mck(self,input1="",print_output_here=""):
        "Apply an MCK to locally running tools."

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
        smck_ids = self.oi.read_mck_links_from_db(input1)

        # for each smck_id, apply the smck config
        for smck_id in smck_ids:

            # apply the smck config
            self.apply_smck(smck_id)


    def apply_smck(self,input1="",print_output_here=""):
        "Apply an SMCK to a locally running tool."

        # check for empty print_output_here
        # if it is found, use self.print_output
        if print_output_here == "":
            print_output_here = self.print_output

        # make sure we have the smck_id
        smck_id = self.__get_smck_id_from_smck_identifier__(input1)
        
        # if we don't
        if smck_id == -1:

            # info for user
            if print_output_here:
                print "SMCK",input1,"has not been recognised as a valid SMCK."
            return

        # get the smck_info
        smck_info = self.oi.read_smck_info_from_db(smck_id)

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
        patch_config = self.__unicode_to_str__(smck_info['SMCK_CONFIG']['ToolInfo'] )

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
        monitCategory_patch_config = self.__unicode_to_str__(smck_info['SMCK_CONFIG']['MonitCategoryInfo'] )

        # if monitCategory_patch_config is not empty, then we must apply this patch
        if not monitCategory_patch_config == {}:

            # get the MonitCategory name
            monitCategory_name = self.__unicode_to_str__(smck_info['SMCK_CONFIG']['MonitCategoryName'] )

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
        
        # get list of all mck_to_smk links
        mck_to_smk_links = []
        mck_to_smk_links = self.oi.get_all_mck_to_smk_links()

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


    def print_all_mck_to_smk_links(self):
        "Print all MCK to SMK links."

        # get list of all mck_to_smk links
        mck_to_smk_links = []
        mck_to_smk_links = self.oi.get_all_mck_to_smk_links()

        # reverse the list in place
        mck_to_smk_links.reverse()

        # loop over the list in reverse (lowest smk to largest smk)
        for n, link in enumerate(mck_to_smk_links):

            # get some values
            start_smk = link['SMK']
            end_smk = -1
            mck_id = link['MCK']

            # if this in an active link, then we want to print this information
            print_active = ""
            if link['ACTIVE'] == '1':
                print_active = "ACTIVE"
            
            # if this is not the last element in the list, then get one less than the next smk
            if n+1 < len(mck_to_smk_links):
                
                # variables for looping over SMKs (since the list will include deactivated links
                found_next_smk = False
                i = 1
                
                # loop over links
                while not_found_next_smk:
                    
                    # next SMK in list
                    next_smk = mck_to_smk_links[n+i]['SMK']

                    # if it's still the same SMK
                    if next_smk == start_smk:
                        # go to the next one
                        i = i+1
                    # else we've actually found the next SMK
                    else:
                        end_smk = next_smk - 1
                        found_next_smk = True

            # print the results
            if end_smk != -1:
                print "SMK",start_smk,"to SMK",end_smk,"= MCK",mck_id,print_active
            else:
                print "SMK",start_smk,"and greater = MCK",mck_id,print_active


    def upload_mck_to_smk_link(self,input_mck_id="",input_smk="",comment=""):
        """Upload a link between an SMK and an MCK.
        All SMK from the input SMK onwards will be linked to the input MCK,
        unless another link to a larger SMK is uploaded.
        ie. if upload_mck_to_smk_link(71,123)
        and    upload_mck_to_smk_link(72,456)
        are both called, then:
        SMK 123 to SMK 455  will link to MCK 71
        SMK 456 and greater will link to MCK 72
        As uploaded MCK have positive (non-zero) values,
        linking an SMK to MCK=0 is intended to signify 
        that the default tool configurations should be used. 
        ie. no MCK should be applied for this SMK.
        An optional comment can be attached to the upload.
        If you upload a link for an SMK, and a link already exists for that SMK,
        then a new link will be created and made active.
        The origional link will be deactivated (but will remain in the database for posterity)."""

        # is this session interactive?
        if self.__is_session_interactive__():

            # if type(input_smk) is not int, then ask the user to input an SMK
            while type(input_smk) is not int:

                # info for user
                print "Please enter an SMK integer to be linked."

                # now get user input
                input_str = raw_input("SMK: ")

                # try to convert the input str to int
                try:
                    input_smk = int(input_str)
                except:
                    print input_str,"could not be converted to an int."

            # if type(input_mck_id) is not int, then ask the user to input an MCK
            while type(input_mck_id) is not int:

                # info for user
                print "Please enter an MCK_ID integer to be linked."

                # now get user input
                input_str = raw_input("MCK_ID: ")            

                # try to convert the input str to int
                try:
                    input_mck_id = int(input_str)
                except:
                    print input_str,"could not be converted to an int."

        # if this session is not interactive
        else:
            
            # if the inputs are not valid
            if (type(input_mck_id) is not int) or (type(input_smk) is not int):

                # return, as inputs are not valid
                # print for logfile
                print "Menu-aware monitoring: upload_mck_to_smk_link(",input_mck_id,",",input_smk,",",comment,") inputs not valid. MCK and SMK must be integers."
                print "No MCK to SMK link created."
                return
            
        if not self.oi.check_if_mck_id_exists(input_mck_id):
            print "MCK",input_mck_id,"does not exist in database."
            print "No MCK to SMK link created."                
            return

        # if we've got this far, then the input MCK and SMK should be valid

        # check if this link already exists as the active link
        if self.oi.check_if_mck_to_smk_link_exists_and_is_active(input_mck_id,input_smk):

            # ensure all other links for this smk are deactivated
            self.oi.deactivate_all_links_for_given_smk_except_for_given_mck(input_smk,input_mck_id)

            # info for user, and return
            print "SMK",input_smk,"linked to MCK",input_mck_id,"is already the active link for this SMK."
            return

        # get the current user for the creator
        creator = self.current_user

        # if a comment has not been provided, then ask for one
        if comment == "":
            comment = self.__ask_for_comment__()

        # try to upload the link
        try:

            # info for user
            print "Now attempting to link MCK",input_mck_id,"to SMK",input_smk

            # upload the link
            self.oi.upload_mck_to_smk_link(input_mck_id,input_smk,creator,comment)

            # info for user
            print "MCK",input_mck_id,"has been linked to SMK",input_smk

            # info for user
            print "Now attempting to deactivate all other links for SMK",input_smk

            # ensure all other links for this smk are deactivated
            self.oi.deactivate_all_links_for_given_smk_except_for_given_mck(input_smk,input_mck_id)

            # info for user
            print "All other links for SMK",input_smk,"have been deactivated."

        # if the upload fails in some way
        except:

            # info for user
            print "An exception occurred:",sys.exc_info()[0],sys.exc_info()[1]
            print "Error in link upload."


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
                processing_step = self.__ask_for_processing_step__()

            # if no processing_step is provided, then ask for one
            if processing_stream=="":
                processing_stream = self.__ask_for_processing_stream__()

        # if no comment is provided, then ask for one
        if comment=="":
            comment = self.__ask_for_comment__()

        # fill extra mck_info
        self.local_global_info['MCK'] = {}
        self.local_global_info['MCK']['MCK_DEFAULT'] = default
        self.local_global_info['MCK']['MCK_ATHENA_VERSION'] = self.current_athena_version
        self.local_global_info['MCK']['MCK_CREATOR'] = self.current_user
        self.local_global_info['MCK']['MCK_COMMENT'] = comment

        # fill extra smck_info for all tools
        for tool_key, tool_value in self.local_global_info['MONITORING_TOOL_DICT'].iteritems():

            # fill extra smck_info
            tool_value['SMCK_PROCESSING_STEP'] = processing_step
            tool_value['SMCK_PROCESSING_STREAM'] = processing_stream
            tool_value['SMCK_DEFAULT'] = default
            tool_value['SMCK_ATHENA_VERSION'] = self.current_athena_version
            tool_value['SMCK_CREATOR'] = self.current_user
            tool_value['SMCK_COMMENT'] = comment

        # json encode the local global info, and dump it to the output file
        json.dump(self.local_global_info, output_file, ensure_ascii=True, sort_keys=True) 

        # close the output file
        output_file.close()


    def load_local_config_from_json(self,input_json_filename="mam_configs.json"):
        """Load all trigger monitoring tool configurations from an input json file.
        Note: this does not apply the json config to running trigger-monitoring tools.
        It only loads the json config into <ThisVariable>.local_global_info 
        to allow for upload to the MaM database, or testing.
        Existing <ThisVariable>.local_global_info will be overwritten.
        Once loaded, this json config can be uploaded to the MaM database by calling 
        <ThisVariable>.upload_default() if this is a default config, or
        <ThisVariable>.upload_all_local_changes_as_smck() if this is a patch config.
        However to upload to the database from a json file, you probably want to instead use 
        <ThisVariable>.upload_config_from_json(input_json_filename)"""
        
        # create a file-like-object to read the json from
        input_file = open( input_json_filename , "r" )

        # json decode the local global info, and pass it to self.local_global_info, converting unicode to str
        self.local_global_info = self.__unicode_to_str__( json.load(input_file) )

        # update self.local
        self.__update_local_pointer__()

        # close the input file
        input_file.close()


    def upload_config_from_json(self,input_json_filename="mam_configs.json",print_output_here=""):
        """Load all trigger monitoring tool configurations from an input json file, 
        and upload these configurations to the database.
        If you only want to load configurations from a json file, 
        but not immediately upload to the database, then consider instead calling 
        <ThisVariable>.load_local_config_from_json(input_json_filename)"""
        
        # check for empty print_output_here
        # if it is found, use self.print_output
        if print_output_here == "":
            print_output_here = self.print_output

        # first we load the json file
        self.load_local_config_from_json(input_json_filename)

        # check that there are SMCK in this upload
        if len(self.local_global_info['MONITORING_TOOL_DICT']) == 0:
            
            # info for user
            print "The file",input_json_filename,"contains no tool configurations to be uploaded."
            return

        # then we extract whether this is a default configuration or not
        default = self.local_global_info['MCK']['MCK_DEFAULT']

        # and we extract the comment, athena version, stream, step, and user
        comment = self.local_global_info['MCK']['MCK_COMMENT']
        athena_version = self.local_global_info['MCK']['MCK_ATHENA_VERSION']
        user = self.local_global_info['MCK']['MCK_CREATOR']
        for value in self.local_global_info['MONITORING_TOOL_DICT'].values():
            processing_stream = value['SMCK_PROCESSING_STREAM']
            processing_step = value['SMCK_PROCESSING_STEP']
            break

        # if this is a default config, then use upload_default
        if default == 1:
            self.upload_default(comment,athena_version,user,print_output_here)
        # else we use upload_all_local_changes_as_smck
        else:
            self.upload_all_local_changes_as_smck(processing_step,processing_stream,comment,athena_version,user,print_output_here)


    def diff_json_files(self,input_default_config_file="mam_default_configs.json",input_config_file="mam_configs.json",output_json_filename="mam_patch_configs.json"):
        """Input a default config, and an alternative config. A diff will be performed. 
        The parts of the alternative config that are different to the default config 
        will be returned as a new json file."""

        # open all file-like-objects
        input_default_file = open( input_default_config_file , "r" )
        input_alt_file = open( input_config_file , "r" )
        output_file = open( output_json_filename , "w" )

        # get the input dictionaries
        input_default_dict = self.__unicode_to_str__( json.load(input_default_file) )
        input_alt_dict = self.__unicode_to_str__( json.load(input_alt_file) )

        # perform a diff of these dicts
        # we want diffed_info2, 
        # which is the 'patch' to apply to the default to get the current local configuration
        diffed_info1, diffed_info2 = self.__calculate_diff__(input_default_dict,input_alt_dict,False)

        # json encode the diffed 'patch' info, and dump it to the output file
        json.dump(diffed_info2, output_file, ensure_ascii=True, sort_keys=True)

        # close all files
        input_default_file.close()
        input_alt_file.close()
        output_file.close()
