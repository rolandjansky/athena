if [ -e main.C ]; then
    rm main.C
fi

cat <<EOF >> main.C
{
std::string incpath = gSystem->GetIncludePath();
incpath += "-I${TestArea}/InstallArea/include/TriggerMenuNtuple";
gSystem->SetIncludePath(incpath.c_str());
gSystem->Load("${TestArea}/InstallArea/${CMTCONFIG}/lib/libTriggerMenuNtupleDict.so");

gROOT->LoadMacro("TrigMuon.C+");
gROOT->LoadMacro("myTrigVarAnalysis.cxx+");
gROOT->LoadMacro("example.C+");

example("ntuple/trigntuple.root");
}
EOF
