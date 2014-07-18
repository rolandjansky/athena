# Define the symbols we might want to change:
CXX	:= g++
CXXFLAGS := -g -O2
OBJECTS := InDetTrackTree.o RealTrackTree.o TrackingAna.o

TrackingAna:
	$(CXX) $(CXXFLAGS) `root-config --cflags` -c InDetTrackTree.C RealTrackTree.C TrackingAna.C
	$(CXX) $(OBJECTS) `root-config --ldflags --libs` -o TrackingAna.exe

clean:
	rm *.o

binclean:
	rm *.o *.exe
