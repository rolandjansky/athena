
unset IP
for p in `echo ${CMTPATH} | sed -e 's#:# #g'`; do 
  if test -d ${p}/InstallArea; then
    export IP="${IP} -I${p}/InstallArea/include"
  fi
done
echo ${IP}

