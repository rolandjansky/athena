#!/afs/cern.ch/atlas/offline/external/tcsh/6.15.00/i386_linux26/bin/tcsh -f

set my_cwd=`pwd`

set home_dir='/afs/cern.ch/user/s/seuster/scratch0/ValGrind/scripts'

set jobOptions='../rdotoesdnotrigger.py'
set jobOptions='JetRec_RTT2_jobOptions.py'

set jobName="NONE"
set jobName=`pwd`
set jobName=`dirname $jobName`
set jobName=`basename $jobName`

pwd
ls -l

if ( -e veto.log ) then
 echo "job was vetoed, just write short html file"
cat <<EOF > ./Report.html
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
     "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<title>Valgrind Report for $jobName release $AtlasVersion of project $AtlasProject</title>
</head>
<body>
<h1>Valgrind Report for:</h1>
 <h3>JOB WAS VETOED !</h3>
</html>
EOF
 exit 0
endif

mkdir -p processed

set logfile=`grep -l 'bin/athena.py' valgrind.out.process.*`
set pid_vgj=$logfile:e

echo "Logfile : $logfile"
echo "PID     : $pid_vgj"

if ( "$logfile" == "" ) then
 echo "could not find any logfile !!"
 exit (0)
endif

echo "0 0" > mem
@ n = 5
foreach i ( `grep $pid_vgj top.log | gawk ' { print $5 }' | sed "s/m/\\*1024/g;s/g/\\*1048576/g"` )
echo -n " $n " >> mem
# expr ` echo "$i" | tr \, \ ` >> mem
perl -l -e " print $i " >> mem
@ n += 5
end

echo "0 0" > cpu
@ n = 5
foreach i ( `grep $pid_vgj top.log | gawk ' { print $9 }'` )
echo " $n $i" >> cpu
@ n += 5
end

# /afs/cern.ch/atlas/offline/external/LCGCMT/LCGCMT_56/InstallArea/i686-slc4-gcc34-opt/bin/
root -l -b -q memConsumption.C
mv mem.gif mem.eps processed/

set jobOptions=`grep '\.py$' $logfile | grep -v "bin/athena.py" | gawk ' { print $2 }'`

#if ( "x${jobOptions}x" == "xx" ) then
# echo "could not find any jobOptions !!"
# exit 0
#endif

# set comments=`grep 'VGCOMMENT:' $jobOptions | gawk -F \: ' { print $2 }'`
set comments=""

set ref_nightly=`echo $AtlasVersion | awk '{print index($0,"rel_")}'`
echo $ref_nightly
if ( $ref_nightly > 0 ) then
 set ref_nightly=`echo $AtlasVersion | awk -F \_ '{print $2}'`
 @ ref_nightly --
 if ( $ref_nightly < 0 ) then
  set ref_nightly=6
 endif
 set ref_nightly="rel_$ref_nightly"
 echo $ref_nightly $AtlasVersion
else
 set ref_nightly=''
endif
# set ref_nightly=''

echo $logfile $pid_vgj $jobOptions

cd processed

# somehow, output for a different process is found also in this file...
grep '=='${pid_vgj}'== ' ../$logfile > tmp.logfile

csplit -s -z -k -f error.report tmp.logfile '/=='${pid_vgj}'== ERROR SUMMARY: /+9' >& /dev/null
mv error.report00 error.report
mv error.report01 leak.report
rm -f tmp.logfile

csplit -s -z -k -f error.summary error.report '/=='${pid_vgj}'== ERROR SUMMARY:/' >& /dev/null
mv error.summary00 error.report
mv error.summary01 error.summary

csplit -s -z -k -f error.tmp error.report '%=='${pid_vgj}'==    '${jobOptions}'%+2' >& /dev/null
mv error.tmp00 error.report

csplit -s -z -k -f leak.summary leak.report '/=='${pid_vgj}'== LEAK SUMMARY:/' >& /dev/null
mv leak.summary00 leak.report
mv leak.summary01 leak.summary

mkdir -p errors
mkdir -p leaks

csplit -s -z -k -f errors/process_ -n4 error.report '/=='${pid_vgj}'== $/+1' '{*}'
csplit -s -z -k -f leaks/process_ -n4 leak.report '/^=='${pid_vgj}'== [0-9a-zA-Z]/' '{*}'

# cp -f ${home_dir}/html/head/*.html .

foreach i ( Definitely_Lost Indirectly_Lost Possibly_Lost Still_Reachable )
 echo preparing $i
 set name_i=`echo $i | sed "s/_//g"`
 set name_j=`echo $i | sed "s/_/ /g"`
cat <<EOF > $name_i.htm
<!DOCTYPE doctype PUBLIC "-//w3c//dtd html 4.0 transitional//en">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
<title>Valgrind Report: ${name_j}</title>
</head>

<body bgcolor="#ffffff">

<center>
<h1>Valgrind reported as ${name_j}:</h1></center>
<center>
<h3>
<table cellpadding="10">
<tbody><tr align="center">

<a href=../Report.html>Back to main</a>

<td> running number
<td> Loss Report number
<td> bytes lost
<td> in function
<td> in library

EOF
end

foreach i ( Mismatched_Free Conditional_Jump Invalid_Read Invalid_Write Use_Of_Uninitialized_Value Open_Files Other_Errors )
 set name_i=`echo $i | sed "s/_//g"`
 set name_j=`echo $i | sed "s/_/ /g"`
cat <<EOF > $name_i.htm
<!DOCTYPE doctype PUBLIC "-//w3c//dtd html 4.0 transitional//en">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
<title>Valgrind Report: ${name_j}</title>
</head>

<body bgcolor="#ffffff">

<center>
<h1>Valgrind reported as ${name_j}:</h1></center>
<center>
<h3>
<table cellpadding="10">
<tbody><tr align="center">

<a href=../Report.html>Back to main</a>

<td> running number
<td> Error Report number
<td> in function
<td> in library
EOF
end

@ ndl = 0
@ nil = 0
@ npl = 0
@ nsr = 0

rm -f leak.list

pwd
ls -lR

cat <<EOF > LeaksList.htm
<!DOCTYPE doctype PUBLIC "-//w3c//dtd html 4.0 transitional//en">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">

<title>Valgrind Report of memory problems</title>
</head>

<body bgcolor="#ffffff">

EOF

cat <<EOF > ErrorReport.htm
<!DOCTYPE doctype PUBLIC "-//w3c//dtd html 4.0 transitional//en">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">

<title>Valgrind Report of Errors</title>
</head>

<body bgcolor="#ffffff">

EOF

set failure=`grep "Valgrind cannot continue." ../$logfile | wc -l`

touch leaks/dummy
set nl=`ls -1 leaks/* | wc -l`

echo "files in leaks : $nl"

if ( $nl > 1 ) then

foreach i ( leaks/process_* )
 # echo $i | awk -F \: ' {print $1 } '
 set dl=`grep -q 'definitely lost' $i ; echo $? ; true`
 set il=`grep -q 'indirectly lost' $i ; echo $? ; true`
 set pl=`grep -q 'possibly lost' $i   ; echo $? ; true`
 set sr=`grep -q 'still reachable' $i ; echo $? ; true`
 
 set mem=`head -1 $i | awk ' {print $2 } '`
 # set fun=`grep -m 1 '==    by ' $i | sed 's/: /# /g;s/ (in / #/g;s/)$//g' | awk -F \# ' { print $2 } '`
 # set lib=`grep -m 1 '==    by ' $i | sed 's/: /# /g;s/ (in / #/g;s/)$//g' | awk -F \# ' { print $3 } '`
 # set fun=`grep -m 1 '==    by ' $i | sed 's/: /# /g;s/ (in / #/g;s/)$//g' | awk -F \# ' { print $2 } '`
 set lib=`grep -m 1 '==    by ' $i | awk -F \( ' { print $NF } '`
 # set r=`grep -m 1 '==    by ' $i | awk ' { print $1 " " $2 " " $3 } '`
 # set fun=`grep -m 1 '==    by ' $i | sed "s/$r//g"`
 set r=`grep -m 1 '==    by ' $i | awk ' { print $1 } '`
 set fun=`grep -m 1 '==    by ' $i | sed "s/$r//g"`
 set r=`grep -m 1 '==    by ' $i | awk ' { print $2 } '`
 set fun=`echo "$fun" | sed "s/$r//g"`
 set r=`grep -m 1 '==    by ' $i | awk ' { print $3 } '`
 set fun=`echo "$fun" | sed "s/$r//g"`

 set rlib=`echo "($lib" | sed "s/\//\\\//g"`
 set fun=`echo "$fun" | sed "s/$rlib//g"`
 set nr=`echo $i | tail -c5`
 set lib=`echo $lib | sed 's/)$//g;s/^in//g'`
 
 # replace "<" and ">" - these are special characters in html
 set fun=`echo "$fun" | sed "s/</\&lt;/g;s/>/\&gt;/g"`
 set lib=`echo $lib | sed "s/</\&lt;/g;s/>/\&gt;/g"`
 
 # definitely lost
 if ( $dl == 0 ) then
   
  @ ndl ++
  
  echo '</td></tr>\n<tr align="center">\n' >> DefinitelyLost.htm
  
  echo "<td>$ndl" >> DefinitelyLost.htm
  # echo "<td><a href=$i>Loss Report $nr</a>" >> DefinitelyLost.htm
  echo "<td><a href=LeaksList.htm#lossreport_$nr>Loss Report $nr</a>" >> DefinitelyLost.htm
  echo "<td> $mem" >> DefinitelyLost.htm
  echo "<td> $fun" >> DefinitelyLost.htm
  echo "<td> $lib" >> DefinitelyLost.htm
  
  echo "<h2><a name=\042lossreport_$nr\042 href=\042#lossreport_$nr\042>Loss Report $nr</a></h2>" >> LeaksList.htm
  echo "<pre><code>" >> LeaksList.htm
  # replace "<" and ">" - these are special characters in html
  cat $i | sed "s/</\&lt;/g;s/>/\&gt;/g" >> LeaksList.htm
  echo "</code></pre>" >> LeaksList.htm
  echo "<a href=../Report.html>Back to main</a>" >> LeaksList.htm

 endif
 
 # indirectly lost
 if ( $il == 0 ) then
  
  @ nil ++
  
  echo '</td></tr>\n<tr align="center">\n' >> IndirectlyLost.htm
   
  echo "<td>$nil" >> IndirectlyLost.htm
  # echo "<td><a href=$i>Loss Report $nr</a>" >> IndirectlyLost.htm
  echo "<td><a href=LeaksList.htm#lossreport_$nr>Loss Report $nr</a>" >> IndirectlyLost.htm
  echo "<td> $mem" >> IndirectlyLost.htm
  echo "<td> $fun" >> IndirectlyLost.htm
  echo "<td> $lib" >> IndirectlyLost.htm
  
  echo "<h2><a name=\042lossreport_$nr\042 href=\042#lossreport_$nr\042>Loss Report $nr</a></h2>" >> LeaksList.htm
  echo "<pre><code>" >> LeaksList.htm
  # replace "<" and ">" - these are special characters in html
  cat $i | sed "s/</\&lt;/g;s/>/\&gt;/g" >> LeaksList.htm
  echo "</code></pre>" >> LeaksList.htm
  echo "<a href=../Report.html>Back to main</a>" >> LeaksList.htm
  
 endif

 # possibly lost
 if ( $pl == 0 ) then
  
  @ npl ++
  
  echo '</td></tr>\n<tr align="center">\n' >> PossiblyLost.htm
   
  echo "<td>$npl" >> PossiblyLost.htm
  # echo "<td><a href=$i>Loss Report $nr</a>" >> PossiblyLost.htm
  echo "<td><a href=LeaksList.htm#lossreport_$nr>Loss Report $nr</a>" >> PossiblyLost.htm
  echo "<td> $mem" >> PossiblyLost.htm
  echo "<td> $fun" >> PossiblyLost.htm
  echo "<td> $lib" >> PossiblyLost.htm

  echo "<h2><a name=\042lossreport_$nr\042 href=\042#lossreport_$nr\042>Loss Report $nr</a></h2>" >> LeaksList.htm
  echo "<pre><code>" >> LeaksList.htm
  # replace "<" and ">" - these are special characters in html
  cat $i | sed "s/</\&lt;/g;s/>/\&gt;/g" >> LeaksList.htm
  echo "</code></pre>" >> LeaksList.htm
  echo "<a href=../Report.html>Back to main</a>" >> LeaksList.htm
 endif

 # still reachable
 if ( $sr == 0 ) then
  
  @ nsr ++
  
  echo '</td></tr>\n<tr align="center">\n' >> StillReachable.htm
   
  echo "<td>$nsr" >> StillReachable.htm
  # echo "<td><a href=$i>Loss Report $nr</a>" >> StillReachable.htm
  echo "<td><a href=LeaksList.htm#lossreport_$nr>Loss Report $nr</a>" >> StillReachable.htm
  echo "<td> $mem" >> StillReachable.htm
  echo "<td> $fun" >> StillReachable.htm
  echo "<td> $lib" >> StillReachable.htm

  echo "<h2><a name=\042lossreport_$nr\042 href=\042#lossreport_$nr\042>Loss Report $nr</a></h2>" >> LeaksList.htm
  echo "<pre><code>" >> LeaksList.htm
  # replace "<" and ">" - these are special characters in html
  cat $i | sed "s/</\&lt;/g;s/>/\&gt;/g" >> LeaksList.htm
  echo "</code></pre>" >> LeaksList.htm
  echo "<a href=../Report.html>Back to main</a>" >> LeaksList.htm
 endif
 
 set bn=`basename $i`
 sed 's/=='$pid_vgj'==/==PID==/g;s/at 0x[0-9,A-F]\{3,8\}:/at 0xFFFFFFFF:/g' $i | \
 sed 's/by 0x[0-9,A-F]\{3,8\}:/by 0xFFFFFFFF:/g' | \
 sed 's/Address 0x[0-9,a-f,A-F]\{3,8\} /Address 0xFFFFFFFF /g' | \
 sed 's/'$AtlasVersion'/RELEASE/g' | \
 sed 's/ in loss record [0-9]\+ of [0-9]\+/ in loss record N of ALL/g' > $bn
 md5sum $bn >> leak.list
 rm -f $bn
 
end

endif


if ( $ndl == 0 ) then
 echo "<a href=../Report.html>Back to main</a>" >> DefinitelyLost.htm
endif
if ( $nil == 0 ) then
 echo "<a href=../Report.html>Back to main</a>" >> IndirectlyLost.htm
endif
if ( $npl == 0 ) then
 echo "<a href=../Report.html>Back to main</a>" >> PossiblyLost.htm
endif
if ( $nsr == 0 ) then
 echo "<a href=../Report.html>Back to main</a>" >> StillReachable.htm
endif

cat ${home_dir}/html/tail/Tail.html >> DefinitelyLost.htm
cat ${home_dir}/html/tail/Tail.html >> IndirectlyLost.htm
cat ${home_dir}/html/tail/Tail.html >> PossiblyLost.htm
cat ${home_dir}/html/tail/Tail.html >> StillReachable.htm

echo "</body></htm>" >> LeaksList.htm

# and process the error reports about mismatched free and jumps on uninitialized values.
# plus invalid read ! And Use of uninitialised value of size + open files + other errors

@ nmm = 0
@ nju = 0
@ nir = 0
@ niw = 0
@ nuu = 0
@ nof = 0
@ noe = 0

rm -f error.list

touch errors/dummy
set nl=`ls -1 errors/* | wc -l`

echo "files in errors : $nl"

if ( $nl > 1 ) then

foreach i ( errors/process_* )
 # echo $i | awk -F \: ' {print $1 } '
 
 set mm=`grep -q 'Mismatched free' $i ; echo $?`
 set ju=`grep -q 'Conditional jump or move' $i ; echo $?`
 set ir=`grep -q 'Invalid read' $i   ; echo $?`
 set iw=`grep -q 'Invalid write' $i   ; echo $?`
 set uu=`grep -q -e 'Use of uninitialised value of size' -e 'points to uninitialised byte' $i   ; echo $?`
 set of=`grep -q 'Open file descriptor ' $i   ; echo $?`
 set oe=0
 
 # set fun=`grep -m 1 '==    by ' $i | sed 's/: /# /g;s/ (in / #/g;s/)$//g' | awk -F \# ' { print $2 } '`
 # set lib=`grep -m 1 '==    by ' $i | sed 's/: /# /g;s/ (in / #/g;s/)$//g' | awk -F \# ' { print $3 } '`
 # set nr=`echo $i | tail -c5`

 set lib=`grep -m 1 '==    by ' $i | awk -F \( ' { print $NF } '`
 set r=`grep -m 1 '==    by ' $i | awk ' { print $1 } '`
 set fun=`grep -m 1 '==    by ' $i | sed "s/$r//g"`
 set r=`grep -m 1 '==    by ' $i | awk ' { print $2 } '`
 set fun=`echo "$fun" | sed "s/$r//g"`
 set r=`grep -m 1 '==    by ' $i | awk ' { print $3 } '`
 set fun=`echo "$fun" | sed "s/$r//g"`
 set rlib=`echo "($lib" | sed "s/\//\\\//g"`
 set fun=`echo "$fun" | sed "s/$rlib//g"`
 set nr=`echo $i | tail -c5`
 set lib=`echo $lib | sed 's/)$//g;s/^in//g'`

 # Mismatched free
 if ( $mm == 0 ) then

  @ oe = 1

  @ nmm ++
  
  echo '</td></tr>\n<tr align="center">\n' >> MismatchedFree.htm
  
  echo "<td>$nmm" >> MismatchedFree.htm
  # echo "<td><a href=$i>Error Report $nr</a>" >> MismatchedFree.htm
  echo "<td><a href=ErrorReport.htm#errorreport_$nr>Error Report $nr</a>" >> MismatchedFree.htm
  echo "<td> $fun" >> MismatchedFree.htm
  echo "<td> $lib" >> MismatchedFree.htm

  echo "<h2><a name=\042errorreport_$nr\042 href=\042#errorreport_$nr\042>Error Report $nr</a></h2>" >> ErrorReport.htm
  echo "<pre><code>" >> ErrorReport.htm
  # replace "<" and ">" - these are special characters in html
  cat $i | sed "s/</\&lt;/g;s/>/\&gt;/g" >> ErrorReport.htm
  echo "</code></pre>" >> ErrorReport.htm
  echo "<a href=../Report.html>Back to main</a>" >> ErrorReport.htm
  
 endif

 # conditional jumps
 if ( $ju == 0 ) then
  
  @ oe = 1

  @ nju ++
  
  echo '</td></tr>\n<tr align="center">\n' >> ConditionalJump.htm
  
  echo "<td>$nju" >> ConditionalJump.htm
  # echo "<td><a href=$i>Error Report $nr</a>" >> ConditionalJump.htm
  echo "<td><a href=ErrorReport.htm#errorreport_$nr>Error Report $nr</a>" >> ConditionalJump.htm
  echo "<td> $fun" >> ConditionalJump.htm
  echo "<td> $lib" >> ConditionalJump.htm

  echo "<h2><a name=\042errorreport_$nr\042 href=\042#errorreport_$nr\042>Error Report $nr</a></h2>" >> ErrorReport.htm
  echo "<pre><code>" >> ErrorReport.htm
  # replace "<" and ">" - these are special characters in html
  cat $i | sed "s/</\&lt;/g;s/>/\&gt;/g" >> ErrorReport.htm
  echo "</code></pre>" >> ErrorReport.htm
  echo "<a href=../Report.html>Back to main</a>" >> ErrorReport.htm

 endif

 # invalid reads
 if ( $ir == 0 ) then
  
  @ oe = 1

  @ nir ++
  
  echo '</td></tr>\n<tr align="center">\n' >> InvalidRead.htm
  
  echo "<td>$nir" >> InvalidRead.htm
  # echo "<td><a href=$i>Error Report $nr</a>" >> InvalidRead.htm
  echo "<td><a href=ErrorReport.htm#errorreport_$nr>Error Report $nr</a>" >> InvalidRead.htm
  echo "<td> $fun" >> InvalidRead.htm
  echo "<td> $lib" >> InvalidRead.htm
  
  echo "<h2><a name=\042errorreport_$nr\042 href=\042#errorreport_$nr\042>Error Report $nr</a></h2>" >> ErrorReport.htm
  echo "<pre><code>" >> ErrorReport.htm
  # replace "<" and ">" - these are special characters in html
  cat $i | sed "s/</\&lt;/g;s/>/\&gt;/g" >> ErrorReport.htm
  echo "</code></pre>" >> ErrorReport.htm
  echo "<a href=../Report.html>Back to main</a>" >> ErrorReport.htm
  
 endif
 # invalid writes
 if ( $iw == 0 ) then
  
  @ oe = 1

  @ niw ++
  
  echo '</td></tr>\n<tr align="center">\n' >> InvalidWrite.htm
  
  echo "<td>$nir" >> InvalidWrite.htm
  # echo "<td><a href=$i>Error Report $nr</a>" >> InvalidWrite.htm
  echo "<td><a href=ErrorReport.htm#errorreport_$nr>Error Report $nr</a>" >> InvalidWrite.htm
  echo "<td> $fun" >> InvalidWrite.htm
  echo "<td> $lib" >> InvalidWrite.htm
  
  echo "<h2><a name=\042errorreport_$nr\042 href=\042#errorreport_$nr\042>Error Report $nr</a></h2>" >> ErrorReport.htm
  echo "<pre><code>" >> ErrorReport.htm
  # replace "<" and ">" - these are special characters in html
  cat $i | sed "s/</\&lt;/g;s/>/\&gt;/g" >> ErrorReport.htm
  echo "</code></pre>" >> ErrorReport.htm
  echo "<a href=../Report.html>Back to main</a>" >> ErrorReport.htm
  
 endif
######################
 # use of uninitialized value
 if ( $uu == 0 ) then

  @ oe = 1

  @ nuu ++
  
  echo '</td></tr>\n<tr align="center">\n' >> UseOfUninitializedValue.htm
  
  echo "<td>$nir" >> UseOfUninitializedValue.htm
  # echo "<td><a href=$i>Error Report $nr</a>" >> UseOfUninitializedValue.htm
  echo "<td><a href=ErrorReport.htm#errorreport_$nr>Error Report $nr</a>" >> UseOfUninitializedValue.htm
  echo "<td> $fun" >> UseOfUninitializedValue.htm
  echo "<td> $lib" >> UseOfUninitializedValue.htm
  
  echo "<h2><a name=\042errorreport_$nr\042 href=\042#errorreport_$nr\042>Error Report $nr</a></h2>" >> ErrorReport.htm
  echo "<pre><code>" >> ErrorReport.htm
  # replace "<" and ">" - these are special characters in html
  cat $i | sed "s/</\&lt;/g;s/>/\&gt;/g" >> ErrorReport.htm
  echo "</code></pre>" >> ErrorReport.htm
  echo "<a href=../Report.html>Back to main</a>" >> ErrorReport.htm
  
 endif

 # open file
 if ( $of == 0 ) then
  
  @ oe = 1
  
  @ nof ++
  
  echo '</td></tr>\n<tr align="center">\n' >> OpenFiles.htm
  
  echo "<td>$nir" >> OpenFiles.htm
  # echo "<td><a href=$i>Error Report $nr</a>" >> OpenFiles.htm
  echo "<td><a href=ErrorReport.htm#errorreport_$nr>Error Report $nr</a>" >> OpenFiles.htm
  echo "<td> $fun" >> OpenFiles.htm
  echo "<td> $lib" >> OpenFiles.htm
  
  echo "<h2><a name=\042errorreport_$nr\042 href=\042#errorreport_$nr\042>Error Report $nr</a></h2>" >> ErrorReport.htm
  echo "<pre><code>" >> ErrorReport.htm
  # replace "<" and ">" - these are special characters in html
  cat $i | sed "s/</\&lt;/g;s/>/\&gt;/g" >> ErrorReport.htm
  echo "</code></pre>" >> ErrorReport.htm
  echo "<a href=../Report.html>Back to main</a>" >> ErrorReport.htm
  
 endif

 # other errors
 if ( $oe == 0 ) then
  
  @ noe ++
  
  echo '</td></tr>\n<tr align="center">\n' >> OtherErrors.htm
  
  echo "<td>$nir" >> OtherErrors.htm
  # echo "<td><a href=$i>Error Report $nr</a>" >> OtherErrors.htm
  echo "<td><a href=ErrorReport.htm#errorreport_$nr>Error Report $nr</a>" >> OtherErrors.htm
  echo "<td> $fun" >> OtherErrors.htm
  echo "<td> $lib" >> OtherErrors.htm
  
  echo "<h2><a name=\042errorreport_$nr\042 href=\042#errorreport_$nr\042>Error Report $nr</a></h2>" >> ErrorReport.htm
  echo "<pre><code>" >> ErrorReport.htm
  # replace "<" and ">" - these are special characters in html
  cat $i | sed "s/</\&lt;/g;s/>/\&gt;/g" >> ErrorReport.htm
  echo "</code></pre>" >> ErrorReport.htm
  echo "<a href=../Report.html>Back to main</a>" >> ErrorReport.htm
  
 endif
#################
 set bn=`basename $i`
 sed 's/=='$pid_vgj'==/==PID==/g;s/at 0x[0-9,A-F]\{3,8\}:/at 0xFFFFFFFF:/g' $i | \
 sed 's/by 0x[0-9,A-F]\{3,8\}:/by 0xFFFFFFFF:/g' | \
 sed 's/Address 0x[0-9,a-f,A-F]\{3,8\} /Address 0xFFFFFFFF /g' | \
 sed 's/'$AtlasVersion'/RELEASE/g' | \
 sed 's/ in loss record [0-9]\+ of [0-9]\+/ in loss record N of ALL/g' > $bn
 md5sum $bn >> error.list
 rm -f $bn

end

endif

cat ${home_dir}/html/tail/Tail.html >> MismatchedFree.htm
cat ${home_dir}/html/tail/Tail.html >> ConditionalJump.htm
cat ${home_dir}/html/tail/Tail.html >> InvalidRead.htm
cat ${home_dir}/html/tail/Tail.html >> InvalidWrite.htm
cat ${home_dir}/html/tail/Tail.html >> UseOfUninitializedValue.htm
cat ${home_dir}/html/tail/Tail.html >> OpenFiles.htm
cat ${home_dir}/html/tail/Tail.html >> OtherErrors.htm

echo "</body></htm>" >> ErrorReport.htm

set utime=`tail -2 ../out.valgrind.log | head -1 | gawk '{print $1 }'`
set prCPU=`tail -2 ../out.valgrind.log | head -1 | gawk '{print $4 }'`
set pyRes=`grep "INFO leaving with code" ../out.valgrind.log | sed 's/^Py:Athena//g;s/INFO//g;s/^[ \t]*//'`

set pyResCode=`echo $pyRes | sed "s/leaving with code \(.*\):.*/\1/g"`

# fill the final report
# the python steering file already produced the first few lines for us.
#<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
#     "http://www.w3.org/TR/html4/loose.dtd">
#<html>
#<head>
#<title>Valgrind Report for $jobName release $AtlasVersion of project $AtlasProject</title>
#</head>
#<body>
#<h1>Valgrind Report for:</h1>
#<table border="0">
#  <tr>
#    <td>release</td>
#    <td>$AtlasVersion</td>
#  </tr>
#  <tr>
#    <td>project</td>
#    <td>$AtlasProject</td>
#  </tr>
#  <tr>
#    <td>AtlasArea points to</td>
#    <td>$AtlasArea</td>
#  </tr>
#  <tr>
#    <td>CMTCONFIG</td>
#    <td>$CMTCONFIG</td>
#  </tr>

cat <<EOF >> ../Report.html
  <tr>
    <td>jobOptions</td>
    <td><a href=$jobOptions>$jobOptions</a></td>
  </tr>
  <tr>
    <td>usertime</td>
    <td>$utime</td>
  </tr>
  <tr>
    <td>and got</td>
    <td>$prCPU</td>
  </tr>
  <tr>
    <td>job finished with</td>
    <td>$pyRes</td>
  </tr>
  <tr>
    <td>logfile is here:</td>
    <td><a href=out.valgrind.log>out.valgrind.log</a></td>
  </tr>
</table>

<a href=processed/mem.eps><img src=processed/mem.gif align="right" vspace="10" hspace="20" ></a>

<pre>
 <code>
EOF

sed 's/=='${pid_vgj}'== //g' leak.summary | \
sed "s/definitely lost/<a href=processed\/DefinitelyLost.htm>definitely lost<\/a>/g" | \
sed "s/indirectly lost/<a href=processed\/IndirectlyLost.htm>indirectly lost<\/a>/g" | \
sed "s/possibly lost/<a href=processed\/PossiblyLost.htm>possibly lost<\/a>/g" | \
sed "s/still reachable/<a href=processed\/StillReachable.htm>still reachable<\/a>/g" >> ../Report.html

echo >> ../Report.html
echo "reports of definitely lost : $ndl" >> ../Report.html
echo "reports of indirectly lost : $nil" >> ../Report.html
echo "reports of   possibly lost : $npl" >> ../Report.html
echo "reports of still reachable : $nsr" >> ../Report.html

set doy=`date -r $AtlasArea/cmt +'%j'`

cat <<EOF >> ../Report.html
 </code>
</pre>

<pre>
 <code>
ERROR SUMMARY:
   <a href=processed/MismatchedFree.htm>Mismatched free() / delete / delete []</a>                       : $nmm
   <a href=processed/ConditionalJump.htm>Conditional jump or move depends on uninitialised value(s)</a>   : $nju
   <a href=processed/InvalidRead.htm>Invalid read</a>                                                 : $nir
   <a href=processed/InvalidWrite.htm>Invalid write</a>                                                : $niw
   <a href=processed/UseOfUninitializedValue.htm>Use of uninitialized Values</a>                                  : $nuu
   <a href=processed/OpenFiles.htm>Open Files</a>                                                   : $nof
   <a href=processed/OtherErrors.htm>Other Errors (might not be errors at all)</a>                    : $noe
 </code>
</pre>

EOF

if ( $failure > 0 ) then
cat <<EOF >> ../Report.html
<h3>FAILURE:</h3>
<pre>
 <code>
EOF

grep -v "^==" ../$logfile >> ../Report.html

cat <<EOF >> ../Report.html
 </code>
</pre>

EOF
endif

set rdir=`echo $VG_RDIR | sed "s/$AtlasVersion/$ref_nightly/g"`

if ( "$ref_nightly" != "" ) then

cp -f $rdir/report_def.log ..
cp -f $rdir/report_ind.log ..
cp -f $rdir/report_pos.log ..
cp -f $rdir/report_str.log ..
echo "$doy $ndl" >> ../report_def.log
echo "$doy $nil" >> ../report_ind.log
echo "$doy $npl" >> ../report_pos.log
echo "$doy $nsr" >> ../report_str.log

cp -f $rdir/report_mfd.log ..
cp -f $rdir/report_cjm.log ..
cp -f $rdir/report_inv.log ..
cp -f $rdir/report_stc.log ..
echo "$doy $nmm" >> ../report_mfd.log
echo "$doy $nju" >> ../report_cjm.log
echo "$doy $nir" >> ../report_inv.log
echo "$doy $pyResCode" >> ../report_stc.log

cat <<EOF >> ../Report.html
<p>
<h4>History:</h4>
EOF

endif

cat <<EOF >> ../Report.html
<br>
<a href=processed/history.eps><img src=processed/history.gif></a>

</body>
</html> 
EOF

# possible diffs need PID and addresses modified ...
grep '^=='$pid_vgj'==' ../$logfile | \
 sed 's/=='$pid_vgj'==/==PID==/g;s/at 0x[0-9,A-F]\{3,8\}:/at 0xFFFFFFFF:/g' | \
 sed 's/by 0x[0-9,A-F]\{3,8\}:/by 0xFFFFFFFF:/g' | \
 sed 's/Address 0x[0-9,a-f,A-F]\{3,8\} /Address 0xFFFFFFFF /g' | \
 sed 's/'$AtlasVersion'/RELEASE/g' | \
 sed 's/ in loss record [0-9]\+ of [0-9]\+/ in loss record N of ALL/g' > prepForDiff

set nll=`wc -l leak.list | gawk ' {print $1 }'`
set nel=`wc -l error.list | gawk ' {print $1 }'`
set mll=`wc -l $rdir/leak.list | gawk ' {print $1 }'`
set mel=`wc -l $rdir/error.list | gawk ' {print $1 }'`

#foreach i ( `sed "s/  /./g" leak.list` )
# echo $i
#end

set ref_nightly =""
if ( "$ref_nightly" != "" ) then

cat <<EOF > DiffToYesterday.htm
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
     "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<title>diffs to Valgrind Report from Yesterday</title>
</head>
<body>
<h1>Diffs to Yesterday:</h1>

<table border="0">
  <tr>
    <td> </td>
    <td>$AtlasVersion</td>
    <td>$ref_nightly</td>
  </tr>
  <tr>
    <td>leak reports</td>
    <td>$nll</td>
    <td>$mll</td>
  </tr>
  <tr>
    <td>error reports</td>
    <td>$nel</td>
    <td>$mel</td>
  </tr>
</table>

In detail:<br>
<a href=NewReports.htm>new leaks</a><br>
<a href=DisappearedReports.htm>disappeared leaks</a><br>
<a href=OldReports.htm>old leaks</a><br>

Note: small changes in the output can lead to a report classified as "new". The report from the reference nightly then shows up as "disappeared".
</body>
</htm> 
EOF

cat <<EOF > NewReports.htm
<!DOCTYPE HTM PUBLIC "-//W3C//DTD HTM 4.01 Transitional//EN"
     "http://www.w3.org/TR/htm4/loose.dtd">
<htm>
<head>
<title>new leak reports</title>
</head>
<body>
<h1>new leak reports:</h1>
<table border="0">
EOF

cat <<EOF > OldReports.htm
<!DOCTYPE HTM PUBLIC "-//W3C//DTD HTM 4.01 Transitional//EN"
     "http://www.w3.org/TR/htm4/loose.dtd">
<htm>
<head>
<title>old leak reports</title>
</head>
<body>
<h1>old leak reports:</h1>
<table border="0">
EOF

cat <<EOF > DisappearedReports.htm
<!DOCTYPE HTM PUBLIC "-//W3C//DTD HTM 4.01 Transitional//EN"
     "http://www.w3.org/TR/htm4/loose.dtd">
<htm>
<head>
<title>disappeared leak reports</title>
</head>
<body>
<h1>disappeared leak reports:</h1>
<table border="0">
EOF

set new=0
set dis=$mll
set old=0

set foundreports=""

foreach i ( `sed "s/  /./g" leak.list` )
 set checksum=$i:r
 set filename=$i:e

 set nr=`echo $filename | tr \_ \.`
 
 set tmp=`grep $checksum $rdir/leak.list`
 set result=$status
 set tmp=`echo $tmp | sed "s/$checksum//g"`
 # echo $checksum $filename $result $tmp "."

 if ( $result ) then
  echo "<tr align="center"><td><a href=leaks/$filename>Loss Report $nr:e</a> </td></tr>" >> NewReports.htm
  @ new ++
  @ dis --
 else
  echo "<tr align="center"><td><a href=leaks/$filename>Loss Report $nr:e</a>" >> OldReports.htm
  foreach j ( $tmp )
   set onr=`echo $j | tr \_ \.`
   echo " <td><a href=$tmp>old Loss Report $onr:e</a>" >> OldReports.htm
   set foundreports="$foundreports $j"
  end
  echo "</td></tr>" >> OldReports.htm
  @ old ++
  @ dis --
 endif
end

echo $new $old $dis
# echo $foundreports

foreach i ( $rdir/leaks/process* )
 set filename=`basename $i`
 echo $foundreports | grep -q $filename
 set result=$status
 # echo $i $filename $result
 if ( $result ) then
  echo "<tr align="center"><td><a href=$filename>Loss Report</a> </td></tr>" >> DisappearedReports.htm
 endif
end

cat <<EOF >> NewReports.htm
</table>
</body>
</htm> 
EOF

cat <<EOF >> OldReports.htm
</table>
</body>
</htm> 
EOF

cat <<EOF >> DisappearedReports.htm
</table>
</body>
</htm> 
EOF
endif

# post-postprocess two htm files
mv LeaksList.htm LeaksList.bak.htm
mv ErrorReport.htm ErrorReport.bak.htm

sed 's/(\(\([a-zA-Z0-9._]*\.[c,h,i].*\):[0-9]\+\))$/& (<a href=http:\/\/alxr.usatlas.bnl.gov\/lxr\/search\?filestring=\2>LXR: \2<\/a>)/g' \
  LeaksList.bak.htm > LeaksList.htm

sed 's/(\(\([a-zA-Z0-9._]*\.[c,h,i].*\):[0-9]\+\))$/& (<a href=http:\/\/alxr.usatlas.bnl.gov\/lxr\/search\?filestring=\2>LXR: \2<\/a>)/g' \
  ErrorReport.bak.htm > ErrorReport.htm

# sed 's/(\(\([a-zA-Z0-9._]*\):[0-9]\+\))$/& (<a href=http:\/\/alxr.usatlas.bnl.gov\/lxr\/search\?filestring=\2>\1<\/a>)/g' ../makeRTT/Results/rel_1/devval/build/i686-slc4-gcc34-dbg/offline/ValgrindRTTJobs/Valgrind_JobTransforms/ESDtoESD/5/processed/LeaksList.htm> ../makeRTT/Results/rel_1/devval/build/i686-slc4-gcc34-dbg/offline/ValgrindRTTJobs/Valgrind_JobTransforms/ESDtoESD/5/processed/LeaksList.html

cd ..
# /afs/cern.ch/atlas/offline/external/LCGCMT/LCGCMT_56/InstallArea/i686-slc4-gcc34-opt/bin/
root -l -b -q historyNightlies.C
mv history.gif history.eps processed/
