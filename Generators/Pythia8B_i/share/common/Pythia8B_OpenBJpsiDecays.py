########################################################
# OpenBJpsiDecays.py
# Opens all B->charmonium decays
# i.e. B->J/psi, psi(2S), chi_c0,1,2,h_c
########################################################

# B0
genSeq.Pythia8B.Commands += ['511:onIfAny = 443']
genSeq.Pythia8B.Commands += ['511:onIfAny = 100443']
genSeq.Pythia8B.Commands += ['511:onIfAny = 445']
genSeq.Pythia8B.Commands += ['511:onIfAny = 10441']
genSeq.Pythia8B.Commands += ['511:onIfAny = 10443']
genSeq.Pythia8B.Commands += ['511:onIfAny = 20443']
# B+/-
genSeq.Pythia8B.Commands += ['521:onIfAny = 443']
genSeq.Pythia8B.Commands += ['521:onIfAny = 100443']
genSeq.Pythia8B.Commands += ['521:onIfAny = 445']
genSeq.Pythia8B.Commands += ['521:onIfAny = 10441']
genSeq.Pythia8B.Commands += ['521:onIfAny = 10443']
genSeq.Pythia8B.Commands += ['521:onIfAny = 20443']
# Bs
genSeq.Pythia8B.Commands += ['531:onIfAny = 443']
genSeq.Pythia8B.Commands += ['531:onIfAny = 100443']
genSeq.Pythia8B.Commands += ['531:onIfAny = 445']
genSeq.Pythia8B.Commands += ['531:onIfAny = 10441']
genSeq.Pythia8B.Commands += ['531:onIfAny = 10443']
genSeq.Pythia8B.Commands += ['531:onIfAny = 20443']
# Bc
genSeq.Pythia8B.Commands += ['541:onIfAny = 443']
genSeq.Pythia8B.Commands += ['541:onIfAny = 100443']
genSeq.Pythia8B.Commands += ['541:onIfAny = 445']
genSeq.Pythia8B.Commands += ['541:onIfAny = 10441']
genSeq.Pythia8B.Commands += ['541:onIfAny = 10443']
genSeq.Pythia8B.Commands += ['541:onIfAny = 20443']
# LambdaB
genSeq.Pythia8B.Commands += ['5122:onIfAny = 443']
genSeq.Pythia8B.Commands += ['5122:onIfAny = 100443']
genSeq.Pythia8B.Commands += ['5122:onIfAny = 445']
genSeq.Pythia8B.Commands += ['5122:onIfAny = 10441']
genSeq.Pythia8B.Commands += ['5122:onIfAny = 10443']
genSeq.Pythia8B.Commands += ['5122:onIfAny = 20443']
# Xb+/-
genSeq.Pythia8B.Commands += ['5132:onIfAny = 443']
genSeq.Pythia8B.Commands += ['5132:onIfAny = 100443']
genSeq.Pythia8B.Commands += ['5132:onIfAny = 445']
genSeq.Pythia8B.Commands += ['5132:onIfAny = 10441']
genSeq.Pythia8B.Commands += ['5132:onIfAny = 10443']
genSeq.Pythia8B.Commands += ['5132:onIfAny = 20443']
# Xb
genSeq.Pythia8B.Commands += ['5232:onIfAny = 443']
genSeq.Pythia8B.Commands += ['5232:onIfAny = 100443']
genSeq.Pythia8B.Commands += ['5232:onIfAny = 445']
genSeq.Pythia8B.Commands += ['5232:onIfAny = 10441']
genSeq.Pythia8B.Commands += ['5232:onIfAny = 10443']
genSeq.Pythia8B.Commands += ['5232:onIfAny = 20443']
# Omega_b+/-
genSeq.Pythia8B.Commands += ['5332:onIfAny = 443']
genSeq.Pythia8B.Commands += ['5332:onIfAny = 100443']
genSeq.Pythia8B.Commands += ['5332:onIfAny = 445']
genSeq.Pythia8B.Commands += ['5332:onIfAny = 10441']
genSeq.Pythia8B.Commands += ['5332:onIfAny = 10443']
genSeq.Pythia8B.Commands += ['5332:onIfAny = 20443']
