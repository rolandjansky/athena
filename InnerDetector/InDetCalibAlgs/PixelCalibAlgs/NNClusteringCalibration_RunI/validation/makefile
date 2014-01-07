all: produceValidationNtuple


RIOs.o : RIOs.C RIOs.h
	g++ -c RIOs.C -g  `root-config --cflags` `root-config --libs`

writeNtplue.o : writeNtuple.C  writeNtuple.h RIOs.o
	g++ -c writeNtuple.C -g  `root-config --cflags` `root-config --libs` -L . -l  TJetNet  -I . 

produceValidationNtuple : produceTrkValidation.cpp writeNtplue.o RIOs.o 
	g++ -o produceTrkValidation produceTrkValidation.cpp RIOs.C -g  `root-config --cflags` `root-config --libs` -L . -lTJetNet   -I .
# ln -sf libprint.so.1 libprint.so	
clean :
	rm *.o
