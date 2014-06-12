<!--
var FMT = "bec side ft sl ch 0 Proposed Flag # onlid"
//var FMT = "|OFF_ID|ONL_ID|TT_COOL_ID|SUBCALO|FT|SLOT|CHAN|REG|SAM|ETA|PHI|ETAF|PHIF|"

function putHelper() {
    var help = "<br /><hr />"
           + "<ul>"
           + "<li>More information <a href='https://twiki.cern.ch/twiki/bin/view/Atlas/LArIDTranslator'>here</a></li>"
           + "<li>Use the fields to make your selection and press the Submit button</li>"
           + "<li>Set ranges using , (e.g. for CH: 15,19 means 15<=CH<=19)</li>"
           + "<li>Alternatively, fill by hand the top input bar with a sql selection (use =,and,or,like,between,in) and press Enter</li>"
           + "<li>Double-click on a channel to get the standard format of it printed at the bottom:<br />"+FMT+"</li>"
           + "<li>DB file generated with tags ATLAS-GEO-16-00-00, COMCOND-ES1PST-004-01 (UPD1) and COMCOND-BLKPST-004-01 (UPD4) for run 0x7fffffff, i.e. IOV to infinity</li>"
           + "<li>Double-click on the title <LArID translator browser> at the top to perform an extraction of webdisplay info</li>"
           + "<li>Code available on svn <a href='https://svnweb.cern.ch/trac/atlasusr/browser/mplamond/projects/LArIdtranslator/trunk'>here</a></li>"
           + "<li>Contact <a href='http://cern.ch/mplamond'>Mathieu.Plamondon@cern.ch</a> for bugs and suggestions</li>"
           + "</ul>";
    document.getElementById("help").innerHTML = help;
    //document.getElementById("help").innerHTML += '<img width="300" src="/afs/cern.ch/user/l/larmon/public/ATLAS_schema.png">';
}

function putCaloCellsHelper() {
    var help = "<br /><hr />"
           + "<ul>"
           + "<li>Fill-in a run number and select a stream</li>"
           + "<li>By default, only <h style='color:red'>red</h> flags are reported. Check the box to display also the channels with the yellow ones.</li>"
           + "<li>Selections are effective only in the DET, AC and SAM fields</li>"
           + "<li>The DEVIATION column gives 4 links in blue to the history tool for this channel and the value of the deviation. A click on the latter redirects to the relevant savannah report: if bold, this link does not exist; if in italic, the channel number wasn't found in the savannah page (verify that this channel is concerned by that entry).</li>"
           + "<li>Double-click on a channel to get the standard format of it printed at the bottom:<br />"+FMT+"</li>"
           + "<li>Contact <a href='http://cern.ch/mplamond'>Mathieu.Plamondon@cern.ch</a> for bugs and suggestions</li>"
           + "</ul>";
    document.getElementById("help").innerHTML = help;
}

function ResetForm(f) {
  var el = document.forms[f].elements;
  for (var i=0;i<el.length;i++) {
    if ( el[i].hasAttribute("value") ) {
      if ( el[i].getAttribute("value")!="Reset" && el[i].getAttribute("value")!="Submit" && el[i].getAttribute("value")!="Help" ) el[i].setAttribute("value","");
    }
    if ( el[i].hasAttribute("selected") ) el[i].removeAttribute("selected");
    opt = el[i].getElementsByTagName("option");
    for (var j=0;j<opt.length;j++) {
      if ( opt[j].hasAttribute("selected") ) opt[j].removeAttribute("selected");
    }
  }
  if ( f>0 ) { 
    document.getElementById("help").innerHTML = "";
    var el0 = document.forms[0].elements;
    el0[0].setAttribute("value","");
  }   
}

function PrintFormat(fmt) {
  if ( !document.getElementById("format") ) {
    document.getElementById("help").innerHTML += "<br /><hr />"+FMT+"<br /><hr /><pre id=\"format\"></pre>";
  } 
  document.getElementById("format").innerHTML += fmt+"<br />";
}

function ChangeColor(tr) {
 if (tr.style.backgroundColor!="white") { tr.style.backgroundColor="white"; }
 else { tr.style.backgroundColor="#DDFFDD"; }
}

function showit(td,i) {
  td.innerHTML = i;
}

-->
