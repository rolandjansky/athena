#########################################################################
#      preInclude.RHadrons.py - Chiara Debenedetti, 3 Jun 2011 #
#                              - Andy Haas, 4 Apr 2012
#########################################################################

import os
import shutil
import sys

from AthenaCommon import Logging
rhlog = Logging.logging.getLogger('RHadronConfig')

def create_rhadron_particles_file(CASE, MASS, MODEL, MASSX):
  """particles.txt Templates"""

  rhlog.info('INFO: MASSX is '+str(MASSX)+' GeV')
  MASS = (float(MASS)-100)  # The table masses are by default 100 GeV, so that part is subtracted

  particles = {
    "gluino" : {
      "generic" : [
        '       1000021   %3.3f   # ~g'             % ( MASS + 100.000 ),
        '       1000993   %3.3f   # ~g_glueball'    % ( MASS + 100.700 ),
        '       1009213   %3.3f   # ~g_rho+'        % ( MASS + 100.650 ),
        '       1009313   %3.3f   # ~g_K*0'         % ( MASS + 100.825 ),
        '       1009323   %3.3f   # ~g_K*+'         % ( MASS + 100.825 ),
        '       1009413   %3.3f   # ~g_D*+'         % ( MASS + 102.000 ),
        '       1009423   %3.3f   # ~g_D*0'         % ( MASS + 102.000 ),
        '       1009433   %3.3f   # ~g_D*s+'        % ( MASS + 102.200 ),
        '       1009113   %3.3f   # ~g_rho0'        % ( MASS + 100.650 ),
        '       1009223   %3.3f   # ~g_omega'       % ( MASS + 100.650 ),
        '       1009333   %3.3f   # ~g_phi'         % ( MASS + 101.800 ),
        '       1091114   %3.3f   # ~g_Delta-'      % ( MASS + 100.975 ),
        '       1092114   %3.3f   # ~g_Delta0'      % ( MASS + 100.975 ),
        '       1092214   %3.3f   # ~g_Delta+'      % ( MASS + 100.975 ),
        '       1092224   %3.3f   # ~g_Delta++'     % ( MASS + 100.975 ),
        '       1093114   %3.3f   # ~g_Sigma*-'     % ( MASS + 101.150 ),
        '       1093214   %3.3f   # ~g_Sigma*0'     % ( MASS + 101.150 ),
        '       1093224   %3.3f   # ~g_Sigma*+'     % ( MASS + 101.150 ),
        '       1093314   %3.3f   # ~g_Xi*-'        % ( MASS + 101.300 ),
        '       1093324   %3.3f   # ~g_Xi*0'        % ( MASS + 101.300 ),
        '       1093334   %3.3f   # ~g_Omega-'      % ( MASS + 101.600 ),
        '       1094114   %3.3f   # ~g_Sigmac*0'    % ( MASS + 102.300 ),
        '       1094214   %3.3f   # ~g_Sigmac*+'    % ( MASS + 102.300 ),
        '      -1009213   %3.3f   # ~g_rho-'        % ( MASS + 100.650 ),
        '      -1009313   %3.3f   # ~g_K*bar0'      % ( MASS + 100.825 ),
        '      -1009323   %3.3f   # ~g_K*-'         % ( MASS + 100.825 ),
        '      -1009413   %3.3f   # ~g_D*-'         % ( MASS + 102.000 ),
        '      -1009423   %3.3f   # ~g_aD*0'        % ( MASS + 102.000 ),
        '      -1009433   %3.3f   # ~g_D*s-'        % ( MASS + 102.200 ),
        '      -1091114   %3.3f   # ~g_Deltabar+'   % ( MASS + 100.975 ),
        '      -1092114   %3.3f   # ~g_Deltabar0'   % ( MASS + 100.975 ),
        '      -1092214   %3.3f   # ~g_Deltabar-'   % ( MASS + 100.975 ),
        '      -1092224   %3.3f   # ~g_Deltabar--'  % ( MASS + 100.975 ),
        '      -1093114   %3.3f   # ~g_Sigma*bar+'  % ( MASS + 101.150 ),
        '      -1093214   %3.3f   # ~g_Sigma*bar0'  % ( MASS + 101.150 ),
        '      -1093224   %3.3f   # ~g_Sigma*bar-'  % ( MASS + 101.150 ),
        '      -1093314   %3.3f   # ~g_Xi*bar+'     % ( MASS + 101.300 ),
        '      -1093324   %3.3f   # ~g_Xi*bar0'     % ( MASS + 101.300 ),
        '      -1093334   %3.3f   # ~g_Omegabar+'   % ( MASS + 101.600 ),
        '      -1094114   %3.3f   # ~g_Sigmac*bar0' % ( MASS + 102.300 ),
        '      -1094214   %3.3f   # ~g_Sigmac*bar-' % ( MASS + 102.300 ),
        '       1000039   %3.3f   # ~Gravitino'     % ( MASSX ),
        '       1000022   %3.3f   # ~chi_10'        % ( MASSX )
      ],
      "regge" : [
        '       1000021   %3.3f   # ~g'          % (MASS + 100.0   ),
        '       1000993   %3.3f   # ~g_glueball' % (MASS + 100.700 ),
        '       1009213   %3.3f   # ~g_rho+'     % (MASS + 100.700 ),
        '       1009113   %3.3f   # ~g_rho0'     % (MASS + 100.700 ),
        '       1009313   %3.3f   # ~g_K0'       % (MASS + 100.700 ),
        '       1009323   %3.3f   # ~g_K+'       % (MASS + 100.700 ),
        '       1093122   %3.3f   # ~g_Lambda0'  % (MASS + 100.700 ),
        '      -1009213   %3.3f   # ~g_rho-'     % (MASS + 100.700 ),
        '      -1009313   %3.3f   # ~g_K0bar'    % (MASS + 100.700 ),
        '      -1009323   %3.3f   # ~g_K-'       % (MASS + 100.700 ),
        '      -1093212   %3.3f   # ~g_Sigma0'   % (MASS + 100.700 ), # guess
        '       1093212   %3.3f   # ~g_aSigma0'  % (MASS + 100.700 ), # guess
        '       1000039   %3.3f   # ~Gravitino'  % ( MASSX ),
        '       1000022   %3.3f   # ~chi_10'     % ( MASSX )
      ],
      "intermediate" : [
        '       1000021   %3.3f   # ~g'          % ( MASS + 100.0),
        '       1000991   %3.3f   # ~g_glueball' % ( MASS + 100.330),
        '       1009211   %3.3f   # ~g_pi+'      % ( MASS + 100.330),
        '      -1009211   %3.3f   # ~g_pi-'      % ( MASS + 100.330),
        '       1009111   %3.3f   # ~g_pi0'      % ( MASS + 100.330),
        '       1009311   %3.3f   # ~g_K0'       % ( MASS + 100.460),
        '      -1009311   %3.3f   # ~g_K0bar'    % ( MASS + 100.460),
        '       1009321   %3.3f   # ~g_K+'       % ( MASS + 100.460),
        '      -1009321   %3.3f   # ~g_K-'       % ( MASS + 100.460),
        '       1093122   %3.3f   # ~g_Lambda0'  % ( MASS + 100.280),
        '       1092212   %3.3f   # ~g_proton'   % ( MASS + 100.660),
        '      -1092212   %3.3f   # ~g_aproton'  % ( MASS + 100.660),
        '       1092112   %3.3f   # ~g_neutron'  % ( MASS + 100.660),
        '      -1092112   %3.3f   # ~g_aneutron' % ( MASS + 100.660),
        '       1092214   %3.3f   # ~g_Delta+'   % ( MASS + 100.530),
        '      -1092214   %3.3f   # ~g_Delta-'   % ( MASS + 100.530),
        '       1092114   %3.3f   # ~g_Delta0'   % ( MASS + 100.530),
        '      -1092114   %3.3f   # ~g_aDelta0'  % ( MASS + 100.530),
        '       1000039   %3.3f   # ~Gravitino'  % ( MASSX ),
        '       1000022   %3.3f   # ~chi_10'     % ( MASSX )
      ]
    }, # end of gluino cases
    "stop" : {
      "generic" :[
        '     1000006  %3.3f   # ~t_1'       % ( MASS + 100.000 ),
        '     1000612  %3.3f   # ~T+  '      % ( MASS + 100.325 ),
        '     1000622  %3.3f   # ~T0  '      % ( MASS + 100.325 ),
        '     1000632  %3.3f   # ~T_s+'      % ( MASS + 100.500 ),
        '     1000642  %3.3f   # ~T_c0'      % ( MASS + 101.500 ),
        '     1000652  %3.3f   # ~T_b+'      % ( MASS + 104.800 ),
        '     1006113  %3.3f   # ~T_dd10'    % ( MASS + 100.650 ),
        '     1006211  %3.3f   # ~T_ud0+'    % ( MASS + 100.650 ),
        '     1006213  %3.3f   # ~T_ud1+'    % ( MASS + 100.650 ),
        '     1006223  %3.3f   # ~T_uu1++ '  % ( MASS + 100.650 ),
        '     1006311  %3.3f   # ~T_sd00  '  % ( MASS + 100.825 ),
        '     1006313  %3.3f   # ~T_sd10  '  % ( MASS + 100.825 ),
        '     1006321  %3.3f   # ~T_su0+  '  % ( MASS + 100.825 ),
        '     1006323  %3.3f   # ~T_su1+  '  % ( MASS + 100.825 ),
        '     1006333  %3.3f   # ~T_ss10  '  % ( MASS + 101.000 ),
        '    -1000006  %3.3f   # ~t_1bar'    % ( MASS + 100.000 ),
        '    -1000612  %3.3f   # ~Tbar-  '   % ( MASS + 100.325 ),
        '    -1000622  %3.3f   # ~Tbar0  '   % ( MASS + 100.325 ),
        '    -1000632  %3.3f   # ~Tbar_s-'   % ( MASS + 100.500 ),
        '    -1000642  %3.3f   # ~Tbar_c0'   % ( MASS + 101.500 ),
        '    -1000652  %3.3f   # ~Tbar_b-'   % ( MASS + 104.800 ),
        '    -1006113  %3.3f   # ~Tbar_dd10' % ( MASS + 100.650 ),
        '    -1006211  %3.3f   # ~Tbar_ud0-' % ( MASS + 100.650 ),
        '    -1006213  %3.3f   # ~Tbar_ud1-' % ( MASS + 100.650 ),
        '    -1006223  %3.3f   # ~Tbar_uu1--'% ( MASS + 100.650 ),
        '    -1006311  %3.3f   # ~Tbar_sd00 '% ( MASS + 100.825 ),
        '    -1006313  %3.3f   # ~Tbar_sd10 '% ( MASS + 100.825 ),
        '    -1006321  %3.3f   # ~Tbar_su0- '% ( MASS + 100.825 ),
        '    -1006323  %3.3f   # ~Tbar_su1- '% ( MASS + 100.825 ),
        '    -1006333  %3.3f   # ~Tbar_ss10 '% ( MASS + 101.000 ),
        '     1000039  %3.3f   # ~Gravitino' % ( MASSX ),
        '     1000022  %3.3f   # ~chi_10'    % ( MASSX )
      ],
      "regge" : [
        '     1000006  %3.3f   # ~t_1'       % ( MASS +  100.000 ),
        '     1000612  %3.3f   # ~T+  '      % ( MASS +  100.325 ),
        '     1000622  %3.3f   # ~T0  '      % ( MASS +  100.325 ),
        '     1006211  %3.3f   # ~T_ud0+'    % ( MASS +  100.650 ),
        '    -1000006  %3.3f   # ~t_1bar'    % ( MASS +  100.000 ),
        '    -1000612  %3.3f   # ~Tbar-  '   % ( MASS +  100.325 ),
        '    -1000622  %3.3f   # ~Tbar0  '   % ( MASS +  100.325 ),
        '    -1006211  %3.3f   # ~Tbar_ud0-' % ( MASS +  100.650 ),
        '     1000039  %3.3f   # ~Gravitino' % ( MASSX ),
        '     1000022  %3.3f   # ~chi_10'    % ( MASSX )
      ],
      "intermediate" : []
    }, # end of stop cases
    "sbottom" : {
      "generic" : [],
      "regge" : [
        '     1000005  %3.3f   # ~b'         % ( MASS + 100.000 ),
        '     1000512  %3.3f   # ~B0  '      % ( MASS + 100.325 ),
        '     1000522  %3.3f   # ~B-  '      % ( MASS + 100.325 ),
        '     1005211  %3.3f   # ~B_ud0'     % ( MASS + 100.650 ),
        '    -1000005  %3.3f   # ~bbar'      % ( MASS + 100.000 ),
        '    -1000512  %3.3f   # ~Bbar0  '   % ( MASS + 100.325 ),
        '    -1000522  %3.3f   # ~Bbar+  '   % ( MASS + 100.325 ),
        '    -1005211  %3.3f   # ~Bbar_ud0'  % ( MASS + 100.650 ),
        '     1000039  %3.3f   # ~Gravitino' % ( MASSX ),
        '     1000022  %3.3f   # ~chi_10'    % ( MASSX )
        ],
      "intermediate" : []
    } # end of sbottom cases
  } # end of pdgtable dict

  # Write a new particle.txt file
  with open('particles.txt','w') as f:
    for line in particles[CASE][MODEL]:
      f.writelines(line + "\n")

def create_rhadron_pdgtable(CASE, MASS, MODEL):
  """PDGTABLE Templates"""

  MASS = (float(MASS)-100) * 1000.0 # Convert mass to MeV (the table masses are by default 100 GeV, so that part is subtracted)

  pdgtable = {
    "gluino" : {
      "generic" : [
        'M 1000021                          %11.5E    +0.0E+00 -0.0E+00 ~g            0'  % ( MASS + 100.E+03   ),
        'W 1000021                          %11.5E    +0.0E+00 -0.0E+00 ~g            0'  % ( 0.E+00       ),
        'M 1009213                          %11.5E    +0.0E+00 -0.0E+00 ~g_rho+       +'  % ( MASS + 100.65E+03   ),
        'W 1009213                          %11.5E    +0.0E+00 -0.0E+00 ~g_rho+       +'  % ( 0.E+00      ),
        'M 1009113                          %11.5E    +0.0E+00 -0.0E+00 ~g_rho0       0'  % ( MASS + 100.65E+03   ),
        'W 1009113                          %11.5E    +0.0E+00 -0.0E+00 ~g_rho0       0'  % ( 0.E+00      ),
        'M 1091114                          %11.5E    +0.0E+00 -0.0E+00 ~g_Delta-     -'  % ( MASS + 100.975E+03  ),
        'W 1091114                          %11.5E    +0.0E+00 -0.0E+00 ~g_Delta-     -'  % ( 0.E+00      ),
        'M 1092114                          %11.5E    +0.0E+00 -0.0E+00 ~g_Delta0     0'  % ( MASS + 100.975E+03 ),
        'W 1092114                          %11.5E    +0.0E+00 -0.0E+00 ~g_Delta0     0'  % ( 0.E+00      ),
        'M 1092214                          %11.5E    +0.0E+00 -0.0E+00 ~g_Delta+     +'  % ( MASS + 100.975E+03 ),
        'W 1092214                          %11.5E    +0.0E+00 -0.0E+00 ~g_Delta+     +'  % ( 0.E+00      ),
        'M 1092224                          %11.5E    +0.0E+00 -0.0E+00 ~g_Delta++    ++' % ( MASS + 100.975E+03 ),
        'W 1092224                          %11.5E    +0.0E+00 -0.0E+00 ~g_Delta++    ++' % ( 0.E+00      ),
        'M 1000993                          %11.5E    +0.0E+00 -0.0E+00 ~g_glueball   0'  % ( MASS + 100.700E+03 ),
        'W 1000993                          %11.5E    +0.0E+00 -0.0E+00 ~g_glueball   0'  % ( 0.E+00        ),
        'M 1009313                          %11.5E    +0.0E+00 -0.0E+00 ~g_K*0        0'  % ( MASS + 100.825E+03 ),
        'W 1009313                          %11.5E    +0.0E+00 -0.0E+00 ~g_K*0        0'  % ( 0.E+00        ),
        'M 1009323                          %11.5E    +0.0E+00 -0.0E+00 ~g_K*+        +'  % ( MASS + 100.825E+03 ),
        'W 1009323                          %11.5E    +0.0E+00 -0.0E+00 ~g_K*+        +'  % ( 0.E+00        ),
        'M 1009223                          %11.5E    +0.0E+00 -0.0E+00 ~g_omega      0'  % ( MASS + 100.650E+03 ),
        'W 1009223                          %11.5E    +0.0E+00 -0.0E+00 ~g_omega      0'  % ( 0.E+00        ),
        'M 1009333                          %11.5E    +0.0E+00 -0.0E+00 ~g_phi        0'  % ( MASS + 101.800E+03 ),
        'W 1009333                          %11.5E    +0.0E+00 -0.0E+00 ~g_phi        0'  % ( 0.E+00        ),
        'M 1009413                          %11.5E    +0.0E+00 -0.0E+00 ~g_D*+        +'  % ( MASS + 102.000E+03 ),
        'W 1009413                          %11.5E    +0.0E+00 -0.0E+00 ~g_D*+        +'  % ( 0.E+00        ),
        'M 1009423                          %11.5E    +0.0E+00 -0.0E+00 ~g_D*0        0'  % ( MASS + 102.000E+03 ),
        'W 1009423                          %11.5E    +0.0E+00 -0.0E+00 ~g_D*0        0'  % ( 0.E+00        ),
        'M 1009433                          %11.5E    +0.0E+00 -0.0E+00 ~g_D*s+       +'  % ( MASS + 102.200E+03 ),
        'W 1009433                          %11.5E    +0.0E+00 -0.0E+00 ~g_D*s+       +'  % ( 0.E+00        ),
        'M 1093114                          %11.5E    +0.0E+00 -0.0E+00 ~g_Sigma*-    -'  % ( MASS + 101.150E+03  ),
        'W 1093114                          %11.5E    +0.0E+00 -0.0E+00 ~g_Sigma*-    -'  % ( 0.E+00        ),
        'M 1093214                          %11.5E    +0.0E+00 -0.0E+00 ~g_Sigma*0    0'  % ( MASS + 101.150E+03  ),
        'W 1093214                          %11.5E    +0.0E+00 -0.0E+00 ~g_Sigma*0    0'  % ( 0.E+00        ),
        'M 1093224                          %11.5E    +0.0E+00 -0.0E+00 ~g_Sigma*+    +'  % ( MASS + 101.150E+03  ),
        'W 1093224                          %11.5E    +0.0E+00 -0.0E+00 ~g_Sigma*+    +'  % ( 0.E+00        ),
        'M 1093314                          %11.5E    +0.0E+00 -0.0E+00 ~g_Xi*-       -'  % ( MASS + 101.300E+03  ),
        'W 1093314                          %11.5E    +0.0E+00 -0.0E+00 ~g_Xi*-       -'  % ( 0.E+00        ),
        'M 1093324                          %11.5E    +0.0E+00 -0.0E+00 ~g_Xi*0       0'  % ( MASS + 101.300E+03  ),
        'W 1093324                          %11.5E    +0.0E+00 -0.0E+00 ~g_Xi*0       0'  % ( 0.E+00        ),
        'M 1093334                          %11.5E    +0.0E+00 -0.0E+00 ~g_Omega-     -'  % ( MASS + 101.600E+03  ),
        'W 1093334                          %11.5E    +0.0E+00 -0.0E+00 ~g_Omega-     -'  % ( 0.E+00        ),
        'M 1094114                          %11.5E    +0.0E+00 -0.0E+00 ~g_Sigmac*0   0'  % ( MASS + 102.300E+03  ),
        'W 1094114                          %11.5E    +0.0E+00 -0.0E+00 ~g_Sigmac*0   0'  % ( 0.E+00        ),
        'M 1094214                          %11.5E    +0.0E+00 -0.0E+00 ~g_Sigmac*+   +'  % ( MASS + 102.300E+03  ),
        'W 1094214                          %11.5E    +0.0E+00 -0.0E+00 ~g_Sigmac*+   +'  % ( 0.E+00        )
      ],
      "regge" : [
        'M 1000021                          %11.5E    +0.0E+00 -0.0E+00 ~g            0'  % ( MASS + 100.E+03     ),
        'W 1000021                          %11.5E    +0.0E+00 -0.0E+00 ~g            0'  % ( 0.E+00       ),
        'M 1000993                          %11.5E    +0.0E+00 -0.0E+00 ~g_glueball   0'  % ( MASS + 100.700E+03  ),
        'W 1000993                          %11.5E    +0.0E+00 -0.0E+00 ~g_glueball   0'  % ( 0.E+00       ),
        'M 1009213                          %11.5E    +0.0E+00 -0.0E+00 ~g_rho+       +'  % ( MASS + 100.700E+03  ),
        'W 1009213                          %11.5E    +0.0E+00 -0.0E+00 ~g_rho+       +'  % ( 0.E+00      ),
        'M 1009113                          %11.5E    +0.0E+00 -0.0E+00 ~g_rho0       0'  % ( MASS + 100.700E+03  ),
        'W 1009113                          %11.5E    +0.0E+00 -0.0E+00 ~g_rho0       0'  % ( 0.E+00      ),
        'M 1009313                          %11.5E    +0.0E+00 -0.0E+00 ~g_K0         0'  % ( MASS + 100.700E+03  ),
        'W 1009313                          %11.5E    +0.0E+00 -0.0E+00 ~g_K0         0'  % ( 0.E+00      ),
        'M 1093122                          %11.5E    +0.0E+00 -0.0E+00 ~g_Lambda0    0'  % ( MASS + 100.280E+03  ),
        'W 1093122                          %11.5E    +0.0E+00 -0.0E+00 ~g_Lambda0    0'  % ( 0.E+00       ),
        'M 1009323                          %11.5E    +0.0E+00 -0.0E+00 ~g_K+         +'  % ( MASS + 100.700E+03  ),
        'W 1009323                          %11.5E    +0.0E+00 -0.0E+00 ~g_K+         +'  % ( 0.E+00      ),
        'M 1009113                          %11.5E    +0.0E+00 -0.0E+00 ~g_Lambda0    0'  % ( MASS + 100.700E+03  ),
        'W 1009113                          %11.5E    +0.0E+00 -0.0E+00 ~g_Lambda0    0'  % ( 0.E+00      )
      ],
      "intermediate" : [
        'M 1000021                          %11.5E    +0.0E+00 -0.0E+00 ~g            0'  % ( MASS + 100.E+03     ),
        'W 1000021                          %11.5E    +0.0E+00 -0.0E+00 ~g            0'  % ( 0.E+00       ),
        'M 1000991                          %11.5E    +0.0E+00 -0.0E+00 ~g_glueball   0'  % ( MASS + 100.330E+03  ),
        'W 1000991                          %11.5E    +0.0E+00 -0.0E+00 ~g_glueball   0'  % ( 0.E+00       ),
        'M 1009211                          %11.5E    +0.0E+00 -0.0E+00 ~g_pi+        +'  % ( MASS + 100.330E+03  ),
        'W 1009211                          %11.5E    +0.0E+00 -0.0E+00 ~g_pi+        +'  % ( 0.E+00       ),
        'M 1009111                          %11.5E    +0.0E+00 -0.0E+00 ~g_pi0        0'  % ( MASS + 100.330E+03  ),
        'W 1009111                          %11.5E    +0.0E+00 -0.0E+00 ~g_pi0        0'  % ( 0.E+00       ),
        'M 1009311                          %11.5E    +0.0E+00 -0.0E+00 ~g_K0         0'  % ( MASS + 100.460E+03  ),
        'W 1009311                          %11.5E    +0.0E+00 -0.0E+00 ~g_K0         0'  % ( 0.E+00       ),
        'M 1009321                          %11.5E    +0.0E+00 -0.0E+00 ~g_K+         +'  % ( MASS + 100.460E+03  ),
        'W 1009321                          %11.5E    +0.0E+00 -0.0E+00 ~g_K+         +'  % ( 0.E+00       ),
        'M 1093122                          %11.5E    +0.0E+00 -0.0E+00 ~g_Lambda0    0'  % ( MASS + 100.280E+03  ),
        'W 1093122                          %11.5E    +0.0E+00 -0.0E+00 ~g_Lambda0    0'  % ( 0.E+00       ),
        'M 1092212                          %11.5E    +0.0E+00 -0.0E+00 ~g_proton     +'  % ( MASS + 100.660E+03  ),
        'W 1092212                          %11.5E    +0.0E+00 -0.0E+00 ~g_proton     +'  % ( 0.E+00       ),
        'M 1092112                          %11.5E    +0.0E+00 -0.0E+00 ~g_neutron    0'  % ( MASS + 100.660E+03  ),
        'W 1092112                          %11.5E    +0.0E+00 -0.0E+00 ~g_neutron    0'  % ( 0.E+00       ),
        'M 1092214                          %11.5E    +0.0E+00 -0.0E+00 ~g_Delta+     +'  % ( MASS + 100.530E+03  ),
        'W 1092214                          %11.5E    +0.0E+00 -0.0E+00 ~g_Delta+     +'  % ( 0.E+00       ),
        'M 1092114                          %11.5E    +0.0E+00 -0.0E+00 ~g_Delta0     0'  % ( MASS + 100.530E+03  ),
        'W 1092114                          %11.5E    +0.0E+00 -0.0E+00 ~g_Delta0     0'  % ( 0.E+00       )
      ]
    }, # end of gluino cases
    "stop" : {
      "generic" : [
        'M 1000006                          %11.5E    +0.0E+00 -0.0E+00 ~t_1          0'  % ( MASS + 100.000E+03  ),
        'W 1000006                          %11.5E    +0.0E+00 -0.0E+00 ~t_1          0'  % ( 0.E+00       ),
        'M 1000612                          %11.5E    +0.0E+00 -0.0E+00 ~T+           +'  % ( MASS + 100.325E+03  ),
        'W 1000612                          %11.5E    +0.0E+00 -0.0E+00 ~T+           +'  % ( 0.E+00       ),
        'M 1000622                          %11.5E    +0.0E+00 -0.0E+00 ~T0           0'  % ( MASS + 100.325E+03  ),
        'W 1000622                          %11.5E    +0.0E+00 -0.0E+00 ~T0           0'  % ( 0.E+00       ),
        'M 1000632                          %11.5E    +0.0E+00 -0.0E+00 ~T_s+         +'  % ( MASS + 100.500E+03  ),
        'W 1000632                          %11.5E    +0.0E+00 -0.0E+00 ~T_s+         +'  % ( 0.E+00       ),
        'M 1000642                          %11.5E    +0.0E+00 -0.0E+00 ~T_c0         0'  % ( MASS + 101.500E+03  ),
        'W 1000642                          %11.5E    +0.0E+00 -0.0E+00 ~T_c0         0'  % ( 0.E+00       ),
        'M 1000652                          %11.5E    +0.0E+00 -0.0E+00 ~T_b+         +'  % ( MASS + 104.800E+03  ),
        'W 1000652                          %11.5E    +0.0E+00 -0.0E+00 ~T_b+         +'  % ( 0.E+00       ),
        'M 1006113                          %11.5E    +0.0E+00 -0.0E+00 ~T_dd10       0'  % ( MASS + 100.650E+03  ),
        'W 1006113                          %11.5E    +0.0E+00 -0.0E+00 ~T_dd10       0'  % ( 0.E+00       ),
        'M 1006211                          %11.5E    +0.0E+00 -0.0E+00 ~T_ud0+       +'  % ( MASS + 100.650E+03  ),
        'W 1006211                          %11.5E    +0.0E+00 -0.0E+00 ~T_ud0+       +'  % ( 0.E+00       ),
        'M 1006213                          %11.5E    +0.0E+00 -0.0E+00 ~T_ud1+       +'  % ( MASS + 100.650E+03  ),
        'W 1006213                          %11.5E    +0.0E+00 -0.0E+00 ~T_ud1+       +'  % ( 0.E+00       ),
        'M 1006223                          %11.5E    +0.0E+00 -0.0E+00 ~T_uu1++      ++' % ( MASS + 100.650E+03  ),
        'W 1006223                          %11.5E    +0.0E+00 -0.0E+00 ~T_uu1++      ++' % ( 0.E+00       ),
        'M 1006311                          %11.5E    +0.0E+00 -0.0E+00 ~T_sd00       0'  % ( MASS + 100.825E+03  ),
        'W 1006311                          %11.5E    +0.0E+00 -0.0E+00 ~T_sd00       0'  % ( 0.E+00       ),
        'M 1006313                          %11.5E    +0.0E+00 -0.0E+00 ~T_sd10       0'  % ( MASS + 100.825E+03  ),
        'W 1006313                          %11.5E    +0.0E+00 -0.0E+00 ~T_sd10       0'  % ( 0.E+00       ),
        'M 1006321                          %11.5E    +0.0E+00 -0.0E+00 ~T_su0+       +'  % ( MASS + 100.825E+03  ),
        'W 1006321                          %11.5E    +0.0E+00 -0.0E+00 ~T_su0+       +'  % ( 0.E+00       ),
        'M 1006323                          %11.5E    +0.0E+00 -0.0E+00 ~T_su1+       +'  % ( MASS + 100.825E+03  ),
        'W 1006323                          %11.5E    +0.0E+00 -0.0E+00 ~T_su1+       +'  % ( 0.E+00       ),
        'M 1006333                          %11.5E    +0.0E+00 -0.0E+00 ~T_ss10       0'  % ( MASS + 101.000E+03  ),
        'W 1006333                          %11.5E    +0.0E+00 -0.0E+00 ~T_ss10       0'  % ( 0.E+00       )
      ],
      "regge" : [
        'M 1000006                          %11.5E    +0.0E+00 -0.0E+00 ~t_1          0' % ( MASS + 100.000E+03  ),
        'W 1000006                          %11.5E    +0.0E+00 -0.0E+00 ~t_1          0' % ( 0.E+00       ),
        'M 1000612                          %11.5E    +0.0E+00 -0.0E+00 ~T+           +' % ( MASS + 100.325E+03  ),
        'W 1000612                          %11.5E    +0.0E+00 -0.0E+00 ~T+           +' % ( 0.E+00       ),
        'M 1000622                          %11.5E    +0.0E+00 -0.0E+00 ~T0           0' % ( MASS + 100.325E+03  ),
        'W 1000622                          %11.5E    +0.0E+00 -0.0E+00 ~T0           0' % ( 0.E+00       ),
        'M 1006211                          %11.5E    +0.0E+00 -0.0E+00 ~T_ud0+       +' % ( MASS + 100.650E+03  ),
        'W 1006211                          %11.5E    +0.0E+00 -0.0E+00 ~T_ud0+       +' % ( 0.E+00       )
      ],
      "intermediate" : []
    }, # end of stop cases
    "sbottom" : {
      "generic" : [],
      "regge" : [
        'M 1000005                          %11.5E    +0.0E+00 -0.0E+00 ~b            0' % ( MASS + 100.000E+03 ),
        'W 1000005                          %11.5E    +0.0E+00 -0.0E+00 ~b            0' % ( 0.E+00      ),
        'M 1000512                          %11.5E    +0.0E+00 -0.0E+00 ~B0           0' % ( MASS + 100.325E+03 ),
        'W 1000512                          %11.5E    +0.0E+00 -0.0E+00 ~B0           0' % ( 0.E+00      ),
        'M 1000522                          %11.5E    +0.0E+00 -0.0E+00 ~B-           -' % ( MASS + 100.325E+03 ),
        'W 1000522                          %11.5E    +0.0E+00 -0.0E+00 ~B-           -' % ( 0.E+00      ),
        'M 1005211                          %11.5E    +0.0E+00 -0.0E+00 ~B_ud0        0' % ( MASS + 100.650E+03 ),
        'W 1005211                          %11.5E    +0.0E+00 -0.0E+00 ~B_ud0        0' % ( 0.E+00      )
      ],
      "intermediate" : []
    } # end of sbottom cases
  } # end of pdgtable dict

  import ExtraParticles.PDGHelpers

  # Append the R-Hadron particles to the PDGTABLE.MeV file
  with open('PDGTABLE.MeV','a') as f:
    f.writelines("* R-Hadron additions: Case=%s, Model=%s, Mass=%d MeV\n" % (CASE, MODEL, (MASS  + 100000)))
    for line in pdgtable[CASE][MODEL]:
      f.writelines(line + "\n")

def addLineToPhysicsConfiguration(KEY, VALUE):
  os.system('touch PhysicsConfiguration.txt')
  newphysconfig = "{key} = {value}".format(key=KEY, value=VALUE)
  os.system('echo "%s" >> PhysicsConfiguration.txt' % newphysconfig)

def load_files_for_rhadrons_scenario(CASE, MASS, MODEL, MASSX):
  rhlog.info("MASS: "+str(MASS))
  rhlog.info("CASE: "+CASE)
  rhlog.info("MODEL: "+MODEL)
  rhlog.info("MASSX: "+str(MASSX))

  # Create custom PDGTABLE.MeV file
  create_rhadron_pdgtable(CASE, MASS, MODEL)

  # Download the process list
  proclistname = "ProcessList_{model}_{case}.txt".format(case=CASE, model=MODEL)
  os.system('get_files -remove -data %s' % proclistname)
  shutil.move(proclistname, 'ProcessList.txt')

  # Create particles.txt file
  create_rhadron_particles_file(CASE, MASS, MODEL, MASSX)

  # Remove existing physics configuration file ([MDJ]: FIXME: Is this happening earlier, or is it needed?)
  if os.path.isfile('PhysicsConfiguration.txt'):
    rhlog.warning("load_files_for_rhadrons_scenario() Found pre-existing PhysicsConfiguration.txt file - deleting.")
    os.remove('PhysicsConfiguration.txt')

  # Add additional physics configuration options
  if MODEL == "regge":
    addLineToPhysicsConfiguration("ReggeModel", "1.")

doG4SimConfig = True
# Note that for 'modern' samples where we want to use Pythia8, we should have the metadata
#  to determine which generator was used to produce the sample.  Assuming consistency is the goal.
usePythia8 = False
# Keep the run number, aka DSID
runNumber = -1
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
import PyUtils.AthFile as af
try:
  f = af.fopen(athenaCommonFlags.FilesInput()[0])
  if 'generators' in f.infos['tag_info']:
    usePythia8 = 'Py8' in f.infos['tag_info']['generators'] or 'Pythia8' in f.infos['tag_info']['generators']
  # MC Channel Number.  Try the standard two spots, and fall back to the run number for evgen
  if 'mc_channel_number' in f.infos and len(f.infos['mc_channel_number'])>0:
    runNumber = f.infos['mc_channel_number'][0]
  elif 'mc_channel_number' in f.infos['tag_info']:
    runNumber = f.infos['tag_info']['mc_channel_number']
  else:
    runNumber = f.infos['run_number'][0]
  if "StreamHITS" in f.infos["stream_names"]:
    from Digitization.DigitizationFlags import digitizationFlags
    simdict = digitizationFlags.specialConfiguration.get_Value()
    doG4SimConfig = False
  else:
    from G4AtlasApps.SimFlags import simFlags
    simdict = simFlags.specialConfiguration.get_Value()
except:
  from G4AtlasApps.SimFlags import simFlags
  simdict = simFlags.specialConfiguration.get_Value()

## Compulsory keys
assert "MASS" in simdict
MASSX=100.0
if simdict.has_key("MASSX"):
  MASSX = float(simdict["MASSX"])

rhlog.info('MASSX (neutralino/gravitino mass) set to '+str(MASSX)+' GeV')

if usePythia8:
  # Add the appropriate physics tool
  from G4AtlasApps.SimFlags import simFlags
  simFlags.PhysicsOptions += ["RHadronsPythia8PhysicsTool"]

  rhlog.info("Doing Pythia8")

  # In case we want to use Pythia8 for decays during simulation
  if simdict.has_key("LIFETIME"):

      # Set up the job options if they're going to be needed
      if float(simdict["LIFETIME"])>=1. or not hasattr(runArgs,'outputEVNT_TRFile') or not 'CASE' in simdict:
          # From the run number, load up the configuration.  Not the most beautiful thing, but this works.
          from glob import glob
          # Default position: look in cvmfs for job options
          cvmfs_mc15 = '/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/'
          JO = glob(cvmfs_mc15+'/share/DSID'+str(runNumber/1000)+'xxx/MC15.'+str(runNumber)+'*.py')
          if len(JO)>0:
              JO = JO[0]
          else:
              # Miss.  Try local
              JO = glob('MC15.'+str(runNumber)+'*.py')
              if len(JO)>0: JO=JO[0]
              else:
                  # Miss.  Try one directory deeper
                  JO = glob('*/MC15.'+str(runNumber)+'*.py')
                  if len(JO)>0: JO=JO[0]
                  else:
                      # Miss.  Fall back to datapath
                      for adir in os.environ['DATAPATH'].split(":"):
                          JO = glob(adir+'/MC15.'+str(runNumber)+'*.py')
                          if len(JO)>0:
                              JO=JO[0]
                              break
          if not JO:
              raise RuntimeError('Could not locate job options for DSID '+str(runNumber))
              sys.exit(1)
          # add any necessary elements to the runArgs here!
          runArgs.jobConfig = [JO.split('/')[-1] if '/' in JO else JO]
          runArgs.runNumber = runNumber
          # Set up evgenLog logger - use this one
          evgenLog=rhlog
          # Set up evgenConfig just for a holder
          class dummyClass():
              def __init(self):
                  pass
              keywords = [] # So that they can be +='d in
          evgenConfig = dummyClass()
          # Set up a fake pythia8...
          genSeq = dummyClass()
          genSeq.Pythia8 = dummyClass()
          genSeq.Pythia8.Commands = []
          # Set up a fake TestHepMC
          testSeq = dummyClass()
          testSeq.TestHepMC = dummyClass()
          # Block includes that we don't want running
          include.block('MC15JobOptions/MadGraphControl_SimplifiedModelPostInclude.py')
          include.block('MC15JobOptions/Pythia8_A14_NNPDF23LO_EvtGen_Common.py')
          include.block('MC15JobOptions/Pythia8_MadGraph.py')

          # Make sure all the files can be found
          from EvgenJobTransforms.jo_proxy import mk_jo_proxy
          mk_jo_proxy(cvmfs_mc15, "MC15JobOptions", "_joproxy15")

          # Updating JOBOPTSEARCHPATH env var on the athena side
          import re,os
          from AthenaCommon import Include
          Include.optionsPathEnv = os.environ[ 'JOBOPTSEARCHPATH' ]
          Include.optionsPath = re.split( ',|' + os.pathsep, Include.optionsPathEnv )

          # Include the job options themselves
          include(JO)

          # Build the param card, aka SLHA file
          from MadGraphControl.MadGraphUtils import build_param_card
          build_param_card(param_card_old='param_card.SM.%s.%s.dat'%(gentype,decaytype),param_card_new='SLHA_INPUT.DAT',masses=masses,decays=decays)

          # In case the CASE wasn't set, figure it out now
          if not 'CASE' in simdict:
              # Pop open the param card and figure out what's what.  Should be short, so we can stupid-read it
              my_param_card = open('SLHA_INPUT.DAT','r')
              masses = False
              my_case = None
              for l in my_param_card.readlines():
                  if 'BLOCK' in l.split('#')[0].split() and 'MASS' in l.split('#')[0].split():
                      masses = True
                      continue
                  elif 'BLOCK' in l.split('#')[0].split() and masses:
                      break
                  elif masses and len(l.split('#')[0].split())>1:
                      my_id = l.split('#')[0].split()[0]
                      my_m  = float(l.split('#')[0].split()[1])
                      # Mass cases for non-SUSY particles
                      if float(my_id)<1000000: continue
                      # Heavy; not what we're looking for
                      if my_m>4000.: continue
                      # Sleptons -- not what we're looking for
                      if my_id in ['1000011','1000012','1000013','1000014','1000015','1000016']: continue
                      if my_id in ['2000011','2000013','2000015']: continue
                      # Electroweakinos -- not what we're looking for
                      if my_id in ['1000022','1000023','1000024','1000025','1000035','1000037','1000039']: continue
                      # Squarks -- should not get here, so warn
                      if my_id in ['1000001','2000001','1000002','2000002','1000003','2000003','1000004','2000004']:
                          rhlog.warning('Encountered light squark.  Skipping, but this may indicate a problem.')
                          continue
                      if my_id in ['1000021']: # Gluino
                          my_case = 'gluino'
                      elif my_id in ['1000006','2000006']: # Stop
                          my_case = 'stop'
                      elif my_id in ['1000005','2000005']: # Sbottom
                          my_case = 'sbottom'
                      else:
                          raise RuntimeError('Uncertain how to deal with case for ID '+str(my_id))
                      # All the cases I handle so far
                      rhlog.info('Auto-detected case: '+my_case)
                  # Done with line handling
              my_param_card.close()
              if my_case is not None:
                  simdict['CASE'] = my_case
              else:
                  raise RuntimeError('No CASE in EVNT metadata and could not decode one from the param card')
          # Done handling case specification

  # Last step, load up the files
  load_files_for_rhadrons_scenario(simdict["CASE"], simdict["MASS"], "generic", MASSX)
  # Add any lines that were missing
  # In case we want to use Pythia8 for decays during simulation
  if simdict.has_key("LIFETIME"):

      if float(simdict["LIFETIME"])<1. and hasattr(runArgs,'outputEVNT_TRFile'):
          rhlog.warning('Lifetime specified at <1ns, but you are writing stopped particle positions.')
          rhlog.warning('Assuming that you mean to use infinite lifetimes, and ignoring the setting')
      else:
          addLineToPhysicsConfiguration("DoDecays","1")
          addLineToPhysicsConfiguration("HadronLifeTime", simdict["LIFETIME"])
      # If we reading particle records, and the lifetime is short, stop them as well
      if float(simdict['LIFETIME'])<1. and hasattr(runArgs,'inputEVNT_TRFile'):
          addLineToPhysicsConfiguration("DoDecays","1")
          addLineToPhysicsConfiguration("HadronLifeTime", 0.000001)
  # Done with the Pythia8 setup

else:
  # Add the appropriate physics tool
  from G4AtlasApps.SimFlags import simFlags
  simFlags.PhysicsOptions += ["RHadronsPythiaPhysicsTool"]

  assert "CASE" in simdict
  assert "MODEL" in simdict

  load_files_for_rhadrons_scenario(simdict["CASE"], simdict["MASS"], simdict["MODEL"], MASSX)

  ## Optional keys
  if simdict.has_key("XsecMultiplier"):
    addLineToPhysicsConfiguration("XsecMultiplier", simdict["XsecMultiplier"])
  if simdict.has_key("Mixing"):
    addLineToPhysicsConfiguration("Mixing", simdict["Mixing"])

  if doG4SimConfig:
    from G4AtlasApps.SimFlags import simFlags
    simFlags.PhysicsOptions += ["RHadronsPhysicsTool"]

  # In case we want to use Pythia for decays in line...
  if simdict.has_key("DECAYS"):
    addLineToPhysicsConfiguration("DoDecays","1")
    if simdict.has_key("LIFETIME"):
      addLineToPhysicsConfiguration("HadronLifeTime", simdict["LIFETIME"])
    else:
      addLineToPhysicsConfiguration("HadronLifeTime", "0.000001")

    def rhad_applycalomctruthstrategy():
      rhlog.error("ERROR rhad_applycalomctruthstrategy is obsolete")
      rhlog.error("Please request replacment configuration.")
      import sys
      sys.exit(1)
      ## from G4AtlasApps import AtlasG4Eng
      ## myDecay = AtlasG4Eng.G4Eng.Dict_MCTruthStrg.get('Decay')
      ## myDecay.add_Volumes('CALO::CALO', 1)
      ## AtlasG4Eng.G4Eng.menu_MCTruth.set_TruthStrategiesParameter("DecayPrimaryMinEnergy", -1)
      ## AtlasG4Eng.G4Eng.menu_MCTruth.set_TruthStrategiesParameter("DecaySecondaryMinEnergy", -1)


    simFlags.InitFunctions.add_function("preInitMCTruth", rhad_applycalomctruthstrategy)

    from AthenaCommon.AlgSequence import AlgSequence
    genSeq = AlgSequence()

    from PythiaRhad_i.PythiaRhad_iConf import PythiaRhad
    genSeq +=PythiaRhad()
    genSeq.PythiaRhad.useAtlasPythiaTune09=False
    genSeq.PythiaRhad.Tune_Name="PYTUNE_103"
    #genSeq.PythiaRhad.Tune_Name="ATLAS_20110003"
    genSeq.PythiaRhad.PythiaCommand += [
      "pyinit pylisti 12",
      "pyinit pylistf 1",
      "pystat 1 3 4 5",
      "pyinit dumpr 1 5",
      "pydat2 pmas 6 1 172.5",    # TOP mass
      "pydat2 pmas 24 1 80.399",  # PDG2010 W mass
      "pydat2 pmas 23 1 91.1876", # PDG2010 Z0 mass
    ]

    #-------------------------#
    # R-hadron commands below #
    #-------------------------#

    # Add some commands valid for both gluino and stop cases
    genSeq.PythiaRhad.PythiaCommand += [
      "pysubs ckin 3 18.",        # pT cut at 18 GeV
      #"pypars mstp 81 1",        # Old shower/multiple-interaction model (new model is not compatible with R-hadron fragmentation)
      #"pydat1 mstj 11 4",        # Set longitudinal fragmentation function to Pythia default
      "pymssm imss 1 1",          # General MSSM simulation
      "pymssm imss 3 1",          # Tell Pythia that rmss 3 below should be interpreted as the gluino pole mass
      "pymssm imss 5 1",          # Set stop, sbottom and stau masses and mixing by hand (26-28 for mixing not set!)
      #"pymssm rmss 1 4000.0",    # Photino mass
      "pymssm rmss 2 8000.0",    # Wino/Zino mass
      #"pymssm rmss 3 10000.0",    # Gluino  mass
      "pymssm rmss 4 40000.0",    # Higgsino mass parameter
      "pymssm rmss 6 1500.0",    # Left slepton mass
      "pymssm rmss 7 1200.0",    # Right slepton mass
      "pymssm rmss 8 4800.0",     # Left squark mass
      "pymssm rmss 9 4200.0",     # Right squark mass
      "pymssm rmss 10 4800.0",    # stop2 mass
      #"pymssm rmss 11 8000.0",    # sbottom1 mass
      #"pymssm rmss 12 8000.0",    # stop1 mass
      "pymssm rmss 13 1500.0",    # Left stau mass
      "pymssm rmss 14 1200.0",    # Right stau mass
      #"pymssm rmss 21 10000.0e9",  # Gravitino mass
      "pysubs msel 0",            # Turn off all processes
      "pypars mstp 111 0",        # Turn off master switch for fragmentation and decay
      #"pyinit pylisti 12",       # dumps the full decay table, etc.
      "pyinit pylistf 3"         # dumps pythia event
      #"pystat 2"
    ]

    pdg={}
    q3={}
    apflag={}
    names={}
    antinames={}
    masses={}

    # Gluino setups
    pdg[("generic", "gluino")] = [1000993, 1009213, 1009313, 1009323, 1009113, 1009223, 1009333, 1091114, 1092114, 1092214, 1092224, 1093114, 1093214, 1093224, 1093314, 1093324, 1093334, 0, 0, 0]
    q3[("generic", "gluino")] = [0, 3, 0, 3, 0, 0, 0, -3, 0, 3, 6, -3, 0, 3, -3, 0, -3, 0, 0, 0]
    apflag[("generic", "gluino")] = [0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0]
    names[("generic", "gluino")] = ["~g_ball", "~g_rho+", "~g_K*0", "~g_K*+", "~g_rho0", "~g_omega", "~g_phi", "~g_Dlt-", "~g_Dlt0", "~g_Dlt+", "~g_Dlt++", "~g_Sgm*-", "~g_Sgm*0", "~g_Sgm*+", "~g_Xi*-", "~g_Xi*0 ", "~g_Omg-", " ", " ", " "]
    antinames[("generic", "gluino")] = [" ", "~g_rho-", "~g_K*br0", "~g_K*-", " ", " ", " ", "~g_Dltb+", "~g_Dltb0", "~g_Dltb-", "~g_Dlb--", "~g_Sgmb+", "~g_Sgmb0", "~g_Sgmb-", "~g_Xibr+", "~g_Xib0", "~g_Omgb+", " ", " ", " "]
    masses[("generic", "gluino")] = [0.700, 0.650, 0.825, 0.825, 0.650, 0.650, 1.800, 0.975, 0.975, 0.975, 0.975, 1.150, 1.150, 1.150, 1.300, 1.300, 1.600, 0.650, 0.825, 0.825, 0.975, 0.975, 0.975, 0.975, 1.150, 1.150, 1.150, 1.300, 1.300, 1.600]

    pdg[("regge", "gluino")] = [1000993, 1009213, 1009113, 1009313, 1009323, 1093122, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    q3[("regge", "gluino")] = [0, 3, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    apflag[("regge", "gluino")] = [0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    names[("regge", "gluino")] = ["~g_gball", "~g_rho+", "~g_rho0", "~g_K0", "~g_K+", "~g_L0", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "]
    antinames[("regge", "gluino")] = [" ", "~g_rho-", " ", "~g_K0bar", "~g_K-", "~g_L0bar", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "]
    masses[("regge", "gluino")] = [0.330, 0.330, 0.330, 0.460, 0.460, 0.280, 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.]

    pdg[("intermediate", "gluino")] = [1000991, 1009211, 1009111, 1009311, 1009321, 1093122, 1092212, 1092112, 1092214, 1092114, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    q3[("intermediate", "gluino")] = [0, 3, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    apflag[("intermediate", "gluino")] = [0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    names[("intermediate", "gluino")] = ["~g_gball", "~g_pi+", "~g_pi0", "~g_K0", "~g_K+", "~g_L0", "~g_prot", "~g_neutr", "~g_Delt+", "~g_Delt0", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "]
    antinames[("intermediate", "gluino")] = [" ", "~g_pi-", " ", "~g_K0bar", "~g_K-", "~g_L0bar", "~g_aprot", "~g_aneut", "~g_Dltb-", "~g_Dltb0", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "]
    masses[("intermediate", "gluino")] = [0.330, 0.330, 0.330, 0.460, 0.460, 0.280, 0.660, 0.660, 0.530, 0.530, 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.]

    # Stop setups
    pdg[("generic", "stop")] = [1000612, 1000622, 1000632, 1000642, 1000652, 1006113, 1006211, 1006213, 1006223, 1006311, 1006313, 1006321, 1006323, 1006333, 0, 0, 0, 0, 0, 0]
    q3[("generic", "stop")] = [3, 0, 3, 0, 3, 0, 3, 3, 6, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0]
    apflag[("generic", "stop")] = [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0]
    names[("generic", "stop")] = ["~T+", "~T0", "~T_s+", "~T_c0", "~T_b+", "~T_dd10", "~T_ud0+", "~T_ud1+", "~T_uu1++", "~T_sd00", "~T_sd10", "~T_su0+", "~T_su1+", "~T_ss10", " ", " ", " ", " ", " ", " "]
    antinames[("generic", "stop")] = ["~Tb-", "~Tb0", "~Tb_s-", "~Tb_c0", "~Tb_b-", "~Tb_dd10", "~Tb_ud0-", "~Tb_ud1-", "~Tb_uu--", "~Tb_sd00", "~Tb_sd10", "~Tb_su0-", "~Tb_su1-", "~Tb_ss10", " ", " ", " ", " ", " ", " "]
    masses[("generic", "stop")] = [0.330, 0.330, 0.330, 0.460, 0.460, 0.280, 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.]

    pdg[("regge", "stop")] = [1000612, 1000622, 1006211, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    q3[("regge", "stop")] = [3, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    apflag[("regge", "stop")] = [1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    names[("regge", "stop")] = ["~T+", "~T0", "~T_ud0+", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "]
    antinames[("regge", "stop")] = ["~Tb-", "~Tb0", "~Tb_ud-", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "]
    masses[("regge", "stop")] = [0.330, 0.330, 0.330, 0.460, 0.460, 0.280, 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.]

    # Sbottom
    pdg[("regge", "sbottom")] = [1000512, 1000522, 1005211, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    q3[("regge", "sbottom")] = [0, -3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    apflag[("regge", "sbottom")] = [1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    names[("regge", "sbottom")] = ["~B0", "~B-", "~B_ud0", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "]
    antinames[("regge", "sbottom")] = ["~Bb0", "~Bb+", "~Bb_ud0", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "]
    masses[("regge", "sbottom")] = [0.330, 0.330, 0.330, 0.460, 0.460, 0.280, 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.]

    genSeq.PythiaRhad.RHadronPDGids = pdg[(simdict["MODEL"],simdict["CASE"])]
    genSeq.PythiaRhad.PygiveCommand = []
    for i in range(1,20):
      KC = str(400+i)
      genSeq.PythiaRhad.PygiveCommand += [
        "KCHG("+KC+",1)="+str(q3[(simdict["MODEL"],simdict["CASE"])][i-1]),
        "KCHG("+KC+",2)=0",
        "KCHG("+KC+",3)="+str(apflag[(simdict["MODEL"],simdict["CASE"])][i-1]),
        "KCHG("+KC+",4)="+str(pdg[(simdict["MODEL"],simdict["CASE"])][i-1]),
        "CHAF("+KC+",1)="+names[(simdict["MODEL"],simdict["CASE"])][i-1],
        "CHAF("+KC+",2)="+antinames[(simdict["MODEL"],simdict["CASE"])][i-1],
      ]
      genSeq.PythiaRhad.PygiveCommand += [
        "PMAS("+KC+",1)="+str(float(simdict["MASS"])+masses[(simdict["MODEL"],simdict["CASE"])][i-1])+"D0",
        "PMAS("+KC+",2)="+str(float(simdict["MASS"])+masses[(simdict["MODEL"],simdict["CASE"])][i-1])+"D0"
      ]
    rhlog.info("preInclude.Rhadrons PygiveCommand is:")
    rhlog.info(genSeq.PythiaRhad.PygiveCommand)

    if (simdict["MODEL"]=='regge'):
      genSeq.PythiaRhad.RunReggeModel=True
      genSeq.PythiaRhad.RunIntermediateModel=False
    if (simdict["MODEL"]=='intermediate'):
      genSeq.PythiaRhad.RunReggeModel=False
      genSeq.PythiaRhad.RunIntermediateModel=True

    if (simdict["CASE"]=='gluino'):
      genSeq.PythiaRhad.RunGluinoHadrons=True
      genSeq.PythiaRhad.RunStopHadrons=False
      genSeq.PythiaRhad.RunSbottomHadrons=False
      genSeq.PythiaRhad.randomtshift=50 # +-X ns, overrides tshift if non-zero
      genSeq.PythiaRhad.rh_decay=True
      genSeq.PythiaRhad.strip_out_rh=True
      genSeq.PythiaRhad.boost_rh_to_rest_frame=True
      genSeq.PythiaRhad.rotate_rh=True
      genSeq.PythiaRhad.translate_rh_to_stopping_position=True
      genSeq.PythiaRhad.EnableAfterInitialize=False # added for PythiaRhad_i-00-04-02
      genSeq.PythiaRhad.StoppingInput = [ [ 0,0,0,0,0,0 ] ]
      try:
        include("StoppingInput.txt")
      except:
        pass
      genSeq.PythiaRhad.PythiaCommand += [
        "pymssm imss 1 1",                          # General MSSM simulation
        "pymssm imss 3 1",                          # Tell Pythia that rmss 3 below should be interpreted as the gluino pole mass
        "pymssm imss 5 0",                          # allow pythia to calculate squark masses/mixings
        "pymssm rmss 1 "+str(MASSX),                # # Photino mass
        #"pymssm rmss 2 10000.0",                    # Wino/Zino mass
        "pymssm rmss 3 "+str(simdict["MASS"])+".0", # Gluino  mass
        #"pymssm rmss 4 10000.0",                    # Higgsino mass parameter
        #"pymssm rmss 7 10000.0",                    # Right slepton mass
        "pymssm rmss 8 4800.0",                     # Left squark mass
        "pymssm rmss 9 4200.0",                     # Right squark mass
        "pymssm rmss 10 4800.0",                    # stop2 mass
        "pymssm rmss 11 4200.0",                    # sbottom1 mass
        "pymssm rmss 12 4100.0",                    # stop1 mass
        "pymssm rmss 21 "+str(MASSX)+"e9",          # Gravitino mass (was MASSX before)
        "pymssm imss 11 1",                         # make N1 the LSP (1 would make N1 NLSP and gravitino LSP) (default is 0)
        "pymssm rmss 29 7.0e5",                     # Planck mass, controls BR(g~ -> g+Gravitino), leave high to avoid gravitino production (default is 2.4e18)
        "pydat3 mdcy 1000022 1 0",                  # kill neutralino decays
        # "pydat1 mstj 45 6",                       # allow CMshower->ttbar in gluino decays
        # "pydat1 mstj 43 1",                       # z definition in CM shower
        "pysubs msel 0",                            # Turn off all processes
        "pysubs msub 243 1",                        # turn on ffbar -> ~g~g
        "pysubs msub 244 1",                        # turn on gg -> ~g~g
        "pypars mstp 111 0",                        # Turn off master switch for fragmentation and decay
        "pypars mstp 127 1",                        # allow to continue even if there's no processes with non-vanishing xs
        "pyinit pylisti 12",                        # dumps the full decay table, etc.
        # "pyinit pylistf 1",                       # dumps pythia event
        "pyinit dumpr 0 100",                       # write out events 1 to 100
        "pystat 2"
      ]
      if simdict.has_key("NOGLUINOGLUONDECAY"):
        rhlog.info("preInclude.Rhadrons: NOGLUINOGLUONDECAY")
        genSeq.PythiaRhad.PythiaCommand += [
          "pydat3 mdme 1975 1 0",
          "pymssm imss 11 0"      # switch off gravitino, just to be sure we don't get gluon decay through it
        ]
        if simdict.has_key("NOGLUINOLIGHTSQUARKDECAY"):
          rhlog.info("preInclude.Rhadrons: NOGLUINOLIGHTSQUARKDECAY")
          genSeq.PythiaRhad.PythiaCommand += [
            "pydat3 mdme 2000 1 0",
            "pydat3 mdme 2001 1 0",
            "pydat3 mdme 2002 1 0",
            "pydat3 mdme 2003 1 0",
            "pydat3 mdme 2004 1 0"
          ]
        if simdict.has_key("NOGLUINOTTBARDECAY"):
          rhlog.info("preInclude.Rhadrons: NOGLUINOTTBARDECAY")
          genSeq.PythiaRhad.PythiaCommand += [
            "pydat3 mdme 2005 1 0"
          ]
        if ('GBALLPROB' in globals()):
          genSeq.PythiaRhad.GluinoBallProbability=GBALLPROB
    elif (simdict["CASE"]=='stop'):
      genSeq.PythiaRhad.RunGluinoHadrons=False
      genSeq.PythiaRhad.RunStopHadrons=True
      genSeq.PythiaRhad.RunSbottomHadrons=False
      genSeq.PythiaRhad.randomtshift=50 # +-X ns, overrides tshift if non-zero
      genSeq.PythiaRhad.rh_decay=True
      genSeq.PythiaRhad.strip_out_rh=True
      genSeq.PythiaRhad.boost_rh_to_rest_frame=True
      genSeq.PythiaRhad.rotate_rh=True
      genSeq.PythiaRhad.translate_rh_to_stopping_position=True
      genSeq.PythiaRhad.EnableAfterInitialize=False # added for PythiaRhad_i-00-04-02
      genSeq.PythiaRhad.StoppingInput = [ [ 0,0,0,0,0,0 ] ]
      try:
        include("StoppingInput.txt")
      except:
        pass
      genSeq.PythiaRhad.PythiaCommand += [
        "pymssm imss 1 1",                           # General MSSM simulation
        "pymssm imss 3 1",                           # Tell Pythia that rmss 3 below should be interpreted as the gluino pole mass
        "pymssm imss 5 1",                           # Set stop, sbottom and stau masses and mixing by hand (26-28 for mixing not set!)
        "pymssm rmss 1 "+str(MASSX),                 # Photino mass
        #"pymssm rmss 2 10000.0",                     # Wino/Zino mass
        "pymssm rmss 3 25000.0",                     # Gluino  mass
        #"pymssm rmss 4 10000.0",                     # Higgsino mass parameter
        #"pymssm rmss 7 10000.0",                     # Right slepton mass
        "pymssm rmss 8 4800.0",                      # Left squark mass
        "pymssm rmss 9 4200.0",                      # Right squark mass
        "pymssm rmss 10 4800.0",                     # stop2 mass
        "pymssm rmss 11 4200.0",                     # sbottom1 mass
        "pymssm rmss 12 "+str(simdict["MASS"])+".0", # stop1 mass
        "pymssm rmss 21 "+str(MASSX)+"e9",           # Gravitino mass
        "pymssm imss 11 1",                          # make N1 the LSP (1 would make N1 NLSP and gravitino LSP)
        "pymssm rmss 29 7.0e5",                      # Planck mass, controls BR(g~ -> g+Gravitino), leave high to avoid gravitino production
        "pydat3 mdcy 1000022 1 0",                   # kill neutralino decays
        # "pydat1 mstj 45 6",                        # allow CMshower->ttbar in gluino decays
        # "pydat1 mstj 43 1",                        # z definition in CM shower
        "pysubs msel 0",                             # Turn off all processes
        "pysubs msub 261 1",                         # turn on ffbar -> stop1stop1bar
        "pysubs msub 264 1",                         # turn on gg -> stop1stop1bar
        "pypars mstp 111 0",                         # Turn off master switch for fragmentation and decay
        "pyinit pylisti 12",                         # dumps the full decay table, etc.
        # "pyinit pylistf 1",                        # dumps pythia event
        "pyinit dumpr 0 100",                        # write out events 1 to 100
        "pystat 2"
      ]
    elif (simdict["CASE"]=='sbottom'):
      genSeq.PythiaRhad.RunGluinoHadrons=False
      genSeq.PythiaRhad.RunStopHadrons=False
      genSeq.PythiaRhad.RunSbottomHadrons=True
      genSeq.PythiaRhad.randomtshift=50 # +-X ns, overrides tshift if non-zero
      genSeq.PythiaRhad.rh_decay=True
      genSeq.PythiaRhad.strip_out_rh=True
      genSeq.PythiaRhad.boost_rh_to_rest_frame=True
      genSeq.PythiaRhad.rotate_rh=True
      genSeq.PythiaRhad.translate_rh_to_stopping_position=True
      genSeq.PythiaRhad.EnableAfterInitialize=False # added for PythiaRhad_i-00-04-02
      genSeq.PythiaRhad.StoppingInput = [ [ 0,0,0,0,0,0 ] ]
      try:
        include("StoppingInput.txt")
      except:
        pass
      genSeq.PythiaRhad.PythiaCommand += [
        "pymssm imss 1 1",                           # General MSSM simulation
        "pymssm imss 3 1",                           # Tell Pythia that rmss 3 below should be interpreted as the gluino pole mass
        "pymssm imss 5 1",                           # Set stop, sbottom and stau masses and mixing by hand (26-28 for mixing not set!)
        "pymssm rmss 1 "+str(MASSX),                 # Photino mass
        #"pymssm rmss 2 10000.0",                     # Wino/Zino mass
        "pymssm rmss 3 25000.0",                     # Gluino  mass
        #"pymssm rmss 4 10000.0",                     # Higgsino mass parameter
        #"pymssm rmss 7 10000.0",                     # Right slepton mass
        "pymssm rmss 8 4800.0",                      # Left squark mass
        "pymssm rmss 9 4200.0",                      # Right squark mass
        "pymssm rmss 10 4800.0",                     # stop2 mass
        "pymssm rmss 11 "+str(simdict["MASS"])+".0", # sbottom1 mass
        "pymssm rmss 12 4100.0",                     # stop1 mass
        "pymssm rmss 21 "+str(MASSX)+"e9",           # Gravitino mass
        "pymssm imss 11 1",                          # make N1 the LSP (1 would make N1 NLSP and gravitino LSP)
        "pymssm rmss 29 7.0e5",                      # Planck mass, controls BR(g~ -> g+Gravitino), leave high to avoid gravitino production
        "pydat3 mdcy 1000022 1 0",                   # kill neutralino decays
        # "pydat1 parj 64 -10000000.0",
        # "pydat3 mdcy 5 1 1",
        # "pydat1 mstj 45 6",                        # allow CMshower->ttbar in gluino decays
        # "pydat1 mstj 43 1",                        # z definition in CM shower
        "pysubs msel 0",                             # Turn off all processes
        "pysubs msub 287 1",                         # turn on ffbar -> sbottom1sbottom1bar
        "pysubs msub 289 1",                         # turn on gg -> sbottom1sbottom1bar
        "pysubs msub 291 1",                         # turn on bb -> sbottom1sbottom1
        "pypars mstp 111 0",                         # Turn off master switch for fragmentation and decay
        "pyinit pylisti 12",                         # dumps the full decay table, etc.
        # "pyinit pylistf 1",                        # dumps pythia event
        "pyinit dumpr 0 100",                        # write out events 1 to 100
        "pystat 2"
      ]

  del doG4SimConfig, simdict, MASSX
