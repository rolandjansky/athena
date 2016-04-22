<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN"
"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html>
<?php include("https://gartoni.web.cern.ch/gartoni/menu.php"); ?>
<script> function getSelectedValues(select) { var result = []; var options = select && select.options; var opt; for (var i=0, iLen=options.length; i<iLen; i++) { opt = options[i]; if (opt.selected) { result.push(opt.value || opt.text); } } return result; } </script>
<div id="center">
  <div id="center1" style="float:left; margin:30px 0px 0px 30px; width:100%;">
  PAGE_HERE
  </div>
  <div id="center2" style="float:left; margin:0; width=100%;">
    <figure id="myfigure"></figure>
  </div>
</div>
<div id="footer">
	<p></p>
</div>
</body>
</html>

