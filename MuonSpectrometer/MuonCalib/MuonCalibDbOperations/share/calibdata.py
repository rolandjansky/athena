#!/bin/env python

from CalibDataClass import *
import readline
import sys
import traceback
import os
import fnmatch
import getopt
import re
import fileinput
import time

# initializing some things, do not edit 
usedb = CalibDbInfo.calibdb
noask = False
force = False
debug = False
quiet = False
nobatch = False
upload = False
createid = False
showlist = False
modifyhead = False
copy = False
copydata = []
head_id = None
tube_id = None
tube = None
chamber = None
lowrun = None
uprun = None
lowtime = None
uptime = None
copysrc = None
syncdb = False
constant_type = None
constant_arg = None
insert_time = None
sitename = None
implementation = None
status_flag = "NULL"
valid = []
delete = []
upload_args = []
rootfile = None
luminosity = None
newidfile = None
cd = None
autoimpl = False
synclimit = 1

datatypes = ['t0','rt','adc','id']

def usage():
	print("\n --yes to avoid being asked for confirmation (-y)")
	print("\n --quiet (-q) to run with no output.  Will return 0 if no errors, 1 if any problem.")
	print("\n --force (-F) to force delete of existing data attached to head id before upload or copy")
	print("\n --list to list head id information from database (-l)")
	print("\n --listdb to list available databases.")  
	print("\n--db=DATABASE to specify database to operate on (or to be used as destination in case of copy commands)")
	print("\t If --db not specified actions will apply to default set by calibdb variable in CalibDbInfo.py")
	print("\n --id=head id to apply actions to (-i headid).  Not required by actions that \n\tmay create new head id (upload,copy,newid).  If specified for upload or copy \n\tthen inserted data will be associated with that id. \n\t--force may be required to delete existing data for id")
	print("\n --upload (-u) --path=/path/to/files [--path=/path/to/files] (-p)")
	print("\tPath to 1 or more dir of files to upload named with chamber in filenames.\n\tIf directory is named YYYYMMDD will automatically use for lowtime/uptime. \n\tAssume 24HR period with end at midnight UTC of day in dir name.")
	print("\tUpload can be a single filename.  Filename doesn't have to contain chamber name if --chamber is specified.\n\tAny number of path arguments can be used though generally 2 would be used to upload RT and T0 together.\n")
	print(" --implementation='implementation string'\n")
	print("\tImplementation is ignored for RT as it is derived from type and path")
	print("\n --type=[GTPM][T0BS][T0MT] (-t)")
	print("\tArgument required if files do not have header.  Ignored on per-file basis if standard header is provided in any file.  Only one type can be specified (if uploading RT and T0 together at least one set must specify correct type in file headers). For RT upload type arguments are:")
	print(" \tG = Gasmon RT")
	print(" \tT = temperature-corrected")
	print(" \tP = pressure-corrected")
	print(" \tM = magnetic-field corrected")
	print("\tFor T0 or ADC spectra types are:")
	print(" \tT0BS = T0 Beam Splash")
	print(" \tT0MT = T0 Mag,Temp corrected (also will accept T0CalibrationMT)")
	print("\n --sync --from=database [--synclimit=<integer> (0 for no limit)] [--db=database]\n\tCompares head ids in source(--from) and destination db (--db), and synchronizes ids/data from\n\tsource if not found in destination (must be configured as 'replica').\n\tIf synclimit is not specified, will only sync highest head id.  Otherwise sync until synclimit ids have been copied.\n\tIf both source and destination already contain same head ids then program will exit with no action.")
	print("\n --copy=[t0,adc,rt] --from=ID[@DATABASE] (copy single id) or --from=database (for syncing to replicas) [--force]  \n\tIf you do not provide existing --id to copy into then you may provide\n\timplementation, lowrun, uprun, lowtime, uptime to create new id with those parameters.\n\tTo modify some destination fields add --modifyhead and provide new values.  \n\tFor convenience --implementation will be accepted without --modifyhead.\n\tIf target is specified as a replica in CalibDbInfo.py then you\n\tdo not need to specify destination ID or new id information - source meta information will be copied exactly.")
	print("\tMultiple types may be copied at once, separate them with commas.  t0 and adc are synonynms that copy the same tables (MDT_TUBE*)\n")
	print("\tIn any case where a database is not provided for source (@DATABASE) or destination (--db)\n\tlocal database specified in config variable 'calibdb' is used")
	print("\n --tube=tubeid (-b tubeid) \n\tNumeric ID (321937408) or tube name string (BIL1C01) to apply operations to")
	print(" --chamber=chamberid (-c chamberid) \n\tChamber id or string to apply operations to.")
	print("\n\tTube and chamber arguments can be applied to limit scope of any relevant operation.");
	print("\tIf no tube or chamber is provided, operations effect all data for head id.")
	print("\tMust provide --id with --tube or --chamber \n\tThey cannot be used to create new id with info for only one tube or chamber.")
	print("\n --modifyhead --implementation='string' (only implementation,rootfile,statusflag supported).  \n\tUse alone or in conjunction with 'copy' commands to modify existing head ids")
	print("\n --newid to create a new headid from command line arguments (-n)")
	print("\n --newidfile[=filename] -f [filename] \n\tUse to create a new headid from rows in file if provided \n\tor from info provided in command line args if no filename provided")
	print("\tFile format is 5 columns ordered: \n\tIMPLEMENTATION LOWRUN UPRUN LOWTIME UPTIME\n\tFile can contain any number of rows to insert")
	print("\n--lowrun (-L)")
	print(" --uprun (-U)")
	print("\tlowrun/uprun are required for new head ids")
	print("\n --lowtime (-W) format: '20101231 23:59:00'")
	print(" --uptime (-P) format '20101231 23:59:00'")
	print("\tNote: lowtime/uptime are required for new head ids. \n\tIf first --path argument contains date in YYYYMMDD format then lowtime/uptime will be start and end of that day unless specified. ")
	print("\n --luminosity (optional, defaults to 1.0)")
	print("\n --rootfile (optional, defaults to 'none')")
	print("\n --statusflag (optional, defaults to NULL")
	print("\n --implementation (required for new head id)")
	print("\tNOTE: Modifying other values is implemented internally but there is no option available to specify them - if needed please request.")
	print("\n --delete [rt,t0,adc,id] (-e) to delete values for head id given by --id.\n\tIf id is in list, deletes that head id.\n\tDeleting id implies deleting values associated regardless of specification of adc,t0,rt in command.")
	print("\n --setvalid [rt,t0,adc] (-z) sets data as valid for chamber or head id, requires --id and optionally --chamber args.\n")

def ask_confirm():
	if noask == False and quiet == False:
		confirm = raw_input("Enter 'y' or 'yes' if you wish to continue with operation: ")
	else:
		confirm = "y"
	if confirm not in ("y","yes","Y","YES"):
		return False
	else:
		return True

def perr(error,exc=None,exit=False,):
	if quiet == False:
		output = error
		if isinstance(exc,KeyboardInterrupt):
			output += ": User Interrupt"
		elif isinstance(exc,Exception):
			if not isinstance(exc,SystemExit):
				output += ": " + exc.__str__()
		output += "\n"
		print >> sys.stderr, output
	if exit == True:
		sys.exit(1)

def pout(output,newline=True):
	if newline:
		output += "\n"
	if quiet == False:
		print output
		
def check_path(path):
	if os.path.isdir(path) or os.path.isfile(path):
		return True
	else:
		perr("File/path argument to upload option does not exist", exit=True)		
		#sys.exit(1)

# check --type argument
def check_algo_type(type):
	if re.match(r'[GTPM]', type):
		return True

	if type == 'T0CalibrationMT' or type == 'T0BS' or type == 'T0MT':
		return True

	return False

# check input data type in t0,adc,rt,id
def checktypes(arg):
	if len(list(set(arg) & set(datatypes))) == 0:
		return False
	else:
		return True

def parse_date_dir(upload_arg):
	global lowtime
	global uptime
	# try to parse a date from dirname and figure out IOV
	check = re.compile('.*2[0-9][0-9][0-9][0-1][0-9][0-3][0-9]\/*')
	find = re.compile('2[0-9][0-9][0-9][0-1][0-9][0-3][0-9]')
	check_date = check.match(upload_arg)
	if check_date != None and (lowtime == None and uptime == None):
		upload_date = find.findall(upload_arg)
		# calculate times for IOV from date found in upload_arg
		lowtime = upload_date[0] + " 00:00:00"       
		uptime = upload_date[0] + " 23:59:59"
	elif (lowtime != None and uptime != None):
		upload_date = find.findall(uptime)
	else:
		pout("Upload directory name does not contain date in format: 20091021 and you did not specify lowtime/uptime in arguments.  Today's date will be used.") 
		today = time.strftime('%Y%m%d')
		lowtime = today + " 00:00:00" 
		uptime = today + " 23:59:59"
		return [today]
	return upload_date
				
def summary(cd):
	pout("Committed %s database operations this session in %s separate transactions.  \nRolled back or failed %s write operations.\nTotal walltime %s and total CPU time %s seconds." % (cd.optotal, cd.transtotal, cd.opcount,cd.dbtime['wtime'],cd.dbtime['ptime']))

# TODO:  Rewrite all this to use argparse and 
# consolidate some options into argument-optional
# versions (like --db to list db, --db= to set db, and then can remove --listdb)
# (getopt cannot handle optional arguments)

# options
try:                                
	opts, args = getopt.getopt(sys.argv[1:], "hup:d:t:b:c:m:i:nf:r:ylqDFL:U:W:P:", ["help", "upload","path=","delete=","type=","tube=","chamber=","implementation=","id=","newid","newidfile=","setvalid=","yes","list","quiet","debug","force","lowrun=","uprun=","lowtime=","uptime=","from=","copy=","modifyhead","luminosity=","rootfile=","statusflag=","listdb","db=","sync","synclimit=" ])
except getopt.GetoptError, err:
	print >> sys.stderr, "A bad option was passed:"
	print >> sys.stderr, str(err)
	print >> sys.stderr, "Use option -h or --help for usage instructions"                         
	sys.exit(2)

if len(opts) == 0:
	print("This program requires an option: \n")
	usage()                          
	sys.exit(1)
 
# option handling could probably be done better
# action prerequisite args here
for opt,arg in opts:
	if opt in ["-y","--yes"]:
		noask = True

	if opt in ["-h","--help"]:
		usage()                          
		sys.exit()

	if opt in ["--listdb"]:
		print CalibData.format_dblist()
		sys.exit(0)

	if opt in ["--list", "-l"]:
		showlist = True

	if opt in ["--db"]:
		if arg in CalibDbInfo.databases:
			usedb = arg
		else:
			perr("An unknown database was specified, valid databases are:");
			print CalibData.format_dblist()
			sys.exit(1)

	if opt in ['--sync']:
		syncdb = True

	if opt in ['--synclimit']:
		if re.match("^\d+$", arg, flags=0):
			synclimit = int(arg)
		else:
			perr("--synclimit must be integer",exit=True)

	if opt in ["-L","--lowrun"]:
		lowrun = arg

	if opt in ["-U","--uprun"]:
		uprun = arg

	if opt in ["-W","--lowtime"]:
		lowtime = arg	

	if opt in ["-P","--uptime"]:
		uptime = arg

	if opt in ["--luminosity"]:
		luminosity = arg

	if opt in ["--rootfile"]:
		rootfile = arg

	if opt in ["--statusflag"]:
		status_flag = arg

	if opt in ["-q", "--quiet"]:
		quiet = True

	if opt in ["-D","--debug"]:
		debug = True

	if opt in ["-i","--id"]:
		head_id = arg

	if opt in ["-b","--tube"]:
		tube = arg
		nobatch = True

	if opt in ["-c","--chamber"]:
		chamber = arg
		nobatch = True

	if opt in ["-m","--implementation"]:
		implementation = arg

	if opt in ["--copy"]:
		copydata = arg.split(',')
		copy = True
		if not checktypes(copydata):
			perr("Invalid copy types: %s" % arg)
		upload_args.append('copyarg')

	if opt in ["--from"]:
		copysrc = arg

	if opt in ["-z", "--setvalid"]:
		valid = arg.split(',')
		if not checktypes(valid):
			perr("Invalid types for setting valid: %s" % arg)

	if opt in ["-e", "--delete"]:
		delete = arg.split(',')
		if not checktypes(delete):
			perr("Invalid types for deletion: %s" % arg)

	if opt in ["--upload", "-u"]:
		upload = True

	if opt in ["--modifyhead"]:
		modifyhead = True

	if opt in ["-F","--force"]:
		force = True

	if opt in ["-n", "--newid"]:
		newidfile = None
		createid = True
		if upload == False:
			upload_args = ['createid']

	if opt in ["-f","--newidfile"]:
		implementation = 'fromfile'
		newidfile = arg
		createid = True
		if upload == False:
			upload_args = ['createid']

	if opt in ["-t","--type"]:
		if check_algo_type(arg):
			constant_arg = arg
		else:
			perr("Unknown algorithm type, not one of T0CalibrationMT, T0BS, T0MT or containing one of 'GTPM' for RT")
			sys.exit(1)

	if opt in ["-p", "--path"]:
		if check_path(arg):
			upload_args.append(arg)

try:

	if syncdb == True:
		if copysrc not in CalibDbInfo.databases:
			perr("\nMust specify a valid database key to --from argument",exit=True)
		if usedb not in CalibDbInfo.databases:
			perr("\nMust specify a valid destination database key to --db argument or in 'calibdb' setting in CalibDbInfo",exit=True)

		syncsrc = CalibData()
		syncsrc.debug = debug
		syncsrc.setdb(copysrc)
		cd = CalibData()
		cd.debug = debug
		cd.setdb(usedb)

		srcdata = syncsrc.fetch_head_data()
		dstdata = cd.fetch_head_data()
		dst_ids = []
		sync_ids = []

		for dst_head in dstdata:
			dst_ids.append(dst_head[0])

		scount = 0		
		for src_head in srcdata:
			if scount >= synclimit and synclimit != 0:
				break
			if src_head[0] not in dst_ids:
				sync_ids.append(src_head[0])
				scount += 1

		for copy_id in sync_ids:
			print "\nSyncing head id %s" % copy_id
			syncsrc = CalibData(copy_id)
			syncsrc.setdb(copysrc)
			syncsrc.debug = debug
			syncsrc.get_head_info()
			cd.copy_head(syncsrc)
			cd.copy_tzero(syncsrc,ignore_missing=True)
			cd.copy_rt(syncsrc,ignore_missing=True)


	if upload == True or copy == True or createid == True:
		# open file of head ids or set to 1 just to trigger 1 loop of head_id inserting from params
		if newidfile != None and createid == True:
			try: 
				idfile = open(newidfile, 'r')
			except:
				perr("Unable to open file of new head IDs: %s" % newidfile)
				sys.exit(1)
		else:
			idfile = "1"

		# tally of how many files we read in the upcoming loop
		count_files = 0
		if upload == True:
			pout("\nUploading from : %s" % (' '.join(upload_args)))
		for upload_arg in upload_args:
			if copy == True:
				files = ["copyloop"]
			elif upload_arg == 'createid':
				files = ["idloop"]
			else:
				if os.path.isfile(upload_arg):
					files = [upload_arg]
					dirprep = ''
				else:
					files = os.listdir(upload_arg)
					dirprep = upload_arg + "/"
				
				if head_id == None:			
					upload_date = parse_date_dir(upload_arg)

			for filename in files:
				if not fnmatch.fnmatch(filename, '*.dat'):
					continue
				constant_type = constant_arg
				# upload is initially false, becomes set to a string 
				if filename != 'copyloop' and filename != 'idloop':
					f = open(dirprep + filename, 'r')
					# read first line, determine what kind of upload this is.  If RT with no header, we'll need to seek back to the first line.
					f_start = f.tell()
					rts = f.readline().split()
					# T0 header is len 6, T0 is 19, ADC fit is 17.
					if len(rts) == 6 or len(rts) == 19 or len(rts) == 17:
						if len(rts) == 6:
							ct = rts[3]
							if constant_arg != None:
								perr("Header found in input file, ignoring --type")
						else:
							ct = constant_type
							f.seek(f_start)
						if ct == "T0MT" or ct == "T0CalibrationMT":
							upload = 't0'
							constant_type = "T0MT"
							fieldcount = 19
						elif ct == "T0BS":
							upload = 't0'
							constant_type = ct
							fieldcount = 19
						elif ct == "ADCfit":
							upload = 'adc'
							constant_type = ct
							fieldcount = 17
						else:
							raise Exception("Unrecognized algorithm type '%s' in T0 or ADC header in file %s" % (rts[3], filename))

					# if len == 3 it is RT with no header.  If len == 12 it is RT with standard header
					elif len(rts) == 3 or len(rts) == 12:
						upload = 'rt'
						fieldcount = 3
						if len(rts) == 12:
							# pull constant_type/algorithm from header.  There isn't really a known standard for this so I'll use the one I invented for gas monitor RT uploads.
							match_type = re.findall(r'[GPTM]+',rts[2])
							if match_type:
								constant_type = ''.join(match_type)
								if constant_arg != None:
									perr("Header found in input file, ignoring --type")
							else:
								raise Exception("Unable to find algorithm type from RT header field '%s' in file %s" % (rts[2],filename))
						else:	
							f.seek(f_start) 
					if constant_type == None:
						raise Exception("Unable to determine upload type from file contents: %s\nSpecifying a constant type with --type (aka ALGO_FLAG) is required for uploads if any files do not have a header." % filename)

					# only if no implementation argument on command line, set implementation from date defined by directory name of upload path
					if implementation == None and head_id == None:
						if upload == 'rt':
							implementation = "RT_" 
						elif upload == 'adc' or upload == 't0':
							implementation = "CALIB_"
						implementation += constant_type + '_' + upload_date[0] 
						pout("No implementation specified, generating automatically: %s " % implementation)

			# end of specific to upload stuff......

				if cd == None:
					for line in idfile:
						if newidfile != None and createid == True:
							idlines = line.split()
							implementation = idlines[0]
							lowrun = idlines[1]
							uprun = idlines[2]
							lowtime = idlines[3] + " " + idlines[4]
							uptime = idlines[5] + " " + idlines[6]
						try:
							cd = CalibData(head_id,implementation,lowrun,uprun,lowtime,uptime,luminosity,rootfile,status_flag)
							cd.debug = debug
							cd.setdb(usedb)
							if head_id != None:
								cd.get_head_info()
							if copy == True:
								cdsource = CalibData(copysrc)
								cdsource.debug = debug
								cdsource.get_head_info()
								pout("Source head id %s from database has information:" % cdsource.head_id)
								pout(cdsource.format_headinfo())
						
						except QueryError, exc:
							output = "\nCould not retrieve head id(s) from database: "
							if "cdsource" not in locals() and head_id != None:
								output += "ID %s" % head_id
							if "cdsource" in locals():
								output += " ID " + copysrc
							perr(error=output,exception=exc)
							sys.exit(1)

						if cd.head_id != None:
							pout("Destination head id %s from database has information:" % cd.head_id)
							pout(cd.format_headinfo())

						if implementation != None and copy == True:
							pout("Setting/modifying destination implementation to '%s'" % implementation)
							cdsource.implementation = implementation

						if (copy == True) and tube == None and chamber == None:
							pout("If --id specified, copy will overwrite destination id with source values.\nIf implementation provided on command line it will be used for destination.\nIf no id is specified, a new one will be created with source data.\nIf destination is replica db an exact copy of source, including head id, will be created.")
						
						if cd.head_id == None:
							try:
								if copy == True:
									cd.copy_head(cdsource)
								else:
									cd.write_headid()
								pout("Successful commit will create new head ID: %s" % cd.head_id)
								pout(cd.format_headinfo())
							except HeadInsertError:
								perr("Sorry, failed to setup head_id, quitting now")
								raise

					if ask_confirm() == False:
						sys.exit(0)
							
				if upload != False:
					pout("\nFilename: %s" % filename, newline=False)
					count_files += 1

				# this might seem redundant but we don't know what format we got from extract_tubestring
				# the two methods below take whatever as an input and return a known format even if they might just return the input
				    
				chamber_string = CalibData.extract_tubestring(filename)

				if (chamber_string):
					chamber = CalibData.get_tube_string(chamber_string)
					chamber_id = CalibData.get_tube_id(chamber_string)
				elif (chamber):
					chamber_id = CalibData.get_tube_id(chamber)
					chamber = CalibData.get_tube_string(chamber)
				else:
					if filename != 'copyloop' and filename != 'idloop':
						pout("Skipping %s, could not determine chamber from filename and none given on command line (may be fake bookkeeping chamber)" % filename)
					continue

				pout("Chamber is: %s" % chamber, 0)
				pout("Chamber id is: %s" % chamber_id, 0)

				if tube != None:
					tube_id = CalibData.get_tube_id(tube)
					tube = CalibData.get_tube_string(tube)
					pout("Tube is: %s" % chamber, 0)
					pout("Tube id is: %s" % chamber_id, 0)

				if upload != False:
					cd.type = constant_type
					if upload == 'rt':
						try:
							# insert RT into mdt_rt table, chamber_id was determined from file or arguments by this point 		    
							if force:
								cd.delete_rt(chamber_id)
							cd.insert_rt(chamber_id)
						
						except DataUniqueError:
							perr("\nRT value exists for chamber.\nAdd --force to command to force delete before insert or delete rt with --delete='rt' option")
							raise
						except RTInsertError:
							perr("Problem inserting RT for chamber: %s id: %s" % (chamber,chamber_id))
							perr("Not deleting head id, reuse with --id flag or delete with --delete=id")
							raise
						except Exception:
							perr("Error with RT insert operation")
							raise
							
					# point_nr is just an informational count of datapoints in file (number of lines, start at 0 for first point)
					point_nr = 0

					for line in f:
						try:
							rts = line.split()
							if len(rts) != fieldcount:
								perr("Invalid line in %s file %s, too many or too few fields" % (upload, filename))
							if upload == 'rt':
								cd.insert_rt_map(point_nr,rts[0],rts[1],rts[2])
							elif upload == 't0' or upload == 'adc':
								tube_id_string = rts[0] + "_" + rts[1] + "_" + rts[2] + "-" + rts[4] + "-" + rts[5] + "-" + rts[6]
								tube_id = cd.get_tube_id(tube_id_string)
								pout("Tube ID string: %s" % tube_id_string,0)
								pout("Tube ID: %s" % tube_id)
				
								cd.t0_tube_grouping = "CHAMBER"
								# hmm...not efficient if inserting a batch of them, but allows for not necessarily inserting/overwriting every t0.
								# educate users to use --deletet0 for faster overwrites of existing t0 in batch uploads
								if force == True:
									cd.delete_tube(tube_id,chamber_id)
								if upload == 't0':
									cd.insert_t0(tube_id,chamber_id,rts[7],rts[8],rts[9],rts[10],rts[11],rts[12],rts[13],rts[14],rts[15],rts[16],rts[17],rts[18])
								elif upload == 'adc':
									cd.insert_adc(tube_id,chamber_id,rts[7],rts[8],rts[9],rts[10],rts[11],rts[12],rts[13],rts[14],rts[15],rts[16])                        
						except (RTInsertError, T0InsertError, ADCInsertError):
							perr("Error inserting values read at line %s in file %s" % (point_nr + 1, filename))
							perr("Not deleting head id, reuse with --id flag or delete with --delete=id")
							raise
						except DataUniqueError:
							perr("Values already exist for chamber %s (%s) tube %s (%s) at line %s in file %s.  Use --force to delete existing t0 one-by-one before each insert.  Use --delete=[t0,rt,adc] to delete values.  \n" % (chamber, chamber_id, tube_id_string, tube_id, point_nr + 1, filename))
							raise
						except Exception:
							perr("An error has occurred at line %s in file %s." % (point_nr + 1,filename))
							raise
						point_nr += 1
					
					if upload == 'rt' and point_nr != 100 and point_nr != 200:
						raise Exception("Found %s data points in file %s.  RT must have either 100 or 200 data points." % (point_nr,filename))	
						# reset all values or might validate values from this iteration on next one
					chamber = None
					chamber_id = None
					tube = None
					tube_id = None

			if count_files > 0:
				pout("\nFound %d files in dir %s" % (count_files,upload_arg))		
		pout("\nInserting to head_id %s" % cd.head_id)
			
	if 't0' in copydata or 'adc' in copydata:
		try:
			if force == True:
				cd.delete_tube(tube,chamber)					
			cd.copy_tzero(cdsource,chamber,tube)
			#if implementation != None and head_id != None:
			#	cd.implementation = implementation
			#	cd.write_headid()
			pout("T0/ADC copy complete")
		except DataCopyError:
			perr("Problem copying values from %s to %s@%s" % (copysrc, cd.head_id, cd.database))
			raise
		except DataUniqueError:
			perr("Data for tube(s) or chamber(s) already exists.  Use --force to overwrite.")
			raise
		except HeadInsertError:
			perr("Error modifying head id %s" % cd.head_id)
			raise
		except Exception:
			perr("Error copying T0/ADC")
			raise

	if 'rt' in copydata:
		try:
			if force == True:
				cd.delete_rt(chamber)
			# should just require adding --modifyhead but for convenience in this one case...
			cd.copy_rt(cdsource,chamber)
			if implementation != None and head_id != None:
				cd.implementation = implementation
				cd.write_headid
			pout("RT Copy complete")
		except DataCopyError:
			perr("Problem copying RT values from %s to %s@%s" % (copysrc, cd.head_id, cd.database))
			raise
		except DataUniqueError:
			perr("Data for chamber(s) already exists.  Use --force to overwrite.")
			raise
		except HeadInsertError:
			perr("Error modifying head id %s" % cd.head_id)
			raise
		except Exception:
			perr("Error copying RT")
			raise
	if (modifyhead):
		if cd.head_id == None:
			perr("Must provide --id with --modifyhead")
			sys.exit(1)

		fields = { 'implementation':implementation, 'lowrun':lowrun, 'uprun':uprun, 'lowtime':lowtime, 'uptime':uptime,'luminosity':luminosity, 'rootfile':rootfile, 'sitename':sitename,'insert_time':insert_time, 'status_flag':status_flag }
		modified = False
		try:
			for field,val in fields.items():
				if val != None:
					setattr(cd, field, val) 
					modified = True
			if modified == False:
				perr("No modification specified")
				sys.exit(1)
			cd.write_headid()
		except HeadInsertError, exc:
			perr("Error modifying head id" + cd.head_id, exc)
			raise
				
	if showlist:
		noask = True
		try:
			cd = CalibData()
			cd.debug = debug
			cd.setdb(usedb)
			print cd.format_head_data(cd.fetch_head_data())
			sys.exit(0)
		except QueryError, exc:
			perr("Error querying for head ids", exc)
			sys.exit(1)
	    
	if delete or valid:
		if head_id == None:
			perr("You did not give head id (--id=)")
			sys.exit(1)

		if not isinstance(cd,CalibData):
			cd = CalibData(head_id)
			cd.setdb(usedb)
			cd.debug = debug
		try:
			cd.get_head_info()
		except QueryError, exc:
			perr("Could not retrieve information for head id: %s" % head_id, exc)
			sys.exit(1)

		if tube != None:
			try:
				tube_id = cd.get_tube_id(tube)
				tube = cd.get_tube_string(tube)
			except TubeIdError:
				perr("Invalid tube string or numeric ID was input")
				raise
			except Exception:
				perr("Unable to convert tube string or ID")
				raise
		else:
			tube_id = None

		if chamber != None:
			try:
				chamber_id = cd.get_tube_id(chamber)
				chamber = cd.get_tube_string(chamber)
			except TubeIdError:
				perr("Invalid tube string or numeric ID was input")
				sys.exit(1)
			except Exception:
				perr("Invalid chamber string or ID was put in")
				sys.exit(1)
		else:
			chamber_id = None

		if 'rt' in delete:
			pout("Deleting RT for head id: %s, implementation name: %s " % (head_id,cd.implementation))
			if chamber_id != None:
				pout("Chamber: %s, chamber id: %s" % (chamber, chamber_id))
			if ask_confirm() == False:
				sys.exit(0)
			try:
				cd.delete_rt(chamber_id)
			except DeleteError:
				perr("Error deleting rt for head id %s \n" % head_id)
				raise
			except Exception:
				raise
				
		if 't0' in delete:
			pout("Deleting T0 for head id: %s, implementation name: %s " % (head_id,cd.implementation))
			if chamber != None:
				pout("Deleting t0 for chamber: %s, tube id: %s" % (chamber, chamber_id))
			if tube_id != None:
				pout("Deleting t0 for tube: %s, tube id: %s" % (tube, tube_id))
			if ask_confirm() == False:
				sys.exit(0)
			try:
				cd.delete_tube(tube_id,chamber_id)
			except DeleteError:
				perr("Error deleting t0 for head id %s" % head_id)
				raise
			except Exception:
				raise

		# modify all this so any number of items can be set valid at once....
		if 't0' in valid or 'adc' in valid:
			try:
				cd.set_t0_valid(tube_id)
			except UpdateError:
				print("Error setting t0 valid flag for head id %s \n" % head_id)
			except Exception:
				raise
	
			pout("Committing update of t0 valid flag for head id: %s, implementation name: %s " % (head_id,cd.implementation))
			if tube_id != None:
				pout("Tube: %s, tube id: %s" % (tube, tube_id))

		if 'rt' in valid:
			try:
				cd.set_rt_valid(tube_id)
			except UpdateError:
				perr("Error setting RT valid flag for head id %s \n" % head_id)
			except Exception:
				raise
			pout("Committing update of rt valid flag for head id: %s, implementation name: %s " % (head_id,cd.implementation))
			if tube_id != None:
				pout("Tube: %s, tube id: %s" % (tube, tube_id))

		if 'id' in delete:
			pout("*** Will delete associated RT ***")
			pout("*** Will delete associated T0 ***")
			pout("*** Will delete associated ADC ***")
			pout ("*** Will delete all head id information ***")
			pout(cd.format_headinfo())
			if ask_confirm() == False:
				sys.exit(0)
			try:
				cd.delete_head_id()
				cd.delete_rt()
				cd.delete_tube()
			except DeleteError:
				perr("Error deleting head id: %s" % head_id)
				raise
			except Exception:
				raise
	  
# catch unhandled exceptions
except (Exception,KeyboardInterrupt), exc:
	# catch when we exit(0) by intent
	if isinstance(exc,SystemExit):
		if exc.code == 0:
			sys.exit(0)	    
	if debug == True:
		traceback.print_exc()
	perr("Error performing operation", exc)
	if isinstance(cd,CalibData):
		cd.rollback()
		summary(cd)
	
	sys.exit(1)

# safe and sound with no exceptions and time to commit whatever we did up there 
# ...that is, time to commit anything that didn't get committed automatically when we exceeded 10000 operations for CalibData.opcount internally

try:
	pout("Committing data to database.")
	commit = True
	if debug:
		commit = ask_confirm()
		if commit == False:
			cd.rollback()
	if commit == True:
		cd.commit(force=True)
	summary(cd)
except Exception, exc:
	perr("There was an error committing data", exc)
	if cd:
		cd.rollback()
	summary(cd)
	sys.exit(1)


	

