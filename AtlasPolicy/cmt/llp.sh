
unset LLP
for p in `echo ${CMTPATH} | sed -e 's#:# #g'`; do 
  if test -d ${p}/InstallArea; then
    export LLP=${LLP}:"${p}/InstallArea/${CMTCONFIG}/lib"
  fi
done
echo ${LLP}

