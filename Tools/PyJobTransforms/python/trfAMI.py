from future.utils import iteritems
from future.utils import listitems
from builtins import zip

from builtins import object
from builtins import range
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## @package PyJobTransforms.trfAMI
#  @brief Utilities for configuration of transforms via AMI tags
#  @author atlas-comp-transforms-dev@cern.ch
#  @version $Id$



import ast
import json
import os
import traceback
from json import dumps

import logging
msg = logging.getLogger(__name__)

from PyJobTransforms.trfExceptions import TransformAMIException
from PyJobTransforms.trfDefaultFiles import getInputFileName, getOutputFileName
from PyJobTransforms.trfUtils import convertToStr

from PyJobTransforms.trfExitCodes import trfExit
AMIerrorCode=trfExit.nameToCode('TRF_AMI_ERROR')


## @brief Stores the configuration of a transform 
class TrfConfig(object):
    def __init__(self):
        self.name=None
        self.release=None
        self.physics={}
        self.inFiles={}
        self.outFiles={}
        self.outputs={}
        self.inDS=None
        self.outfmts=[]
        self.newTransform=False

    def __iter__(self):
        theDict=self.inFiles.copy()
        theDict.update(self.outFiles)
        theDict.update(self.physics)
        for (k,v) in iteritems(theDict):
            yield k,v

    def __str__(self):
        string = 'asetup '+self.release+'\n'+self.name
        string += self._argsToString(self.physics) +'\n'
        
        string +='\nInput file arguments:\n'
        if self.inFiles:
            string += self._argsToString(self.inFiles) +'\n'
        if self.inDS:    
            string +='\nExample input dataset: '+ self.inDS + '\n'

        string +='\nOutput file arguments:\n'
        if self.outFiles:
            string += self._argsToString(self.outFiles) + '\n'

        string +='\nAMI outputs:\n'
        if self.outputs != {}:
            string += self.outputs + '\n'
        
        if self.outfmts:    
            string += '\nPossible output data types: '+ str(self.outfmts) + '\n'
        return string

    def _argsToString(self, adict):
        string=''
        for (k,v) in iteritems(adict):
            if self.newTransform:
                if not k.startswith('--'):
                    k = "--"+k
                    # Now the tricky bit: we have to hackishly massage back to a CLI
                    # value depending on the value type and possibly the key
                    # type
                if isinstance(v, dict):
                    # Should be a substep argument
                    if 'Exec' in k: # preExec, postExec
                        string += " " + k
                        for vk, vv in iteritems(v):
                            string += " " + _parseExecDict(vk, vv)
                    elif 'Include' in k: # preInclude, postInclude
                        string += " " + k
                        for vk, vv in iteritems(v):
                            string += " " + _parseIncludeDict(vk, vv)
                    else:
                        # Misc substep string/number argument...?
                        # Be careful with the triggerConfig, which is a bit special in the separator
                        if 'triggerConfig' in k:
                            separator='='
                        else:
                            separator=':'
                        string += " " + k
                        for vk, vv in iteritems(v):
                            string += " " + vk + separator + vv
                elif isinstance(v, (list, tuple)):
                    # athenaopts are special - space separated
                    if "athenaopts" in k:
                        string += " " + k + "=" + "'" + " ".join(v).replace("'", "\\'") + "'"
                    elif "Exec" in k or "Include" in k:
                        # Special intermediate treatment for pre/postExec from prodsys
                        string += " " + k + " " + " ".join(["'"+element.replace("'", "\\'")+"'" for element in v])
                    else:
                        string += " " + k + "=" + " ".join(["'" + element.replace("'", "\\'") + "'" for element in v])
                else:
                    # Assume some vanilla value
                    string +=" "+k+" "+"'"+str(v).replace("'", "\\'")+"'"
            else:
                string +=" "+k+"="+"'"+str(v).replace("'", "\\'")+"'"
#            string += '\n'
        return string

## @brief Back convert a pre/postExec dictionary into a set of command 
#  line compatible strings
def _parseExecDict(substep, value):
    string = ""
    for bit in value:
        string += " '" + substep + ":" + str(bit).replace("'", "\\'")+"'"
    return string

## @brief Back convert a pre/postInclude dictionary into a set of command 
#  line compatible strings
#  By default use a comma for joining up the list values
def _parseIncludeDict(substep, value, joinWithChar = ","):
    string = "'"
    string += substep + ":" + joinWithChar.join(value).replace("'", "\\'")+"'"
    return string

def isNewAMITag(tag):
    newTagDict = {
        'a' : 764,
        'b' : 545,
        'c' : 864,
        'd' : 1351,
        'e' : 3764,
        'f' : 557,
        'g' : 46,
        'h' : 32,
        'j' : 46,
        'k' : 34,
        'm' : 1377,
        'o' : 4741,
        'p' : 2295,
        'q' : 430,
        'r' : 6382,
        's' : 2559,
        't' : 597,
        'u' : 51,
        'v' : 139,
        'w' : 501,
        'x' : 302,
    }

    if tag[0] in newTagDict:
        if int(tag[1:]) > newTagDict[tag[0]]:
            msg.debug('it is a new tag')
            return True

    msg.debug('it is NOT a new tag')
    return False


## @brief Stores the information about a given tag.
class TagInfo(object):
    def __init__(self, tag, suppressNonJobOptions = True):
        self._tag=tag
        self._isNewTag = isNewAMITag(tag)
        self._isProdSys=None
        self._trfs=None
        self._suppressNonJobOptions = suppressNonJobOptions

    @property    
    def isProdSys(self):
        if self._isProdSys is None:
            if self._isNewTag:
                #probably false, as we need to get stuff from ami
                self._isProdSys = False
            else:
                prodtags=getProdSysTagsCharacters()
                if self._tag[0] in prodtags:
                    self._isProdSys=True
                else:
                    self._isProdSys=False
        return self._isProdSys


    @property
    def trfs(self):
        if self._trfs is None:
            if self.isProdSys:
                self._trfs=getTrfConfigFromPANDA(self._tag)
            else:    
                self._trfs = getTrfConfigFromAMI(self._tag, self._suppressNonJobOptions)
        return self._trfs


    def __str__(self):
        string = '\nInformation about tag '+self._tag+':\n'

        if self.isProdSys:
            string +='This is a ProdSys tag. Input and output file arguments are likely to be missing because they are often not part of the tag definition.\n'
        else:
            string +='This is a T0 tag.\n'

        string +='This tag consists of ' + str(len(self.trfs)) + ' transform command(s).\n' 
        string += 'Transform commands follow below.\n'
        string += 'Input and output file names (if present) are only suggestions.\n'

        for trf in self.trfs:
            string+='\n'+str(trf)+'\n'

        return string


## @brief Get an AMI client
#  @note Always return a client to the primary replica.
#  The caller is allowed to update the replica via the 
#  config.endpoints value.
#  @returns pyAMI.client.Client instance
def getAMIClient(endpoints = ['atlas-replica','atlas']):
    msg.debug('Getting AMI client...')
    
    try:
        from pyAMI.client import Client
    except ImportError:
        raise TransformAMIException(AMIerrorCode, 'Import of pyAMI modules failed.')
        
    msg.debug("Attempting to get AMI client for endpoints {0}".format(endpoints))
    amiclient = Client(endpoints, ignore_proxy = True)
    return amiclient

## @brief Get list of characters of ProdSys tags
#  @returns list of characters
def getProdSysTagsCharacters():
    # Due to move to uniform tag definition in AMI this list is now frozen
    # So just hard code it
    msg.debug('Getting list of ProdSys tag characters...')

    defaultList=['z', 'p', 'e', 's', 'd', 'r', 't', 'a', 'b', 'w']

    return defaultList


## @brief Get PANDA client
#  @returns cx_Oracle cursor instance
def getPANDAClient():
    msg.debug('Getting PANDA client...')
    try:
        import cx_Oracle
    except ImportError:
        raise TransformAMIException(AMIerrorCode, 'Import of cx_Oracle failed (is Oracle setup on this machine?).')
        
    try:
        cur = cx_Oracle.connect('atlas_grisli_r/panda_c10@adcr_panda').cursor()
    except Exception: 
        msg.debug('An exception occurred while connecting to PANDA database: %s', traceback.format_exc())
        raise TransformAMIException(AMIerrorCode, 'Failed to get PANDA client connection (N.B. this does not work from outside CERN).')
        
    return cur

## @brief Un-escape information from PANDA
#  @details Provided by Pavel.
def ReadablePANDA(s):
    return s.replace('%0B',' ').replace('%9B','; ').replace('%8B','"').replace('%3B',';').replace('%2C',',').replace('%2B','+')


## @brief Get information about a ProdSys tag from PANDA
#  @param tag Tag for which information is requested
#  @returns list of PyJoCbTransforms.trfAMI.TRFConfig instances
def getTrfConfigFromPANDA(tag):
    
    msg.debug('Using PANDA to get info about tag %s', tag)
            
    try:
        pandaclient=getPANDAClient()
        pandaclient.execute("select trf,trfv,lparams,vparams,formats,cache from t_trf_config where tag='%s' and cid=%d" %(tag[:1],int(tag[1:]) ) )
        result=pandaclient.fetchone()
    except Exception:
        msg.info('An exception occurred: %s', traceback.format_exc())
        raise TransformAMIException(AMIerrorCode, 'Getting tag info from PANDA failed.')

    if result is None:
        raise TransformAMIException(AMIerrorCode, 'Tag %s not found in PANDA database' % tag)
    
    msg.debug('Raw data returned from panda DB is:' + os.linesep + str(result))
    
    trfn=result[0].split(',')
    msg.debug('List of transforms: %s', trfn)
    trfv=result[1].split(',')
    msg.debug('List of releases: %s', trfv)
    lparams=result[2].split(';')
    msg.debug('List of arguments: %s', lparams)
    vparams=result[3].split(';')
    msg.debug('List of argument values: %s', vparams)
    formats=result[4].split('.')
    msg.debug('List of formats: %s', formats)
    cache=result[5].split(',')
    msg.debug('List of caches: %s', formats)


    if not ( len(trfn) == len(trfv) == len(lparams) == len(vparams) ):
        raise TransformAMIException(AMIerrorCode, 'Inconsistency in number of trfs.')
    
    # Cache can be a single value, in which case it needs replicated for other
    # transform steps, or it can be multivalued - great schema design guys :-(
    if len(cache) != len(trfv):
        if len(cache) == 1:
            cache = cache * len(trfv)
        else:
            raise TransformAMIException(AMIerrorCode, 'Inconsistency in number of caches entries vs. release numbers ({0}; {1}).'.format(cache, trfv))

    listOfTrfs=[]

    for iTrf in range(len(trfn)):

        trf = TrfConfig()
        trf.name =trfn[iTrf]
        if '_tf.py' in trf.name:
            trf.newTransform=True
        else:
            trf.newTransform=False
        trf.release=trfv[iTrf] + "," + cache[iTrf]

        keys=lparams[iTrf].split(',')
        values=vparams[iTrf].split(',')

        if ( len(keys) != len(values) ):
            raise TransformAMIException(AMIerrorCode, 'Inconsistency in number of arguments.')

        physics = dict( (k, ReadablePANDA(v) ) for (k,v) in zip(keys, values))
        # Hack to correct trigger keys being stored with spaces in panda  
        for k, v in iteritems(physics):
            if 'triggerConfig' in k or 'triggerConfigByRun' in k:
                if ' ' in v:
                    physics[k] = v.replace(' ', ',')
                    msg.warning('Attempted to correct illegal trigger configuration string: {0} -> {1}'.format(v, physics[k]))
            if 'Exec' in k:
                # Mash up to a list, where %8C is used as the quote delimitation character
                physics[k] = [ execElement.replace("%8C", "") for execElement in v.split("%8C %8C") ]

        msg.debug("Checking for pseudo-argument internal to ProdSys...")
        if 'extraParameter' in physics:
            val=physics.pop('extraParameter')
            msg.debug("Removed extraParamater=%s from arguments.", val)

        msg.debug("Checking for input/output file arguments...")
        for arg in list(physics):
            if arg.lstrip('-').startswith('input') and arg.endswith('File'):
                value=physics.pop(arg)
                msg.debug("Found input file argument %s=%s.", arg, value ) 
                fmt=arg.lstrip('-').replace('input','').replace('File','')
                trf.inFiles[arg]=getInputFileName(arg)
            elif arg.lstrip('-').startswith('output') and arg.endswith('File'):
                value=physics.pop(arg)
                msg.debug("Found output file argument %s=%s.", arg, value )
                fmt=arg.lstrip('-').replace('output','').replace('File','')
                trf.outFiles[arg]=getOutputFileName(fmt)

        msg.debug("Checking for not set arguments...")
        for arg,value in listitems(physics):
            if value=="NONE" or value=="none" or value==["NONE"]:
                val=physics.pop(arg)
                msg.debug("Removed %s=%s from arguments.", arg, val )

        trf.physics=physics

        listOfTrfs.append(trf)
        
        

    listOfTrfs[0].inDS=None # not yet implemented
    listOfTrfs[-1].outfmts=formats
    
    return listOfTrfs


'''
directly copied from pyAMI.atlas 5 API
should be removed once suppressNonJobOptions is in official release
'''
def get_ami_tag(client, tag, suppressNonJobOptions = True):
        '''Get AMI-tag information.

        Args:
            :client: the pyAMI client [ pyAMI.client.Client ]
            :tag: the AMI-tag [ str ]

        Returns:
            an array of python dictionnaries.
        '''

        command = [
                'AMIGetAMITagInfo',
                '-amiTag="%s"' % tag,
        ]

        if suppressNonJobOptions:
            command += ['-suppressNonJobOptions']

        msg.debug(command)

        return client.execute(command, format = 'dict_object').get_rows('amiTagInfo')

def remove_enclosing_quotes(s):
    try:
        if s[0] == s[-1] and s[0] in ('"', "'"):
            s = s[1:-1]
    except Exception:
        pass
    return s

## @brief Get information about a T0 tag from AMI
#  @param tag Tag for which information is requested
#  @returns list of PyJoCbTransforms.trfAMI.TRFConfig instances
def getTrfConfigFromAMI(tag, suppressNonJobOptions = True):
    msg.debug('Using AMI to get info about tag %s', tag)

    try:
#        import pyAMI.atlas.api
        import pyAMI.exception
    except ImportError as e:
        raise TransformAMIException(AMIerrorCode, 'Import of pyAMI modules failed ({0})'.format(e))
        
    try:
        amiclient=getAMIClient()
#        result = pyAMI.atlas.api.get_ami_tag(amiclient, tag)
        result = get_ami_tag(amiclient, tag, suppressNonJobOptions)
    except pyAMI.exception.Error as e:
        msg.warning('An exception occured when connecting to primary AMI: {0}'.format(e))
        msg.debug('Exception: {0}'.format(e))
        if 'please login' in e.message or 'certificate expired' in e.message:
            raise TransformAMIException(AMIerrorCode, 'Getting tag info from AMI failed with credential problem. '
                                        'Please check your AMI account status.')
        if 'Invalid amiTag' in e.message:
            raise TransformAMIException(AMIerrorCode, 'Invalid AMI tag ({0}).'.format(tag))
            
        msg.debug("Error may not be fatal - will try AMI replica catalog")

    try:
        trf = TrfConfig()
        trf.name = result[0]['transformation']
        trf.inputs=result[0].get('inputs', {})
        trf.outputs=result[0].get('outputs', {})
        trf.release = result[0]['SWReleaseCache'].replace('_', ',')

        if 'phconfig' in result[0]:
            trf.physics=deserialiseFromAMIString(result[0]['phconfig'])
        else:
            physics = {}
            for k, v in iteritems(result[0]):
                if 'Exec' in k:
                    execStrList = [execStr for execStr in convertToStr(v).replace('" "', '"" ""').split('" "')]
                    physics[convertToStr(k)] = [remove_enclosing_quotes(execStr).replace('\\"', '"') for execStr in execStrList]
                elif '" "' in v:
                    msg.info('found a quoted space (" ") in parameter value for %s, converting to list', k)
                    subStrList = [subStr for subStr in convertToStr(v).replace('" "', '"" ""').split('" "')]
                    physics[convertToStr(k)] = [remove_enclosing_quotes(subStr).replace('\\"', '"') for subStr in subStrList]
                else:
                    physics[convertToStr(k)] = convertToStr(remove_enclosing_quotes(v))

            msg.debug('Result from AMI after string cleaning:')
            msg.debug('%s', dumps(physics, indent = 4))

            if suppressNonJobOptions:
                for k in list(physics):
                    if k in ['productionStep', 'transformation', 'SWReleaseCache']:
                        physics.pop(k)

            for k, v in iteritems(physics):
                if 'triggerConfig' in k or 'triggerConfigByRun' in k:
                    if ' ' in v:
                        physics[k] = v.replace(' ', ',')
                        msg.warning('Attempted to correct illegal trigger configuration string: {0} -> {1}'.format(v, physics[k]))

            msg.debug("Checking for pseudo-argument internal to ProdSys...")
            if 'extraParameter' in physics:
                val = physics.pop('extraParameter')
                msg.debug("Removed extraParamater=%s from arguments.", val)

            msg.debug("Checking for input/output file arguments...")
            for arg in list(physics):
                if arg.lstrip('-').startswith('input') and arg.endswith('File'):
                    value = physics.pop(arg)
                    msg.debug("Found input file argument %s=%s.", arg, value)
                    fmt = arg.lstrip('-').replace('input', '').replace('File', '')
                    trf.inFiles[arg] = getInputFileName(arg)
                elif arg.lstrip('-').startswith('output') and arg.endswith('File'):
                    value = physics.pop(arg)
                    msg.debug("Found output file argument %s=%s.", arg, value)
                    fmt = arg.lstrip('-').replace('output', '').replace('File', '')
                    trf.outFiles[arg] = getOutputFileName(fmt)

            msg.debug("Checking for not set arguments...")
            for arg, value in listitems(physics):
                if value == "NONE" or value == "none" or value == ["NONE"]:
                    val = physics.pop(arg)
                    msg.debug("Removed %s=%s from arguments.", arg, val)

            trf.physics = physics

        if not isinstance(trf.physics, dict):
            raise TransformAMIException(AMIerrorCode, "Bad result for tag's phconfig: {0}".format(trf.physics))

        if trf.inFiles == {}:
            if 'inputs' in result[0]:
                trf.inFiles=deserialiseFromAMIString(result[0]['inputs'])
                for inFileType, inFileName in iteritems(trf.inFiles):
                    # Not all AMI tags actually have a working filename, so fallback to trfDefaultFiles
                    # if necessary
                    if inFileName == '' or inFileName =={} or inFileName == [] or inFileName == '{}':
                        trf.inFiles[inFileType] = getInputFileName(inFileType, tag)

        if 'outputs' in result[0]:
            outputs=deserialiseFromAMIString(result[0]['outputs'])
            trf.outFiles=dict( (k, getOutputFileName(k.lstrip('output').rstrip('File')) ) for k in outputs )
            trf.outfmts=[ outputs[k]['dstype'] for k in outputs ]
    except KeyError as e:
        raise TransformAMIException(AMIerrorCode, "Missing key in AMI data: {0}".format(e))
    except Exception as e:
        raise TransformAMIException(AMIerrorCode, "Got a very unexpected exception while parsing AMI outputs!"
                                    " Please report.\nParsing:\n{0}\nRaised:\n{1}".format(result, e))

    # Now fix up for command line in the case of a new transform:
    if '_tf.py' in trf.name:
        trf.newTransform=True
    else:
        trf.newTransform=False
        
    return [ trf ]


## @brief Convert from a string to a python object
#  @details As we don't know how the string is encoded we
#  try JSON then a safe eval. This is a bit ugly :-(
def deserialiseFromAMIString(amistring):
    try:
        result = json.loads(amistring)
    except ValueError as e_json:
        msg.debug("Failed to decode {0} as JSON: {1}".format(amistring, e_json))
        try:
            result = ast.literal_eval(amistring)
        except SyntaxError:
            errMsg = "Failed to deserialise AMI string '{0}' using JSON or eval".format(amistring)
            msg.error(errMsg)
            raise TransformAMIException(AMIerrorCode, errMsg)
    return result
