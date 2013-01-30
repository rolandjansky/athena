function helppopup() {
   helpwin = window.open("atlas-runquery-help.html", "Help for Run Query","location=no,width=950,height=700,scrollbars=yes");
}

function openWindow(name,text) { 
   win = window.open("", name, "location=0,left=50,width=720,height=550,scrollbars=1");
   win.locationbar.visible = false;
   win.focus();
   win.document.open();
   win.document.writeln( text );
   win.document.close();
}

function openTextWindow(name,text) { 
   textwin = window.open("", name, "location=0,width=720,height=750,scrollbars=1");
   textwin.locationbar.visible = false;
   textwin.focus();
   textwin.document.open();
   textwin.document.writeln( text );
   textwin.document.close();
}

function openLumiWindow(name,text) { 
   lumiwin = window.open("", name, "location=0,width=950,height=310,scrollbars=1,status=0,toolbar=0");
   lumiwin.locationbar.visible = false;
   lumiwin.focus();
   lumiwin.document.open();
   lumiwin.document.writeln( text );
   lumiwin.document.close();
}

function openNemoWindow(name,text) { 
   lumiwin = window.open("", name, "location=0,width=850,height=710,scrollbars=1,status=0,toolbar=0");
   lumiwin.locationbar.visible = false;
   lumiwin.focus();
   lumiwin.document.open();
   lumiwin.document.writeln( text );
   lumiwin.document.close();
}

function openLargeWindow(name,text) { 
   largewin = window.open("", name, "location=0,width=830,height=600,scrollbars=1");
   largewin.locationbar.visible = false;
   largewin.focus();
   largewin.document.open();
   largewin.document.writeln( text );
   largewin.document.close();
}

function openPageWindow(link,name) { 
   pagewin = window.open(link, name, "location=0,width=900,height=650,scrollbars=1"); 
}


function relpopup() {
   relwin = window.open("atlas-runquery-relnotes.html", "Change logs","location=no,width=645,height=500,scrollbars=yes");
}

function ShowMessage() {
   if ((waitDiv=document.getElementById("wait"))!=null) { 
      if (waitDiv.style){
         document.getElementById('wait').style.visibility = 'visible'
            }
   }
}

var win=null;
function NewWindow(mypage,myname,w,h,scroll,pos){
   if(pos=="random"){LeftPosition=(screen.width)?Math.floor(Math.random()*(screen.width-w)):100;TopPosition=(screen.height)?Math.floor(Math.random()*((screen.height-h)-75)):100;}
   if(pos=="center"){LeftPosition=(screen.width)?(screen.width-w)/2:100;TopPosition=(screen.height)?(screen.height-h)/2:100;}
   else if((pos!="center" && pos!="random") || pos==null){LeftPosition=0;TopPosition=20}
   settings='width='+w+',height='+h+',top='+TopPosition+',left='+LeftPosition+',scrollbars='+scroll+',locationbar=no,directories=no,status=no,menubar=no,toolbar=no,resizable=no';
   win=window.open(mypage,myname,settings);
}

