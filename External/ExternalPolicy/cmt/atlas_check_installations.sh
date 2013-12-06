
# made more generic by allowing for the
# -k=selection-regexp
# -x=exclusion-regexp
# syntax along with
# -s=new-search-path
from=
files=
installdir=
level=

need_files=

self="`basename $0`"

set -f
while test "$#" -gt 0; do
  arg=$1
  shift

  case ${arg} in

    -from=*)
      from=${arg#"-from="}
      need_files=
      ;;
    -files=*)
      files=${arg#"-files="}
      need_files=1
      ;;
    -installdir=*)
      installdir=${arg#"-installdir="}
      need_files=
      ;;
    -level=*)
      level=${arg#"-level="}
      need_files=
      ;;
    *)
       [ -z "$need_files" ] ||
       if [ "$files" ]; then files="$files ${arg}"; else files="${arg}"; fi
      ;;
  esac

done

case ${level} in
  [Ww][Aa][Rr][Nn][Ii][Nn][Gg]*) level=Warning;;
  [Ee][Rr][Rr][Oo][Rr]*) level=Error;;
  *) level=;;
esac
if [ -z "${level}" ]; then
if [ -z "${PEDANTIC}" ]; then
  level=Warning
else
  level=Error
fi
fi

retval=0

select=
exclude=

for ff in `echo ${files}` ; do
  case ${ff} in
    -s=*) from=${ff#"-s="}
     continue;;
    -k=*) select=${ff#"-k="}
     continue;;
    -x=*) exclude=${ff#"-x="}
     continue;;
  esac

  t="${ff#?}"
  c=${ff%"${t}"}
  if [ "$c" != "/" ] && [ -n "${from}" ]; then
    ff=${from}/${ff}
  fi

  set +f
  for f in `echo ${ff}`; do
    fn=
    if [ -n "${select}" ]; then
	[ -n "${fn}" ] || fn=`basename ${f}`
	[ `\expr ${fn} : "${select}"` -ne 0 ] || continue
    fi
    if [ -n "${exclude}" ]; then
	[ -n "${fn}" ] || fn=`basename ${f}`
	[ `\expr ${fn} : "${exclude}"` -eq 0 ] || continue
    fi

    [ "${makecmd}" ] && [ "${cmtmsg}" ] && echo "${self}: Checking ${f}"
    [ -f "${f}" ] || [ -d "${f}" ] ||
      { [ "${makecmd}" ] && [ "${cmtmsg}" ] && echo "${self}: Skipping non-existent ${f}" ; continue; }
    name=`basename ${f}`
    d=`dirname ${f}`
    d=`(cd ${d}; pwd -P)`
    if test -f ${installdir}/${name}.cmtref; then
      ref=`cat ${installdir}/${name}.cmtref`
      refd=`dirname ${ref}`
      refd=`(cd ${refd}; pwd -P)`
      if test ! "${d}" = "${refd}"; then
        \printf "${self}: ${level}: %s\n" \
	  "${name} already installed from different source ${refd}"
        [ ${level} = Warning ] || retval=1
      fi
    elif test -f ${installdir}/${name}; then
      if [ ! ${f} -nt ${installdir}/${name} ] && 
           ! \cmp -s ${f} ${installdir}/${name}; then
        \printf "${self}: ${level}: %s\n" \
         "${name} already installed from unknown source and differs"
        [ ${level} = Warning ] || retval=1
      fi
    fi
  done
  set -f
done

exit ${retval}
