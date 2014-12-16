#avoids ROOT crash
export ROOTENV_NO_HOME=1
#generate command we want
nightlyType=`echo $GAUDISVCROOT | sed 's/\// \/ /g' | gawk '{ print $(NF-6) }'`
GetCommand.py AMI=q400 >&  marksTESTBCT_Via_FCTScript.sh
chmod +x marksTESTBCT_Via_FCTScript.sh
./marksTESTBCT_Via_FCTScript.sh
rm -rf marksTESTBCT_Via_FCTScript.sh

