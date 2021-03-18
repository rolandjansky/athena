

function insertExamples() {
    var x = document.getElementById("exampleRunEvt");
    if(x==null) return;
    insertExamplesRunEvt();
    insertExamplesTime();
    insertExamplesDetectors();
    insertExamplesStreams();
    insertExamplesMagnets();
    insertExamplesDQ();
    insertExamplesTrigger();
    insertExamplesPartition();
    insertExamplesDatasets();
    insertExamplesBeamSpot();
    insertExamplesLAr();
    insertExamplesLumi();
    insertExamplesLHC();
    insertExamplesOther();
}

var newtext = 'blank';
function addColumn(newtext) {
  frm = document.forms["RunQueryInput"].q;
  frm.value = newtext;
}


function insertExamplesRunEvt() {
   var x = document.getElementById("exampleRunEvt");
   x.innerHTML = '\
<div class="Pad">\
  <table class="exampletable">\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r last 10 / show all / nodef &quot;);return false">f r last 10 / show all / nodef </td>\
        <td class="cmt">Select <font color="red">last 10 runs</font>; same in short: <font color="red">f r l 10 / sh all / nodef</font>.</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;find run 267073-267385 and ready / show all&quot;);return false">find run 267073-267385 and ready / show all</td>\
        <td class="cmt"><font color="red">Select</font> runs in given <font color="red">run number range</font> and ATLAS ready for data taking / show most relevant information for selected runs.</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r 267073-267385 and ready / sh r and ev&quot;);return false">f r 267073-267385 and ready / sh all</td>\
        <td class="cmt">Allowed <font color="red">abbreviations</font> &minus; same query as above.</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r 267076+ and ready&quot;);return false">f r 267076+ and ready </td>\
        <td class="cmt">Select all data runs for run 2.</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r 267073,267148,267152,267162,267167,267385 &quot;);return false">f r 267073,267148,267152,267162,267167,267385</td>\
        <td class="cmt">All runs of first week.</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r data15_13TeV.A &quot;);return false">f r data15_13TeV.A </td> \
        <td class="cmt">Select runs belonging to a given <font color="red">run period</font><br>(giving only "A" would use "data15_13TeV" as default)</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r 2015.A &quot;);return false">f r 2015.A </td>\
        <td class="cmt">Shows ready lumiblocks.</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r 2015.B-D&quot;);return false">f r 2015.B-D </td>\
        <td class="cmt">Select runs belonging to the given <font color="red">range of run periods</font></td>\
     </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r 2015.All&quot;);return false">f r 2015.All </td>\
        <td class="cmt">Select runs of 2015 <font color="red">(takes time and might time out)</font></td>\
     </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r data15_13TeV.All&quot;);return false">f r data15_13TeV.All </td>\
        <td class="cmt">Select runs of 2015 with 13TeV <font color="red">(takes time and might time out)</font></td>\
     </tr>\
  </table>\
</div>';
}


function insertExamplesTime() {
   var x = document.getElementById("exampleTime");
   x.innerHTML = '\
<div class="Pad">\
  <table class="exampletable">\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;find time last 24h / sh r and ev and t / utc&quot;);return false">find time last 24h / sh r and ev and t / utc</td>\
        <td class="cmt">Select runs that <font color="red">ended within the last 24h</font>; format examples: 5d, 1d = 24h, 5h, ... <br> extra option <font color="red">utc</font>, default is CEST (CERN local time)</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f t 3.6.2015-10.6.2015 / sh r and ev and t&quot;);return false">f t 3.6.2015-10.6.2015 /  sh r and ev and t</td>\
        <td class="cmt">Select runs with start/end dates within given <font color="red">time period</font>; format: DD.MM.YYYY. This example is for the first week of run 2. </td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f t 16.6.2015+ / sh t&quot;);return false">f 16.6.2015+ / sh t</td>\
        <td class="cmt">Select all runs including and <u>after</u> Jun 16, 2015.</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f t  last 48h and duration 30m+ / sh t and dur &quot;);return false">f t last 48h and duration 30m+ / sh t and dur </td>\
        <td class="cmt">Select runs that had a <font color="red">duration</font> of at least thirty  minutes; <br/>\
          replace "m" by "h" and "s" for hours and seconds, respectively;<br>note: only integer numbers allowed! </td>\
      </tr>\
  </table>\
</div>';
}


function insertExamplesDetectors() {
   var x = document.getElementById("exampleDetectors");
   x.innerHTML = '\
<div class="Pad">\
 <table class="exampletable">\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r 267073-267385 and det pix / sh r and ev and det&quot;);return false">f r 267073-267385  and det pix / sh r and ev and det</td>\
        <td class="cmt">Select runs with all Pixel in and show all <font color="red">participating detectors</font>.</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r  and det any sct / sh r and ev and det&quot;);return false">f r 267073-267385 and det any sct / sh r and ev and det</td>\
        <td class="cmt">Select runs with at least one SCT in and show all participating detectors.</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r 267073-267385 and det all / sh r and ev and det&quot;);return false">f r 267073-267385 and det all / sh r and ev and det</td>\
        <td class="cmt">Select runs with all detectors in; here <i>all</i> means the systems used during the 2008 combined data taking period, ie, not including for example CSC.</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;<b><i>Decode detector mask:</i></b> &quot;);return false"><b><i>Decode detector mask:</a></i></b> </td>\
        <td class="cmt"></td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;detmask 562937068519415&quot;);return false">detmask 562937068519415</td>\
        <td class="cmt">Interpret <font color="red">detector mask</font>.</td>\
      </tr>\
  </table>\
</div>';
}


function insertExamplesStreams() {
   var x = document.getElementById("exampleStreams");
   x.innerHTML = '\
<div class="Pad">\
  <table class="exampletable">\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r 267073-267385 / sh r and ev and st&quot;);return false">f r 267073-267385 / sh r and ev and st</td>\
        <td class="cmt">Show <font color="red">all produced streams and events per stream</font> for selected runs. (For first week of run 2 in this example).</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;</td><td class="&quot;);return false"></td><td class="cmt">\
          By placing the mouse <font color="red"> over</font> the number of events of a stream, one can display the <font color="red">stream overlaps</font> &minus; computed relative to the number of events in that same stream. </td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r 267073-267385 and st *Main* 100k+ / sh st phy*,cal*&quot;);return false">f r 267073-267385 and st *Main* 100k+ / sh st phy*,cal*</td>\
        <td class="cmt">Select runs where physics_IDCosmic stream has more than 10000 events and the RPC* stream is enabled, shows all physics and calibration streams. </td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r 267073-267385 and st *Datascouting* / sh st phy*&quot;);return false">f r 267073-267385 and st *Datascouting* / sh st phy*</td>\
        <td class="cmt">Select runs where a RPC stream but <font color="red">no physics_L1CaloEM </font> stream is written.</td>\
  </table>\
</div>';
}



function insertExamplesMagnets() {
   var x = document.getElementById("exampleMagnets");
   x.innerHTML = '\
<div class="Pad">\
  <table class="exampletable">\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r 267073-267385 and mag s and not mag t / sh r and ev and mag&quot;);return false">f r 267073-267385 and mag s and not mag t / sh r and ev and mag</td>\
        <td class="cmt">Select runs with <font color="red">solenoid on and toroid off</font>.</td>\
      </tr>\
  </table>\
</div>';
}





function insertExamplesDQ() {
   var x = document.getElementById("exampleDQ");
   x.innerHTML = '\
<div class="Pad">\
  <table class="exampletable">\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r 2015.A / sh dqsum&quot;);return false">f r 2015.A / sh dqsum</td>\
        <td class="cmt">Show <font color="red">data quality</font> summary: defects, plots and luminosity losses per system (restricted to LBs with ATLAS READY).</td>\
      </tr>\
      <tr>\
      <td class="pgm" onClick="addColumn(&quot;f r 2015.A and dqsumgrl PHYS_StandardGRL_All_Good_Tight / sh dqsum&quot;);return false">f r 2015.A and dqsumgrl PHYS_StandardGRL_All_Good_Tight / sh dqsum</td>\
      <td class="cmt">Show dq summary relative to <font color="red">GRL or virtual defect</font>, in this example using All_Good_Tight as GRL. If nothing is specified PHYS_StandardGRL_All_Good_25ns is used as default for dqsum. </td>\
      </tr>\
      <tr>\
      <td class="pgm" onClick="addColumn(&quot;f r 2015.A and dqsumgrl no / sh dqsum&quot;);return false">f r 2015.A and dqsumgrl no / sh dqsum</td>\
      <td class="cmt">Show dq summary using <font color="red">no GRL</font>, which means all intolerable defects are taken into account.</td>\
      </tr>\
      <tr>\
      <td class="pgm" onClick="addColumn(&quot;f r 2015.A / sh dqsum and cos&quot;);return false">f r 2015.A / sh dqsum and cos(mics)</td>\
      <td class="cmt">Show dq summary for <font color="red">cosmics</font> run, which means PHYS_Cosmics_AllGood is used as GRL, ATLAS not ready is ignored and losses are calculated using LB fractions.</td>\
      </tr>\
      <tr>\
      <td class="pgm" onClick="addColumn(&quot;f r 2015.A / sh dqsum and heavy&quot;);return false">f r 2015.A / sh dqsum and heavy(ions)</td>\
      <td class="cmt">Show dq summary for <font color="red">heavyions</font> run, which means PHYS_HeavyIonP_All_Good is used as GRL.</td>\
      </tr>\
      <tr>\
      <td class="pgm" onClick="addColumn(&quot;f r 2015.A and logictag DetStatusDEFECTLOGIC-0075 and defecttag DetStatusDEFECTS-repro20-01 / sh dqsum&quot;);return false">f r 2015.A and logictag DetStatusDEFECTLOGIC-0075 and defecttag DetStatusDEFECTS-repro20-01 / sh dqsum</td>\
      <td class="cmt">Show dq summary and specify <font color="red">logic and defect tag</font>, in this case DetStatusDEFECTLOGIC-0075 and  DetStatusDEFECTS-repro20-01, respectively. If nothing is specified HEAD is used as default.</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r 2015.A / sh dq&quot;);return false">f r 2015.A/ sh dq</td>\
        <td class="cmt">Show <font color="red">data quality</font> defects (restricted to LBs with ATLAS READY).</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r 2015.A / sh r and dq det&quot;);return false">f r 2015.A / sh dq det</td>\
        <td class="cmt">Show <font color="red">detector defects</font> in period A of current year.</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r 2015.A / sh dq TRIG_&quot;);return false">f r 2015.A / sh dq TRIG_</td>\
        <td class="cmt">Show defects <font color="red">matching pattern</font>. Use &quot;pattern$&quot; for exact match of pattern</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r 2015.A and dq !CP_TAU &quot;);return false">f r 2015.A and dq !CP_TAU </td>\
        <td class="cmt">Select all lumiblocks in B1 that have <font color="red">no CP_TAU defect</font>. The specified defect has to match exactly, no wild cards.</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r 2015.A / sh r and dq pix,sct DQMFOFL&quot;);return false">f r 2015.A / sh r and dq pix,sct DQMFOFL</td>\
        <td class="cmt">Same as above for <font color="red">"DQMFOFL" folder</font>; \
          choices are: "SHIFTOFL", "SHIFTONL", "DQCALCOFL", "DQMFOFL", "DQMFONL", "DQMFONLLB", "DCSOFL", "TISUMM", "LBSUMM" [default: "SHIFTOFL"].</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r 135195 / sh r and dq DQMFOFL#DetStatusDQMFOFL-express-pass1&quot;);return false">f r 135195 / sh r and dq DQMFOFL#DetStatusDQMFOFL-express-pass1</td>\
        <td class="cmt">Specification of a <font color="red">DB tag</font>\
          [default: "HEAD"].</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r 90270-90350 and dq em y+ and dq pixb y+ / sh dq pix,sct,em,til&quot;);return false">f r 90270-90350 and dq em y+ and dq pixb y+ / sh dq pix,sct,em,til</td>\
        <td class="cmt">Select runs where LAr-EM and Tile DQ flags are at least "Y" and Pixel-B is "G". </td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r 90270-91350 and dq lar g / sh dq lar&quot;);return false">f r 90270-91350 and dq lar g / sh dq lar</td>\
        <td class="cmt">Select runs where all LAr-EM are "green". </td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r 90270-91350 and dq any lar g / sh dq lar&quot;);return false">f r 90270-91350 and dq any lar g / sh dq lar</td>\
        <td class="cmt">Select runs where at least one LAr-EM are "green". </td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r 90270-90350 and dq any pix n.a. / sh dq pix&quot;);return false">f r 90270-90350 and dq any pix n.a. / sh dq pix</td>\
        <td class="cmt">Select runs where at least one Pix qualitity flag is unset. </td>\
      </tr>\
 <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r B / sh dq #DetStatus-v08-pro07&quot;);return false">f r B / sh dq #DetStatus-v08-pro07</td>\
        <td class="cmt">Show <font color="red">data quality </font> defects with COOL tag DetStatus-v08-pro07.</td>\
      </tr>\
  </table>\
</div>\
';
}


function insertExamplesProject() {
   var x = document.getElementById("exampleProject");
   x.innerHTML = '\
<div class="Pad">\
  <table class="exampletable">\
  </table>\
</div>\
';
}

function insertExamplesTrigger() {
   var x = document.getElementById("exampleTrigger");
   x.innerHTML = '\
<div class="Pad">\
  <table class="exampletable">\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r 90270-90350 and ev 100k+ / sh allev&quot;);return false">f r 90270-90350 and ev 100k+ / sh allev</td>\
        <td class="cmt">Show <font color="red">all event numbers, i.e., at EF, L2, L1 and SFO (inclusive)</font> levels.</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r last 10 / sh trigkeys and rel&quot;);return false">f r last 10 / sh trigkeys and rel</td>\
        <td class="cmt">Show <font color="red">all trigger keys and HLT release</font> for selected runs.</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r last 10 / sh trigrates L1_MBTS_*&quot;);return false">f r last 10 / sh trigrates L1_MBTS_*</td>\
        <td class="cmt">Show <font color="red">trigger rates</font> (max 50). Currently only Level 1.</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r 91890-92070 and smk 368,373 / sh trigkeys&quot;);return false">f r 91890-92070 and smk 368,373 / sh trigkeys</td>\
        <td class="cmt">Select runs with <font color="red">trigger "super-master" key 368 or 373</font>, show keys and <font color="red">release</font>.</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r 91890-92070 / sh tr L2_E*,L2_Cosmic* &quot;);return false">f r 91890-92070 / sh tr L2_E*,L2_Cosmic* </td>\
        <td class="cmt">Show <font color="red">show triggers matching patterns L2_E* or L2_Cosmic*</font> [not case sensitive].</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r 91890-92070 and tr EF_e5* &quot;);return false">f r 91890-92070 and tr EF_e5* </td>\
        <td class="cmt">Show <font color="red">runs with triggers matching pattern EF_e5*</font> [not case sensitive].</td>\
      </tr>\
  </table>\
</div>\
';
}


function insertExamplesPartition() {
   var x = document.getElementById("examplePartition");
   x.innerHTML = '\
<div class="Pad">\
  <table class="exampletable">\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f t 10.9.2008-13.9.2008 and part ATLAS / sh t and part&quot;);return false">f t 10.9.2008-13.9.2008 and part ATLAS / sh t and part</td>\
        <td class="cmt">Select runs with <font color="red">DAQ partition</font> "ATLAS".</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r l 10 / sh ready&quot;);return false">f r l 10 / sh ready</td>\
        <td class="cmt">Show (or similar: select) <font color="red">ReadyForPhysics flag</font> for runs.</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f t 10.9.2008-13.9.2008 and ptag data08_1beam* / sh t and ptag&quot;);return false">f t 10.9.2008-13.9.2008 and ptag data08_1beam* / sh t and ptag</td>\
        <td class="cmt">Select runs with single-beam <font color="red">project tag</font> (note that the project tag is denoted "filenameTag" in COOL).</td>\
      </tr>\
      <tr>\
        <td colspan="2">\
        <font size="-1">\
        <table style="border-collapse: collapse;">\
          <tr style="background-color: #a5a2a2;"><td style="padding: 0px; width: 120px;"><b>Project Tag</b></td><td style="padding: 0px;"><b>Definition</b></td></tr>\
          <tr><td style="padding: 0px;">dataYY_900GeV</td><td style="padding: 0px;">900 GeV collisions taken during the year 20YY</td></tr>\
          <tr><td style="padding: 0px;">dataYY_2TeV</td><td style="padding: 0px;">Collisions at 2.X TeV</td></tr>\
          <tr><td style="padding: 0px;">dataYY_7TeV</td><td style="padding: 0px;">Collisions at 7.X TeV</td></tr>\
          <tr><td style="padding: 0px;">dataYY_1beam</td><td style="padding: 0px;">Combined data taking with single beam (or 2 non-colliding beams)</td></tr>\
          <tr><td style="padding: 0px;">dataYY_cos</td><td style="padding: 0px;">Combined cosmic data taking</td></tr>\
        </table>\
        </font>\
        </td>\
      </tr>\
  </table>\
</div>\
';
}

function insertExamplesDatasets() {
   var x = document.getElementById("exampleDatasets");
   x.innerHTML = '\
<div class="Pad">\
  <table class="exampletable">\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f run last 10 / sh time and datasets&quot;);return false">f run last 10 / sh time and datasets</td>\
        <td class="cmt">Select last 10 runs and show <font color="red">all datasets produced in Tier-0.</font></td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r l 10 / sh t and da&quot;);return false">f r l 10 / sh t and da</td>\
        <td class="cmt">As above <font color="red">in short.</font></td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r l 10 / sh t and da caf&quot;);return false">f r l 10 / sh t and da caf</td>\
        <td class="cmt">Also checks if files are <font color="red">available on CAF pool (ATLCAL)</font> (slow query). <br>Note: <b>recent replications (< 24h) may not be included.</b></td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r l 10 / sh t and da *ntup*,*TAG*&quot;);return false">f r l 10 / sh t and da *ntup*,*TAG*</td>\
        <td class="cmt">As above but selecting <font color="red">dataset name patterns</font> - selection is case insensitive.</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r l 10 / sh t and da *ntup*,*TAG* caf&quot;);return false">f r l 10 / sh t and da *ntup*,*TAG* caf</td>\
        <td class="cmt">As above but including CAF pool availability information.</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f run last 10 / sh time and str&quot;);return false">f run last 10 / sh time and str</td>\
        <td class="cmt">Select last 10 runs and show streams: move mouse over stream info to obtain information on the <font color="red">data types produced in the reconstruction and merging steps of the Tier-0, and their CAF replication status</font>.</td>\
      </tr>\
  </table>\
</div>\
';
}

function insertExamplesBeamSpot() {
   var x = document.getElementById("exampleBeamSpot");
   x.innerHTML = '\
<div class="Pad">\
  <table class="exampletable">\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;find run last 10 / show bs &quot;);return false">find run last 10 / show bs </td>\
        <td class="cmt">Show <font color="red">offline beam spot parameters</font> for selected runs.</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;find run last 10 / show bs &lt;COOL-tag&gt;&quot;);return false">find run last 10 / show bs &lt;COOL-tag&gt;</td>\
        <td class="cmt">Same as above but assign which <font color="red">COOL tag</font> to use;<br>default tag: "IndetBeampos-ES1-UPD2".</td>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;find run last 10 / show bs online&quot;);return false">find run last 10 / show bs online </td>\
        <td class="cmt">Show <font color="red">online beam spot parameters</font> for selected runs.</td>\
      </tr>\
      </tr>\
  </table>\
</div>\
';
}


function insertExamplesLAr() {
   var x = document.getElementById("exampleLAr");
   x.innerHTML = '\
<div class="Pad">\
  <table class="exampletable">\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;find run last 10 / show larcond &quot;);return false">find run last 10 / show larcond </td>\
        <td class="cmt">Show <font color="red">LAr conditions</font> for selected runs.</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r l 10 and larc nsamples 7 and larc runtype 1 / sh larc &quot;);return false">f r l 10 and larc nsamples 7 and larc runtype 1 / sh larc </td>\
        <td class="cmt">As above, in short, and selecting on <font color="red">number of samples and run tupe</font>.</td>\
      </tr>\
  </table>\
</div>\
';
}

function insertExamplesLumi() {
   var x = document.getElementById("exampleLumi");
   x.innerHTML = '\
<div class="Pad">\
  <table class="exampletable">\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;find run 152166 / show lhc &quot;);return false">find run 152166 / show lhc </td>\
        <td class="cmt">Show LHC information and <font color="red">online luminosity</font> for selected runs.</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;find run 152166-152508 and lumi 20ub+ / show lhc &quot;);return false">find run 152166-152508 and lumi 20ub+ / show lhc </td>\
        <td class="cmt">Find runs within given range with <font color="red">integrated luminosity (online) larger than 20 inverse microbarns</font> (the &quot;+&quot; (&quot;&minus;&quot;) sign indicates &quot;more than&quot; (&quot;less than&quot;)).<br>Allowed units are: b, mb, ub, nb, pb, fb.<br>Note that only luminosity with <b>stable beams</b> is considered (hence: the requirement explicitly selects runs with stable beams).</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;find run 142193 / show lumi &quot;);return false">find run 142193 / show lumi </td>\
        <td class="cmt">Show <font color="red">offline luminosity</font> for selected runs.</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;find run 142193 / show lumi 0 OflLumi-Dec2009-001&quot;);return false">find run 142193 / show lumi 0 OflLumi-Dec2009-001</td>\
        <td class="cmt">Show offline luminosity for <font color="red">channel 0 and COOL tag OflLumi-Dec2009-001</font>;<br> default channel and tag: 0, OflLumi-Dec2009-001</td>\
      </tr>\
  </table>\
</div>\
';
}

function insertExamplesLHC() {
   var x = document.getElementById("exampleLHC");
   x.innerHTML = '\
<div class="Pad">\
  <table class="exampletable">\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;find run last 10 / show lhc &quot;);return false">find run last 10 / show lhc </td>\
        <td class="cmt">Show <font color="red">LHC information (stable beams, beam energy and intensities, online luminosity)</font> for selected runs.</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;find run last 10 / show lhc all &quot;);return false">find run last 10 / show lhc all</td>\
        <td class="cmt">Show also: <font color="red">beam mode and buch pattern (BCID mask)</font>.</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;find run last 20 and lhc stablebeams true / show lhc&quot;);return false">find run last 20 and lhc stablebeams true / show lhc</td>\
        <td class="cmt">Select runs with <font color="red">at least one LB with stable-beams</font> (can replace "true" by "t").</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r l 10 and lhc beamenergy 3400+ / sh t and lhc&quot;);return false">f r l 10 and lhc beamenergy 3400+ / sh lhc</td>\
        <td class="cmt">As above, in short, and selecting on <font color="red">the beam energy</font>.<br>Numbers can be replaced by ranges, eg: 3400+, 3700-, 3400-3700.</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r 152772-152876 and lhc fillnumber 1031-1033 / sh lhc&quot;);return false">f r 152772-152876 and lhc fillnumber 1031-1033 / sh lhc</td>\
        <td class="cmt">Select runs according to <font color="red">LHC fill number</font>.</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;find run last 10 / show bpm &quot;);return false">find run last 10 / show bpm </td>\
        <td class="cmt">Show <font color="red">beam positions measured by Beam Position Monitors (BPM)</font> for selected runs (the closest BPMs are located +-21m from the ATLAS IP).</td>\
      </tr>\
  </table>\
</div>\
';
}

function insertExamplesOther() {
   var x = document.getElementById("exampleOther");
   x.innerHTML = '\
<div class="Pad">\
  <table class="exampletable">\
      <tr>\
        <td><b><i>Summary pages:</i></b></td>\
        <td></td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r A-B / show summary &quot;);return false">f r A-B / show summary </td>\
        <td class="cmt">Provide <font color="red">summary tables and plots</font> for selected runs.</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r A-B / show dqsummary &quot;);return false">f r A-B / show dqsummary </td>\
        <td class="cmt">Provide <font color="red">summary data quality tables</font> for selected runs.</td>\
      </tr>\
      <tr>\
        <td><b><i>Extra options</i></b> (3rd parameter):</td>\
        <td></td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r G / show r and ev / nodef &quot;);return false">f r F / show r and ev / nodef </td>\
        <td class="cmt">Select runs with <font color="red">without default conditions</font>.</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r G / show r and ev / grl grl.xml:runlist &quot;);return false">f r G / show r and ev / grl grl.xml:runlist </td>\
        <td class="cmt">Produce <font color="red">GRL</font> with optionally give file and runlist name (they defaults to \'MyLBCollection.xml:MyLBCollection\').</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;f r G / show r and ev / root &quot;);return false">f r G / show r and ev / root </td>\
        <td class="cmt">Produce root files and overview histograms.</td>\
      </tr>\
        <td><b><i>Pileup:</i></b></td>\
        <td></td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;pileup 1.5e28 55 2 100000&quot;);return false">pileup 1.5e28 55 2 100000</td>\
        <td class="cmt">Compute <font color="red">pileup expectation</font>. The arguments are (in this order): <b>instantaneous luminosity</b>, <b>cross section</b> (unit: mb), <b>number of colliding bunches</b>, [<b>total number of triggered events in sample</b> (optional), default: 1]</td>\
      </tr>\
      <tr>\
        <td class="pgm" onClick="addColumn(&quot;pileup 1e10 17 55 2 1&quot;);return false">pileup 1e10 17 55 2 1</td>\
        <td class="cmt">Compute <font color="red">pileup expectation</font>. The arguments are (in this order): <b>beam intensity</b> (number of protons), <b>transverse beam size</b> (microns), <b>cross section</b> (mb), <b>number of colliding bunches</b>, [<b>total number of triggered events in sample</b> (optional), default: 1]</td>\
      </tr>\
 </table>\
</div>\
';
}


//insertExamples();

