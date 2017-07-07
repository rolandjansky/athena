if [ -e main.C ]; then
    rm main.C
fi

cat <<EOF >> main.C
{
std::string incpath = gSystem->GetIncludePath();
incpath += "-I${TestArea}/InstallArea/include/TriggerMenuNtuple";
gSystem->SetIncludePath(incpath.c_str());
gSystem->Load("${TestArea}/InstallArea/${CMTCONFIG}/lib/libTriggerMenuNtupleDict.so");

gROOT->LoadMacro("TrigMenu.C+");
gROOT->LoadMacro("L1_Cluster.C+");
gROOT->LoadMacro("L2_Cluster.C+");
gROOT->LoadMacro("L2_Track.C+");
gROOT->LoadMacro("TrigMuon.C+");
gROOT->LoadMacro("TrigVarInElectronChain.cxx+");
gROOT->LoadMacro("TrigVarInMuonChain.cxx+");
gROOT->LoadMacro("TrigVarInJetChain.cxx+");
gROOT->LoadMacro("HTMLUtil.cxx+");
gROOT->LoadMacro("createMenuPlot.C+");

createMenuPlot();
}
EOF
