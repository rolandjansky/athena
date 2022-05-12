
# setupATLAS
# lsetup "asetup MCProd,20.7.9.9.20,here" "rucio -w"
# (need Generators/Prophecy4fControl, e.g. "cmt co Generators/Prophecy4fControl", "cd Generators/Prophecy4fControl/cmt;make")
# lsetup panda
# voms-proxy-init -voms atlas
# ** change dataset name according to submitter, and remove "--official..." if you don't have a prod-role grid proxy certificate

# pathena --trf "export MYINDS=%IN; export MYOUTDS=%OUT.root; export IRANDOM=%RNDM:0; export IRANDOM=10\${IRANDOM}; export FIRSTEVENT=%FIRSTEVENT; ./generate_prophecy.sh" \
#     --inDS mc15_13TeV.343981.PowhegPythia8EvtGen_NNLOPS_nnlo_30_ggH125_gamgam.evgen.TXT.e5607 \
#     --outDS group.phys-higgs.user.schaffer.345060.PowhegPythia8EvtGen_NNLOPS_nnlo_30_ggH125_ZZ4l.evgen.e5607_190506_01/ \
#     --extFile=generate_prophecy.sh \
#     --nFilesPerJob=5 --nfiles 5000 \
#     --official --voms=atlas:/atlas/phys-higgs/Role=production

# with local copy of Hto4l:
# pathena --trf "export MYINDS=%IN; export MYOUTDS=%OUT.root; export IRANDOM=%RNDM:0; export IRANDOM=10\${IRANDOM}; export FIRSTEVENT=%FIRSTEVENT; ./generate_hto4l.sh" \
#     --inDS mc15_13TeV.343981.PowhegPythia8EvtGen_NNLOPS_nnlo_30_ggH125_gamgam.evgen.TXT.e5607 \
#     --outDS user.schaffer.345060.PowhegHto4lPythia8EvtGen_NNLOPS_nnlo_30_ggH125_ZZ4l.evgen.e5607_190601_01/ \
#     --extFile=generate_hto4l.sh \
#     --extFile=Hto4l \
#     --nFilesPerJob=8 --nfiles 5000 

pathena --trf "export MYINDS=%IN; export MYOUTDS=%OUT.root; export IRANDOM=%RNDM:0; export IRANDOM=10\${IRANDOM}; export FIRSTEVENT=%FIRSTEVENT; ./generate_hto4l.sh" \
    --inDS mc15_13TeV.343981.PowhegPythia8EvtGen_NNLOPS_nnlo_30_ggH125_gamgam.evgen.TXT.e5607 \
    --outDS user.schaffer.345060.PowhegHto4lPythia8EvtGen_NNLOPS_nnlo_30_ggH125_ZZ4l.evgen.e5607_190601_01/ \
    --extFile=generate_hto4l.sh \
    --nFilesPerJob=8 --nfiles 5000 
