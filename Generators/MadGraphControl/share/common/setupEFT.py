import os, re, subprocess, shutil
from MadGraphControl.MadGraphUtils import modify_param_card

mgmodels='/cvmfs/atlas.cern.ch/repo/sw/Generators/madgraph/models/latest/'

def parse_SMEFT_JO(physics_short,processes):    

    # check if reweighting required
    reweight_name=None
    for s in physics_short.split('_'):
        if 'rwgt' in s:
            reweight_name=s.replace('rwgt','')
            physics_short=physics_short.replace('_'+s,'')
            break

    # check if custom param card needed
    param_name=None
    for s in physics_short.split('_'):
        if 'param' in s:
            param_name=s.replace('param','')
            physics_short=physics_short.replace('_'+s,'')
            break
        
    if reweight_name!=None and param_name!=None:
        raise RuntimeError("Can only do either reweighting or use a specific param card")
        
    # find list of non-zero parameters (if not doing reweighting or getting parameters from param card)
    eft_params=None
    if reweight_name==None and param_name==None:
        params_re=r"_(?P<param>[A-Za-z0-9]+)_(?P<value>[-+]?\d+p?\d*)"
        eft_params_list=re.findall(params_re,physics_short)
        eft_params={}
        for e in eft_params_list:
            eft_params[e[0]]=e[1].replace('p','.')
        physics_short=re.sub(params_re,"",physics_short)

    # the last three blocks in name, except for reweight and eft parameters
    # TODO: update to be more robust and allow e.g. for filters in the name
    jobconfig_tokens=physics_short.split('_')
    print jobconfig_tokens
    model_token=jobconfig_tokens[-3]
    process_token=jobconfig_tokens[-2]
    eft_order_token=jobconfig_tokens[-1]
    
    # determine process (and EFT order)
    eft_order=eft_order_token.replace('SQ','^2').replace('lq','<=').replace('eq','==')
    # add space between number and letter
    for x in re.findall(r"\d[A-Za-z]",eft_order):
        eft_order.replace(x,x[0]+' '+x[1])

    if not process_token in processes:
        raise RuntimeError("Unkown process "+process_token)
    elif not "EFTORDER" in processes[process_token]:
        raise RuntimeError("No EFTORDER in process")
    else:
        process=processes[process_token].replace("EFTORDER",eft_order)

    return model_token,process,param_name,reweight_name,eft_params

# build proc, param, and reweight card from 4 last blocks of job config name and 'processes' dict
def setup_SMEFT_from_JOname(processes):
    physics_short=get_physics_short()
    model_nick,process,param_name,reweight_name,eft_params=parse_SMEFT_JO(physics_short,processes)
    return setup_SMEFT(process,model_nick,param_name,reweight_name,eft_params)
    
def setup_SMEFT(process,model_nick,param_name,reweight_name,eft_params):
  
    if model_nick=="EFTAaUm":
        model="SMEFTsim_A_U35_alphaScheme_UFO"
        eft_parameter_block='frblock'
        restriction=None
        sm_restriction='SMlimit'
    elif model_nick=="EFTAWUm":
        model="SMEFTsim_A_U35_MwScheme_UFO"
        eft_parameter_block='frblock'
        restriction=None
        sm_restriction='SMlimit'
    elif model_nick=="EFTBaUm":
        model="SMEFT_alpha_FLU_UFO"
        eft_parameter_block='NEWCOUP'
        restriction=None
        sm_restriction='SM'
    elif model_nick=="EFTBWUm":
        model="SMEFT_mW_FLU_UFO"
        eft_parameter_block='NEWCOUP'
        restriction=None
        sm_restriction='SM'
    elif model_nick=="EFTAaU":
        model="SMEFTsim_A_U35_alphaScheme_UFO"
        eft_parameter_block='frblock'
        restriction='massless'
        sm_restriction='SMlimit_massless'
    elif model_nick=="EFTAWU":
        model="SMEFTsim_A_U35_MwScheme_UFO"
        eft_parameter_block='frblock'
        restriction='massless'
        sm_restriction='SMlimit_massless'

    else:
        raise RuntimeError("Unkown model: "+model_nick)

    # get reweight card and param card for reweighting
    reweight_card_loc=None
    if reweight_name!=None: 
        param_card_loc='MadGraph_param_card_'+model_nick+'_reweight'+reweight_name+'.dat'
        restrict_card_loc='MadGraph_restrict_card_'+model_nick+'_reweight'+reweight_name+'.dat'
        reweight_card_loc='MadGraph_reweight_card_'+model_nick+'_'+reweight_name+'.dat'
        get_param_file = subprocess.Popen(['get_files','-data', param_card_loc])
        get_param_file.wait()
        if not os.path.exists(param_card_loc):
            raise RuntimeError("Cannot find "+param_card_loc)
        get_restrict_file = subprocess.Popen(['get_files','-data', restrict_card_loc])
        get_restrict_file.wait()
        if not os.path.exists(restrict_card_loc):
            print 'running without user defined restriction card'
            restrict_card_loc=None
        get_reweight_file = subprocess.Popen(['get_files','-data', reweight_card_loc])
        get_reweight_file.wait()
        if not os.path.exists(reweight_card_loc):
            raise RuntimeError("Cannot find "+reweight_card_loc)
        eft_params=None

    # get param card and set non-zero parameters
    elif param_name!=None:
        param_card_loc='MadGraph_param_card_'+model_nick+'_'+param_name+'.dat'
        restrict_card_loc='MadGraph_restrict_card_'+model_nick+'_'+param_name+'.dat'
        get_param_file = subprocess.Popen(['get_files','-data', param_card_loc])
        get_param_file.wait()
        if not os.path.exists(param_card_loc):
            raise RuntimeError("Cannot find "+param_card_loc)
        get_restrict_file = subprocess.Popen(['get_files','-data', restrict_card_loc])
        get_restrict_file.wait()
        if not os.path.exists(restrict_card_loc):
            print 'running without user defined restriction card'
            restrict_card_loc=None

    # set only one or a few parameter non-zero
    # dynamically create restricted model that only contains the relevant operators
    else:       
        param_card_default='MadGraph_param_card_'+model_nick+'.dat'
        param_card_loc=param_card_default.replace('.dat','_updated.dat')
        modify_param_card(param_card_default,output_location=param_card_loc,params={eft_parameter_block:eft_params})
        if os.path.exists('mgmodels_local'):
            shutil.rmtree('mgmodels_local')
        os.mkdir('mgmodels_local')
        restricted_model='mgmodels_local/'+model
        shutil.copytree(mgmodels+model,restricted_model)
        eft_params_to_keep=eft_params
        for p in eft_params_to_keep:
            eft_params_to_keep[p]='9.999999e-01'
        if len(eft_params_to_keep)==0:
            process=process.replace('NP==0','')
        restriction='without_irrelevant_couplings'
        if len(eft_params_to_keep)>0:  
            modify_param_card(param_card_default,output_location=restricted_model+'/'+'restrict_'+restriction+'.dat',params={eft_parameter_block:eft_params_to_keep})
        else:
            shutil.copy(param_card_default,restricted_model+'/'+'restrict_'+restriction+'.dat')
        model='./'+restricted_model
        restrict_card_loc=None

    if restrict_card_loc!=None and (reweight_name!=None or param_name!=None):
        if os.path.exists('mgmodels_local'):
            shutil.rmtree('mgmodels_local')
        os.mkdir('mgmodels_local')
        restricted_model='mgmodels_local/'+model
        shutil.copytree(mgmodels+model,restricted_model)
        restriction='without_irrelevant_couplings'
        shutil.copy(restrict_card_loc,restricted_model+'/'+'restrict_'+restriction+'.dat')
        model='./'+restricted_model

    # write process card
    proc_card="import model "+model
    if restriction!=None:
        proc_card+='-'+restriction
    proc_card+="\n"
    proc_card+=process+"\noutput -f\n"
    process_dir=new_process(proc_card)

    if param_card_loc!=None:
        shutil.move(param_card_loc,process_dir+'/Cards/param_card.dat')
    if reweight_card_loc!=None:
        shutil.move(reweight_card_loc,process_dir+'/Cards/reweight_card.dat')

    return process_dir



