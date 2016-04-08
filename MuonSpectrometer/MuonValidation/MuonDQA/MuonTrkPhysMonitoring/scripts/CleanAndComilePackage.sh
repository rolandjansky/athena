cd ${1}; 

echo 'CMT Configuration..';
cmt config ;
echo "Sourcing CMT Setup...";
source setup.sh ;
echo "Make Clean.."
make clean;
echo "Make...";
make;
STATUS=$?

cd -;

if [ "$STATUS" -ne "0" ]; then
  echo "WARNING!!! Compile Failed of $1"
fi

