
f=`pwd`
if test -d ${f} ; then find ${f} -name '*.py' -o -name '*.pyc' | sed -e 's#'"${f}"'/##'; fi
