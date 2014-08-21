
INCDIR="../"

.PHONY: TestClient

#Get OS type
OSTYPE=$(shell uname -s )

#Check for linux
ifeq '$(findstring Linux,${OSTYPE})' 'Linux'
OSTYPE=LINUX
endif
ifeq '$(findstring Darwin,${OSTYPE})' 'Darwin'
OSTYPE=DARWIN
EXEEXTN=OSX
endif

TestClient: TestClient.cxx ../src/ONCRPCXDRProcs.cxx
	g++ -g -D${OSTYPE} -I${INCDIR} -o $@${EXEEXTN} $^ 

TestXDR: TestXDR.cxx ../src/ONCRPCXDRProcs.cxx
	g++ -g -D${OSTYPE} -I${INCDIR} -o $@${EXEEXTN} $^ 
