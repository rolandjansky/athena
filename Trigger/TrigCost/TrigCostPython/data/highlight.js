var show_flags = new Array() ;
show_flags.push("ps"         ) ;
show_flags.push("pt"         ) ;
show_flags.push("diff"       ) ;
show_flags.push("isZero"     ) ;
show_flags.push("_empty"     ) ;
show_flags.push("_firstempty") ;
show_flags.push("_cosmic"    ) ;
show_flags.push("_unpaired"  ) ;

function init(){
  get('nSigma_threshold'). addEventListener('keyup', highlight_nSigma_percent, false) ;
  get('nSigma_threshold'). addEventListener('focus', highlight_nSigma_percent, false) ;
  get('percent_threshold').addEventListener('keyup', highlight_nSigma_percent, false) ;
  get('percent_threshold').addEventListener('focus', highlight_nSigma_percent, false) ;
  
  get('diff_threshold') .addEventListener('keyup', highlight_diffs , false) ;
  get('diff_threshold') .addEventListener('focus', highlight_diffs , false) ;
  get('ratio_threshold').addEventListener('keyup', highlight_ratios, false) ;
  get('ratio_threshold').addEventListener('focus', highlight_ratios, false) ;
  
  parseAndExecuteArguments() ;
  
  for(var i=0 ; i<show_flags.length ; i++){
    if(get_value("show_"+show_flags[i])==true){
      var elements = getElementsByClass("rate_" + show_flags[i]) ;
      var className = show_flags[i]+'_hide' ;
      for(var j=0 ; j<elements.length ; j++){
        removeClassName(elements[j], className) ;
      }
    }
  }
}

// Functions used for getting values and setting values
function get_value(name){
  if(name=="show_ps"         ) return show_ps         ;
  if(name=="show_pt"         ) return show_pt         ;
  if(name=="show_diff"       ) return show_diff       ;
  if(name=="show_isZero"     ) return show_isZero     ;
  if(name=="show__empty"     ) return show_empty      ;
  if(name=="show__firstempty") return show_firstempty ;
  if(name=="show__cosmic"    ) return show_cosmic     ;
  if(name=="show__unpaired"  ) return show_unpaired   ;
}
function set_value(name, value){
  if(name=="show_ps"         ) show_ps         = value ;
  if(name=="show_pt"         ) show_pt         = value ;
  if(name=="show_diff"       ) show_diff       = value ;
  if(name=="show_isZero"     ) show_isZero     = value ;
  if(name=="show__empty"     ) show_empty      = value ;
  if(name=="show__firstempty") show_firstempty = value ;
  if(name=="show__cosmic"    ) show_cosmic     = value ;
  if(name=="show__unpaired"  ) show_unpaired   = value ;
}

// Helper functions to get elements
function get(id){ return document.getElementById(id) ; }
function get_all_elements(){
  var all_elements = new Array() ;
  all_elements = document.getElementsByTagName("*") ;
  return all_elements ;
}
function getElementsByClass(theClass){
  var classElements = new Array() ;
  var all_elements = get_all_elements() ;
  var j = 0 ;
  for(i=0; i<all_elements.length; i++){
    if(all_elements[i].className.match(theClass)){
      classElements[j] = all_elements[i] ;
      j++ ;
    }
  }
  return classElements ;
}

// Trim functions taken from http://www.webtoolkit.info/javascript-trim.html
function trim(str, chars) {
  return ltrim(rtrim(str, chars), chars);
}
function ltrim(str, chars) {
  chars = chars || "\\s";
  return str.replace(new RegExp("^[" + chars + "]+", "g"), "");
}
function rtrim(str, chars) {
  chars = chars || "\\s";
  return str.replace(new RegExp("[" + chars + "]+$", "g"), "");
}

// Add and remove className
function addClassName(element, name){
  var classes = element.className.split(' ') ;
  for(var i=0 ; i<classes.length ; i++){
    if(classes[i]==name) return ; // Don't change anything if class name matches
  }
  var finalClassName = element.className + ' ' + name ;
  finalClassName = trim(finalClassName) ;
  element.className = finalClassName ;
}
function removeClassName(element, name){
  var finalClassName = '' ;
  var classes = element.className.split(' ') ;
  for(var i=0 ; i<classes.length ; i++){
    if(classes[i]!=name) finalClassName = finalClassName + ' ' + classes[i] ;
  }
  finalClassName = trim(finalClassName) ;
  element.className = finalClassName ;
}

// Show or hide different columns (or rows) when the user clicks on the checkboxes
function toggle_show(name){
  var className = name + '_hide' ;
  var varName   = "show_" + name ;
  
  // Toggle value
  var flag = (get_value(varName)) ? false : true ;
  set_value(varName, flag) ;

  // show/hide columns
  var elements = getElementsByClass("rate_" + name) ;
  for(var i=0 ; i<elements.length ; i++){
    if(get_value(varName)==false){
      addClassName(elements[i], className) ;
    }
    else{
      removeClassName(elements[i], className) ;
    }
  }
  
  // Change the checkbox last to show the change has been applied
  get('checkbox_show_'+name).checked = flag ;
  updateURLField() ;
}

// Show or hide different rows when the user clicks on the checkboxes
function toggle_show_row(name){
  var className = name + '_hide' ;
  var varName   = "show_" + name ;
  
  // Toggle value
  var flag = (get_value(varName)) ? false : true ;
  set_value(varName, flag) ;

  // show/hide columns
  var elements = getElementsByClass("chain_name") ;
  var nHide = 0 ;
  var nShow = 0 ;
  for(var i=0 ; i<elements.length ; i++){
    var inner = elements[i].innerHTML.toLowerCase()
    if(inner.match(name)!=null){
      if(get_value(varName)==false){
        nHide++ ;
        addClassName(elements[i].parentNode, className) ;
      }
      else{
        removeClassName(elements[i].parentNode, className) ;
        nShow++ ;
      }
    }
  }
    
  // Change the checkbox last to show the change has been applied
  get('checkbox_show_'+name).checked = flag ;
  updateURLField() ;
}

function highlight_diffs(){
  reset_highlights('rate_diff') ;
  var threshold = get('diff_threshold').value
  var elements = getElementsByClass('rate_diff') ;
  for(var i=0 ; i<elements.length ; i++){
    var element = elements[i] ;
    var difference = parseFloat(element.innerHTML) ;
    if(Math.abs(difference)>threshold){
      element.className = element.className + ' highlight' ;
    }
  }
  get('bool_highlight_diffs').className = 'black' ;
  updateURLField() ;
}

function highlight_ratios(){
  reset_highlights('rate_ratio') ;
  var threshold = get('ratio_threshold').value
  var elements = getElementsByClass('rate_ratio') ;
  for(var i=0 ; i<elements.length ; i++){
    var element = elements[i] ;
    var text = element.innerHTML ;
    var value = parseFloat(text) ;
    if(Math.abs(value)>threshold || Math.abs(value)<1/threshold){
      if(value!=0) element.className = element.className + ' highlight' ;
    }
  }
  get('bool_highlight_ratios').className = 'black' ;
  updateURLField() ;
}

function highlight_nSigma_percent(){
  //reset_highlights('rate_ratio') ;
  var n = get('nSigma_threshold').value
  var p = get('percent_threshold').value
  var elements = getElementsByClass('rate_ratio') ;
  for(var i=0 ; i<elements.length ; i++){
    var hide_row = true ;
    var element = elements[i] ;
    var text = element.innerHTML ;
    var value = parseFloat(text.split('+-')[0]) ;
    var error = parseFloat(text.split('+-')[1]) ;
    var diff = Math.abs(value-1.0) ;
    if(error>0){
      if(diff/error>n && diff>0.01*p) hide_row = false ;
    }
    else{
      if(diff>0.01*p && value!=0) hide_row = false ;
    }
    if(hide_row==true) element.parentNode.className='hidden' ;
  }
  get('bool_highlight_nSigma_percent').className = 'black' ;
  updateURLField() ;
}

// Reset functions
function reset_nSigma_percent(){
  var table = get('chain_table') ;
  var trs = table.getElementsByTagName('tr') ;
  for(var row=1 ; row<trs.length ; row++){
    trs[row].className = '' ;
  }
  get('bool_highlight_nSigma_percent').className = 'white' ;
  updateURLField() ;
}

function reset_diffs(){
  reset_highlights('rate_diff') ;
  get('bool_highlight_diffs').className = 'white' ;
  updateURLField() ;
}

function reset_ratios(){
  reset_highlights('rate_ratio') ;
  get('bool_highlight_ratios').className = 'white' ;
  updateURLField() ;
}

function reset_highlights(theClass){
  var counter = 0 ;
  var all_elements = get_all_elements() ;
  for(var i=0 ; i<all_elements.length ; i++){
    if(all_elements[i].className.match(theClass) ){
      all_elements[i].className = theClass ;
      counter++ ;
    }
  }
}

// Functions for parsing url arguments
// Aidan Randle-Conde <aidan@cern.ch>
function getUrlArgument(name){
  name = name.replace(/[\[]/, "\\\]").replace(/[\]]/, "\\\]") ;
  var regexS  = "[\\?&]"+name+"=([^&#]*)" ;
  var regex   = new RegExp(regexS) ;
  var results = regex.exec( window.location.href ) ;
  if(results == null){ return "" ; }
  else { return results[1] ; }
}
function execute(fname){
  if(fname=='ratio')   highlight_ratios()         ;
  if(fname=='diff')    highlight_diffs()          ;
  if(fname=='nSigma')  highlight_nSigma_percent() ;
  if(fname=='percent') highlight_nSigma_percent() ;
}
function applyArgument(name){
  if(getUrlArgument(name + '_threshold') != ""){
    threshold = parseFloat(getUrlArgument(name + '_threshold')) ;
    get(name+'_threshold').value = threshold ;
    execute(name) ;
  }
}
function parseAndExecuteArguments(){
  applyArgument('ratio'  ) ;
  applyArgument('diff'   ) ;
  applyArgument('nSigma' ) ;
  applyArgument('percent') ;
  if(getUrlArgument('regex') != ''){
    get('search').value = getUrlArgument('regex') ;
    get('search').focus() ;
    get('search').blur() ;
  }
  if(getUrlArgument('collapse')==1){
    get('collapsable').className = 'collapsed' ;
    get('expandCollapse').value  = 'Expand' ;
  }
  if(getUrlArgument('collapse')!='' && getUrlArgument('collapse')==0){
    get('collapsable').className = '' ;
    get('expandCollapse').value  = 'Collapse' ;
  }
  for(var i=0 ; i<show_flags.length ; i++){
    if(getUrlArgument('show_'+show_flags[i])==1){ set_value('show_'+show_flags[i], true) ; }
  }
  updateURLField() ;

  // Set consistent state for checkboxes
  for(var i=0 ; i<show_flags.length ; i++){
    if(get_value('show_'+show_flags[i])==true) get("checkbox_show_"+show_flags[i]).checked = "true" ;
  }
}

function updateURLField(){
  var url = window.location.href.split('?')[0] + '?' ;
  if(get('bool_highlight_diffs').className=='black'){
    url += 'diff_threshold=' + get('diff_threshold').value + '&';
  }
  if(get('bool_highlight_ratios').className=='black'){
    url += 'ratio_threshold=' + get('ratio_threshold').value + '&' ;
  }
  if(get('bool_highlight_nSigma_percent').className=='black'){
    url += 'nSigma_threshold='  + get('nSigma_threshold').value   + '&' ;
    url += 'percent_threshold=' + get('percent_threshold').value + '&' ;
  }
  if(get('collapsible_filter').className=='collapsed'){ url += 'collapse=1&' ; }
  if(get('search')){
    if(get('search').value!=''){ url += 'regex=' + get('search').value + '&' ; }
  }
  for(var i=0 ; i<show_flags.length ; i++){
    if(get_value("show_"+show_flags[i])==true){ url += 'show_'+show_flags[i]+'=1&' ; }
  }
  
  get('url_link').href            = url ;  
  get('url_link').innerHTML       = url ;
  get('url_link_small').href      = url ;
  get('url_link_clear').innerHTML = url.split('?')[0] ;
  get('url_link_clear').href      = url.split('?')[0] ;
}

// Expand or collapse the filter settings
function expandCollapse(divid, buttonid){
  var div = get(divid) ;
  if(div.className==''){
    div.className = 'collapsed' ;
    get(buttonid).value="Expand" ;
  }
  else{
    div.className = '' ;
    get(buttonid).value="Collapse" ;
  }
  updateURLField() ;
}
