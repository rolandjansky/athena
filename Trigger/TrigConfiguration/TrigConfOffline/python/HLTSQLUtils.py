# $Id: HLTSQLUtils.py,v 1.7 2009/03/02 16:25:20 nozicka Exp $
# Created by Miroslav Nozicka <nozicka@mail.desy.de>

def get_cursor (host, user, db="", type="mysql", ask_passwd=True, passwd=""):

  """ Gets a python DB cursor for a given triple: db, host, user. The passwd
  flag can be used to trigger a little trigger interactive password request to
  the user. The type can be either 'mysql' or 'oracle' and that will return one
  of the two different cursor implementations.  """

  if type.lower() == "mysql":
    return _get_mysql_cursor(host, db, user, ask_passwd, passwd)
  elif type.lower() == "oracle":
    return _get_oracle_cursor(host, user, ask_passwd, passwd)
  elif type.lower() == "sqlite":
    return _get_sqlite_cursor(host)
  
def _get_oracle_cursor (tns, user, ask_passwd=True, passwd=""):
  """Returns an Oracle cursor"""
  try:
    from cx_Oracle import connect
  except ImportError:
    print "ERROR: Have you setup your Oracle environment correctly?"
    print "You have to set these enviroment variables to make it work:"
    print "LD_LIBRARY_PATH: include the directory of the client libraries"
    print "TNS_ADMIN: include the TNS definitions %s" % \
          "(e.g.: /afs/cern.ch/project/oracle/admin)"
    sys.exit(2)
  
  if ask_passwd:
    from getpass import getpass
    passwd = getpass("[Oracle] database password for %s@%s: " % (user, tns))
    connection = connect (user, passwd, tns, threaded=True)
  else:
    connection = connect (user, passwd, tns, threaded=True)
  return connection.cursor()

def _get_mysql_cursor (host, db, user, ask_passwd=True, passwd=""):
  """Returns a MySQL cursor"""

  from MySQLdb import connect
  from getpass import getpass
  
  if ask_passwd:
    passwd = getpass("[MySQL] `%s' database password for %s@%s: " %
                    (db, user, host))
  connection = connect (host=host, user=user, passwd=passwd, db=db, connect_timeout=10)
  return connection.cursor()

def _get_sqlite_cursor(file):
  """Returns a SQLite cursor"""
  from sqlite3 import dbapi2 as sqlite
  connection = sqlite.connect(file)
  return connection.cursor()


