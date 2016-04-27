#!/bin/ksh
#
# 2009 Atlas Detector Software --- Wolfgang.Liebig -at- cern.ch
#
echo "************************************************"
echo "* making ntuple variable documentation snippet *"
echo "************************************************"
echo "mkNtpDoc : Usage: mkNtpDoc headerFileWithNtupleVariables.h"
echo " "
if [ $# -lt 1 ]
then
  echo "mkNtpDoc : ERROR, wrong parameters (see welcome message for usage)."
  echo "mkNtpDoc : exit..."
  exit 1
fi
#
headerfile="$1"
itemfile1=`echo $headerfile | awk -F. '{print "../" $1 ".tmp1"}'`
itemfile2=`echo $headerfile | awk -F. '{print "../" $1 ".tmp2"}'`
cxxfile=`echo $headerfile | awk -F. '{print "../src/" $1 ".cxx"}'`
branchfile=`echo $headerfile | awk -F. '{print "../" $1 ".branches"}'`
docufile=`echo $headerfile | awk -F. '{print "../" $1 ".html"}'`
wikifile=`echo $headerfile | awk -F. '{print "../" $1 ".wiki"}'`

if [ -r ${docufile} ]
then
  rm ${docufile}
  rm ${wikifile}
fi
#
# this commands first selects all lines with ntuple variables *and* doxygen comments,
# then extracts c++ member variable type, name, and doxy comment from the text line
#
isAlgorithm=`grep "public AthAlgorithm" ${headerfile}`
if [ ! "${isAlgorithm}" ]
then
  cat ${headerfile} |  grep "mutable" | grep "int\|vector\|float" \
   | grep -v "^[ ]*//"  | grep "//!<" |  gawk '{ 
   print "ITEM " substr($3,1,index($3,";")-1)
   split($0,x,"//!<"); iunit = index(x[2],"UNIT:")
   if (iunit>2) print "TEXT " substr(x[2],1,iunit-1) ; else print "TEXT " x[2]
   if (substr($2,1,3)== "std") print "TYPE std::vector&lt;" substr($2,13,index($2,">")-13) "&gt;"
       else print "TYPE " $2
   if (iunit>2) print "UNIT " substr(x[2],iunit+5,length(x[2])-iunit-4)
       else print "UNIT -"}' > ${itemfile1}
else
  cat ${headerfile} | grep "int\|long\|vector\|float" \
   | grep -v "^[ ]*//"  | grep "//!<" |  gawk '{ 
   print "ITEM " substr($2,1,index($2,";")-1)
   split($0,x,"//!<"); iunit = index(x[2],"UNIT:")
   if (iunit>2) print "TEXT " substr(x[2],1,iunit-1) ; else print "TEXT " x[2]
   if (substr($1,1,3)== "std") print "TYPE std::vector&lt;" substr($1,13,index($1,">")-13) "&gt;"
       else print "TYPE " $1
   if (iunit>2) print "UNIT " substr(x[2],iunit+5,length(x[2])-iunit-4)
       else print "UNIT -"}' > ${itemfile1}
fi
#
# this part goes into the .cxx file to replace the c++ variable name with the TTree branch name
#
grep Branch ${cxxfile} >${branchfile}
touch ${itemfile2}
cat $itemfile1|while read line
do
  test=`echo $line | grep "ITEM "`
  if [ ! "${test}" ] # == "" ]
  then
    echo $line >> ${itemfile2}
  else
    memberVariable=`echo $line | gawk '{ print substr($0,6,length($0)-5) }'`
    ntupleVariable=`grep \&${memberVariable}, ${branchfile} | cut -d\\" -f2`
    if [ ! $ntupleVariable ] ; then
      ntupleVariable=`grep \&${memberVariable}\) ${branchfile} | cut -d\\" -f2`
    fi
    echo "ITEM ${ntupleVariable}" >> ${itemfile2}
  fi
done

echo "<table>" > ${docufile}
cat ${itemfile2} | gawk '{
 if ($1=="ITEM") print "<tr><td><b>" substr($0,6,length($0)-5) "</b></td>"
 if ($1=="TEXT") print "<td>" substr($0,6,length($0)-5) "</td>"
 if ($1=="TYPE") print "<td><tt>" substr($0,6,length($0)-5) "</tt></td>"
 if ($1=="UNIT") {if ($2=="-") print "<td>&nbsp;</td></tr>" 
                  else print "<td>" substr($0,6,length($0)-5) "</td></tr>" } }'\
 >> $docufile
echo "</table>" >> ${docufile}

echo "<noautolink>" > ${wikifile}
echo "|  *Branch Name*  |  *Description*  |  *Type*  |  *Unit*  |" >>${wikifile}
cat ${itemfile2} | gawk '{
 output="| <b>"
 if ($1=="ITEM") {output=(output substr($0,6,length($0)-5) "</b> | ");  getline}
 if ($1=="TEXT") {output=(output substr($0,6,length($0)-5) " | =="); getline}
 if ($1=="TYPE") {output=(output substr($0,6,length($0)-5) "== | "); getline}
 if ($1=="UNIT") {if ($2=="-") print output " |" 
                  else print output substr($0,6,length($0)-5) " |" } }'\
 >> $wikifile
echo "</noautolink>" >> ${wikifile}

rm ${itemfile1}
rm ${itemfile2}
rm ${branchfile}
echo "mkNtpDoc: wrote html snippet to ${docufile}"
