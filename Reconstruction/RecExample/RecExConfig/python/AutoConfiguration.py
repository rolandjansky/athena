# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        AutoConfiguration.py
## Author:      David Cote (DESY/CERN), Michael Boehler (DESY)
## Created:     July 2008
## Description: This is a collection of so-called "auto-configuration" functions for RecExCommon
##=============================================================================
from AthenaCommon.GlobalFlags import globalflags
from RecExConfig.RecFlags import rec
from RecExConfig.RecoFunctions import ItemInList, ItemInListStartsWith,AddValidItemToList,RemoveValidItemFromList

from AthenaCommon.Logging import logging

logAutoConfiguration = logging.getLogger( 'AutoConfiguration' )

KnownCosmicsProjects=["data08_calocomm","data08_muoncomm","data08_cos","data08_cosmag","data08_idcomm",
                      "data09_cos","data09_cosmag","data09_idcomm","EO_NOISE","data09_calocomm","data09_muoncomm","data09_calophys",
                      "data10_muoncomm","data10_idcomm","data10_larcomm","data10_tilecomm","data10_calocomm","data10_calib","data10_cos",
                      "data11_calib","data11_calocomm","data11_cos","data11_idcomm","data11_larcomm","data11_muoncomm","data11_tilecomm",
                      "data12_calib","data12_calocomm","data12_cos","data12_idcomm","data12_larcomm","data12_muoncomm","data12_tilecomm",
                      "data13_calib","data13_calocomm","data13_cos","data13_idcomm","data13_larcomm","data13_muoncomm","data13_tilecomm",
                      "data14_calib","data14_calocomm","data14_cos","data14_idcomm","data14_larcomm","data14_muoncomm","data14_tilecomm",
                      "data15_calib","data15_calocomm","data15_cos","data15_idcomm","data15_larcomm","data15_muoncomm","data15_tilecomm",
                      "data16_calib","data16_calocomm","data16_cos","data16_idcomm","data16_larcomm","data16_muoncomm","data16_tilecomm"
                      ]

# Abandon the single-beam reconstruction all together, 26 February 2011 
Known1BeamProjects=["data08_1beam","data08_1beammag",
                    "data09_1beam","data09_1beammag",
                    "data10_1beam","data10_1beammag",
                    ]

KnownCollisionsProjects=["data08","data08_coll900","data09","data09_coll900","data09_900GeV","data09_2TeV",
                         "data10_900GeV","data10_2TeV","data10_7TeV","data10_10TeV","data10_comm",
                         "data11_7TeV","data11_8TeV","data11_2p76TeV","data11_comm","data11_900GeV","data11_1beam","data11_hip", 
                         "data12_8TeV","data12_comm","mc12_8TeV","IS_SIMULATION","data12_1beam","data12_900GeV",
                         "data13_8TeV","data13_comm","data13_2p76TeV","data13_1beam",
                         "data14_comm","data15_comm","data15_900GeV","data15_1beam","data15_13TeV","data15_5TeV",
                         "data16_comm","data16_13TeV","data16_900GeV","data16_1beam"
                         ]

KnownHeavyIonProjects=["data10_hi","data11_hi","data15_hi","data16_hi"]

KnownHeavyIonProtonProjects=["data12_hip","data13_hip"]

KnownTestProjects=["data_test"]

KnownProjects=[]
AddValidItemToList(KnownCosmicsProjects,KnownProjects)
AddValidItemToList(Known1BeamProjects,KnownProjects)
AddValidItemToList(KnownCollisionsProjects,KnownProjects)
AddValidItemToList(KnownHeavyIonProjects,KnownProjects)
AddValidItemToList(KnownHeavyIonProtonProjects,KnownProjects)
AddValidItemToList(KnownTestProjects,KnownProjects)

fullSolenoidCurrent=7730.0
fullToroidCurrent=20500.0

def GetRunNumber():
    runNb=None
    from RecExConfig.RecFlags import rec
    if rec.RunNumber.isDefault():
        from RecExConfig.InputFilePeeker import inputFileSummary
        try:
            runNb=inputFileSummary['run_number'][0]
        except Exception:
            from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
            if not athenaCommonFlags.isOnline(): logAutoConfiguration.error("No RunNumber stored in InputFile!")
    else:
        runNb=rec.RunNumber()
    logAutoConfiguration.debug("RunNumber is: %s",runNb)
    return runNb


def GetLBNumber():
    # empty files do not have any lumi blocks!!
    from RecExConfig.InputFilePeeker import inputFileSummary
    if inputFileSummary['nentries']==0:
        return None

    
    lbs=[0,]
    try:
        lbs=inputFileSummary['lumi_block']
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
    from RecExConfig.InputFilePeeker import inputFileSummary
    solenoidCurrent=None
    toroidCurrent=None
    if inputFileSummary['metadata'].has_key('/EXT/DCS/MAGNETS/SENSORDATA'):
        try:
            solenoidCurrent=inputFileSummary['metadata']['/EXT/DCS/MAGNETS/SENSORDATA']['CentralSol_Current']['value']
            toroidCurrent=inputFileSummary['metadata']['/EXT/DCS/MAGNETS/SENSORDATA']['Toroids_Current']['value']
        except:
            logAutoConfiguration.warning("Unable to find solenoid and toroid currents in /EXT/DCS/MAGNETS/SENSORDATA")

        # consistency check of GEO and b field values in case of simulated data
        if inputFileSummary['evt_type'][0]=='IS_SIMULATION':
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
    from RecExConfig.InputFilePeeker import inputFileSummary
    if inputFileSummary['file_type']=='bs' and inputFileSummary['evt_type'][0]!='IS_SIMULATION':
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
    elif inputFileSummary['file_type']=='bs' and inputFileSummary['evt_type'][0]=='IS_SIMULATION':
        logAutoConfiguration.info("Field info is not stored in MC BS values are set via conditions tag:")
        if solenoidCurrent==None or toroidCurrent==None:
            solenoidCurrent,toroidCurrent=GetApproximateFieldFromConditions()
        if solenoidCurrent==None or toroidCurrent==None:
            logAutoConfiguration.warning("BField of MC BS cannot be autoconfigured!! BField is turned ON")
            solenoidCurrent=fullSolenoidCurrent
            toroidCurrent=fullToroidCurrent
    elif inputFileSummary['file_type']=='pool':
        solenoidCurrent,toroidCurrent=GetFieldFromInputFile()
        if solenoidCurrent==None or toroidCurrent==None:
            solenoidCurrent,toroidCurrent=GetApproximateFieldFromConditions()             
        if solenoidCurrent==None or toroidCurrent==None:
            solenoidCurrent,toroidCurrent=GetApproximateFieldFromGeo() 
    else:
        raise RuntimeError("Don't know how to interpret file_type '%s'"%inputFileSummary['file_type'])

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

    from RecExConfig.InputFilePeeker import inputFileSummary
    if inputFileSummary['file_type']=='bs':
        geo='ATLAS-GEO-20-00-01'
        project=GetProjectName()
        if "data11" in project:
            geo='ATLAS-GEO-18-01-01'
        if "data10" in project or "data09" in project or "data08" in project:
            geo='ATLAS-GEO-16-00-01'
        if inputFileSummary['evt_type'][0]=='IS_SIMULATION':
            try: geo = inputFileSummary['geometry']
            except: logAutoConfiguration.warning("Input simulated bs file does not contain bs_metadata with geometry. Probably an old file.")
            pass
        globalflags.DetDescrVersion.set_Value_and_Lock(geo)
        logAutoConfiguration.info("Set GeometryVersion to '%s'"%geo)
            
    elif inputFileSummary['file_type']=='pool':
        # configure Geometry from input file
        globalflags.DetDescrVersion.set_Value_and_Lock(inputFileSummary['geometry'])
        logAutoConfiguration.info("Configured geometry from input file: %s ",globalflags.DetDescrVersion() )
    else:
        raise RuntimeError("Don't know how to interpret file_type '%s'"%inputFileSummary['file_type'])

    return
   

def ConfigureFieldAndGeo():
    from RecExConfig.InputFilePeeker import inputFileSummary
    if inputFileSummary['file_type']=='bs':        
        ConfigureField()
        ConfigureGeo()
    elif inputFileSummary['file_type']=='pool':
        ConfigureGeo()
        ConfigureField()
    else:
        raise RuntimeError("Don't know how to interpret file_type '%s'"%inputFileSummary['file_type'])
    return

def GetProjectName():
    # special option for online running
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    if athenaCommonFlags.isOnline():
        from RecExOnline.OnlineISConfiguration import GetRunType
        rec.projectName=GetRunType()[2]
    # continue with options for offline running
    if rec.projectName()=="":
        from RecExConfig.InputFilePeeker import inputFileSummary
        whatIsIt=None
        try:
            whatIsIt=inputFileSummary['evt_type'][0]
        except:
            pass
        if whatIsIt=='IS_SIMULATION':
            project='IS_SIMULATION'
        else:
            try:
                project = inputFileSummary['tag_info']['project_name']
            except:
                from RecExConfig.GetCool import cool
                project = cool.fileNameTag()
            pass
        #rec.projectName.set_Value_and_Lock(project)
        logAutoConfiguration.info("Success! GetProjectName() found a project named %s"%project)
    else:
        project=rec.projectName()
    if not ItemInList(project,KnownProjects):
        logAutoConfiguration.warning("Project '%s' is not part of the KnownProjects list."%project)
        #print KnownProjects

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
    from RecExConfig.InputFilePeeker import inputFileSummary
    try:
        listOfKnownBeamTypes=['cosmics' ,'singlebeam','collisions']
        if inputFileSummary['beam_type'][0] in listOfKnownBeamTypes:
            BeamType=inputFileSummary['beam_type'][0]
    except:
        logAutoConfiguration.info("beam type not stored in input file, set beam type according to project name...")
        
    if BeamType==None:
        project=GetProjectName()
        if ItemInList(project,KnownCosmicsProjects): BeamType='cosmics'
        elif ItemInList(project,KnownTestProjects): BeamType='cosmics'
        elif ItemInList(project,Known1BeamProjects): BeamType='singlebeam'
        elif ItemInList(project,KnownCollisionsProjects): BeamType='collisions'
        elif ItemInList(project,KnownHeavyIonProjects): BeamType='collisions'
        elif ItemInList(project,KnownHeavyIonProtonProjects): BeamType='collisions'
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
        if ItemInList(project,KnownHeavyIonProjects):
            rec.doHeavyIon.set_Value_and_Lock(True)
            rec.doHIP.set_Value_and_Lock(False)
            logAutoConfiguration.info("Set rec.doHeavyIon=True based on project tag '%s'" % project)
        elif ItemInList(project,KnownHeavyIonProtonProjects):
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
    from RecExConfig.InputFilePeeker import inputFileSummary
    from AthenaCommon.BeamFlags import jobproperties
    if inputFileSummary['evt_type'][0]=='IS_DATA':
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
            jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(inputFileSummary['metadata']['/Digitization/Parameters']['numberOfCollisions'])
        except:
            logAutoConfiguration.warning("numberOfCollisions could not be auto configured, no info available using default value: %s "
                                         ,jobproperties.Beam.numberOfCollisions() )
            return
                
    logAutoConfiguration.info("Auto configured numberOfCollisions: %s ", jobproperties.Beam.numberOfCollisions())
    return

def ConfigureBeamEnergy():
    from RecExConfig.InputFilePeeker import inputFileSummary
    from AthenaCommon.BeamFlags import jobproperties

    # pool files
    if inputFileSummary['file_type']=='pool':
        if type(inputFileSummary['beam_energy'][0])!=float:
            logAutoConfiguration.warning("beam energy is not storted in input file, setting default DATA options!!")
        else:
            jobproperties.Beam.energy.set_Value_and_Lock( float(inputFileSummary['beam_energy'][0]))
            logAutoConfiguration.info("Auto-configured beam energy : %s MeV" , jobproperties.Beam.energy()) 
            return
    # BS files    
    else:
        if inputFileSummary['evt_type'][0]=='IS_DATA':
            
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
                    beamEnergy = float( (str(projectName).split('_')[1]).replace('TeV','',1).replace('p','.'))/2 * 1000000.
                    if '5TeV' in projectName:
                        beamEnergy=2510000.
                elif projectName.endswith("_hi") or projectName.endswith("_hip"):
                    #beamEnergy=1380000. # 1.38 TeV (=3.5 TeV * (Z=82/A=208))
                    # Pb (p) beam energy in p-Pb collisions in 2011 will be 1.38 (3.5) TeV. sqrt(s_NN)=4.4 TeV  
                    #beamEnergy=1577000. # 1.577 TeV (=4 TeV * (Z=82/A=208))
                    # Pb (p) Beam energy in p-Pb collisions in 2012 will be 1.577 (4) TeV.
                    beamEnergy=2510000. # 2.51 TeV (=6.37 TeV * (Z=82/A=208))
                    # Pb (p) Beam energy in p-Pb collisions in 2012 will be 1.577 (4) TeV.
                else:
                    logAutoConfiguration.warning("Could not auto-configure beam energy based on project name: %s" , projectName)
                    return
                if beamEnergy!=None:
                    jobproperties.Beam.energy.set_Value_and_Lock(beamEnergy)
                
        elif inputFileSummary['evt_type'][0]=='IS_SIMULATION':
            if inputFileSummary['beam_energy'][0]!='N/A' and inputFileSummary['beam_energy'][0]!='' :
                jobproperties.Beam.energy.set_Value_and_Lock(float(inputFileSummary['beam_energy'][0]))
            else:
                logAutoConfiguration.warning("beam energy is not storted in input file!!") 
                return

    logAutoConfiguration.info("Auto-configured beam energy : %s MeV" , jobproperties.Beam.energy()) 
    return


def ConfigureBeamBunchSpacing():
    from AthenaCommon.BeamFlags import jobproperties
    from RecExConfig.InputFilePeeker import inputFileSummary
    # auto configure bunch spacing not yet possible for DATA 
    # ---> this is still missing
    if inputFileSummary['evt_type'][0]=='IS_SIMULATION':
        try:
            jobproperties.Beam.bunchSpacing.set_Value_and_Lock(inputFileSummary['metadata']['/Digitization/Parameters']['intraTrainBunchSpacing'])
            logAutoConfiguration.info("Auto configured bunchSpacing: %s ", jobproperties.Beam.bunchSpacing())
        except:
            logAutoConfiguration.warning("bunchSpacing could not be auto configured, no info available using default value: %s ",jobproperties.Beam.bunchSpacing() )
                        
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
            raise RuntimeError," readRDO locked True and no Stream1 nor StreamRDO nor StreamRAW !"
    if rec.readESD.is_locked() and rec.readESD():
        if 'StreamESD_ref' in streams:
            return  'StreamESD_ref'
        else:
            raise RuntimeError," readESD locked True and no StreamESD !"

    if rec.readAOD.is_locked() and rec.readAOD():
        if 'StreamAOD_ref' in streams:
            return  'StreamAOD_ref'
        else:
            raise RuntimeError," readAOD locked True and no StreamAOD !"


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
        raise RuntimeError, " no known streams !" 

    return None



def ConfigureInputType():
    from RecExConfig.InputFilePeeker import inputFileSummary
    from AthenaCommon.GlobalFlags  import globalflags
    streamsName=[]        
    if inputFileSummary['file_type']=='bs':
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
        if inputFileSummary.has_key('stream_names'):
            streamsName=inputFileSummary['stream_names']
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
    ##
      
    if inputFileSummary['TagStreamsRef']!=None:
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
    elif ItemInListStartsWith ("Stream1", streamsName)  or ItemInListStartsWith ("StreamRDO", streamsName) or OverlapLists(streamsName,listRAWtoDPD):
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
        logAutoConfiguration.info("Input StreamHITS detected")
    elif inputFileSummary['file_type']!='bs':
        raise RuntimeError("ConfigureInputType cannot handle streamsName==%s"%streamsName)
    return

def ConfigureTriggerStream():
    from RecExConfig.InputFilePeeker import inputFileSummary
    streamName=''

    if inputFileSummary['file_type'] == 'bs':
        try:
            streamName=inputFileSummary['bs_metadata']['Stream'].split('_')[1]
        except:
            logAutoConfiguration.warning("Input file does not contain bs_metadata! Trying to specify otherwise!")
    elif inputFileSummary['file_type'] == 'pool':    
        try:
            streamName=inputFileSummary['tag_info']['triggerStreamOfFile']
        except:
            logAutoConfiguration.warning("Input file does not contain triggerStreamOfFile! Trying to specify otherwise!")

    if streamName=='':
        try:
            if len(inputFileSummary['stream_tags'])==1:
                streamName=inputFileSummary['stream_tags'][0]['stream_name']
        except:
            logAutoConfiguration.warning("No trigger stream found in input file!!! ") 


    if streamName=='':
        logAutoConfiguration.info("Failed to find triggerStream from inputFileSummary. OK for MC but can be problematic for data.")
        logAutoConfiguration.info("Keeping input value untouched: rec.triggerStream='%s'"%rec.triggerStream())
        return
    
    logAutoConfiguration.info("Set rec.triggerStream='%s' from inputFileSummary."%streamName)
    rec.triggerStream=streamName
    return


def ConfigureConditionsTag(conditionsTagPrefix,conditionsTagSuffix):
    from AthenaCommon.BFieldFlags import jobproperties
    if globalflags.ConditionsTag.is_locked():
        logAutoConfiguration.info("conditionsTag is locked to value: '%s'."%globalflags.ConditionsTag())
        return
    
    from RecExConfig.InputFilePeeker import inputFileSummary
    if inputFileSummary['file_type'] == 'pool':

        if conditionsTagPrefix!=None and conditionsTagSuffix!=None:
            try: 
                ConfigureConditionsTagName(conditionsTagPrefix,conditionsTagSuffix)
            except:
                logAutoConfiguration.warning("ConditionsTag could not be auto-configured based on input pre- and suffix!!! ")
        else:
            try: 
                globalflags.ConditionsTag.set_Value_and_Lock(inputFileSummary['conditions_tag'])
                logAutoConfiguration.info("Auto-configured ConditionsTag '%s' from inputFileSummary ",globalflags.ConditionsTag())
            except:
                logAutoConfiguration.warning("ConditionsTag could not be auto-configured no info stored in inputFileSummary!!!")

            
    elif inputFileSummary['file_type'] == 'bs' and inputFileSummary['evt_type'][0]=='IS_SIMULATION':
        cond="OFLCOND-SIM-00-00-00"
        try:
            cond=inputFileSummary['conditions_tag']
        except:
            logAutoConfiguration.warning("Input simulated bs file does not contain bs_metadata with conditions_tag !")
        globalflags.ConditionsTag.set_Value_and_Lock(cond)
        logAutoConfiguration.info("Set conditionsTag to '%s'"%cond)
    elif inputFileSummary['file_type'] == 'bs' and inputFileSummary['evt_type'][0]=='IS_DATA':
        ConfigureConditionsTagName(conditionsTagPrefix,conditionsTagSuffix)  
    else:
        raise RuntimeError("Don't know how to configure conditionsTag for file_type: %s"%inputFileSummary['file_type'])
    return



def ConfigureConditionsTagName(conditionsTagPrefix,conditionsTagSuffix):

    # setting a default conditions tag name   
    if conditionsTagPrefix==None:
        conditionsTagPrefix='COMCOND-'
        
        # Online Reconstruction setup
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        if athenaCommonFlags.isOnline():
            conditionsTagPrefix+='HLTP'
        else:
            conditionsTagPrefix+='BLKP'

    if conditionsTagSuffix is None:
        #2012 way of running: No mag-field dependency
        if athenaCommonFlags.isOnline():
            conditionsTagSuffix="-004-01"
        else:
            conditionsTagSuffix="A-006-01"
    else:
                
        # auto configuring beamtype and magnetic field charakter in conditions tag name    
        MagField=''            
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        if not athenaCommonFlags.isOnline():
            from AthenaCommon.BFieldFlags import jobproperties        
            # Setting character for magnetig field setup in conditions tag
            #configuring conditionsTag based on magnetic field status
            ConfigureField()
            #define toroid/solenoid on/off
        
            toroidOn=(jobproperties.BField.barrelToroidOn() and jobproperties.BField.endcapToroidOn())
            solenoidOn=jobproperties.BField.solenoidOn()
        
            #consistency check...
            if jobproperties.BField.barrelToroidOn()==False and jobproperties.BField.endcapToroidOn()==True:
                raise RuntimeError("Don't know how to configure conditionsTag when barrelToroid==False and endcapToroidOn==True")
            if jobproperties.BField.barrelToroidOn()==True and jobproperties.BField.endcapToroidOn()==False:
                raise RuntimeError("Don't know how to configure conditionsTag when barrelToroid==True and endcapToroidOn==False")
        
        
            if toroidOn==True and solenoidOn==True:     MagField='ST'
            elif toroidOn==False and solenoidOn==True:  MagField='S' 
            elif toroidOn==True and solenoidOn==False:  MagField='T' 
            elif toroidOn==False and solenoidOn==False: MagField='' 
            else:
                raise RuntimeError("Logic error!!")
        
            conditionsTagPrefix+=MagField
        pass
    pass

    condName=conditionsTagPrefix+conditionsTagSuffix
    globalflags.ConditionsTag.set_Value_and_Lock(condName)
    logAutoConfiguration.info("Auto-configured conditionsTag '%s'."%globalflags.ConditionsTag())
    return




def ConfigureConditionsTagESDtoESD():
    if globalflags.ConditionsTag.is_locked():
        logAutoConfiguration.error("conditionsTag was locked to '%s'. This is not expected.",globalflags.ConditionsTag())
    
    from RecExConfig.InputFilePeeker import inputFileSummary
    if inputFileSummary['file_type'] == 'pool':
        oldTag=inputFileSummary['conditions_tag']
        if oldTag=='COMCOND-REPC-002-10': newTag='COMCOND-REPC-004-00'
        elif oldTag=='COMCOND-REPC-002-11': newTag='COMCOND-REPC-004-01'
        elif oldTag=='COMCOND-REPC-002-12': newTag='COMCOND-REPC-004-02'
        elif oldTag=='COMCOND-REPC-002-13': newTag='COMCOND-REPC-004-03'
        elif oldTag.startswith('COMCOND-REPC-004-'): newTag=oldTag
        else: raise RuntimeError("Don't know how to configure conditionsTag for input conditionsTag '%s'"%oldTag)
        globalflags.ConditionsTag.set_Value_and_Lock(newTag)
    else:
        raise RuntimeError("file_type is %s but ConfigureConditionsTagESDtoESD is only defined for pool inputs"%inputFileSummary['file_type'])

    logAutoConfiguration.info("ConditionsTag set to: '%s'"%globalflags.ConditionsTag())
    return

def ConfigureFieldAndGeoESDtoESD():
    from RecExConfig.InputFilePeeker import inputFileSummary
    if inputFileSummary['file_type']=='pool':
        logAutoConfiguration.info("InputFile is pool format. Configuring B-Field based on Geometry.")
            
        # configure Geometry for pool input
        if globalflags.DetDescrVersion.is_locked():
            logAutoConfiguration.error("Geometry was locked to '%s'. This is not expected.",globalflags.DetDescrVersion())

        inGeo=inputFileSummary['geometry']
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
        raise RuntimeError("file_type is %s but ConfigureFieldAndGeoESDtoESD is only defined for pool inputs"%inputFileSummary['file_type'])

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
    
    from RecExConfig.InputFilePeeker import inputFileSummary
    if inputFileSummary['file_type'] == 'bs'and inputFileSummary['evt_type'][0]!='IS_SIMULATION':
        rec.doTruth.set_Value_and_Lock(False)
        logAutoConfiguration.info("Input is bytestream. Auto-configuring doTruth=%s"%rec.doTruth())

    if inputFileSummary['file_type'] == 'pool':
        itemsList=inputFileSummary['eventdata_itemsList']
        # if no truth in input file, switch doTruth to False
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
        from RecExConfig.InputFilePeeker import inputFileSummary
        if inputFileSummary['file_type'] == 'pool':
            try:
                ItemDic=inputFileSummary['eventdata_itemsDic']
                if ItemDic.has_key(collectionname):
                    logAutoConfiguration.info("found collection with name %s in input file." % collectionname)
                    print ItemDic[collectionname]
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
                logAutoConfiguration.warning("IsInInputFile: Something's wrong. Wrong file:%s ", inputFileSummary['file_name'])
    except Exception:
        logAutoConfiguration.warning("Could not run IsInInputFile. input file maybe not specified at this point")#

    logAutoConfiguration.info("looks like object of name %s is NOT in input file. Returning False" % object)
    return False

def ConfigureSimulationOrRealData():
    from RecExConfig.InputFilePeeker import inputFileSummary
    whatIsIt="N/A"
    try:
        whatIsIt=inputFileSummary['evt_type'][0]
    except:
        if inputFileSummary['nentries']==0:
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
    allDefaultKeys=['ProjectName','RealOrSim','FieldAndGeo','BeamType','ConditionsTag','DoTruth','InputType','BeamEnergy','LumiFlags','TriggerStream']
    if l.__contains__('everything'):
        l.remove('everything')
        AddValidItemToList(allDefaultKeys,l)
        logAutoConfiguration.info("Auto-configuration key 'everything' requested. All default keys will be used.")

    logAutoConfiguration.info("Auto-configuration will procede according to the following keys:")
    logAutoConfiguration.info(l.__str__())

    ######################################################################
    # Auto-configure according to requested keys, in a pre-defined order #
    ######################################################################
    if ItemInList("ProjectName",l):  #has to be before BeamType        
        rec.projectName=GetProjectName()

    if ItemInList("BeamType",l):  #has to be before RealOrSim (DC: not yet, but maybe in the future?)
        ConfigureBeamType()

    if ItemInList("RealOrSim",l):
        ConfigureSimulationOrRealData()
    
    if ItemInList("TriggerStream",l): #has to be before ConditionsTag
        ConfigureTriggerStream()                

    if ItemInList("ConditionsTag",l) or ItemInListStartsWith('ConditionsTag=',l):
        conditionsTagPrefix=None
        conditionsTagSuffix=None
        # check if the conditions tag has to be set for re-processing
        if ItemInList("Repro",l):
            l.remove('Repro')
            conditionsTagPrefix='COMCOND-REPP'
            conditionsTagSuffix='-006-00'
        # check if the conditions tag suffix is set by user
        for key in l:
            if key.startswith('ConditionsTag='):
                l.remove(key)
                try:
                    temp_string=key.split('=')[1].split('*')
                    conditionsTagPrefix=temp_string[0]
                    conditionsTagSuffix=temp_string[1]
                except:
                    raise RuntimeError("Syntax error, unable to auto-configure ConditionsTag %s!! (use syntax like: COMCOND-ES1P*-002-00" %key )
                    
        # configure conditions tag finally
        ConfigureConditionsTag(conditionsTagPrefix,conditionsTagSuffix)
            
        
    if ItemInList("FieldAndGeo",l):
        ConfigureFieldAndGeo()

    if ItemInList("Field",l):
        ConfigureField()

    if ItemInList("Geo",l):
        ConfigureGeo()

    if ItemInList("DoTruth",l):
        ConfigureDoTruth()

    if ItemInList("InputType",l):
        ConfigureInputType()
        
    if ItemInList("BeamEnergy",l):
        ConfigureBeamEnergy()
        
    if ItemInList("LumiFlags",l):
        ConfigureLumiFlags()

    if ItemInList("ConditionsTagESDToESD",l):
        ConfigureConditionsTagESDtoESD()

    if ItemInList("FieldAndGeoESDToESD",l):
        ConfigureFieldAndGeoESDtoESD()

    #Final sanity check...
    for key in l:
        if not ItemInList(key,allDefaultKeys):
            #these special keys are allowed to not be allDefaultKeys, however check for conflicts

            if key=="ConditionsTagESDToESD":
                if ItemInList("ConditionsTag",l):
                    raise RuntimeError("Auto-configure either ConditionsTag or ConditionsTagESDToESD, but doing both is not allowed.")
                pass
            elif key=="FieldAndGeoESDToESD":
                if ItemInList("FieldAndGeo",l):
                    raise RuntimeError("Auto-configure either FieldAndGeo or FieldAndGeoESDtoESD, but doing both is not allowed.")
                pass
            elif key=="TriggerStream" or key=="Field" or key=="Geo":
                pass
            else:
                raise RuntimeError("Unknown key '%s'. This cannot be used with rec.AutoConfiguration."%key)

    logAutoConfiguration.info("Leaving auto-configuration")
    return
