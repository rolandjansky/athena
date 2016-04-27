
include Makefile.arch

#include $(CMTROOT)/src/Makefile.header

#include $(CMTROOT)/src/constituents.make
#------------------------------------------------------------------------------


TRACKPLOTTERO    = trackplotter.$(ObjSuf) trackplotterDict.$(ObjSuf) display_track.$(ObjSuf) PlotValNtuple.$(ObjSuf) ShapeCreator.$(ObjSuf) TrackDataNtupleTree.$(ObjSuf) TrkValidationNtupleOutputTrack.$(ObjSuf) TrkValidationNtupleRIOs.$(ObjSuf)
TRACKPLOTTERS    = trackplotter.$(SrcSuf) trackplotterDict.$(SrcSuf) display_track.$(SrcSuf) PlotValNtuple.$(SrcSuf) ShapeCreator.$(SrcSuf) TrackDataNtupleTree.$(SrcSuf) TrkValidationNtupleOutputTrack.$(SrcSuf) TrkValidationNtupleRIOs.$(SrcSuf)
TRACKPLOTTER     = trackplotter$(ExeSuf)


OBJS         +=  $(TRACKPLOTTERO)
PROGRAMS     +=  $(TRACKPLOTTER)

#------------------------------------------------------------------------------

.SUFFIXES: .$(SrcSuf) .$(ObjSuf) .$(DllSuf)

all:            $(PROGRAMS)


$(TRACKPLOTTER):   $(TRACKPLOTTERO)
		$(LD) $(LDFLAGS) $^ $(GLIBS) $(OutPutOpt)$@
		@echo "$@ done"


clean:
		@rm -f $(OBJS) core

distclean:      clean
		@rm -f $(PROGRAMS) $(EVENTSO) $(EVENTLIB) *Dict.* *.def *.exp \
		   *.root *.ps *.so *.lib *.dll *.d .def so_locations
		@rm -rf cxx_repository
		-@cd RootShower && $(MAKE) distclean

.SUFFIXES: .$(SrcSuf)

###


trackplotter.$(ObjSuf): trackplotter.h
trackplotterDict.$(SrcSuf): trackplotter.h trackplotterLinkDef.h
	@echo "Generating dictionary $@..."
	@rootcint -f $@ -c $^

.$(SrcSuf).$(ObjSuf):
	$(CXX) $(CXXFLAGS) -c $<
