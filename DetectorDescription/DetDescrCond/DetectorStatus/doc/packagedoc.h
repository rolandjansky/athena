/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page DetectorStatus_page DetectorStatus package

@section DetectorStatus_introductionDetectorStatus Introduction

*** THIS PACKAGE IS COMPLETELY DEPRECATED ***

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
override this default information. 


*/
