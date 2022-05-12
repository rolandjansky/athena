# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import os, re, time, glob, shutil
from pathlib import Path
from zipfile import ZipFile, ZIP_DEFLATED

# This method reads the files in the given directory, sorts them by run/event number,
# finds atlantis and vp1 files belonging to the same event and returns a list of events
# and their corresponing files: (event, run, atlantis, vp1)
# checkpair=True and remove=True, remove the files that do not form an atlanits-vp1 pair
# checkpair=True and remove=False, generate event list for atlantis-vp1 pairs
# checkpair=False, generate event list without checking for valid pairs
def getEventlist(msg, directory, checkpair, remove=True, patternAtlantis='.xml', patternVP1='.pool.root'):
    msg.verbose('%s begin get event list', time.ctime(time.time()))
    filelist = []
    files = os.listdir(directory)

    # look for JiveXML_{RunNumber}_{EventNumber}.xml or vp1_r{RunNumber}_ev{EventNumber}_{TimeStamps}CEST.pool.root files
    # excluding the CastorScript bookkeeping files with .STATUS suffix to the original data files
    pattern = r'(?:JiveXML|vp1)_(?:|r)(\d+)_(?:|ev)(\d+)'+f'(?:{re.escape(patternAtlantis)}|_.+CEST{re.escape(patternVP1)})'+r'(?!\.)'

    # Build a list of files ordered by run/event number
    for file in files:
        matches = re.search(pattern, file)

        # Event file, add tot the list
        if matches:
            run = "%012d" % int(matches.group(1))
            event = "%012d" % int(matches.group(2))

            fileentry = run, event, file
            filelist.append(fileentry)

    i = 0
    eventlist = []
    filelist.sort()
    numfiles = len(filelist)

    # Now loop through the files to form pairs
    while i < numfiles-1:
        if checkpair and (filelist[i][0] != filelist[i+1][0] or filelist[i][1] != filelist[i+1][1]):

            # Make sure that files without a partner (atlantis-vp1) are also removed
            if remove and i == 0:
                msg.warning("One of the files is missing for run %s, event %s, removing the other as well.", filelist[i][0], filelist[i][1])
                try:
                    msg.verbose("Removing %s/%s", directory, filelist[i][2])
                    os.unlink("%s/%s" % (directory, filelist[i][2]))
                except OSError as err:
                    msg.warning("Could not remove '%s': %s", filelist[i][2], err)

            # Do not include such files in the event list
            i = i + 1
        else:
            # Build event list
            evententry = filelist[i][0], filelist[i][1], filelist[i][2], filelist[i+1][2]
            eventlist.append(evententry)
            i = i + 1

    msg.verbose('%s end get event list', time.ctime(time.time()))
    return eventlist

# Prune events in the given directory if the number exceeds the specified number
def pruneEvents(msg, directory, maxevents, eventlist):
    msg.verbose('%s begin prune events', time.ctime(time.time()))
    i = 0
    numevents = len(eventlist)

    # Check if there are more events than allowed and prune a number of files equal to the excess
    if numevents > maxevents:
        for i in range(numevents-maxevents):
            msg.verbose("maxevents=%d, numevents=%d, i=%d", maxevents, numevents, i)
            run, event, atlantis, vp1 = eventlist.pop(0)
            msg.verbose("Going to prune files %s and %s for run %s and event %s.", atlantis, vp1, run, event)
            try:
                msg.verbose("%s Trying to unlink %s", time.ctime(time.time()), atlantis)
                os.unlink("%s/%s" % (directory, atlantis))
                msg.verbose("%s Trying to unlink %s", time.ctime(time.time()), vp1)
                os.unlink("%s/%s" % (directory, vp1))
                msg.verbose("%s Done with unlink", time.ctime(time.time()))
            except OSError as err:
                msg.warning("Could not remove files for run %s, event %s: %s", run, event, err)

    else:
        msg.debug("Nothing to prune (%d <= %d).", numevents, maxevents)
    msg.verbose('%s end prune events', time.ctime(time.time()))

# Build the event.list file that is used by atlas-live.cern.ch for synchronizing events
def writeEventlist(msg, directory, eventlist, listname='event'):
    msg.verbose('%s begin write event list', time.ctime(time.time()))
    pid = os.getpid()
    try:
        file = open("%s/%s.%d" % (directory, listname, pid), 'w')
        for run, event, atlantis, vp1 in eventlist:
            file.write("run:%s,event:%s,atlantis:%s,vp1:%s\n" % (run, event, atlantis, vp1))
        file.close()
    except IOError as err:
        msg.warning("Could not write event list: %s", err)

    # Rename for an atomic overwrite operation
    try:
        os.rename("%s/%s.%d" % (directory, listname, pid), "%s/%s.list" % (directory, listname))
    except OSError as err:
        msg.warning("Could not rename %s.%d to %s.list: %s", listname, pid, listname, err)
    msg.verbose('%s end write event list', time.ctime(time.time()))

# Perform all of these in one command
def cleanDirectory(msg, directory, maxevents, checkpair):
    msg.verbose('%s begin clean directory', time.ctime(time.time()))
    eventlist = getEventlist(msg, directory, checkpair)
    if maxevents>0:
        pruneEvents(msg, directory, maxevents, eventlist)
    writeEventlist(msg, directory, eventlist)

    # disable this for beam splashes. Call zipXMLFile directly in OnlineEventDisplaysSvc.py to transfer every event.
    if len(eventlist)>0:
        prepareFilesForTransfer(msg, directory, eventlist, pair=checkpair, timeinterval=60)

    msg.verbose('%s end clean directory', time.ctime(time.time()))

def prepareFilesForTransfer(msg, directory, eventlist, pair, timeinterval):
    """Preparing the list of files for CastorScript to transfer to EOS

    CastorScript is configured to look for *.zip and *.online.pool.root files to transfer.
    This function count the number of CastorScript bookkeeping files *.COPIED
    and compare with the number of original data files *.zip and *.online.pool.root to obtain the transfer status.
    If all transfer has been completed, get the most recent event from eventlist and add it to the transferlist.
    Zip the corresponding xml file or rename the corresponding pool.root file to trigger the transfer.

    eventlist: list of events that can be transferred
    pair: True to transfer atlantis-vp1 pairs. False to transfer atlantis file only.
    timeinterval: time interval between two events in the transfer list (in seconds).
    """
    msg.verbose('%s begin prepare files for transfer', time.ctime(time.time()))
    transferlist = getEventlist(msg, directory, checkpair=pair, remove=False, patternAtlantis='.zip', patternVP1='.online.pool.root')
    if len(transferlist)>0 and eventlist[-1][0] == transferlist[-1][0] and eventlist[-1][1] == transferlist[-1][1]:
        msg.debug("Last event already in transfer list. No new event to transfer.")
        return

    # Check transfer status
    matchAtlantis = glob.glob(f"{directory}/*.zip") # atlantis files ready for transfer
    copiedAtlantis = glob.glob(f"{directory}/*.zip.COPIED") # CastorScript bookkeeping files indicating the transfer is done
    matchVP1 = glob.glob(f"{directory}/*.online.pool.root") # VP1 files ready for transfer
    copiedVP1 = glob.glob(f"{directory}/*.online.pool.root.COPIED") # CastorScript bookkeeping files indicating the transfer is done
    # Check if the transfer is done
    if len(matchAtlantis)>len(copiedAtlantis) or len(matchVP1)>len(copiedVP1):
        msg.debug("There are files in transfer. Do not attemp to add new event to the transfer list.")
        return
    # Check if the previous transferred event is too new
    elif len(transferlist)>0: # use the sorted list here
        try:
            age = time.time() - os.path.getmtime(f'{directory}/{transferlist[-1][2]}') # check the timestamp of the latest atlantis zip file
            if age < timeinterval:
                msg.debug("Wait for %ds before adding new events to the transfer queue. Last event in the queue was added %ds ago.", timeinterval, age)
                return
        except OSError as err:
            msg.warning("Failed to check the timestamp of %s. %s", transferlist[-1][2], err)
            return

    # Get the latest event from event list to transfer
    run, event, atlantis, vp1 = eventlist[-1]

    # Handle atlantis files
    msg.debug('%s going to zip file %s', time.ctime(time.time()), atlantis)
    zipXMLFile(msg, directory, atlantis)

    # Handle VP1 files
    if pair:
        msg.debug('%s going to rename ESD file %s', time.ctime(time.time()), vp1)
        renameESDFile(msg, directory, vp1)

    writeEventlist(msg, directory, transferlist, listname='transfer')
    msg.verbose('%s end prepare files for transfer', time.ctime(time.time()))

def zipXMLFile(msg, directory, filename):
    """Zip the JiveXML file for the specified event.

    Looks for a JiveXML file with the required filename in the given directory,
    and if one is found, zip it. The original file is not deleted.
    Zip the file to .tmp first, and then rename to .zip
    to avoid triggering the transfer before the zip file is closed.
    """
    msg.verbose('%s begin zipXMLFile', time.ctime(time.time()))
    if Path(filename).suffix != '.xml':
        msg.warning("Unexpected Atlantis file name: %s", filename)
        return
    matchingFiles = glob.glob(f"{directory}/{filename}")
    if len(matchingFiles) == 1: # Only proceed if exactly one matching file found, for safety
        msg.verbose('exactly one matching file found')
        matchingFilePath = Path(matchingFiles[0])
        tmpFilePath      = matchingFilePath.with_suffix('.tmp')
        zipFilePath      = matchingFilePath.with_suffix('.zip')
        matchingFilePath = Path(matchingFilePath)
        matchingFileName = matchingFilePath.name
        msg.verbose('Zipping %s to %s', matchingFileName, zipFilePath.name)
        try:
            with ZipFile(tmpFilePath,'w', compression=ZIP_DEFLATED) as z:
                z.write(matchingFilePath.as_posix(), arcname=matchingFileName)
            os.rename(f'{directory}/{tmpFilePath.name}', f'{directory}/{zipFilePath.name}')
        except OSError as err:
            msg.warning("Could not zip %s: %s", filename, err)
    msg.verbose('%s end of zipXMLFile', time.ctime(time.time()))

def renameESDFile(msg, directory, filename):
    """Rename the ESD for the specified event.

    Looks for an ESD file with the required filename in the given directory,
    and if one is found, rename it to .online.pool.root. The original file is not deleted.
    """
    msg.verbose('%s begin renameESD', time.ctime(time.time()))
    if Path(filename).suffixes != ['.pool', '.root']:
        msg.warning("Unexpected VP1 file name: %s", filename)
        return
    orgname = f'{directory}/{filename}'
    newname = orgname.replace('.pool.root', '.online.pool.root')
    try:
        shutil.copyfile(Path(orgname), Path(newname))
    except OSError as err:
        msg.warning("Could not copy %s to %s: %s", orgname, newname, err)
    msg.verbose('%s end of renameESD', time.ctime(time.time()))
