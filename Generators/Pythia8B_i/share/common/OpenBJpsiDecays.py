########################################################
# OpenBJpsiDecays.py
# Opens all B->charmonium decays
# i.e. B->J/psi, psi(2S), chi_c0,1,2,h_c
########################################################

# B0
topAlg.Pythia8B.Commands += ['511:onIfAny = 443']
topAlg.Pythia8B.Commands += ['511:onIfAny = 100443']
topAlg.Pythia8B.Commands += ['511:onIfAny = 445']
topAlg.Pythia8B.Commands += ['511:onIfAny = 10441']
topAlg.Pythia8B.Commands += ['511:onIfAny = 10443']
topAlg.Pythia8B.Commands += ['511:onIfAny = 20443']
# B+/-
topAlg.Pythia8B.Commands += ['521:onIfAny = 443']
topAlg.Pythia8B.Commands += ['521:onIfAny = 100443']
topAlg.Pythia8B.Commands += ['521:onIfAny = 445']
topAlg.Pythia8B.Commands += ['521:onIfAny = 10441']
topAlg.Pythia8B.Commands += ['521:onIfAny = 10443']
topAlg.Pythia8B.Commands += ['521:onIfAny = 20443']
# Bs
topAlg.Pythia8B.Commands += ['531:onIfAny = 443']
topAlg.Pythia8B.Commands += ['531:onIfAny = 100443']
topAlg.Pythia8B.Commands += ['531:onIfAny = 445']
topAlg.Pythia8B.Commands += ['531:onIfAny = 10441']
topAlg.Pythia8B.Commands += ['531:onIfAny = 10443']
topAlg.Pythia8B.Commands += ['531:onIfAny = 20443']
# Bc
topAlg.Pythia8B.Commands += ['541:onIfAny = 443']
topAlg.Pythia8B.Commands += ['541:onIfAny = 100443']
topAlg.Pythia8B.Commands += ['541:onIfAny = 445']
topAlg.Pythia8B.Commands += ['541:onIfAny = 10441']
topAlg.Pythia8B.Commands += ['541:onIfAny = 10443']
topAlg.Pythia8B.Commands += ['541:onIfAny = 20443']
# LambdaB
topAlg.Pythia8B.Commands += ['5122:onIfAny = 443']
topAlg.Pythia8B.Commands += ['5122:onIfAny = 100443']
topAlg.Pythia8B.Commands += ['5122:onIfAny = 445']
topAlg.Pythia8B.Commands += ['5122:onIfAny = 10441']
topAlg.Pythia8B.Commands += ['5122:onIfAny = 10443']
topAlg.Pythia8B.Commands += ['5122:onIfAny = 20443']
# Xb+/-
topAlg.Pythia8B.Commands += ['5132:onIfAny = 443']
topAlg.Pythia8B.Commands += ['5132:onIfAny = 100443']
topAlg.Pythia8B.Commands += ['5132:onIfAny = 445']
topAlg.Pythia8B.Commands += ['5132:onIfAny = 10441']
topAlg.Pythia8B.Commands += ['5132:onIfAny = 10443']
topAlg.Pythia8B.Commands += ['5132:onIfAny = 20443']
# Xb
topAlg.Pythia8B.Commands += ['5232:onIfAny = 443']
topAlg.Pythia8B.Commands += ['5232:onIfAny = 100443']
topAlg.Pythia8B.Commands += ['5232:onIfAny = 445']
topAlg.Pythia8B.Commands += ['5232:onIfAny = 10441']
topAlg.Pythia8B.Commands += ['5232:onIfAny = 10443']
topAlg.Pythia8B.Commands += ['5232:onIfAny = 20443']
# Omega_b+/-
topAlg.Pythia8B.Commands += ['5332:onIfAny = 443']
topAlg.Pythia8B.Commands += ['5332:onIfAny = 100443']
topAlg.Pythia8B.Commands += ['5332:onIfAny = 445']
topAlg.Pythia8B.Commands += ['5332:onIfAny = 10441']
topAlg.Pythia8B.Commands += ['5332:onIfAny = 10443']
topAlg.Pythia8B.Commands += ['5332:onIfAny = 20443']
