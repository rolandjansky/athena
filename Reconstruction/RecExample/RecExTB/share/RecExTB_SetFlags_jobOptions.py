# Set the FilePrefix variable
# find the files
import string
dataPathList = os.environ[ 'DATAPATH' ].split(os.pathsep)
dataPathList.insert(0, os.curdir)
from AthenaCommon.Utils.unixtools import FindFile
filename = FindFile( 'prefix.txt', dataPathList, os.R_OK )
if filename is None:
   print('ERROR: SetFlags.py could not find list of file prefixes, prefix.txt')
text = open(filename,'r')
for line in text.readlines():
    words = string.split(line)
    if len(words) >= 2:
        Run = words[0]
        if string.atoi(Run) == RunNumber:
            FilePrefix = words[1]
text.close()
print('Prefix ', FilePrefix)
