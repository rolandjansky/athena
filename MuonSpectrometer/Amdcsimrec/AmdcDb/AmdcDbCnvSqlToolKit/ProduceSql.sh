#!/bin/sh -f
#----------------------------------------------------
GetMax_XXX_DATA_ID(){

 Max_XXX_DATA_ID=0

 rm -fr test.sql
 echo "select max(${TableName}_DATA_ID) from ATLASDD.${TableName}_DATA;" > test.sql
 echo "EXIT"  >> test.sql

 export ORACLE_CERN=/afs/cern.ch/project/oracle

 grep -n atlasdd_r $CORAL_AUTH_PATH/authentication.xml|sed 's/:.*//'>DBLINE
 PASSLINE=`head -n1 DBLINE`
 let PASSLINE++
 PASSLINE="${PASSLINE}p"
 rm -fr FILEMEAGBID 
 . $ORACLE_CERN/script/setoraenv.sh -s 10205 > FILEMEAGBID
 rm -fr FILEMEAGBID 
 
 rm -fr GetMax_XXX_DATA_ID_Out
 expect -c "
 spawn sqlplus -s atlasdd_r@atlas_dd @test.sql 
 expect -nocase \"password:\" {send \"`sed -n "${PASSLINE}" $CORAL_AUTH_PATH/authentication.xml|sed 's/.*value="//'|sed 's!"/>!!'`\r\"; interact}
 ">GetMax_XXX_DATA_ID_Out
 rm -fr test.sql

 rm -fr FILEMEAGBID 
 grep 'ERROR' GetMax_XXX_DATA_ID_Out > FILEMEAGBID

 if [ "$?"  == "1" ] 
 then
   TotNber=`wc GetMax_XXX_DATA_ID_Out | awk '{print $1}' `
   ii=0
   jj=0
   nombre=$TotNber
   echo "" > FileBidon1
   until [ $ii -eq $nombre ]
   do
     jj=$(( ${jj} + 1 ))
     linecount="${jj}p"
     sed -n "${linecount}" GetMax_XXX_DATA_ID_Out|tr -cd '[[:digit:]]' >> FileBidon1
     if [ ${jj} -ne $nombre ]
     then
       echo "\n" >> FileBidon1
     fi
     ii=$(( ${ii} + 1 ))
   done
   tail -n 1 FileBidon1 > FileBidon2
   Max_XXX_DATA_ID=`cat FileBidon2 | tr -cd '[[:digit:]]'`
   rm -fr FileBidon1 FileBidon2
 else
   echo "==> Error in GetMax_XXX_DATA_ID for TableName ="${TableName}
 fi

 rm -fr DBLINE
 rm -fr FILEMEAGBID
 rm -fr GetMax_XXX_DATA_ID_Out
 
}
#----------------------------------------------------
GetNew_XXX_DATA_ID(){

 New_XXX_DATA_ID=0
 
 GetMax_XXX_DATA_ID

 New_XXX_DATA_ID=0
 until [ ${New_XXX_DATA_ID} -gt ${Max_XXX_DATA_ID} ]
 do
   New_XXX_DATA_ID=$(( ${New_XXX_DATA_ID} + 10000 ))
 done
#TEST
#New_XXX_DATA_ID=$(( ${New_XXX_DATA_ID} - 10000 ))

 if [ "${TableName}" = "AGDD" ]
 then
   New_XXX_DATA_ID=$(( ${Max_XXX_DATA_ID} + 1 ))
#TEST
#  New_XXX_DATA_ID=$(( ${New_XXX_DATA_ID} - 1 ))
 fi
 
 if [ "${TableName}" = "AMDC" ]
 then
   New_XXX_DATA_ID=$(( ${Max_XXX_DATA_ID} + 1 ))
#TEST
#  New_XXX_DATA_ID=$(( ${New_XXX_DATA_ID} - 1 ))
 fi
 
}

#----------------------------------------------------
STARTDIR=$1
TagsIdFile=TagsId.txt
OUTPUTDIR=SqlFiles
CnvSqlToolKitDIR=AmdcDbCnvSqlToolKit
WebAdress="http://atlas-php.web.cern.ch/atlas-php/DDDB/" 

 echo "--> Checking ${TagsIdFile}"
 if [ ! -f ${TagsIdFile} ] 
 then
   echo ${TagsIdFile}" not existing. Nothing done"
   echo " "
   exit
 fi

 rm -fr FILEMEGABID
 grep 'XXXXXX' ${TagsIdFile}  > FILEMEGABID
 if [ "$?" == "0" ]
 then 
   rm -fr FILEMEGABID
   echo ${TagsIdFile}" is invalid. Nothing done"
   echo " "
   exit
 fi
 rm -fr FILEMEGABID

 echo "--> Prepare ${CnvSqlToolKitDIR} dir"
 cd ${CnvSqlToolKitDIR}
  rm -fr main main.o main.cc makefile
  cp  main.cc_KEEP  main.cc
  cp  makefile_KEEP makefile
  make
 cd ..
 
 echo "--> Prepare ${OUTPUTDIR} dir"
 rm -fr ${OUTPUTDIR}
 mkdir ${OUTPUTDIR}

 echo "--> Extract files from ${STARTDIR} to ${OUTPUTDIR} dir"
 for FileCur in `ls ${STARTDIR}| grep  PREsql | grep Out.AmdcOracle.AM.`
 do
  cp -r ${STARTDIR}/${FileCur} ${OUTPUTDIR}/${FileCur}
 done
 for FileCur in `ls ${STARTDIR}| grep  temp | grep Out.AmdcOracle.AM.`
 do
  cp -r ${STARTDIR}/${FileCur} ${OUTPUTDIR}/${FileCur}
 done

 echo "--> Clean ${OUTPUTDIR} dir"
 cd ${OUTPUTDIR}
 for FileCur in `ls | grep -v PREsql | grep -v temp`
 do
   rm  ${FileCur}
 done
 for FileCur in `ls `
 do
   rm -fr FILEMEGABID
   grep 'No ' ${FileCur} | grep ' in AmdcDb' > FILEMEGABID
   if [ "$?" == "0" ]
   then 
     rm  ${FileCur}
   fi
   rm -fr FILEMEGABID
 done
 cd ..

 echo "--> Rename ${OUTPUTDIR} dir files"
 cd ${OUTPUTDIR}
 for FileCur in `ls`
 do
   FileCurNew=${FileCur%%_*}
   FileCurTOBETRANSFORMED=${FileCur}TOBETRANSFORMED
   mv ${FileCur} ${FileCurTOBETRANSFORMED}
   mv ${FileCurTOBETRANSFORMED} ${FileCurNew}
 done
 cd ..

 echo "--> Rename ${OUTPUTDIR} dir data files"
 cd ${OUTPUTDIR}
 for FileCur in `ls | grep data`
 do
   FileCurNew=${FileCur%.*}
   TableName=${FileCurNew##*.}
   FileCurTOBETRANSFORMED=${FileCur}TOBETRANSFORMED
   mv ${FileCur} ${FileCurTOBETRANSFORMED}
   mv ${FileCurTOBETRANSFORMED} ${TableName}.data
 done
 cd ..

 echo "--> Work out ${OUTPUTDIR} dir sql files"
 cd ${OUTPUTDIR}
 for FileCur in `ls | grep PREsql`
 do
   FileCurNew=${FileCur%.*}
   TableName=${FileCurNew##*.}
   rm -fr configure.txt
   grep ${TableName} ../${TagsIdFile} > configure.txt
   if [ "$?" == "0" ]
   then 
     TagIds=`grep ${TableName} ../${TagsIdFile} | awk '{print $2}' `
     GetNew_XXX_DATA_ID ;
     rm -fr configure.txt
     echo ${TableName} " " "${TagIds}" " " ${New_XXX_DATA_ID} " " ${WebAdress} > configure.txt 
     ../${CnvSqlToolKitDIR}/main 
   else
    echo "Table "  ${TableName} " not found in ../${TagsIdFile} "
    cat ../${TagsIdFile}
   fi
   rm -fr configure.txt
 done
 cd ..

 echo "--> Clean ${OUTPUTDIR} dir"
 cd ${OUTPUTDIR}
 for FileCur in `ls | grep Out.AmdcOracle.AM.`
 do
   rm  ${FileCur}
 done
 cd ..

 echo "--> Clean ${CnvSqlToolKitDIR} dir"
 cd ${CnvSqlToolKitDIR}
  rm -fr main main.o main.cc makefile
 cd ..
 
 
