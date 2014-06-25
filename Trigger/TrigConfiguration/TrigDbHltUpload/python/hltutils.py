#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# $Id: hltutils.py,v 1.4 2007-09-17 10:37:51 stelzer Exp $
#
# Andreas Hoecker, 12/11/2005
#
# utility library
# ------------------------------------------------------------------------
#
import sys # for exiting

dbverbose      = False
traceinsert    = False
tracesqlinsert = False
dumpsqlfile    = None   # will be a filehandle

def dbVerbose(set=None):
    #if not locals().has_key('dbverbose'): # init for the first time
    global dbverbose
    if set!=None:
        dbverbose = set
    return dbverbose

def traceInsert(set=None):    
    global traceinsert
    if set!=None:
        traceinsert = set
    return traceinsert

def dumpSQL(set=None):    
    global tracesqlinsert
    if set!=None:
        tracesqlinsert = set
    return tracesqlinsert

def dumpSQLFile(filename=None):    
    global dumpsqlfile
    if filename==None: return dumpsqlfile           # no arg given, so we just return what we have

    if dumpsqlfile != None and filename!=dumpsqlfile.name: # there is an open file and the name change, we close the old one
        dumpsqlfile.close()
        dumpsqlfile = None

    if dumpsqlfile==None:   # since a new name is given, we need to open new file
        if filename!="":
            dumpsqlfile = open(filename,"w")
        else:
            dumpsqlfile = None
            
    return dumpsqlfile


def get_new_index( cursor, table, column ):
    """retrieve the new PK index"""
    
    query = "SELECT DISTINCT COUNT(*) FROM %s " % (table)
    if dbVerbose(): print type(query), query
    cursor.execute( query )
    row = cursor.fetchone()

    if row[0] == 0: return 1

    query = "SELECT DISTINCT MAX(%s) FROM %s" % (column, table)
    cursor.execute( query )
    row = cursor.fetchone()
    return int(row[0]) + 1 # increment index




# retrieve the new PK index
def get_column( cursor, table, prefix, column, id ):
    query = "SELECT DISTINCT %s.%s_%s FROM %s WHERE %s_ID = %i;" % \
            (table, prefix, column, table, prefix, id)
    
    if dbVerbose(): print type(query), query
    cursor.execute( query )
    row = cursor.fetchone()
    if len(row) == 0:
        print "Error in get_column: entry not found; query: %s" % query
        sys.exit(1)
    return row[0]



def get_lastversion( cursor, table_name, table_prefix, columnList):
  # Test for type of database in use
  oracle_db = False
  mysql_db = False
  if str(type(cursor)).lower().find('oracle') >= 0: oracle_db = True
  if str(type(cursor)).lower().find('mysql') >= 0: mysql_db = True

  query = "SELECT DISTINCT %s.%s_VERSION FROM %s WHERE " % \
          (table_name, table_prefix, table_name)

  for col in columnList:
    if col.test == True:
      if oracle_db and col.name.rfind("TIME") >=0 :
        query += "%s.%s = to_date('%s' , '%s') AND  " % (table_name, col.name, col.value, time_format)
      elif col.type == "S":
        query += "%s.%s = '%s' AND " % (table_name, col.name, format_dbstring(col.value))
      elif col.type == "I":
        query += "%s.%s = %i AND " % (table_name, col.name, int(col.value))
      elif col.type == "F":
        query += "%s.%s = %g AND " % (table_name, col.name, float(col.value))
      else:
        print "*** unknown type in function get_lastversion ==> abort"
        sys.exit(1)

  query = query.rstrip( "AND " )
  query += " ORDER BY %s.%s_VERSION DESC" % (table_name, table_prefix)
  if isinstance(query, unicode): query = str(query)
  if dbVerbose(): print type(query), len(query), query
  cursor.execute( query )
  result = cursor.fetchall()
  if len(result) > 0:
    if dbVerbose():
      print "Last version in %s.%s_VERSION: %s" % (table_name, table_prefix, float(result[0][0]))
    return float(result[0][0])
  else:
    if dbVerbose():
      print "Could not find a version!"
    return 0




def get_rows( cursor, table_name, table_prefix, columnList, order = ""):
    # Test for type of database in use
    oracle_db = False
    mysql_db = False
    if str(type(cursor)).lower().find('oracle') >= 0: oracle_db = True
    if str(type(cursor)).lower().find('mysql') >= 0: mysql_db = True
    
    query = "SELECT DISTINCT %s.%s_ID FROM %s WHERE " % \
            (table_name, table_prefix, table_name)
    for col in columnList:
        if col.test == False: continue
        if col.type == "S":
            query += "%s.%s = '%s' AND " % (table_name, col.name, format_dbstring(col.value))
        elif col.type == "I":
            query += "%s.%s = %i AND " % (table_name, col.name, int(col.value))
        elif col.type == "F":
            query += "%s.%s = %g AND " % (table_name, col.name, float(col.value))
        elif col.type == "T":
            if oracle_db :
                query += "%s.%s = to_date('%s' , '%s') AND  " % (table_name, col.name, col.value, time_format)
            else:
                query += "%s.%s = '%s' AND " % (table_name, col.name, col.value)
        else:
            print "*** unknown type in function check_row ==> abort"
            sys.exit(1)

    query = query.rstrip( "AND " )
    if order != "": query += " ORDER BY %s ASC" % order

    # In case the query is not str or none oracle_db has problems reformat query
    if isinstance(query, unicode): query = str(query)
    if dbVerbose():
        print type(query), query

    cursor.execute( query )

    result = cursor.fetchall()
    if dbVerbose():
        print result
    return result



def get_row_ids( cursor, table_name, table_prefix, columnList, order = ""):
    result = get_rows( cursor, table_name, table_prefix, columnList, order)
    if result!=None and len(result) > 0:
        return [int(x[0]) for x in result]
    else:
        return []



def format_dbstring(str_input):
    """Function formats the character for database reading resp. writing
    returns formatted string"""
    if(isinstance(str_input,str) or isinstance(str_input,unicode)):
        str_output = str_input.replace('\'',"\''")
    else:
        str_output = str_input

    if str_output == "":
        str_output = "~"

    return str_output

