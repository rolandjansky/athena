if [  -f FCS_CellDict.cxx ];
then
 rm FCS_CellDict.cxx
 rm FCS_CellDict.h
fi
rootcint FCS_CellDict.cxx -c FCS_Cell.h Linkdef.h

#compile
g++ -g -c run.C FCS_CellDict.cxx -I $(root-config --incdir) -I . -fPIC $CPPEXPFLAGS -std=c++11

#link executable
g++ -g -o CaloHitAna run.o FCS_CellDict.o -L $(root-config --libdir) -lGui -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -lpthread -Wl,-rpath,$(root-config --libdir) -lm -ldl -fPIC $CPPEXPFLAGS -std=c++11
	
#this creates a shared library for ROOT (which can be loaded by hand)
g++ -shared -o FCS_Cell_h.so FCS_CellDict.o -L $(root-config --libdir) -lGui -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -lpthread -Wl,-rpath,$(root-config --libdir) -lm -ldl -fPIC $CPPEXPFLAGS -std=c++11
	
#example
g++ -g -c CaloHitAna2.C -I $(root-config --incdir) -I . -fPIC $CPPEXPFLAGS -std=c++11

g++ -g -o CaloHitAna2 CaloHitAna2.o FCS_CellDict.o -L $(root-config --libdir) -lGui -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -lpthread -Wl,-rpath,$(root-config --libdir) -lm -ldl -lstdc++ -fPIC $CPPEXPFLAGS -std=c++11
	
#another example
g++ -g -c CaloHitAna3.C FCS_CellDict.cxx -Wall -I $(root-config --incdir) -I . -fPIC $CPPEXPFLAGS -Wall -std=c++11

g++ -g -o CaloHitAna3 CaloHitAna3.o FCS_CellDict.o -L $(root-config --libdir) -lGui -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -lpthread -Wl,-rpath,$(root-config --libdir) -lm -ldl -lstdc++ -fPIC $CPPEXPFLAGS -Wall -std=c++11

