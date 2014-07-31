#!/bin/sh

## Check arguments
if [ $# -lt 1 ] ; then
   echo "Usage: tct_dqwebdisplay.sh <monitorfile> [<dqconfig>] [<hanrunconfig>] [<han10mconfig>] [<han30mconfig>] [<handir>] [<wwwdir>]"
   exit 1 
fi

## intercept dryrun option if given
DRYRUN=0 
if [ "$1" = "-d" ] ; then
  DRYRUN=1
  shift
fi

## Check existence of template files
if [ ! -f $1 ] ; then
   echo "ERROR: Cannot find monitoring file " $1
   exit 1
fi

## setup configuration
monitorfile=$1
display="Tier0ChainTests.LocalDQWebDisplayCosmics"
if [ $# -gt 1 ] ; then
  display=$2
fi
dqconfig=`tct_dqutil.py ${display} dqconfig.config`

## han configuration files
defhanrun=`tct_dqutil.py ${display} dqconfig.hcfg`
hanrun=$defhanrun
if [ $# -gt 2 ] ; then
  if [ ! -f $3 ] ; then 
   echo "ERROR: Cannot find han configuration file $3. Exit."
   exit 1
  fi
  hanrun=$3
fi
defhan10m=`tct_dqutil.py ${display} dqconfig.hcfg_min10`
han10m=$defhan10m
if [ $# -gt 3 ] ; then
  if [ ! -f $4 ] ; then
   echo "ERROR: Cannot find han configuration file $4. Exit."
   exit 1
  fi
  han10m=$4
fi
defhan30m=`tct_dqutil.py ${display} dqconfig.hcfg_min30`
han30m=$defhan30m
if [ $# -gt 4 ] ; then
  if [ ! -f $5 ] ; then
   echo "ERROR: Cannot find han configuration file $5. Exit."
   exit 1
  fi
  han30m=$5
fi

## han results directory
defhandir=`tct_dqutil.py ${display} dqconfig.hanResultsDir`
handir=$defhandir
if [ $# -gt 5 ] ; then
  if [ ! -d $6 ]; then
   echo "ERROR: Cannot find directory $6. Exit."
   exit 1
  fi
  handir=$6
fi 
if [ ! -d $defhandir ]; then
  if [ ! $handir = $defhandir ]; then
    ln -s $handir $defhandir
  else
    mkdir -p $defhandir
  fi
fi

## dq web directory
defwebdir=`tct_dqutil.py ${display} dqconfig.htmlDir`
webdirtemp=$defwebdir
if [ $# -gt 6 ] ; then
  if [ ! -d $7 ]; then
   echo "ERROR: Cannot find directory $7. Exit."
   exit 1
  fi
  webdirtemp=$7
fi 
if [ ! -d $defwebdir ]; then
  if [ ! $webdirtemp = $defwebdir ]; then
    ln -s $webdirtemp $defwebdir
  else
    mkdir -p $defwebdir
  fi
fi

version=$(date +%u)
if [ $version = 7 ]; then
  version=0
fi
timestamp=$(date "+%Y%m%d%H%M%S")
dqdir=`echo $AtlasArea | cut -d "/" -f 8,9,10 | gawk '{ gsub("/","_") ; print $0}'`
finalwebdir=`echo $webdirtemp | gawk '{ gsub("wwwtemp","www") ; print $0}'`
mkdir -p $finalwebdir

webdirtemp=${webdirtemp}/${timestamp}
finalwebdir="${finalwebdir}/${dqdir}_${dqconfig}"
htmldir="http://atlas-project-fullchaintest.web.cern.ch/atlas-project-FullChainTest/tier0/dqm/www/${dqdir}_${dqconfig}"

## create dqwebdisplay

if [ -f MyCOOL_histo.db ] ; then
  rm -f MyCOOL_histo.db 
fi

if [ -d $webdirtemp ] ; then
  rm -Rf $webdirtemp
fi
if [ -d $finalwebdir ] ; then
  rm -Rf $finalwebdir
fi

## preparation
tct_CreateDB.py # create dummy cool database
echo # fall-back han configuration files
if [ ! -f $defhanrun ]; then 
  if [ ! $hanrun = $defhanrun ]; then
    ln -s $hanrun $defhanrun
  else
    hanconfigfile=/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/tct_reference_run.hcfg
    echo "WARNING: Using han configuration fall-back file: ${hanconfigfile}"
    ln -s $hanconfigfile $defhanrun
  fi
fi
if [ ! -f $defhan10m ]; then
  if [ ! $han10m = $defhan10m ]; then
    ln -s $han10m $defhan10m
  else
    hanconfigfile=/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/tct_reference_minutes10.hcfg
    echo "WARNING: Using han configuration fall-back file: ${hanconfigfile}"
    ln -s $hanconfigfile $defhan10m
  fi
fi
if [ ! -f $defhan30m ]; then
  if [ ! $han30m = $defhan30m ]; then
    ln -s $han30m $defhan30m
  else
    hanconfigfile=/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/tct_reference_minutes30.hcfg
    echo "WARNING: Using han configuration fall-back file: ${hanconfigfile}"
    ln -s $hanconfigfile $defhan30m
  fi
fi

## run dqwebdisplay
RUNCMD="DQWebDisplay.py $monitorfile $display $timestamp"
echo
echo ">> Now running command:"
echo ">> ===================="
echo "$RUNCMD"
echo 
if [ $DRYRUN -ne 1 ]; then
  $RUNCMD
  cp -f ${handir}/${timestamp}/*/*/*han.root ./ ; tct_fixhanrootnames.py
  mv $webdirtemp $finalwebdir # final webdir reflects atlas project and release 
fi
echo
echo ">> Finished command:"
echo ">> ================="
echo "$RUNCMD"
echo 

## make weblink file
rm -f dqweblink.html
cat > dqweblink.html <<EOF
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Frameset//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transititional.dtd">
<html>
  <head>
    <title>Home | TCT Redirection page</title>
       <META http-equiv="refresh" content="1; URL=$htmldir">
   </head>

<body>
You will be redirected automatically in 1 second to the <a href="$htmldir">tct DQ webpage</a>.
</body>
</html>
EOF

## this should only be reached if all successful ;-)
echo "ErrorCode=0 (OK)"

