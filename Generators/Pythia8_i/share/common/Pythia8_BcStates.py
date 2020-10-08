genSeq.Pythia8.Commands += ['541:m0 = 6.2749']# PDG 2018
genSeq.Pythia8.Commands += ['541:tau0 = 1.5199e-01']# PDG 2018
genSeq.Pythia8.Commands += ['543:m0 = 6.346'] # M(Bc)+71.1 MeV
genSeq.Pythia8.Commands += ['100541:all B_c(2S)+ B_c(2S)- 1 3 0 6.8423 0 0 0 0'] # M(Bc)+288.3+2m(pi+)
genSeq.Pythia8.Commands += ['100541:addChannel 1 1.0 0 541 211 -211']  
genSeq.Pythia8.Commands += ['100543:all B*_c(2S)+ B*_c(2S)- 3 3 0 6.873 0 0 0 0'] # M(Bc2S)+30.7 MeV
genSeq.Pythia8.Commands += ['100543:addChannel 1 1.0 0 100541 22'] 
