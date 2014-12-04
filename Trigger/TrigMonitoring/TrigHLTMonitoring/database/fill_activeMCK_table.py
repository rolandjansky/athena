# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#import sqlite3
import hashlib
import cx_Oracle
conn = cx_Oracle.connect('tklimk/IiUTYhtOYL955!@devdb11')
print conn.version
#conn.close()



#tkconn = sqlite3.connect('test.db') #test.db is a database that gets created with ./createDB.sh

#You can browse the databse with:
#>sqlite3 test.db
#and then inside the interactive environment:
#.tables
#(see more options with .help)


c = conn.cursor()

#c.execute("SELECT * FROM sig_config")
#r = c.fetchone()                                                                                
#print r                                                                                         


c.execute("INSERT INTO activeMCK_table VALUES (1, 'activeMCK', 4)")

# Commit to the database the new entries and close the connection                                                                                          
conn.commit()
conn.close()
