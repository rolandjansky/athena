

if [ -d "$TestArea/Trigger/TrigCost" ]
then
    dir_list=`ls $TestArea/Trigger/TrigCost`
fi

pushd `pwd`

for dir in $dir_list
do
  wdir=$TestArea/Trigger/TrigCost/$dir
  if [ ! -d "$wdir" ]
  then
      continue
  fi

  echo "------------------------------------------------------------------"
  cd $wdir
  echo "${dir}"
  
  if [ "$1" = "u" -o  "$1" = "-u" ]
  then
      svn update
  fi
  if [ "$1" = "s" -o "$1" = "-s" ]
  then
      svn status -u
  fi
  if [ "$1" = "i" -o "$1" = "-i" ]
  then
      svn info
  fi

  if [ "$1" = "c" -o "$1" = "-c" ]
  then
      rm -r i686-slc5-gcc43-opt
  fi
  if [ "$1" = "m" -o "$1" = "-m" ]
  then
      cd cmt
      cmt bro make -j2
  fi

done

popd