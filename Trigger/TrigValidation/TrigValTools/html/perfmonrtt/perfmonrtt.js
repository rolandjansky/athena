
/***********************************************
* AnyLink Drop Down Menu- © Dynamic Drive (www.dynamicdrive.com)
* This notice MUST stay intact for legal use
* Visit http://www.dynamicdrive.com/ for full source code
***********************************************/

/***********************************************
* Adopted for ATLAS TriggerValidation
* by Frank Winklmeier
***********************************************/

var menuwidth='150px' //default menu width
var disappeardelay=250  //menu disappear speed onMouseout (in miliseconds)
var hidemenu_onclick="yes" //hide menu when user clicks within menu?

/////No further editting needed

var ie4=document.all
var ns6=document.getElementById&&!document.all

function getposOffset(what, offsettype){
var totaloffset=(offsettype=="left")? what.offsetLeft : what.offsetTop;
var parentEl=what.offsetParent;
while (parentEl!=null){
totaloffset=(offsettype=="left")? totaloffset+parentEl.offsetLeft : totaloffset+parentEl.offsetTop;
parentEl=parentEl.offsetParent;
}
return totaloffset;
}


function showhide(obj, e, visible, hidden){
if (ie4||ns6)
dropmenuobj.style.left=dropmenuobj.style.top="-500px"

  dropmenuobj.widthobj=dropmenuobj.style
  dropmenuobj.widthobj.width=menuwidth

if (e.type=="click" && obj.visibility!=visible || e.type=="mouseover")
obj.visibility=visible
else if (e.type=="click")
obj.visibility=hidden
}

function iecompattest(){
return (document.compatMode && document.compatMode!="BackCompat")? document.documentElement : document.body
}

function clearbrowseredge(obj, whichedge){
var edgeoffset=0
if (whichedge=="rightedge"){
var windowedge=ie4 && !window.opera? iecompattest().scrollLeft+iecompattest().clientWidth-15 : window.pageXOffset+window.innerWidth-15
dropmenuobj.contentmeasure=dropmenuobj.offsetWidth
if (windowedge-dropmenuobj.x < dropmenuobj.contentmeasure)
edgeoffset=dropmenuobj.contentmeasure-obj.offsetWidth
}
else{
var topedge=ie4 && !window.opera? iecompattest().scrollTop : window.pageYOffset
var windowedge=ie4 && !window.opera? iecompattest().scrollTop+iecompattest().clientHeight-15 : window.pageYOffset+window.innerHeight-18
dropmenuobj.contentmeasure=dropmenuobj.offsetHeight
if (windowedge-dropmenuobj.y < dropmenuobj.contentmeasure){ //move up?
edgeoffset=dropmenuobj.contentmeasure+obj.offsetHeight
if ((dropmenuobj.y-topedge)<dropmenuobj.contentmeasure) //up no good either?
edgeoffset=dropmenuobj.y+obj.offsetHeight-topedge
}
}
return edgeoffset
}


function dropdown(obj, e, menu){
if (window.event) event.cancelBubble=true
else if (e.stopPropagation) e.stopPropagation()
clearhide()
dropmenuobj=document.getElementById? document.getElementById(menu) : dropmenudiv

if (ie4||ns6){
showhide(dropmenuobj.style, e, "visible", "hidden")
dropmenuobj.x=getposOffset(obj, "left")
dropmenuobj.y=getposOffset(obj, "top")
dropmenuobj.style.left=dropmenuobj.x-clearbrowseredge(obj, "rightedge")+"px"
dropmenuobj.style.top=dropmenuobj.y-clearbrowseredge(obj, "bottomedge")+obj.offsetHeight+"px"
}

return clickreturnvalue()
}

function clickreturnvalue(){
if (ie4||ns6) return false
else return true
}

function contains_ns6(a, b) {
while (b.parentNode)
if ((b = b.parentNode) == a)
return true;
return false;
}

function dynamichide(e){
if (ie4&&!dropmenuobj.contains(e.toElement))
delayhide()
else if (ns6&&e.currentTarget!= e.relatedTarget&& !contains_ns6(e.currentTarget, e.relatedTarget))
delayhide()
}

function hidemenu(e){
if (typeof dropmenuobj!="undefined"){
if (ie4||ns6)
dropmenuobj.style.visibility="hidden"
}
}

function delayhide(){
if (ie4||ns6)
delayhide_t=setTimeout("hidemenu()",disappeardelay)
}

function clearhide(){
if (typeof delayhide_t!="undefined")
clearTimeout(delayhide_t)
}

if (hidemenu_onclick=="yes")
document.onclick=hidemenu



// Checkbox saving inspired by:
// -----------------------------------------------------//
// SAVE FORM FIELD SELECTIONS IN COOKIES 		//
// formcookie_saverestore.js				//
// Written by Tony Davis, T. Davis Consulting, Inc.	//
// Date written: September 27, 2005			//
// Email: tony@tdavisconsulting.com			//
// Web site: http://www.tdavisconsulting.com		//
// -----------------------------------------------------//

//
// Save disabled checkboxes into cookie
//
function saveSelections() {

  // Expire cookie in 999 days.
  var today = new Date();
  var exp   = new Date(today.getTime()+999*24*60*60*1000);
  
  for (f = 0; f < document.forms.length; f++) {
    frm = document.forms[f];
    var value = "formname=" + frm.name;
    for (i = 0; i < frm.length; i++) {
      var e = frm.elements[i];
      if (e.type=="checkbox") {
        if (e.checked==false) value += "|" + e.value;
      }
      else if (e.type=="radio") {
        if (e.checked==true) value += "|" + e.value;
      }
    }
    //  console.log("Set cookie %s: %s",frm.name,value);
    setCookie(frm.name, value, exp);
  }
}

//
// Restore disabled checkboxes from cookie
//
function loadSelections(formId) {

  var frm = document.forms["form"+formId];
   
  cookie = getCookie(frm.name);
  //  console.log("Loading cookie %s: %s", frm.name, cookie);
  if (cookie==null) return;
  var values = cookie.split("|");

  // Loop over all form elements
  for (i = 0; i < frm.length; i++) {
    e = frm.elements[i];
    for (j = 1; j < values.length; j++) {
      if (e.type=="checkbox") {
        if (values[j]==e.value) e.checked = false;
      }
      else if (e.type=="radio") {
        if (values[j]==e.value) e.checked = true;
      }
    }
    
  }
}

/// COOKIE FUNCTIONS

function setCookie(name, value, expires, path, domain, secure) {
  document.cookie = "trigperfmonrtt." + name + "=" + escape(value) +
    ((expires) ? "; expires=" + expires.toGMTString() : "") +
    ((path) ? "; path=" + path : "") +
    ((domain) ? "; domain=" + domain : "") +
    ((secure) ? "; secure" : "");
}

function getCookie(name) {
  var dc = document.cookie;
  var prefix = "trigperfmonrtt." + name + "=";
  var begin = dc.indexOf("; " + prefix);
  if (begin == -1) {
    begin = dc.indexOf(prefix);
    if (begin != 0) return null;
  } else {
    begin += 2;
  }
  var end = document.cookie.indexOf(";", begin);
  if (end == -1) {
    end = dc.length;
  }
  return unescape(dc.substring(begin + prefix.length, end));
}


  
/**********************************
* Misc functions
**********************************/

// Reset all forms
function resetForms() {
  for (i=0; i<document.forms.length; i++)
    document.forms[i].reset();
}
        
// Toggle visibility of builds
function toggleBuild(tableId, build) {
  if (document.getElementById) {
    chk = document.forms["form"+tableId].elements;

    // Count how many builds are selectedbrowser
    var nBuilds = 0;
    for (i=0; i<chk.length; i++) if (chk[i].checked) nBuilds++;

    // Get current table
    t = document.getElementById("table"+tableId);

    for (i=1; i<t.rows.length; i++) {
      // Change rowspan (IE works better without it)
      if (navigator.appName!="Microsoft Internet Explorer") {
        if (t.rows[i].cells[0].className=="testName")
          t.rows[i].cells[0].rowSpan = nBuilds+2;
      }

      // Toggle visibility
      if (build) {   // for a given build
        if (t.rows[i].id==build)
          t.rows[i].style.display = (t.rows[i].style.display=="") ?  "none" : "";
      }
      else {         // for all builds
        for (c=0; c<chk.length; c++) {
          if (t.rows[i].id==chk[c].value)
            t.rows[i].style.display = (chk[c].checked) ?  "" : "none";
        }
      }      
    }
  }
}

// Load test results in main frame depending on checkbox selection
function loadMainFrame(radios, frame) {
  for (i=0; i<radios.length; i++) {
    if (radios[i].checked) frame.location.href = radios[i].value + ".html";
  }
}


// Open mem graph in new window
function mem(url) {
  w = window.open(url, "mem", "width=850,height=650,resizable=yes,location=yes,menubar=yes,scrollbars=yes,status=yes,toolbar=yes");
  w.focus();
}
    
