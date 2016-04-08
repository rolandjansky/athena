# -*- coding: utf-8 -*-
# elastic scattering with Pythia8
# Z decays into leptons and tau decays disabled
job.Pythia8_i.Commands += ["SoftQCD:elastic = on", # elastic scattering
                           "SigmaElastic:setOwn = on", # t-generation accoring to elastic formula
                           "SigmaTotal:setOwn = on", # use own total cross section
                           "SigmaTotal:sigmaTot = 100.", # actual value of sig_tot
                           "SigmaElastic:bSlope = 18.", #  nuclear slope
                           "SigmaElastic:rho = 0.12.", #  rho-parameter
                           "SigmaElastic:lambda = 0.72", # form factor parameter
                           "SigmaElastic:phaseConst = 0.577", # Coulomb phase constant (gamma_E)
                           "SigmaElastic:tAbsMin = 4e-5", # t_min for event generation 
                           "Beams:allowMomentumSpread  = on", # set up beam class (incoming protons) 
                           "Beams:sigmaPxA = 0.010045", # set divergence in Px beam 1 (0.226 murad) 
                           "Beams:sigmaPyA = 0.009485", # set divergence in Py beam 1 
                           "Beams:sigmaPzA = 0.3955", # set energy smearing beam 1 in GeV  (1.13E-4%)
                           "Beams:sigmaPxB = 0.010045", # set divergence in Px beam 2 (0.226 murad) 
                           "Beams:sigmaPyB = 0.009485", # set divergence in Py beam 2 
                           "Beams:sigmaPzB = 0.3955", # set energy smearing beam 2 in GeV  (1.13E-4%)
                           "Beams:maxDevA = 100.0", # cut off smearing at 100 sigma
                           "Beams:maxDevB = 100.0", # cut off smearing at 100 sigma
                           "Beams:allowVertexSpread  = on", # beam class vertex 
                           "Beams:sigmaVertexX = 0.250 ", # spread in x
                           "Beams:sigmaVertexY = 0.237", # spread in y
                           "Beams:sigmaVertexZ = 0.0", # don't spread in z
                           "Beams:maxDevVertex = 10.0", # cut off spread at 10 sigma
			   "Random:setSeed=on", #from here added#########################################################
			   "Random:seed=4211",
                           ]
