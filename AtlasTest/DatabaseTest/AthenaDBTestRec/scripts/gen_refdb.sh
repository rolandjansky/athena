#/bin/ksh
# gen_refdb.sh <config> <connectionstring>
#
# e.g. for a local SQLite
#  gen_refdb.sh refc1301 "sqlite:X:refc1301.db:REFC1301"
# for Oracle
#  gen_refdb.sh refc1301 "oracle:server:schema:dbname:user:passwd"
#
CFILE=../config/${1}.txt
echo Generate reference conditions DB with all IOVs valid from file $CFILE
TestCoolRecWriter.exe $2 1 $CFILE 0
echo All done
