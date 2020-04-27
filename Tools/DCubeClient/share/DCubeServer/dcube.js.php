<?php 
ob_start ("ob_gzhandler");
header("Content-type: text/javascript; charset: UTF-8");
header("Cache-Control: must-revalidate");
$offset = 60 * 60  * 12;
$ExpStr = "Expires: " . 
  gmdate("D, d M Y H:i:s",
	 time() + $offset) . " GMT";
header($ExpStr);
?>

/**
 * @file dcube.js.php	
 * @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMagh.edu.pl
 * @brief A set of java script functions to deal with web display
 * histograms.
 */	
//! string with URL or PHP path
var where='./';
//! DOM td element  
var el;

function createCookie(name,value,days) {
	if (days) {
	  var date = new Date();
	  date.setTime(date.getTime()+(days*24*60*60*1000));
	  var expires = "; expires="+date.toGMTString();
	}
	else var expires = "";
	document.cookie = name+"="+value+expires+"; path=/; domain=cern.ch";
}

function readCookie(name) {
	var nameEQ = name + "=";
	var ca = document.cookie.split(';');
	for(var i=0;i < ca.length;i++) {
		var c = ca[i];
		while (c.charAt(0)==' ') c = c.substring(1,c.length);
		if (c.indexOf(nameEQ) == 0) return c.substring(nameEQ.length,c.length);
	}
	return null;
}

function eraseCookie(name) {
	createCookie(name,"",-1);
}

function setGNumPicToShow( value ) {
  gNumPicToShow = parseInt(value);
}
var gNumPicToShow = readCookie('DCubeServerPlotsPerPage'); 
if ( gNumPicToShow == null ) { 
  gNumPicToShow = 20; 
} else {
  gNumPicToShow = parseInt(gNumPicToShow);
}

/**
 * @author Krzysztof Ciba 
 * @brief set status bar message 
 */
function statusBar(message){
 window.status = message;
}
/**
 * @author Krzysztof Ciba (Krzysztof.Ciba@NOSPAMagh.edu.pl)
 * @brief slof for onclick event in dir/hist div
 * @param id id for div to show/hide
 */
function showhide( id, fid, pid, src ) {

  var toShow = document.getElementById( id );
  var dirIcon = document.getElementById( fid );
  var plot = document.getElementById( pid );

  if ( toShow.style.display === "none" || 
       toShow.style.display === "" ) {
    toShow.style.display = "block";
    if ( fid !== null ) {
      dirIcon.src = where+"./images/tdirectory_o.gif";
    }
    if ( plot !== null ) {
			showPlot( pid, src );
		}
  } else {
    toShow.style.display = "none";
    if ( fid !== null ) {
      dirIcon.src = where+"./images/tdirectory_c.gif";
    }
  }
}


function showPlot(id, src) {
  var dd =document.getElementById( id );
  dd.src = loading.src;
  plot = new Image();
  plot.src = src;
  dd.src = plot.src;
  return dd;
}

/**
 * @brief slot for show/hide legend
 */
function toggleLegend( ) {
  legend = document.getElementById( "pval_legend" );		
  if ( legend.style.display == "none" )	{
    legend.style.display = "block";
  }  else {
    legend.style.display = "none";
  } 
};
function selPlot( id, src ) {
	idNum = id.split("_")[1];
	var sel = new Array("reg_", "dif_", "prx_", "pry_");
	for ( var i = 0; i < sel.length; i++ ) {
		sid = sel[i] + idNum;
		if ( id === sid ) {			
			document.getElementById(sid).childNodes[0].style.backgroundImage = selLeft;
      document.getElementById(sid).childNodes[1].style.backgroundImage = selBkg;
      document.getElementById(sid).childNodes[1].style.color = "#ffffff";
      document.getElementById(sid).childNodes[2].style.backgroundImage = selRight;
		} else {
			unsel = document.getElementById( sid );
			if ( unsel != null ) {
				unsel.childNodes[0].style.backgroundImage = pLeft;
				unsel.childNodes[1].style.backgroundImage = pBkg;
				unsel.childNodes[1].style.color = "#000000";
				unsel.childNodes[2].style.backgroundImage = pRight;
			}
		}
	}
	var dd=showPlot( "plot_" + idNum, src );
	var ap=dd.parentElement;
	if  (ap.tagName=='A' || ap.tagName=='a' ) {
	  ap.setAttribute('href','?rid='+id);
	} 
}

function selPane( id ) {
  var tids = new Array("t1", "t2", "t3", "t4");
  var cids = new Array("c1", "c2", "c3", "c4");
  cur_tid = "t" + id;
  cur_cid = "c" + id;
  for ( var i=0; i < tids.length; i++ ) {
    if ( cur_tid == tids[i] ) {
      document.getElementById(tids[i]).childNodes[0].style.backgroundImage = selLeft;
      document.getElementById(tids[i]).childNodes[1].style.backgroundImage = selBkg;
      document.getElementById(tids[i]).childNodes[1].style.color = "#ffffff";
      document.getElementById(tids[i]).childNodes[2].style.backgroundImage = selRight;
    } else {
      document.getElementById(tids[i]).childNodes[0].style.backgroundImage = pLeft;
      document.getElementById(tids[i]).childNodes[1].style.backgroundImage = pBkg;
      document.getElementById(tids[i]).childNodes[1].style.color = "#000000";
      document.getElementById(tids[i]).childNodes[2].style.backgroundImage = pRight;
    }
    if ( cur_cid != cids[i] ) {
      document.getElementById(cids[i]).style.display = "none";
    } else {
      document.getElementById(cids[i]).style.display = "block";
    }
  }
}
//! 1st shown picture gallery
var gFirstPic = 0;


// open given picture
function selPic( id) {
  var idNum = id.split("_")[1];
  var rid = "r_"+idNum;
  var dd=document.getElementById(rid);
  dd.onclick();
  if (rid!=id) {
    var ad=document.getElementById(id);
    ad.onclick();
  }
  while (dd.parentElement.tagName=='DIV') {
    dd=dd.parentElement;
    if (dd.getAttribute("style")=="display: none;") {
      dd.setAttribute("style","display: block;");
    }
  }
  return dd;
}

function gSetFirstPic( val ) {

  sum = gFirstPic + val;
  if ( sum < 0 ) { 
    gFirtsPic = 0; } 
  else if ( sum > picTable.length ) { 
    gFirstPic = sum - val;
  } else {
    gFirstPic = sum;
  }
}


//! getNext
function gGetNext() {
  gSetFirstPic( gNumPicToShow );
  to = gFirstPic+gNumPicToShow;

  if ( gFirstPic >= picTable.length ) {
    gGetEnd();
  } 
    
  if ( gFirstPic < picTable.length ) {
    
    if ( to <= picTable.length ) 
      gShow( picTable.slice(gFirstPic, to), picInfoTable.slice(gFirstPic, to) );
    
    if ( to > picTable.length )
      gShow( picTable.slice( gFirstPic ) , picInfoTable.slice( gFirstPic ) );
  
    gTitle( gFirstPic );
    
  }
} 
//! get previous
function gGetPrev() {
  gSetFirstPic( -gNumPicToShow );

  if ( gFirstPic < 0 ) {
    gGetBeg();
  } else {
  to = gFirstPic+gNumPicToShow;

    if ( to > picTable.length ) {
      gShow(picTable.slice(gFirstPic), picInfoTable.slice(gFirstPic));
    } else {
      gShow(picTable.slice(gFirstPic, to), picInfoTable.slice(gFirstPic, to));
    }
  }
  gTitle( gFirstPic );
}
//! get end
function gGetEnd() {
  
  var gLast = ( picTable.length/gNumPicToShow ) * gNumPicToShow - ( picTable.length%gNumPicToShow );
 
  if ( gLast == picTable.length ) gLast -= gNumPicToShow;
  gFirstPic = gLast;
  to = gLast+gNumPicToShow;
  
  
  if ( to > picTable.length ) {
    gShow( picTable.slice(gLast), picInfoTable.slice(gLast));
  } 
  
  if ( to <= picTable.length ) {
    gShow( picTable.slice(gLast, to), picInfoTable.slice(gLast, to ) );
  } 
  gTitle( gLast );
}

//! get begin
function gGetBeg() {
  var toShow = new Array();
  gFirstPic = 0;
  to = gFirstPic+gNumPicToShow;
  if (to > picTable.length ) {
    gShow( picTable.slice(gFirstPic), picInfoTable.slice(gFirstPic));
  } else {
    gShow( picTable.slice(gFirstPic, to), picInfoTable.slice(gFirstPic, to));
  }
  gTitle( gFirstPic );
}

function gOnLoad( show ) {
  
  if ( show === true ) {
    document.getElementById("loading").display = "";
  } else {
    document.getElementById("loading").display = "none";
  }
    
}

//! show pictures
function gShow( arr, arrInfo ) {
  
  //alert("arr.length" + arr.length );

  for ( i = 0; i < gNumPicToShow; i++ ) {
    var img = document.getElementById("gi_"+i);
    img.src = loading.src;
  }


  for ( i = 0; i < gNumPicToShow; i++ ) {
    var imgDiv = document.getElementById("gp_"+i);
    var img = document.getElementById("gi_"+i);
    var imgInfo = document.getElementById("gs_"+i)
   
     if ( i < arr.length ) {
      if ( arr[i] != '' && arrInfo[i] != '' ) {
	imgDiv.style.display = 'block';
	img.src = arr[i];
	img.style.display = 'block';
	imgInfo.innerHTML = arrInfo[i];
      } else if ( arrInfo[i] != '' && arr[i] == '' ) {
	imgDiv.style.display = 'block';
	img.src = '';
	img.style.display= 'none';
	imgInfo.innerHTML = arrInfo[i];
      } else {
	imgDiv.style.display = 'none';
	img.src = '';
	img.style.display = 'none';
      }
    } else {
      imgDiv.style.display = 'none';
      img.src = '';
      img.style.display = 'none';
    }
  }
  

}

//! select pictures 
function gOnSelect( val ) {
  gFirstPic = parseInt(val);
  to = gFirstPic+gNumPicToShow;
  if (to > picTable.length ) {
    gShow( picTable.slice(gFirstPic), picInfoTable.slice(gFirstPic));
    
  } else {
    gShow( picTable.slice(gFirstPic, to), picInfoTable.slice(gFirstPic, to) ) ;
  }
  gTitle( gFirstPic );
}

//! set title
function gTitle( from ) {

  gtt = document.getElementById('gtt');
  gbt = document.getElementById('gbt');
  to = from + gNumPicToShow;

  if ( to > picTable.length ) to = picTable.length;

  if ( from == to ) {
    gtt.innerHTML = "Last plot (Total: "+ picTable.length+" plots)";
    gbt.innerHTML = "Last plot (Total: "+ picTable.length+" plots)";
  } else {
    gtt.innerHTML = "Plots from " + (from+1) + " to " + to + " (Total "+ picTable.length+" plots)";
    gbt.innerHTML = "Plots from " + (from+1) + " to " + to + " (Total "+ picTable.length+" plots)";
  }
}


function setupGallery( value ) {
  createCookie( 'DCubeServerPlotsPerPage', value, 3650 );
  setGNumPicToShow( value );
  document.location.reload();
}

function setupPane( value ) {
  createCookie( 'DCubeServerFirstPane', value, 3650 );
  setPane(value);
}