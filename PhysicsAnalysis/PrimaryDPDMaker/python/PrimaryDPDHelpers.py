# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

##-----------------------------------------------------------------------------
## Name: PrimaryDPDHelpers.py
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
##
## Description: Some useful functions used by the PrimaryDPDMaker
##
##-----------------------------------------------------------------------------


## for messaging
from AthenaCommon.Logging import logging
msg = logging.getLogger( 'PrimaryDPDHelpers' )


## Import needed modules
import PyUtils.RootUtils as ru
ROOT = ru.import_root()


# This function correctly determines the name of the output file.
# If the transform set the FileName to the "official" name and locked
# the jobProperty, that one will be used. Otherwise the user sets it.
def buildFileName( jobPropFlag ):
    if jobPropFlag.is_locked() :
        fileName = jobPropFlag.FileName
        pass
    else :
        from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
        fileName = primDPD.OutputDirectoryName()+primDPD.OutputPoolRootFileNamePrefix()+jobPropFlag.StreamName+primDPD.OutputMiddleName()+".pool.root"
        pass

    return fileName





## A small function that checks the author of the egamma object
def checkEgammaAuthor(egammaCand, authorName="either"):
    try:
        if authorName == "either" or authorName == "" :
            return True

        elif authorName == "central" :
            if ROOT.egamma.author(egammaCand) != 8 :
                return True
            else :
                msg.debug("Egamma object of type=%s failed authorName=%s", type(egammaCand), authorName)
                return False

        elif authorName == "forward" :
            if ROOT.egamma.author(egammaCand) == 8 :
                return True
            else :
                msg.debug("Egamma object of type=%s failed authorName=%s", type(egammaCand), authorName)
                return False

        elif authorName == "onlyEgamma" or authorName == "onlyElectron" :
            if ROOT.egamma.author(egammaCand) == 1 :
                return True
            else :
                msg.debug("Egamma object of type=%s failed authorName=%s", type(egammaCand), authorName)
                return False

        elif authorName == "egamma" or authorName == "Electron" :
            if ROOT.egamma.author(egammaCand) == 1 or ROOT.egamma.author(egammaCand) == 3 :
                return True
            else :
                msg.debug("Egamma object of type=%s failed authorName=%s", type(egammaCand), authorName)
                return False

        elif authorName == "onlySofte" :
            if ROOT.egamma.author(egammaCand) == 2 :
                return True
            else :
                msg.debug("Egamma object of type=%s failed authorName=%s", type(egammaCand), authorName)
                return False

        elif authorName == "softe" :
            if ROOT.egamma.author(egammaCand) == 2 or ROOT.egamma.author(egammaCand) == 3 :
                return True
            else :
                msg.debug("Egamma object of type=%s failed authorName=%s", type(egammaCand), authorName)
                return False

        elif authorName == "eitherElectron" :
            if ROOT.egamma.author(egammaCand) == 1 \
                   or ROOT.egamma.author(egammaCand) == 2 \
                   or ROOT.egamma.author(egammaCand) == 3 :
                return True
            else :
                msg.debug("Egamma object of type=%s failed authorName=%s", type(egammaCand), authorName)
                return False

        elif authorName == "Photon" :
            if ROOT.egamma.author(egammaCand) == 4 :
                return True
            else :
                msg.debug("Egamma object of type=%s failed authorName=%s", type(egammaCand), authorName)
                return False

        else :
            msg.debug("Unrecognized authorName=%s of egamma object of type=%s", authorName, type(egammaCand))
            return False
        

    except TypeError as e:
        msg.error("Type error when calling checkEgammaAuthor %s",e)
        import traceback
        msg.error(traceback.format_exc())
        return e


    msg.error("Problem checking the author of the egamm object with provided authorName=%s and type of passed object=%s", authorName, type(egammaCand))
    return False






## A small function that checks the IsEM particle ID for electrons
def checkElectronIsEM(electronCand, isemName="None"):
    try:
        if isemName == "None" :
            return True

        elif isemName == "Loose" :
            if electronCand.isem(ROOT.egammaPID.ElectronLoose) == 0 or electronCand.isem(ROOT.egammaPID.frwdElectronLoose) == 0 :
                return True
            else :
                msg.debug("Electron object of type=%s failed isemName=%s", type(electronCand), isemName)
                return False

        elif isemName == "Medium" :
            if electronCand.isem(ROOT.egammaPID.ElectronMedium) == 0 :
                return True
            else :
                msg.debug("Electron object of type=%s failed isemName=%s", type(electronCand), isemName)
                return False

        elif isemName == "Tight" :
            if electronCand.isem(ROOT.egammaPID.ElectronTight) == 0 or electronCand.isem(ROOT.egammaPID.frwdElectronTight) == 0 :
                return True
            else :
                msg.debug("Electron object of type=%s failed isemName=%s", type(electronCand), isemName)
                return False

        elif isemName == "TightNoIsolation" :
            if electronCand.isem(ROOT.egammaPID.ElectronTightNoIsolation) == 0 :
                return True
            else :
                msg.debug("Electron object of type=%s failed isemName=%s", type(electronCand), isemName)
                return False

        else :
            try :
                assert isinstance( isemName, str )
                cut = getattr( ROOT.egammaPID, isemName )
                if electronCand.isem( cut ) == 0 :
                    return True
                else :
                    msg.debug("Electron object of type=%s failed isemName=%s", type(electronCand), isemName)
                    return False
                pass
            except AttributeError as err :
                msg.error("AttributeError when calling checkElectronIsEM with isemName=%s of electron object of type=%s. The error is %s", isemName, type(electronCand), err)
                import traceback
                msg.error(traceback.format_exc())
                return False


    except TypeError as e:
        msg.error("Type error when calling checkElectronIsEM %s",e)
        import traceback
        msg.error(traceback.format_exc())
        return e

    msg.error("Problem checking the IsEM of the electron object with provided IsEM=%s and type of passed object=%s", isemName, type(electronCand))
    return False





## A small function that checks the IsEM particle ID for photons
def checkPhotonIsEM(photonCand, isemName="None"):
    try:
        if isemName == "None" :
            return True

        elif isemName == "Loose" or isemName == "PhotonLoose" :
            if photonCand.isem(ROOT.egammaPID.PhotonLoose) == 0 :
                return True
            else :
                msg.debug("Photon object of type=%s failed isemName=%s", type(photonCand), isemName)
                return False

        elif isemName == "Photon" or isemName == "PhotonTight" or isemName == "Tight" :
            if photonCand.isem(ROOT.egammaPID.PhotonTight) == 0 :
                return True
            else :
                msg.debug("Photon object of type=%s failed isemName=%s", type(photonCand), isemName)
                return False

        else :
            try :
                assert isinstance( isemName, str )
                cut = getattr( ROOT.egammaPID, isemName )
                if photonCand.isem( cut ) == 0 :
                    return True
                else :
                    msg.debug("Photon object of type=%s failed isemName=%s", type(photonCand), isemName)
                    return False
                pass
            except AttributeError as err :
                msg.error("AttributeError when calling checkElectronIsEM with isemName=%s of electron object of type=%s. The error is %s", isemName, type(photonCand), err)
                import traceback
                msg.error(traceback.format_exc())
                return False
            
    except TypeError as e:
        msg.error("Type error when calling checkPhotonIsEM %s",e)
        import traceback
        msg.error(traceback.format_exc())
        return e

    msg.error("Problem checking the IsEM of the photon object with provided IsEM=%s and type of passed object=%s", isemName, type(photonCand))
    return False


## A small function that checks the author of the mu object
def checkMuonAuthor(muonCandidate, authorName="all"):

    try:
        if authorName == "all" :
            return True
    except TypeError as e:
        msg.error("Type error when calling checkMuonAuthor %s",e)
        import traceback
        msg.error(traceback.format_exc())
        return e

    muCandAuthor = ''
    authorNum = muonCandidate.author()
    if authorNum == 1 or authorNum == 6 or authorNum == 12:
        muCandAuthor = 'combined'
    elif authorNum == 2 or authorNum == 7 or authorNum == 13:
        muCandAuthor = 'lowpt'
    elif authorNum == 4 or authorNum == 5 or authorNum == 10 or authorNum == 11 :
        muCandAuthor = 'standalone'
    elif authorNum == 14 :
        muCandAuthor = 'calo'
    try:

        if muCandAuthor == authorName :
            return True
        
        else :
            return False
    except TypeError as e:
        msg.error("Type error when calling checkMuonAuthor %s",e)
        import traceback
        msg.error(traceback.format_exc())
        return e
    
