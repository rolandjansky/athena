# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys, os, string


def usage():
    print "Usage: python updatePlugins.py OB PLUGIN IDTAG OLDTAG NEWTAG"
    sys.exit(1)


def search_tags(idtag, oldtag, newtag):
    command = "${HOME}/daq/Applications/Pixel/CoralDB/ccdb --db $PIX_CONN_DB  getExistingTags"
    print command
    lines = os.popen(command).readlines()

    count = 0
    search = "ObjDictTag [%s]" % idtag
    idfound = False
    oldfound = False
    newfound = True
    for i in lines:
        if i.find(search)>=0:
            idfound = True
            if i.find(oldtag)>=0:
                oldfound = True
                if i.find(newtag)<0:
                    newfound = False
                    return True
    if idfound == False:
        print idtag + " not found in $PIX_CONN_DB"
        sys.exit(1)
    if oldfound == False:
        print oldtag + " not found in idTag " + idtag
        sys.exit(1)
    if newfound == True:
        print newtag + " already exists and will not be updated"
        sys.exit(1)
    return False


def check_type(object, idtag, tag):
    command = "${HOME}/daq/Applications/Pixel/CoralDB/ccdb --db $PIX_CONN_DB  --idTag " + idtag + " --tags " + tag + " objectType " + object
    print command
    lines = os.popen(command).readlines()
    return lines


def update_modIds(object, idtag, tag):
    # Check that the given object is an optoboard
    lines = check_type(object, idtag, newtag)
    if len(lines) < 1:
        print object + " does not exist. Can't do anything with it."
        sys.exit(1)
    elif not string.strip(lines[0]) == "OPTOBOARD":
        print object + " is not an optoboard and this script only works for optoboards. I stop here."
        sys.exit(1)
    command = "${HOME}/daq/Applications/Pixel/CoralDB/ccdb --db $PIX_CONN_DB  --idTag " + idtag + " --tags " + tag + " findInConn " + object
    print command
    lines = os.popen(command).readlines()
    if len(lines) < 1:
        print "The OB is not connected to a ROD, cannot update plugins!"
        sys.exit(1) 
    if len(lines) > 1:
        print "Why do I get more than one line??? I will only consider the one with slopt UP"

    for i in lines:
        if i.find("UP")>=0:
            tokens = i.split("/")
            tokens1 = tokens[1].split("--->")
            slot = string.strip(tokens1[0])

    slot_num = -1
    if slot[1] == 'A':
        slot_num = 0
    if slot[1] == 'B':
        slot_num = 1
    if slot[1] == 'C':
        slot_num = 2
    if slot[1] == 'D':
        slot_num = 3
    if slot_num == -1:
        print "The RX slot is not valid. It should be A, B, C or D"
        sys.exit(1)

    command = "${HOME}/daq/Applications/Pixel/CoralDB/ccdb --db $PIX_CONN_DB  --idTag " + idtag + " --tags " + tag + " findOutConn " + object
    print command
    lines = os.popen(command).readlines()

    pp0 = ""
    for i in lines:
        if i.find("PP0")>=0:
            tokens = i.split("--->")
            tokens1 = tokens[1].split("/")
            pp0 = string.strip(tokens1[0])

    if not pp0 == "":
        command = "${HOME}/daq/Applications/Pixel/CoralDB/ccdb --db $PIX_CONN_DB  --idTag " + idtag + " --tags " + tag + " findOutConn " + pp0
        print command
        lines = os.popen(command).readlines()

    for i in lines:
        if i.find("UP")>=0:
            tokens = i.split("--->")
            tokens1 = tokens[1].split("/")
            module = string.strip(tokens1[0])
            tokens2 = tokens[0].split("/")
            mod_slot = string.strip(tokens2[1])
            if mod_slot[0] == 'M':
                mod_slot_num = int(mod_slot[1])
            else:
                mod_slot_num = int(mod_slot[0])
            modId = slot_num * 7 + (mod_slot_num - 1) % 7
            command = "${HOME}/daq/Applications/Pixel/CoralDB/ccdb --db $PIX_CONN_DB  --idTag " + idtag + " --tags " + tag + " updateCLOB " + module + " Identifier_ModId 'int %d '" %(modId)
            print command
            os.system(command)

            
if not len(sys.argv) == 6:
    usage()

ob = sys.argv[1]
plugin = sys.argv[2]
idtag = sys.argv[3]
oldtag = sys.argv[4]
newtag = sys.argv[5]

# Check that idTag exists, old tag is there and new tag not. Stop otherwise
search_tags(idtag, oldtag, newtag)
command = "${HOME}/daq/Applications/Pixel/CoralDB/ccdb --db $PIX_CONN_DB  --idTag " + idtag + " --tags " + oldtag + " copyTags " + newtag
print command
os.system(command)

# Check that the given object is an optoboard
lines = check_type(ob, idtag, newtag)
if len(lines) < 1:
    print ob + " does not exist. Can't do anything with it."
    sys.exit(1)
elif not string.strip(lines[0]) == "OPTOBOARD":
    print ob + " is not an optoboard and this script only works for optoboards. I stop here."
    sys.exit(1)

# Read the name of the ROD to which it is connected and modify the connection 
command = "${HOME}/daq/Applications/Pixel/CoralDB/ccdb --db $PIX_CONN_DB  --idTag " + idtag + " --tags " + newtag + " findInConn " + ob
print command
lines = os.popen(command).readlines()
if len(lines) < 1:
    print "The OB is not connected to a ROD, cannot update plugins!"
    sys.exit(1) 
if len(lines) > 1:
    print "Why do I get more than one line??? I will only consider the one with slopt UP"

# Look for the ROD name
rod = ""

for i in lines:
    if i.find("UP")>=0:
        tokens = i.split("/")
        rod = string.strip(tokens[0])

# Delete the old connection and insert a new one
if not rod == "":
    command = "${HOME}/daq/Applications/Pixel/CoralDB/ccdb --db $PIX_CONN_DB  --idTag " + idtag + " --tags " + newtag + " deleteInConnection " + ob + " UP"
    print command
    lines = os.popen(command).readlines()
    if not len(lines) == 1:
        print "Strange output! Something went wrong. Stopping here."
        sys.exit(1)
    elif not string.strip(lines[0]) == "1":
        print "Connection was not deleted. Stopping here."
        sys.exit(1)
    command = "${HOME}/daq/Applications/Pixel/CoralDB/ccdb --db $PIX_CONN_DB  --idTag " + idtag + " --tags " + newtag + " insertConnection " + rod + " " + plugin + " " + ob + " UP"
    print command
    os.system(command)

    update_modIds(ob, idtag, newtag)

    command = "${HOME}/daq/Applications/Pixel/CoralDB/ccdb --db $PIX_CONN_DB  --idTag " + idtag + " --tags " + newtag + " setHistoricTag"
    print command
    os.system(command)

else:
    print "No UP connection for the optoboard found, not updating"
    sys.exit(1)
