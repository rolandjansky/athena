# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function


##=============================================================================
## Name:        AutoConfiguration.py
## Author:      David Cote (DESY/CERN), Michael Boehler (DESY)
## Created:     July 2008
## Description: This is a collection of so-called "auto-configuration" functions for RecExCommon
##=============================================================================
from AthenaCommon.GlobalFlags import globalflags
from RecExConfig.RecFlags import rec
from RecExConfig.RecoFunctions import ItemInListStartsWith

from AthenaCommon.Logging import logging

logAutoConfiguration = logging.getLogger( 'AutoConfiguration' )

KnownCosmicsProjects=frozenset(["data08_calocomm","data08_muoncomm","data08_cos","data08_cosmag","data08_idcomm",
                                "data09_cos","data09_cosmag","data09_idcomm","EO_NOISE","data09_calocomm","data09_muoncomm","data09_calophys",
                                "data10_muoncomm","data10_idcomm","data10_larcomm","data10_tilecomm","data10_calocomm","data10_calib","data10_cos",
                                "data11_calib","data11_calocomm","data11_cos","data11_idcomm","data11_larcomm","data11_muoncomm","data11_tilecomm",
                                "data12_calib","data12_calocomm","data12_cos","data12_idcomm","data12_larcomm","data12_muoncomm","data12_tilecomm",
                                "data13_calib","data13_calocomm","data13_cos","data13_idcomm","data13_larcomm","data13_muoncomm","data13_tilecomm",
                                "data14_calib","data14_calocomm","data14_cos","data14_idcomm","data14_larcomm","data14_muoncomm","data14_tilecomm",
                                "data15_calib","data15_calocomm","data15_cos","data15_idcomm","data15_larcomm","data15_muoncomm","data15_tilecomm",
                                "data16_calib","data16_calocomm","data16_cos","data16_idcomm","data16_larcomm","data16_muoncomm","data16_tilecomm",
                                "data17_calib","data17_calocomm","data17_cos","data17_idcomm","data17_larcomm","data17_muoncomm","data17_tilecomm",
                                "data18_calib","data18_calocomm","data18_cos","data18_idcomm","data18_larcomm","data18_muoncomm","data18_tilecomm",
                                "data19_calib","data19_calocomm","data19_cos","data19_idcomm","data19_larcomm","data19_muoncomm","data19_tilecomm",
                                "data20_calib","data20_calocomm","data20_cos","data20_idcomm","data20_larcomm","data20_muoncomm","data20_tilecomm",
                                ])

# Abandon the single-beam reconstruction all together, 26 February 2011 
Known1BeamProjects=frozenset(["data08_1beam","data08_1beammag",
                              "data09_1beam","data09_1beammag",
                              "data10_1beam","data10_1beammag",
                              ])

KnownCollisionsProjects=frozenset(["data08","data08_coll900","data09","data09_coll900","data09_900GeV","data09_2TeV",
                                   "data10_900GeV","data10_2TeV","data10_7TeV","data10_10TeV","data10_comm",
                                   "data11_7TeV","data11_8TeV","data11_2p76TeV","data11_comm","data11_900GeV","data11_1beam","data11_hip", 
                                   "data12_8TeV","data12_comm","mc12_8TeV","IS_SIMULATION","data12_1beam","data12_900GeV",
                                   "data13_8TeV","data13_comm","data13_2p76TeV","data13_1beam",
                                   "data14_comm","data15_comm","data15_900GeV","data15_1beam","data15_13TeV","data15_5TeV",
                                   "data16_comm","data16_13TeV","data16_900GeV","data16_1beam",
                                   "data17_comm","data17_13TeV","data17_900GeV","data17_1beam","data17_5TeV",
                                   "data18_comm","data18_13TeV","data18_900GeV","data18_1beam","data18_1p8TeV"
                                   ])

KnownHeavyIonProjects=frozenset(["data10_hi","data11_hi","data15_hi","data16_hi","data17_hi","data18_hi"])

KnownHeavyIonProtonProjects=frozenset(["data12_hip","data13_hip","data16_hip","data16_hip5TeV","data16_hip8TeV"])

KnownTestProjects=frozenset(["data_test"])

KnownProjects=KnownCosmicsProjects | Known1BeamProjects | KnownCollisionsProjects | KnownHeavyIonProjects | KnownHeavyIonProtonProjects | KnownTestProjects

fullSolenoidCurrent=7730.0
fullToroidCurrent=20500.0

def GetRunNumber():
    runNb=None
    from RecExConfig.RecFlags import rec
    if rec.RunNumber.isDefault():
        from PyUtils.MetaReaderPeeker import metadata
        try:
            runNb = metadata['runNumbers'][0]
        except Exception:
            from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
            if not athenaCommonFlags.isOnline(): logAutoConfiguration.error("No RunNumber stored in InputFile!")
    else:
        runNb=rec.RunNumber()
    logAutoConfiguration.debug("RunNumber is: %s",runNb)
    return runNb


def GetLBNumber():
    # empty files do not have any lumi blocks!!
    from PyUtils.MetaReaderPeeker import metadata
    if metadata['nentries'] == 0:
        return None

    lbs=[0,]
    try:
        lbs = metadata['lumiBlockNumbers']

    except Exception:
        logAutoConfiguration.error("No LumiBlock number stored in InputFile! Use 0")
        
    if len(lbs)>1:
        logAutoConfiguration.warning("Data from more than one lumi-block in the same file. Use first lumi-block number.")
    lb=lbs[0]
    logAutoConfiguration.debug("LumiBlock Number is: %i",lb)
    return lb

def GetFieldFromCool():
    logAutoConfiguration.info("Reading magnetic field status from COOL database.")

    try:
        from RecExConfig.GetCool import cool
        return cool.solenoidCurrent(),cool.toroidCurrent()
    except:
        return None,None
    


def GetFieldFromInputFile():
    logAutoConfiguration.info("Reading magnetic field status from input file metadata.")
    from PyUtils.MetaReaderPeeker import metadata

    solenoidCurrent=None
    toroidCurrent=None

    if '/EXT/DCS/MAGNETS/SENSORDATA' in metadata:
        # where index in ['/EXT/DCS/MAGNETS/SENSORDATA']['value'][index] or
        #                ['/EXT/DCS/MAGNETS/SENSORDATA']['quality_invalid'][index] is:
        # 0 = CentralSol_Current
        # 1 = CentralSol_SCurrent
        # 2 = Toroids_Current
        # 3 = Toroids_SCurrent
        try:
            solenoidCurrent = metadata['/EXT/DCS/MAGNETS/SENSORDATA']['value'][0]  # CentralSol_Current
            toroidCurrent = metadata['/EXT/DCS/MAGNETS/SENSORDATA']['value'][2]  # Toroids_Current
        except:
            logAutoConfiguration.warning("Unable to find solenoid and toroid currents in /EXT/DCS/MAGNETS/SENSORDATA")


        # consistency check of GEO and b field values in case of simulated data
        if metadata['eventTypes'][0] == 'IS_SIMULATION':
            from AthenaCommon.GlobalFlags import globalflags
            if globalflags.DetDescrVersion().startswith('ATLAS-') and (solenoidCurrent,toroidCurrent)!= (GetApproximateFieldFromGeo()):
                # if the values differ, the Field is configured due to GEO
                tempSol,tempTor = GetApproximateFieldFromGeo()
                logAutoConfiguration.warning("BField current in input file (%s,%s) differs from GEO version (%s,%s)!! Field is configured due to GEO version", solenoidCurrent,toroidCurrent,tempSol,tempTor)
                logAutoConfiguration.warning("Field is configured due to GEO version")
                solenoidCurrent,toroidCurrent= GetApproximateFieldFromGeo()
    else:
        logAutoConfiguration.warning("Input file metadata don't have necessary info to get field conditions. It's probably too old.")
    return solenoidCurrent,toroidCurrent

def GetApproximateFieldFromGeo():
    #Note that the values returned are not exact. They should be interpreted only as Field ON/OFF.
    #The values are set in line 26 at the beginning of this script
    #fullSolenoidCurrent is set to 7700.0
    #fullToroidCurrent is set to 20500.0 
    logAutoConfiguration.info("Trying to guess magnetic field status from geometryVersion.")
    solenoidCurrent=None
    toroidCurrent=None
    from AthenaCommon.GlobalFlags import globalflags
    geo=globalflags.DetDescrVersion()
    if geo.startswith('ATLAS-GEONSF-'):
        solenoidCurrent=0.0
        toroidCurrent=fullToroidCurrent
    elif geo.startswith('ATLAS-GEONTF-'):
        solenoidCurrent=fullSolenoidCurrent
        toroidCurrent=0.0
    elif geo.startswith('ATLAS-GEONF-'):
        solenoidCurrent=0.0
        toroidCurrent=0.0
    elif geo.startswith('ATLAS-'):
        solenoidCurrent=fullSolenoidCurrent
        toroidCurrent=fullToroidCurrent
    else:
        logAutoConfiguration.warning("Don't know how to interpret magnetic field status from geometry '%s'."%geo)
    return solenoidCurrent,toroidCurrent

def GetApproximateFieldFromConditions():
    #Note that the values returned are not exact. They should be interpreted only as Field ON/OFF.
    #The values are set in line 26 at the beginning of this script
    #fullSolenoidCurrent is set to 7700.0
    #fullToroidCurrent is set to 20500.0 
    logAutoConfiguration.info("Trying to guess magnetic field status from conditionsTag")
    solenoidCurrent=None
    toroidCurrent=None
    from AthenaCommon.GlobalFlags import globalflags
    cond=globalflags.ConditionsTag()
    if cond.startswith('COMCOND-ES1CST-') or cond.startswith('COMCOND-ES1SST-') or cond.startswith('COMCOND-ES1PST-'): 
        solenoidCurrent=fullSolenoidCurrent
        toroidCurrent=fullToroidCurrent
    elif cond.startswith('COMCOND-ES1CS-')or cond.startswith('COMCOND-ES1SS-')or cond.startswith('COMCOND-ES1PS-'): 
        solenoidCurrent=fullSolenoidCurrent
        toroidCurrent=0.0
    elif cond.startswith('COMCOND-ES1CT-') or cond.startswith('COMCOND-ES1ST-') or cond.startswith('COMCOND-ES1PT-'): 
        solenoidCurrent=0.0
        toroidCurrent=fullToroidCurrent
    elif cond.startswith('COMCOND-ES1C-') or cond.startswith('COMCOND-ES1S-') or cond.startswith('COMCOND-ES1P-'): 
        solenoidCurrent=0.0
        toroidCurrent=0.0
    else:
        logAutoConfiguration.warning("Don't know how to interpret magnetic field status from conditionsTag '%s'."%cond)
    return solenoidCurrent,toroidCurrent


def ConfigureField():
    from AthenaCommon.BFieldFlags import jobproperties
    if jobproperties.BField.barrelToroidOn.is_locked() and jobproperties.BField.endcapToroidOn.is_locked() and jobproperties.BField.solenoidOn.is_locked():
        logAutoConfiguration.info("Field status is locked. Don't attempt to re-configure it.")
        return

    solenoidCurrent=None
    toroidCurrent=None

    from PyUtils.MetaReaderPeeker import metadata

    if metadata['file_type'] == 'BS' and metadata['eventTypes'][0] != 'IS_SIMULATION':
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        # online running -> getting field from IS
        if athenaCommonFlags.isOnline():
            from RecExOnline.OnlineISConfiguration import GetBFields
            Fields=GetBFields()
            solenoidCurrent=Fields[0]
            toroidCurrent=Fields[1]
        # offline running -> getting field from COOL    
        else:
            solenoidCurrent,toroidCurrent=GetFieldFromCool()

    elif metadata['file_type'] == 'BS' and metadata['eventTypes'][0] == 'IS_SIMULATION':
        logAutoConfiguration.info("Field info is not stored in MC BS values are set via conditions tag:")
        if solenoidCurrent==None or toroidCurrent==None:
            solenoidCurrent,toroidCurrent=GetApproximateFieldFromConditions()
        if solenoidCurrent==None or toroidCurrent==None:
            logAutoConfiguration.warning("BField of MC BS cannot be autoconfigured!! BField is turned ON")
            solenoidCurrent=fullSolenoidCurrent
            toroidCurrent=fullToroidCurrent

    elif metadata['file_type'] == 'POOL':
        solenoidCurrent,toroidCurrent=GetFieldFromInputFile()
        if solenoidCurrent==None or toroidCurrent==None:
            solenoidCurrent,toroidCurrent=GetApproximateFieldFromConditions()             
        if solenoidCurrent==None or toroidCurrent==None:
            solenoidCurrent,toroidCurrent=GetApproximateFieldFromGeo() 
    else:
        raise RuntimeError("Don't know how to interpret file_type '%s'"%metadata['file_type'])

    if solenoidCurrent==None or toroidCurrent==None:
        raise RuntimeError("Unable to determine field status for this file.")

    if toroidCurrent>1.:
        jobproperties.BField.barrelToroidOn.set_Value_and_Lock(True)
        jobproperties.BField.endcapToroidOn.set_Value_and_Lock(True)
    else:
        jobproperties.BField.barrelToroidOn.set_Value_and_Lock(False)
        jobproperties.BField.endcapToroidOn.set_Value_and_Lock(False)
        
    if solenoidCurrent>1.:
        jobproperties.BField.solenoidOn.set_Value_and_Lock(True)
    else:
        jobproperties.BField.solenoidOn.set_Value_and_Lock(False)        

    logAutoConfiguration.info("Field status sucessfully auto-configured to values:")
    logAutoConfiguration.info("BField: barrelToroidOn = %s ",jobproperties.BField.barrelToroidOn() )
    logAutoConfiguration.info("BField: BField.endcapToroidOn = %s ",jobproperties.BField.endcapToroidOn() )
    logAutoConfiguration.info("BField: BField.solenoidOn = %s ",jobproperties.BField.solenoidOn() )
    return


def ConfigureGeo():
    from AthenaCommon.GlobalFlags import globalflags
    if globalflags.DetDescrVersion.is_locked():
        logAutoConfiguration.info("Geometry was locked to: %s ",globalflags.DetDescrVersion() )
        return

    from PyUtils.MetaReaderPeeker import metadata
    if metadata['file_type'] == 'BS':
        geo="ATLAS-R2-2015-03-01-00" #geo='ATLAS-GEO-20-00-01'
        project=GetProjectName()
        if "data12" in project:
            geo="ATLAS-R1-2012-03-00-00"
        if "data11" in project:
            geo="ATLAS-R1-2011-02-00-00"  #'ATLAS-GEO-18-01-01'
        if "data10" in project or "data09" in project or "data08" in project:
            geo="ATLAS-R1-2010-02-00-00" #geo='ATLAS-GEO-16-00-01'
        if metadata['eventTypes'][0] == 'IS_SIMULATION':
            try: geo = metadata['GeoAtlas']
            except: logAutoConfiguration.warning("Input simulated bs file does not contain bs_metadata with geometry. Probably an old file.")
            pass
        globalflags.DetDescrVersion.set_Value_and_Lock(geo)
        logAutoConfiguration.info("Set GeometryVersion to '%s'"%geo)

    elif metadata['file_type'] == 'POOL':
        # configure Geometry from input file
        globalflags.DetDescrVersion.set_Value_and_Lock(metadata['GeoAtlas'])
        logAutoConfiguration.info("Configured geometry from input file: %s ",globalflags.DetDescrVersion() )
    else:
        raise RuntimeError("Don't know how to interpret file_type {}".format(metadata['file_type']))
    return
   

def ConfigureFieldAndGeo():
    from PyUtils.MetaReaderPeeker import metadata

    if metadata['file_type'] == 'BS':
        ConfigureField()
        ConfigureGeo()
    elif metadata['file_type'] == 'POOL':
        ConfigureGeo()
        ConfigureField()
    else:
        raise RuntimeError("Don't know how to interpret file_type '%s'"%metadata['file_type'])
    return

def GetProjectName():
    # special option for online running
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    if athenaCommonFlags.isOnline():
        from RecExOnline.OnlineISConfiguration import GetRunType
        rec.projectName=GetRunType()[2]
    # continue with options for offline running
    if rec.projectName()=="":
        from PyUtils.MetaReaderPeeker import metadata
        whatIsIt=None
        try:
            whatIsIt = metadata['eventTypes'][0]
        except:
            pass
        if whatIsIt=='IS_SIMULATION':
            project='IS_SIMULATION'
        else:
            try:
                project = metadata['project_name']
            except:
                from RecExConfig.GetCool import cool
                project = cool.fileNameTag()
            pass
        #rec.projectName.set_Value_and_Lock(project)
        logAutoConfiguration.info("Success! GetProjectName() found a project named %s"%project)
    else:
        project=rec.projectName()
    if not project in KnownProjects:
        logAutoConfiguration.warning("Project '%s' is not part of the KnownProjects list."%project)
        #print(KnownProjects)

    return project

def ConfigureBeamType():
    logAutoConfiguration.debug("Configuring beamType...")
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.beamType.is_locked():      
        logAutoConfiguration.info("beamType is locked to '%s'. We won't change change it."%jobproperties.Beam.beamType())
        return

    BeamType=None
    # special option for online running
    # some code removed here due to (o.b.o. A.Vogel)
    # Run Control sets a beam type, but that is not suitable here - better use the project name instead

    from PyUtils.MetaReaderPeeker import metadata
    try:
        listOfKnownBeamTypes=['cosmics' ,'singlebeam','collisions']
        if metadata['beam_type'] in listOfKnownBeamTypes:
            BeamType = metadata['beam_type']
    except:
        logAutoConfiguration.info("beam type not stored in input file, set beam type according to project name...")
        
    if BeamType==None:
        project=GetProjectName()
        if project in KnownCosmicsProjects: BeamType='cosmics'
        elif project in KnownTestProjects: BeamType='cosmics'
        elif project in Known1BeamProjects: BeamType='singlebeam'
        elif project in KnownCollisionsProjects: BeamType='collisions'
        elif project in KnownHeavyIonProjects: BeamType='collisions'
        elif project in KnownHeavyIonProtonProjects: BeamType='collisions'
    # special treatment for online, set to collisions if undefined
    if BeamType==None:
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        if athenaCommonFlags.isOnline():
            BeamType='collisions' # best guess if we have encountered an unknown project name
            logAutoConfiguration.warning("Unable to determine BeamType from project name, guessing: %s" % BeamType)
            
    if BeamType==None:
        raise RuntimeError("Unable to determine beamType from projectName '%s'"%project)
    else:
        from AthenaCommon.BeamFlags import jobproperties
        jobproperties.Beam.beamType.set_Value_and_Lock( BeamType )
        logAutoConfiguration.info("Auto configured BeamType: %s ", BeamType)


    #AutoConfigure HI reco if needed:
    #Note: HeavyIon and HIP (HeavyIon - proton) are mutually exclusive
    if not rec.doHeavyIon.is_locked() and not rec.doHIP.is_locked():
        project=GetProjectName()
        if project in KnownHeavyIonProjects:
            rec.doHeavyIon.set_Value_and_Lock(True)
            rec.doHIP.set_Value_and_Lock(False)
            logAutoConfiguration.info("Set rec.doHeavyIon=True based on project tag '%s'" % project)
        elif project in KnownHeavyIonProtonProjects:
            rec.doHeavyIon.set_Value_and_Lock(False)
            rec.doHIP.set_Value_and_Lock(True)
            logAutoConfiguration.info("Set rec.doHIP=True based on project tag '%s'" % project)
        else:
            logAutoConfiguration.info("Not a heavy ion project")
            pass
    else:
        logAutoConfiguration.info("rec.doHeavyIon or rec.doHIP already locked")

    return


def ConfigureNumberOfCollisions():
    from PyUtils.MetaReaderPeeker import metadata

    from AthenaCommon.BeamFlags import jobproperties
    if metadata['eventTypes'][0] == 'IS_DATA':
        # Make sure that beamType is set:
        if not jobproperties.Beam.beamType.is_locked():
            ConfigureBeamType()
        # configure number of collision setup for collisions (these values have to be updated for the given beam energies)
        if jobproperties.Beam.beamType() == 'collisions' :
            jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(2.)
        elif jobproperties.Beam.beamType() == 'cosmics' or  jobproperties.Beam.beamType() == 'singlebeam':
            jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(0.)
    else:
        # auto configure number of collissions
        try:
            jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(metadata['numberOfCollisions'])
        except:
            logAutoConfiguration.warning("numberOfCollisions could not be auto configured, no info available using default value: %s "
                                         ,jobproperties.Beam.numberOfCollisions() )
            return
                
    logAutoConfiguration.info("Auto configured numberOfCollisions: %s ", jobproperties.Beam.numberOfCollisions())
    return

def ConfigureBeamEnergy():
    from AthenaCommon.BeamFlags import jobproperties

    from PyUtils.MetaReaderPeeker import metadata

    # pool files
    if metadata['file_type'] == 'POOL':
        if 'beam_energy' not in metadata:
            logAutoConfiguration.warning("beam energy is not storted in input file, setting default DATA options!!")
        else:
            jobproperties.Beam.energy.set_Value_and_Lock( float(metadata['beam_energy']))
            logAutoConfiguration.info("Auto-configured beam energy : %s MeV" , jobproperties.Beam.energy())
            return
    # BS files    
    else:
        if metadata['eventTypes'][0] == 'IS_DATA':

            # Make sure that beamType is set:
            if not jobproperties.Beam.beamType.is_locked():
                ConfigureBeamType()
      
            # special option for online running
            from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
            if athenaCommonFlags.isOnline():
                from RecExOnline.OnlineISConfiguration import GetRunType
                jobproperties.Beam.energy.set_Value_and_Lock(float(GetRunType()[1]))
                logAutoConfiguration.info("Auto-configured beam energy : %s MeV" , jobproperties.Beam.energy())
                return

            # configure Beam energy depending on beam type:
            if jobproperties.Beam.beamType() == 'cosmics':
                jobproperties.Beam.energy.set_Value_and_Lock(0.)
            elif jobproperties.Beam.beamType() == 'singlebeam':
                jobproperties.Beam.energy.set_Value_and_Lock(450000.)
            elif jobproperties.Beam.beamType() == 'collisions':
                projectName=str(GetProjectName())
                beamEnergy=None          

                if 'GeV' in projectName:
                    beamEnergy = float( (str(projectName).split('_')[1]).replace('GeV','',1))/2 * 1000.
                elif 'TeV' in projectName:
                    if 'hip5TeV' in projectName:
                        # Approximate 'beam energy' here as sqrt(sNN)/2.
                        beamEnergy = 1577000.
                    elif 'hip8TeV' in projectName:
                        # Approximate 'beam energy' here as sqrt(sNN)/2.
                        beamEnergy = 2510000.
                    else:
                        beamEnergy = float( (str(projectName).split('_')[1]).replace('TeV','',1).replace('p','.'))/2 * 1000000.
                        if '5TeV' in projectName:
                            # these are actually sqrt(s) = 5.02 TeV
                            beamEnergy=2510000.
                elif projectName.endswith("_hi") or projectName.endswith("_hip"):
                    if projectName in ('data10_hi', 'data11_hi'):
                        beamEnergy=1380000. # 1.38 TeV (=3.5 TeV * (Z=82/A=208))
                    elif projectName == 'data12_hi':
                        beamEnergy=1577000. # 1.577 TeV (=4 TeV * (Z=82/A=208))
                    elif projectName in ('data12_hip', 'data13_hip'):
                        # Pb (p) Beam energy in p-Pb collisions in 2012/3 was 1.577 (4) TeV.
                        # Approximate 'beam energy' here as sqrt(sNN)/2.
                        beamEnergy=2510000.
                    elif projectName in ('data15_hi', 'data18_hi'):
                        beamEnergy=2510000. # 2.51 TeV (=6.37 TeV * (Z=82/A=208)) - lowered to 6.37 to match s_NN = 5.02 in Pb-p runs.
                    elif projectName == 'data17_hi':
                        beamEnergy=2721000. # 2.72 TeV for Xe-Xe (=6.5 TeV * (Z=54/A=129))
                else:
                    logAutoConfiguration.warning("Could not auto-configure beam energy based on project name: %s" , projectName)
                    return
                if beamEnergy!=None:
                    jobproperties.Beam.energy.set_Value_and_Lock(beamEnergy)
        elif metadata['eventTypes'][0] == 'IS_SIMULATION':
            if metadata['beam_energy'] != 'N/A' and metadata['beam_energy'] != '':
                jobproperties.Beam.energy.set_Value_and_Lock(float(metadata['beam_energy']))
            else:
                logAutoConfiguration.warning("beam energy is not storted in input file!!") 
                return

    logAutoConfiguration.info("Auto-configured beam energy : %s MeV" , jobproperties.Beam.energy()) 
    return


def ConfigureBeamBunchSpacing():
    from AthenaCommon.BeamFlags import jobproperties
    from PyUtils.MetaReaderPeeker import metadata

    # auto configure bunch spacing not yet possible for DATA 
    # ---> this is still missing

    if metadata['eventTypes'][0] == 'IS_SIMULATION':
        try:
            jobproperties.Beam.bunchSpacing.set_Value_and_Lock(int(metadata['intraTrainBunchSpacing']))
            logAutoConfiguration.info("Auto configured bunchSpacing: %s ", jobproperties.Beam.bunchSpacing())
        except:
            logAutoConfiguration.warning("bunchSpacing could not be auto configured, no info available using default value: %s ",jobproperties.Beam.bunchSpacing() )

    else:
        #real data
        if not jobproperties.Beam.beamType.is_locked():
            ConfigureBeamType()
        if jobproperties.Beam.beamType() == 'collisions':
            runnumber = GetRunNumber()
            if runnumber is None:
                  logAutoConfiguration.info("Couldn't get run number. Aborting.")
                  return
            if (runnumber<236107): # run-1
                  logAutoConfiguration.info("Run-1 data, set to 50ns.")
                  jobproperties.Beam.bunchSpacing.set_Value_and_Lock(50)
                  # could have just left default, but at least this is explicit.
                  return
            #Real data collision run, try to get bunch spacing from database
            from CoolConvUtilities.BunchSpacingUtils import bunchSpacingOfRun
            
            lb=GetLBNumber()
            if lb is None:
                logAutoConfiguration.info("No LB nubmer, autoconfigure to 25 ns")
                jobproperties.Beam.bunchSpacing.set_Value_and_Lock(25)
                return

            bc=bunchSpacingOfRun(runnumber,lb)
            if bc is None:
                logAutoConfiguration.info("No data, autoconfigure to 25 ns")
                jobproperties.Beam.bunchSpacing.set_Value_and_Lock(25)
                return

            if bc==1: 
                jobproperties.Beam.bunchSpacing.set_Value_and_Lock(25)
                logAutoConfiguration.info("Autoconfigure bunch-spacing to 25 ns")
            else:
                logAutoConfiguration.info("larger bunch spacing, leave default value of %i ns" % jobproperties.Beam.bunchSpacing())
                pass
            pass
        else:
            logAutoConfiguration.info("Not a collisions run, bunch spacing not autoconfigured")
            pass
        pass
    return
 


def ConfigureLumiFlags():
    from AthenaCommon.BeamFlags import jobproperties

    if jobproperties.Beam.numberOfCollisions.is_locked():
        logAutoConfiguration.warning("numberOfCollisions was set and locked (no autoconfiguration): %s ", jobproperties.Beam.numberOfCollisions()  )
    else:
        ConfigureNumberOfCollisions()
        
    if jobproperties.Beam.bunchSpacing.is_locked():
        logAutoConfiguration.info("bunchSpacing was set and locked (no autoconfiguration): %s ",jobproperties.Beam.bunchSpacing() )
    else:
        ConfigureBeamBunchSpacing()

    return


def GetDefaultTagRefStream(streams):
    # spot the most downstream target : this will be the default
    # except if one readXYZ is locked    
    from RecExConfig.RecFlags import rec

    if rec.readRDO.is_locked() and rec.readRDO():
        if 'Stream1_ref' in streams:
            return  'Stream1_ref'
        if 'StreamRDO_ref' in streams:
            return  'StreamRDO_ref'
        elif 'StreamRAW_ref' in streams:
            return  'StreamRAW_ref'
        else:
            raise RuntimeError(" readRDO locked True and no Stream1 nor StreamRDO nor StreamRAW !")
    if rec.readESD.is_locked() and rec.readESD():
        if 'StreamESD_ref' in streams:
            return  'StreamESD_ref'
        else:
            raise RuntimeError(" readESD locked True and no StreamESD !")

    if rec.readAOD.is_locked() and rec.readAOD():
        if 'StreamAOD_ref' in streams:
            return  'StreamAOD_ref'
        else:
            raise RuntimeError(" readAOD locked True and no StreamAOD !")


    # now deal with default case    
    if 'StreamAOD_ref' in streams:
        return 'StreamAOD_ref'
    elif 'StreamESD_ref' in streams:
        return 'StreamESD_ref'
    elif 'Stream1_ref' in streams:
        return 'Stream1_ref'
    elif 'StreamRDO_ref' in streams:
        return 'StreamRDO_ref'
    elif 'StreamRAW_ref' in streams:
        return 'StreamRAW_ref'
    else:
        raise RuntimeError(" no known streams !")

    return None



def ConfigureInputType():
    from PyUtils.MetaReaderPeeker import metadata

    from AthenaCommon.GlobalFlags  import globalflags
    streamsName=[]

    if metadata['file_type'] == 'BS':
        #byte stream
        logAutoConfiguration.info("Input BS detected")   
        globalflags.InputFormat='bytestream'
        rec.readRDO=True
        rec.readESD=False
        rec.readAOD=False
        rec.doAOD=False
        rec.doESD=True
        rec.doWriteAOD=False
        logAutoConfiguration.info ("BS format: setting globalflags.InputFormat=%s "%globalflags.InputFormat() )
        logAutoConfiguration.info ("BS format: setting rec.readRDO=%s "%rec.readRDO() )
        logAutoConfiguration.info ("BS format: setting rec.doAOD=%s "%rec.doAOD() )
        logAutoConfiguration.info ("BS format: setting rec.doESD=%s "%rec.doESD() )
        logAutoConfiguration.info ("BS format: setting rec.doWriteAOD=%s "%rec.doWriteAOD() )
    else:        
        globalflags.InputFormat='pool'
        #Get streamsName
        streamsName=[]
        if 'processingTags' in metadata:
            streamsName = metadata['processingTags']
        if streamsName==None:
            streamsName=[]
        logAutoConfiguration.info("Extracted streams %s from input file " % streamsName )   

        if len(streamsName)==0:
           logAutoConfiguration.warning("ConfigureInputType: no stream in input file.")   
        elif len(streamsName)>1:
            logAutoConfiguration.warning("ConfigureInputType: more than one stream: %s in input file. Default action. Consider the highest one.")
        else:
            rec.mergingStreamName=streamsName[0]

    from RecExConfig.RecoFunctions import OverlapLists
    try:
        from PrimaryDPDMaker.PrimaryDPDFlags import listRAWtoDPD,listESDtoDPD,listAODtoDPD
    except:
        logAutoConfiguration.warning("Unable to import PrimaryDPDFlags. OK for ATN tests below AtlasAnalysis, otherwise suspicious.")
        listRAWtoDPD=[]
        listESDtoDPD=[]
        listAODtoDPD=[]

    ## these lines have been added to guarantee the backward compatibility for the old DPD naming convention cosmics
    try:
        from PrimaryDPDMaker.PrimaryDPDFlags import listBackwardCompatibleAODtoDPD, listBackwardCompatibleESDtoDPD
        listESDtoDPD.extend(listBackwardCompatibleESDtoDPD)
        listAODtoDPD.extend(listBackwardCompatibleAODtoDPD)
    except:
        logAutoConfiguration.warning("Primary DPDMake does not support the old naming convention!!")   

    if 'TagStreamsRef' in metadata and metadata['TagStreamsRef']!=None:
        logAutoConfiguration.info("Input TAG detected")
        rec.readTAG=True
        logAutoConfiguration.info ("Auto configured rec.readTAG=%s "%rec.readTAG() )

    from RecExConfig.RecoFunctions import ItemInListStartsWith    
    if ItemInListStartsWith ("StreamAOD", streamsName) or ItemInListStartsWith('StreamDAOD',streamsName) or ItemInListStartsWith('StreamD2AOD',streamsName) or OverlapLists(streamsName,listAODtoDPD) or ItemInListStartsWith('DAOD',streamsName) or ItemInListStartsWith('D2AOD',streamsName):
        logAutoConfiguration.info("Input AOD detected")   
        rec.readRDO=False
        rec.readESD=False
        rec.readAOD=True
        rec.doAOD=False
        rec.doESD=False
        logAutoConfiguration.info ("setting rec.readAOD=%s "%rec.readAOD() )
    elif ItemInListStartsWith ("StreamESD", streamsName) or ItemInListStartsWith('StreamDESD',streamsName) or ItemInListStartsWith('StreamD2ESD',streamsName) or OverlapLists(streamsName,listESDtoDPD) or ItemInListStartsWith('DESD',streamsName) or ItemInListStartsWith('D2ESD',streamsName):
        logAutoConfiguration.info("Input ESD detected")   
        rec.readRDO=False
        rec.readESD=True
        rec.readAOD=False
        rec.doAOD=True
        rec.doESD=False
        logAutoConfiguration.info ("setting rec.readESD=%s "%rec.readESD() )
        logAutoConfiguration.info ("setting rec.doAOD=%s "%rec.doAOD() )
    elif ItemInListStartsWith ("Stream1", streamsName) or ItemInListStartsWith ("StreamRDO", streamsName) or ItemInListStartsWith ("OutputStreamRDO", streamsName) or OverlapLists(streamsName,listRAWtoDPD):
        logAutoConfiguration.info("Input RDO detected")   
        rec.readRDO=True
        rec.readESD=False
        rec.readAOD=False
        rec.doAOD=False
        rec.doESD=True
        logAutoConfiguration.info ("setting rec.readRDO=%s "%rec.readRDO() )
        logAutoConfiguration.info ("setting rec.doESD=%s "% rec.doESD() )
    elif ItemInListStartsWith ("StreamEVGEN", streamsName):
        logAutoConfiguration.info("Input EVGEN detected")   
        rec.readRDO=False
        rec.readESD=False
        rec.readAOD=False
        rec.doAOD=False
        rec.doESD=False
        logAutoConfiguration.info ("setting all unlocked rec.readXXX and rec.doXXX to False")
    elif ItemInListStartsWith ("StreamHITS", streamsName):
        logAutoConfiguration.info("Input StreamEVGEN detected")

    elif metadata['file_type'] != 'BS':
        raise RuntimeError("ConfigureInputType cannot handle streamsName==%s"%streamsName)
    return

def ConfigureTriggerStream():
    from PyUtils.MetaReaderPeeker import metadata

    streamName=''
    if metadata['file_type'] == 'BS':
        try:
            streamName = metadata['stream'].split('_')[1]
        except:
            logAutoConfiguration.warning("Input file does not contain bs_metadata! Trying to specify otherwise!")
    elif metadata['file_type'] == 'POOL':
        try:
            streamName = metadata['triggerStreamOfFile']
        except:
            logAutoConfiguration.warning("Input file does not contain triggerStreamOfFile! Trying to specify otherwise!")

    if streamName=='':
        try:
            if len(metadata['processingTags'])==1:
                streamName = metadata['processingTags'][0]
        except:
            logAutoConfiguration.warning("No trigger stream found in input file!!! ") 


    if streamName=='':
        logAutoConfiguration.info("Failed to find triggerStream from MetaReaderPeeker. OK for MC but can be problematic for data.")
        logAutoConfiguration.info("Keeping input value untouched: rec.triggerStream='%s'"%rec.triggerStream())
        return
    
    logAutoConfiguration.info("Set rec.triggerStream='%s' from MetaReaderPeeker."%streamName)
    rec.triggerStream=streamName
    return


def ConfigureConditionsTag():
    if globalflags.ConditionsTag.is_locked():
        logAutoConfiguration.info("conditionsTag is locked to value: '%s'."%globalflags.ConditionsTag())
        return

    from PyUtils.MetaReaderPeeker import metadata
    if metadata['file_type'] == 'POOL' or metadata['file_type'] == 'BS' and metadata['eventTypes'][0] == 'IS_SIMULATION':
        try:
            globalflags.ConditionsTag.set_Value_and_Lock(metadata['IOVDbGlobalTag'])
            logAutoConfiguration.info("Auto-configured ConditionsTag '%s' from MetaReaderPeeker ",globalflags.ConditionsTag())
        except:
            logAutoConfiguration.error("ConditionsTag could not be auto-configured no info stored in MetaReaderPeeker!!!")
            #logAutoConfiguration.warning("Input simulated bs file does not contain bs_metadata with conditions_tag !")


            pass
        pass
    else: #Regular data files 
        try:
            year=int(rec.projectName()[4:6])
        except:
            logAutoConfiguration.warning("Failed to extract year from project tag "+ rec.projectName() +". Guessing 2015")
            year=15
        if (year<14): #Run1
            globalflags.ConditionsTag.set_Value_and_Lock("COMCOND-BLKPA-RUN1-09")
            logAutoConfiguration.info("Found run 1 input bytestream file, autoconfigure conditions tag to '%s'"%globalflags.ConditionsTag())
        else:
            globalflags.ConditionsTag.set_Value_and_Lock("CONDBR2-BLKPA-2015-17")
            logAutoConfiguration.info("Found run 2 input bytestream file, autoconfigure conditions tag to '%s'"%globalflags.ConditionsTag())
            pass
        pass
    return


def ConfigureFieldAndGeoESDtoESD():
    from PyUtils.MetaReaderPeeker import metadata
    if metadata['file_type'] == 'POOL':
        logAutoConfiguration.info("InputFile is pool format. Configuring B-Field based on Geometry.")
            
        # configure Geometry for pool input
        if globalflags.DetDescrVersion.is_locked():
            logAutoConfiguration.error("Geometry was locked to '%s'. This is not expected.",globalflags.DetDescrVersion())

        inGeo = metadata['GeoAtlas']
        if inGeo=='ATLAS-GEO-03-00-00': newGeo='ATLAS-GEO-16-00-00'
        elif inGeo=='ATLAS-GEO-04-00-00': newGeo='ATLAS-GEO-16-00-00'
        elif inGeo=='ATLAS-GEONF-04-00-00': newGeo='ATLAS-GEO-16-00-00'
        elif inGeo=='ATLAS-GEONTF-05-00-00': newGeo='ATLAS-GEO-16-00-00'
        else: newGeo='ATLAS-GEO-16-00-00'
        #raise RuntimeError("Don't know how to configure geometry for input geo: '%s'"%inGeo)
        
        globalflags.DetDescrVersion.set_Value_and_Lock(newGeo)

        #configure B-Field
        ConfigureField()
    else:
        raise RuntimeError("file_type is %s but ConfigureFieldAndGeoESDtoESD is only defined for pool inputs"%metadata['file_type'])

    logAutoConfiguration.info("Auto configured Geometry: %s ",globalflags.DetDescrVersion() )
    from AthenaCommon.BFieldFlags import jobproperties
    logAutoConfiguration.info("Auto configured B Field: ");
    logAutoConfiguration.info("BField: barrelToroidOn = %s ",jobproperties.BField.barrelToroidOn() )
    logAutoConfiguration.info("BField: BField.endcapToroidOn = %s ",jobproperties.BField.endcapToroidOn() )
    logAutoConfiguration.info("BField: BField.solenoidOn = %s ",jobproperties.BField.solenoidOn() )
    return

def ConfigureDoTruth():
    if rec.doTruth.is_locked():
        logAutoConfiguration.info ("rec.doTruth=%s is locked. Auto-config will not attempt to change it."%rec.doTruth())
        return

    from PyUtils.MetaReaderPeeker import metadata, convert_itemList
    if metadata['file_type'] == 'BS' and metadata['eventTypes'][0] != 'IS_SIMULATION':
        rec.doTruth.set_Value_and_Lock(False)
        logAutoConfiguration.info("Input is bytestream. Auto-configuring doTruth=%s"%rec.doTruth())

    if metadata['file_type'] == 'POOL':
        itemsList = convert_itemList(layout='#join')
        itemsHaveTruth=False
        for item in itemsList:
            if item.startswith('McEventCollection#') or item.startswith("xAOD::TruthEventContainer#"):
                itemsHaveTruth=True

        rec.doTruth.set_Value_and_Lock(itemsHaveTruth)
        if itemsHaveTruth:
            logAutoConfiguration.info("Input has McEventCollection. Auto-configuring doTruth=%s"%rec.doTruth())
        else:
            logAutoConfiguration.info("Input has no McEventCollection. Auto-configuring doTruth=%s"%rec.doTruth())
        
    return

def IsInInputFile(collectionname,key=None):
    logAutoConfiguration.info("in IsInInputFile...")
    try:
        from PyUtils.MetaReaderPeeker import metadata, convert_itemList
        if metadata['file_type'] == 'POOL':
            try:
                ItemDic = convert_itemList(layout='dict')
                if collectionname in ItemDic:
                    logAutoConfiguration.info("found collection with name %s in input file." % collectionname)
                    print(ItemDic[collectionname])
                    if key is None:
                        logAutoConfiguration.info("no explicit storegate key given. Returning True")
                        return True
                    if key in ItemDic[collectionname]:
                        logAutoConfiguration.info("collection with key %s is in input file. Returning True" % key)
                        return True
                    else:
                        logAutoConfiguration.info("collection with key %s is NOT in input file. Returning False" % key)
                        return False
                    logAutoConfiguration.info("Shouldn't be here !")
                    return False
            except Exception:
                logAutoConfiguration.warning("IsInInputFile: Something's wrong. Wrong file:%s ", metadata['file_name'])
                import traceback
                logAutoConfiguration.warning(traceback.format_exc())
    except Exception:
        logAutoConfiguration.warning("Could not run IsInInputFile. input file maybe not specified at this point")#

    logAutoConfiguration.info("looks like object of name %s is NOT in input file. Returning False" % key)
    return False

def ConfigureSimulationOrRealData():
    from PyUtils.MetaReaderPeeker import metadata
    whatIsIt="N/A"
    try:
        whatIsIt = metadata['eventTypes'][0]
    except:
        if metadata['nentries'] == 0:
            logAutoConfiguration.error("Input file has no events: unable to configure SimulationOrRealData.")
            return
        pass
            
    if whatIsIt=='IS_DATA':
        logAutoConfiguration.info('Input file is real data.')
        from AthenaCommon.GlobalFlags import globalflags
        globalflags.DataSource='data'
        rec.Commissioning=True                   
    elif whatIsIt=='IS_SIMULATION':
        from AthenaCommon.GlobalFlags import globalflags
        globalflags.DataSource.set_Value_and_Lock('geant4')
        logAutoConfiguration.info('Input file is simulation.')
    else:
        raise RuntimeError("Don't know how to interpret key %s"%whatIsIt)
    return

def ConfigureFromListOfKeys(l):
    keys=set(l)
    allDefaultKeys=frozenset(('ProjectName','RealOrSim','FieldAndGeo','BeamType','ConditionsTag','DoTruth','InputType','BeamEnergy','LumiFlags','TriggerStream'))
    if 'everything' in keys:
        keys.remove('everything')
        keys |= allDefaultKeys;
        logAutoConfiguration.info("Auto-configuration key 'everything' requested. All default keys will be used.")

    logAutoConfiguration.info("Auto-configuration will procede according to the following keys:")
    logAutoConfiguration.info(keys.__str__())

    ######################################################################
    # Auto-configure according to requested keys, in a pre-defined order #
    ######################################################################
    if "ProjectName" in keys:  #has to be before BeamType        
        rec.projectName=GetProjectName()

    if "BeamType" in keys:  #has to be before RealOrSim (DC: not yet, but maybe in the future?)
        ConfigureBeamType()

    if "RealOrSim" in keys:
        ConfigureSimulationOrRealData()
    
    if "TriggerStream" in keys: 
        ConfigureTriggerStream()                

    if "ConditionsTag" in keys:
        ConfigureConditionsTag()
        
    if "FieldAndGeo" in keys:
        ConfigureFieldAndGeo()

    if "Field" in keys:
        ConfigureField()

    if "Geo" in keys:
        ConfigureGeo()

    if "DoTruth" in keys:
        ConfigureDoTruth()

    if "InputType" in keys:
        ConfigureInputType()
        
    if "BeamEnergy" in keys:
        ConfigureBeamEnergy()
        
    if "LumiFlags" in keys:
        ConfigureLumiFlags()

    if "FieldAndGeoESDToESD" in keys:
        ConfigureFieldAndGeoESDtoESD()

    #Final sanity check...
    for key in keys:
        if not key in allDefaultKeys:
            #these special keys are allowed to not be allDefaultKeys, however check for conflicts
            if key=="FieldAndGeoESDToESD":
                if ItemInList("FieldAndGeo",l):
                    raise RuntimeError("Auto-configure either FieldAndGeo or FieldAndGeoESDtoESD, but doing both is not allowed.")
                pass
            elif key=="TriggerStream" or key=="Field" or key=="Geo":
                pass
            else:
                raise RuntimeError("Unknown key '%s'. This cannot be used with rec.AutoConfiguration."%key)

    logAutoConfiguration.info("Leaving auto-configuration")
    return
