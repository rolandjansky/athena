include $(ROOTSYS)/test/Makefile.arch

MERGEO       = mergeD3PD.$(ObjSuf)
MERGES       = mergeD3PD.$(SrcSuf)
MERGE        = mergeD3PD$(ExeSuf)

FILTERO       = FilterAndMergeD3PD.$(ObjSuf)
FILTERS       = FilterAndMergeD3PD.$(SrcSuf)
FILTER        = FilterAndMergeD3PD$(ExeSuf)

OBJS          = $(FILTERO) $(MERGEO) 
PROGRAMS      = $(FILTER) $(MERGE)

#------------------------------------------------------------------------------

.SUFFIXES: .$(SrcSuf) .$(ObjSuf) .$(DllSuf)

all:            $(PROGRAMS)

$(FILTER):      $(FILTERO)
		$(LD) $(LDFLAGS) $^ $(LIBS) $(OutPutOpt)$@
		$(MT_EXE)
		@echo "$@ done"

$(MERGE):      $(MERGEO)
		$(LD) $(LDFLAGS) $^ $(LIBS) $(OutPutOpt)$@
		$(MT_EXE)
		@echo "$@ done"

clean:
		@rm -f $(OBJS) core

.SUFFIXES: .$(SrcSuf)


.$(SrcSuf).$(ObjSuf):
	$(CXX)  $(CXXFLAGS) -c $<
