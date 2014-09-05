# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def compRatesHead():

  string = '''<style type="text/css">
table, th{ border : 1px solid black ; }
table{ margin : auto ; }
th
{
  padding : 10px ;
  background-color : gray ;
  color : white ;
  font-weight : normal ;
  font-family : arial, sans-serif ;
}
td
{
  padding-left  : 15px ;
  padding-right : 15px ;
  font-size:10pt ;
}
td.rate, td.rate_ratio, td.rate_diff, td.chain_name{ font-size:10pt ; }

/* Colors */
td.rate      { background-color : #ffeeee ; }
td.rate_ratio{ background-color : #eeffee ; }
td.rate_diff { background-color : #eeeeff ; }

/* Showing/hiding flags for various filter functions */
.white{ color:white ; }
.black{ color:black ; }

a.link{ text-decoration : underline ; }
tr.hidden{ display : none ; }

div.collapsed{ display : none ; }
div.collapsable, div#collapsible_snippet
{
  background-color : #eeeeee ;
  width:810px ;
  margin:auto ; margin-bottom:20px ;
}
table.filter_wrapper
{
  border : 0px solid black ;
  width : 900px ;
  margin : auto ;
  border-collapse : collapse ;
}
td.filter_wrapper{ padding : 5px ; }
table.filter_12col, table.filter_6col, table.filter_4col, table.filter_3col
{
  background-color : #ffffdd ;
  background-color : #ffffff ;
  border : 1px solid black ;
}
td.filter_main_12col, td.filter_main_6col, td.filter_4col, td.filter_3col, td.filter_status{ padding : 0px ; }
table.filter_12col{ width : 890px ; }
table.filter_6col { width : 440px ; }
table.filter_4col { width : 290px ; }
table.filter_3col { width : 215px ; }
td.filter_main_12col{ width : 860px ; }
td.filter_main_6col { width : 410px ; }
td.filter_main_4col { width : 260px ; }
td.filter_main_3col { width : 185px ; }
td.filter_status{ width : 30px ; }
input.input_filter{ width : 50px ; }

td.highlight{ border : 2px solid red ; }

tr.regex_hide, tr.isZero_hide, tr._empty_hide, tr._firstempty_hide, tr._cosmic_hide, tr._unpaired_hide { display : none ; }
.diff_hide, .ratio_hide , .ps_hide, .pt_hide, .isZero_hide{ display : none ; }
th.rate_isZero, td.rate_isZero{ display : none ; } /* Must override the final column settings.  The final column is only used for bookkeeping */

div.h2_wrapper{
  width : 600px ;
  margin : auto ;
}

div#snippet{
  width : 600px ;
  margin : auto ;
}

</style>

<script type="text/ecmascript">
var difference_threshold = 1e6 ;
var      ratio_threshold = 1e6 ;
var     nSigma_threshold = 1e6 ;
var    percent_threshold = 1e6 ;
</script>
</head>
<body onload="init()">

<div class="h2_wrapper"><h2 style="display:inline">Search and filter settings</h2>
<input type="submit" id="expandCollapse_filter" onclick="expandCollapse('collapsible_filter', 'expandCollapse_filter')" value="Expand"/>
<a href="" id="url_link_small">URL</a>
</div>

<div id="collapsible_filter" class="collapsed">

<table class="filter_12col">
  <tbody>
    <tr>
      <td style="width:100px">URL</td>
      <td style="width:700px"><a id="url_link" href=""></a></td>
    </tr>
    <tr>
      <td style="width:100px">Clear all</td>
      <td style="width:700px"><a id="url_link_clear" href=""></a></td>
    </tr>
  </tbody>
</table>

<table class="filter_wrapper">
  <tbody>
     <tr>
      <td class="filter_wrapper" colspan="12">
        <table class="filter_12col">
          <tbody>
            <tr>
	      <td class="filter_status">
	        <span id="bool_highlight_nSigma_percent" class="white">&#x2713;</span>
	      </td>
	      <td class="filter_main_12col">
	        Show only chains where the difference is greater than <input class="input_filter" type="text" id="nSigma_threshold" value="2"/> &sigma; and the difference is greater than <input class="input_filter" type="text" id="percent_threshold" value="10"/> %
	      </td>
	    </tr>
	  </tbody>
        </table>
      </td>
   </tr>
    <tr>
      <td class="filter_wrapper" colspan="6">
        <table class="filter_6col">
          <tbody>
            <tr>
	      <td class="filter_status">
	        <span id="bool_highlight_diffs" class="white">&#x2713;</span>
	      </td>
	      <td class="filter_main_6col">
	        Highlight differences greater than <input class="input_filter" type="text" value="1.0" id="diff_threshold"/>Hz
	      </td>
	    </tr>
	  </tbody>
        </table>
      </td>
      <td class="filter_wrapper" colspan="6">
        <table class="filter_6col">
          <tbody>
            <tr>
	      <td class="filter_status">
	        <span id="bool_highlight_ratios" class="white">&#x2713;</span>
	      </td>
	      <td class="filter_main_6col">
	        Highlight ratios greater than <input class="input_filter" type="text" value="2" id="ratio_threshold"/>
	      </td>
	    </tr>
	  </tbody>
        </table>
      </td>
    </tr>
    <tr>
      <td class="filter_wrapper" colspan="3">
        <table class="filter_3col">
          <tbody>
            <tr>
	      <td class="filter_status">
	      </td>
	      <td class="filter_main_3col">
	        <input type="checkbox" id="checkbox_show_diff"   onclick="toggle_show('diff')"/> Show differences<br />
	      </td>
	    </tr>
	  </tbody>
        </table>
      </td>
      <td class="filter_wrapper" colspan="3">
        <table class="filter_3col">
          <tbody>
            <tr>
	      <td class="filter_status">
	      </td>
	      <td class="filter_main_3col">
	        <input type="checkbox" id="checkbox_show_ps" onclick="toggle_show('ps')"/> Show prescales<br />
	      </td>
	    </tr>
	  </tbody>
        </table>
      </td>
      <td class="filter_wrapper" colspan="3">
        <table class="filter_3col">
          <tbody>
            <tr>
	      <td class="filter_status">
	      </td>
	      <td class="filter_main_3col">
	        <input type="checkbox" id="checkbox_show_pt" onclick="toggle_show('pt')"/> Show passthroughs<br />
	      </td>
	    </tr>
	  </tbody>
        </table>
      </td>
      <td class="filter_wrapper" colspan="3">
        <table class="filter_3col">
          <tbody>
            <tr>
	      <td class="filter_status">
	      </td>
	      <td class="filter_main_3col">
	        <input type="checkbox" id="checkbox_show_isZero" onclick="toggle_show('isZero')"/> Show zero rates
	      </td>
	    </tr>
	  </tbody>
        </table>
      </td>
   </tr>




   <tr>
      <td class="filter_wrapper" colspan="3">
        <table class="filter_3col">
          <tbody>
            <tr>
	      <td class="filter_status">
	      </td>
	      <td class="filter_main_3col">
	        <input type="checkbox" id="checkbox_show__empty"   onclick="toggle_show_row('_empty')"/> Show "_empty" chains<br />
	      </td>
	    </tr>
	  </tbody>
        </table>
      </td>
      <td class="filter_wrapper" colspan="3">
        <table class="filter_3col">
          <tbody>
            <tr>
	      <td class="filter_status">
	      </td>
	      <td class="filter_main_3col">
	        <input type="checkbox" id="checkbox_show__firstempty" onclick="toggle_show_row('_firstempty')"/> Show "_firstempty" chains<br />
	      </td>
	    </tr>
	  </tbody>
        </table>
      </td>
      <td class="filter_wrapper" colspan="3">
        <table class="filter_3col">
          <tbody>
            <tr>
	      <td class="filter_status">
	      </td>
	      <td class="filter_main_3col">
	        <input type="checkbox" id="checkbox_show__cosmic" onclick="toggle_show('_cosmic')"/> Show "_cosmic" chains<br />
	      </td>
	    </tr>
	  </tbody>
        </table>
      </td>
      <td class="filter_wrapper" colspan="3">
        <table class="filter_3col">
          <tbody>
            <tr>
	      <td class="filter_status">
	      </td>
	      <td class="filter_main_3col">
	        <input type="checkbox" id="checkbox_show__unpaired" onclick="toggle_show('_unpaired')"/> Show "_unpaired" chains<br />
	      </td>
	    </tr>
	  </tbody>
        </table>
      </td>
   </tr>
  </tbody>
</table>'''
  return string
