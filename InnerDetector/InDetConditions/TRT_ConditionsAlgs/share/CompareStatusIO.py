import os,sys,argparse

parser = argparse.ArgumentParser(description="%prog [options]", formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument("-doStatus", dest='doStatus', action='store_true', default=False, help="compare Status tag")
parser.add_argument("-doStatusHT", dest='doHT', action='store_true', default=False, help="compare StatusHT tag")
parser.add_argument("-doStatusPermanent", dest='doPermanent', action='store_true', default=False, help="compare StatusPermanent tag")
parser.add_argument("-inFile", dest='IFile', default=None, help="hand-made file (input to txt2db_template.py) to compare to output")
parser.add_argument("-outFile", dest='OFile', default=None, help="file dumped from db2txt_template.py to compare to input")
args = parser.parse_args()

IFileIn= args.IFile
OFileIn= args.OFile

ITxt=open(IFileIn).readlines()
OTxt=open(OFileIn).readlines()

Inewtokens=[]
Onewtokens=[]

def main():
    isPermanent=False
    isHT=False
    isStatus=False

    if not (args.doStatus or args.doPermanent or args.doHT):
        print "please specify doStatus, doPermanent, and/or doHT"
        return

    # figure out type we are comparing
    for line in OTxt:
        if "permanent Status" in line:
            isPermanent=True
        if "HT Status" in line:
            isHT=True
        if "Status" in line and not ("permanent" in line or "HT" in line):
            isStatus=True
    if (args.doPermanent and not isPermanent) or (args.doHT and not isHT) or (args.doStatus and not isStatus):
        print "check your inputs, they don't have the requested Status type"
        return

    # loop through input
    # FIXME: assuming the input only has one conditions type (HT or Permanent but not both)
    for line in ITxt:
        line=line.replace("  "," ")
        line=line[0].replace(" ","")+line[1:]      # remove leading spaces
        line=line[:-1]+line[-1].replace(" ","")    # remove trailing spaces
        tokens=line.split(" ")
        if len(tokens)<6:
            if not "Status:" in tokens:
                print "length not satisfied: ",tokens
            continue
        Inewtokens.append(line.split("\n")[0])

    # loop through output to match input formatting
    seenStart=False
    seenEnd=False
    for line in OTxt:
        line=line.replace("  "," ")
        line=line[0].replace(" ","")+line[1:]      # remove leading spaces
        if isPermanent or isStatus:
            line=line.replace("257","1")           # different formatting of "bad" straws
        line=line[:-1]+line[-1].replace(" ","")    # remove trailing spaces
        if (isHT or isStatus) and "2" in line.split(" ")[-1].split("\n")[0]:
            # ignore lines that end in 2 because they're Xe in StatusHT and alive in Status
            continue
        tokens=line.split(" ")
        if args.doPermanent:
            if (not seenEnd) and "permanent" in tokens:
                seenStart=True
            elif seenStart and "Status:" in tokens:
                seenEnd=True
        elif args.doHT:
            if (not seenEnd) and "HT" in tokens:
                seenStart=True
            elif seenStart and "Status:" in tokens:
                seenEnd=True
        elif args.doStatus:
            if (not seenEnd) and "Status:" in tokens and not ("HT" in tokens or "permanent" in tokens):
                seenStart=True
            elif seenStart and ("HT" in tokens or "permanent" in tokens):
                seenEnd=True
        if seenStart and not seenEnd:
            if len(tokens)<6 :
                if not "Status:" in tokens:
                    print "length not satisfied: ",tokens
                continue
            Onewtokens.append(tokens[0]+" " \
                                  +tokens[2]+" " \
                                  +tokens[4]+" " \
                                  +tokens[3]+" " \
                                  +tokens[1]+" " \
                                  +tokens[5].split("\n")[0] \
                                  )

    print set(Inewtokens) ^ set(Onewtokens) # checking symmetric difference

if __name__=='__main__':
    main()
