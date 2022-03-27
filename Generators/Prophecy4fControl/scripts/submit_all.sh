pathena --trf "export MYINDS=%IN; export MYOUTDS=%OUT.root; export IRANDOM=%RNDM:0; export IRANDOM=10\${IRANDOM}; export FIRSTEVENT=%FIRSTEVENT; ./generate_prophecy.sh" \
    --inDS mc15_13TeV.343981.PowhegPythia8EvtGen_NNLOPS_nnlo_30_ggH125_gamgam.evgen.TXT.e5607 \
    --outDS user.schaffer.345060.PowhegPythia8EvtGen_NNLOPS_nnlo_30_ggH125_ZZ4l.evgen.e5607_190506_01/ \
    --extFile=generate_prophecy.sh \
    --nFilesPerJob=5 --nfiles 5000 
