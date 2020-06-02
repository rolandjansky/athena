#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# This function merges a list of input LHE file to make one outputFile.  The header is taken from the first
# file, but the number of events is updated to equal the total number of events in all the input files
def merge_lhe_files(listOfFiles,outputFile):
    if(os.path.exists(outputFile)):
      printfunc ("outputFile ",outputFile," already exists.  Will rename to ",outputFile,".OLD")
      os.rename(outputFile,outputFile+".OLD")
    output = open(outputFile,'w')
    holdHeader = ""
    nevents=0
    for file in listOfFiles:
       cmd = "grep /event "+file+" | wc -l"
       nevents+=int(subprocess.check_output(cmd,stderr=subprocess.STDOUT,shell=True))

    for file in listOfFiles:
       inHeader = True
       header = ""
       printfunc ("*** Starting file ",file)
       for line in open(file,"r"):
##        Reading first event signals that we are done with all the header information
##        Using this approach means the script will properly handle any metadata stored
##        at the beginning of the file.  Note:  aside from the number of events, no metadata
##        is updated after the first header is read (eg the random number seed recorded will be
##        that of the first file.
          if("<event" in line and inHeader):
             inHeader = False
             if(len(holdHeader)<1):
                holdHeader = header
                output.write(header)
             output.write(line)
##        each input file ends with "</LesHouchesEvents>".  We don't want to write this out until all
##        the files have been read.  The elif below writes out all the events.
          elif(not inHeader and not ("</LesHouchesEvents>" in line)):
              output.write(line)
          if(inHeader):
##           Format for storing number of events different in MG and Powheg
             if("nevents" in line and "Required accuracy" not in line):
##              MG5 format is "n = nevents"
                tmp = line.split("=")
                line = line.replace(tmp[0],str(nevents))
             elif("numevts" in line):
##              Powheg format is numevts n
                tmp = line.split(" ")
                nnn = str(nevents)
                line = line.replace(tmp[1],nnn)
             header+=line
    output.write("</LesHouchesEvents>\n")
    output.close()

