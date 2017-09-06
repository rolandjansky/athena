g++ -g -c run.C FCS_CellDict.cxx -I $(root-config --incdir) -I . -fPIC $CPPEXPFLAGS -std=c++11
g++ -g -o CaloHitAna run.o FCS_CellDict.o -L $(root-config --libdir) -lGui -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -lpthread -Wl,-rpath,$(root-config --libdir) -lm -ldl -fPIC $CPPEXPFLAGS -std=c++11


