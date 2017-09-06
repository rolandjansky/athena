/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page DetectorStatus_page DetectorStatus package

@section DetectorStatus_introductionDetectorStatus Introduction

The DetectorStatus package implements a service to manage detector status 
information, primarily to flag events where some or all subdetectors are 
unusable, or should be used with caution. The service maintains a DetStatusMap
object, which consists of pairs of string identifiers and associated DetStatus
objects. These contain an integer code (mapping to the traffic light colours:
1=red, 2=yellow, 3=green), plus floats specifying the fraction of dead modules
and the thrust of the dead modules (giving an idea of their concentration). The
DetStatus and DetStatusMap objects are defined elsewhere in the 
DetDescrConditons data objects package.

For more details on detector status handling, see the CoolDetStatus wiki page
at https://twiki.cern.ch/twiki/bin/view/Atlas/CoolDetStatus

The DetStatusSvc reads detector status information
from the conditions database, or from file-level meta-data.
A common interface is used to query the DetStatusMap object, which resides
in the transient detector store. To read detector status 
information from the conditions database, include the following in your
job options:
<pre>
include("DetectorStatus/DetStatusSvc_CondDB.py")
</pre>

@section DetectorStatus_flagsDetectorStatus Flags and conditions DB storage

Each DetStatus object has a corresponding string identifier, which is stored
in COOL as the name of a COOL channel in the detector status folder. The
currently defined list of COOL channels and string identifier names 
is as follows:
<pre>
101 PIXB   Pixel barrel
102 PIX0   Pixel B-layer
104 PIXEA  Pixel endcap A
105 PIXEC  Pixel endcap C
111 SCTB   SCT barrel
114 SCTEA  SCT endcap A
115 SCTEC  SCT endcap C
121 TRTB   TRT barrel 
124 TRTEA  TRT endcap A
125 TRTEC  TRT endcap C
130 IDGL   ID global
131 IDGB   ID global barrel (obsolete)
134 IDGEA  ID global endcap A (obsolete)
135 IDGEC  ID global endcap C (obsolete)
140 IDAL   ID alignment
141 IDAB   ID alignment barrel (obsolete)
144 IDAEA  ID alignment endcap A (obsolete)
145 IDAEC  ID alignment endcap C (obsolete)
150 IDBS   Beamspot
160 IDPF   ID performance
161 IDVX   ID vertexing quality
170 IDBCM  ID Beam conditions monitor

202 EMBA   LAR EM barrel +z
203 EMBC   LAR EM barrel -z
204 EMECA  LAR EM endcap A
205 EMECC  LAR EM endcap C
214 HECA   LAR HEC endcap A
215 HECC   LAR HEC endcap C
224 FCALA  LAR FCAL endcap A
225 FCALC  LAR FCAL endcap C
230 TIGB   Tile global 
232 TILBA  Tile barrel +z
233 TILBC  Tile barrel -z
234 TIEBA  Tile extemded barrel A
235 TIEBC  Tile extended barrel C
244 MBTSA  MBTS trigger scintillator side A
245 MBTSC  MBTS trigger scintillator side C
251 CALB   Calorimeter global barrel
254 CALEA  Calorimeter global endcap A
255 CALEC  Calorimeter global endcap C

302 MDTBA  MDT barrel +z
303 MDTBC  MDT barrel -z
304 MDTEA  MDT endcap A
305 MDTEC  MDT endcap C
312 RPCBA  RPC barrel A
313 RPCBC  RPC barrel C
324 TGCEA  TGC endcap A
325 TGCEC  TGC endcap C
334 CSCEA  CSC endcap A
335 CSCEC  CSC endcap C

350 LCD    LUCID global
353 LCDA   LUCID side A
354 LCDC   LUCID side C
360 ALFA   ALFA luminosity monitor
370 ZDC    ZDC luminosity monitor

401 L1CAL  L1 calorimeter trigger
402 L1MUB   L1 muon trigger barrel
403 L1MUE   L1 muon trigger endcaps
404 L1CTP  L1 central trigger

411 TRCAL  Calorimeter trigger
412 HLTEF  HLT-event filter (obselete)

420 TRDF   trigger dataflow
421 TRBJT  trigger b-jet slice
422 TRBPH  trigger B-physics slice
423 TRCOS  trigger cosmics slice
424 TRELE  trigger electron slice
425 TRGAM  trigger gamma slice
426 TRJET  trigger jet slice
427 TRMET  trigger muon slice
428 TRMBI  trigger min bias slice
429 TRMUO  trigger muon slice
430 TRTAU  trigger tau slice
431 TRIDT  trigger ID tracking

450 LUMI   Offline luminosity OK
451 LUMIONL Online luminosity OK
460 RUNCLT Run-control automatic flag
461 RCOPS  Run-control operator flag

480 ATLGL  Global data quality flag (DQ shifter)
481 ATLSOL Solenoid status
482 ATLTOR Toroid status

501 EIDB   electron ID barrel
502 EIDCR  electron ID crack region
503 EIDE   electron ID endcap 
505 PIDB   photon ID barrel
506 PIDCR  photon ID crack region
507 PIDE   photon ID endcap
508 EIDF   electron ID forward region
509 EIDSOFT low PT electronID
511 MIDB   muon ID barrel
514 MIDEA  muon ID endcap A
515 MIDEC  muon ID endcap C
510 MSTACO  muon ID Staco algorithm
511 MMUIDCB muon ID MuID combined algorithm
512 MMUIDVX muon ID MuID VX algorithm
513 MMUGIRL muon ID MuGirl algorithm
514 MMUBOY  muon ID MuonBoy algorithm
515 MMUIDSA muon ID MuID standalone algorithm
516 MMUTAG  muon ID MuTag algorithm
517 MMTIMO  muon ID MuTagIMO algorithm
518 MCMUTAG muon ID CaloMuonTag algorithm
519 MCALLHR muon ID CaloLHR algorithm
521 JETB   jet barrel
524 JETEA  jet endcap A
525 JETEC  jet endcap C
526 JETFA  jet forward side A
527 JETFC  jet forward side C
530 MET    missing ET 
531 METCALO missing ET calorimeters only
532 METMUON missing ET with muons
541 BTGLIFE   b-tagging lifetime-based
544 BTGSOFTE  b-tagging soft electron
545 BTGSOFTM  b-tagging soft muon
551 TAUB   tau ID barrel
552 TAUEA  tau ID crack region
553 TAUEC  tau ID endcap 
</pre>
For each COOL channel, the COOL payload consists of three items: the integer
'Code' which gives the status, the float 'deadFrac' giving the fraction of dead
modules, and the float 'Thrust' giving the thrust value of the dead modules.
The bottom two bits of the Code variable define the colour (unknown=0, red=1,
yellow=2, green=3), and the other bits are free for the user (e.g. to 
indicate more details of the type of failure).

Detector status information is stored in the COOL conditions database
in the /GLOBAL/DETSTATUS/LBSUMM folder, indexed by luminosity blocks.
The LBSUMM folder in the OFLP200 database contains a default set
of entries valid for all runs, given GREEN status for all detectors with 
zero dead fraction, tagged with the COOL tag DetStatusLBSumm_nominal. 
Entering the status for a particular run will then
override this default information. For testing purposes, it is possible to
enter information using a different tag, for example the predefined tag 
DetStatusLBSumm_undefined has all status flags set to undefined for all runs,
and can be selected by using the job option line:
<pre>
include("DetectorStatus/DetStatusSvc_CondDB.py")
from IOVDbSvc.CondDB import conddb
conddb.addOverride('/GLOBAL/DETSTATUS/LBSUMM','DetStatusLBSumm_BadEMB')
</pre>

@section DetectorStatus_queryDetectorStatus Querying the Detector Status in Athena algorithms

The DetStatusSvc has various methods which provide access to the DetStatus
object asociated with a particular identifier string. You can retrieve a 
pointer to the DetStatus for a given identifier, or retrieve iterators to 
let you loop over all the status objects.  The following code shows how
to do this:
<pre>
  // get DetStatusSvc interface
  const IDetStatusSvc* p_detstatussvc;
  if (StatusCode::SUCCESS!=service("DetStatusSvc",p_detstatussvc)) {
    m_log << MSG::ERROR << "Cannot get DetStatusSvc" << endmsg;
    return StatusCode::FAILURE;
  }

  // print all status information to MsgSvc
  if (par_print) p_detstatussvc->print();

  // test a particular one - SCTB
  const DetStatus* ptr=p_detstatussvc->findStatus("SCTB");
  if (ptr!=0) {
    m_log << MSG::INFO << "SCTB status found to be " << ptr->colour() << endmsg;
  } else {
    m_log << MSG::ERROR << "Could not find status of SCTB" << endmsg;
  }
  // loop over all status words, print those which are bad
  DetStatusMap::const_iterator begin,end;
  p_detstatussvc->getIter(begin,end);
  for (DetStatusMap::const_iterator itr=begin;itr!=end;++itr) {
    if (itr->second.code()<3) m_log << MSG::WARNING << "Status of " <<
      itr->first << " is bad: fullcode " << itr->second.fullCode() << endmsg;
  }
</pre>

The DetStatusSvc also implements a veto functionality, allowing you to specify
status requirements on events to be analysed. The joboption properties 
StatusNames and StatusReqs are used to specify pairs of names and minimum
status levels (red=1, yellow=2, green=3). For example:
<pre>
DetStatusSvc.StatusNames=['SCTB','SCTECA']
DetStatusSvc.StatusReqs=[2,3]
</pre>
specifies that the SCTB status must be at least 2 (yellow) and the SCTECA 
(endcap A)
status must be at least 3 (green). There must be an equal number of entries
in the two lists. Once this is specified, the bool vetoed() method will be true
if the event should be vetoed according to these criteria. Detector status
identifiers whose first letters match up to the length of the matching 
string will be considered to match, so e.g. the string SCT will match
SCTB, SCTECA and SCTECB. This is an easy way to set status requirements
on all parts of a detector.

An alternative way to specify detector status is with a string of the form:
<pre>
DetStatusSvc.StringReqs='SCTB 2 SCTEA 3'
</pre>
This is of the same form as detector status requirements specifications in 
the LumiCalc.py tool.

@section DetectorStatus_algDetectorStatus The DetStatusAlg algorithm

The DetStatusAlg algorithm provides a way to trigger use of the services, and
also serves as an example for accessing detector status information in user
code. It has the following joboption properties:
 - Print: call DetStatusSvc::print() to print the current status objects
 - Veto: call DetStatusSvc::vetoed() and count how many events are vetoed.
The veto functionality also sets the DetStatusAlg filterPassed property to
false, allowing it to be used in algorithm sequences.

@section DetectorStatus_detStatustools Command-line tools

Two command-line tools are provided to interact with the detector status 
information: detStatus_query.py and detStatus_set.py. They can be used to
query or set the detector status for one or all of the status flags, and
print their options if given without arguments. For more
details, see the CoolDetStatus wiki page.

@section DetectorStatus_jobOptionsDetectorStatus Job-option files

The package has two main job-options, that setup the DetStatusSvc to 
enable detector status to be read
from the conditions database, and write it to file-level meta-data.

@subsection jobOptions1DetectorStatus DetStatusSvc_CondDB.py

@include DetStatusSvc_CondDB.py

@subsection joboptions2DetectorStatus DetStatusSvc_ToFileMetaData.py

@include DetStatusSvc_ToFileMetaData.py





*/
