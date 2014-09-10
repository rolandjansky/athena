
do_cleanup ()
{
  p=$1

  echo "# Doing cleanup in the installation area ${p}" 1>&2

  a=`find ${p}/InstallArea -name '*.cmtref' -print`

  for f in `echo ${a}`; do
    ref=`cat ${f}`
    d=`dirname ${f}`
    n=`basename ${ref}`

    if test ! -e "${ref}"; then
      echo "# ${d}/${n} is out-of-date in the installation area. It will be removed" 1>&2
      /bin/rm -f ${f}
      /bin/rm -f ${d}/${n}
    fi
  done
}

##set -x

unset installarea

pwd=`cmt -quiet show pwd`

for tp in `echo ${CMTPATH} | sed -e 's#:# #g'`; do 

  p=`(cd ${tp}; cmt -quiet show pwd)`

  if test -d ${p}/InstallArea; then
    #echo "InstallArea installed in ${p}"

    if test "${installarea}" = ""; then

      x=`echo ${pwd} | grep -e ${p}`
      status=$?

      if test "${status}" = 0; then

        # Try a cleanup in this install area
        do_cleanup ${p}
        exit 0
      fi
    fi
  fi

done


