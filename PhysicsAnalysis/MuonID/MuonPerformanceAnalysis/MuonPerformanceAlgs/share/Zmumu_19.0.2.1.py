if not 'FILELIST' in globals():
	FILELIST="filelist.txt"
	
print "running with file list %s"%FILELIST				

with open(FILELIST,'r') as input:
    InputAODList=[bla for bla in input.read().split("\n") if bla != ""and not '#' in bla ]
    #print InputAODList


