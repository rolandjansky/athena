#!/bin/bash --norc
#
# Author Paolo Bagnaia <paolo.bagnaia@cern.ch>
# and Elena Solfaroli Camillocci <elena.solfaroli.camillocci@cern.ch>
# [Manual in CreateMDTConfig_readme.sh]
#
# choose the chambers for CreateMDTConfig_config - PB 100319 v 3.1.2
#
# ./CreateMDTConfig_chambers.sh -i inputfile -o outputfile
#
vers=3.1.2
echo -en "\n\n `date +'%d-%b-%y %T'` - CreateMDTConfig_" > /dev/stderr
echo -en "chambers.sh - vers $vers\n\n" > /dev/stderr
#
if [ $# != 4 ] ; then
   echo -en "./CreateMDTConfig_chambers.sh -i inputfile -o outputfile\n\n"
   exit
fi
infile="./doesnot/exist"
outfile="/dev/stdout"
while getopts "hi:o:" opt ; do
   case "$opt" in
      i) infile=$OPTARG       ;;
      o) outfile=$OPTARG      ;;
      h | * ) echo -en "./CreateMDTConfig_chambers.sh -i inputfile";
              echo -en " -o outputfile\n\n" ; exit ;;
   esac
done
echo -en "input file : $infile - output file : $outfile \n" > /dev/stderr
#
if [ "$ROOTSYS" = "" ] ; then
   echo -en "run root login - exit\n" > /dev/stderr
   exit
fi
if [ ! -f $infile ] ; then
   echo -en "file $infile not present - exit\n" > /dev/stderr
   exit
fi
if ( [ $outfile != "/dev/stdout" ] && [ -f $outfile ] ) ; then
   echo -en "file $outfile already present - exit\n" > /dev/stderr
   exit
fi
#
echo -e "#    created by $USER with v $vers on "`date +"%d-%b-%y %T"` > $outfile
ln -s $infile ./input.temp.root
#
cat << E_O_F >> temp.scandir.C
{
// scandir.C - V 2 PB 090611
// root -b -n -l -q scandir.C > some_file.txt
// cat this_file | grep -e '^use_ ' | cut -f 2 -d ' ' | cut -f 5 -d '/'
//
   int ncha=0;
   TString s0="/MDT", s1, s2, s3, scur;
   TKey *key1, *key2, *key3;
   TDirectory *curdir;
   TFile f("./input.temp.root");
   curdir=f.GetDirectory(s0,false,"");
   TIter next1 (curdir.GetListOfKeys());
//
   while ((key1=(TKey*)next1())) {
      s1=key1->GetName();
//    cout << " ===== key1 : '" << s1 << "'" << endl;
      if ((s1.BeginsWith("Barrel")) || (s1.BeginsWith("Endcap"))) {
         scur=s0 + "/" + s1;
//       cout << " ===== Barrel '" << scur << "'" << endl;
         curdir=f.GetDirectory(scur,false,"");
         TIter next2 (curdir.GetListOfKeys());
         while ((key2=(TKey*)next2())) {
            s2=key2->GetName();
//          cout << "\t ===== key2 : '" << s2 << "'" << endl;
            if (s2.BeginsWith("Sector"))  {
               scur=s0 + "/" + s1 + "/" + s2;
//             cout << "\t ===== Sector '" << scur << "'" << endl;
               curdir=f.GetDirectory(scur,false,"");
               TIter next3 (curdir.GetListOfKeys());
               while ((key3=(TKey*)next3())) {
                  s3=key3->GetName();
//                cout << "\t\t ===== key3 : '" << s3 << "'" << endl;
                  if ( s3 != "OVERVIEW") {
                     cout << "use_ " << scur << "/" << s3 << endl;
                     ncha++;
                  }
               }
            }
         }
      }
   }
//cout << " end - chambers : " << ncha << ". ==============" << endl;
}
E_O_F
root -b -n -l -q ./temp.scandir.C >> $outfile
#
rm -f ./input.temp.root
rm -f ./temp.scandir.C
exit
