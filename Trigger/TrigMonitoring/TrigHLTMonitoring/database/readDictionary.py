# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

include("tools.py")

def readDictionary(*vargs, **kwargs):
    # read data from all dictionaries available
    import sys
    import hashlib
    slice_list = ['TAU','EGAMMA','JET','BJET','BPHYS','MET','MINBIAS','MUON','IDTRK','IDJPSI','CALORAW','CALOESD']
    slice_match_pattern = ['TAU','EGAMMA','JET','BJET','BPHYS','MET','MINBIAS','MUON','T','JPSI','Calo','Calo']
    run_type_list = ['Physics', 'Standby', 'Cosmics']
    dictionaries=[]

    ConfigNameMissing=[]
    ConfigTypeMissing=[]
    ConfigRecostepMissing=[]
    ConfigCodeMissing=[]
    ConfigCodeHashMissing=[]
    ConfigScriptMissing=[]
    ConfigScriptHashMissing=[]
    slice_match_pattern_list=[]

    for sl_index, sl in enumerate(slice_list):
        for rtl in run_type_list:
           dictpath = "dictionaries/"
           my_dictionary_file=dictpath+sl+"_"+rtl+"_"+"dictionary.py"
           #sys.path.insert(0, dictpath)
           sys.path.insert(1, dictpath) # they say it is better to use 1, not 0 ...
           #sys.path.append(dictpath)
           #sys.path.append("dictionaries")
           #import my_dictionary_file
           #sys.path.append(os.path.abspath(my_dictionary_file)) 
           #print "here", my_dictionary_file
           #print "non zero file ", is_non_zero_file(my_dictionary_file)
           my_dictionary=sl+"_"+rtl+"_"+"dictionary"
           tmp = "dictionaries."+my_dictionary 
           my_dict=sl+"_"+rtl+"_"+"dict"
           #print "la la ", my_dictionary                                                                                                           
           #print my_dict                                                                                                                           
           #print ("import %s as %s" % (my_dictionary,my_dict))                                                                                 
           if is_non_zero_file(my_dictionary_file):
             #exec "import %s as %s" % (my_dictionary,my_dict)
             #for mod in sys.modules.values():
             #   reload(mod)
             #imp.reload(my_dictionary)

             # Now we need to delete imported module first because in python interactive mode it does not get updated imported file..
             if my_dictionary in sys.modules:  
                 del(sys.modules[my_dictionary]) 
             exec "import %s as %s" % (my_dictionary,my_dict)

             my_dict=eval(my_dict)
             #print "checking reading dictionary.. ", my_dict 
             #print my_dict.Slice                                                                                                                 
             dictionaries.append(my_dict)
             slice_match_pattern_list.append(slice_match_pattern[sl_index])
           else:
             #print "Dictionary is empty or does not exist! We leave active key DB values for this dictionary"
             print "Dictionary is empty or does not exist! We leave release values for this dictionary and put None to config code and config script of DB entries"
             ConfigNameMissing.append(sl)
             ConfigTypeMissing.append(rtl)
             print "missing dict ", sl, " Type ", rtl
             if sl.count("RAW"):
                ConfigRecostepMissing.append('RAW') 
             else:
                ConfigRecostepMissing.append('ESD') 
             #ConfigRecostepMissing.append('ESD') # put it for now, later have to change!!! 
             config_code_missing=None
             #config_code_missing=0
             str_code_missing=str(config_code_missing)
             ConfigCodeMissing.append(config_code_missing)
             hash_code_missing=hashlib.md5(str_code_missing).hexdigest()  
             ConfigCodeHashMissing.append(hash_code_missing)
             #config_script_missing=0
             config_script_missing=None
             str_script_missing=str(config_script_missing)
             ConfigScriptMissing.append(config_script_missing)
             hash_script_missing=hashlib.md5(str_script_missing).hexdigest() 
             ConfigScriptHashMissing.append(hash_script_missing)
             # put here dics from DB? Or pass them somehow to upload that we know which dics are they
    #print dictionaries


    ConfigName=[]
    ConfigRunType=[]
    ConfigRecostep=[]
    ConfigCode=[]
    ConfigCodeHash=[]
    ConfigScript=[]
    ConfigScriptHash=[]

    #dictionaries = [tau_dict,egamma_dict,jet_dict]

    for ndict, dict in enumerate(dictionaries):
       print "dict ", dict.Slice, " Type ", dict.RunType 
       ConfigRunType.append(dict.RunType)
       ConfigName.append(dict.Slice)
       ConfigRecostep.append(dict.Recostep)
       ConfigScript.append(dict.Script)
       ConfigScriptHash.append(hashlib.md5(dict.Script).hexdigest())
       #print "hash script ", hashlib.md5(dict.Script).hexdigest() 
       d={}
       d["MonitoringTriggers"]=dict.MonitoringTriggers
       d["Config"]=dict.Config
       d["PkgName"]=dict.PkgName
       d["CreatorName"]=dict.CreatorName
       #print "dict d ", d
       #print ConfigName
       #print ConfigRunType
       #print ConfigRecostep
       #print ConfigCode

       if validateDictionary(d,dict.Slice,slice_match_pattern_list[ndict]): # put tool_match_pattern[ndict] as a second argument
         #print "HERE!!!!!!!"
         #print isDictionaryLikeInRelease(d)
         if isDictionaryLikeInRelease(d)==0:
           ConfigCode.append(d)
           d_str = str(d)
           #print "String d: " , d_str
           d_hash = hashlib.md5(d_str).hexdigest()
           #print "hash ", d_hash
           ConfigCodeHash.append(d_hash)
           #return ConfigName,ConfigRecostep,MontoolConfigCode,MontoolConfigScript
         elif isDictionaryLikeInRelease(d)==1:
           # in this case we have all parameters like in release and therefore we put "None" to config in the DB
           none_str = str(None)
           ConfigCode.append(None)
           none_str_hash=hashlib.md5(none_str).hexdigest()
           ConfigCodeHash.append(none_str_hash)
       else:
          print "Check dictionaries!"
          return 0

    # append missing dictionaries to others

    for i, entry in enumerate(ConfigNameMissing):    
      ConfigName.append(entry)
      ConfigRunType.append(ConfigTypeMissing[i])
      ConfigRecostep.append(ConfigRecostepMissing[i])
      ConfigCode.append(ConfigCodeMissing[i])
      ConfigCodeHash.append(ConfigCodeHashMissing[i])
      ConfigScript.append(ConfigScriptMissing[i])
      ConfigScriptHash.append(ConfigScriptHashMissing[i])


    #return ConfigName,ConfigRunType,ConfigRecostep,ConfigCode,ConfigCodeHash,ConfigScript,ConfigScriptHash,ConfigNameMissing,ConfigTypeMissing,ConfigRecostepMissing
    return ConfigName,ConfigRunType,ConfigRecostep,ConfigCode,ConfigCodeHash,ConfigScript,ConfigScriptHash
