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

#This DB has five tables: master_table, mt2type, type, type2sig, sig_config                      

#c.execute("INSERT INTO cool_table VALUES (1, 'SMK', 'Super Master Key')")

# here we put initial values to smk2mck_table
c.execute("INSERT INTO smk2mck_table VALUES (1, 'SMK2MCK', 1, 1, 'SMK to MCK correspondence')")
c.execute("INSERT INTO smk2mck_table VALUES (2, 'SMK2MCK', 2, 4, 'SMK to MCK correspondence')")
c.execute("INSERT INTO smk2mck_table VALUES (3, 'SMK2MCK', 3, 5, 'SMK to MCK correspondence')")
c.execute("INSERT INTO smk2mck_table VALUES (4, 'SMK2MCK', 4, 6, 'SMK to MCK correspondence')")

# here!!!

# get here active MCK ID
#include("tools.py")
# activeMCK=getActiveKeyMCK('oracle')
#c.execute("INSERT INTO smk2mck_table VALUES (3, 'SMK2MCK', 3, "+str(activeMCK)+", 'SMK to MCK correspondence')")

# Commit to the database the new entries and close the connection                                                                                          
conn.commit()
conn.close()
