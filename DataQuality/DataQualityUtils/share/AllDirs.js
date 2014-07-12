
// ************************************************************
// $Id: AllDirs.js,v 1.3 2009-03-16 19:42:18 boelaert Exp $
// ************************************************************


var xmlDoc;
var xmlhttp;
var y;
var selectionList;
var htmlIndex;
var Dir;
var Selection = new Array();
var CountSel=0;

function createMenu(){
	selectionList = xmlDoc;	
	Selection = new Array();
	CountSel =0;
	createSubMenu("subdir");
}


function loadXMLDoc(url)
{
	xmlhttp=null;
	if (window.XMLHttpRequest)
  	{// code for IE7, Firefox, Mozilla, etc.
  		xmlhttp=new XMLHttpRequest();
  	}
	else if (window.ActiveXObject)
  	{// code for IE5, IE6
		xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
  	}
	if (xmlhttp!=null)
  	{
  		xmlhttp.onreadystatechange=function(){
        	onResponse();
		  }	
	xmlhttp.open("GET",url,true);
  	xmlhttp.send(null);
  	}
	else
  	{
  	alert("Your browser does not support XMLHTTP.");
  	}
}


function onResponse()
{
	if(xmlhttp.readyState!=4) return;
	if(xmlhttp.status!=200)
  	{
  		alert("Problem retrieving XML data");
  		return;
  	}
	xmlDoc = xmlhttp.responseXML;
	createMenu();
	displaySubdirs();	
}


function containsHistos(system)
{
	subs = system.getElementsByTagName("dir");
	value = false;
	var k1=0;
	if(system.getAttribute('histo')=='yes'){
		value = true;
	}
	while (k1<subs.length && !value){
		if (subs[k1].getAttribute('histo')=='yes')
		{
		value = true;
		}
		k1++;
	}
	return value;
}


function createSubMenu(tagDir)
{
  var htmlIndex = String(window.location);
  Dir = tagDir;
  id = htmlIndex.indexOf("index.html");
  if (id>0){
	htmlIndex = htmlIndex.substring(0,id-1);
  }
  txt ="";
  if (tagDir == "subdir"){
    temp=selectionList.getElementsByTagName("dir");	
    templi = temp[0].childNodes;
    var y = new Array();
    var counter = 0;
    for (k=0;k<templi.length;k++){
 	if(templi[k].nodeName=="dir" && containsHistos(templi[k])) {
		y[counter]=templi[k];
		counter++;
	}
    }	
    for (j=0;j<y.length;j++){	
	tempList=y[j].getElementsByTagName("a");
	if (tempList.length>0){
		txt ="<select id=\"menu_"+tagDir + "\" onchange=\"createSubMenu(\'sub"+tagDir+"\');\" >\n";
		txt +="<option>--select directory--</option>\n";	
		break;
	}
    }
    if (txt ==""){
	txt +="<select id=\"menu_"+tagDir + "\" onchange=\"createSubMenu(\'sub"+tagDir+"\');\" >\n";
	txt +="<option>--no histograms--</option>\n";
    }
  }
  else {
    txt +="<select id=\"menu_"+tagDir + "\" onchange=\"createSubMenu(\'sub"+tagDir+"\');\" >\n";
    prevDir = tagDir.substring(3);
    CountSel2 =Math.round(prevDir.length/3-2);
    if (CountSel2<CountSel) {
    	var Selection2 = new Array();
	for (i=0;i<CountSel2;i++) {
		Selection2[i]=Selection[i];
	}
	for (i = CountSel2; i<CountSel; i++){
	tagD = "sub" + prevDir;
	document.getElementById("Select_"+tagD).innerHTML=" ";	
	selectionList=selectionList.parentNode;
	}
	Selection = Selection2;
	CountSel = CountSel2;
    }
    var mylist = document.getElementById("menu_"+prevDir);
    prevDirIDStat = mylist.options[mylist.selectedIndex].text;		
    templi=prevDirIDStat.split(" -");
    prevDirID = templi[0];
    Selection[CountSel]=prevDirID;
    CountSel++;
    x = selectionList.getElementsByTagName("dir");
    for (i=0;i<x.length;i++)
	{
	if (x[i].getAttribute("tag")==prevDir && x[i].getAttribute("id")==prevDirID){
		selectionList = x[i];
		templi=x[i].childNodes;
		var y = new Array();
		var counter = 0;
		for (k=0;k<templi.length;k++){
			if(templi[k].nodeName=="dir" && containsHistos(templi[k])){
			y[counter]=templi[k];
			counter++;
			}
		}
		templi =x[i].getElementsByTagName("a");				
		if (templi.length==1){
			pageLink = templi[0].getAttribute("href");
			window.location = htmlIndex+"/"+pageLink ;	
			return;
		}

	}
     }
     txt +="<option>--select subdirectory--</option>\n";		
}
for (j=0;j<y.length;j++)
       {
        	txt +="<option>"+ y[j].getAttribute("id")+" - "+ y[j].getAttribute("status")+ "</option>";	

 }
 txt +="</select>\n"; 
 txt +="<p></p>\n<div id=\"Select_sub"+tagDir + "\"> </div>";
 document.getElementById("Select_"+tagDir).innerHTML=txt;
 displaySubdirs();
  }
  
 function displaySubdirs()
 {
  	x = xmlDoc.getElementsByTagName("dir");
	var directory;
	var globalName="";
	if (CountSel!=0) {
	globalName+="Overall Status/";
	}
	var DirName="";
	if(CountSel==0){
		Selection[CountSel]="Overall Status";
		for (i=0;i<x.length;i++)
		{	
			if (x[i].getAttribute("id")==Selection[0]){
				directory = x[i];
				DirName = directory.getAttribute("id");
				globalName += DirName +"/";
				DirStatus = directory.getAttribute("status");
				histo = directory.getAttribute("histo");
				temp = x[i].childNodes;
			}
		}
	}
	else {
 	    	for (i=0;i<x.length;i++)
		{	
			if (x[i].getAttribute("id")==Selection[CountSel-1]){
				directory = x[i];	
				DirName = directory.getAttribute("id");
				globalName += DirName +"/";
				DirStatus = directory.getAttribute("status");
				histo = directory.getAttribute("histo");
			
			}
		}
	}	


	if (globalName!="Overall Status/"){
//	document.write(x.length + " name: " + globalName + " status " + DirStatus + " histo " +  histo);	
        }
	var htmlIndex = String(window.location);
  	id = htmlIndex.indexOf("index.html");	
  	if (id>0){
		htmlIndex = htmlIndex.substring(0,id-1);
	  }	
	dirList = directory.getElementsByTagName("dir");
	txt = '<hr />\n<h2>' + globalName+ '</h2> \n<font class="DQGroup">\n'
	txt += '<ul><li>\n';
	num_lists = 1;
	if (histo=="yes"){
		html = directory.firstChild;
		while(html.nodeName!="a"){
			html=html.nextSibling;
		}
		txt +='<a href=\"'+htmlIndex + '/' + html.getAttribute("href")+'\">'+DirName+':</a>';
	}
	else {
		txt +=DirName +':'
	}
	txt+= '&nbsp;&nbsp;&nbsp;<font class=\"'+DirStatus+'\">'+DirStatus+'</font>\n';
	prevDirTag = directory.getAttribute("tag");
	for (i=0;i<dirList.length;i++)
		{	
			DirTag = dirList[i].getAttribute("tag");
			if (DirTag.length > prevDirTag.length) {  // current dir is subdirectory of previous dir -> new item in new list
				txt += '<ul><li>\n';
				num_lists++;
			}
			else if ( DirTag.length == prevDirTag.length ) { // current dir and previous dir in same motherdir -> new item
				txt+='</li>\n<li>\n';
			}
			else { // close list
				txt+='</li>';
				diff = (prevDirTag.length-DirTag.length)/3;  // count how many lists (= number of "sub" differences) to close
				//document.write(diff  +'\n');
				while (diff>0){
					txt+='</ul>\n</li>\n';
					diff--;
					num_lists--;
				}
				txt+='<li>\n';
			}
			DirName = dirList[i].getAttribute("id");
			DirStatus = dirList[i].getAttribute("status");
			histo = dirList[i].getAttribute("histo");
			if (histo=="yes") { // dir contains histos
				html = dirList[i].firstChild;
				while(html.nodeName!="a"){
				html=html.nextSibling;
				}
				txt +='<a href=\"'+htmlIndex + '/' + html.getAttribute("href")+'\">'+DirName+':</a>';
			}
			else {
				txt +=DirName +':'
			}
			txt+= '&nbsp;&nbsp;&nbsp;<font class=\"'+DirStatus+'\">'+DirStatus+'</font>\n';
			prevDirTag = DirTag;			
		}
	if (num_lists > 0) {
		txt +='</li>\n';
	}
	for(i=0; i<num_lists-1; i++){
		txt+='</ul>\n</li>\n';
	}
	if (num_lists > 0) {
		txt +='</ul>\n';
	}
	txt+='</font>';	
	document.getElementById("Display_dir").innerHTML=txt;
//	createMenu('All')
 }
 
function cursor(message)
{
	document.getElementById("trail").innerHTML=message;
	document.getElementById("trail").style.visibility="visible";
	// trail.innerHTML=message;
	// trail.style.visibility="visible";
	// trail.style.position="absolute";
	// trail.style.left=event.clientX+10;
	// trail.style.top=event.clientY; 
//	document.write("ok");
}

function hidecursor()
{
	trail.style.visibility="hidden";
}
	
var horizontal_offset="9px" //horizontal offset of hint box from anchor link

/////No further editting needed

var vertical_offset="0" //horizontal offset of hint box from anchor link. No need to change.
var ie=document.all
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

function iecompattest(){
return (document.compatMode && document.compatMode!="BackCompat")? document.documentElement : document.body
}

function clearbrowseredge(obj, whichedge){
var edgeoffset=(whichedge=="rightedge")? parseInt(horizontal_offset)*-1 : parseInt(vertical_offset)*-1
if (whichedge=="rightedge"){
	var windowedge=ie && !window.opera? iecompattest().scrollLeft+iecompattest().clientWidth-30 : window.pageXOffset+window.innerWidth-40
	dropmenuobj.contentmeasure=dropmenuobj.offsetWidth
	if (windowedge-dropmenuobj.x < dropmenuobj.contentmeasure)
		edgeoffset=dropmenuobj.contentmeasure+obj.offsetWidth+parseInt(horizontal_offset)
}
else{
	var windowedge=ie && !window.opera? iecompattest().scrollTop+iecompattest().clientHeight-15 : window.pageYOffset+window.innerHeight-18
	dropmenuobj.contentmeasure=dropmenuobj.offsetHeight
	if (windowedge-dropmenuobj.y < dropmenuobj.contentmeasure)
		edgeoffset=dropmenuobj.contentmeasure-obj.offsetHeight
}	
return edgeoffset
}

function showhint(menucontents, obj, e, tipwidth){
if ((ie||ns6) && document.getElementById("hintbox")){
	dropmenuobj=document.getElementById("hintbox")
	dropmenuobj.innerHTML=menucontents
	dropmenuobj.style.left=dropmenuobj.style.top=-500
	if (tipwidth!=""){
		dropmenuobj.widthobj=dropmenuobj.style
		dropmenuobj.widthobj.width=tipwidth
	}
	dropmenuobj.x=getposOffset(obj, "left")
	dropmenuobj.y=getposOffset(obj, "top")
	dropmenuobj.style.left=dropmenuobj.x-clearbrowseredge(obj, "rightedge")+obj.offsetWidth+"px"
	dropmenuobj.style.top=dropmenuobj.y-clearbrowseredge(obj, "bottomedge")+"px"
	dropmenuobj.style.visibility="visible"
	obj.onmouseout=hidetip
}
}

function hidetip(e){
dropmenuobj.style.visibility="hidden"
dropmenuobj.style.left="-500px"
}

function createhintbox(){
var divblock=document.createElement("div")
divblock.setAttribute("id", "hintbox")
document.body.appendChild(divblock)
}

if (window.addEventListener)
window.addEventListener("load", createhintbox, false)
else if (window.attachEvent)
window.attachEvent("onload", createhintbox)
else if (document.getElementById)
window.onload=createhintbox 
