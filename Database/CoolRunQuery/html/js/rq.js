
function toggle_dq(div) {
   var hide = div.innerHTML.match(/hide/);
   var res_table = document.getElementById('resulttable');
   var fields = res_table.getElementsByTagName('table');
   for (var i=0; i<fields.length; i++) {
      if(fields[i].className != 'dqdefects') continue;
      var lines = fields[i].getElementsByTagName('tr');
      for (var l=0; l<lines.length; l++) {
         var classname = lines[l].className;
         if(classname.match(/tolerable/)) {
            if(hide) {
               lines[l].style.visibility="collapse";
            } else {
               lines[l].style.visibility="visible";
            }
         }
      }      
   }
   div.innerHTML=hide?"[show tolerable]":"[hide tolerable]";
}

function switchUTC(span) {
   if( span.className.match(/TZactive/) ) return;
   span.className = "TZactive";
   var other = document.getElementById('TZlocal');
   other.className = "TZinactive";

   for(var l=0; l<document.styleSheets.length; l++) {
      if (document.styleSheets[l].cssRules)
         crossrules=document.styleSheets[l].cssRules;
      else if (document.styleSheets[l].rules)
         crossrules=document.styleSheets[l].rules;
      for(var r=0; r<crossrules.length; r++) {
         if(crossrules[r].selectorText.match(/TZactive/)) {
            //alert(crossrules[r].selectorText);
            //crossrules[r].style.backgroundColor="green";
         }
      }
   }   
}

function switchLocal(span) {
   if( span.className.match(/TZactive/) ) return;
   span.className = "TZactive";
   var other = document.getElementById('TZutc');
   other.className = "TZinactive";
}
