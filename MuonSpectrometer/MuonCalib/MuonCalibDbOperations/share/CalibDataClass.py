#!/bin/env python

import cx_Oracle
import sys
import traceback
import time
import datetime
import calendar
import re
import os
import subprocess

# database configuration info
import CalibDbInfo

# if ATLAS env is initialized import from there but fallback to copy in working dir if available
try:
	from MuonCalibIdentifier.MuonFixedIdUnpack import *
except ImportError:
	from MuonFixedIdUnpack import *

class CalibDataError(Exception):
	def __init__(self, caller, exc=None):
		if caller.debug == True:
			caller.ptrace()
		if isinstance(exc,cx_Oracle.Error):
			error, = exc.args
			if isinstance(error,str):
				self.args += error,
			else:
				self.args += error.message,
			return
		if isinstance(exc,str):
			self.args += exc,
			return
		if isinstance(exc,Exception):
			self.args = exc.args
			return	

class HeadInsertError(CalibDataError): pass
class RTInsertError(CalibDataError): pass
class T0InsertError(CalibDataError): pass
class ADCInsertError(CalibDataError): pass
class MapInsertError(CalibDataError): pass
class DeleteError(CalibDataError): pass
class QueryError(CalibDataError): pass
class UpdateError(CalibDataError): pass
class DataUniqueError(CalibDataError): pass
class DataCopyError(CalibDataError): pass

class CalibData:
	service_re = re.compile('\(SERVICE_NAME\s*\=\s*([A-Za-z.]+)\)+')
	# unix timestamp
	unixts = re.compile('[0-9]{10,11}')

	def __init__(self,head_id=None,impl=None,lr=None,ur=None,lt=None,ut=None,luminosity=None,rootfile=None,statusflag=None):
     
      	# counter of operations not yet committed
		self.opcount = 0 
		# committed operations
		self.optotal = 0
		# increments when transaction is committed
		self.transtotal = 0
		# total time for database ops (seconds)
		self.dbtime = { 'wtime': 0, 'ptime': 0 }
		#internal timer used to calculate timing intervals
		self.timer = { 'wtime':0, 'ptime': 0 }
		# variable to "lock" timer if engaged
		self.timelock = False

		# break operations up into transactions less than maxops?
		# this really only fully applies to delete and batch insert operations from files (not t0 or rt copying operations).  
		# For a copy operation, if maxops is less than the number of rows inserted into a single table at once then 
		# it will cause operations to be broken up into a single transaction for each insert (so insert into MDT_TUBE, commit, then MDT_TUBE_C, commit, etc)  
		# It's always been broken for the case of batch copies, and there doesn't seem any point to fixing it now since we've done 
		# away with Oracle streams and don't need the per-transaction row limiting enabled - in fact breaking any mass insert/copy up into multiple
		# transactions is certainly a bad idea which would never have happened except for the issues that large transactions cause with Oracle Streams.    
		self.limit = False
		self.maxops = 10000
		
		self.debug = False

		if lr == None:
				self.lowrun = 1
		else:
				self.lowrun = lr
            
		if ur == None:
				self.uprun = 999999
		else:
				self.uprun = ur

		if lt == None:
				lt = "19700101 00:00:00"
		if ut == None:
				ut = "20690721 00:37:33"
            
		if self.unixts.match(str(lt)) != None:
				self.lowtime_string = datetime.datetime.fromtimestamp(lt)
				self.lowtime = lt
		else:
			lt = lt.replace("-","")
			lowtime_converted = time.strptime(lt, "%Y%m%d %H:%M:%S")
			self.lowtime = calendar.timegm(lowtime_converted)
			self.lowtime_string = lt
	    
		if self.unixts.match(str(ut)) != None:
			self.uptime_string = datetime.datetime.fromtimestamp(ut)
			self.uptime = ut
		else:
			ut = ut.replace("-","")
			uptime_converted = time.strptime(ut, "%Y%m%d %H:%M:%S")
			self.uptime = calendar.timegm(uptime_converted)
			self.uptime_string = ut
	        
		if luminosity == None:
				self.luminosity = 1.0
		else:
				self.luminosity = luminosity

		if rootfile == None:
				self.rootfile = "calib_fit.root"
		else:
				self.rootfile = rootfile

		if statusflag == None:
				self.statusflag = "NULL"
		else:
				self.statusflag = statusflag
            
		self.implementation = impl
			
		self.type = None
		self.calibflag = 0
		self.insert_time = None

		# static rt "dummy" vars
		self.histogram = ""
		self.n_segs = 10000
		self.aver_angle = 0
		self.delta_aver_angle = 0
		self.aver_spread = 9.99989986
		self.delta_aver_spread = 9.99989986
		self.convergence = 0
		self.b_aver = 0
		self.t_aver = 0
		self.hv = 0
		self.curr = 0
		self.gas = 0
		self.validflag = 3
		self.bins = 100

		# t0 related
		# this can't be global, i think
		self.t0_tube_grouping = None
		self.tzeros = []

		id,db = self.parse_id_arg(head_id)
		
		self.head_id = id

		if db != None:
			self.setdb(db)
		else:
			self.setdb(CalibDbInfo.calibdb)

		self.schema_active = None
		self.schema_archived = None
		self.schema_archive_connection_string = None
		self.mconn = None
		self.dconn = None
		self.dcursor = None
		self.mcursor = None
		self.mschema = None
		self.dschema = None
		self.writer_account = None
		self.dbuser = None
		self.proxyuser = True
		self.linkid = None	

	def __del__(self):
		self.drop_dblink()
		if self.dconn != None:
				if self.debug == True:
					print("Closing calibration database connection")
				self.dconn.close()
		if self.mconn != None and self.replica == False:
				if self.debug == True:
					print("Closing metadata database connection")
				try:
					self.mconn.close()
				except cx_Oracle.InterfaceError, exc:
					# IF there is only one consolidated schema then dconn and mconn point to the same connection
					# ignore a 'not connected' error from trying to close that connection twice
					if exc == 'not connected':
						pass

		
	def ptrace(self):
		if self.debug == True:
			traceback.print_exc()

	@staticmethod
	def format_dblist():
		
		dblist = '%-10s  %-28s  %-28s \n' % ("ID","SERVICE","SCHEMA")
		dblist += '---------------------------------------------------------------\n'
		for ldb in CalibDbInfo.databases.keys():
			if ldb == CalibDbInfo.calibdb:
				isdefault = "(Default)"
			else:
				isdefault = ""
			if ldb not in CalibDbInfo.db.keys():
				schema = "undefined"
			else:
				schema = CalibDbInfo.db[ldb]
			dblist += "%-10s  %-28s  %-28s %s \n" % (ldb,CalibData.service_re.search(CalibDbInfo.databases[ldb]).group(1),schema,isdefault)
		return dblist

	def setdb(self,dbid):
		if dbid in CalibDbInfo.databases:
			self.database = dbid
			self.sitename = CalibDbInfo.sitename[self.database]
			self.tnsname = CalibDbInfo.databases[self.database]
			self.replica = CalibDbInfo.replica[self.database]
			self.service = self.service_re.search(self.tnsname).group(1)

			if self.service == None:
				raise Exception("SERVICE_NAME not found in databases[%s]" % dbid)
		else:
			raise Exception("Tried to set CalibData object to unknown database %s" % (dbid))

	def dbgout(self, output):
		if self.debug:
			print output + '\n'

	# returns False if no class attributes in list are unset
	def missing_attributes(self,checkvars):
		required = dict( (name,getattr(self,name)) for name in checkvars)
		for varname,var in required.items():
			if var == None:
				return varname
		return False

	def userstrings(self,access='read',schema='meta'):
		var = { 'write': 'dbw', 'read': 'dbr' }
		rval = { }
		attr = var[access]
		passattr = var[access] + '_password'
		proxyattr = var[access] + '_proxy' 
		obj = getattr(CalibDbInfo,attr)
		passobj = getattr(CalibDbInfo,passattr)
		proxyobj = getattr(CalibDbInfo,proxyattr)
		rval['password'] = passobj[self.database]

		if schema == 'meta' or (schema == 'data' and access == 'read'):
			rval['user'] = obj[self.database]
			try:
				rval['user'] += '[' + proxyobj[self.database] + ']'
			except KeyError:
				pass
			return rval

		if schema == 'data':
			if self.dschema == None:
				self.set_active_db()
			try:
				# see if there is an _proxy array with index for this database.  If so 
				# continue on to set the user using queried writer_account under proxy user
				# if exception, use queried writer account as-is.  
				# we are assuming that if the meta table writer used a proxy account then so does this one (info is not discoverable from the database)
				test = proxyobj[self.database]
				rval['user'] = obj[self.database] + '[' +   self.writer_account + ']'
			except KeyError:
				rval['user'] = self.writer_account
				pass
						
		return rval
			
	# access can be 'read' or 'write'
	# there is a potential bug here - if a read connection is requested after a write connection 
	# we will close the write connection with uncommitted data to open the read connection
	# It's ok as long as we are careful not to do anything in that order inside the class (users of class shouldn't be able to cause it)
	def connect(self,access='read',schema='meta'):
		try: 
			userinfo = self.userstrings(access,schema)
			userstring = userinfo['user']
			userpass = userinfo['password']
			cstring = userstring + "/" + userpass + "@" + self.tnsname
			
			dbname = CalibDbInfo.db[self.database] if schema == 'meta' else 'queried data schema'
			
			dbgstring = '%s %s connection to %s requested - doing nothing and keeping existing open' % (access,schema,dbname)
			dbgstring2 = 'New connection required - closing %s schema database connection' % schema
			dbgstring3 = 'Opening connection with connection information: %s' % cstring

			if schema == 'meta': 
				if self.mconn != None:
					if self.metauser == userstring:
						self.dbgout(dbgstring)
						return
					self.dbgout(dbgstring2)
					self.mconn.close()    
				
				self.dbgout(dbgstring3)
				self.mconn = cx_Oracle.connect(cstring)
				self.mcursor = self.mconn.cursor()
				self.metauser = userstring
				self.mschema = CalibDbInfo.db[self.database]
				self.set_active_db()
				
			elif schema == 'data':
				# it is possible the same user is used for writing both schemas - don't open a new connection in that case
				if self.metauser == userstring and self.mconn != None:
					self.dconn = self.mconn
					self.dcursor = self.dconn.cursor()
					self.datauser = userstring

				if self.dconn != None:
					if self.datauser == userstring:
						self.dbgout(dbgstring)
						return
					self.dbgout(dbgstring2)
					self.dconn.close()
	
				self.dbgout(dbgstring3)
				self.dconn = cx_Oracle.connect(cstring)
				self.dcursor = self.dconn.cursor()
				self.datauser = userstring

		except Exception, exc:
			output = "There was an error connecting to the database "
			if self.mschema != None and schema == "meta":
				output += "Schema: %s" % self.mschema
			if self.dschema != None and schema == "data":
				output += "Schema: %s" % self.dschema
			output += "\n"
			if isinstance(exc,cx_Oracle.Error):
				error, = exc.args
				output += error.message
			elif isinstance(exc,KeyError):
				output += "There is configuration missing for database identifier '%s' - check CalibDbInfo.py" % self.database
			else:
				output += exc.args[0]
			raise CalibDataError(self,output)

	# only necessary to call this when creating new head_id entries, existing head_id will populate active_schema and writer_account when we retrieve it
	# note that when retrieving an existing head_id, active_schema in this object doesn't necessarily == database active_schema 
	def set_active_db(self):
		schema_sql = self.mschema + '.MDT_DATA_SCHEMA'
		sql = "select WRITER_ACCOUNT,SCHEMA_NAME from %s where ACTIVE = 1 " % (schema_sql)

		try:
			self.dbgout("SQL in set_active_db: %s" % sql)
			self.mcursor.execute(sql)
		except Exception, exc:
			raise QueryError(self,exc)

		result = self.mcursor.fetchone()

		if result:
			self.active_schema = result[1]
			self.writer_account = result[0]	
			
	def timestart(self):
		self.timelock = True
		self.timer['wtime'] = time.time() 
		self.timer['ptime'] = time.clock()
		
	def timestop(self):
		if self.timelock == False:
			raise Exception("Called timestop without calling timestart")
		self.timelock = False
		welap = time.time() - self.timer['wtime'] 
		pelap = time.clock() - self.timer['ptime']
		self.dbtime['wtime'] += welap
		self.dbtime['ptime'] += pelap
	
	# what I think I really want to do here is write an "exec" function that timestarts/timestops for all operations and takes as argument what type of op to exec
	# so if necessary it can adjust SQL or other to stay within transaction limits.   What I have is good enough for now, just a bit redundant. 

	def exec_delete(self,sql,ctype="data"):
		loop = True
		rowcount = 0
		self.timestart()
		if self.limit == True:
			sql_del_limit = " AND ROWNUM < %s" % (self.maxops + 1)
		else:
			sql_del_limit = ""
		while loop:
			esql = sql + sql_del_limit
			self.dbgout("SQL executed in exec_delete: %s" % esql)
			if ctype == "data":
				self.dcursor.execute(esql)
				if self.dcursor.rowcount > 0:
					self.opcount += self.dcursor.rowcount
					rowcount = self.dcursor.rowcount
			elif ctype == "meta":
				self.mcursor.execute(esql)
				if self.mcursor.rowcount > 0:	
					self.opcount += self.mcursor.rowcount
					rowcount = self.mcursor.rowcount	
			if rowcount > 0:
				rowcount = 0
				self.commit()
				loop = True
			else:
				loop = False
		self.timestop()
		
	def exec_insert(self,sql,ctype="data"):
		self.timestart()
		if ctype == "data":
			self.dcursor.execute(sql)
			if self.dcursor.rowcount > 0:
				self.opcount += self.dcursor.rowcount
		elif ctype == "meta":
			self.mcursor.execute(sql)
			if self.mcursor.rowcount > 0:	
				self.opcount += self.mcursor.rowcount	
		# commit only commits if opcount is >= maxops
		self.commit()
		self.timestop()
	
	def check_opcount(self):
		if self.opcount >= self.maxops and self.limit == True:
			if self.debug:
				confirm = "y"
				print ("Ready to commit %s operations" % self.opcount)
				confirm = raw_input("Enter 'y' or 'yes' if you wish to continue with operation: ")
				if confirm not in ("y","yes","Y"):
					raise Exception("User cancelled operation")
			return True
		else:
			return False

	def commit(self,force=False):	
		did_commit = False	
		
		try:
			if self.mconn != None and self.replica == False:	
				if self.check_opcount() or force:	
					self.mconn.commit()
					did_commit = True
					self.mcursor = self.mconn.cursor()
			if self.dconn != None:
				if self.check_opcount() or force:
					self.dconn.commit()
					did_commit = True
					self.dcursor = self.dconn.cursor()
			if did_commit:
				self.optotal += self.opcount
				self.opcount = 0 
				self.transtotal += 1
					
		except Exception, exc:
			raise CalibDataError(self,exc)

	def rollback(self):
		try:
			if self.mconn != None:
				if self.mcursor.rowcount > 0:
					self.opcount += self.mcursor.rowcount
				self.mcursor.close()
				self.mconn.rollback()
				self.mcursor = self.mconn.cursor()
			if self.dconn != None:
				if self.dcursor.rowcount > 0:
					self.opcount += self.dcursor.rowcount
				try: 
					self.dcursor.close()
				except cx_Oracle.InterfaceError, exc:
					# sometimes the cursor isn't open if we didn't do anything - not a problem I need to know about
					if exc == 'not open':
						pass
				self.dconn.rollback()
				self.dcursor = self.dconn.cursor()   
		except Exception, exc:
				raise CalibDataError(self,exc)
				
		# split (if necessary) id argument like headid@database into id and database.  returns "None" if no database or None id provided
	def parse_id_arg(self,id):
		db = None

		if id == None:
			return id,db

		list = str(id).split('@',1)

		id = list[0]

		if len(list) > 1:
			db = list[1]

		return id,db

		# service is service attribute from source object used in copy operation
	def setup_dblink(self,srcdb,service): 

		if srcdb != self.database:
			# The need for @loopback comes up in testing when we copy between databases on the same server.  Will not be allowed without specifying @loopback.  
			if service == self.service:
				lb = '@loopback'
			else:
				lb = ''

			linkid = service + lb

			dblink_sql = "CREATE DATABASE LINK %s CONNECT TO %s identified by %s using '%s'" % (linkid, CalibDbInfo.dbr[srcdb],CalibDbInfo.dbr_password[srcdb],CalibDbInfo.databases[srcdb])
			check_sql = "SELECT DB_LINK from USER_DB_LINKS where lower(DB_LINK) like '%s%s'" % (linkid,'%');

			if self.linkid == None:
				try:
					self.connect("write","meta")
					self.dbgout ("SQL in setup_dblink: %s" % check_sql)
					self.mcursor.execute(check_sql)
					self.linkid = self.mcursor.fetchone()
					if self.linkid != None:
						self.drop_dblink()
					self.dbgout("SQL in setup_dblink: %s" % dblink_sql)
					self.mcursor.execute(dblink_sql)
					self.linkid = linkid
				except Exception, exc:
					raise CalibDataError(self,exc)			

	def drop_dblink(self):
		if self.linkid != None:
			drop_sql = "DROP DATABASE LINK %s" % self.linkid
			self.dbgout("Deleting database link")
			self.connect("write","meta")
			try:
				self.dbgout("SQL in drop_dblink: %s" % drop_sql)
				self.mcursor.execute(drop_sql)
			except Exception, exc:
				raise CalibDataError(self,exc)			

	def format_headinfo(self):
		info = "Implementation name: %s \n" % self.implementation
		info += "Lower Time IOV: %s, %s epoch seconds \n" % (self.lowtime_string, self.lowtime)
		info += "Upper Time IOV: %s, %s epoch seconds \n" % (self.uptime_string, self.uptime)
		info += "Lower Run IOV: %s \n" % self.lowrun
		info += "Upper Run IOV: %s \n" % self.uprun
		info += "Luminosity: %s \n" % self.luminosity
		info += "Rootfile: %s \n" % self.rootfile
		info += "Sitename: %s \n" % self.sitename
		info += "Insert Time: %s \n" % self.insert_time
		info += "Status Flag: %s \n" % self.statusflag
		return info

	# this is meant to be used to extract some valid tube/chamber identifier from any given string containing one.  Intended use is for uploading
	# from batches of files where each filename identifies a tube
	@staticmethod
	def extract_tubestring(fullstring):
		mid = MuonFixedIdUnpack()
		hardware_tubestring = mid.online_re.search(fullstring)
		software_tubestring = mid.offline_re.search(fullstring)
		numeric_tubestring = mid.numeric_re.search(fullstring)
		
		if (hardware_tubestring):
			return hardware_tubestring.group()
		if (software_tubestring):
			return software_tubestring.group()
		if (numeric_tubestring):
			# make sure it is not actually a date 
			datematch = re.compile('(19|20)\d\d(0[1-9]|1[012])(0[1-9]|[12][0-9]|3[01])')
			if not datematch.search(fullstring):
				return numeric_tubestring.group()
	
		return False

	@staticmethod
	def convert_tube(tube):
		exepath = sys.path[0] + "/muonfixedid"
		retval = subprocess.Popen([exepath,tube], stdout=subprocess.PIPE).stdout.read()
		if "ERROR" in retval:
				print retval
				raise Exception
		else:
				return retval

	@staticmethod
	def get_tube_id(tube):
		mid=MuonFixedIdUnpack(tube)
		return mid.identifier
            
	@staticmethod
	def get_tube_string(tube):
		try:
			mid = MuonFixedIdUnpack(tube)
			string = "%s_%s_%s" % (mid.stationNameString(),mid.stationPhi(),mid.stationEta())
			return string
		except ValueError:
			# will throw exception if format doesn't match regex
			MuonFixedIdUnpack(tube)
			return tube

	def get_head_info(self):
		self.connect("read","meta")
		sql = "select head.head_id, head.implementation, head.data_schema, sh.SCHEMA_NAME, head.lowrun,head.uprun,head.lowtime,head.uptime, head.luminosity, head.rootfile, head.site_name, head.insert_time, head.status_flag, sh.WRITER_ACCOUNT, sh.ACTIVE, sh.ARCHIVED, sh.ARCHIVE_CONNECTION_STRING from %s.MDT_HEAD head, %s.MDT_DATA_SCHEMA sh where head.DATA_SCHEMA = sh.SCHEMA_NAME AND " % (self.mschema,self.mschema)
		
		sql += "head.HEAD_ID = %s " % self.head_id
        
		sql += "ORDER BY head.head_id DESC"
        
		self.dbgout("SQL from get_head_info: %s" % sql)

		try:
				self.mcursor.execute(sql)
		except Exception, exc:
				raise QueryError(self,exc)

		result = self.mcursor.fetchone()
		if result:
			self.head_id = result[0]
			self.implementation = result[1]
			self.dschema = result[2]
			self.calibdbwriter = result[3]
			self.lowrun = result[4]
			self.uprun = result[5]
			self.lowtime = result[6]
			self.uptime = result[7]
			self.luminosity = result[8]
			self.rootfile = result[9]
			self.sitename = result[10]
			# the result will be a python datetime object which I am converting to the format oracle would expect if we use this value in an INSERT statement
			self.insert_time = self.convert_datetime_oracle(result[11])
			self.statusflag = result[12]
			# this value is used in connection strings
			self.calibdbwriter = result[13]
			# this value may be altered and used to write to MDT_DATA_SCHEMA table
			self.writer_account = result[13]
			self.schema_active = result[14]
			self.schema_archived = result[15]
			if result[16] == None:	
				self.schema_archive_connection_string = 'NULL'
			else:
				self.schema_archive_connection_string = result[16]

			if self.statusflag == None:
				self.statusflag = "NULL"
		else:
			raise QueryError(self,"No results")
        
		self.lowtime_string = datetime.datetime.fromtimestamp(self.lowtime)
		self.uptime_string = datetime.datetime.fromtimestamp(self.uptime)

	def convert_datetime_oracle(self,dtobj):
		#current_time = datetime.datetime.utcnow().strftime("%d-%b-%y %I.%M.%S.%f %p %z")
		if isinstance(dtobj,datetime.datetime):
			return dtobj.strftime('%d-%b-%y %I.%M.%S.%f %p %z').upper()
		else:
			raise Exception("Not a datetime object")

	def get_t0(self,tube_id=None,chamber_id=None):
		# **** I have no idea if this really works anymore
		self.connect("read","data")
		sql = "SELECT T.chamber, T.tube_id, T.P4, T.ADC_1, T.validflag, T.nhits_above_adc_cut, V.algo_flag,V.chisquare_1,V.P4_err,V.P5,V.P5_err,V.P0,C.P0_ERR,V.P6,V.p6_err, V.tube_grouping from"+self.dschema + ".MDT_TUBE T, " + self.dschema + ".MDT_TUBE_V V," + self.dschema + ".MDT_TUBE_C C where T.head_id = :hid AND C.head_id = :hid AND V.head_id = :hid"
		if chamber_id != None:
				sql += " AND t.chamber = :cid AND C.tube_id IN (SELECT tube_id from %s.MDT_TUBE T WHERE T.chamber = :cid) AND V.tube_id IN (SELECT tube_id from %s.MDT_TUBE T WHERE T.chamber = :cid)"
		if tube_id != None:
				sql += " AND T.tube_id = :tid AND C.tube_id = :tid AND V.tube_id = :tid"
        
		self.dbgout("SQL in get_tzero: %s" % sql)

		try:
				self.dcursor.execute(sql,{'hid':self.head_id,'cid':chamber_id,'tid':tube_id})
		except Exception, exc:
				raise QueryError(self,exc)

		rowcount = 0
		for r in self.cursor.fetchall():
					self.tzeros[rowcount] = r 
					rowcount+=1
					
	def get_rt(self,chamber_id=None):
		self.connect("read","data")
		sql = "SELECT HEAD_ID, REGION_ID,CALIBFLAG,LOWRUN,UPRUN,HISTOGRAM,N_SEGS,AVER_ANGLE,DELTA_AVER_ANGLE,AVER_SPREAD,DELTA_AVER_SPREAD,CONVERGENCE,B_AVER,T_AVER,HV,CURR,GAS,VALIDFLAG,ALGO_FLAG,BINSFROM %s.MDT_RT" % (self.dschema )
		if chamber_id != None:
			sql += " AND region_id = :rid"
		try:
			self.dcursor.execute(sql,{'rid':chamber_id})
		except Exception, exc:
			raise QueryError(self,exc)
		rowcount = 0
		for r in self.cursor.fetchall():
			self.rt[rowcount] = r 
			rowcount+=1

	def check_copy_args(self,sourceobj,fn):
		if not isinstance(sourceobj,CalibData):
				raise Exception("Called %s without providing copy source CalibData object" % fn)
		if sourceobj.mschema == None:
				raise Exception("CalibData object passed to %s not initialized with get_head_info() method" % fn)

	# used either in copy to replica or to write a new schema entry as independent operation 
	def write_data_schema(self):
		if self.dschema == None or self.writer_account == None or self.schema_active == None or self.schema_archived == None or self.schema_archive_connection_string == None:
			raise Exception("Required members in CalibData object not set, cannot check or write data schema.\nself.dschema: %s, self.writer_account: %s, self.schema_active: %s, self.schema_archived: %s self.schema_archive_connection_string: %s" % (self.dschema,self.writer_account,self.schema_active,self.schema_archived,self.schema_archive_connection_string))

		sql_exists = "SELECT SCHEMA_NAME from %s.MDT_DATA_SCHEMA WHERE SCHEMA_NAME = '%s'" % (self.mschema, self.dschema)

		sql_insert = "INSERT INTO %s.MDT_DATA_SCHEMA VALUES ('%s','%s',%s,%s,%s)" % (self.mschema,self.dschema,self.writer_account,self.schema_active,self.schema_archived,self.schema_archive_connection_string)

		self.dbgout("SQL from write_data_schema: %s" % sql_exists)

		try:
			self.mcursor.execute(sql_exists)
			self.mcursor.fetchall()
			if self.mcursor.rowcount > 0:
				if self.replica == False:
					raise Exception(self,"Tried to write new MDT_DATA SCHEMA %s but SCHEMA_NAME exists" % self.dschema)
				else:
					return
		except Exception,e:
			raise QueryError(self,e)

		
		self.dbgout("SQL from write_data_schema: %s" % sql_insert)

		try:
			self.mcursor.execute(sql_insert)
		except Exception,exc:
			raise HeadInsertError(self,"Unable to insert into MDT_DATA_SCHEMA: %s" % exc)


	def write_headid(self):

		required = dict( (name,getattr(self,name)) for name in [ 'implementation', 'lowrun', 'uprun', 'lowtime', 'uptime', 'luminosity', 'rootfile', 'sitename' ])
		for varname,var in required.items():
			if var == None:
				raise HeadInsertError(self,"Write head ID - %s was not set, cannot create new id" % varname)

		if self.replica and self.head_id == None:
			raise HeadInsertError(self,"Cannot insert new meta information into replica database because self.head_id is not set")
	    
		self.connect("write","meta")
		newid = self.mcursor.var(cx_Oracle.NUMBER)
		schema = self.mcursor.var(cx_Oracle.STRING)

		if self.head_id == None or self.replica:
			if self.head_id == None:
				self.head_id = 'NULL'
			if self.dschema == None:
				self.dschema = 'NULL'
			if self.insert_time == None:
				self.insert_time = 'NULL'
			sql = "INSERT INTO %s.MDT_HEAD(HEAD_ID,IMPLEMENTATION, LOWRUN, UPRUN, LOWTIME, UPTIME, LUMINOSITY, ROOTFILE, SITE_NAME, DATA_SCHEMA,INSERT_TIME,STATUS_FLAG) VALUES(%s,'%s',%s,%s,%s,%s,%s,'%s','%s'," % (self.mschema, self.head_id, self.implementation, self.lowrun, self.uprun, self.lowtime, self.uptime, self.luminosity, self.rootfile,self.sitename)
			if self.replica:
				# if we're actually inserting values copied from the source to replica we'll need to quote these strings
				sql += "'%s','%s'," % (self.dschema,self.insert_time)
			else: 
				# non-replica (site) calibdbs have a trigger to set schema and insert time so we insert unquoted NULL on those columns
				sql += "%s,%s," % (self.dschema,self.insert_time)
			sql += "%s) RETURNING head_id,data_schema into :newid,:schema" % (self.statusflag)
				
			update = False
		else: 
			sql = "UPDATE %s.MDT_HEAD SET IMPLEMENTATION='%s', LOWRUN=%s, UPRUN=%s, LOWTIME=%s, UPTIME=%s, LUMINOSITY=%s,ROOTFILE='%s',SITE_NAME='%s', INSERT_TIME = '%s', STATUS_FLAG=%s WHERE HEAD_ID=%s" % (self.mschema, self.implementation,self.lowrun,self.uprun,self.lowtime,self.uptime,self.luminosity,self.rootfile,self.sitename,self.insert_time,self.status_flag, self.head_id)
			update = True
		
		self.dbgout("SQL from write_headid: %s" % sql)
		try:
			if update:
				self.exec_insert(sql,"meta")
			else:
				# not using exec_insert to avoid passing bind variables (this is the only place it comes up and essentially amounts to counting 1 operation so it probably isn't worth a lot of thought)
				self.timestart()
				self.mcursor.execute(sql,{'newid':newid, 'schema':schema})
				self.timestop()
				if self.mcursor.rowcount > 0:
					self.opcount += self.mcursor.rowcount
				self.head_id = int(newid.getvalue())
				if self.replica == False:
					self.dschema = schema.getvalue()
		except Exception, exc:
			raise HeadInsertError(self,exc)

	def copy_head(self,sourceobj=None):
		self.connect("write","meta")
		self.check_copy_args(sourceobj,"copy_head")
		fromid = sourceobj.head_id
		srcdb_id = sourceobj.database
		self.setup_dblink(srcdb_id, sourceobj.service)

		if self.replica:
			self.head_id = sourceobj.head_id
			self.dschema = CalibDbInfo.schema[self.database]
			self.insert_time = sourceobj.insert_time
		
		self.implementation = sourceobj.implementation
		self.lowrun = sourceobj.lowrun
		self.uprun = sourceobj.uprun
		self.lowtime = sourceobj.lowtime
		self.uptime = sourceobj.uptime
		self.luminosity = sourceobj.luminosity
		self.rootfile = sourceobj.rootfile
		self.sitename = sourceobj.sitename
		self.statusflag = sourceobj.statusflag

		self.schema_active = sourceobj.schema_active
		self.schema_archived = sourceobj.schema_archived
		self.schema_archive_connection_string = sourceobj.schema_archive_connection_string
		
		#if not self.replica:
		#	self.writer_account = sourceobj.writer_account

		if self.replica:
			self.write_data_schema()

		self.write_headid()

		if self.replica:
			if int(self.head_id) != int(sourceobj.head_id):
				raise HeadInsertError(self,"Destination is configured as 'replica' but got new head_id %s from insert when expecting id %s - cannot replicate head_id from source.  Check your CalibDbInfo.py configuration." % (self.head_id, sourceobj.head_id))
		    
	def copy_rt(self,sourceobj,chamber=None,ignore_missing=False):
		self.check_copy_args(sourceobj,"copy_rt")

		fromid = sourceobj.head_id
		srcdb_id = sourceobj.database
		self.setup_dblink(srcdb_id,sourceobj.service)
				
		self.connect("write","data")

		if self.head_id == None and self.linkid == None:
			raise Exception("Cannot copy RT in same database without setting head_id for CalibData object")
		elif self.replica == False or self.linkid == None:
			id_select = self.head_id
		else:
			id_select = "head_id"
				
		schema_sql = "%s.MDT_RT" % sourceobj.dschema	
		schema_sql_map = schema_sql + "_MAP"
		
		if self.linkid != None:
			schema_sql += "@%s" % self.linkid
			schema_sql_map += "@%s" % self.linkid
	
		sql_rt = "INSERT INTO %s.MDT_RT (HEAD_ID, REGION_ID,CALIBFLAG,LOWRUN,UPRUN,HISTOGRAM,N_SEGS,AVER_ANGLE,DELTA_AVER_ANGLE,AVER_SPREAD,DELTA_AVER_SPREAD,CONVERGENCE,B_AVER,T_AVER,HV,CURR,GAS,VALIDFLAG,ALGO_FLAG,BINS) SELECT %s,REGION_ID,CALIBFLAG,LOWRUN,UPRUN,HISTOGRAM,N_SEGS,AVER_ANGLE,DELTA_AVER_ANGLE,AVER_SPREAD,DELTA_AVER_SPREAD,CONVERGENCE,B_AVER,T_AVER,HV,CURR,GAS,VALIDFLAG,ALGO_FLAG,BINS FROM %s WHERE HEAD_ID = %s" % (self.dschema, id_select, schema_sql, fromid)
		
		sql_rt_map = "INSERT INTO %s.MDT_RT_MAP(HEAD_ID, REGION_ID,CALIBFLAG,POINT_NR,S,T,R) SELECT %s,REGION_ID,CALIBFLAG,POINT_NR,S,T,R FROM %s WHERE HEAD_ID = %s" % (self.dschema, id_select, schema_sql_map, fromid)
			
		if chamber != None:
			chamber_id = self.get_tube_id(chamber)
			chamber_sql = " AND region_id = %s" % chamber_id
			sql_rt += chamber_sql
			sql_rt_map += chamber_sql
		
		self.dbgout("SQL in copy_rt: %s" % sql_rt_map)

		try:	
			self.dbgout("SQL in copy_rt: %s" % sql_rt)
			self.exec_insert(sql_rt)
			self.dbgout("SQL in copy_rt: %s" % sql_rt_map)
			self.exec_insert(sql_rt_map)
			if self.dcursor.rowcount == 0 and ignore_missing == False:
				raise DataCopyError(self,"There is no RT data in the source head id")
		except cx_Oracle.IntegrityError,exc:
				raise DataUniqueError(self,exc)
		except HeadInsertError:
			raise
		except Exception, exc:
			raise DataCopyError(self,exc)
			    
		# copy existing T0 to this object.  new head_id must exist (use write_headid if object created with empty headid)
	def copy_tzero(self,sourceobj=None,chamber=None,tube=None,ignore_missing=False):

		self.check_copy_args(sourceobj,"copy_tzero_sql")

		fromid = sourceobj.head_id
		srcdb_id = sourceobj.database
		self.setup_dblink(srcdb_id,sourceobj.service)
		
		self.connect("write","data")
        
		if self.head_id == None and self.linkid == None:
			raise Exception("Cannot copy tzero in same database without setting head_id for CalibData object")
		elif self.replica == False or self.linkid == None:
			id_select = self.head_id
		else:
			id_select = "head_id"
		
		schema_sql = "%s.MDT_TUBE" % sourceobj.dschema	
		schema_sql_tv = schema_sql + "_V"
		schema_sql_tc = schema_sql + "_C"
		
		if self.linkid != None:
			schema_sql += "@%s" % self.linkid
			schema_sql_tv += "@%s" % self.linkid
			schema_sql_tc += "@%s" % self.linkid

		mdt_tube_sql = "INSERT INTO %s.MDT_TUBE SELECT tube_id,%s,chamber,calibflag,validflag,nhits,nhits_above_adc_cut,p4,adc_0,adc_1,adc_2,adc_3,insert_time FROM %s WHERE head_id = %s" % (self.dschema,id_select,schema_sql,fromid)

		mdt_tube_v_sql = "INSERT INTO %s.MDT_TUBE_V (head_id,tube_id,calibflag, chisquare_1, chisquare_2, p0, p1, p2, p3, p5, p6, p7, p8, p9, p4_err, p5_err, p6_err, algo_flag, entries, tube_grouping) SELECT %s, tube_id,calibflag, chisquare_1, chisquare_2, p0, p1, p2, p3, p5, p6, p7, p8, p9, p4_err, p5_err, p6_err, algo_flag, entries, tube_grouping FROM %s WHERE head_id = %s" % (self.dschema,id_select,schema_sql_tv,fromid)

		mdt_tube_c_sql = "INSERT INTO %s.MDT_TUBE_C (head_id, tube_id,calibflag, p0_err, p1_err, p2_err, p3_err, p7_err, p8_err, p9_err, cov_1, cov_2, cov_3, cov_4, adc_0_err, adc_1_err, adc_2_err, adc_3_err, adc_chisquare, adc_pedestal, adc_pedestal_width) SELECT %s,tube_id,calibflag, p0_err, p1_err, p2_err, p3_err, p7_err, p8_err, p9_err, cov_1, cov_2, cov_3, cov_4, adc_0_err, adc_1_err, adc_2_err, adc_3_err, adc_chisquare, adc_pedestal, adc_pedestal_width FROM %s WHERE head_id = %s" % (self.dschema,id_select,schema_sql_tc,fromid)
       
		if chamber != None and tube == None:
				chamber_id = self.get_tube_id(chamber)
				chamber_sql = " AND chamber = %s" % chamber_id
				chamber_tube_sql = " AND tube_id IN (SELECT tube_id FROM %s WHERE chamber=%s)" % (schema_sql,chamber_id)
				mdt_tube_sql += chamber_sql
				mdt_tube_v_sql += chamber_tube_sql
				mdt_tube_c_sql += chamber_tube_sql
		elif tube != None and chamber == None:
				tube_id = self.get_tube_id(tube)
				tube_sql = " AND tube_id = %s" % tube_id
				mdt_tube_sql += tube_sql
				mdt_tube_v_sql += tube_sql
				mdt_tube_c_sql += tube_sql
		elif tube != None and chamber != None:
				raise DataCopyError(self,"Cannot operate on both chamber and single tube")

		self.dbgout("SQL in copy_tzero: %s" % mdt_tube_sql)
		self.dbgout("SQL in copy_tzero: %s" % mdt_tube_c_sql)
		self.dbgout("SQL in copy_tzero: %s" % mdt_tube_v_sql)

		try:
			#if chamber == None and tube == None:
			#	self.copy_head(sourceobj)
			self.exec_insert(mdt_tube_sql) 
			if self.dcursor.rowcount == 0 and ignore_missing == False:
				raise DataCopyError(self,"There is no T0 data in the source head id")
			self.exec_insert(mdt_tube_c_sql)
			self.exec_insert(mdt_tube_v_sql)		
		except cx_Oracle.IntegrityError, exc:
			raise DataUniqueError(self,exc)
		except HeadInsertError:
			raise
		except cx_Oracle.Error, exc:
			raise DataCopyError(self,exc)
                        
	def insert_rt_map(self,point_nr, r, t, s):
		self.connect("write","data")
		sql_map = "INSERT INTO %s.MDT_RT_MAP(HEAD_ID, REGION_ID,CALIBFLAG,POINT_NR,S,T,R) VALUES (%s,%s,%s,%s,%s,%s,%s)" % ( self.dschema, self.head_id,self.regionid,self.calibflag,point_nr,s,t,r)
		if self.debug == True:
			self.dbgout("SQL from insert_rt_map: %s" % sql_map)
		try: 
			if point_nr == 99 or point_nr == 199:
                                tmax = float(t)
				if tmax > 800. or tmax < 700.:
					raise Exception("Tmax value %s: Tmax must be in range 700 to 800 ns" % t)
			self.exec_insert(sql_map)
		except cx_Oracle.IntegrityError, exc:
			raise DataUniqueError(self,exc)
		except Exception, exc:
			raise MapInsertError(self,exc)
    
	def insert_rt(self,regionid):
		if self.type == None:
				raise RTInsertError(self,"Cannot insert RT until you set object 'type' variable")
        
		self.connect("write","data")
		sql_rt = "INSERT INTO %s.MDT_RT (HEAD_ID, REGION_ID,CALIBFLAG,LOWRUN,UPRUN,HISTOGRAM,N_SEGS,AVER_ANGLE,DELTA_AVER_ANGLE,AVER_SPREAD,DELTA_AVER_SPREAD,CONVERGENCE,B_AVER,T_AVER,HV,CURR,GAS,VALIDFLAG,ALGO_FLAG,BINS) VALUES (%s,%s,%s,%s,%s,'%s',%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,'%s',%s)" % (self.dschema,self.head_id,regionid,self.calibflag,self.lowrun,self.uprun,self.histogram,self.n_segs,self.aver_angle,self.delta_aver_angle,self.aver_spread,self.delta_aver_spread,self.convergence,self.b_aver,self.t_aver,self.hv,self.curr,self.gas,self.validflag,self.type,self.bins)
		
		self.dbgout("SQL from insert_rt: %s" % sql_rt)

		try: 
				self.regionid = regionid
				self.exec_insert(sql_rt)
		except cx_Oracle.IntegrityError, exc:
				raise DataUniqueError(self,exc)
		except Exception, exc:
				raise RTInsertError(self,exc)

	def insert_adc(self,tube_id,chamber_id,nhits,adc_0,adc_0_err,adc_1,adc_1_err,adc_2,adc_2_err,adc_3,adc_3_err,adc_chisquare):
		self.connect("write","data")
		sql_tube = 'INSERT INTO %s.MDT_TUBE (HEAD_ID,TUBE_ID,CHAMBER,VALIDFLAG,NHITS,NHITS_ABOVE_ADC_CUT,P4,ADC_0,ADC_1,ADC_2,ADC_3) VALUES (%s,%s,%s,0,%s,%s,%s,%s,%s,%s,%s)' % (self.dschema,self.head_id,tube_id,chamber_id,self.validflag,nhits,nhits,adc_0,adc_1,adc_2,adc_3)       
		sql_tube_c = "INSERT INTO %s.MDT_TUBE_C (HEAD_ID,TUBE_ID,ADC_0_ERR,ADC_1_ERR,ADC_2_ERR,ADC_3_ERR,ADC_CHISQUARE) VALUES (%s,%s,%s,%s,%s,%s,%s)" % (self.dschema,self.head_id,tube_id, adc_0_err, adc_1_err, adc_2_err, adc_3_err, adc_chisquare) 

		if self.debug == True:
			self.dbgout("SQL from insert_adc: %s" % sql_tube)
			self.dbgout("SQL from insert_adc: %s" % sql_tube_c)

		try:
			self.exec_insert(sql_tube)
			self.exec_insert(sql_tube_c)
		except cx_Oracle.IntegrityError, exc:
			raise DataUniqueError(self,exc)
		except Exception, exc:
			raise ADCInsertError(self,exc)		
	   
	def insert_t0(self,tube_id,chamber_id, tzero,avg_adc,calibflag,stats,chi2,t0err,tmax,tmax_err,noise,noise_err,slope,slope_err):
		# ,tzero,avg_adc,calibflag,stats,chi2,t0err,tmax,tmax_err,noise,noise_err,slope,slope_err
		self.connect("write","data")
		
		sql_tube = "INSERT INTO %s.MDT_TUBE (HEAD_ID,TUBE_ID,CALIBFLAG,CHAMBER,P4,ADC_1,VALIDFLAG,NHITS,NHITS_ABOVE_ADC_CUT) VALUES (%s,%s,%s,%s,%s,%s,%s,%s,%s)" % (self.dschema,self.head_id,tube_id,calibflag,chamber_id,tzero,avg_adc,self.validflag,stats,stats)

		#V.algo_flag,V.chisquare_1,V.P4_err,V.P5,V.P5_err,V.P0,V.P6,V.p6_err, V.tube_grouping)
		sql_tube_c = "INSERT INTO %s.MDT_TUBE_C (HEAD_ID,TUBE_ID,CALIBFLAG,P0_ERR) VALUES (%s,%s,%s,%s)" % (self.dschema,self.head_id,tube_id,calibflag,noise_err)
		sql_tube_v = "INSERT INTO %s.MDT_TUBE_V (HEAD_ID,TUBE_ID,CALIBFLAG,CHISQUARE_1,P4_ERR,P5,P5_ERR,P0,P6,P6_ERR,ALGO_FLAG,TUBE_GROUPING) VALUES (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,'%s','%s')" % (self.dschema,self.head_id,tube_id,calibflag,chi2,t0err,tmax,tmax_err,noise,slope,slope_err,self.type,self.t0_tube_grouping)

		if self.debug == True:
			self.dbgout("SQL from insert_t0: %s" % sql_tube)
			self.dbgout("SQL from insert_t0: %s" % sql_tube_c)
			self.dbgout("SQL from insert_t0: %s" % sql_tube_v)

		try:
			self.exec_insert(sql_tube)
			self.exec_insert(sql_tube_c)
			self.exec_insert(sql_tube_v)
		except cx_Oracle.IntegrityError, exc:
			raise DataUniqueError(self,exc)
		except Exception, exc:
			raise T0InsertError(self,exc)		
			
	# returns true on success or else throws DeleteError
	# I split the mdt_head and mdt_rt/mdt_rt_map just to be more flexible, no commit happens during this function
	def delete_head_id(self):
		self.connect("write","meta")
		sql = "DELETE FROM %s.MDT_HEAD WHERE HEAD_ID=%s and IMPLEMENTATION='%s'" % ( self.mschema, self.head_id, self.implementation)
		
		self.dbgout("SQL from delete_head_id: %s" % sql)

		try:
				self.exec_delete(sql,"meta")
		except Exception, exc:
				raise DeleteError(self,exc)

	def delete_rt(self,region=None):
		self.connect("write","data")
		sql_rt_map = "DELETE FROM %s.MDT_RT_MAP WHERE HEAD_ID=%s" % (self.dschema, self.head_id)
		sql_rt = "DELETE FROM %s.MDT_RT WHERE HEAD_ID=%s" % (self.dschema, self.head_id)

		if region != None:
		# aka as chamber_id
			region_id = self.get_tube_id(region)
			sql_rt_map += " AND REGION_ID=%s" % region_id
			sql_rt += " AND REGION_ID=%s" % region_id
		try:
			self.exec_delete(sql_rt_map)	
			self.exec_delete(sql_rt)						
		except Exception, exc:
			raise DeleteError(self,exc)
	
	def delete_tube(self,tube=None,chamber=None):
		self.connect("write","data")	
		sql_tube_v = "DELETE FROM %s.MDT_TUBE_V WHERE HEAD_ID=%s" % (self.dschema, self.head_id)
		sql_tube_c = "DELETE FROM %s.MDT_TUBE_C WHERE HEAD_ID=%s" % (self.dschema, self.head_id)
		sql_tube = "DELETE FROM %s.MDT_TUBE WHERE HEAD_ID=%s" % (self.dschema, self.head_id)
		if chamber != None:
			chamber_id = self.get_tube_id(chamber)
			sql_tube += " AND CHAMBER=%s" % chamber_id
			chamber_tube_sql = " AND tube_id IN (SELECT tube_id FROM %s.MDT_TUBE WHERE chamber=%s)" % (self.dschema,chamber_id)
			sql_tube_v += chamber_tube_sql
			sql_tube_c += chamber_tube_sql
		elif tube != None and chamber == None:
			tube_id = self.get_tube_id(tube)
			sql_tube_v += " AND TUBE_ID=%s" % tube_id
			sql_tube_c += " AND TUBE_ID=%s" % tube_id
			sql_tube += " AND TUBE_ID=%s" % tube_id
       
		self.dbgout("SQL from delete_t0: %s" % sql_tube_v)
		self.dbgout("SQL from delete_t0: %s" % sql_tube_c)
		self.dbgout("SQL from delete_t0: %s" % sql_tube)

		try:
			self.exec_delete(sql_tube_v)
			self.exec_delete(sql_tube_c)
			self.exec_delete(sql_tube)
		except Exception, exc:
			raise DeleteError(self,exc)

	def set_rt_valid(self,tube=None):
		self.connect("write","data")
		sql_valid = "UPDATE %s.MDT_RT SET VALIDFLAG=3 WHERE HEAD_ID=%s " % (self.dschema, self.head_id)
		if tube != None:
			sql_valid += "AND REGION_ID=%s" % tube

		self.dbgout("SQL from set_rt_valid: %s" % sql_valid)

		try:
			self.exec_insert(sql_valid)
		except Exception, exc:
			raise UpdateError(self,exc)

	def set_t0_valid(self,tube=None):
		self.connect("write","data")
		sql_valid = "UPDATE %s.MDT_TUBE SET VALIDFLAG=3 WHERE HEAD_ID=%s " % (self.dschema, self.head_id)
		if tube != None:
			sql_valid += "AND TUBE_ID=%s" % tube

		self.dbgout("SQL from set_t0_valid: %s" % sql_valid)

		try:
			self.exec_insert(sql_valid)
		except Exception, exc:   
			raise DeleteError(self,exc)
	
	def fetch_head_data(self):
		self.connect()
		sql = "SELECT HEAD_ID,IMPLEMENTATION,LOWRUN,UPRUN,LOWTIME,UPTIME,LUMINOSITY,SITE_NAME,INSERT_TIME FROM %s.MDT_HEAD ORDER BY HEAD_ID DESC" % (self.mschema)
        
		self.dbgout("SQL from fetch_head_data: %s" % sql)
		
		try:
			self.mcursor.execute(sql)
		except Exception, exc:
			raise QueryError(self,exc)
			#"%-8s\t%-30s\t%-8s\t%-8s\t%-10s\t%-10s\t%-2s\t%-2s\t%-28s\n"
 #"%-8s%-30s%-8s%-8s%-10s%-10s%-2s%-2s%-28s\n"
		head_row = self.mcursor.fetchone()
		head_data_rows = []
		while head_row:		
				head_data_rows.append(head_row)
				head_row = self.mcursor.fetchone()
		return head_data_rows

	def format_head_data(self,head_data_rows):
		text = "%-10s%-35s%-10s%-10s%-12s%-12s%-5s%-6s%-30s\n" % ('HEAD_ID','IMPLEMENTATION','LOWRUN','UPRUN','LOWTIME','UPTIME','LUM','SITE','INSERT TIME')
		for data in head_data_rows:
			text += "%-10s%-35s%-10s%-10s%-12s%-12s%-5s%-6s%-30s\n" % (data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7],data[8])
		return text







