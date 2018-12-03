#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# Authors: Ben Smart (ben.smart@cern.ch), Xanthe Hoad (xanthe.hoad@cern.ch)
# See https://twiki.cern.ch/twiki/bin/view/Atlas/MaDQM for more information

import sys,os
# import subprocess. Required to get user
import subprocess
from xml.dom import minidom
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
        self.version = '1.4.8'
        # flag for setting whether to print out anything to screen or not
        self.print_output = True
         # create oracle interaction objects
        self.oi = OracleInterface()
        # holder for current user
        self.current_user = ""
        self.__get_current_user__()
        # Since this code is Athena-independent, we use a dummy 'Athena version'
        # If called from MenuAwareMonitoring, this will be updated to the real version
        self.current_athena_version = "MaMStandalone"
        self.current_nightly_version = "MaMStandalone"
        # holder for default global_info
        self.default_global_info = {}
        # holder for current local global_info
        self.local_global_info = {}
        self.local_global_info['MONITORING_TOOL_DICT'] = {}
        # pointer to local tool info
        self.local = self.local_global_info['MONITORING_TOOL_DICT']
        # flag to record if we have connected to Oracle
        self.connected_to_oracle = False
        self.connection_alias = ""
        # flag to indicate if we are connected to the replica database with a readonly connection
        self.readonly_db_connection = False
        # greetings
        print "Welcome to Menu-aware Monitoring (MaM) version",self.version
        print "See https://twiki.cern.ch/twiki/bin/view/Atlas/MaDQM for more info"
        # now connect to oracle
        self.__connect_to_oracle__(alias,database_username,database_password,database_name,database_directory)
        if self.connected_to_oracle == False:
            if alias == "TRIGGERDBR2MAM":
                print "Exiting"
                sys.exit(1)
            else:
                print "Some Menu-aware Monitoring functions will not be available to you"
        else:
            self.connection_alias = alias


    def __connect_to_oracle__(self,alias,database_username,database_password,database_name,database_directory):
        "Connect to the Oracle server."

        if self.connected_to_oracle:
            print "MaM is already connected to the Oracle database"
        else:
            if alias is "CUSTOM_DB":
                print "Connecting to database",database_name,"with provided username, password and directory"
                print "WARNING: MaM may not work as expected while using a custom DB"
                self.oi.connect_to_oracle(database_username,database_password,database_name,database_directory)
                self.connected_to_oracle = True

            elif alias in ("TRIGGERDB","TRIGGERDBREPR","TRIGGERDBREPR_R","TRIGGERDBR2MAM"):
                # get the connection from authentication.xml
                connectionSvcs = self._getConnectionServicesForAlias(alias)
                for connectionSvc in connectionSvcs:
                    print "Trying connection service",connectionSvc
                    if 'oracle' in connectionSvc:
                        try:
                            oracle_server = connectionSvc.split('/')[2]
                            user,pw = self._readAuthentication(alias)[connectionSvc]
                            if alias in ["TRIGGERDBREPR","TRIGGERDBREPR_R"]:
                                directory = "ATLAS_CONF_TRIGGER_REPR"
                            else:
                                directory = "ATLAS_CONF_TRIGGER_RUN2"
                            print "User %s, server %s, directory %s" % (user,oracle_server,directory)
                            self.oi.connect_to_oracle(user,pw,oracle_server,directory)
                            # flag to record that we have connected
                            self.connected_to_oracle = True
                            if alias in ["TRIGGERDB","TRIGGERDBREPR_R"]:
                                self.readonly_db_connection = True
                            break
                        except:
                            print "Direct Oracle connection not possible, trying Frontier if available..."
                            self.connected_to_oracle = False
                    elif 'frontier' in connectionSvc:
                        try:
                            frontier_server = os.getenv('FRONTIER_SERVER',None)
                            if alias in ["TRIGGERDBREPR","TRIGGERDBREPR_R"]:
                                directory = "ATLAS_CONF_TRIGGER_REPR"
                            else:
                                directory = "ATLAS_CONF_TRIGGER_RUN2"
                            schema = directory
                            print "Frontier Server %s, schema %s" % (frontier_server,schema)
                            # connect to oracle
                            self.oi.connect_to_oracle_via_frontier(frontier_server,schema,directory)
                            # flag to record that we have connected
                            self.connected_to_oracle = True
                            if alias in ["TRIGGERDB","TRIGGERDBREPR_R"]:
                                self.readonly_db_connection = True
                            break
                        except:
                            print "Oracle connection via Frontier not possible"
                            self.connected_to_oracle = False
                    else:
                        print "Error while connecting to Oracle database, no connections found"
                        self.connected_to_oracle = False
            else:
                print "Error while connecting to Oracle database: Unrecognised database alias",alias
                self.connected_to_oracle = False


    def __disconnect_from_oracle__(self):
        "Disconnect from the Oracle server."

        if self.connected_to_oracle:
            print "We are now disconnecting from the Oracle database"
            if self.oi.disconnect_from_oracle(): #returns True if disconnection is successful
                self.connected_to_oracle = False
                self.readonly_db_connection = False


    def _getConnectionServicesForAlias(self,alias):
        connectionServices = None # list of services

        if alias is "TRIGGERDBR2MAM":
            DBLOOKUP_PATH = 'MAM_CORAL_DBLOOKUP_PATH'
        elif alias is "TRIGGERDBREPR":
            DBLOOKUP_PATH = '/afs/cern.ch/user/a/attrgcnf/.dbauth/menuexperts'
        else:
            # TRIGGERDBREPR_R uses this path
            DBLOOKUP_PATH = 'CORAL_DBLOOKUP_PATH'
        if alias is "TRIGGERDBREPR_R":
            # _R means we access the regular auth files instead of the ones with read/write access
            # need to now change the alias back to TRIGGERDBREPR so that we look up the right details in the auth files
            alias = "TRIGGERDBREPR"
        dblookupfilename = self._getFileLocalOrPath('dblookup.xml',DBLOOKUP_PATH)
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


    def _readAuthentication(self,alias):
        """ read authentication.xml, first from local directory, then from all paths specified in CORAL_AUTH_PATH
        returns dictionary d with d[connection] -> (user,pw)
        """

        authDict = {}
        if alias is "TRIGGERDBR2MAM":
            AUTH_PATH = 'MAM_CORAL_AUTH_PATH'
        elif alias is "TRIGGERDBREPR":
            AUTH_PATH = '/afs/cern.ch/user/a/attrgcnf/.dbauth/menuexperts'
        else:
            AUTH_PATH = 'CORAL_AUTH_PATH'
        dbauthfilename = self._getFileLocalOrPath('authentication.xml',AUTH_PATH)
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


    def FindFile(self,filename, pathlist, access ):
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


    def _getFileLocalOrPath(self,filename, pathenv):
        """looks for filename in local directory and then in all paths specified in environment variable 'pathenv'
        returns path/filename if existing, otherwise None """

        #Removed: breaks some Tier0Tests
        #if os.path.exists(filename):
        #    print( "Using local file %s" % filename)
        #    return filename

        pathlist = os.getenv(pathenv,'').split(os.pathsep)
        resolvedfilename = self.FindFile(filename, pathlist, os.R_OK)
        if resolvedfilename:
            return resolvedfilename
        else:
            resolvedfilename = pathenv + '/' + filename
            return resolvedfilename

        print("No file %s found locally nor in %s" % (filename, pathenv) )
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
            print "__get_config_hash__ has been passed a string, rather than an smck_config dict. This is unexpected! Continuing anyway..."
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

            # if need be, then this list can be extended at a later date
            valid_input = ['ALL','ESD','AOD']

            print "Please specify which processing step(s) you wish this input to be for."
            print "The default is 'ALL' for all processing steps."
            print "Valid options are:",valid_input
            print "Hit <enter> without any input to select the default option (ALL)"

            # now get user input
            user_input = raw_input("processing step: ").upper()
            # if input is blank, interpret this as 'ALL'
            if user_input == "":
                user_input = "ALL"
                print "You have selected",user_input

            # check for valid input
            # if input is not valid, ask for it again
            while not valid_input.__contains__(user_input):
                # warning to user that input was not understood
                print "The input",user_input,"was not recognised. Please specify a valid option."
                user_input = raw_input("processing step: ")
                # if input is blank, interpret this as 'ALL'
                if user_input == "":
                    user_input = "ALL"
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

            # if need be, then this list can be extended at a later date
            valid_input = ['ALL','BULK','EXPRESS']

            print "Please specify which processing stream(s) you wish this input to be for."
            print "The default is 'ALL' for all processing streams."
            print "Valid options are:",valid_input
            print "Hit <enter> without any input to select the default option (ALL)"

            # now get user input
            user_input = raw_input("processing stream: ").upper()
            # if input is blank, interpret this as 'ALL'
            if user_input == "":
                user_input = "ALL"
                print "You have selected",user_input

            # check for valid input
            # if input is not valid, ask for it again
            while not valid_input.__contains__(user_input):
                # warning to user that input was not understood
                print "The input",user_input,"was not recognised. Please specify a valid option."
                user_input = raw_input("processing stream: ")
                # if input is blank, interpret this as 'ALL'
                if user_input == "":
                    user_input = "ALL"
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


    def link_smk_to_mck(self,input_smk="",input_mck_id="",comment=""):
        """Upload a link between an SMK and an MCK.
        An optional comment can be attached to the upload.
        If you upload a link for an SMK, and a link already exists for that SMK,
        then a new link will be created and made active.
        The original link will be deactivated (but will remain in the database for posterity)."""

        # is this session interactive?
        if self.__is_session_interactive__():

            # if type(input_smk) is not int, then ask the user to input an SMK
            while type(input_smk) is not int:
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
                print "Menu-aware monitoring: link_smk_to_mck(",input_smk,",",input_mck_id,",",comment,") inputs not valid. MCK and SMK must be integers."
                print "No MCK to SMK link created."
                return

        if not self.oi.check_if_smk_exists(input_smk):
            print "SMK",input_smk,"does not exist."
            print "No MCK to SMK link created."
            return

        if (not self.oi.check_if_mck_id_exists(input_mck_id)) and (input_mck_id is not 0):
            print "MCK",input_mck_id,"does not exist in database."
            print "No MCK to SMK link created."
            return

        # if we've got this far, then the input MCK and SMK should be valid

        # check if this link already exists as the active link
        if self.oi.check_if_smk_to_mck_link_exists_and_is_active(input_smk,input_mck_id):
            # ensure all other links for this smk are deactivated
            self.oi.deactivate_all_links_for_given_smk_except_for_given_mck(input_smk,input_mck_id)
            print "SMK",input_smk,"linked to MCK",input_mck_id,"is already the active link for this SMK."
            return

        # check if there are any other active links for this smk
        active_smk_to_mck_link_search_results = self.oi.find_active_smk_to_mck_link(input_smk)

        if len(active_smk_to_mck_link_search_results) > 0:
            # we have found an active link. Upload should not proceed for safety.
            print "SMK",input_smk,"is already linked to MCK",active_smk_to_mck_link_search_results[0][0]
            print "Will now print full link details:"
            print active_smk_to_mck_link_search_results
            print "Deactivate this link first if you want to create a new link."
            print "You can use force_deactivate_all_links_for_smk(smk) (or tick the force upload box if using the GUI)."
            print "You should make sure this is what you really want to do first before proceeding!"
            return

        # get the current user for the creator
        creator = self.current_user

        # if a comment has not been provided, then ask for one
        if comment == "":
            comment = self.__ask_for_comment__()

        # try to upload the link
        try:
            print "Now attempting to link MCK",input_mck_id,"to SMK",input_smk
            if not self.oi.check_if_smk_to_mck_link_exists(input_smk,input_mck_id):
                # link does not exist
                # upload the link
                self.oi.upload_mck_to_smk_link(input_mck_id,input_smk,creator,comment)
            self.oi.activate_smk_mck_link(input_smk,input_mck_id)
            print "MCK",input_mck_id,"has been linked to SMK",input_smk
            # ensure all other links for this smk are deactivated (should already be true)
            self.oi.deactivate_all_links_for_given_smk_except_for_given_mck(input_smk,input_mck_id)

        # if the upload fails in some way
        except:
            print "An exception occurred:",sys.exc_info()[0],sys.exc_info()[1]
            print "Error in link upload."


    def force_deactivate_all_links_for_smk(self,input_smk,GUI=False):
        """Allows the user to clear all links for an smk, so that a new link can be created"""

        # check if there are any other active links for this smk
        active_smk_to_mck_link_search_results = self.oi.find_active_smk_to_mck_link(input_smk)

        if len(active_smk_to_mck_link_search_results) > 0:
            print "SMK",input_smk,"is already linked to MCK",active_smk_to_mck_link_search_results[0][0]
            print "Will now print full link details."
            print active_smk_to_mck_link_search_results

            if GUI is False:
                print "Requested force deactivate all links for SMK",input_smk
                print "Do you really want to do this?"
                user_input = raw_input("y/n: ")
                if user_input != 'y':
                    print "Aborted."
                    return

            print "Force deactivating all links for SMK",input_smk,"..."
            self.oi.deactivate_all_links_for_given_smk(input_smk)
            print "All links deactivated."

        else:
            print "SMK",input_smk,"is not linked to any MCK."
            return


    def print_all_mck_to_smk_links(self,print_deactivated_links=False):
        """Print MCK to SMK links. By default prints only active links.
        print_all_mck_to_smk_links(True) to print all links."""

        if self.connected_to_oracle == False:
            print "MaM is not connected to the database, so this function is not available."
            return

        # get list of all mck_to_smk links
        mck_to_smk_links = []
        mck_to_smk_links = self.oi.get_all_mck_to_smk_links()

        # reverse the list in place
        mck_to_smk_links.reverse()

        # loop over the list in reverse (lowest smk to largest smk)
        for link in mck_to_smk_links:
            # print the results
            if link['ACTIVE'] == '1':
                print "SMK",link['SMK'],"is linked to MCK",link['MCK'],"(ACTIVE LINK)"
            elif print_deactivated_links:
                print "SMK",link['SMK'],"was previously linked to MCK",link['MCK'],"(DEACTIVATED LINK)"


    def __get_smck_id_from_smck_identifier__(self,smck_identifier,print_output_here=""):
        "Input can either be an SMCK_ID or an SMCK_TOOL_PATCH_VERSION. Output will be the the correct SMCK_ID."

        if self.connected_to_oracle == False:
            print "MaM is not connected to the database, so this function is not available."
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
            print "MaM is not connected to the database, so this function is not available."
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


    def check_compatibility_of_two_release_versions(self,version1,version2):
        """ Check if two releases versions are compatible.
        If they do, MAM will apply MCKs from either release in the other."""

        if type(version2) is list:
            if version1 in version2:
                return True
        elif version1 == version2:
                return True
        return False


    def clone_mck_for_new_release(self,mck_id,project="",version=""):

        if self.connected_to_oracle == False:
            print "MaM is not connected to the database, so this function is not available."
            return

        if self.readonly_db_connection == True:
            print "Your connection is read only, so this function is not available to you."
            return

        # check mck exists
        if not self.oi.check_if_mck_id_exists(mck_id):
            print "MCK",mck_id,"doesn't exist."
            return

        if not project or not version:
            print "Please give an AtlasProject and an AtlasVersion to assign to the cloned MCK"
            print "Usage: clone_mck_for_new_release(mck_id,project,version)"
            return

        requested_release = project + "-" + version

        # get the original mck info
        mck_info = self.oi.read_mck_info_from_db(mck_id)
        smck_ids = self.oi.read_mck_links_from_db(mck_id)

        # check if mck already exists with the requested release
        search_results = self.oi.check_if_mck_exists(smck_ids)
        # first check if links were found
        if search_results != -1:
            # loop over possible matches
            for row_mck_id in search_results:
                # check if this is an exact match
                if len(self.oi.read_mck_links_from_db(row_mck_id)) == len(smck_ids):
                    # if so, check the release
                    if self.oi.read_mck_info_from_db(row_mck_id)['MCK_ATHENA_VERSION'] == requested_release:
                        print "MCK",row_mck_id,"is an existing clone of MCK",mck_id,"valid in",requested_release
                        print "A new clone will not be created."
                        return

        # alter the mck info
        mck_info['MCK_COMMENT'] = "Clone of MCK " + str(mck_id) + " originally for " + mck_info['MCK_ATHENA_VERSION']
        mck_info['MCK_ATHENA_VERSION'] = requested_release
        del mck_info['MCK_CREATION_DATE']
        del mck_info['MCK_ID']

        # create the new mck, this modifies mck_info
        self.oi.upload_mck(mck_info)
        clone_mck_id = mck_info['MCK_ID']

        # link this new mck to the original smck_ids
        for smck_id in smck_ids:
            self.oi.upload_mck_smck_link(clone_mck_id,smck_id)

        print "Cloned MCK",mck_id,"to create MCK",int(clone_mck_id)
        print mck_info


    def upload_mck(self,input_smck_list=[],comment="",print_output_here=""):
        """input_smck_list should be a list of SMCK, identified be either their SMCK_ID or SMCK_TOOL_PATCH_VERSION.
        An MCK will be uploaded, linking to these SMCK.
        Optional comment can be provided."""

        if self.connected_to_oracle == False:
            print "MaM is not connected to the database, so this function is not available."
            return

        if self.readonly_db_connection == True:
            print "Your connection is read only, so this function is not available to you."
            return

        # check for empty print_output_here
        # if it is found, use self.print_output
        if print_output_here == "":
            print_output_here = self.print_output

        # if the input is empty, then we exit
        if input_smck_list == []:
            if print_output_here:
                print "No list of SMCK has been provided."
                print "No MCK upload is possible without a list of SMCKs."
            return

        # holder for smck_ids
        smck_ids = []

        # loop over input
        for smck_identifier in input_smck_list:
            if smck_identifier not in smck_ids:
                smck_id = self.__get_smck_id_from_smck_identifier__(smck_identifier)

                # if an smck_id has not been found
                if smck_id == -1:
                    if print_output_here:
                        print "Problem with requested SMCK",smck_identifier

                # smck is real and not already in the list, so add it
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

        if print_output_here:
            print "This MCK has been uploaded. It is MCK",int(mck_id)


    def upload_mck_with_specified_release(self,input_smck_list=[],athena_release="",comment="",print_output_here=""):
        """input_smck_list should be a list of SMCK, identified be either their SMCK_ID or SMCK_TOOL_PATCH_VERSION.
        An MCK will be uploaded, linking to these SMCK.
        The release that will be assigned to this MCK must be specified.
        Optional comment can be provided."""

        if self.connected_to_oracle == False:
            print "MaM is not connected to the database, so this function is not available."
            return

        if self.readonly_db_connection == True:
            print "Your connection is read only, so this function is not available to you."
            return

        # check for empty print_output_here
        # if it is found, use self.print_output
        if print_output_here == "":
            print_output_here = self.print_output

        if input_smck_list == []:
            if print_output_here:
                print "No list of SMCK has been provided."
                print "No MCK upload is possible without a list of SMCKs."
            return

        if athena_release == "":
            if print_output_here:
                print "No release has been specified."
                print "No MCK upload is possible without a specified release."
            return

        # holder for smck_ids
        smck_ids = []

        # loop over input
        for smck_identifier in input_smck_list:
            # get smck_id
            smck_id = self.__get_smck_id_from_smck_identifier__(smck_identifier)
            # if an smck_id has not been found
            if smck_id == -1:
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
            if print_output_here:
                print "This MCK already exists. It is MCK",int(mck_id)
            return

        # if no comment is provided, then ask for one
        if comment=="":
            comment = self.__ask_for_comment__()

        # fill mck_info
        mck_info = {}
        mck_info['MCK_DEFAULT'] = 0
        mck_info['MCK_ATHENA_VERSION'] = athena_release
        mck_info['MCK_CREATOR'] = self.current_user
        mck_info['MCK_COMMENT'] = comment

        # upload mck_info
        mck_id = self.oi.upload_mck(mck_info)

        # link this mck to the smck_ids
        for smck_id in smck_ids:
            # upload this link
            self.oi.upload_mck_smck_link(mck_id,smck_id)

        if print_output_here:
            print "This MCK has been uploaded. It is MCK",int(mck_id)


    def load_local_config_from_json(self,input_json_filename="mam_configs.json"):
        """Load all trigger monitoring tool configurations from an input json file.
        Note: this does not apply the json config to running trigger-monitoring tools.
        It only loads the json config into <ThisVariable>.local_global_info
        to allow for upload to the MaM database, or testing.
        Existing <ThisVariable>.local_global_info will be overwritten.
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


    def upload_config_from_json(self,input_json_filename="mam_configs.json"):
        """Load all trigger monitoring tool configurations from an input json file,
        and upload these configurations to the database."""

        if self.connected_to_oracle == False:
            print "MaM is not connected to the database, so this function is not available."
            return

        if self.readonly_db_connection == True:
            print "Your connection is read only, so this function is not available to you."
            return

        # first we load the json file
        self.load_local_config_from_json(input_json_filename)

        # check that there are SMCK in this upload
        if len(self.local_global_info['MONITORING_TOOL_DICT']) == 0:
            print "The file",input_json_filename,"contains no tool configurations to be uploaded."
            return

        # then we extract whether this is a default configuration or not
        default = int(self.local_global_info['MCK']['MCK_DEFAULT'])

        # Don't upload defaults to the DB
        if default == 1:
            print "Defaults cannot be uploaded to the DB."
            return

        # upload this self.local_global_info to the database, and get the new mck_id and smck_ids
        mck_id, smck_ids = self.oi.upload_mck_and_smck(self.local_global_info)

        # print smck info
        print "The following SMCK have been created (or already exist):"
        for smck_id in smck_ids:
            # get smck_info
            tool_value = self.get_smck_info_from_db(smck_id)
            # print info for this smck
            print "SMCK_ID =",tool_value['SMCK_ID']
            print "SMCK_TOOL_PATCH_VERSION =",tool_value['SMCK_TOOL_PATCH_VERSION']

        # also print mck info
        print "An MCK has also been created (or already exists), linking to the above SMCK:"
        print "MCK_ID =",int(mck_id)

        # return the new_mck_id and new_smck_ids
        return mck_id, smck_ids


    def get_global_info_from_db(self,mck_id):
        "For an input MCK number (MCK_ID), get all related MCK and SMCK info, and return it as a dictionary."

        if self.connected_to_oracle == False:
            print "MaM is not connected to the database, so this function is not available."
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


    def get_default_from_json(self,input_json_filename=""):
        """Get a default configuration from a json file and store it as self.default_global_info"""

        # create a file-like-object to read the json from
        input_file = open( input_json_filename , "r" )

        # json decode the local global info, and pass it to self.local_global_info, converting unicode to str
        self.default_global_info = self.oi.__unicode_to_str__( json.load(input_file) )

        # close the input file
        input_file.close()


    def dump_mck_to_json(self,mck_id,output_json_filename=""):
        "Dump the contents of an MCK to a json file, including the contents of linked SMCKs"

        mck_info = self.oi.read_mck_info_from_db(mck_id)
        if mck_info == -1:
            print "MCK",mck_id,"has not been recognised as a valid MCK."
            return

        smck_ids = self.oi.read_mck_links_from_db(mck_id)

        if output_json_filename == "":
            output_json_filename = "MCK_"+str(mck_id)+".json"

        output_file = open( output_json_filename, "w" )

        mck_dump_info = {}
        # datetime.datetime objects are not JSON serializable
        # seeing as this info is not used later, we replace with the ctime
        mck_info['MCK_CREATION_DATE'] = mck_info['MCK_CREATION_DATE'].ctime()
        mck_dump_info['MCK'] = mck_info

        # combine rest of the MCK info in the MONITORING_TOOL_DICT
        mck_dump_info['MONITORING_TOOL_DICT'] = {}
        for smck_id in smck_ids:
            smck_info = self.oi.read_smck_info_from_db(smck_id)
            smck_info['SMCK_CREATION_DATE'] = smck_info['SMCK_CREATION_DATE'].ctime()
            tool_type = smck_info['SMCK_TOOL_TYPE']
            mck_dump_info['MONITORING_TOOL_DICT'][tool_type] = smck_info

        json.dump(mck_dump_info, output_file, ensure_ascii=True, sort_keys=True)
        output_file.close()


    def search(self,flag1="",input1="",print_output_here=""):
        """Search the Oracle database for something.
            input1 is is what is to be searched for.
            flag1 specifies what kind of input input1 is."""

        if self.connected_to_oracle == False:
            print "MaM is not connected to the database, so this function is not available."
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
        if flag1 == "":
            if print_output_here:
                print "search takes two arguments: flag, input."
                print "The input is what is to be searched for."
                print "The flag specifies what the input is. Recognised flags are listed below"
                print "Valid arguments therefore include:"
                print "'MCK_ID',1"
                print "'SMCK_ID',17"
                print "'SMCK_TOOL_PATCH_VERSION','HLTMuonMon_1'"
                print "'SMCK_TOOL_TYPE','HLTMuonMon'"
                print "'MCK_CREATOR','bsmart'"
                print "'SMCK_SLICE_TYPE','Muon'"
                print "Recognised flags are:"

                # print available columns
                for row in database_column_list:
                    # print this column name, with a preceding -
                    print row['COLUMN_NAME']

                print "You can leave the input blank to print all entries for the flag, but you must enter a flag."

            # if we do not have the flag, then we can not search, and so must exit
            return

        if flag1.upper() == 'SMK_ID':
            # the DB uses Super Master Table ID instead of Super Master Key, we accept either
            flag1 = 'SMT_ID'

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

            if print_output_here:
                # explain problem
                print "You have attempted to search for an SMCK_CONFIG."
                print "Unfortunately, due to limitations in Oracle, this is not possible."
                print "This is because the SMCK_CONFIG is stored as a CLOB, and in Oracle it is not possible to perform SQL query comparisons to CLOB objects, (WHERE SMCK_CONFIG = user_input)."
                # explain the alternative
                print "To allow for SMCK_CONFIG comparisons, each config is hashed (using sha512) and hashes are compared."
                print "Your input will now be hashed, and that hash will be searched for."
                print "As this may not be exactly what you intended, do take care when interpreting these search results."
                # convert the flag
                flag1 = 'smck_config_hash'
                # convert the input to a hash
                input1 = self.__get_config_hash__(input1)

        # now lets search
        search_results_list = self.oi.column_search(input1,table_name,column_name)

        # and print our results in a neat manner
        if print_output_here:
            # remind the user what was searched for
            print input1,"has been searched for in the column",column_name,"of table",table_name,"in the Menu-aware Monitoring Oracle database."

            # if no results have been found
            if len(search_results_list) == 0:
                print "No results have been found."
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

        # return the search results
        return search_results_list


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


    def create_sqlite_file_to_copy_to_cool(self,mck,run,runend="",info="",project="",version=""):
        """Create an sqlite file which can be used to manually add data to COOL"""
        # this way https://twiki.cern.ch/twiki/bin/view/AtlasComputing/CoolPublishing#Updating_data_on_the_online_data

        if runend and run >= runend:
            print "runend must be greater than run"
            return

        # check mck exists
        if mck and not self.oi.check_if_mck_id_exists(mck):
            print "MCK",mck,"doesn't exist."
            return

        if mck == 0 and ( not project or not version ):
            print "Please give the AtlasProject and AtlasVersion that you want to write MCK 0 to COOL for."
            print "Usage: create_sqlite_file_to_copy_to_cool(mck,run,runend,info,project,version)"
            print "project and version are compulsory for MCK 0."
            return

        if mck and ( project or version ):
            print "The AtlasProject and AtlasVersion are taken from the MCK info, and should not be specified manually except for MCK 0."
            print "If you want to assign an MCK to a new release consider using the function mam.ms.clone_mck_for_new_release(mck_id,project="",version="")"
            print "Continuing but ignoring project and version variables..."

        # TODO: check that run exists
        # something similar to http://acode-browser.usatlas.bnl.gov/lxr/ident?_i=getOnlineRun

        from PyCool import cool

        # Fodler name, description, versioning mode, database spec
        folder_name = "/TRIGGER/HLT/MenuAwareMonConfigKey"
        desc = '<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type="71" clid="40774348" /></addrHeader><typeName>AthenaAttributeList</typeName>'
        fversion = cool.FolderVersioning.MULTI_VERSION
        dbspec = "sqlite://;schema=MCKfolderforCOOL.sqlite;dbname=CONDBR2"

        # column definition
        schema = [('MonConfigKey', cool.StorageType.UInt32),    # the mck
                  ('Info', cool.StorageType.String4k)]          # optional info

        # create sqlite file
        print 'Creating sqlite file'
        dbSvc = cool.DatabaseSvcFactory.databaseService()
        try:
            db = dbSvc.createDatabase(dbspec)
        except:
            print 'File already exists, please remove/rename'
            return

        # setup record and folder specs
        rspec = cool.RecordSpecification()
        for col, type in schema:
            rspec.extend(col, type)
        fspec = cool.FolderSpecification(fversion, rspec)

        # make folder
        print 'Creating folder', folder_name
        folder = db.createFolder(folder_name, fspec, desc, True)

        # make channel 0
        print 'Creating channel 0 for folder', folder_name
        chan = 0
        folder.createChannel(chan, 'MonConfigKey', 'MCKs tagged by release')

        # setup the payload and tag
        data = cool.Record(rspec)
        data['MonConfigKey'] = mck
        data['Info'] = info
        if mck:
            release = self.oi.read_mck_info_from_db(mck)['MCK_ATHENA_VERSION']
        else:
            release = project + '-' + version
        tag = folder_name.split('/')[-1] + '-' + release

        iov_since = (int(run)<<32)
        if runend:
            iov_until = (int(runend+1)<<32)
        else:
            iov_until = (int(run+1)<<32)
        print "Storing MCK",mck,"with tag",tag,"for IOV",iov_since,"to",iov_until
        folder.storeObject(iov_since,iov_until,data,chan,tag)
