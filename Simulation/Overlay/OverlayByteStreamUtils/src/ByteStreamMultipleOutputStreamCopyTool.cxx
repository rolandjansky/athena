/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file ByteStreamMultipleOutputStreamCopyTool.cxx
 *  @brief This file contains the implementation for the ByteStreamMultipleOutputStreamCopyTool class.
 *  @author Andy Haas <ahaas@slac.stanford.edu>
 **/

#include "ByteStreamMultipleOutputStreamCopyTool.h"

// Gaudi
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"

#define IsBitSet(val, bit) ((val) & (1 << (bit)))

#include <vector>
#include <map>
#include "TRandom3.h"

TRandom3 myrand;

class lbninfo{
public:
  lbninfo(): currentevent(0),nevt(0),magic(0),noalgps(-1),j40ps(-1),written_j40(0),written_no_j40(0) {}

  std::map<int,int> wantedmap;//map of nevent wanted by each stream
  std::map<int, std::vector<int> > streammap;//map of actual events to be written to each stream
  int currentevent, nevt;//keeps track of the number of events we've seen on this lbn, and the number there should be in total
  int magic;//to make sure we're real...
  int noalgps;//the HLT prescale of noalg trigger
  int j40ps;//the HLT prescale of j40 trigger
  std::vector<int> trigmap;//whether the j40 trigger fired for each of the nevt events
  std::vector<int> trigevt;//event number for each of the nevt events, as taken from the trigfile, for cross-checking
  int written_j40, written_no_j40;

  //makes the map of the event sequence to be written for each stream, based on the number of events wanted by each stream
  void makestreammap(){
    for (std::map<int,int>::const_iterator s=wantedmap.begin(); s!=wantedmap.end(); ++s){
      int stream=s->first, wanted=s->second;
      for (int e=0; e<wanted; ++e){
	int chosen_evt = myrand.Integer(nevt);//chose a random event in the lbn
	if (noalgps>0){
	  //check if the chosen event is a j40
	  while (trigmap[chosen_evt]==1){
	    //skip it, with probability j40ps/noalgps
	    assert(noalgps>=j40ps);
	    double psratio = (double)j40ps/(double)noalgps;
	    if (myrand.Uniform(1.0)>psratio){
	      //skip it, choose another random event
	      chosen_evt = myrand.Integer(nevt);
	    }
	    else {
	      //we'll keep it
	      break;
	    }
	  }
	}//using trigmap
        streammap[chosen_evt].push_back(stream);
      }
    }
  }

  //makes a string for printout of the streammap
  std::string streameventstring(){
    std::ostringstream out;
    for (std::map<int, std::vector<int> >::const_iterator s=streammap.begin(); s!=streammap.end(); ++s){
      out<<"\nevent "<<s->first<<" goes to stream(s)";
      for (unsigned int e=0; e<s->second.size(); ++e){
        out<<" "<<s->second[e];
      }
    }
    return out.str();
  }

};

std::map<int, std::map<int, lbninfo> > runlbnmap;

/// Constructor
ByteStreamMultipleOutputStreamCopyTool::ByteStreamMultipleOutputStreamCopyTool(
                                                                               const std::string& type,
                                                                               const std::string& name,
                                                                               const IInterface* parent)
  : AthAlgTool(type, name, parent),
    m_inputSvc("ByteStreamEventStorageInputSvc", name),
    m_uselbnmap(false)
{
  // Declare IAthenaOutputStreamTool interface
  declareInterface<IAthenaOutputStreamTool>(this);

  declareProperty("lbn_map_file",m_lbn_map_file="lbn_anal_map.txt");
  declareProperty("skipevents",m_skipevents=0);
  declareProperty("trigfile",m_trigfile="");
  declareProperty("NoutputSvc",m_NoutputSvc=50);
  for (int i=0; i<m_NoutputSvc; ++i){
    char buf[100];
    sprintf(buf,"ByteStreamEventStorageOutputSvc%d",i);
    //std::cout<<"Making "<<buf<<std::endl;
    m_outputSvc.push_back(ServiceHandle<ByteStreamOutputSvc>(buf,name));

    //sprintf(buf,"ByteStreamOutputSvc%d",i);
    //declareProperty(buf, m_outputSvc[i], "Handle to the ByteStreamOutputSvc");
  }

  declareProperty("ByteStreamOutputSvc0", m_outputSvc[0], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc1", m_outputSvc[1], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc2", m_outputSvc[2], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc3", m_outputSvc[3], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc4", m_outputSvc[4], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc5", m_outputSvc[5], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc6", m_outputSvc[6], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc7", m_outputSvc[7], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc8", m_outputSvc[8], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc9", m_outputSvc[9], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc10", m_outputSvc[10], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc11", m_outputSvc[11], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc12", m_outputSvc[12], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc13", m_outputSvc[13], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc14", m_outputSvc[14], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc15", m_outputSvc[15], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc16", m_outputSvc[16], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc17", m_outputSvc[17], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc18", m_outputSvc[18], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc19", m_outputSvc[19], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc20", m_outputSvc[20], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc21", m_outputSvc[21], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc22", m_outputSvc[22], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc23", m_outputSvc[23], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc24", m_outputSvc[24], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc25", m_outputSvc[25], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc26", m_outputSvc[26], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc27", m_outputSvc[27], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc28", m_outputSvc[28], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc29", m_outputSvc[29], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc30", m_outputSvc[30], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc31", m_outputSvc[31], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc32", m_outputSvc[32], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc33", m_outputSvc[33], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc34", m_outputSvc[34], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc35", m_outputSvc[35], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc36", m_outputSvc[36], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc37", m_outputSvc[37], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc38", m_outputSvc[38], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc39", m_outputSvc[39], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc40", m_outputSvc[40], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc41", m_outputSvc[41], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc42", m_outputSvc[42], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc43", m_outputSvc[43], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc44", m_outputSvc[44], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc45", m_outputSvc[45], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc46", m_outputSvc[46], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc47", m_outputSvc[47], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc48", m_outputSvc[48], "Handle to the ByteStreamOutputSvc");
  declareProperty("ByteStreamOutputSvc49", m_outputSvc[49], "Handle to the ByteStreamOutputSvc");

  declareProperty("ByteStreamInputSvc",  m_inputSvc, "Handle to the ByteStreamInputSvc");
}
//__________________________________________________________________________
ByteStreamMultipleOutputStreamCopyTool::~ByteStreamMultipleOutputStreamCopyTool() {
}
//__________________________________________________________________________
StatusCode ByteStreamMultipleOutputStreamCopyTool::initialize() {
  ATH_MSG_INFO( "Initializing " << name() << " - package version " << PACKAGE_VERSION );

  CHECK( ::AlgTool::initialize() );

  // retrieve services
  CHECK( m_inputSvc.retrieve() );

  for (int i=0; i<m_NoutputSvc; ++i){
    CHECK( m_outputSvc[i].retrieve() );
  }

  if (m_lbn_map_file!=std::string("") && m_lbn_map_file!=std::string("random") && m_lbn_map_file!=std::string("serial") ) m_uselbnmap=true;
  ATH_MSG_INFO( "lbn_map_file is "<<m_lbn_map_file<<" and uselbnmap is "<<m_uselbnmap );

  if (m_uselbnmap==true) initlbnmap();

  return(StatusCode::SUCCESS);
}

void ByteStreamMultipleOutputStreamCopyTool::initlbnmap(){

  //read the runlbn map
  std::string file = m_lbn_map_file;
  ATH_MSG_INFO( "Going to open "<<file );
  FILE *fp = fopen(file.c_str(),"r");
  if (!fp) {
    ATH_MSG_ERROR( "Could not open "<<file<<"!" );
    throw;
  }
  char *line=new char[500];
  int stream,run,lbn,nevt,nwanted;
  float intlumi,intlumifrac;
  while (fgets(line,480,fp)) {
    if (line[0]=='-') continue;
    int s=sscanf(&line[0],"stream %d, run %d, lbn %d, has %d events and %f/ub, %f intlumi of run, and %d wanted",&stream,&run,&lbn,&nevt,&intlumi,&intlumifrac,&nwanted);
    if (s>0) {
      ATH_MSG_DEBUG(run<<" "<<lbn<<" has "<<nevt<<" events and we want "<<nwanted<<" of them for stream "<<stream);
      runlbnmap[run][lbn].magic=9999;
      runlbnmap[run][lbn].nevt=nevt;
      for (int w=0; w<m_NoutputSvc; ++w){runlbnmap[run][lbn].wantedmap[w]=nwanted;}
    }
    else{
      ATH_MSG_ERROR("Line in "<<file<<" not understood!");
      ATH_MSG_ERROR(line);
    }
  }
  fclose(fp);
  delete [] line;

  if (m_trigfile!=""){
    //read the trigger info file
    std::string file = m_trigfile;
    ATH_MSG_INFO( "Going to open "<<file );
    FILE *fp = fopen(file.c_str(),"r");
    if (!fp) {
      ATH_MSG_ERROR( "Could not open "<<file<<"!" );
      throw;
    }
    char *line=new char[500];
    int run,evt,time,lbn,noalg,j40,psnoalg,psj40;
    int evtinfile=0;
    while (fgets(line,480,fp)) {
      if (line[0]=='-') continue;
      int s=sscanf(&line[0],"run_nbr=%d, evt_nbr=%d, time_stamp=%d, lbk_nbr=%d, noalg=%d, j40=%d, noalgps=%d, j40ps=%d",&run,&evt,&time,&lbn,&noalg,&j40,&psnoalg,&psj40);
      if (s>0) {
	ATH_MSG_INFO(evtinfile<<": "<<run<<" "<<lbn<<" "<<evt<<": passed noalg j40 "<<noalg<<" "<<j40<<", prescale noalg j40 "<<psnoalg<<" "<<psj40);
	
	if (runlbnmap[run][lbn].noalgps!=-1){
	  assert(runlbnmap[run][lbn].noalgps == psnoalg);//it should be the same for every event in the lbn
	}
	else runlbnmap[run][lbn].noalgps=psnoalg;
	
	if (runlbnmap[run][lbn].j40ps!=-1){
	  assert(runlbnmap[run][lbn].j40ps == psj40);//it should be the same for every event in the lbn
	}
	else runlbnmap[run][lbn].j40ps=psj40;

	runlbnmap[run][lbn].trigmap.push_back(j40);//whether j40 fired for this event index in the file
	runlbnmap[run][lbn].trigevt.push_back(evt);//event number for this event index in the file, for cross-checking
	++evtinfile;
      }
      else{
	ATH_MSG_ERROR("Line in "<<file<<" not understood!");
	ATH_MSG_ERROR(line);
      }
    }
    fclose(fp);
    delete [] line;
  }

  myrand.SetSeed();
  ATH_MSG_INFO("Seed is "<<myrand.GetSeed());
  
  //go through the runlbn map and figure out which events to put to which stream(s)
  for (auto r = runlbnmap.begin(); r!=runlbnmap.end(); r++ ){
    for (auto l = r->second.begin(); l!=r->second.end(); l++ ){
      l->second.makestreammap();
      ATH_MSG_INFO(r->first<<" "<<l->first<<" wants: "<<l->second.streameventstring());
    }
  }
  
}

//__________________________________________________________________________
StatusCode ByteStreamMultipleOutputStreamCopyTool::finalize() {

  for (auto r = runlbnmap.begin(); r!=runlbnmap.end(); r++ ){
    for (auto l = r->second.begin(); l!=r->second.end(); l++ ){
      
      if (l->second.magic!=9999){
        ATH_MSG_WARNING("Run "<<r->first<<" saw events but wasn't in the map");
        continue;
      }

      ATH_MSG_INFO(r->first<<" "<<l->first<<" wrote out "<<l->second.written_j40<<" j40 and "<<l->second.written_no_j40<<" non-j40 events");

      if (l->second.currentevent!=0){
        //check that we got all the events we were promised
        if (l->second.currentevent!=l->second.nevt){
          ATH_MSG_WARNING(r->first<<" "<<l->first<<" saw "<<l->second.currentevent<<" and we expected "<<l->second.nevt<<" events!");
        }
        else {
          ATH_MSG_INFO(r->first<<" "<<l->first<<" saw all "<<l->second.nevt<<" events expected.");
        }
      }
    }
  }


  for (int i=0; i<m_NoutputSvc; ++i){
    CHECK( m_outputSvc[i].release() );
  }
  return(::AlgTool::finalize());
}
//__________________________________________________________________________
StatusCode ByteStreamMultipleOutputStreamCopyTool::connectServices(const std::string& /*dataStore*/,
                                                                   const std::string& /*cnvSvc*/,
                                                                   bool /*extendProvenenceRecord*/) {
  /// Do nothing for connectServices
  return StatusCode::SUCCESS;
}
//__________________________________________________________________________
StatusCode ByteStreamMultipleOutputStreamCopyTool::connectOutput(const std::string& /*outputName*/) {
  /// Do nothing for connectOutput
  return StatusCode::SUCCESS;
}
//__________________________________________________________________________
StatusCode ByteStreamMultipleOutputStreamCopyTool::commitOutput(bool/* doCommit*/) {
  ATH_MSG_DEBUG( "In commitOutput" );
  const RawEvent* re_c = m_inputSvc->currentEvent() ;
  if(!re_c){
    ATH_MSG_ERROR( " failed to get the current event from ByteStreamInputSvc  " );
    return StatusCode::FAILURE ;
  }

  int run = re_c->run_no();
  ATH_MSG_DEBUG("run is "<<run);
  uint64_t eventNumber;
  if (re_c->version() < 0x03010000) {    eventNumber=re_c->lvl1_id();  } 
  else {    eventNumber=re_c->global_id();  }
  ATH_MSG_DEBUG("event is "<<eventNumber);
  int lbn = re_c->lumi_block();
  ATH_MSG_DEBUG("lbn is "<<lbn);
  int bcid = re_c->bc_id();
  ATH_MSG_DEBUG("bcid is "<<bcid);
  int nL1word = re_c->nlvl1_trigger_info();
  ATH_MSG_DEBUG("nL1word is "<<nL1word);
  const OFFLINE_FRAGMENTS_NAMESPACE::DataType* L1word; re_c->lvl1_trigger_info(L1word);
  int bit=240;//2*8 gets us to the Trigger AfterVeto (TAV) bank
  ATH_MSG_DEBUG("bit "<<bit<<" fired? "<< IsBitSet(L1word[2*8+bit/32],bit%32) );

  //Skip the first N events, if asked to do so...
  if (m_skipevents>0){
    ATH_MSG_INFO("Skipping this event because skipevents="<<m_skipevents<<".");
    --m_skipevents;
    return StatusCode::SUCCESS ;
  }

  if (m_uselbnmap==true){

    //See if we want this event in any stream...
    lbninfo &l = runlbnmap[run][lbn];
    if (l.magic!=9999){
      ATH_MSG_WARNING( "Event in lbn "<<lbn<<" not in map... skipping." );
      return StatusCode::SUCCESS ;
    }

    if (l.noalgps>0){
      //using trigmap info... check the event numbers line up
      if (l.trigevt[l.currentevent] != (int)eventNumber){
	ATH_MSG_ERROR("Trigmap was for event "<<l.trigevt[l.currentevent]<<" but we are on event "<<eventNumber);
      }
      else{
	ATH_MSG_INFO("Trigmap matches for event "<<l.trigevt[l.currentevent]<<" and we are on event "<<eventNumber);
      }
    }

    if ((l.currentevent>=l.nevt || ((unsigned int)l.currentevent)>l.trigmap.size()) && l.noalgps>0) {
      ATH_MSG_WARNING("Only expecting "<<l.nevt<<" events in "<<run<<" "<<lbn<<" with trigmap size "<<l.trigmap.size());
    }

    std::vector<int> streams= l.streammap[l.currentevent];//so at this point the first event is at index 0, last at nevt-1
    if (l.noalgps<0){ATH_MSG_INFO(lbn<<" "<<l.currentevent<<" goes to "<<streams.size()<<" streams");}
    else{ATH_MSG_INFO(lbn<<" "<<l.currentevent<<" goes to "<<streams.size()<<" streams and j40 is "<<l.trigmap[l.currentevent]);}

    RawEvent* re =  const_cast<RawEvent*>(re_c);
    for (unsigned int s=0; s<streams.size(); ++s){
      if (streams[s]>=m_NoutputSvc) ATH_MSG_ERROR("Stream "<<streams[s]<< "is more than we have ("<<m_NoutputSvc<<")");
      else{
        if( !m_outputSvc[streams[s]]->putEvent(re) ) {
          ATH_MSG_ERROR( " failed to write event to ByteStreamOutputSvc"<<streams[s]);
          return StatusCode::FAILURE ;
        }
        if (l.noalgps>0){ if (l.trigmap[l.currentevent]) l.written_j40++; else l.written_no_j40++; }
        ATH_MSG_INFO( " wrote event to ByteStreamOutputSvc"<<streams[s] );
      }
    }
    l.currentevent++;

  }//m_usebnlmap
  else{
    RawEvent* re =  const_cast<RawEvent*>(re_c);
    static int eventstream=-1,ntotevents=0;
    if (m_lbn_map_file==std::string("random")){
      eventstream = rand()%m_NoutputSvc;
      if (eventstream<0 || eventstream>=m_NoutputSvc){
        ATH_MSG_ERROR("Only have "<<m_NoutputSvc<<" streams but eventstream is "<<eventstream<<"!");
        return StatusCode::FAILURE ;
      }
    }
    else if (m_lbn_map_file==std::string("serial")){
      if (ntotevents%100==0){ ++eventstream; }
      if (eventstream<0 || eventstream>=m_NoutputSvc){
        ATH_MSG_WARNING("Skipping event because only have "<<m_NoutputSvc<<" streams (of 100 events)");
        return StatusCode::SUCCESS ;
      }
    }
    else {
      ATH_MSG_ERROR( " I don't know what lbn_map_file="<<m_lbn_map_file<<" means!");
      return StatusCode::FAILURE ;
    }

    if( !m_outputSvc[eventstream]->putEvent(re) ) {
      ATH_MSG_ERROR( " failed to write event to ByteStreamOutputSvc"<<eventstream);
      return StatusCode::FAILURE ;
    }
    ATH_MSG_INFO( " wrote event "<<ntotevents<<" to ByteStreamOutputSvc"<<eventstream);
    ++ntotevents;
  }
  return StatusCode::SUCCESS ;
}

//__________________________________________________________________________
StatusCode ByteStreamMultipleOutputStreamCopyTool::finalizeOutput()
{
  return  StatusCode::SUCCESS ;
}

//__________________________________________________________________________
StatusCode ByteStreamMultipleOutputStreamCopyTool::streamObjects(const TypeKeyPairs&  typeKeys, const std::string& /*outputName*/ ) {
  if ( typeKeys.size()!=0){
    MsgStream log(msgSvc(), name());
    ATH_MSG_WARNING( " Streaming objects is not supported.  The whole input event is written out" );
  }

  return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode ByteStreamMultipleOutputStreamCopyTool::streamObjects(const DataObjectVec& dataObjects, const std::string& /*outputName*/ ) {
  if ( dataObjects.size()!=0){
    ATH_MSG_WARNING( " Streaming objects is not supported.  The whole input event is written out" );
  }

  return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode ByteStreamMultipleOutputStreamCopyTool::fillObjectRefs(const DataObjectVec& dataObjects) {
  if ( dataObjects.size()!=0){
    ATH_MSG_WARNING( " fillObjectRefs is not supported.  The whole input event is written out" );
  }

  return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode ByteStreamMultipleOutputStreamCopyTool::getInputItemList(SG::IFolder* /*m_p2BWrittenFromTool*/) {
  return(StatusCode::SUCCESS);
}
