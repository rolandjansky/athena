# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

import os, re, time

# This method reads the files in the given directory, sorts them by run/event number,
# finds atlantis and vp1 files belonging to the same event and returns a list of events
# and their corresponing files: (event, run, atlantis, vp1)
def getEventlist(msg, directory):
	filelist = []
	files = os.listdir(directory)

	# Build a list of files ordered by run/event number
	for file in files:
		matches = re.search('(?:JiveXML|vp1)_(\d+)_(\d+)(?:\.xml|_.+\.pool\.root)', file)

		# Event file, add tot the list
		if matches:
			run = "%012d" % long(matches.group(1))
			event = "%012d" % long(matches.group(2))

			fileentry = run, event, file
			filelist.append(fileentry)

		# Something else, check if it is one of our bookkeeping files, otherwise try to remove it
		elif file not in ['latest_vp1event', 'event.list']:

			# If the file is recent, it might be from another thread... delete after 5 minutes
			try:
				if time.time() - os.path.getmtime(file) > 300:
					msg.info("File '%s' does not belong in the output directory, removing it." % file)
					try:
						os.unlink("%s/%s" % (directory, file))
					except OSError, err:
						msg.warning("Could not remove '%s': %s" % (file, err))
			except OSError:
				# File was probably a temp file from another thread that already disappeared
				pass

	i = 0
	eventlist = []
	filelist.sort()
	numfiles = len(filelist)

	# Now loop through the files to form pairs
	while i < numfiles-1:
		#if filelist[i][0] != filelist[i+1][0] or filelist[i][1] != filelist[i+1][1]:

			# Make sure that files without a partner (atlantis-vp1) are also removed
		#	if i == 0:
		#		msg.warning("One of the files is missing for run %s, event %s, removing the other as well." % (filelist[i][0], filelist[i][1]))
		#		try:
		#			os.unlink("%s/%s" % (directory, filelist[i][2]))
		#		except OSError, err:
		#			msg.warning("Could not remove '%s': %s" % (filelist[i][2], err))

			# Do not include such files in the list for atlas-live.cern.ch
		#	i = i + 1
		#else:
			# Build list for atlas-live.cern.ch
			evententry = filelist[i][0], filelist[i][1], filelist[i][2], filelist[i+1][2]
			eventlist.append(evententry)
			i = i + 1

	return eventlist;

# Prune events in the given directory if the number exceeds the specified number
def pruneEvents(msg, directory, maxevents, eventlist):
	i = 0
	numevents = len(eventlist)

	# Check if there are more events than allowed and prune a number of files equal to the excess
	if numevents > maxevents:
		for i in range(numevents-maxevents):
			run, event, atlantis, vp1 = eventlist.pop(0)
			msg.debug("Going to prune files %s and %s for run %s and event %s." % (atlantis, vp1, run, event))
			try:
				os.unlink("%s/%s" % (directory, atlantis))
				os.unlink("%s/%s" % (directory, vp1))
			except OSError, err:
				msg.warning("Could not remove files for run %s, event %s: %s" % (run, event, err))

	else:
		msg.debug("Nothing to prune (%d <= %d)." % (numevents, maxevents))

# Build the event.list file that is used by atlas-live.cern.ch for synchronizing events
def writeEventlist(msg, directory, eventlist):
	pid = os.getpid()
	try:
		file = open("%s/event.%d" % (directory, pid), 'w')
		for run, event, atlantis, vp1 in eventlist:
			file.write("run:%s,event:%s,atlantis:%s,vp1:%s\n" % (run, event, atlantis, vp1))
		file.close()
	except IOError, err:
		msg.warning("Could not write event list: %s" % err)

	# Rename for an atomic overwrite operation
	try:
		os.rename("%s/event.%d" % (directory, pid), "%s/event.list" % directory)
	except OSError, err:
		msg.warning("Could not rename event.%d to event.list: %s" % (pid, err))

# Perform all of these in one command
def cleanDirectory(msg, directory, maxevents):
	eventlist = getEventlist(msg, directory)
	if maxevents:
		pruneEvents(msg, directory, maxevents, eventlist)
	writeEventlist(msg, directory, eventlist)

