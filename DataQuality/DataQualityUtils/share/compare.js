
// ************************************************************
// $Id: compare.js,v 1.1 2008-09-17 12:36:36 mgwilson Exp $
// ************************************************************

var xmlDoc;
var xmlhttp;

function loadXMLDoc(url,hisname)
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
        onResponse(hisname);  
  } 
  xmlhttp.open("GET",url,true);
  xmlhttp.send(null);
  }
else
  {
  alert("Your browser does not support XMLHTTP.");
  }
}


 function isThere(url) {
         var req= new XMLHttpRequest(); // XMLHttpRequest object
         try {
                 req.open("HEAD", url, false);
                req.send(null);
                return req.status== 200 ? true : false;
         }
         catch (er) {
                return false;
         }
 }

function onResponse(hisname)
{
if(xmlhttp.readyState!=4) return;
if(xmlhttp.status!=200)
  {
  alert("Problem retrieving XML data");
  return;
  }
xmlDoc = xmlhttp.responseXML;
createStreamMenu(hisname);
}

function createStreamMenu(hisname){
  x=xmlDoc.getElementsByTagName("Stream");	
  text ="<select id=\"StreamInput\" onchange=\"createRunMenu(\'"+hisname+"\');\" >\n";
  text +="<option>Select Stream</option>\n";
  for (i=0;i<x.length;i++)
	     { 
	     urlList = x[i].getElementsByTagName("html");
	     url = urlList[urlList.length-1].childNodes[0].nodeValue+"/"+hisname+".html";
		if (isThere(url))
		{	
	     	text +="<option>";
	     	text +=x[i].getAttribute("id");
	     	text += "</option>\n";
		}
	     }
  text +="</select>\n";
  document.getElementById("streamSelect").innerHTML=text;
}

function createRunMenu(hisname){
    var mylist = document.getElementById("StreamInput");
    stream = mylist.options[mylist.selectedIndex].text;
    x=xmlDoc.getElementsByTagName("Stream");
    text = " ";
    for (i=0;i<x.length;i++){
	y = x[i].getAttribute("id");
	if (y==stream){
	    runs = x[i].getElementsByTagName("Run");
	    text +="<select id=\"RunInput\" onchange=\"find2ndHisto(\'"+hisname+"\',\'"+stream+"\');\" >\n";
	    text +="<option>Select Run</option>\n";
	    for (j=0;j<runs.length;j++){
		urlList = runs[j].getElementsByTagName("html");
	        url = urlList[0].childNodes[0].nodeValue+"/"+hisname+".html";
		if  (isThere(url))
		{	
		    text +="<option>";
		    text +=runs[j].getAttribute("runNumber");
		    text += "</option>\n";
		}
	    }
	}
    }
    text +="</select>\n";
    document.getElementById("runSelect").innerHTML=text;
}


function compare(runlist,hisname){
    loadXMLDoc(runlist,hisname);
}

function insertIFrame(fromSource,tag){
    var insertionPt = document.getElementById(tag);
    insertionPt.innerHTML='';
    var iframe=document.createElement('iframe');
    iframe.setAttribute('id', 'contentFRM');
    iframe.setAttribute('frameborder', '0');
    iframe.setAttribute('src',fromSource);
    iframe.setAttribute('width', '100%');
    iframe.setAttribute('height','900');
   // iframe.setAttribute('scrolling','no');
    insertionPt.appendChild(iframe);
}

function find2ndHisto(hisname,stream){
    mydiv = document.getElementById("compareButton");
    mydiv.style.display = "none"; //to hide it
    var mylist = document.getElementById("RunInput");
    run = mylist.options[mylist.selectedIndex].text;
    x=xmlDoc.getElementsByTagName("Stream");
    html="";
    for (i=0;i<x.length;i++){
 	y = x[i].getAttribute("id");
 	if (y==stream){
 	    runs = x[i].getElementsByTagName("Run");
 	    for (j=0;j<runs.length;j++){
 		if(runs[j].getAttribute("runNumber")==run){
 		    html += (runs[j].getElementsByTagName("html")[0].childNodes[0].nodeValue+"/"+hisname+".html");
 		}
 	    }
 	}
     }
//    document.write(html);
    insertIFrame(html,'secondHisto');	
    mydiv = document.getElementById("compareButton");
    mydiv.style.display = "none"; //to hide it
    test =document.getElementById('contentFRM');
    mydiv = test.getElementById("compareButton");
    document.write(mydiv.style.display);	
    mydiv.style.display = "none";
    document.write(mydiv.style.display);

}





