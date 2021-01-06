#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# script to produce a one-to-one copy of the oracle Run 2 MC TriggerDB to sqlite

from TrigConfIO.TriggerConfigAccessBase import ConfigDBLoader
from copy import deepcopy
import sqlite3

def parseCmdline():
    import argparse
    parser = argparse.ArgumentParser(prog = "AtlTriggerDBCopy.py", description="Example: %(prog)s -d TRIGGERDBMC -c triggerDBMC_Run2.db")
    parser.add_argument("-d", "--dbalias", dest="dbalias", help="TriggerDB connection alias for the source DB")
    parser.add_argument("-c", "--create", dest="createfile", help="create sqlite db file")
    parser.add_argument("-v", help="increase output verbosity", action="count", default=0)
    args = parser.parse_args()
    if not args.dbalias:
        print("No source Trigger DB specified")
        parser.print_help()
        return None
    if not args.createfile:
        print("No sqlite target file specified")
        parser.print_help(parser)
        return None
    return args

# class to handle the oracle db connection
# it uses the CORAL authentication method implemented in the TrigConfIO.TriggerConfigAccessBase (ConfigDBLoader)
class DBConnection:
    def __init__(self, dbalias):
        self.dbalias = dbalias
        self.connection = None
        self.schema = None

    def __enter__(self):
        credentials = ConfigDBLoader.getConnectionParameters(self.dbalias)
        self.connection, self.schema = ConfigDBLoader.getConnection(credentials)
        print(f"Opening connection to {self.dbalias}")
        return self

    def __exit__(self, type, value, traceback):
        self.connection.close()
        print(f"Closing connection to {self.dbalias}" )

    def cursor(self):
        return self.connection.cursor()

    def schema(self):
        return self.schema

# class to handle the insertion of schema and data on the sqlite side
class SQLiteInserter:

    def __init__(self, filename = None):
        self.filename = filename
        self.connection = None
        self.activeTransaction = False
        self.insCountInTrans = 0
        self.maxInsertsPerTransaction = 1000
        if self.filename is None:
            print("Not connected to an output file, will print the insert commands instead")
        import os
        if filename and os.path.exists(filename):
            print(f"Target file {filename} exists already, will abort in order to prevent overwriting")
            raise RuntimeError("Target file already exists")
        if self.filename is not None:
            self.connection = sqlite3.connect(self.filename)
            self.cursor = self.connection.cursor()
    def commit(self):
        if self.activeTransaction:
            self.cursor.execute('COMMIT')
            self.activeTransaction = False
            self.insCountInTrans = 0

    def insert(self, statement, data):
        if self.connection is None:
            return            
        if not self.activeTransaction:
            self.cursor.execute('BEGIN TRANSACTION')
            self.activeTransaction = True
        self.cursor.execute(statement, data)
        self.insCountInTrans += 1
        if self.insCountInTrans==self.maxInsertsPerTransaction:
            self.commit()

    def insertBulk(self, statement, data):
        if self.connection is None:
            return
        self.cursor.executemany(statement, iter(data))
        self.connection.commit()

    def createSchema(self, creationFileName):
        if self.connection is None:
            return
        with open(creationFileName) as fh:
            print("Creating sqlite db %s from %s" % (self.filename, creationFileName) )
            self.cursor.executescript( fh.read() )


# class to handle the extraction of schema and data on the oracle side
class OracleExporter:

    def __init__(self, connection):
        self.connection = connection
        self.primaryKeys = None
        self.foreignKeys = None
        self.indexes = None
        self.allTables = None
        self.ignoreTablesCreate = [ 'TT_WRITELOCK', 'HLT_SMT_TO_HRE', 'HLT_PARAMETER', 'HLT_RELEASE', 'TRIGGER_LOG',
                                    'DBCOPY_SOURCE_DATABASE', 'HLT_RULE_SET', 'TEMP', 'HLT_RULE_PARAMETER', 'HLT_RULE_COMPONENT',
                                    'HLT_SETUP', 'TT_USERS', 'HLT_RULE', "HLT_HRC_TO_HRP", "HLT_HRE_TO_HRS",
                                    "HLT_HRS_TO_HRU", "HLT_HRU_TO_HRC", "HLT_PRESCALE_SET_ALIAS", "HLT_PRESCALE_SET_COLL",
                                    "PRESCALE_SET_ALIAS", "TRIGGER_ALIAS", "L1_PRESCALE_SET_ALIAS", "L1_CALO_SIN_COS",
                                    "L1_CI_TO_CSC", "L1_JET_INPUT", "L1_MUON_THRESHOLD_SET"]
        self.ignoreTablesFill = self.ignoreTablesCreate + [ ]
        print("Tables to ignore creation: %r" % len(self.ignoreTablesCreate))
        print("Tables to ignore filling: %r" % len(self.ignoreTablesFill))


    def getTables(self, exceptTables):
        if self.allTables is None:
            query_ListAllTables = "SELECT table_name FROM all_tables WHERE owner=:SCHEMA"
            data = { "SCHEMA" : self.connection.schema }
            result = self.executeQuery(query_ListAllTables, data)
            self.allTables = [x[0] for x in result]
            print("All tables: %i" % len(self.allTables))
        tables = deepcopy(self.allTables)
        if exceptTables is not None:
            for x in exceptTables:
                if x in tables:
                    tables.remove(x)
        return tables

    def executeQuery(self, query, data = {}):
        cursor = self.connection.cursor()
        cursor.arraysize = 1000
        cursor.execute( query, **data )
        return cursor        

    def tableSize(self,tableName):
        query_TableSize = "SELECT COUNT(*) FROM {schema}.{tableName}"
        qdict = { "schema" : self.connection.schema, "tableName" : tableName }
        result = self.executeQuery( query_TableSize.format(**qdict) )
        row = result.fetchone()
        return row[0]

    def columnNames(self,tableName):
        query_ColumnNames = "SELECT column_name, data_type FROM sys.all_tab_columns WHERE owner = :SCHEMA AND table_name = :TABLE_NAME ORDER BY column_id"
        data = { "SCHEMA" : self.connection.schema, "TABLE_NAME" : tableName }
        result = self.executeQuery( query = query_ColumnNames, data = data )
        colNames, colTypes = zip(*result)
        return colNames, colTypes

    def getIndexes(self):
        if self.indexes is not None:
            return self.indexes
        self.indexes = {}
        print("retrieving indexes from Oracle")
        query_Indexes = """
        SELECT table_name, index_name, column_name FROM sys.all_ind_columns WHERE table_owner = :SCHEMA
        """
        data = { "SCHEMA" : self.connection.schema }
        result = self.executeQuery( query = query_Indexes, data = data )
        for table_name, index_name, column_name in result:
            if table_name not in self.indexes:
                self.indexes[table_name] = {}
            self.indexes[table_name][index_name] = column_name
        return self.indexes

    def getPrimaryKeys(self):
        if self.primaryKeys is not None:
            return self.primaryKeys
        print("retrieving primary key constraints from Oracle")
        query_PrimaryKeys = """
        SELECT table_name, column_name, position FROM sys.all_cons_columns WHERE owner = :SCHEMA AND constraint_name in 
        (SELECT constraint_name FROM sys.all_constraints WHERE owner = :SCHEMA AND constraint_type='P') 
        ORDER BY table_name, position
        """
        data = { "SCHEMA" : self.connection.schema }
        pk = {}
        result = self.executeQuery( query = query_PrimaryKeys, data = data )
        for table_name, column_name, _ in result:
            if table_name not in pk:
                pk[table_name] = []
            pk[table_name] += [ column_name ]
        self.primaryKeys = pk
        return self.primaryKeys

    def getForeignKeys(self):
        if self.foreignKeys is not None:
            return self.foreignKeys
        print("retrieving foreign key constraints from Oracle")
        query_ForeignKeys = """
        SELECT c.table_name, c.constraint_name, a.column_name, c.r_constraint_name, r.table_name, r.column_name 
        FROM sys.all_constraints c
        INNER JOIN
        sys.all_cons_columns a
        on (c.constraint_type='R' and c.constraint_name = a.constraint_name and c.owner = :SCHEMA and a.owner = :SCHEMA)
        INNER JOIN
        sys.all_cons_columns r
        on (c.r_constraint_name = r.constraint_name and r.owner = :SCHEMA)
        """        
        data = { "SCHEMA" : self.connection.schema }
        fk = {}
        result = self.executeQuery( query = query_ForeignKeys, data = data )
        for table, _, fkcol, _, rtable, rcol in result:
            if table not in fk:
                fk[table] = []
            fk[table] += [ { "col" : fkcol, "rtab" : rtable, "rcol" : rcol } ]
        self.foreignKeys = fk
        return self.foreignKeys

    def sqliteType(self, oraType):
        if oraType.startswith("TIMESTAMP"):
            return "TEXT"
        d = { "VARCHAR2" : "TEXT",
              "CHAR" : "TEXT",
              "NUMBER" : "INTEGER",
              "CLOB" : "TEXT"
          }
        return d[oraType]

    def tableCreationCommand(self, tableName, primaryKeys, foreignKeys, indexes):
        colNames, colTypes = self.columnNames(tableName)
        lines = []
        for colName, colType in zip(colNames,colTypes):
            lines.append( "%s %s" % (colName, self.sqliteType(colType)) )
        lines.append( "PRIMARY KEY (%s)" % ",".join(primaryKeys) )
        for fk in foreignKeys:
            lines.append( "FOREIGN KEY (%s) REFERENCES %s (%s)" % (fk["col"], fk["rtab"], fk["rcol"]) )
        creationCommand = f"CREATE TABLE IF NOT EXISTS {tableName} (\n    "
        creationCommand += ",\n    ".join(lines)
        creationCommand += "\n);\n"
        for index_name in indexes:
            creationCommand += "CREATE INDEX %s on %s(%s);\n" % (index_name, tableName, indexes[index_name])
        return creationCommand

    def extractSchema(self, creationFileName):
        fk = self.getForeignKeys()
        pk = self.getPrimaryKeys()
        indexes = self.getIndexes()
        with open(creationFileName, "w") as fh:
            print("Creating schema file for sqlite: %s" % creationFileName)
            for tableName in self.getTables(exceptTables = self.ignoreTablesCreate):
                print(self.tableCreationCommand( tableName, pk[tableName], fk.get(tableName, list()), indexes.get(tableName, list())), file = fh)

    def copyTable(self, tableName, sqliteInserter, size):
        # build insert statement
        schema = self.connection.schema
        colNames,_ = self.columnNames(tableName)
        colNameList = ",".join(colNames)
        bindVarList = ",".join(len(colNames)*["?"])
        insertStatement = f"INSERT INTO {tableName} ({colNameList}) VALUES ({bindVarList})"
        # select entries from oracle
        selectQuery = f"SELECT {colNameList} FROM {schema}.{tableName}"
        #print(selectQuery)
        result = self.executeQuery( selectQuery )
        useBulkInsert = True
        if useBulkInsert:
            sqliteInserter.insertBulk(insertStatement, result)
        else:
            c = 0
            for data in result:
                if c%10000==0:
                    print("%i / %i" % (c, size))
                c+=1
                sqliteInserter.insert(insertStatement, data)
                if c==size:
                    break # just doing some timing measurements
        sqliteInserter.commit()


def main():
    args = parseCmdline()
    if args is None:
        return 1

    # instantiate the sqlite inserter with the sqlite db filename
    sqliteInserter = SQLiteInserter(args.createfile)

    # connect to oracle and do the work
    with DBConnection(args.dbalias) as oraConn:
        oraExp = OracleExporter(oraConn)

        # extract the schema into a temporary file and create it on the sqlite side
        tempTableCreationFileName = "tmpCreateTablesSQLite.sql"
        oraExp.extractSchema(tempTableCreationFileName)
        sqliteInserter.createSchema(tempTableCreationFileName)

        # collect some info about the size, to not run completely blind
        entries = {}
        tablesToCreate = oraExp.getTables(exceptTables = oraExp.ignoreTablesCreate)
        tablesToFill = oraExp.getTables(exceptTables = oraExp.ignoreTablesFill)
        print("Tables to create: %i" % len(tablesToCreate))
        print("Tables to fill: %i" % len(tablesToFill))


        for tableName in tablesToCreate:
            entries[tableName] = oraExp.tableSize(tableName)
            doNotCopy = tableName not in tablesToFill
            print("  table %s has %i entries %s" % (tableName, entries[tableName], ("(will not copy)" if doNotCopy else "") ) )
        totalEntries = sum(entries.values())
        print("\nTotal number of entries: %i" %totalEntries)
        
        # copy the data one table at the time
        print("Start copying data")
        copiedEntries = 0
        for tableName in tablesToFill:            
            print("Copying table %s" % tableName, end = '', flush=True)
            oraExp.copyTable(tableName, sqliteInserter, entries[tableName])
            copiedEntries += entries[tableName]
            print(" => done %i / %i (%f%%)" % (copiedEntries, totalEntries, 100 * copiedEntries/totalEntries))

    sqliteInserter.connection.close()


if __name__ == "__main__":
    import sys
    sys.exit(main())
