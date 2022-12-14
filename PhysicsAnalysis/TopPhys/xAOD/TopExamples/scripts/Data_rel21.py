# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import TopExamples.grid
import TopExamples.ami

# Period containers
DATA15 = ['data15_13TeV.periodA.physics_Main.PhysCont.DAOD_{}.grp15_v01_p3180',
          'data15_13TeV.periodC.physics_Main.PhysCont.DAOD_{}.grp15_v01_p3180',
          'data15_13TeV.periodD.physics_Main.PhysCont.DAOD_{}.grp15_v01_p3180',
          'data15_13TeV.periodE.physics_Main.PhysCont.DAOD_{}.grp15_v01_p3180',
          'data15_13TeV.periodF.physics_Main.PhysCont.DAOD_{}.grp15_v01_p3180',
          'data15_13TeV.periodG.physics_Main.PhysCont.DAOD_{}.grp15_v01_p3180',
          'data15_13TeV.periodH.physics_Main.PhysCont.DAOD_{}.grp15_v01_p3180',
          'data15_13TeV.periodJ.physics_Main.PhysCont.DAOD_{}.grp15_v01_p3180',]

DATA16 = ['data16_13TeV.periodA.physics_Main.PhysCont.DAOD_{}.grp16_v01_p3180',
          'data16_13TeV.periodB.physics_Main.PhysCont.DAOD_{}.grp16_v01_p3180',
          'data16_13TeV.periodC.physics_Main.PhysCont.DAOD_{}.grp16_v01_p3180',
          'data16_13TeV.periodD.physics_Main.PhysCont.DAOD_{}.grp16_v01_p3180',
          'data16_13TeV.periodE.physics_Main.PhysCont.DAOD_{}.grp16_v01_p3180',
          'data16_13TeV.periodF.physics_Main.PhysCont.DAOD_{}.grp16_v01_p3180',
          'data16_13TeV.periodG.physics_Main.PhysCont.DAOD_{}.grp16_v01_p3180',
          'data16_13TeV.periodI.physics_Main.PhysCont.DAOD_{}.grp16_v01_p3180',
          'data16_13TeV.periodK.physics_Main.PhysCont.DAOD_{}.grp16_v01_p3180',
          'data16_13TeV.periodL.physics_Main.PhysCont.DAOD_{}.grp16_v01_p3180',]

DATA17 = ['data17_13TeV.periodB.physics_Main.PhysCont.DAOD_{}.grp17_v01_p3213',
          'data17_13TeV.periodC.physics_Main.PhysCont.DAOD_{}.grp17_v01_p3213',
          'data17_13TeV.periodD.physics_Main.PhysCont.DAOD_{}.grp17_v01_p3267',
          'data17_13TeV.periodE.physics_Main.PhysCont.DAOD_{}.grp17_v01_p3267',
          # Period F is waiting for feedback on the runs to use...
          ]

DATA15RUN = [ 'data15_13TeV.00276689.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00284006.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00271048.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00270806.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00281381.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00284427.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00283780.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00279867.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00282784.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00279928.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00276336.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00276731.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00271516.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00276329.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00276416.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00279284.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00270953.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00276511.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00276245.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00283608.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00276952.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00282992.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00267152.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00281143.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00279345.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00279259.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00280423.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00270816.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00283074.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00280319.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00276073.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00280977.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00280368.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00271388.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00267073.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00280673.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00281070.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00279685.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00281411.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00284285.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00276161.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00281385.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00276212.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00281317.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00267167.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00276790.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00279813.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00276183.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00280500.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00278912.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00279932.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00283155.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00278968.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00282631.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00284473.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00266919.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00276954.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00266904.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00281130.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00283270.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00281075.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00278748.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00280464.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00276189.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00281074.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00271421.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00284484.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00279169.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00271744.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00280520.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00280950.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00270588.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00279279.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00279764.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00267162.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00270949.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00280273.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00276330.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00280614.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00280853.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00282625.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00276262.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00279515.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00270448.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00278727.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00267639.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00278970.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00270441.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00278880.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00280231.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00276176.physics_Main.deriv.DAOD_{}.r9412_p3083_p3180',
              'data15_13TeV.00282712.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00271595.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00280862.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00276778.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00279598.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00276181.physics_Main.deriv.DAOD_{}.r9412_p3083_p3180',
              'data15_13TeV.00283429.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00284420.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00284154.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00279984.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00267638.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00284213.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00271298.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00280753.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data15_13TeV.00276147.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              ]

# all data16 runs available with 21.0.19.4 

DATA16RUN = [ 'data16_13TeV.00297730.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00298595.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00298609.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00298633.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00298687.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00298690.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00298771.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00298773.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00298862.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00298967.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00299055.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00299144.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00299147.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00299184.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00299241.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00299243.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00299288.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00299343.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00299584.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00300279.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00300345.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00300415.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00300418.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00300487.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00300540.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00300571.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00300600.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00300655.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00300687.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00300784.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00300800.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00300863.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00300908.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00301912.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00301915.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00301918.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00301932.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00301973.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00302053.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00302137.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00302265.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00302269.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00302300.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00302347.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00302380.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00302391.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00302393.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00302737.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00302829.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00302831.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00302872.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00302919.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00302925.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00302956.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00303007.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00303059.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00303079.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00303201.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00303208.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00303264.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00303266.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00303291.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00303304.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00303338.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00303421.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00303499.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00303560.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00303638.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00303832.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00303846.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00303892.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00303943.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00304006.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00304008.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00304128.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00304178.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00304198.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00304211.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00304243.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00304308.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00304337.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00304409.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00304431.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00304494.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00305380.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00305543.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00305571.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00305618.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00305671.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00305674.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00305723.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00305727.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00305735.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00305777.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00305811.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00305920.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00306269.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00306278.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00306310.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00306384.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00306419.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00306442.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00306448.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00306451.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00307126.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00307195.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00307259.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00307306.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00307354.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00307358.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00307394.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00307454.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00307514.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00307539.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00307569.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00307601.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00307619.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00307656.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00307710.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00307716.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00307732.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00307861.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00307935.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00308047.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00308084.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00309375.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00309390.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00309440.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00309516.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00309640.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00309674.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00309759.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00310015.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00310247.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00310249.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00310341.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00310370.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00310405.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00310468.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00310473.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00310634.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00310691.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00310738.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00310809.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00310863.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00310872.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00310969.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00311071.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00311170.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00311244.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00311287.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00311321.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00311365.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00311402.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00311473.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              'data16_13TeV.00311481.physics_Main.deriv.DAOD_{}.r9264_p3083_p3180',
              ]

# Use the period containers which have removed duplicate runs with different tags
DATA17RUN = [ ]

# Individual runs 
TopExamples.grid.Add('Data15Runs_TOPQ1').datasets = [i.format('TOPQ1') for i in DATA15RUN]
TopExamples.grid.Add('Data15Runs_TOPQ2').datasets = [i.format('TOPQ2') for i in DATA15RUN]
TopExamples.grid.Add('Data15Runs_TOPQ3').datasets = [i.format('TOPQ3') for i in DATA15RUN]
TopExamples.grid.Add('Data15Runs_TOPQ4').datasets = [i.format('TOPQ4') for i in DATA15RUN]
TopExamples.grid.Add('Data15Runs_TOPQ5').datasets = [i.format('TOPQ5') for i in DATA15RUN]

TopExamples.grid.Add('Data16Runs_TOPQ1').datasets = [i.format('TOPQ1') for i in DATA16RUN]
TopExamples.grid.Add('Data16Runs_TOPQ2').datasets = [i.format('TOPQ2') for i in DATA16RUN]
TopExamples.grid.Add('Data16Runs_TOPQ3').datasets = [i.format('TOPQ3') for i in DATA16RUN]
TopExamples.grid.Add('Data16Runs_TOPQ4').datasets = [i.format('TOPQ4') for i in DATA16RUN]
TopExamples.grid.Add('Data16Runs_TOPQ5').datasets = [i.format('TOPQ5') for i in DATA16RUN]

TopExamples.grid.Add('Data17Runs_TOPQ1').datasets = [i.format('TOPQ1') for i in DATA17RUN]
TopExamples.grid.Add('Data17Runs_TOPQ2').datasets = [i.format('TOPQ2') for i in DATA17RUN]
TopExamples.grid.Add('Data17Runs_TOPQ3').datasets = [i.format('TOPQ3') for i in DATA17RUN]
TopExamples.grid.Add('Data17Runs_TOPQ4').datasets = [i.format('TOPQ4') for i in DATA17RUN]
TopExamples.grid.Add('Data17Runs_TOPQ5').datasets = [i.format('TOPQ5') for i in DATA17RUN]

# Period containers - preferred choice
TopExamples.grid.Add('Data15_TOPQ1').datasets = [i.format('TOPQ1') for i in DATA15]
TopExamples.grid.Add('Data15_TOPQ2').datasets = [i.format('TOPQ2') for i in DATA15]
TopExamples.grid.Add('Data15_TOPQ3').datasets = [i.format('TOPQ3') for i in DATA15]
TopExamples.grid.Add('Data15_TOPQ4').datasets = [i.format('TOPQ4') for i in DATA15]

TopExamples.grid.Add('Data16_TOPQ1').datasets = [i.format('TOPQ1') for i in DATA16]
TopExamples.grid.Add('Data16_TOPQ2').datasets = [i.format('TOPQ2') for i in DATA16]
TopExamples.grid.Add('Data16_TOPQ3').datasets = [i.format('TOPQ3') for i in DATA16]
TopExamples.grid.Add('Data16_TOPQ4').datasets = [i.format('TOPQ4') for i in DATA16]

TopExamples.grid.Add('Data17_TOPQ1').datasets = [i.format('TOPQ1') for i in DATA17]
TopExamples.grid.Add('Data17_TOPQ2').datasets = [i.format('TOPQ2') for i in DATA17]
TopExamples.grid.Add('Data17_TOPQ3').datasets = [i.format('TOPQ3') for i in DATA17]
TopExamples.grid.Add('Data17_TOPQ4').datasets = [i.format('TOPQ4') for i in DATA17]
