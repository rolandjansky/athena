CXX	:= g++
CXXFLAGS := -g -O2 '-I./TrigD3PD/'
OBJECTS := InDetTrackTree.o RealTrackTree.o EnhancedVertexMonitoring.o ./TrigD3PD/libTriggerD3PDHelpers.a

TrackingAna:
	$(CXX) $(CXXFLAGS) `root-config --cflags` -c InDetTrackTree.C RealTrackTree.C EnhancedVertexMonitoring.C
	$(CXX) $(OBJECTS) `root-config --ldflags --libs` -o EnhancedVertexMonitoring.exe

clean:
	rm *.o

binclean:
	rm *.o *.exe
