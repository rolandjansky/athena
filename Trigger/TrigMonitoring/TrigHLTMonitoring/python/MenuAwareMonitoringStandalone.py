#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# Authors: Ben Smart (Ben.Smart@cern.ch), Xanthe Hoad (Xanthe.Hoad@cern.ch)
#

import sys,os
# import subprocess. Required to get user
import subprocess
from xml.dom import minidom
# import oracle interaction class
from OracleInterface import OracleInterface
# import hash library for generating smck config hashes
import hashlib
# import json for converting configuration dictionaries into strings for hashing
import json

# This is the 'Athenaless' part of MenuAwareMonitoring
# ie. this and OracleInterface.py can be used without Athena
class MenuAwareMonitoringStandalone:
    """Menu-aware Monitoring standalone 'Athena-less' class.
    Provides functionality for the upload/download of trigger-monitoring tool configurations to an Oracle database."""


    def __init__(self):

        # MaM code version
        self.version = '1.2.7'

        # flag for setting whether to print out anything to screen or not
        self.print_output = True

         # create oracle interaction object
        self.oi = OracleInterface()

        # holder for current user
        self.current_user = ""
        self.__get_current_user__()

        # Since this code is Athena-independent, we use a dummy 'Athena version'
        self.current_athena_version = "MaM P1 GUI"

        # holder for current local global_info
        self.local_global_info = {}
        self.local_global_info['MONITORING_TOOL_DICT'] = {}

        # pointer to local tool info
        self.local = self.local_global_info['MONITORING_TOOL_DICT']

        # flag to record if we have connected to Oracle
        self.connected_to_oracle = False

        # now connect to oracle
        # TODO This needs to be modified to get the correct login details
        self.__connect_to_oracle__()

        # print guide for user
        print ""
        print "Authors of this code: Ben Smart (Ben.Smart@cern.ch), Xanthe Hoad (Xanthe.Hoad@cern.ch)"
        print "This is Menu-aware monitoring version",self.version
        print "You are",self.current_user
        print ""


    def __connect_to_oracle__(self,database_username="",database_password="",database_name=""):
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

                # get the connection from authentication.xml
                connectionSvc = MenuAwareMonitoringStandalone._getConnectionServicesForAlias("TRIGGERDBR2MAM")[0]
                print "Connection Service %s" % connectionSvc
                user,pw = MenuAwareMonitoringStandalone._readAuthentication()[connectionSvc]
                server = connectionSvc.split('/')[2]
                directory = "ATLAS_CONF_TRIGGER_RUN2"
                #print "User %s, pw %s, server %s" % (user,pw,server)
                # connect to oracle
                #self.oi.connect_to_oracle(database_username,database_password,database_name)
                self.oi.connect_to_oracle(user,pw,server,directory)
                #print "Got here"
                # flag to record that we have connected
                self.connected_to_oracle = True

            except :

                # info for user
                print "Error while connecting to Oracle database. Exiting. " 
            
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

    @staticmethod
    def _getConnectionServicesForAlias(alias):
        connectionServices = None # list of services

        dblookupfilename = MenuAwareMonitoringStandalone._getFileLocalOrPath('dblookup.xml','CORAL_DBLOOKUP_PATH')
        if dblookupfilename == None: return None

        doc = minidom.parse(dblookupfilename)
        for ls in doc.getElementsByTagName('logicalservice'):
            if ls.attributes['name'].value != alias: continue
            connectionServices = [str(s.attributes['name'].value) for s in ls.getElementsByTagName('service')]
        doc.unlink()

        print( "For alias '%s' found list of connections %r" % (alias,connectionServices) )
        if connectionServices == None:
            print("Trigger connection alias '%s' is not defined in %s" % (alias,dblookupfilename))
        return connectionServices

    @staticmethod
    def _readAuthentication():
        """ read authentication.xml, first from local directory, then from all paths specified in CORAL_AUTH_PATH
        returns dictionary d with d[connection] -> (user,pw)
        """
        
        authDict = {}

        dbauthfilename = MenuAwareMonitoringStandalone._getFileLocalOrPath('authentication.xml','CORAL_AUTH_PATH')
        if dbauthfilename == None: return authDict
        
        doc = minidom.parse(dbauthfilename)
        for cn in doc.getElementsByTagName('connection'):
            user = ""
            pw = ""
            svc = cn.attributes['name'].value
            for p in cn.getElementsByTagName('parameter'):
                if p.attributes['name'].value == 'user': user = p.attributes['value'].value
                if p.attributes['name'].value == 'password': pw = p.attributes['value'].value
            authDict[cn.attributes['name'].value] = (user,pw)
        doc.unlink()
        return authDict
    
    @staticmethod
    def FindFile( filename, pathlist, access ):
        """Find <filename> with rights <access> through <pathlist>."""

        # special case for those filenames that already contain a path
        if os.path.dirname( filename ):
            if os.access( filename, access ):
                return filename

        # test the file name in all possible paths until first found
        for path in pathlist:
            f = os.path.join( path, filename )
            if os.access( f, access ):
                return f

        return None
    
    @staticmethod
    def _getFileLocalOrPath(filename, pathenv):
        """looks for filename in local directory and then in all paths specified in environment variable 'pathenv'
        returns path/filename if existing, otherwise None """

        if os.path.exists(filename):
            print( "Using local file %s" % filename)
            return filename

        pathlist = os.getenv(pathenv,'').split(os.pathsep)
        resolvedfilename = MenuAwareMonitoringStandalone.FindFile(filename, pathlist, os.R_OK)
        if resolvedfilename:
            return resolvedfilename

        print("No file %s found locally nor in %s" % (filename, os.getenv('CORAL_DBLOOKUP_PATH')) )
        return None


    def __get_current_user__(self):
        "Get the current user."

        # get the current user
        self.current_user = subprocess.check_output("echo $USER", shell=True).replace("\n","")


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

        # # check if this is an interactive session
        # # (if -i was passed as a command-line argument)
        # return sys.argv.__contains__('-i')
        
        # THIS IS A DUMMY FUNCTION.
        return True


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
            
        if (not self.oi.check_if_mck_id_exists(input_mck_id)) and (input_mck_id is not 0):
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

        # check if there are any other active links for this smk
        active_smk_to_mck_link_search_results = self.oi.find_active_smk_to_mck_link(input_smk)

        if len(active_smk_to_mck_link_search_results) > 0:
            # we have found an active link. Upload should not proceed for safety.
            print "SMK",input_smk,"is already linked to MCK",active_smk_to_mck_link_search_results[0][0],". Will now print full link details."
            print active_smk_to_mck_link_search_results
            print "Deactivate this link first if you want to create a new link. You can use force_deactivate_all_links_for_smk(smk) (or tick the force upload box if using the GUI), but be sure this is what you want to do."
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

    def force_deactivate_all_links_for_smk(self,input_smk,GUI=False):
        """Allows the user to clear all links for an smk, so that a new link can be created"""

        # check if there are any other active links for this smk
        active_smk_to_mck_link_search_results = self.oi.find_active_smk_to_mck_link(input_smk)

        if len(active_smk_to_mck_link_search_results) > 0:
            print "SMK",input_smk,"is already linked to MCK",active_smk_to_mck_link_search_results[0][0],". Will now print full link details."
            print active_smk_to_mck_link_search_results
        else:
            print "SMK",input_smk,"is not linked to any MCK."
            return

        if GUI is False:
            print "Will force deactivate all links for SMK",input_smk,". Do you really want to do this?"
            user_input = raw_input("y/n: ")
        
            if user_input != 'y':
                print "Aborted."
                return

        print "Force deactivating all links for SMK",input_smk,"..."
        self.oi.deactivate_all_links_for_given_smk(input_smk)
        print "All links deactivated."

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


    def get_default_mck_id_from_db(self,input_athena_version=""):
        """Get the MCK number (MCK_ID) of the default for this Athena version.
        If input_athena_version=='', the current Athena version is used."""

        # if no input Athena version is provided, then use the current version
        if input_athena_version == "":
            input_athena_version = self.current_athena_version

        # search for default mck
        return self.oi.read_default_mck_id_from_db(input_athena_version)


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




if __name__=="__main__":

    connectionSvc = MenuAwareMonitoringStandalone._getConnectionServicesForAlias("TRIGGERDBR2MAM")[0]
    print "Connection service ", connectionSvc
    user,pw = MenuAwareMonitoringStandalone._readAuthentication()[connectionSvc]
    oi = OracleInterface()
    oi.connect_to_oracle(user,pw,connectionSvc.split('/')[2])
    conn = oi.conn
    print conn

