# this file should be included by the top level makefile

ROOTCFLAGS+=$(shell root-config --cflags)
CXXFLAGS+=-g -fPIC -Wall -DFTK_STANDALONE -I. $(ROOTCFLAGS) $(BOOST_CXXFLAGS) $(DCAP_CXXFLAGS) $(EIGEN_CXXFLAGS)
LIBS+=-L$(shell root-config --libdir) $(shell root-config --prefix=${ROOTSYS} --libs) $(BOOST_LDFLAGS) $(DCAP_LDFLAGS)

ifeq (i686,$(findstring i686,$(CMTCONFIG)))
        CXXFLAGS+=-m32
        LIBS+=-m32
else
        CXXFLAGS+=-m64
        LIBS+=-m64
endif

CXXFLAGS+=-D__USE_XOPEN2K8 -std=c++11 -Wno-unused-local-typedefs

INSTALLAREA=$(SITEROOT)/AtlasCore/$(AtlasVersion)/InstallArea/$(CMTCONFIG)/include
ifneq ("$(wildcard $(INSTALLAREA))","")
CXXFLAGS+=-I$(INSTALLAREA)
endif

# DICT_* are used to create an dynamic library with all the classes
# used for the FTK simulation I/O
DICT_HDRS = TrigFTKSim/FTKRoad.h TrigFTKSim/FTKHit.h \
        TrigFTKSim/FTKRoadStream.h TrigFTKSim/FTKRawHit.h \
        TrigFTKSim/FTKTrack.h TrigFTKSim/FTKTrackStream.h \
        TrigFTKSim/FTKSS.h TrigFTKSim/MultiTruth.h \
        TrigFTKSim/FTKTruthTrack.h TrigFTKSim/FTKPattern.h \
        TrigFTKSim/FTKSimLinkDef.h
DICT_OBJS = tmp/FTKRoad.o tmp/FTKHit.o tmp/FTKTrack.o \
        tmp/FTKTruthTrack.o tmp/FTKRawHit.o tmp/FTKSplitEncoder.o \
        tmp/FTKRoadStream.o tmp/FTKRoadKDTree.o tmp/FTKTrackStream.o \
        tmp/FTKSS.o tmp/MultiTruth.o tmp/FTKPattern.o \
        tmp/TrigFTKSim_Dic.o tmp/FTKSetup.o

# FTKSIM_OBJ defines all the files used for an FTK static library
FTKSIM_OBJS = tmp/tsp/FTKTSPBank.o tmp/tsp/TSPMap.o tmp/tsp/TSPLevel.o \
        tmp/PatternBank.o tmp/RoadFinder.o \
        tmp/FTK_AMBank.o tmp/atlClustering.o tmp/FTKRoadFileOutput.o \
        tmp/FTKTrackInput.o tmp/FTKRoadFileInput.o \
        tmp/FTK_RawInput.o tmp/FTK_RegionalRawInput.o tmp/FTKDataInput.o \
        tmp/FTKRegionMap.o tmp/FTKTrackFileOutput.o \
        tmp/FTKSector711DB.o \
	tmp/ftkdefs.o tmp/FTKPMap.o tmp/FTKRegionMap.o tmp/FTKSectorMap.o \
        tmp/FTKSSMap.o tmp/ftk_dcap.o tmp/ftkdefs.o \
        tmp/FTKCacheLookup.o \
        tmp/FTKLogging.o tmp/FTKSteering.o tmp/FTKRootFile.o tmp/FTKMergeRoot.o \
        tmp/FTKPatternBySector.o tmp/FTKPatternOneSector.o \
        tmp/FTKPatternWithCoverage.o tmp/tsp/FTKAMSplit.o \
	tmp/FTK_CompressedAMBank.o tmp/FTK_AMsimulation_base.o \
	tmp/FTKConstantBank.o tmp/TrackFitter.o tmp/TrackFitter711.o tmp/FTK_SingleTrackInput.o 

SECWALK_OBJS = sectorwalk.o common_fcn.o \
                $(DICT_OBJS)

SECFOAM_OBJS = sectorfoam.o common_fcn.o \
                $(DICT_OBJS)

PV_OBJS = pattvolume.o common_fcn.o \
        $(DICT_OBJS)

CL_OBJS = convert_lookup.o common_fcn.o \
        $(DICT_OBJS)

RF_OBJS = road_finder.o common_fcn.o \
        $(DICT_OBJS)

RM_OBJS = road_merger.o common_fcn.o \
        $(DICT_OBJS)

HF_OBJS = hit_filter.o \
        $(DICT_OBJS)

TF_OBJS = track_fitter.o common_fcn.o \
        $(DICT_OBJS)

TF711_OBJS = track_fitter711.o common_fcn.o \
        $(DICT_OBJS)

TM_OBJS = track_merger.o common_fcn.o \
        $(DICT_OBJS)

QF_OBJS = quick_fit.o \
        $(DICT_OBJS)

PM_OBJS = patmerge.o \
        $(DICT_OBJS)

EFF_OBJS = efficiency.o \
        $(DICT_OBJS)

DATAFLOW_OBJS = dataflow.o \
        $(DICT_OBJS)

MSB_OBJS = makecompressedbank.o \
        $(DICT_OBJS)

PBI_OBJS = patternbankinfo.o \
        $(DICT_OBJS)

PAB_OBJS = partitionbalancing.o \
        $(DICT_OBJS)

CFO_OBJS = compare_fitter_output.o \
        $(DICT_OBJS)

PMR_OBJS = patmergeroot.o \
        $(DICT_OBJS)

PMT_OBJS = patmergetest.o \
        $(DICT_OBJS)

PR_OBJS = patreduce.o \
        $(DICT_OBJS)

P2H_OBJS = patt2hits.o common_fcn.o \
        $(DICT_OBJS)

FA2R_OBJS = ftkascii2root.o $(DICT_OBJS)

TSP_OBJS = common_fcn.o tmp/ftk_dcap.o tmp/FTKRegionMap.o tmp/FTKPMap.o tmp/FTKSSMap.o \
        tmp/tsp/TSPMap.o tmp/tsp/TSPLevel.o tmp/tsp/TSPROOTBankGenerator.o \
        $(DICT_OBJS)

DCSTAT_OBJS = ftk_DCBankStat.o	

AMBOPT_OBJS = ambankopt.o $(DICT_OBJS)

# SPLIT_OBJS = ftkamsplit.o $(DICT_OBJS)

FTKLIB_OBJS = $(DICT_OBJS)


##hit_filter 
###patt2hits 
all:  convert_lookup road_finder road_merger \
        track_fitter track_fitter711 \
        track_merger tsp_bank_generator \
        patmerge patmergeroot patmergetest quick_fit ftk_DCBankStat efficiency dataflow \
        libftk_classes.so libTrigFTKSim.a

extra:  pattvolume ftkascii2root sectorwalk sectorfoam \
        patreduce ambbankopt

-include *.d
-include tmp/*.d
-include tmp/tsp/*.d

patt2hits : $(P2H_OBJS) libTrigFTKSim.a
	$(CXX) -o $@ $(P2H_OBJS) $(LIBS) libTrigFTKSim.a
patt2hits.clean:
	rm -f $(P2H_OBJS) patt2hts patt2hits.o patt2hits.d common_fcn.d

pattvolume: $(RF_OBJS) libTrigFTKSim.a
	$(CXX) -o $@ $(RF_OBJS) $(LIBS) libTrigFTKSim.a
pattvolume.clean:
	rm -f $(RF_OBJS) pattvolume pattvolume.o pattvolume.d common_fcn.d

efficiency: $(EFF_OBJS) libTrigFTKSim.a
	$(CXX) -o $@ $(EFF_OBJS) $(LIBS) libTrigFTKSim.a
efficiency.clean:
	rm -f $(EFF_OBJS) efficiency efficiency.o efficiency.d common_fcn.d

dataflow: $(DATAFLOW_OBJS) libTrigFTKSim.a
	$(CXX) -o $@ $(DATAFLOW_OBJS) $(LIBS) libTrigFTKSim.a
dataflow.clean:
	rm -f $(DATAFLOW_OBJS) dataflow dataflow.o dataflow.d common_fcn.d

convert_lookup: $(CL_OBJS) libTrigFTKSim.a
	$(CXX) -o $@ $(CL_OBJS) $(LIBS) libTrigFTKSim.a
convert_lookup.clean:
	rm -f $(RF_OBJS) convert_lookup convert_lookup.o convert_lookup.d common_fcn.d

road_finder: $(RF_OBJS) libTrigFTKSim.a
	$(CXX) -o $@ $(RF_OBJS) $(LIBS) libTrigFTKSim.a
road_finder.clean:
	rm -f $(RF_OBJS) road_finder road_finder.o road_finder.d common_fcn.d

road_merger: $(RM_OBJS) libTrigFTKSim.a
	$(CXX) -o $@ $(RM_OBJS) $(LIBS) libTrigFTKSim.a
road_merger.clean:
	rm -f $(RM_OBJS) road_merger road_merger.o road_merger.d common_fcn.d

hit_filter: $(HF_OBJS) libTrigFTKSim.a
	$(CXX) -o $@ $(HF_OBJS) $(LIBS) libTrigFTKSim.a
hit_filter.clean:
	rm -f  $(HF_OBJS) hit_filter hit_filter.o hit_filter.d common_fcn.d

track_fitter: $(TF_OBJS) libTrigFTKSim.a
	$(CXX) -o $@ $(TF_OBJS) $(LIBS) libTrigFTKSim.a
track_fitter.clean:
	rm -f $(TF_OBJS) track_fitter track_fitter.o track_fitter.d common_fcn.d

track_fitter711: $(TF711_OBJS) libTrigFTKSim.a
	$(CXX) -o $@ $(TF711_OBJS) $(LIBS) libTrigFTKSim.a
track_fitter711.clean:
	rm -f $(TF711_OBJS) track_fitter711 track_fitter711.o track_fitter711.d common_fcn.d

track_merger: $(TM_OBJS) libTrigFTKSim.a
	$(CXX) -o $@ $(TM_OBJS) $(LIBS) libTrigFTKSim.a
track_merger.clean:
	rm -f $(TM_OBJS) track_merger track_merger.o track_merger.d common_fcn.d

quick_fit: $(QF_OBJS) libTrigFTKSim.a
	$(CXX) -o $@ $(QF_OBJS) $(LIBS) libTrigFTKSim.a
quick_fit.clean:
	rm -f $(QF_OBJS) quick_fit quick_fit.o quick_fit.d

patmerge: $(PM_OBJS) libTrigFTKSim.a
	$(CXX) -o $@ $(PM_OBJS) $(LIBS) libTrigFTKSim.a
patmerge.clean:
	rm -f $(PM_OBJS) patmerge patmerge.o patmerge.d

makecompressedbank: $(MSB_OBJS) libTrigFTKSim.a
	$(CXX) -o $@ $(MSB_OBJS) $(LIBS) libTrigFTKSim.a
makecompressedbank.clean:
	rm -f $(MSB_OBJS) makecompressedbank makecompressedbank.o makecompressedbank.d

patternbankinfo: $(PBI_OBJS) libTrigFTKSim.a
	$(CXX) -o $@ $(PBI_OBJS) $(LIBS) libTrigFTKSim.a
patternbankinfo.clean:
	rm -f $(PBI_OBJS) patternbankinfo patternbankinfo.o patternbankinfo.d

partitionbalancing: $(PAB_OBJS) libTrigFTKSim.a
	$(CXX) -o $@ $(PAB_OBJS) $(LIBS) libTrigFTKSim.a
partitionbalancing.clean:
	rm -f $(PAB_OBJS) partitionbalancing partitionbalancing.o partitionbalancing.d

compare_fitter_output: $(CFO_OBJS) libTrigFTKSim.a
	$(CXX) -o $@ $(CFO_OBJS) $(LIBS) libTrigFTKSim.a
compare_fitter_output.clean:
	rm -f $(CFO_OBJS) compare_fitter_output compare_fitter_output.o compare_fitter_output.d

patmergeroot: $(PMR_OBJS) libTrigFTKSim.a
	$(CXX) -o $@ $(PMR_OBJS) $(LIBS) libTrigFTKSim.a
patmergeroot.clean:
	rm -f $(PMR_OBJS) patmergeroot patmergeroot.o patmergeroot.d

patmergetest: $(PMT_OBJS) libTrigFTKSim.a
	$(CXX) -o $@ $(PMT_OBJS) $(LIBS) libTrigFTKSim.a
patmergetest.clean:
	rm -f $(PMR_OBJS) patmergetest patmergetest.o patmergetest.d

patreduce: $(PR_OBJS) libTrigFTKSim.a
	$(CXX) -o $@ $(PR_OBJS) $(LIBS) libTrigFTKSim.a
patreduce.clean:
	rm -f $(PR_OBJS) patreduce patreduce.o patreduce.d

tsp_bank_generator: tsp_bank_generator.o $(TSP_OBJS) libTrigFTKSim.a
	$(CXX) -o $@ tsp_bank_generator.o $(TSP_OBJS) $(LIBS) libTrigFTKSim.a
tsp_bank_generator.clean:
	rm -f $(TSP_OBJS) tsp_bank_generator.o tsp_bank_generator.d tsp_bank_generator

ftkascii2root: $(FA2R_OBJS) libTrigFTKSim.a
	$(CXX) -o $@ $(FA2R_OBJS) $(LIBS) libTrigFTKSim.a
ftkascii2root.clean:
	rm -f $(FA2R_OBJS) ftkascii2root ftkascii2root.o ftkascii2root.d

sectorwalk: $(SECWALK_OBJS) libTrigFTKSim.a
	$(CXX) -o $@ $(SECWALK_OBJS) $(LIBS) libTrigFTKSim.a -lMinuit
sectorwalk.clean:
	rm -f $(SECWALK_OBJS) sectorwalk sectorwalk.o sectorwalk.d common_fcn.d

sectorfoam: $(SECFOAM_OBJS) libTrigFTKSim.a
	$(CXX) -o $@ $(SECFOAM_OBJS) $(LIBS) libTrigFTKSim.a -lMinuit -lFoam
sectorfoam.clean:
	rm -f $(SECWALK_OBJS) sectorfoam sectorfoam.o sectorfoam.d common_fcn.d

ftk_DCBankStat: $(DCSTAT_OBJS) libTrigFTKSim.a
	$(CXX) -o $@ $(DCSTAT_OBJS) $(LIBS) libTrigFTKSim.a
ftk_DCBankStat.clean:
	rm -f $(DCSTAT_OBJS) ftk_DCBankStat

ambankopt: $(AMBOPT_OBJS) libTrigFTKSim.a
	$(CXX) -o $@ $(AMBOPT_OBJS) $(LIBS) libTrigFTKSim.a
ambankopt.clean :
	rm -f $(AMBOPT_OBJS) ambankopt

# ftkamsplit: $(SPLIT_OBJS) libTrigFTKSim.a
# 	$(CXX) -o $@ $(SPLIT_OBJS) $(LIBS) libTrigFTKSim.a
# ftkamsplit.clean :
# 	rm -f $(SPLIT_OBJS) ftkamsplit

# shortcut
classes: libftk_classes.so

libftk_classes.so: $(FTKLIB_OBJS)
	$(CXX) -o $@ -shared $(FTKLIB_OBJS) -fPIC $(LIBS)
	# TODO: update to ROOT6 with rootcling
	-rlibmap -f -o $(@:.so=.rootmap) -l $@ -c TrigFTKSim/FTKSimLinkDef.h

libftk_classes.so.clean:
	rm -f $(FTKLIB_OBJS) libftk_classes.so libftk_classes.rootmap TrigFTKSim_Dic_rdict.pcm

libTrigFTKSim.a: $(FTKSIM_OBJS)
	-rm -f $@
	ar cru $@ $(FTKSIM_OBJS)
	ranlib $@

libTrigFTKSim.a.clean:
	rm -f $(FTKSIM_OBJS) libTrigFTKSim.a

tmp/TrigFTKSim_Dic.C: $(DICT_HDRS)
	rootcint -f $@ -c -p $^
	@if [ -f tmp/TrigFTKSim_Dic_rdict.pcm ]; then \
		mv tmp/TrigFTKSim_Dic_rdict.pcm .; \
	fi

tmp/tsp/%.o : src/tsp/%.cxx
	$(CXX) -M -o $(@:.o=.d) $< $(CXXFLAGS) $(TSPCXXFLAGS) 
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(TSPCXXFLAGS)

tmp/%.o : src/%.cxx
	$(CXX) -M -o $(@:.o=.d) $< $(CXXFLAGS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

tmp/%.o : src/%.C
	$(CXX) -c -o $@ $< $(CXXFLAGS)

%.o: %.cc
	$(CXX) -M -o $(@:.o=.d) $< $(CXXFLAGS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

.PHONY : version
version:
	which $(CXX)
	$(CXX) --version

.PHONY : compact
compact:
	@rm -f $(DICT_OBJS) $(RF_OBJS) $(RM_OBJS) $(HF_OBJS) $(TF_OBJS) $(TF711_OBJS) $(TM_OBJS) $(QF_OBJS) $(PM_OBJS) $(FA2R_OBJS) $(TSP_OBJS) $(EFF_OBJS) $(DATAFLOW_OBJS) *.o *.d
	@strip convert_lookup road_finder road_merger hit_filter track_fitter track_fitter711 \
	track_merger quick_fit patmerge patmergeroot patmergetest

.PHONY : clean

clean : pattvolume.clean efficiency.clean dataflow.clean convert_lookup.clean road_finder.clean road_merger.clean hit_filter.clean \
        track_fitter.clean track_fitter711.clean \
        track_merger.clean tsp_bank_generator.clean \
        quick_fit.clean patmerge.clean patmergeroot.clean patmergetest.clean \
        sectorwalk.clean sectorfoam.clean \
        ftkascii2root.clean libftk_classes.so.clean libTrigFTKSim.a.clean \
        ambankopt.clean makecompressedbank.clean patternbankinfo.clean \
	partitionbalancing.clean compare_fitter_output.clean \
        # ftkamsplit.clean \
        ftk_DCBankStat.clean
	rm -f tmp/TrigFTKSim_Dic.C tmp/TrigFTKSim_Dic.h tmp/*.d tmp/tsp/*.d common_fcn.d common_fcn.o
