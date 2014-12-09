# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @package PyJobTransforms.trfAMI
#  @brief Utilities for configuration of transforms via AMI tags
#  @author atlas-comp-transforms-dev@cern.ch
#  @version $Id$



import ast
import os
import traceback

import logging
msg = logging.getLogger(__name__)

from PyJobTransforms.trfExceptions import TransformAMIException
from PyJobTransforms.trfDefaultFiles import getInputFileName, getOutputFileName

from PyJobTransforms.trfExitCodes import trfExit
errCode=trfExit.nameToCode('TRF_AMI_ERROR')


## @brief Stores the configuration of a transform 
class TrfConfig:
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
        for (k,v) in theDict.iteritems():
            yield k,v

    def __str__(self):
        string = 'asetup '+self.release+'\n'+self.name
        string += self._str_to_dict(self.physics) +'\n'
        
        string +='\nInput file arguments:\n'
        if self.inFiles:
            string += self._str_to_dict(self.inFiles) +'\n'
        if self.inDS:    
            string +='\nExample input dataset: '+ self.inDS + '\n'

        string +='\nOutput file arguments:\n'
        if self.outFiles:
            string += self._str_to_dict(self.outFiles) + '\n'

        string +='\nAMI outputs:\n'
        if self.outputs != {}:
            string += self.outputs + '\n'
        
        if self.outfmts:    
            string += '\nPossible output data types: '+ str(self.outfmts) + '\n'
        return string

    def _str_to_dict(self,adict):
        string=''
        for (k,v) in adict.iteritems():
            if self.newTransform:
                # Some keys already have '--' prefixied
                if k.startswith('--'):
                    string +=" "+k+" '"+v.replace("'", "\\'")+"'"
                else:
                    string +=" --"+k+" '"+v.replace("'", "\\'")+"'"
            else:
                string +=" "+k+"='"+v.replace("'", "\\'")+"'"
        return string

## @brief Stores the information about a given tag.
class TagInfo:
    def __init__(self,tag):
        self._tag=tag
        self._isProdSys=None
        self._trfs=None

    @property    
    def isProdSys(self):
        if self._isProdSys is None:
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
                self._trfs=getTrfConfigFromAMI(self._tag)
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


## @brief Get AMI client
#  @param useReplica If @c True CERN replica is used instead of primary AMI.
#  @returns pyAMI.client.AMIClient instance
def getAMIClient(useReplica=False):
    msg.debug('Getting AMI client...')
        
    try:
        from pyAMI.client import AMIClient
        from pyAMI import endpoint
        from pyAMI.endpoint import get_endpoint, get_XSL_URL
    except ImportError:
        raise TransformAMIException(errCode, 'Import of pyAMI modules failed.')
        
    if useReplica:
        endpoint.TYPE = 'replica'
    else:
        endpoint.TYPE = 'main'
    msg.debug('Using endpoint %s ' % get_endpoint())    
    msg.debug('Using xsl_url %s ' % get_XSL_URL())

    amiclient = AMIClient()
    return amiclient

## @brief Get list of characters of ProdSys tags
#  @returns list of characters
def getProdSysTagsCharacters():

    msg.debug('Getting list of ProdSys tag characters...')

    defaultList=['y', 'p', 'e', 's', 'd', 'r', 't', 'a', 'b', 'w']
        
    argv=["SearchQuery"]
    argv.append("-sql=select productionStep.productionStepTag FROM productionStep WHERE ( ( productionStep.writeStatus LIKE 'valid%') AND productionStep.actor = 'TR')")
    argv.append("project=Atlas_Production")
    argv.append("processingStep=Atlas_Production")

    try:
        from pyAMI.exceptions import AMI_Error
    except ImportError:
        msg.warning('Import of pyAMI modules failed (is your release setup correctly?).')
        msg.warning('Returning default list of ProdSys tags.')
        return defaultList

    try:
        amiclient=getAMIClient(False)
        result=amiclient.execute(argv)
    except (AMI_Error, TransformAMIException):
        msg.debug('An exception occured: %s' % traceback.format_exc())
        msg.warning('Getting ProdSysTags from primary AMI failed. Trying CERN replica.')
        
        try:
            amiclient=getAMIClient(True)
            result=amiclient.execute(argv)
        except (AMI_Error, TransformAMIException):
            msg.debug('An exception occured: %s' % traceback.format_exc())
            msg.warning('Getting ProdSysTags from CERN replica failed (do you have the necessary credentials to access AMI?).')
            msg.warning('Returning default list of ProdSysTags.')
            return defaultList
            
    return [ row['productionStepTag'] for row in result.rows() ]


## @brief Get PANDA client
#  @returns cx_Oracle cursor instance
def getPANDAClient():
    msg.debug('Getting PANDA client...')
    try:
        import cx_Oracle
    except ImportError:
        raise TransformAMIException(errCode, 'Import of cx_Oracle failed (is Oracle setup on this machine?).')
        
    try:
        cur = cx_Oracle.connect('atlas_grisli_r/panda_c10@adcr_panda').cursor()
    except: 
        msg.debug('An exception occurred while connecting to PANDA database: %s' % traceback.format_exc())
        raise TransformAMIException(errCode, 'Failed to get PANDA client connection (N.B. this does not work from outside CERN).')
        
    return cur

## @brief Un-escape information from PANDA
#  @detail Provided by Pavel.
def ReadablePANDA(s):
    return s.replace('%0B',' ').replace('%9B','; ').replace('%8B','"').replace('%3B',';').replace('%2C',',').replace('%2B','+')


## @brief Get information about a ProdSys tag from PANDA
#  @param tag Tag for which information is requested
#  @returns list of PyJoCbTransforms.trfAMI.TRFConfig instances
def getTrfConfigFromPANDA(tag):
    
    msg.debug('Using PANDA to get info about tag %s' % tag)
            
    try:
        pandaclient=getPANDAClient()
        pandaclient.execute("select trf,trfv,lparams,vparams,formats,cache from t_trf_config where tag='%s' and cid=%d" %(tag[:1],int(tag[1:]) ) )
        result=pandaclient.fetchone()
    except:
        msg.info('An exception occurred: %s' % traceback.format_exc())
        raise TransformAMIException(errCode, 'Getting tag info from PANDA failed.')

    if result is None:
        raise TransformAMIException(errCode, 'Tag %s not found in PANDA database' % tag)
    
    msg.debug('Raw data returned from panda DB is:' + os.linesep + str(result))
    
    trfn=result[0].split(',')
    msg.debug('List of transforms: %s' % trfn)
    trfv=result[1].split(',')
    msg.debug('List of releases: %s' % trfv)
    lparams=result[2].split(';')
    msg.debug('List of arguments: %s' % lparams)
    vparams=result[3].split(';')
    msg.debug('List of argument values: %s' % vparams)
    formats=result[4].split('.')
    msg.debug('List of formats: %s' % formats)
    cache=result[5].split(',')
    msg.debug('List of caches: %s' % formats)


    if not ( len(trfn) == len(trfv) == len(lparams) == len(vparams) ):
        raise TransformAMIException(errCode, 'Inconsistency in number of trfs.')
    
    # Cache can be a single value, in which case it needs replicated for other
    # transform steps, or it can be multivalued - great schema design guys :-(
    if len(cache) != len(trfv):
        if len(cache) == 1:
            cache = cache * len(trfv)
        else:
            raise TransformAMIException(errCode, 'Inconsistency in number of caches entries vs. release numbers ({0}; {1}).'.format(cache, trfv))

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
            raise TransformAMIException(errCode, 'Inconsistency in number of arguments.')

        physics = dict( (k, ReadablePANDA(v) ) for (k,v) in zip(keys, values))
        # Hack to correct trigger keys being stored with spaces in panda  
        for k, v in physics.iteritems():
            if 'triggerConfig' in k or 'triggerConfigByRun' in k:
                if ' ' in v:
                    physics[k] = v.replace(' ', ',')
                    msg.warning('Attempted to correct illegal trigger configuration string: {0} -> {1}'.format(v, physics[k]))

        msg.debug("Checking for pseudo-argument internal to ProdSys...")
        if 'extraParameter' in physics:
            val=physics.pop('extraParameter')
            msg.debug("Removed extraParamater=%s from arguments." % val)

        msg.debug("Checking for input/output file arguments...")
        for arg in physics.keys():
            if arg.lstrip('-').startswith('input') and arg.endswith('File'):
                value=physics.pop(arg)
                msg.debug("Found input file argument %s=%s." % (arg,value) ) 
                fmt=arg.lstrip('-').replace('input','').replace('File','')
                trf.inFiles[arg]=getInputFileName(arg)
            elif arg.lstrip('-').startswith('output') and arg.endswith('File'):
                value=physics.pop(arg)
                msg.debug("Found output file argument %s=%s." % (arg,value) )
                fmt=arg.lstrip('-').replace('output','').replace('File','')
                trf.outFiles[arg]=getOutputFileName(fmt)

        msg.debug("Checking for not set arguments...")
        for arg,value in physics.items():
            if value=="NONE" or value=="none":
                val=physics.pop(arg)
                msg.debug("Removed %s=%s from arguments." % (arg, val) )

        trf.physics=physics

        listOfTrfs.append(trf)
        
        

    listOfTrfs[0].inDS=None # not yet implemented
    listOfTrfs[-1].outfmts=formats
    
    return listOfTrfs


## @brief Get information about a T0 tag from AMI
#  @param tag Tag for which information is requested
#  @returns list of PyJoCbTransforms.trfAMI.TRFConfig instances
def getTrfConfigFromAMI(tag):

    msg.debug('Using AMI to get info about tag %s' % tag)
    
    try:
        from pyAMI.exceptions import AMI_Error
        from pyAMI.query import get_configtags
    except ImportError:
        raise TransformAMIException(errCode, 'Import of pyAMI modules failed.')
        
    try:
        amiclient=getAMIClient(False)
        result=get_configtags(amiclient, tag)
    except (AMI_Error, TransformAMIException) as e:
        
        if 'Invalid configTag found' in e.args[0]:
            raise TransformAMIException(errCode, 'Tag %s not found in AMI database.' % tag)
            
        msg.debug('An exception occured: %s' % traceback.format_exc())
        msg.warning('Getting tag info from primary AMI failed. Trying CERN replica.')

        try:
            amiclient=getAMIClient(True)
            result=get_configtags(amiclient, tag)
        except (AMI_Error, TransformAMIException):
            msg.debug('An exception occured: %s' % traceback.format_exc())
            raise TransformAMIException(errCode, 'Getting tag info from AMI failed.')

    msg.debug('Raw result from AMI is: %s ' % result)

    if ( result[0]!={'amiTag': tag } and result[0]!={'configTag': tag }):
        msg.warning('Got unexpected result from AMI: %s when asking for tag %s' % (result[0],tag))
        raise TransformAMIException(errCode, 'Getting tag info from AMI failed.')

#     import pprint
#     pprint.pprint(result)

    trf = TrfConfig()
    trf.name=result[1]['transformation']
    trf.outputs=result[1].get('outputs', {})
    trf.release=result[1]['SWReleaseCache'].replace('AtlasProduction-','')
    trf.physics=dict( (k, str(v)) for (k,v) in ast.literal_eval(result[1]['phconfig']).iteritems() )
    trf.inFiles=dict( (k, getInputFileName(k)) for k in ast.literal_eval(result[1]['inputs']).iterkeys() )
    outputs=ast.literal_eval(result[1]['outputs'])                  
    trf.outFiles=dict( (k, getOutputFileName(outputs[k]['dstype']) ) for k in outputs.iterkeys() )
    trf.outfmts=[ outputs[k]['dstype'] for k in outputs.iterkeys() ]

    # Now fix up for command line in the case of a new transform:
    if '_tf.py' in trf.name:
        trf.newTransform=True
    else:
        trf.newTransform=False
        
    return [ trf ]

    

    
