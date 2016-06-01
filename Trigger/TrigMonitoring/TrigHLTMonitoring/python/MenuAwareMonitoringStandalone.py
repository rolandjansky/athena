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
    
    def __init__(self,alias="TRIGGERDBR2MAM",database_username="",database_password="",database_name="",database_directory=""):
        """Setup Menu-aware Monitoring,
        find locally running trigger-monitoring tools,
        connect to the Oracle database,
        and get the current default from the database (if it exists)."""

        # MaM code version
        self.version = '1.2.10'

        # flag for setting whether to print out anything to screen or not
        self.print_output = True

         # create oracle interaction object
        self.oi = OracleInterface()

        # holder for current user
        self.current_user = ""
        self.__get_current_user__()

        # Since this code is Athena-independent, we use a dummy 'Athena version'
        # If called from MenuAwareMonitoring, functions will be passed the correct version from there
        self.current_athena_version = "MaM P1 GUI"
        
        # holder for default global_info
        self.default_global_info = {}

        # holder for current local global_info
        self.local_global_info = {}
        self.local_global_info['MONITORING_TOOL_DICT'] = {}

        # pointer to local tool info
        self.local = self.local_global_info['MONITORING_TOOL_DICT']

        # flag to record if we have connected to Oracle
        self.connected_to_oracle = False
        
        # flag to indicate if we are connected to the replica database with a readonly connection
        self.replica_db_connection = False

        # now connect to oracle
        self.__connect_to_oracle__(alias,database_username,database_password,database_name,database_directory)
        if self.connected_to_oracle == False:
            if alias == "TRIGGERDBR2MAM":
                print "Exiting."
                sys.exit(1)
            else:
                print "Some Menu Amare Monitoring functions will not be available to you."

        # print guide for user
        print "WARNING, THIS VERSION OF MAM IS EXPERIMENTAL, PLEASE USE TAG TrigHLTMonitoring-00-07-13"
        print "Authors of this code: Ben Smart (Ben.Smart@cern.ch), Xanthe Hoad (Xanthe.Hoad@cern.ch)"
        print "This is Menu-aware monitoring version",self.version
        print "You are",self.current_user
        print "This version of MaM is in the process of being tested, please report any bugs to the authors."


    def __connect_to_oracle__(self,alias,database_username,database_password,database_name):
        "Connect to the Oracle server."

        # if we are already connected
        if self.connected_to_oracle:

            # info for user
            print "We are already connected to the Oracle database"

        # else if we are not
        else:
        
            # info for user
            print "We are now connecting to the Oracle database"
            
            if alias is "CUSTOM_DB":
                print "Connecting to database",database_name,"with provided username, password and directory"
                self.oi.connect_to_oracle(database_username,database_password,database_name,database_directory)
                self.connected_to_oracle = True

            elif alias in ("TRIGGERDB","TRIGGERDBREPR","TRIGGERDBR2MAM"):
                # try catch
                try:

                    # get the connection from authentication.xml
                    connectionSvc = MenuAwareMonitoringStandalone._getConnectionServicesForAlias(alias)[0]
                    print "Connection Service %s" % connectionSvc
                    user,pw = MenuAwareMonitoringStandalone._readAuthentication()[connectionSvc]
                    server = connectionSvc.split('/')[2]
                    directory = "ATLAS_CONF_TRIGGER_RUN2"
                    print "User %s, server %s" % (user,server)
                    # connect to oracle
                    self.oi.connect_to_oracle(user,pw,server,directory)
                    # flag to record that we have connected
                    self.connected_to_oracle = True
                    if alias == "TRIGGERDB":
                        self.replica_db_connection = True

                except :
                    # info for user
                    print "Error while connecting to Oracle database." 
                    self.connected_to_oracle = False
            else:
                print "Error while connecting to Oracle database: Unrecognised database alias",alias   
                self.connected_to_oracle = False

    def __disconnect_from_oracle__(self):
        "Disconnect from the Oracle server."

        # if we are connected to Oracle
        if self.connected_to_oracle:

            # info for user
            print "We are now disconnecting from the Oracle database"

            # disconnect from oracle
            if self.oi.disconnect_from_oracle(): #returns True if disconnection is successful
                self.connected_to_oracle = False
                self.replica_db_connection = False

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
        "Is this an interactive Athena session, or a python session?"

        # # check if this is an interactive session
        # # (if -i was passed as a command-line argument)
        if sys.argv.__contains__('athena.py'):
            return sys.argv.__contains__('-i')
        else: 
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
         
        if not self.oi.check_if_smk_exists(input_smk):
            print "SMK",input_smk,"does not exist in the trigger DB."
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
        
        if self.connected_to_oracle == False:
            print "You are not connected to the database, so this function is not available."
            return

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
        
        if self.connected_to_oracle == False:
            print "You are not connected to the database, so this function is not available."
            return
        
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

        # check that the smcks are from a consistent release
        smck_release_dict = {}

        for smck_id in smck_ids:
            smck_release = self.oi.read_smck_info_from_db(smck_id)['SMCK_ATHENA_VERSION']            
            if smck_release in smck_release_dict:
                smck_release_dict[smck_release].append(smck_id)
            else:
                smck_release_dict[smck_release] = [smck_id]
                
        if len(smck_release_dict) > 1:
            print "Your SMCK list contains SMCKs from different Athena releases." 
            print "MCKs must consist of SMCKs from the same release." 
            print "Please try again with a different SMCK list." 
            print "The SMCKs you entered with their releases will be printed below."
            print smck_release_dict
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
        #mck_info['MCK_ATHENA_VERSION'] = self.current_athena_version
        mck_info['MCK_ATHENA_VERSION'] = smck_release_dict.keys()[0] #we only get this far if this dictionary has 1 element
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
        self.local_global_info = self.oi.__unicode_to_str__( json.load(input_file) )

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

        if self.connected_to_oracle == False:
            print "You are not connected to the database, so this function is not available."
            return
        
        if self.replica_db_connection == True:
            print "You are connected to the replica database and your connection is read only, so this function is not available to you."
            return
        
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
    
        if self.connected_to_oracle == False:
            print "You are not connected to the database, so this function is not available."
            return

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
        
        if self.connected_to_oracle == False:
            print "You are not connected to the database, so this function is not available."
            return

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
        
        if self.connected_to_oracle == False:
            print "You are not connected to the database, so this function is not available."
            return

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
        
        if self.connected_to_oracle == False:
            print "You are not connected to the database, so this function is not available."
            return
        
        if self.replica_db_connection == True:
            print "You are connected to the replica database and your connection is read only, so this function is not available to you."
            return

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

    def search(self,input1="",flag1="",print_output_here=""):
        """Search the Oracle database for something.
            input1 is is what is to be searched for.
            flag1 specifies what kind of input input1 is."""
        
        if self.connected_to_oracle == False:
            print "You are not connected to the database, so this function is not available."
            return
        
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
                    if smck_ids is -1:
                        print row_name,"is an MCK which links to no SMCKs. This MCK is empty."
                    else:
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

        else:            
            # return the search results
            return search_results_list
 
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

    def print_all_mck_to_smk_links(self):
        "Print all MCK to SMK links."
        
        if self.connected_to_oracle == False:
            print "You are not connected to the database, so this function is not available."
            return
        
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
                while not found_next_smk:
                    
                    if n+i == len(mck_to_smk_links):
                        # we're at the end of the list, so end the loop
                        end_smk = -1
                        found_next_smk = True
                        break
                    
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


