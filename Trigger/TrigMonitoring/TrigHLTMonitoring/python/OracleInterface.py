# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Authors: Ben Smart (ben.smart@cern.ch), Xanthe Hoad (xanthe.hoad@cern.ch)
# See https://twiki.cern.ch/twiki/bin/view/Atlas/MaDQM for more information

import sys
# needed for oracle database connection
import cx_Oracle
# for converting configuration dictionaries into strings (for storing in the database as CLOB) and vice versa
import json

# put all oracle interaction stuff in a single class
class OracleInterface:
    "An interface to the Oracle database for Menu-aware Monitoring."

    def connect_to_oracle(self,database_username,database_password,database_name,directory=""):
        "Connect to Oracle directly"

        # connection information
        USER = database_username
        PASSWORD = database_password
        DATASOURCE = database_name

        # connect to the oracle database
        self.conn = cx_Oracle.connect(USER, PASSWORD, DATASOURCE)
        self.cursor = self.conn.cursor()

        # optional directory in which to find the MaM schema in a database
        self.directory = directory
        if self.directory != "":
            self.directory = self.directory+"."

    def connect_to_oracle_via_frontier(self,frontier_server,schema,directory=""):
        "Connect to Oracle via Frontier"

        from TrigConfigSvc.TrigConfFrontier import getFrontierCursor

        self.cursor = getFrontierCursor(frontier_server,schema)

        self.directory = directory
        if self.directory != "":
            self.directory = self.directory+"."

    def disconnect_from_oracle(self):

        try:
            self.conn.close()
            return True
        except cx_Oracle.InterfaceError:
            return False
        else:
            return False

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

    def fetch(self, query, parameters_dict = {}):

        # fetch results, based on a query,
        # optionally providing additional parameters as a dictionary,
        # and return all results
        result = []
        self.cursor.execute(query,parameters_dict)
        try:
            for value in self.cursor:
                if len(value) > 1:
                    listvalue = list(value)
                    for x in range(len(value)):
                        if type(value[x]) is cx_Oracle.LOB:
                            # convert this into a string now in order to prevent access errors which occur with fetchall()
                            listvalue[x] = self.__unicode_to_str__(json.loads(value[x].read()))
                    value = tuple(listvalue)
                result.append(value)
        except:
            for value in self.cursor.result:
                if len(value) > 1:
                    listvalue = list(value)
                    for x in range(len(value)):
                        if type(value[x]) == str and 'MonitCategoryInfo' in value[x]:
                            # convert this into a string now in order to prevent access errors which occur with fetchall()
                            listvalue[x] = self.__unicode_to_str__(json.loads(value[x]))
                    value = tuple(listvalue)
                result.append(value)
        return result

    def insert(self, query, parameters_dict = {}):

        # insert a row,
        # optionally providing additional parameters as a dictionary
        try:
            self.cursor.execute(query,parameters_dict)
            self.conn.commit()
        except:
            print "Insert into DB failed. Your connection is probably of the wrong type"


    def read_default_mck_id_from_db(self,athena_version):
        # return mck_id for default config for Athena version athena_version
        # if default does not exist, return -1
        # if there are multiple defaults for this Athena version (this should not be the case), return -2
        # EDIT: I haven't got the constraint on this to work (yet), so for now it returns the first mck_id

        # construct query to search database for default config
        query = """SELECT """+self.directory+"""mck_table.mck_id \
        FROM """+self.directory+"""mck_table \
        WHERE """+self.directory+"""mck_table.mck_default = 1 \
        AND """+self.directory+"""mck_table.mck_athena_version = :ATHENA_VERSION"""

        # create dictionary of input parameter
        parameters_dict = {}
        parameters_dict['ATHENA_VERSION'] = athena_version

        # perform search and extract results
        search_results = self.fetch(query, parameters_dict)

        # check that there is a default
        if len(search_results) == 0:
            print "There is no default mck in the Oracle database for Athena", athena_version
            return -1

        # check that there is only one default
        if len(search_results) > 1:
            print "There are",len(search_results),"default mck numbers for Athena ", athena_version
            for row in search_results:
                print "default mck_id =",row[0]
            print "Returning the first result",search_results[0][0]
            #return -2

        # extract default mck
        default_mck = search_results[0][0]

        # return the default mck_id for this Athena version
        return default_mck


    def delete_default_mck_id_from_db(self,athena_version):
        # delete the default from the DB for the given athena version, including all associated SMCK

        # check for the default
        default_mck = self.read_default_mck_id_from_db(athena_version)
        if default_mck:
            print default_mck
        else:
            return

        query = """DELETE FROM """+self.directory+"""mck_to_smck_link \
        WHERE """+self.directory+"""mck_to_smck_link.link_mck = :DEFAULT_MCK"""

        # create dictionary of input parameter
        parameters_dict = {}
        parameters_dict['DEFAULT_MCK'] = default_mck

        # perform deletion
        self.cursor.execute(query, parameters_dict)

        query = """DELETE FROM """+self.directory+"""smck_table \
        WHERE """+self.directory+"""smck_table.smck_default = 1 \
        AND """+self.directory+"""smck_table.smck_athena_version = :ATHENA_VERSION"""

        # create dictionary of input parameter
        parameters_dict = {}
        parameters_dict['ATHENA_VERSION'] = athena_version

        # perform deletion
        self.cursor.execute(query, parameters_dict)

        # construct query to search database for default config
        query = """DELETE FROM """+self.directory+"""mck_table \
        WHERE """+self.directory+"""mck_table.mck_default = 1 \
        AND """+self.directory+"""mck_table.mck_athena_version = :ATHENA_VERSION"""

        # perform deletion
        self.cursor.execute(query, parameters_dict)

        default_mck = self.read_default_mck_id_from_db(athena_version)
        if default_mck == -1:
            print 'Operation successful.'
        else:
            print 'Operation failed.'


    def upload_mck_and_smck(self, global_info):
        # upload smck_info if they are not already present in the database
        # upload and link mck if it is not already present in the database
        # return mck_id and smck_ids

        # get the tool/smck info
        tool_list = global_info['MONITORING_TOOL_DICT']

        # dict of new smck
        smck_ids = []
        # make new smck
        for tool_key, tool_info in tool_list.iteritems():
            # upload each smck, and add the smck_id to the list
            # if the smck config already exists, then the existing smck_id will be returned
            smck_ids.append(self.upload_smck(tool_info))

        # check if mck already exists
        mck_id = self.check_if_exact_mck_exists(smck_ids)

        # if the mck does not already exist
        if mck_id == -1:
            # get the mck info
            mck_info = global_info['MCK']
            # make a new mck
            mck_id = self.upload_mck(mck_info)
            # now link the mck to the smck
            for smck_id in smck_ids:
                # create link
                self.upload_mck_smck_link(mck_id,smck_id)

        # return mck_id and smck_ids
        return mck_id, smck_ids


    def upload_mck(self,mck_info):
        # upload new mck
        # return new mck_id
        # new values will be inserted into mck_info for future access if so desired

        # prepare insert data
        insert = {}
        for key in mck_info.keys():
            insert[key] = ":"+key
        insert['MCK_ID'] = self.directory+"seq_mck_table_id.NEXTVAL" # from sequence
        insert['MCK_CREATION_DATE'] = "CURRENT_TIMESTAMP"

        # construct input strings for the query
        key_str = ", ".join(insert.keys())
        val_str = ", ".join(insert.values())

        # construct the query
        # new values will be inserted into mck_info for future access if so desired
        query = "INSERT INTO "+self.directory+"mck_table (%s) VALUES (%s) RETURNING mck_id, mck_creation_date into :MCK_ID, :MCK_CREATION_DATE" % (key_str, val_str)

        # prepare the mck_info
        # we must ensure that mck_info variables are of the correct object-type to accept the new mck data
        mck_info['MCK_ID'] = self.cursor.var(cx_Oracle.NUMBER)
        mck_info['MCK_CREATION_DATE'] = self.cursor.var(cx_Oracle.TIMESTAMP)

        # insert the mck data, and get the new mck_id
        self.insert(query,mck_info)

        # now we need to add the new data back into mck_info in useful formats
        mck_info['MCK_ID'] = mck_info['MCK_ID'].getvalue()
        mck_info['MCK_CREATION_DATE'] = mck_info['MCK_CREATION_DATE'].getvalue()

        # return the new mck_id
        return mck_info['MCK_ID']

    def upload_smck(self,smck_info):
        # check if smck_info is already in database
        # if so, return existing smck_id
        # else upload smck_info and return new smck_id

        # check if smck_info is already in database
        smck_id = self.check_if_smck_exists(smck_info)

        # if it is not in the database
        if smck_id == -1:
            # get new smck_tool_patch_version
            smck_info['SMCK_TOOL_PATCH_VERSION'] = self.get_next_smck_tool_patch_version(smck_info['SMCK_TOOL_TYPE'])
            # prepare insert data
            insert = {}
            for key in smck_info.keys():
                insert[key] = ":"+key

            # prepare insert data for things that are set by the server
            insert['SMCK_ID'] = self.directory+"seq_smck_table_id.NEXTVAL" # from sequence
            insert['SMCK_CREATION_DATE'] = "CURRENT_TIMESTAMP"

            # construct input strings for the query
            key_str = ", ".join(insert.keys())
            val_str = ", ".join(insert.values())

            # construct the query
            # new values will be inserted into smck_info for future access if so desired
            query = "INSERT INTO "+self.directory+"smck_table (%s) VALUES (%s) RETURNING smck_id, smck_creation_date into :SMCK_ID, :SMCK_CREATION_DATE" % (key_str, val_str)

            # prepare the smck_info
            # we must ensure that smck_info variables are of the correct object-type to accept the new smck data
            smck_info['SMCK_ID'] = self.cursor.var(cx_Oracle.NUMBER)
            smck_info['SMCK_CREATION_DATE'] = self.cursor.var(cx_Oracle.TIMESTAMP)

            # we must build an smck_info_to_submit,
            # as smck_info contains a dict for smck_config,
            # but we need to insert a json string
            smck_info_to_submit = smck_info.copy()
            smck_info_to_submit['SMCK_CONFIG'] = json.dumps(smck_info['SMCK_CONFIG'], ensure_ascii=True, sort_keys=True)

            # insert the smck data, and get the new smck_id
            self.insert(query,smck_info_to_submit)

            # now we need to add the new data back into smck_info in useful formats
            smck_info['SMCK_ID'] = int(smck_info_to_submit['SMCK_ID'].getvalue())
            smck_info['SMCK_CREATION_DATE'] = smck_info_to_submit['SMCK_CREATION_DATE'].getvalue()

            # return the new smck_id
            return smck_info['SMCK_ID']

        # if the smck_info is already in database
        else:
            # get existing smck_info
            existing_smck_info = self.read_smck_info_from_db(smck_id)
            # fill with the existing values
            smck_info['SMCK_ID'] = smck_id
            smck_info['SMCK_TOOL_PATCH_VERSION'] = existing_smck_info['SMCK_TOOL_PATCH_VERSION']

            # return existing smck_id
            return smck_id


    def upload_mck_smck_link(self,mck_id,smck_id):
        # create link between mck_id and smck_id

        # make query to insert link
        query = """ INSERT INTO """+self.directory+"""mck_to_smck_link \
        (link_mck, link_smck) VALUES (:MCK_ID, :SMCK_ID)"""

        # create dictionary of input parameter
        parameters_dict = {}
        parameters_dict['MCK_ID'] = mck_id
        parameters_dict['SMCK_ID'] = smck_id

        # insert this into the database
        self.insert(query,parameters_dict)


    def check_if_exact_mck_exists(self,smck_ids):
        # check if exactly this mck exists (if an mck exists with exactly the same smck links)
        # if not, return -1

        # check if mck already exists
        search_results = self.check_if_mck_exists(smck_ids)
        # check if there is an exact match
        # if not, leave mck_id = -1
        mck_id = -1

        # first check if links were found
        if search_results != -1:
            # loop over possible matches
            for row_mck_id in search_results:
                # check if this is an exact match
                if len(self.read_mck_links_from_db(row_mck_id)) == len(smck_ids):
                    # if so, this is the existing mck
                    mck_id = row_mck_id
                    break

        # return mck_id
        return mck_id


    def check_if_mck_exists(self,smck_ids):
        # check if an mck exists that links to all the smck in the input smck_ids list
        # if it does, return the search results (list of mck_id), otherwise return -1
        # note: multiple mck_id may be found

        # construct a query with a dummy 1=1 so that all subsequent lines start with AND
        query = "SELECT "+self.directory+"mck_table.mck_id FROM "+self.directory+"mck_table WHERE 1=1 "

        # for each smck_id, find mck that link to that smck
        # returned mck_id must link to all smck
        for smck_id in smck_ids:
            # add a sub-query for each smck
            query += "AND "+self.directory+"mck_table.mck_id IN (SELECT "+self.directory+"mck_to_smck_link.link_mck FROM "+self.directory+"mck_to_smck_link WHERE "+self.directory+"mck_to_smck_link.link_smck = "+str(smck_id)+" ) "

        # search with this query
        search_results = self.fetch(query)

        # if there are results, return them (mck_id)
        # otherwise return -1
        if len(search_results) > 0:
            # return list of mck_id
            # need to use read_mck_links_from_db(mck_id) on each mck_id to check if that mck is an exact match (links to the same, ie. same number of, smck as in the input smck_ids)
            return_list = []
            for row in search_results:
                return_list.append(row[0])
            return return_list
        else:
            return -1

    def check_if_mck_id_exists(self,mck_id):
        # check if an mck exists with the requested mck_id

        if not isinstance(mck_id, int):
            return False

        if mck_id == 0:
            return False

        # construct a query
        query = "SELECT "+self.directory+"mck_table.mck_id FROM "+self.directory+"mck_table WHERE mck_id = "+str(mck_id)

        # search with this query
        search_results = self.fetch(query)

        # if there are results, return them (mck_id)
        # otherwise return -1
        if len(search_results) > 0:
            return True
        else:
            return False

    def check_if_smck_exists(self,smck_info):
        # check if an existing smck contains the same config as the input smck_info
        # if so, return the existing smck_id
        # else return -1

        # construct a query
        query = """SELECT """+self.directory+"""smck_table.smck_id FROM """+self.directory+"""smck_table \
        WHERE """+self.directory+"""smck_table.smck_config_hash = :SMCK_CONFIG_HASH \
        AND   """+self.directory+"""smck_table.smck_tool_type = :SMCK_TOOL_TYPE \
        AND   """+self.directory+"""smck_table.smck_processing_step = :SMCK_PROCESSING_STEP \
        AND   """+self.directory+"""smck_table.smck_processing_stream = :SMCK_PROCESSING_STREAM """

        # construct a dict for the input parameter (smck_config)
        parameters_dict = {}
        #parameters_dict['SMCK_CONFIG'] = json.dumps(smck_info['SMCK_CONFIG'], ensure_ascii=True, sort_keys=True)
        parameters_dict['SMCK_CONFIG_HASH'] = smck_info['SMCK_CONFIG_HASH']
        parameters_dict['SMCK_TOOL_TYPE'] = smck_info['SMCK_TOOL_TYPE']
        parameters_dict['SMCK_PROCESSING_STEP'] = smck_info['SMCK_PROCESSING_STEP']
        parameters_dict['SMCK_PROCESSING_STREAM'] = smck_info['SMCK_PROCESSING_STREAM']

        # perform the search
        search_results = self.fetch(query,parameters_dict)

        # if there are results, return them (smck_id)
        # otherwise return -1
        if len(search_results) > 0:
            # in the case that there are multiple smck_id (there shouldn't be), then we just return the first
            if len(search_results) > 1:
                print "More than 1 smck_id found that matches input smck_info:"
                print "SMCK_TOOL_TYPE: ", smck_info['SMCK_TOOL_TYPE']
                print "SMCK_PROCESSING_STEP: ", smck_info['SMCK_PROCESSING_STEP']
                print "SMCK_PROCESSING_STREAM: ", smck_info['SMCK_PROCESSING_STREAM']
                print "SMCK_CONFIG: ", smck_info['SMCK_CONFIG']
                print "SMCK_CONFIG_HASH: ", smck_info['SMCK_CONFIG_HASH']
                for row in search_results:
                    print "SMCK_ID: ", row[0]
            return search_results[0][0]
        else:
            return -1


    def read_mck_links_from_db(self,mck_id):
        # return list of smck linked to this mck

        # construct the query
        query = """SELECT """+self.directory+"""mck_to_smck_link.link_smck FROM """+self.directory+"""mck_to_smck_link \
        WHERE """+self.directory+"""mck_to_smck_link.link_mck = :MCK_ID"""

        # construct the dict of the input mck_id
        parameters_dict = {}
        parameters_dict['MCK_ID'] = mck_id

        # perform the search
        search_results = self.fetch(query,parameters_dict)

        # if there are results, return them (smck_id)
        # otherwise return -1
        if len(search_results) > 0:
            # return list of smck_id
            return_list = []
            for row in search_results:
                return_list.append(row[0])
            return return_list
        else:
            return -1


    def read_mck_info_from_db(self,mck_id):
        # return mck_info of mck with mck_id

        # construct the query
        query = """SELECT * FROM """+self.directory+"""mck_table \
        WHERE """+self.directory+"""mck_table.mck_id = :MCK_ID"""

        # construct the dict of the input mck_id
        parameters_dict = {}
        parameters_dict['MCK_ID'] = mck_id

        # perform the search
        search_results = self.fetch(query,parameters_dict)

        # if there are results, return them
        # otherwise return -1
        if len(search_results) > 0:

            # create mck_info with returned data
            mck_info = {}
            mck_info['MCK_ID'] =             search_results[0][0]
            mck_info['MCK_DEFAULT'] =        search_results[0][1]
            mck_info['MCK_ATHENA_VERSION'] = search_results[0][2]
            mck_info['MCK_CREATOR'] =        search_results[0][3]
            mck_info['MCK_CREATION_DATE'] =  search_results[0][4]
            mck_info['MCK_COMMENT'] =        search_results[0][5]

            # return filled mck_info
            return mck_info

        else:
            return -1


    def read_smck_info_from_db(self,smck_id):
        # return smck_info of smck with smck_id

        # construct the query
        query = """SELECT * FROM """+self.directory+"""smck_table \
        WHERE """+self.directory+"""smck_table.smck_id = :SMCK_ID"""

        # construct the dict of the input mck_id
        parameters_dict = {}
        parameters_dict['SMCK_ID'] = smck_id

        # perform the search
        search_results = self.fetch(query,parameters_dict)

        # if there are results, return them
        # otherwise return -1
        if len(search_results) > 0:

            # create smck_info with returned data
            smck_info = {}
            smck_info['SMCK_ID'] =                 search_results[0][0]
            smck_info['SMCK_SLICE_TYPE'] =         search_results[0][1]
            smck_info['SMCK_TOOL_TYPE'] =          search_results[0][2]
            smck_info['SMCK_TOOL_PATCH_VERSION'] = search_results[0][3]
            smck_info['SMCK_PROCESSING_STEP'] =    search_results[0][4]
            smck_info['SMCK_PROCESSING_STREAM'] =  search_results[0][5]
            #smck_info['SMCK_CONFIG'] =             json.loads(search_results[0][6].read()) # read() required to extract CLOB data
            smck_info['SMCK_CONFIG'] =             search_results[0][6]
            smck_info['SMCK_CONFIG_HASH'] =        search_results[0][7]
            smck_info['SMCK_DEFAULT'] =            search_results[0][8]
            smck_info['SMCK_ATHENA_VERSION'] =     search_results[0][9]
            smck_info['SMCK_SVN_TAG'] =            search_results[0][10]
            smck_info['SMCK_CREATOR'] =            search_results[0][11]
            smck_info['SMCK_CREATION_DATE'] =      search_results[0][12]
            smck_info['SMCK_COMMENT'] =            search_results[0][13]

            # return filled smck_info
            return smck_info

        else:
            return -1


    def get_smck_id_from_smck_tool_patch_version(self,smck_tool_patch_version):
        # given an smck_tool_patch_version, return the smck_id

        # construct the query
        query = """SELECT """+self.directory+"""smck_table.smck_id FROM """+self.directory+"""smck_table \
        WHERE """+self.directory+"""smck_table.smck_tool_patch_version = :SMCK_TOOL_PATCH_VERSION"""

        # construct the dict of the input smck_tool_patch_version
        parameters_dict = {}
        parameters_dict['SMCK_TOOL_PATCH_VERSION'] = smck_tool_patch_version

        # perform the search
        search_results = self.fetch(query,parameters_dict)

        # if there are results, return them (smck_id)
        # otherwise return -1
        if len(search_results) > 0:
            # return the smck_id
            return search_results[0][0]
        else:
            return -1


    def get_smck_tool_patch_version_from_smck_id(self,smck_id):
        # given an smck_id, return the smck_tool_patch_version

        # construct the query
        query = """SELECT """+self.directory+"""smck_table.smck_tool_patch_version FROM """+self.directory+"""smck_table \
        WHERE """+self.directory+"""smck_table.smck_id = :SMCK_ID"""

        # construct the dict of the input smck_id
        parameters_dict = {}
        parameters_dict['SMCK_ID'] = smck_id

        # perform the search
        search_results = self.fetch(query,parameters_dict)

        # if there are results, return them (smck_tool_patch_version)
        # otherwise return -1
        if len(search_results) > 0:
            # return the smck_tool_patch_version
            return search_results[0][0]
        else:
            return -1


    def get_next_smck_tool_patch_version(self,input_tool_type):

        # construct a query to find all smck_tool_patch_version of this input_tool_type
        query = """SELECT """+self.directory+"""smck_table.smck_tool_patch_version \
        FROM """+self.directory+"""smck_table \
        WHERE """+self.directory+"""smck_table.smck_tool_type=:INPUT_TOOL_TYPE \
        ORDER BY """+self.directory+"""smck_table.smck_id DESC """

        # construct the dict of the input smck_id
        parameters_dict = {}
        parameters_dict['INPUT_TOOL_TYPE'] = input_tool_type

        # perform the search
        search_results = self.fetch(query,parameters_dict)

        # get largest smck_tool_patch_version
        largest_smck_tool_patch_version = -1
        if len(search_results) > 0:
            largest_smck_tool_patch_version = search_results[0][0]

        # make new smck_tool_patch_version
        if largest_smck_tool_patch_version < 0:
            return input_tool_type+"_1"
        else:
            version_number = int(largest_smck_tool_patch_version.split("_")[1])
            return input_tool_type+"_"+str(version_number+1)


    def get_db_tables_and_columns(self):

        # get data on all columns, and associated table names and column data
        # hard-code table names for now, until a better solution is found
        table_names = ['MCK_TABLE','SMCK_TABLE','MCK_TO_SMCK_LINK','MCK_TO_SMK_LINK','SUPER_MASTER_TABLE']

        return_list = []

        try:
            for table in table_names:

                query = "SELECT * FROM "+self.directory+table
                search_results = self.cursor.execute(query).description

                # loop over rows in search results
                for row in search_results:

                    # construct a dictionary of the results
                    row_dict = {}
                    row_dict['TABLE_NAME'] = table.upper()
                    row_dict['COLUMN_NAME'] = row[0].upper()
                    row_dict['DATA_TYPE'] = row[1]
                    row_dict['DATA_LENGTH'] = row[2]

                    # add this dictionary to the return list
                    return_list.append(row_dict)

            # return the return list
            return return_list
        except:
            print "Cannot get database table and column names. Your database connection is probably of the wrong type."


    def get_db_tables_and_columns_old(self):

        # This does not work on the main MaM databases (point 1 and its replica), but is left here for reference
        # (or at least until a fix is found for the above version of the function such that table names don't need to be hardcoded)
        #
        # get data on all columns, and associated table names and column data
        # construct a query
        query = "SELECT table_name, column_name, data_type, data_length FROM "+self.directory+"USER_TAB_COLUMNS"

        # perform the search
        search_results = self.fetch(query)

        # return a list of dictionaries
        return_list = []

        # loop over rows in search results
        for row in search_results:

            # construct a dictionary of the results
            row_dict = {}
            row_dict['TABLE_NAME'] = row[0].upper()
            row_dict['COLUMN_NAME'] = row[1].upper()
            row_dict['DATA_TYPE'] = row[2].upper()
            row_dict['DATA_LENGTH'] = row[3]

            # add this dictionary to the return list
            return_list.append(row_dict)

        # return the return list
        return return_list


    def column_search(self,input1,table_name,column_name):

        # search in table table_name for entries/rows where column_name=input1
        # return a list of dictionaries (one dict per row)

        # list to return
        return_list = []

        # first we get the schema
        database_schema = self.get_db_tables_and_columns()

        # test that the table and column names are valid
        table_and_column_match = False
        for row in database_schema:
            # check if the table and column match
            if row['TABLE_NAME'] == table_name.upper() and row['COLUMN_NAME'] == column_name.upper():
                # we have a match
                table_and_column_match = True
                break

        # if there was no match
        if not table_and_column_match:
            # no match, so return the empty list
            return return_list

        # construct the dict of the various input
        parameters_dict = {}

        # now we construct a query
        query = "SELECT * FROM "+self.directory+table_name
        if input1:
            query = query+" WHERE "+column_name+" = :INPUT1"
            parameters_dict['INPUT1'] = input1

        # perform the search
        search_results = self.fetch(query,parameters_dict)

        # get list of columns in this table
        column_list = []
        for schema_row in database_schema:

            # if this row belongs to the desired table
            if schema_row['TABLE_NAME'] == table_name:
                # then we add this column name to our list
                # we need to add it to the start of the list (insert before element 0),
                # as the database table/column search function returns the results in reversed order(?)
                # column_list.insert(0,schema_row['COLUMN_NAME'])
                # it looks to me like the database table/column search function returns the columns in the right order, so do that
                column_list.append(schema_row['COLUMN_NAME'])

        # loop over results
        for result_row in search_results:
            # the length of column_list should be the same as the length of result_row
            if len(column_list) != len(result_row):
                # something has gone very wrong
                print "ERROR in OracleInterface.column_search(",input1,",",table_name,",",column_name,")"
                print "column_list =",column_list
                print "result_row =",result_row
                return return_list

            # we can zip up column_list and result_row to make a dictionary
            row_dict = dict( zip( column_list, result_row) )

            # need to check if this is an smck,
            # in which case we need to turn the json CLOB into a dict
            if table_name == 'SMCK_TABLE':
                # first we read out the CLOB
                # then we turn the json string into a dict
                #row_dict['SMCK_CONFIG'] = json.loads(row_dict['SMCK_CONFIG'].read())
                #this is already done
                pass

            # add this dict to the return list
            return_list.append(row_dict)

        # now our list of dictionaries should be complete, so we return it
        return return_list

    def get_smk_list(self,print_list=True):
        # get all SMK IDs in a format that the GUI can handle

        query = """SELECT SMT_ID FROM """+self.directory+"""super_master_table """
        results = self.fetch(query)
        smk_ids = []
        for smk in results:
            smk_ids.append(smk[0])
        smk_ids.sort()
        if print_list==True:
            for smk in smk_ids:
                print smk
        return smk_ids

    def get_mck_list(self,print_list=True):
        # get all MCK IDs in a format that the GUI can handle

        query = """SELECT MCK_ID FROM """+self.directory+"""mck_table """
        results = self.fetch(query)
        mck_ids = []
        for mck in results:
            mck_ids.append(mck[0])
        mck_ids.sort()
        if print_list==True:
            for mck in mck_ids:
                print mck
        return mck_ids

    def get_smck_list(self,print_list=True):
        # get all SMCK IDs in a format that the GUI can handle

        query = """SELECT SMCK_ID FROM """+self.directory+"""smck_table """
        results = self.fetch(query)
        smck_ids = []
        for smck in results:
            smck_ids.append(smck[0])
        smck_ids.sort()
        if print_list==True:
            for smck in smck_ids:
                print smck
        return smck_ids

    def check_if_smk_to_mck_link_exists_and_is_active(self,smk,mck):
        # return True of False, depending on whether this exact mck-smk link exists and is active

        # construct the query
        query = """SELECT * FROM """+self.directory+"""mck_to_smk_link \
        WHERE """+self.directory+"""mck_to_smk_link.smk_link_mck = :MCK \
        AND """+self.directory+"""mck_to_smk_link.smk_link_smk = :SMK \
        AND """+self.directory+"""mck_to_smk_link.smk_link_active_mck = :ACTIVE_LINK """

        # construct the dict of the input mck_id and smk
        parameters_dict = {}
        parameters_dict['MCK'] = mck
        parameters_dict['SMK'] = smk
        parameters_dict['ACTIVE_LINK'] = '1'

        # perform the search
        search_results = self.fetch(query,parameters_dict)

        # if there are results, return True
        # otherwise return False
        if len(search_results) > 0:
            # we have found a match, so return True
            return True
        else:
            # no match has been found, so return False
            return False

    def check_if_smk_to_mck_link_exists(self,smk,mck):
        # return True of False, depending on whether this exact mck-smk link exists

        # construct the query
        query = """SELECT * FROM """+self.directory+"""mck_to_smk_link \
        WHERE """+self.directory+"""mck_to_smk_link.smk_link_mck = :MCK \
        AND """+self.directory+"""mck_to_smk_link.smk_link_smk = :SMK """

        # construct the dict of the input mck_id and smk
        parameters_dict = {}
        parameters_dict['MCK'] = mck
        parameters_dict['SMK'] = smk

        # perform the search
        search_results = self.fetch(query,parameters_dict)

        # if there are results, return True
        # otherwise return False
        if len(search_results) > 0:
            # we have found a match, so return True
            return True
        else:
            # no match has been found, so return False
            return False

    def find_active_smk_to_mck_link(self,smk):
        # returns details of link or an empty list depending on whether an smk link exists for this smk and is active

        # construct the query
        query = """SELECT * FROM """+self.directory+"""mck_to_smk_link \
        WHERE """+self.directory+"""mck_to_smk_link.smk_link_smk = :SMK \
        AND """+self.directory+"""mck_to_smk_link.smk_link_active_mck = :ACTIVE_LINK """

        # construct the dict of the input smk
        parameters_dict = {}
        parameters_dict['SMK'] = smk
        parameters_dict['ACTIVE_LINK'] = '1'

        # perform the search
        search_results = self.fetch(query,parameters_dict)

        return search_results

    def deactivate_all_links_for_given_smk_except_for_given_mck(self,smk,mck):
        # for a given smk, deactivate all existing smk-mck links
        # by setting smk_link_active_mck='0'
        # except for input_mck_id (which is ignored)

        # make query to update links
        query = """ UPDATE """+self.directory+"""mck_to_smk_link \
        SET """+self.directory+"""mck_to_smk_link.smk_link_active_mck = :ACTIVE_LINK \
        WHERE """+self.directory+"""mck_to_smk_link.smk_link_smk = :SMK \
        AND """+self.directory+"""mck_to_smk_link.smk_link_mck != :MCK """

        # create dictionary of input parameter
        parameters_dict = {}
        parameters_dict['MCK'] = mck
        parameters_dict['SMK'] = smk
        parameters_dict['ACTIVE_LINK'] = '0'

        # insert this into the database
        self.insert(query,parameters_dict)

    def deactivate_smk_mck_link(self,smk,mck):
        # for a given smk, deactivate all existing smk-mck links
        # by setting smk_link_active_mck='0'

        # make query to update links
        query = """ UPDATE """+self.directory+"""mck_to_smk_link \
        SET """+self.directory+"""mck_to_smk_link.smk_link_active_mck = :ACTIVE_LINK \
        WHERE """+self.directory+"""mck_to_smk_link.smk_link_smk = :SMK \
        AND """+self.directory+"""mck_to_smk_link.smk_link_mck = :MCK """

        # create dictionary of input parameter
        parameters_dict = {}
        parameters_dict['SMK'] = smk
        parameters_dict['MCK'] = mck
        parameters_dict['ACTIVE_LINK'] = '0'

        # insert this into the database
        self.insert(query,parameters_dict)

    def activate_smk_mck_link(self,smk,mck):
        # he given smk-mck link
        # by setting smk_link_active_mck='1'

        # make query to update links
        query = """ UPDATE """+self.directory+"""mck_to_smk_link \
        SET """+self.directory+"""mck_to_smk_link.smk_link_active_mck = :ACTIVE_LINK \
        WHERE """+self.directory+"""mck_to_smk_link.smk_link_smk = :SMK \
        AND """+self.directory+"""mck_to_smk_link.smk_link_mck = :MCK """

        # create dictionary of input parameter
        parameters_dict = {}
        parameters_dict['SMK'] = smk
        parameters_dict['MCK'] = mck
        parameters_dict['ACTIVE_LINK'] = '1'

        # insert this into the database
        self.insert(query,parameters_dict)

    def deactivate_all_links_for_given_smk(self,smk):
        # for a given smk, deactivate all existing smk-mck links
        # by setting smk_link_active_mck='0'

        # make query to update links
        query = """ UPDATE """+self.directory+"""mck_to_smk_link \
        SET """+self.directory+"""mck_to_smk_link.smk_link_active_mck = :ACTIVE_LINK \
        WHERE """+self.directory+"""mck_to_smk_link.smk_link_smk = :SMK """

        # create dictionary of input parameter
        parameters_dict = {}
        parameters_dict['SMK'] = smk
        parameters_dict['ACTIVE_LINK'] = '0'

        # insert this into the database
        self.insert(query,parameters_dict)


    def check_if_smk_exists(self,smk_id):

        # construct the query
        query = """SELECT * FROM """+self.directory+"""super_master_table \
            WHERE """+self.directory+"""super_master_table.smt_id = :SMK """
        #smt_name

        # construct the dict of the input smk
        parameters_dict = {}
        parameters_dict['SMK'] = smk_id

        # perform the search
        search_results = self.fetch(query,parameters_dict)

        return search_results


    def upload_mck_to_smk_link(self,mck_id,smk,creator,comment=""):
        # create link between mck_id and smk

        # make query to insert link
        query = """ INSERT INTO """+self.directory+"""mck_to_smk_link \
        (smk_link_mck, smk_link_smk, smk_link_active_mck, smk_link_creator, smk_link_creation_date, smk_link_comment) \
        VALUES (:MCK_ID, :SMK, :ACTIVE_LINK, :CREATOR, CURRENT_TIMESTAMP, :USER_COMMENT)"""

        # create dictionary of input parameter
        parameters_dict = {}
        parameters_dict['MCK_ID'] = mck_id
        parameters_dict['SMK'] = smk
        parameters_dict['ACTIVE_LINK'] = '0'
        parameters_dict['CREATOR'] = creator
        parameters_dict['USER_COMMENT'] = comment

        # insert this into the database
        self.insert(query,parameters_dict)


    def get_all_mck_to_smk_links(self):

        # construct a query to find all mck_to_smk links, ordered by smk
        query = """SELECT * \
        FROM """+self.directory+"""mck_to_smk_link \
        ORDER BY """+self.directory+"""mck_to_smk_link.smk_link_smk DESC """

        # perform the search
        search_results = self.fetch(query)

        # for each mck_to_smk link, add a dict to the return list
        return_list = []
        for row in search_results:
            this_dict = {}
            this_dict['MCK']           = row[0]
            this_dict['SMK']           = row[1]
            this_dict['ACTIVE']        = row[2]
            this_dict['CREATOR']       = row[3]
            this_dict['CREATION_DATE'] = row[4]
            this_dict['COMMENT']       = row[5]
            return_list.append(this_dict)

        # return the list of link dicts
        return return_list
