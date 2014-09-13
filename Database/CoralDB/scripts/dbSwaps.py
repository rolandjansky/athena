# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys, os, string


def usage():
    print "Usage: python dbSwaps.py [--OB, --ROD] NAME1 NAME2 IDTAG OLDTAG NEWTAG"
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
        if i.find(search) >= 0:
            idfound = True
            if i.find(oldtag) >= 0:
                oldfound = True
                if i.find(newtag) < 0:
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
 
def swap_optos(idtag, newtag, name1, name2):
    # Read ROD name and plugin for each opto
    command1 = "${HOME}/daq/Applications/Pixel/CoralDB/ccdb --db $PIX_CONN_DB  --idTag " + idtag + " --tags " + newtag + " findInConn " + name1
    print command1
    lines1 = os.popen(command1).readlines()
    print lines1
    command2 = "${HOME}/daq/Applications/Pixel/CoralDB/ccdb --db $PIX_CONN_DB  --idTag " + idtag + " --tags " + newtag + " findInConn " + name2
    print command2
    lines2 = os.popen(command2).readlines()
    print lines2
    if len(lines1) < 1 or len(lines2) < 1:
        print "One of the two OBs is not connected to a ROD, cannot swap!"
        sys.exit(1) 
    if len(lines1) > 1 or len(lines2) > 1:
        print "Why do I get more than one line??? I will only consider the one with slopt UP"

    for i in lines1:
        if i.find("UP") >= 0:
            tokens1 = i.split("/")
            rod1 = string.strip(tokens1[0])
            tokens2 = tokens1[1].split("-")
            plugin1 = string.strip(tokens2[0])

    for i in lines2:
        if i.find("UP") >= 0:
            tokens3 = i.split("/")
            rod2 = string.strip(tokens3[0])
            tokens4 = tokens3[1].split("-")
            plugin2 = string.strip(tokens4[0])

    # Delete existing connections
    command = "${HOME}/daq/Applications/Pixel/CoralDB/ccdb --db $PIX_CONN_DB  --idTag " + idtag + " --tags " + newtag + " deleteInConnection " + name1 + " UP"
    print command
    lines = os.popen(command).readlines()
    if not len(lines) == 1:
        print "Strange output! Something went wrong. Stopping here."
        sys.exit(1)
    elif not string.strip(lines[0]) == "1":
        print "Connection was not deleted. Stopping here."
        sys.exit(1)
        
    command = "${HOME}/daq/Applications/Pixel/CoralDB/ccdb --db $PIX_CONN_DB  --idTag " + idtag + " --tags " + newtag + " deleteInConnection " + name2 + " UP"
    print command
    lines = os.popen(command).readlines()
    if not len(lines) == 1:
        print "Strange output! Something went wrong. Stopping here."
        sys.exit(1)
    elif not string.strip(lines[0]) == "1":
        print "Connection was not deleted. Stopping here."
        sys.exit(1)
        
    # Insert new connections
    command = "${HOME}/daq/Applications/Pixel/CoralDB/ccdb --db $PIX_CONN_DB  --idTag " + idtag + " --tags " + newtag + " insertConnection " + rod2 + " " + plugin2 + " " + name1 + " UP"
    print command
    os.system(command)

    command = "${HOME}/daq/Applications/Pixel/CoralDB/ccdb --db $PIX_CONN_DB  --idTag " + idtag + " --tags " + newtag + " insertConnection " + rod1 + " " + plugin1 + " " + name2 + " UP"
    print command
    os.system(command)
    


def swap_rods(idtag, newtag, name1, name2):
    command1 = "${HOME}/daq/Applications/Pixel/CoralDB/ccdb --db $PIX_CONN_DB  --idTag " + idtag + " --tags " + newtag + " findOutConn " + name1
    print command1
    lines1 = os.popen(command1).readlines()
    print lines1
    command2 = "${HOME}/daq/Applications/Pixel/CoralDB/ccdb --db $PIX_CONN_DB  --idTag " + idtag + " --tags " + newtag + " findOutConn " + name2
    print command2
    lines2 = os.popen(command2).readlines()
    print lines2
    if len(lines1) < 1 or len(lines2) < 1:
        print "One of the two RODs is not connected to anything, cannot swap!"
        sys.exit(1)  

    ob1 = []
    plugin1 = []

    count = 0
    for conn in lines1:
        if conn.find("UP") >= 0:
            print conn
            tokens1 = conn.split("--->")
            tokens2 = tokens1[0].split("/")
            plugin1.append(string.strip(tokens2[1]))
            tokens3 = tokens1[1].split("/")
            ob1.append(string.strip(tokens3[0]))
            command = "${HOME}/daq/Applications/Pixel/CoralDB/ccdb --db $PIX_CONN_DB  --idTag " + idtag + " --tags " + newtag + " deleteOutConnection " + name1 + " " + plugin1[count]
            print command
            output = os.popen(command).readlines()
            if len(output) > 1:
                print "What is all this output?? What went wrong?? Sorry, I don't know what to do now... I exit"
                sys.exit(1)
            count = count + 1

    ob2 = []
    plugin2 = []

    count = 0
    for conn in lines2:
        if conn.find("UP") >= 0:
            print conn
            tokens1 = conn.split("--->")
            tokens2 = tokens1[0].split("/")
            plugin2.append(string.strip(tokens2[1]))
            tokens3 = tokens1[1].split("/")
            ob2.append(string.strip(tokens3[0]))
            command = "${HOME}/daq/Applications/Pixel/CoralDB/ccdb --db $PIX_CONN_DB  --idTag " + idtag + " --tags " + newtag + " deleteOutConnection " + name2 + " " + plugin2[count]
            print command
            output = os.popen(command).readlines()
            if len(output) > 1:
                print "What is all this output?? What went wrong?? Sorry, I don't know what to do now... I exit"
                sys.exit(1)
            count = count + 1

    for i in range(len(ob1)):
        command = "${HOME}/daq/Applications/Pixel/CoralDB/ccdb --db $PIX_CONN_DB  --idTag " + idtag + " --tags " + newtag + " insertConnection " + name2 + " " + plugin1[i] + " " + ob1[i] + " UP" 
        print command
        output = os.popen(command).readlines()
        if len(output) > 1:
            print "What is all this output?? What went wrong?? Sorry, I don't know what to do now... I exit"
            sys.exit(1)

    for i in range(len(ob2)):
        command = "${HOME}/daq/Applications/Pixel/CoralDB/ccdb --db $PIX_CONN_DB  --idTag " + idtag + " --tags " + newtag + " insertConnection " + name1 + " " + plugin2[i] + " " + ob2[i] + " UP" 
        print command
        output = os.popen(command).readlines()
        if len(output) > 1:
            print "What is all this output?? What went wrong?? Sorry, I don't know what to do now... I exit"
            sys.exit(1)
    
    return True

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
            
        
if not len(sys.argv) == 7:
    usage()

mode=""
if sys.argv[1] == "--OB":
    mode = "OB"
elif sys.argv[1] == "--ROD":
    mode = "ROD"
else:
    usage()

name1 = sys.argv[2]
name2 = sys.argv[3]
idtag = sys.argv[4]
oldtag = sys.argv[5]
newtag = sys.argv[6]

# Check that idTag exists, old tag is there and new tag not. Stop otherwise
search_tags(idtag, oldtag, newtag)
command = "${HOME}/daq/Applications/Pixel/CoralDB/ccdb --db $PIX_CONN_DB  --idTag " + idtag + " --tags " + oldtag + " copyTags " + newtag
print command
os.system(command)

if (mode == "OB"):
    # Check that objects are optoboards
    lines1 = check_type(name1, idtag, newtag)
    if len(lines1) < 1:
        print name1 + " does not exist. Can't do anything with it."
        sys.exit(1)
    elif not string.strip(lines1[0]) == "OPTOBOARD":
        print name1 + " is not an optoboard and this option only works for optoboards. I stop here."
        sys.exit(1)
    lines2 = check_type(name2, idtag, newtag)
    if len(lines2) < 1:
        print name2 + " does not exist. Can't do anything with it."
        sys.exit(1)
    elif not string.strip(lines2[0]) == "OPTOBOARD":
        print name2 + " is not an optoboard and this option only works for optoboards. I stop here."
        sys.exit(1)
    # Swap them
    swap_optos(idtag, newtag, name1, name2)

    update_modIds(name1, idtag, newtag)
    update_modIds(name2, idtag, newtag)

if (mode == "ROD"):
    # Check that objects are RODs
    lines1 = check_type(name1, idtag, newtag)
    if len(lines1) < 1:
        print name1 + " does not exist. Can't do anything with it."
        sys.exit(1)
    elif not string.strip(lines1[0]) == "RODBOC":
        print name1 + " is not a ROD and this option only works for RODs. I stop here."
        sys.exit(1)
    lines2 = check_type(name2, idtag, newtag)
    if len(lines2) < 1:
        print name2 + " does not exist. Can't do anything with it."
        sys.exit(1)
    elif not string.strip(lines2[0]) == "RODBOC":
        print name2 + " is not an ROD and this option only works for RODs. I stop here."
        sys.exit(1)
    # Swap them
    swap_rods(idtag, newtag, name1, name2)

    command = "${HOME}/daq/Applications/Pixel/CoralDB/ccdb --db $PIX_CONN_DB  --idTag " + idtag + " --tags " + newtag + " findOutConn " + name1
    print command
    lines = os.popen(command).readlines()

    print lines
    for i in lines:
        print i
        if i.find("UP")>=0:
            print i
            tokens = i.split("--->")
            tokens1 = tokens[1].split("/")
            ob = string.strip(tokens1[0])
            print ob
            update_modIds(ob, idtag, newtag)

command = "${HOME}/daq/Applications/Pixel/CoralDB/ccdb --db $PIX_CONN_DB  --idTag " + idtag + " --tags " + newtag + " setHistoricTag"
print command
os.system(command)
