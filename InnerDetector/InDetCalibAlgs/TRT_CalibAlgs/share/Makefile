LFLAGS = -g -O2 -m32 -Wno-deprecated -L${ROOTSYS}/lib  -I${ROOTSYS}/include -lCore -lCint -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -pthread -lm -ldl -rdynamic  -lProof -lThread
#LFLAGS = -g -O2 -Wno-deprecated -L${ROOTSYS}/lib  -I${ROOTSYS}/include -lCore -lCint -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -pthread -lm -ldl -rdynamic  -lProof -lThread

TRT_StrawStatusReport: TRT_StrawStatusReport.o
	g++ -g -o $@ $^

%.o: %.cc 
	g++ -g -c -o $@ $< 

%: %.cpp
	g++ -o $@ ${LFLAGS} $@.cpp

%: %.cc
	g++ -o $@ $@.cc

clean:
	rm *.o *~

