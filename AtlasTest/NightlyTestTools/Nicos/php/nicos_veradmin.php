<?php

$tmm = get_current_user();
$file_addpkg = "temp/addpkg." . $tmm;
if ( "${_SERVER['REQUEST_METHOD']}" == "GET" )
{
$addpkg_pointer = fopen ($file_addpkg, "w");
}
else
{
$addpkg_pointer = fopen ($file_addpkg, "a");
}
fclose ($addpkg_pointer);

?>

<HTML>
<HEAD>
<head><title>NICOS version administrator page</title>
<style TYPE='text/css'> 

body 
{
  color: black;
  link: navy;
  vlink: maroon;
  alink: tomato;
  background: cornsilk;
} 

TABLE.header 
{
  background: #99CCFF;
  color: #CC0000;
} 

TD.a 
{
  color:#FFCC00;
  background-color:#000000;
  font-color:#FFCC00;
}
A.a
{
  color:#FFCC00;
  background-color:#000000;
  foreground-color:#ffcc00;
  font-color:#FFCC00;
}
A.b
{
  color:#FFCC00;
  background-color:#0069b3;
  foreground-color:#ffcc00;
  text-decoration:none;
  font-color:#FFCC00;
}
A 
{
  color:white;
  text-decoration:none;
}
A:hover
{
  color:#FFCC00;
  text-decoration:underline;
}
A.menu
{
  color:black; 
  text-decoration:underline;
}
A:hover.menu 
{
  color:#FFFF00;
  text-decoration:underline;
}
A.esp 
{
  color:#FF3333; 
  text-decoration:underline;
}
A:hover.esp 
{
  color:#33FFFF;
  text-decoration:none;
}

A.div 
{
  color:#FFCC00; 
  text-decoration:none;
}
A:hover.div 
{
  color:#FF3333;
  text-decoration:none;
}
TD.b 
{
  color:#FFCC00;
  background-color:#0069B3;
  font-weight: bold;
  font-style: italic;
}
B.b
{
  color:black;
  background-color:skyblue;
}
I.b
{
  color:blue;
  background-color:skyblue;
}
TD.c
{
  color: cornsilk;
  background-color: black;
}
P.a
{
  color:#FFCC00;
  background-color:#ffffff;
  font-color:#FFCC00;
}
BLOCKQUOTE.b
{
  color:#FFCC00;
  background-color:red;
  font-color:#FFCC00;
}
B.a
{
  color:#FFCC00;
  background-color:#000000;
  font-color:#FFCC00;
}
B.c
{
  color:maroon;
  background-color:#99CCFF;
  font-color:maroon;
}
FORM.a
{
  color:#FFCC00;
  background-color:#000000;
  font-color:#FFCC00;
}
P.c
{
  color:#FFCC00;
  background-color:black;
}
INPUT
{
  color:#000000;
  background-color:#DDDDDD;
  font-weight: bold;
}
INPUT.del
{
  color:red;
  background-color:#0069B3;
  font-weight: bold;
}
INPUT.add
{
  color:maroon;
  background-color:#99CCFF;
  font-size:12px;
  font-family:verdana;
}
SELECT
{
  color:#000000;
  background-color:#DDDDDD;
}
TEXTAREA
{
  color:#FFCC00;
  background-color:#0069B3;
  font-size:11px;
  font-family:verdana;
}

</style>

</head>

<BODY class=body >
<table class=header border=0 cellpadding=5 cellspacing=0 width="100%">
<tr><td bgcolor="#CCCFFF" align=right><font size="-1">
NICOS version 0.2
</font></td></tr>
<tr><td bgcolor="#CCCFFF" align=center>
<br><EM><B><BIG>NICOS (NIghtly COntrol System): Version Administartor
<br><br></EM></B></BIG></td></tr>
<tr><td align=center>
<br><BIG><B>Project: POOL<br></BIG>
<font size="-1"> 
Location: /afs/usatlas.bnl.gov/project/nightly/POOL
</font><br>
</td></tr>

</table>

<P>

<?PHP

include "nicos_simpledb.php";
$file="nicos_veradmin.db";
$conn = connect_pb ($file);
$my_lines = select_pb ($conn, "id","x>-1");
$amountofrows = count_pb ($my_lines);
$package = $_POST['package'];
$act = $_POST['act'];
$label_add="";

$pkgname = $_GET['pkgname'];
if (!(isset($pkgname))){
$pkgname = $_POST['pkgname'];
}
$vername = $_GET['vername'];
if (!(isset($vername))){
$vername = $_POST['vername'];
}
$autname = $_GET['autname'];
if (!(isset($autname))){
$autname = $_POST['autname'];
}

function pack_selected (){
  global $package;
  global $amountofrows;
  global $my_lines;  
  $event = 0;
      for($i=0; $i < $amountofrows; $i++)
       {
       $pkg = $my_lines["id"][$i];
       $ver = $my_lines["version"][$i];
       $adr = $my_lines["author"][$i];
       if ($package == $pkg) {
        ?> <td> 
        <INPUT Type='text' name='vers' value='<? echo "$ver"; ?>'> 
        </td>
        <td> 
        <INPUT Type='text' name='adre' value='<? echo "$adr"; ?>'>
        </td> <?  
       $event = 1;
       break;
       } 
       }
       if ( $event == 0 )
       {
        ?> <td> 
        <INPUT Type='text' name='vers' value=" "> 
        </td>
        <td> 
        <INPUT Type='text' name='adre' value=" ">
        </td> <?  
       }
}

function add_pack(){
  $pkgname = $_POST['pkgname'];
  $vername = $_POST['vername'];
  $autname = $_POST['autname'];
  global $file;
  global $os;
  global $file_addpkg;
  if (${pkgname})
  {
         $addpkg_pointer = fopen($file_addpkg, "r");
         $addpkg_data = file($file_addpkg);
         fclose ($addpkg_pointer);   
         $GetLine = explode("\t",$addpkg_data[0]);

if( count($GetLine) >= 2 )
{ 
$v2 = "$GetLine[1]";
}
else
{ 
$v2 = "$GetLine[0]";
}
$v2="$GetLine[0]";
$v3="$GetLine[1]";
$vvvr = count($GetLine) - 4;
$vvvf = count($GetLine) - 3;
$vvvg = count($GetLine) - 2;
$vvvh = count($GetLine) - 1;
$v3="$GetLine[$vvvf]";
$v4="$GetLine[$vvvg]";
$v5="$GetLine[$vvvh]";
$v6="$GetLine[$vvvr]";

        if ( $pkgname != $v3 && $pkgname != $v4 && $pkgname != $v5 && $pkgname != $v6 )
           {
           insert_pb($file,"${pkgname};${vername};${autname};");
           $addpkg_pointer = fopen($file_addpkg, "a");
           fwrite ($addpkg_pointer, "${pkgname}\t");
           fclose ($addpkg_pointer);   
           }
  }
}

function deledit_pack(){
  global $act; 
  $pkgname = $_POST['package'];
  $vername = $_POST['vers'];
  $autname = $_POST['adre'];
  global $file;
  if ($act == "e")
    { 
  update_pb($file,"id","x == $pkgname","version","$vername" );
  update_pb($file,"id","x == $pkgname","author","$autname" );
    }
  if ($act == "d")
    { 
  delete_pb($file,"id","x == $pkgname");
  delete_pb($file,"id","x == ''");
    }
}
 

?> 

<table width=100%>
 <tr>
    <td>
       <center> 
       <BIG><b class=c>&nbsp;&nbsp;Current versions&nbsp;&nbsp;</b></BIG><br> 
         <form action="<?= $_SERVER['PHP_SELF'] ?>" method="POST">

           <table> 
             <tr> 
               <td class='b'><center>Package</center></td> 
               <td class='b'><center>Version</center></td> 
               <td class='b'><center>Author</center></td> 
               <td colspan=2 class='b'><center>Actions</center></td> 
             </tr> 
               <td> 
                <select name="package" onchange="this.form.submit();"> 
<option value="" selected>Choose one</option>
<?PHP
for ($i=0;$i< $amountofrows;$i++)
{
$id = $my_lines["id"][$i];
if ( $id != "" )
{
if ( $package == $id ) 
{ print "<option value='$id' selected='selected'>$id</option>";}
else
{ print "<option value='$id'>$id</option>";}
}
}
?>
                </select> 
              </td> 

<?php pack_selected(); ?>

              <td> 
                <select name='act' size="2">
                <option value="e"> Edit</option>
                <option value="d"> Delete</option>
              </select>
              </td> 
              <td> 
                <input type='button' name='Perf' value='Perform' onclick='<?php deledit_pack(); ?> this.form.submit(); var inf = "To update the form please click on Refresh button or choose another package"; alert(inf); ' >  
                <input type='submit' value='Refresh'> 
              </td> 
            </tr> 
          </table> 
        </form> 
      </center> 
    </td> 
  </tr> 
 </table> 

<BR><BR>

<table width=100%>
 <tr>
    <td>
       <center> 
       <BIG><b class=c>&nbsp;&nbsp;Add new package&nbsp;&nbsp;</b></BIG><br> 

         <form action="<?= $_SERVER['PHP_SELF'] ?>" method="POST" name="formx" >

           <table> 
             <tr> 
               <td class='b'><center>Package</center></td> 
               <td class='b'><center>Version</center></td> 
               <td class='b'><center>Author</center></td> 
               <td colspan=1 class='b'><center>Actions</center></td> 
             </tr> 
               <td> 
                <input type='text' name='pkgname' value="">
               </td> 
               <td> 
                <input type='text' name='vername' value="">
               </td> 
               <td> 
                <input type='text' name='autname' value="">
               </td> 

               <td> 
                <input type='button' value='Add package' onclick='this.form.submit(); <?php add_pack(); ?> form1 = document.forms["formx"]; var info = "package " + form1.pkgname.value + " was added (version " + form1.vername.value + ", author " + form1.autname.value + " )"; alert(info);' >     
                <input type='hidden' name='os' value="<?= $_POST['pkgname'] ?>" > 
               </td> 
            </tr> 
          </table> 
        </form> 

      </center> 
    </td> 
  </tr> 
 </table> 

<BR>

</body>
</HTML>

