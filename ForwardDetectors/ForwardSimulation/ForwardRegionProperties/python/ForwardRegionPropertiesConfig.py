# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

from AthenaCommon import CfgMgr
#from AthenaCommon.SystemOfUnits import *

def getForwardRegionProperties(name="ForwardRegionProperties", **kwargs):
    # Settings of optics to be used
    import os
    twiss_path = os.getenv('TwissFilesPATH')
    twiss_beam1 = None
    twiss_beam2 = None
    twiss_momentum = -1.
    from G4AtlasApps.SimFlags import simFlags
    if simFlags.TwissFileBeam1.statusOn:
        if os.access(simFlags.TwissFileBeam1(),os.R_OK): twiss_beam1 = simFlags.TwissFileBeam1()
        elif os.access(twiss_path+simFlags.TwissFileBeam1(),os.R_OK): twiss_beam1 = twiss_path+simFlags.TwissFileBeam1()
    if simFlags.TwissFileBeam2.statusOn:
        if os.access(simFlags.TwissFileBeam2(),os.R_OK): twiss_beam2 = simFlags.TwissFileBeam2()
        elif os.access(twiss_path+simFlags.TwissFileBeam2(),os.R_OK): twiss_beam2 = twiss_path+simFlags.TwissFileBeam2()
    if twiss_beam1 is None or twiss_beam2 is None:
        twiss_energy = '7.0TeV'
        if simFlags.TwissEnergy.statusOn:
            twiss_energy = twiss_energy = '%1.1fTeV'%(simFlags.TwissEnergy()*0.000001)
        else:
            from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
            if athenaCommonFlags.PoolEvgenInput.statusOn and len(athenaCommonFlags.PoolEvgenInput.get_Value())>0:
                inputfile = athenaCommonFlags.PoolEvgenInput.get_Value()[0]
                import PyUtils.AthFile as af
                try:
                    f = af.fopen(inputfile)
                    if 'beam_energy' in f.infos and 'N/A' not in f.infos['beam_energy']:
                        if type(f.infos['beam_energy']) is list and float(f.infos['beam_energy'][0])>100.:
                            twiss_energy = '%1.1fTeV'%(float(f.infos['beam_energy'][0])*0.000001)
                        elif float(f.infos['beam_energy'])>100.:
                            twiss_energy = '%1.1fTeV'%(float(f.infos['beam_energy'][0])*0.000001)
                except AssertionError:
                    print ("ForwardRegionPropertiesConfig.py ERROR Failed to open input file: %s", inputfile)
                    pass
        twiss_beta = '%07.2fm'%(0.001*simFlags.TwissFileBeta())
        if not (simFlags.TwissFileNomReal.statusOn and simFlags.TwissFileVersion.statusOn):
            print ("ForwardRegionPropertiesConfig.py ERROR Need to either provide file names or set file name and file version flags.")
            raise Exception('Not enough information to locate Twiss files. Need to either provide file names or set file name and file version flags.')
        twiss_nomreal = simFlags.TwissFileNomReal()
        twiss_version = simFlags.TwissFileVersion()

        # Getting paths to the twiss files, momentum calculation; you can switch to local files
        import re,math
        twiss_beam1 = os.path.join(twiss_path, twiss_energy, twiss_beta, twiss_nomreal, twiss_version, 'beam1.tfs')
        twiss_beam2 = os.path.join(twiss_path, twiss_energy, twiss_beta, twiss_nomreal, twiss_version, 'beam2.tfs')
        twiss_momentum =  math.sqrt(float(re.findall("\\d+.\\d+", twiss_energy)[0])**2 - (0.938e-3)**2)*1e3
    else:
        # Have to sort out twiss momentum based on file name
        tmp = twiss_beam1.split('TeV')[0]
        tmp_spot = len(tmp)
        if simFlags.TwissEnergy.statusOn:
            twiss_energy = '%1.1fTeV'%(simFlags.TwissEnergy()*0.000001)
        else:
            while True:
                try:
                    tmp_energy = float( tmp[tmp_spot:] ) # noqa: F841
                    tmp_spot -= 1
                except ValueError:
                    twiss_energy = float( tmp[tmp_spot+1:] )
                    break
                pass
        import re,math
        twiss_momentum =  math.sqrt(float(re.findall("\\d+.\\d+", twiss_energy)[0])**2 - (0.938e-3)**2)*1e3

    # properties of the field set according to the optics settings above
    kwargs.setdefault("twissFileB1", twiss_beam1)
    kwargs.setdefault("twissFileB2", twiss_beam2)
    kwargs.setdefault("momentum", twiss_momentum)
    return CfgMgr.ForwardRegionProperties(name, **kwargs)
