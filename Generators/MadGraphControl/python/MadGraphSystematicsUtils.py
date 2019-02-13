# utils to set systematics according to base fragment

# use some helper functions from MadGraphUtils
import MadGraphUtils,ast
from MadGraphUtilsHelpers import *

#==================================================================================
# main function: figure out run_card arguments that need to be set
# depends on
# * base fragment
# * NLO vs LO
# * MadGraph version
def get_pdf_and_systematic_settings(the_base_fragment,isNLO):
    ### Set settings according to included base fragment
    runcard_settings={}
    
    basefragment_settings={}
    basefragment_settings['central_pdf']=None
    basefragment_settings['alternative_pdfs']=None
    basefragment_settings['pdf_variations']=None
    basefragment_settings['scale_variations']=None
    for s in basefragment_settings:
        if s in the_base_fragment:
            if isinstance(basefragment_settings[s],list) and len(basefragment_settings[s])==0:
                continue
            basefragment_settings[s]=the_base_fragment[s]
        else:
            MadGraphUtils.mglog.warning('base fragment include does not define "'+s+'"')
            if s=='central_pdf':               
                raise RuntimeError('No "central_pdf" configured in base fragment')   

    ### Check base fragment input
    for s in basefragment_settings:
        if s=='central_pdf':
            if not isinstance(basefragment_settings[s],int):
                raise RuntimeError(s+', configured in base fragment, has to be an integer')
            continue
        if basefragment_settings[s]==None:
            continue
        if s=='scale_variations':
            if not isinstance(basefragment_settings[s],list):
                raise RuntimeError(s+', configured in base fragment, has to be a list of integers')
            for pdf in basefragment_settings[s]:
                if not isinstance(pdf,float) and not isinstance(pdf,int):
                    raise RuntimeError(s+', configured in base fragment, has to be a list of integers')
        else:
            if not isinstance(basefragment_settings[s],list):
                raise RuntimeError(s+', configured in base fragment, has to be a list of integers')
            for pdf in basefragment_settings[s]:
                if not isinstance(pdf,int):
                    raise RuntimeError(s+', configured in base fragment, has to be a list of integers')
    
    ### Resolve smaller issues with base fragment input
    if basefragment_settings['alternative_pdfs']!=None:
        # if a PDF set is included as variation (i.e. nominal + error pdf) there is no need to have it as alternative pdf (i.e. only nominal)
        if basefragment_settings['pdf_variations']!=None:
            basefragment_settings['alternative_pdfs']=[ a for a in basefragment_settings['alternative_pdfs']  if a not in basefragment_settings['pdf_variations'] ]
        # the central pdf does not need to be included as alternative PDF
        if basefragment_settings['central_pdf'] in basefragment_settings['alternative_pdfs']:
            basefragment_settings['alternative_pdfs'].remove('central_pdf')
  
    ### Set central PDF
    runcard_settings['pdlabel']='lhapdf'
    runcard_settings['lhaid']=str(basefragment_settings['central_pdf'])

    # turn on LO systematics and use new systematics script
    if not isNLO:
        runcard_settings['systematics_program']='none'
        for s in ['alternative_pdfs','pdf_variations','scale_variations']:
            if basefragment_settings[s]!=None and len(basefragment_settings[s])>0:
                # set use_syst true if some variations are used
                runcard_settings['use_syst']='True'
                # use the MadGraph systematics program != syscalc
                runcard_settings['systematics_program']='systematics'
                break

    ### Set PDFs to be included as weights                
    if isNLO:
        # pdf weights with NLO syntax
        if basefragment_settings['pdf_variations']!=None and basefragment_settings['central_pdf'] in basefragment_settings['pdf_variations']:
            runcard_settings['reweight_pdf']='True'
        else:
            runcard_settings['reweight_pdf']='False'
        if basefragment_settings['pdf_variations']!=None:
            for v in basefragment_settings['pdf_variations']:
                if v==basefragment_settings['central_pdf']:
                    continue
                runcard_settings['lhaid']+=' '+str(v)
                runcard_settings['reweight_pdf']+=' True'
        if basefragment_settings['alternative_pdfs']!=None:
            for a in basefragment_settings['alternative_pdfs']:
                runcard_settings['lhaid']+=' '+str(a)
                runcard_settings['reweight_pdf']+=' False'
            
    else: #not NLO
        sys_pdfs=[]
        runcard_settings['sys_pdf']=''
        if basefragment_settings['pdf_variations']!=None:
            for v in basefragment_settings['pdf_variations']:
                sys_pdfs.append(MadGraphUtils.get_lhapdf_id_and_name(v)[1])
        if basefragment_settings['alternative_pdfs']!=None:
            for a in basefragment_settings['alternative_pdfs']:
                sys_pdfs.append(MadGraphUtils.get_lhapdf_id_and_name(a)[1]+' 1')        
        runcard_settings['sys_pdf']+=' '.join(sys_pdfs)


    ### Set scale variations to be included as weights
    if isNLO and basefragment_settings['scale_variations']==None:
        runcard_settings['reweight_scale']='False'
    if basefragment_settings['scale_variations']!=None:
        if isNLO:
            runcard_settings['reweight_scale']='True'
            runcard_settings['rw_rscale']=' '.join([str(s) for s in basefragment_settings['scale_variations']])
            runcard_settings['rw_fscale']=' '.join([str(s) for s in basefragment_settings['scale_variations']])
        else:
            runcard_settings['sys_scalefact']=' '.join([str(s) for s in basefragment_settings['scale_variations']])

    ###
    if is_version_or_newer([2,6,2]):
        convertSysCalcArguments(runcard_settings)
    return runcard_settings

#==================================================================================
# this function is called during build_run card to check the consistency of user-provided arguments with the inlude
# and throw errors, warnings, or corrects the input as is appropriate
def setup_pdf_and_systematic_weights(the_base_fragment,extras,isNLO):
    ### options in run cards that affect PDF and systematics weights behavior

    ### set all relevant keys to lowercase and clean them up
    for k in extras:
        k_clean=k.lower().replace("'",'').replace('"','')
        if k_clean!=k and k_clean in systematics_run_card_options(isNLO):
            extras[k_clean]=extras[k]
            extras.pop(k,None)
        
    ### Check compatibility of user setting and base fragment inclusion
    if base_fragment_setup_check(the_base_fragment,extras,isNLO):
        return
    # if something is set that contradicts the base fragment: bad!
    for o in systematics_run_card_options(isNLO):
        if o in extras:
            raise RuntimeError('Error, you tried to set "'+str(o)+'" by hand, but you should trust the base fragment with the following options: '+', '.join(systematics_run_card_options(isNLO)))


    new_settings=get_pdf_and_systematic_settings(the_base_fragment,isNLO)
    ### backup extras (user set parameters for run_card)
    user_set_extras=dict(extras)
    for s in new_settings:
        if s!=None:
            extras[s]=new_settings[s]

    ### Make sure everything has been set
    MadGraphUtils.mglog.info('PDF and scale settings were set as follows:')
    for p in systematics_run_card_options(isNLO):
        user_set='not set'
        if p in user_set_extras:
            user_set=str(extras_user[p])
        new_value='not set'
        if p in extras:
            new_value=str(extras[p])   
        MadGraphUtils.mglog.info('MadGraphUtils set '+str(p)+' to "'+new_value+'", was set to "'+user_set+'"')


#==================================================================================
# check whether a configuration is in agreement with base fragment
# true if nothing needs to be done
# false if still needs setup
# error if inconsistent config
def base_fragment_setup_check(the_base_fragment,extras,isNLO):
    # no include: allow it (with warning), as long as lhapdf is used
    # if not (e.g. because no choice was made and the internal pdf ise used): error
    if the_base_fragment == None:
        MadGraphUtils.mglog.warning('!!! No pdf base fragment was included in your job options. PDFs should be set with an include file. You might be unable to follow the PDF4LHC uncertainty prescription. Let\'s hope you know what you doing !!!')
        if not checkSetting('pdlabel','lhapdf',extras)  or not checkSettingExists('lhaid',extras):
            MadGraphUtils.mglog.warning('!!! No pdf base fragment was included in your job options and you did not specify a LHAPDF yourself -- in the future, this will cause an error !!!')
            #TODO: in the future this should be an error
            #raise RuntimeError('No pdf base fragment was included in your job options and you did not specify a LHAPDF yourself')
        return True
    else:
        # if setting is already exactly as it should be -- great!
        correct_settings=get_pdf_and_systematic_settings(the_base_fragment,isNLO)
        allgood=True
        for s in correct_settings:
            if s==None and s in extras:
                allgood=False
                break
            if s not in extras or extras[s]!=correct_settings[s]:
                allgood=False
                break
        if allgood:
            return True
    # no error but also nothing set
    return False

#==================================================================================
# convert settings from the syscalc syntax to the new systematics syntax which is steered via "systematics_arguments"
# will modify the dict and also return new setting
def convertSysCalcArguments(extras):
    if 'systematics_program' in extras:
        if extras['systematics_program'].lower=='none':
            MadGraphUtils.mglog.warning('no need to convert systematics arguments if systematcs are not run')
            return
        if extras['systematics_program'].lower=='syscalc':
            MadGraphUtils.mglog.warning('systematics already correct for chosen systematics program SysCalc')
            return

    if 'systematics_arguments' in extras:
        MadGraphUtils.mglog.warning('systematics_arguments already defined, will be overwritten')
    systematics_arguments={}
    systematics_arguments['dyn']='-1'
    systematics_arguments['mur']='1'
    systematics_arguments['muf']='1'
        
    if 'sys_scalefact' in extras:
        sys_scalefact=extras['sys_scalefact']
        extras.pop('sys_scalefact')
        if len(sys_scalefact.split())>0:
            systematics_arguments['mur']=','.join(sys_scalefact.split())
            systematics_arguments['muf']=','.join(sys_scalefact.split())

    systematics_arguments['pdf']='central'
    if 'sys_pdf' in extras:
        sys_pdf=extras['sys_pdf']
        extras.pop('sys_pdf')
        spl=sys_pdf.replace('&&',' ').split()
        i=0
        pdfs=[]
        while i <len(spl):
            pdfs.append(spl[i])
            if i+1<len(spl) and spl[i+1].isdigit() and int(spl[i+1])<1000:
                pdfs[-1]+='@'+str(int(spl[i+1])-1)
                i+=1
            i+=1
        if len(pdfs)>0:
            systematics_arguments['pdf']=','.join(pdfs)
    extras['systematics_arguments']='['+','.join(["'--"+k+"="+systematics_arguments[k]+"'" for k in systematics_arguments])+']'
    return extras['systematics_arguments']


#==================================================================================
# create a map from the 'systematics_arguments' run card argument
def parse_systematics_arguments(sys_args):
    parsed={}
    if sys_args.strip().startswith('['):
        argument_list = ast.literal_eval(sys_args)
        for a in argument_list:
            key,value=parse_systematics_argument(a)
            parsed[key]=value
    else:
        arg=sys_args.replace("'",'').replace('"','')
        key,value=parse_systematics_argument(a)
    return parsed
 
def parse_systematics_argument(sys_arg):
    spl=sys_arg.strip().split('=')
    return (spl[0].strip()[2:],spl[1].strip())

#==================================================================================
# these arguments steer systematics
def systematics_run_card_options(isNLO):
    if isNLO:
        return ['pdlabel','lhaid','reweight_pdf','reweight_scale','rw_rscale','rw_fscale' ]
    else:
        return  ['pdlabel','lhaid','use_syst','sys_scalefact','sys_pdf','systematics_arguments']
