#!/bin/bash
#
#  PCDio wrapper
#

err () {
  echo '*' commands:
  echo
  echo $0 init tag[:rev] [filename]
  echo '*' create a new calibration tag=tag, revision=rev, origin=file:filename from the text file filename. By default rev=1, filename=pcd.dat .
  echo
  echo $0 copy tag[:rev] oldtag[:oldrev]
  echo '*' create a new calibration tag=tag, revision=rev, origin=\"oldtag:oldrev\" from existing calibration tag=oldtag, revision=oldrev. By default rev=1, oldrev=largest available revision for tag oldtag.
  echo
  echo $0 update tag[:rev] analid [opt]
  echo '*' take the calibration tag=tag, revision=rev, apply analisys analid, and create a new calibration tag=tag, revision=largest available revision + 1, origin=\"tag:rev:analid\". By default rev=largest available revision. opt=option accepted by extractAnalysis.
  echo
  #echo $0 list [tag]
  #echo '*' list calibrations which exist for tag tag. By default, list all existing calibrations.
  #echo
  echo $0 dump tag[:rev] [filename]
  echo '*' dump calibration tag=tag, revision=rev to the text file filename. By default rev=largest available revision, filename=pcd.dat .
  exit 1
}

#
# tag:rev parser
#

parse_tagrev () {
  have_rev=yes
  tag=${tagrev%:*}
  rev=${tagrev#*:}
  if [[ $tag == $tagrev ]]; then
    have_rev=no
    rev=1
  fi
}

#
# main switch
#

if [[ $# -lt 1 ]]; then
  err
fi

case $1 in
init)
  if [[ $# -lt 2 ]]; then
    err
  fi

  filename=
  if [[ $# -ge 3 ]]; then
    filename=$3
    if [[ -e $filename ]]; then
      cp $filename pcd.dat
    else
      echo file $filename does not exist
      exit 1
    fi
  fi

  tagrev=$2
  parse_tagrev

  echo talkToPCDDb -s $tag $rev file:$filename
  talkToPCDDb -s $tag $rev file:$filename
  ;;

copy)
  if [[ $# -lt 3 ]]; then
    err
  fi

  tagrev=$2
  parse_tagrev
  newtag=$tag
  newrev=$rev

  tagrev=$3
  parse_tagrev

  if [[ $have_rev == yes ]]; then
    talkToPCDDb -r $tag $rev
  else
    talkToPCDDb -r $tag
    rev=`cat pcd.rev`
  fi
  mv pcd.sav pcd.dat
  echo talkToPCDDb -s $newtag $newrev $tag:$rev
  talkToPCDDb -s $newtag $newrev $tag:$rev
  ;;

update)
  if [[ $# -lt 3 ]]; then
    err
  fi

  tagrev=$2
  parse_tagrev
  talkToPCDDb -r $tag
  newrev=`cat pcd.rev`
  if [[ $have_rev == yes ]]; then
    talkToPCDDb -r $tag $rev
  fi
  newrev=$[$newrev + 1]
  mv pcd.sav pcd.dat

  analid=$3
  opt=
  if [[ $# -ge 4 ]]; then
    opt=$4
  fi

  temp=$CAN_CALIB_DB
  export CAN_CALIB_DB=oracle://ATLAS_COOLPROD/ATLAS_COOLONL_PIXEL
  echo extractAnalysis $opt $analid
  extractAnalysis $opt $analid
  export CAN_CALIB_DB=$temp
  mv pcd.sav pcd.dat
  echo talkToPCDDb -s $tag $newrev $tag:$rev:$analid
  talkToPCDDb -s $tag $newrev $tag:$rev:$analid
  ;;

list)
  ;;

dump)
  if [[ $# -lt 2 ]]; then
    err
  fi

  tagrev=$2
  parse_tagrev
  if [[ $have_rev == yes ]]; then
    talkToPCDDb -r $tag $rev
  else
    talkToPCDDb -r $tag
  fi
  mv pcd.sav pcd.dat

  if [[ $# -ge 3 ]]; then
    filename=$3
    cp pcd.dat $filename
  fi
  ;;

*)
  echo pcd: unrecognized command $1
  err

esac

exit 0
