#!/bin/sh

#check script for D3PD production
# v1.0 initial version


#fixed values
MAXLENGTHONPANDA=90

#default parameters
proj=mc11_7TeV
tag=p833
type=NTUP_SMWZ
input=datasets
inputtype=AOD
retpanda=1
typecheck=1
grid=0
gsetup=0
debug=0

#help
HELP="Usage: `basename $0` [-ngh] [-p <project>] [-q <d3pdtag>] [-t <NTUP type>] [-i <input dataset file>] [-d <input data type>] [-r <0 or 1>] [-l <debug level>]

Arguments:
   -p     Set project of D3PD (default:$proj)
   -q     Set tag of D3PD (default:$tag)
   -t     Set type of D3PD (default:$type)
   -i     Set input dataset file (default:$input)
          (text file in which input (D)AOD(ESD)s are listed)
   -d     Set input dataset type (default:$inputtype)
   -r     Set if force to retrieve panda information
          even if there are old files (default:$retpanda, 0:false,1:true)
   -l     Set debug level (default:$debug)
   -n     No check for NTUP type for submitted job
   -g     Use dq2-ls for type check (for NTUTP) before
   -h     Print Help
"
#argument check
while getopts p:q:t:i:d:r:l:gnh? OPT;do
  case $OPT in
    "p" ) proj="$OPTARG";;
    "q" ) tag="$OPTARG";;
    "t" ) type="$OPTARG";;
    "i" ) input="$OPTARG";;
    "d" ) inputtype="$OPTARG";;
    "r" ) retpanda="$OPTARG";;
    "l" ) debug="$OPTARG";;
    "g" ) grid=1;;
    "n" ) typecheck=0;;
    "h" ) echo  "$HELP" 1>&2; exit 0;;
    * ) echo  "$HELP" 1>&2; exit 0;;
  esac
done

#function for print
function myprint(){
  level=${2:-0}
  if [ $level -le $debug ];then
    echo $1
  fi
}

#function to retrieve panda information
function getPanda(){
  poutput=${1:-"pandaoutput"}
  pproj=${2:-$proj}
  ptag=${3:-$tag}
  echo "getting information from panda for $pproj $pptag to $poutput..."
  echo "wget -O $poutput.tmp \"http://panda.cern.ch/server/pandamon/query/?mode=taskquery&qDSInput=$pproj%25&qPriority=$ptag&qTASEARCH=TASK&qMore=&qsubmit=QuerySubmit\""
  wget -O $poutput.tmp "http://panda.cern.ch/server/pandamon/query/?mode=taskquery&qDSInput=$pproj%25&qPriority=$ptag&qTASEARCH=TASK&qMore=&qsubmit=QuerySubmit" >/dev/null 2>&1
  delimiter="http:\/\/panda.cern.ch\/server\/pandamon\/query?mode=showtask0\&reqid="
  sed "s/$delimiter/\n$delimiter/g" $poutput.tmp|grep $pproj |cut -d' ' -f 1,2,51|sed 's/href=//g'|sed 's/>//g' > $poutput
  rm -f $poutput.tmp
  echo ""
}

#function to set up grid
function setupGrid(){
  echo "dq2 client is not set up, now seting up..."
  source /afs/cern.ch/atlas/offline/external/GRID/ddm/DQ2Clients/setup.sh
  voms-proxy-info >/dev/null 2>&1
  if [ $? -ne 0 ];then
    voms-proxy-init -voms atlas
  fi
  gsetup=1
}

#start! print date
date

#check input dataset file
if [ ! -f "$input" ];then
  echo "please make input datasets file: $input"
  exit
fi

#get d3pd information from panda
d3pdlist="d3pdlist.$proj.$tag"
if [ $retpanda -eq 0 ] && [ -f $d3pdlist ];then
  echo "panda d3pd information file exists, use it:"
  ls -l $d3pdlist
  echo ""
else
  getPanda $d3pdlist $proj $tag
fi

#check if input datasets were already submitted to corresponding tag
reqFile=request.txt
reqForTypeFile=requestFor$type.txt
reqNotReady=notReady.txt
reqAborted=aborted.txt
submittedFile=submitted.txt
rm -f $reqFile $reqForTypeFile $submittedFile $reqNotReady $reqAborted

while read d;do
  if [ "$d" == "" ];then continue;fi
  echo $d
  prefix=`echo ${d%.$inputtype*}`
  prefix=`echo ${prefix%.*}` # remove merge/recon
  postfix=`echo ${d#*.${inputtype}.}|cut -d/ -f1`
  prefixtest=`echo $prefix|grep $proj`
  if [ "$prefixtest" = "" -o "$postfix" = "" ];then
    echo "error on $d, prefix=$prefix, postfix=$postfix"
    echo "not $proj, or name is wrong"
    exit
  fi

  #name length check for panda monitor (max MAXLENGTHONPANDA)
  tmpname=${prefix}.xxxxx.${postfix}_${tag} #xxxxx = recon or merge
  if [ ${#tmpname} -gt $MAXLENGTHONPANDA ];then
    echo "name is too long, is shortened"
    excess=$((${#tmpname}-$MAXLENGTHONPANDA))
    postfixlength=$((${#postfix}-$excess))
    postfix=${postfix: 0: $postfixlength}
  fi

  echo "check $prefix, $postfix"
  pl=(`grep $prefix $d3pdlist|grep $postfix`)
  if [ "$pl" != "" ];then
    echo job for $d was already submitted for $tag
    if [ $typecheck -eq 0 ];then
      echo "no type check, please check panda, or dq2"
      echo $d >> $submittedFile
    else
      echo "check if it was submitted for $type..."

      typetest=""
      if [ $grid -eq 1 ];then
        if [ $gsetup -eq 0 ];then
          setupGrid
        fi
        typetest=`dq2-ls $prefix*$type*$postfix*$tag/`
      fi

      if [ "$typetest" == "" ];then
        njobs=$((${#pl[*]}/3))
        i=0
        while [ $i -lt $njobs ];do
          url=${pl[(($i*3))]}
          status=${pl[(($i*3))+2]}
          wget -O inputtest "$url" >/dev/null 2>&1
          typetest=`grep $type inputtest`
          inputtest=`grep ${d#/} inputtest`
          if [ "$typetest" != "" ] && [ "$inputtest" != "" ];then
            break
          fi
          i=$(($i+1))
        done
      fi
      if [ "$typetest" != "" ];then
        echo "job for $type was submitted"
        echo $d >> $submittedFile
      else
        echo "job for $tag was submitted, but for $type is not submitted"
        echo $d >> $reqForTypeFile
      fi
    fi
  else
    echo "job for $type was not submitted"
    echo $d >> $reqFile
  fi
  echo ""
  echo ""
done<$input

#check input datasets
rm -f inputlist.*
if [ -f $reqFile ] || [ -f $reqForTypeFile ];then
  reqs=()
  if [ -f $reqFile ];then reqs=(${reqs[*]} $reqFile);fi
  if [ -f $reqForTypeFile ];then reqs=(${reqs[*]} $reqForTypeFile);fi
  for rf in ${reqs[*]};do
    mv $rf{,.tmp}
    while read d;do
      complete=""
      if [ $grid -eq 1 ];then
        if [ $gsetup -eq 0 ];then
          setupGrid
        fi
        complete=`dq2-ls -r ${d%/}/|grep " COMPLETE:"`
      fi
      if [ "$complete" == "" ];then
        prefix=`echo ${d%.$inputtype*}`
        prefix=`echo ${prefix%.*}` # remove merge/recon
        postfix=`echo ${d#*.$inputtype.}|cut -d/ -f1`
        inputtag=`echo ${d##*_}|cut -d/ -f1`
        inputlist=inputlist.$proj.$inputtag

        #name length check for panda monitor (max MAXLENGTHONPANDA)
        tmpname=${prefix}.xxxxx.${postfix} #xxxxx = recon or merge
        if [ ${#tmpname} -gt $MAXLENGTHONPANDA ];then
          echo "name is too long, is shortened"
          excess=$((${#tmpname}-$MAXLENGTHONPANDA))
          postfixlength=$((${#postfix}-$excess))
          postfix=${postfix: 0: $postfixlength}
        fi

        #if [ $retpanda -eq 0 ] && [ -f $inputlist ];then
        if [ -f $inputlist ];then
          #echo "input $inputtype information file for $proj, $inputtype exists, use it:"
          echo ""
        else
          getPanda $inputlist $proj $inputtag
        fi
        pl=(`grep $prefix $inputlist|grep $postfix`)
        if [ "$pl" == "" ];then
          echo "$d is not ready, job is not submitted"
          echo $d >> $reqNotReady
        else
          status=${pl[2]}
          if [ $status != "aborted" ];then
            echo "$d was defined"
            echo $d >> $rf
          else
            echo "$d was aborted, status=$status"
            echo $d >> $reqAborted
          fi
        fi
      else
        echo "$d is ready"
        echo $d >> $rf
      fi
    done < $rf.tmp
    rm -f $rf.tmp
  done
else
  echo "no requests are found!!!"
fi

#summary
printf "\n\n"
printf "*****************SUMMARY*****************\n"
printf "checked for output $proj, $tag, $type\n"
printf "input datasets are $inputlist in $input\n\n"

printf "output files:\n"
files=($reqFile $reqForTypeFile $submittedFile $reqNotReady $reqAborted)
explanations=("  %-24s: request datasets" "  %-24s: request datasets" "  %-24s: already submitted: already submitted" "  %-24s: input datasets are not ready" "  %-24s: tasks of input datasets were aborted")
i=0
while [ $i -lt ${#files[*]} ];do
  if [ -f ${files[$i]} ];then
    n=`wc ${files[$i]}|awk '{print $1}'`
  else
    n=0
  fi
  printf "${explanations[$i]}, %4d dtaset(s)\n" ${files[$i]} $n
  i=$(($i+1))
done

#printf "note:\n"
#printf "  For datasets in $reqForTypeFile, job was submitted for $tag, but not for $type\n"
#printf "  better to inform it to production manager\n\n"

if [ $typecheck -eq 0 ];then
  printf "  Datasets in $submittedFile are not checked for type\n"
  printf "  better to check panda/dq2 to confirm if $type of $tag exists or not\n"
fi
