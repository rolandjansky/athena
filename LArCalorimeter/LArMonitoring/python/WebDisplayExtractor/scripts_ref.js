<!--

var FMT = "bec side ft sl ch 0 ProposedFlag # onlid";
var runnum = 0; var processing = 1;
var ecal_content; var upd4_content;
var TotalHN = 0; var TotalHN_PS = 0;
var TotalSpo = 0;var TotalSpo_PS = 0;
var nEMBA = 0;var nEMBC = 0;
var nEMECA = 0;var nEMECC = 0;
var nHECA = 0;var nHECC = 0;
var nFCALA = 0;var nFCALC = 0;


function Helper() {
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
}

function ResetForm() {
  var el = document.forms[0].elements;
  for (var i=0;i<el.length;i++) {
    if ( el[i].hasAttribute("value") ) {
      if ( el[i].getAttribute("value")!="Reset" && el[i].getAttribute("value")!="Submit" && el[i].getAttribute("value")!="Help" && el[i].getAttribute("value")!="Report" ) el[i].setAttribute("value","");
    }
    if ( el[i].hasAttribute("selected") ) el[i].removeAttribute("selected");
    opt = el[i].getElementsByTagName("option");
    for (var j=0;j<opt.length;j++) {
      if ( opt[j].hasAttribute("selected") ) opt[j].removeAttribute("selected");
    }
  }
  document.getElementById("help").innerHTML = "";
}

function ChangeColor(tr) {
 if (tr.style.backgroundColor!="white") { tr.style.backgroundColor="white"; }
 else { tr.style.backgroundColor="#DDFFDD"; }
}

function ToggleFlag(td) {
 if ( td.innerHTML=="" ) { td.innerHTML="sporadicBurstNoise"; td.style.backgroundColor="#FF6600"; }
 else if ( td.innerHTML=="sporadicBurstNoise" ) { td.innerHTML="highNoiseHG"; td.style.backgroundColor="#FF0000"; }
 else if ( td.innerHTML=="highNoiseHG" ) { td.innerHTML="Warning:ECAL"; td.style.backgroundColor="#FFFF00"; }
 else if ( td.innerHTML=="Warning:ECAL" ) { td.innerHTML="PedShift:ECAL"; td.style.backgroundColor="#FFFFCC"; }
 else if ( td.innerHTML=="PedShift:ECAL" ) { td.innerHTML="BeamBkg"; td.style.backgroundColor="#009900"; }
 else if ( td.innerHTML=="BeamBkg" ) { td.innerHTML=""; td.style.backgroundColor=""; }
 
 if ( td.style.fontWeight!="bold" ) td.style.fontWeight="bold";
}

function showit(td,i) {
  td.innerHTML = i;
}

function showit2(td) {
  var tmp = td.innerHTML;
  for (i=0;i<1;i++) { 
    //tmp = tmp.replace("showit2","showit3");
    //tmp = tmp.replace("show","hide");
    tmp = tmp.replace("<!--f","<!--b-->");
    tmp = tmp.replace("f-->","<!--e-->");
  }
  td.innerHTML = tmp;
}

//function showit2(td) {
//  var tmp = td.innerHTML;
//  for (i=0;i<1;i++) { 
//    tmp = tmp.replace("showit2","showit3");
//    tmp = tmp.replace("show","hide");
//    tmp = tmp.replace("<!--f","<!--b-->");
//    tmp = tmp.replace("f-->","<!--e-->");
//  }
//  td.innerHTML = tmp;
//}

function showit3(p) {
  var tmp = p.innerHTML;
  for (i=0;i<1;i++) { 
    tmp = tmp.replace("showit3","showit2");
    tmp = tmp.replace("hide","show");
    tmp = tmp.replace("<!--b-->","<!--f");
    tmp = tmp.replace("<!--e-->","f-->");
  }
  p.innerHTML = tmp;
}

function PrintDBExpertFormat(tr) {

  var td = tr.getElementsByTagName("td");
  var bec = 1;
  if ( td[2].innerHTML=="EMB" ) bec = 0;
  var side = 0;
  if ( td[3].innerHTML=="A+" ) side = 1;
  var flag = td[17].innerHTML;
  var sampling = td[7].innerHTML;

  //document.getElementById("help").innerHTML += "MY TEST "+td[2].innerHTML+td[3].innerHTML+" "+bec+" "+side+" "+td[4].innerHTML+" "+flag+"<br>";

  if ( flag != "highNoiseHG" && flag != "sporadicBurstNoise") return;

  // Check for duplicated cells associated to several clusters
  var cell_id = td[1].innerHTML;
  var det = td[2].innerHTML+td[3].innerHTML;
  check = upd4_content.indexOf(cell_id,0);
  if (check > 0) return; 

  // Increment counters
  if ( flag == "highNoiseHG") {
     TotalHN +=1; 
     if (sampling==0 && td[2].innerHTML != "HEC") TotalHN_PS +=1; 
  }
  if ( flag == "sporadicBurstNoise") {
     TotalSpo +=1;
     if (sampling==0 && td[2].innerHTML != "HEC") TotalSpo_PS +=1; 
  }
  if ( det == "EMBA+") { nEMBA +=1; }
  if ( det == "EMBC-") { nEMBC +=1; }
  if ( det == "EMECA+") { nEMECA +=1; }
  if ( det == "EMECC-") { nEMECC +=1; }
  if ( det == "HECA+") { nHECA +=1; }
  if ( det == "HECC-") { nHECC +=1; }
  if ( det == "FCALA+") { nFCALA +=1; }
  if ( det == "FCALA+") { nFCALC +=1; }	

  // store the channel
  var content = bec+" "+side+" "+td[4].innerHTML+" "+td[5].innerHTML+" "+td[6].innerHTML+" 0 "+flag+" # "+td[1].innerHTML;
  upd4_content +=content+"%0A";
  document.getElementById("help").innerHTML += content+"<br />";

}

function PrintInfoECAL(tr) {
  var td = tr.getElementsByTagName("td");
  //var flag = td[16].innerHTML;
  var flag = td[17].innerHTML;
  var substr = flag.split(":");
  //document.getElementById("help").innerHTML += "MY TEST ECAL"+td[4].innerHTML+" "+flag+"<br>";
  if ( substr.length == 1 ) return;
  var part = td[2].innerHTML+td[3].innerHTML[0]; 
  var ft = td[4].innerHTML; 
  var slot = td[5].innerHTML; 
  var ch = td[6].innerHTML;
  var sampling = td[7].innerHTML;
  //////////////////////////////////////////////////////////////////
  url="https://lar-elec-automatic-validation.web.cern.ch/lar-elec-automatic-validation/cgi-bin/History.sh?";
  url_html=url+"sub_sub_partition="+part+"&ft="+ft+"&sl="+slot+"&ch="+ch+"&printFormat=Ratio";
  url_ascii=url+"sub_sub_partition="+part+"%26ft="+ft+"%26sl="+slot+"%26ch="+ch+"%26printFormat=Ratio";
  var content= part+" "+ft+" "+slot+" "+ch+" "+sampling+" "+flag+" ";
  ////////////////////////////////////////////////////////////////////
  document.getElementById("help").innerHTML += content+"<a href="+url_html+">link to ECAL history</a>"+"<br />";
  ecal_content += " * "+content+" Link to ECAL history: "+url_ascii+"%0A";
  //ecal_content += " * "+content+" Link to ECAL history:<a href='"+url_ascii+"'>link to ECAL history</a>%0A";
}

function Report(info,run,proc,defects,beambkg){
  runnum = run;
  processing = proc;
  //var dstring = String(defects);
  //var dc = dstring.charAt(dstring.length()-1);
  

  /// start with cleaning the "help" section
  document.getElementById("help").innerHTML = "";

  /// Title & Infos
  if (info == 0) {
      document.getElementById("help").innerHTML += "<hr />Defect DB upload (<font color=green>G</font>=defect absent <font color=yellow>Y</font>=defect present,recoverable <font color=red>R</font>=defect present,unrecoverable)<hr />";
      document.getElementById("help").innerHTML += defects+'<br>';
      //if (beambkg == 1) document.getElementById("help").innerHTML += "NB:  This list produced with BB <br>";
      // Add button to upload defects and launch LCE jobs
      document.getElementById("help").innerHTML += '<br><input type="button" value="Upload Defects" onclick="UploadDefects()"/>';
  }

  /// Bad Channels
  if (info == 1) {

    // reset the global variables used to store channels
    ecal_content = "";
    upd4_content = "";
    TotalHN = 0; TotalHN_PS = 0;
    TotalSpo = 0; TotalSpo_PS = 0;
    nEMBA = 0;nEMBC = 0;
    nEMECA = 0;nEMECC = 0;
    nHECA = 0;nHECC = 0;
    nFCALA = 0;nFCALC = 0;

    // Retrieve list of channels
    var tr = document.getElementsByTagName("tr");

    // First, channels that have to be put in the DB
    document.getElementById("help").innerHTML += "<hr />"; 
    document.getElementById("help").innerHTML += "Cells requiring masking: ";
    document.getElementById("help").innerHTML += "BEC Side FT SL CH CL flag # onlid <hr />";
    var help = document.getElementById("help").innerHTML;

    // Print cells
    for (var i=0;i<tr.length;i++) {
      var td = tr[i].getElementsByTagName("td");
      if ( td.length<10 ) continue;
      if ( td[1].innerHTML[1] != "x" ) continue;
      if ( td[16].innerHTML[1] == "" ) continue; 
      help += PrintDBExpertFormat(tr[i]); 
    }

   // Print summary numbers
    document.getElementById("help").innerHTML += "<br>Total number of sporadicBurstNoise (PS) : "+TotalSpo+" ("+TotalSpo_PS+")<br>";
    document.getElementById("help").innerHTML += "Total number of highNoiseHG (PS) : "+TotalHN+" ("+TotalHN_PS+")<br>";
    if (beambkg == 1) {
      document.getElementById("help").innerHTML += "NB : this list has been produced with BeamBkg option<br>";
    }

    if (TotalSpo > 700 || TotalHN > 30) document.getElementById("help").innerHTML += "<font color=red>The number of proposed channels seems to be really high. It is perhaps due to beam background. Please contact the DQ coordinators if you have any doubt </font><br>"

    // Add button to email the LAr DB Experts
      if (beambkg == 1)document.getElementById("help").innerHTML += '<br><input type="button" value="Send Mail to DB Experts" onclick="emailDBExperts(1)"/>';
      else document.getElementById("help").innerHTML += '<br><input type="button" value="Send Mail to DB Experts" onclick="emailDBExperts(0)"/>';

    // Fill & Print defects // BULK only
    var defects = "";
    document.getElementById("help").innerHTML += "<hr />";

    if (processing == 1 || processing == 101 || processing == 111) {
        document.getElementById("help").innerHTML += "Text file for Defect DB upload (<font color=green>G</font>=defect absent <font color=yellow>Y</font>=defect present,recoverable <font color=red>R</font>=defect present,unrecoverable)<hr />";
	defects += "@"+runnum+"<br>";
	defects += "LAR_UNCHECKED <font color=green>G</font> # ES has been checked by LADIeS <br>"
    }

    if (processing == 2 || processing == 102) {

	defects += "@"+runnum+"<br>";
	defects += "LAR_BULK_UNCHECKED <font color=green>G</font> # BULK has been checked by LADIeS <br>"
	if (nEMBA>0) { defects += "LAR_EMBA_NOISYCHANNEL <font color=yellow>Y</font> # "+nEMBA+" Noisy Cells - To be masked after reprocessing<br>"; }
	if (nEMBC>0) { defects += "LAR_EMBC_NOISYCHANNEL <font color=yellow>Y</font> # "+nEMBC+" Noisy Cells - To be masked after reprocessing<br>"; }
	if (nEMECA>0) { defects += "LAR_EMECA_NOISYCHANNEL <font color=yellow>Y</font> # "+nEMECA+" Noisy Cells - To be masked after reprocessing<br>"; }
	if (nEMECC>0) { defects += "LAR_EMECC_NOISYCHANNEL <font color=yellow>Y</font> # "+nEMECC+" Noisy Cells - To be masked after reprocessing<br>"; }
	if (nHECA>0) { defects += "LAR_HECA_NOISYCHANNEL <font color=yellow>Y</font> # "+nHECA+" Noisy Cells - To be masked after reprocessing<br>"; }
	if (nHECC>0) { defects += "LAR_HECC_NOISYCHANNEL <font color=yellow>Y</font> # "+nHECC+" Noisy Cells - To be masked after reprocessing<br>"; }
	if (nFCALA>0) { defects += "LAR_FCALA_NOISYCHANNEL <font color=yellow>Y</font> # "+nFCALA+" Noisy Cells - To be masked after reprocessing<br>"; }
	if (nFCALC>0) { defects += "LAR_FCALC_NOISYCHANNEL <font color=yellow>Y</font> # "+nFCALC+" Noisy Cells - To be masked after reprocessing<br>"; }
	
    }
    document.getElementById("help").innerHTML += defects;

    // Then, add channels that have to be reported to the ECAL Team - Only for express stream
    if (processing == 1 || processing == 101 || processing == 111) {
 	document.getElementById("help").innerHTML += "<hr />"; 
    	document.getElementById("help").innerHTML += "Cells to report to the ECAL team: ft sl ch layer flag <hr />";
    	var help = document.getElementById("help").innerHTML;
    	for (var i=0;i<tr.length;i++) {
      		var td = tr[i].getElementsByTagName("td");
      		if ( td.length<10 ) continue;
      		if ( td[1].innerHTML[1] != "x" ) continue;
      		if ( td[16].innerHTML[1] == "" ) continue;
      		help +=PrintInfoECAL(tr[i]); 
    	}
    	// Button to email the ECAL team
	document.getElementById("help").innerHTML += '<br><input type="button" value="Send Mail to ECAL" onclick="emailECAL()"/>';
    }
  }
}

function emailDBExperts(beambkg){
  var email = "atlas-lar-db-experts@cern.ch";
  var DBTAG = "UPD4"; var ADD="";
  if (processing == 2 || processing == 102){
      DBTAG = "UPD3";
      ADD=" they were missed during the Express-Stream "
  }
  var subject = "LADIeS Report: cells to be masked in "+DBTAG+" for run "+runnum;
  var body_message = "Dear LAr DB Experts,%0A%0A the LADIeS inform you that the following channels are inducing clusters,"+ADD+" and need to be added in "+DBTAG+" for run "+runnum+":%0A%0A";
  // summary numbers
  body_message += "Total number of sporadicBurstNoise (PS) : "+TotalSpo+" ("+TotalSpo_PS+")%0A";
  body_message += "Total number of highNoiseHG (PS) : "+TotalHN+" ("+TotalHN_PS+")%0A%0A";
  if (beambkg) body_message += "NB : the following list has been produced with BeamBkg option%0A%0A";
  body_message += upd4_content+"%0A%0A"
  // signature
  body_message += "Sincerely Yours,%0A%0A --The LADIeS Team";
  var mailto_link = 'mailto:'+email+'?subject='+subject+'&body='+body_message;
  win = window.open(mailto_link,'emailWindow');
  if (win && win.open &&!win.closed) win.close();
}

function emailECAL(){

  //var email = "hn-atlas-lar-electronic-calibration@cern.ch,Atlas.Larcalibration@cern.ch";
  var email = "hn-atlas-lar-electronic-calibration@cern.ch;majewski@bnl.gov;benjamin.trocme@cern.ch;larcalib@cern.ch;Atlas.Larcalibration@cern.ch";
  var subject = "LADIeS Report: suspicious cells in run "+runnum;
  var body_message = "Dear ECAL Team,%0A%0A the LADIeS inform you that significant deviations are observed for the following channels in run "+runnum+":%0A%0A"+ecal_content+"%0A Although those channels are not creating clusters (yet), could you have a look at them ?%0A%0ASincerely Yours,%0A%0A --The LADIeS Team";
  var mailto_link = 'mailto:'+email+'?subject='+subject+'&body='+body_message;
  win = window.open(mailto_link,'emailWindow');
  if (win && win.open &&!win.closed) win.close();

  //OutputStream outputStream = new FileOutputStream("ECAL/run_"+runnum+".txt"); 
  //Writer writer = new OutputStreamWriter(outputStream); 
  //writer.write("Hello World"); writer.close();

  //OutputStream outputStream = new FileOutputStream(); 
  //Writer writer = new OutputStreamWriter(outputStream); 
  //writer.write("Hello World"); writer.close();

  // Dump the same info in a text file 
  //var dir = "/afs/cern.ch/user/l/larmon/public/WebDisplayExtractor/ECAL/";
  //var dir = "/var/vhost/atlas-larmon/secure/WebDisplayExtractor/ECAL/";
  //var filename = "ListOfSuspiciousCells_run"+runnum+".txt";
  //var fh = fopen(dir+filename, 3); // Open the file for writing
  //if(fh!=-1) { // If the file has been successfully opened
  //   fwrite(fh,ecal_content); // Write the string to a file
  //}
  //fclose(fh); // Close the file 

  //File f;
  //f=new File("myfile.txt");
  //if(!f.exists()){
  //f.createNewFile();
  //System.out.println("New file \"myfile.txt\" has been created to the current directory");

}

function DefectDB(runnum) {
    // function to be improve to maybe, to load file automatically someday
    if (runnum != ""){
       var url = "https://atlasdqm.cern.ch/defectentry/?run="+runnum+"&filter=LAr";
       window.open(url);
    }
}

function DCSQuery(runnum) {
    if (runnum != ""){
       var url = "http://atlas-runquery.cern.ch/query.py?q=find+run+"+runnum+"+%2F+show++dq+lar+DCSOFL";
       window.open(url);
    }
}

function UploadDefects() {

}

function LArDBlog() {
   var url = "https://atlas-larmon.cern.ch/WebDisplayExtractor/DatabaseChangelog";
   window.open(url);
}

-->
