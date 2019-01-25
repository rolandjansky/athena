# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#=======================================================================
# File: JobProperties/python/JobProperties.py
#=======================================================================
""" JobProperties.  

    prototype for a unified ATLAS job properties definition.

    Here we will add more inforamtion about the usage............
    .............................................................
    .............................................................
    .............................................................
    
"""
#
#
__author__=  'M. Gallas, P. Calafiura, S. Binet'  
__version__= '$Revision: 1.18 $'
__doc__=     "JobProperties"
# data 
__all__ = [ "JobProperties"]
#=======================================================================
# imports
#=======================================================================
import re, os, pickle, pprint, types
import Logging

#=======================================================================
def _isCompatible( allowedTypes, value ):
 # this function is similar to PropertyProxy._isCompatible, but with
 # some twists to handle the peculiarity of JobProperty's implementation

    offcount = 0
    for a in allowedTypes:
        try:
            tp = eval( a )
        except Exception:
            if type(value).__name__ in allowedTypes:
                return True                   # old style, for e.g. function
            offcount += 1
            continue

        if ( tp == str and len(allowedTypes) == 1 ) and not isinstance( value, tp ):
         # special case, insist on exact match for str (no conversions allowed)
            return False
        elif type(value) == str and not isinstance( value, tp ):
         # similarly, insist on exact match for str (no conversions allowed)
            pass # offcount will cause a failure, unless another type matches
        elif ( tp == int or tp == long ) and type(value) == float:
         # special case, insist on strict match for integer types
            pass # id. as for strings above
        elif ( tp == bool ) and not (type(value) in [bool, int]):
         # restrictions on bool, as the code below does "== False", so even though
         # a lot of types convert to bool, only bool and int (and float) behave as
         # expected on comparison
            pass # id. as for strings above
        else:
         # all other types: accept if conversion allowed
            try:
                tp( value )        # conversion needed to check allowedValues
                return True
            except Exception:
                pass

 # accept if no types, or if there are uncheckable types left
    return not (len(allowedTypes) - offcount)

#=======================================================================
# classes
#=======================================================================
class _JobPropertyMeta(type):
    def __new__( self, name, bases, dct ):
     # StoredValue type check
        try:
            sv = dct[ 'StoredValue' ]
            if sv is not None and not _isCompatible( dct[ 'allowedTypes' ], sv ):
                raise TypeError(
                    'type of StoredValue (%s) not in allowedTypes (%s)' % 
                    (type(sv).__name__,dct[ 'allowedTypes' ])
                    )
        except KeyError:
            pass

     # StoredValue value check
        try:
            sv = dct[ 'StoredValue' ]
            if sv is not None and dct[ 'allowedValues' ] and sv not in dct[ 'allowedValues' ]:
                raise TypeError(
                    'value of StoredValue (%s) not in allowedValues (%s)' % 
                    (str(sv),dct[ 'allowedValues' ])
                    )
        except KeyError:
            pass

        return type.__new__( self, name, bases, dct )


class JobProperty(object):
    """ Base class for the job properties.  
        
           The job properties are class definitions that will be 
        instanciated at the time the job property is added to global 
        container for the job properties called  "jobproperties". 
        The job properties can be also added to sub-containers within 
        the "jobproperties" container.        
      
  
        All the job properties must be defined as subclasses of 
        "JobProperty" and, as derived classes, can re-define: 

        statusOn, allowedTypes and allowedValues and StoredValue   

        The setting of the StoredValue will automatically check the new 
        value against the lists allowedTypes and allowedValues. A given 
        StoredValue can not be changed if the corresponding job property
        is locked. 
       
        The actual Value of the JobProperty is (statusOn AND StoredValue)

    """
    __metaclass__ = _JobPropertyMeta

    statusOn=False
    allowedTypes=list()
    allowedValues=list()
    StoredValue=None
    _log=Logging.logging.getLogger('JobProperty ::')
    _nInstancesContextDict=dict()
    _locked=False 

    def __init__(self,context=''):
        """ Each JobProperty has only one possible instance in a given
            context. The context is given by the container and sub-containers
            to which the job property belongs. 
        """
        context_name=context+'.'+self.__class__.__name__
        if not (context_name in self.__class__._nInstancesContextDict.keys()):
            self.__class__._nInstancesContextDict[context_name]=self
            self.__name__=self.__class__.__name__
            self._context_name=context+'.'+self.__class__.__name__
        else:
            self._log.error("There is already an instance of %s at %s ",
                            self.__class__.__name__, context_name)
            raise RuntimeError('JobProperties: JobProperty:: __init__()')

    def __nonzero__(self):
        return self.get_Value()
    
    def __eq__(self, rhs):
        if isinstance(rhs, JobProperty):
            # FIXME: should we only allow comparison between same class
            #        rather than b/w JobProperties ?
            #        OTOH, JobProperty-derived classes are 'singleton'...
            return self() == rhs()
        return self() == rhs

    def __cmp__(self, rhs):
        if isinstance (rhs, JobProperty):
            return cmp (self(), rhs())
        return cmp (self(), rhs)
    
    def __call__(self):
        return self.get_Value()  

    def __str__(self): 
        return self.print_JobProperty('print_v')  

    def __iadd__(self,value):
        aux=self.get_Value()
        aux+=value
        self.StoredValue = aux
        return self.StoredValue
        

    def is_locked(self):
        return self._locked
    
    def _do_action(self):
        """ A place-holder for actions to be taken at the time 
          the JobProperty.StoredValue is set or JobProperty.statusOn 
          is set to True. 

          (for advanced use)
        """
        pass 

    def _undo_action(self):
        """ A place-holder for actions to be taken at the time 
          the JobProperty.statusOn is set to False. 

          (for advanced use)
        """
        pass 

    def set_On(self):
        """ Sets statusOn equals to True.
        """
        if not(self._locked):
            self.__dict__['statusOn']=True
            self._do_action()
        else:
            self._log.info('The JobProperty %s is blocked' % self.__name__)

    def set_Off(self):
        """ Sets statusOn equals to False. 
        """
        if not(self._locked):
            self.__dict__['statusOn']=False
            self._undo_action()
        else:
            self._log.info('The JobProperty %s is blocked' % self.__name__)

    def lock(self):
        """ lock the property
        """
        self._locked=True

    def unlock(self):
        """ unlock the property
        """
        self._locked=False
        self._log.warning('The property %s is being unlocked ',
                          self._context_name)

            
#   def __getattribute__(self, name):
#       obj_p=object.__getattribute__(self, name) 
#       if  name=='StoredValue': 
#           self._log.error('You must access the value of the jobproperty'+
#                          ' using the get_Value() method. ')  
#       return obj_p

    def __setattr__(self, name, n_value):
        if hasattr(self, name):
            if name=='StoredValue' and not(self._locked):
                if len(self.allowedValues) == 0 or \
                      True in [ x == n_value for x in self.allowedValues ]:
                  # WLAV: the following is rather puzzling in the difference
                  # of treatment of bools, especially since it does not work
                  # as expected, and even worse for compatible types. It is
                  # left as-is in place, since changing it has side-effects on
                  # RecExCommon/myTopOptions.py ...
                    if len(self.allowedTypes) == 0:
                        self.__dict__[name] = n_value
                        self.set_On()
                    elif _isCompatible(self.allowedTypes, n_value):
                        self.__dict__[name] = n_value
                        if isinstance(n_value, bool) and n_value is False:
                            self.set_Off() 
                        else:
                            self.set_On()
                    else:
                       raise ValueError(
                           ' %s is not the expected type for: %s' %
                           (n_value, self._context_name)
                           )
                else:
                    raise ValueError(
                        ' %s is not the expected type and/or the value is not allowed for: %s' %
                        (n_value, self._context_name)
                        )
            elif name == 'StoredValue' and self._locked:
                self._log.info('The JobProperty %s is blocked' % self.__name__)
            else: 
                self.__dict__[name] = n_value
        elif name == '__name__' or name == '_context_name':
            self.__dict__[name] = n_value
        else:
            raise AttributeError("JobProperty:: You cannot add attributes to %s" % self)

    def set_Value(self, n_value):
        """ Sets the value of the JobProperty . 
           
                  MyJobProperty.set_Value(NewValue)

            is equivalent to
               
                  MyJobProperty=NewValue 
     
              It is checked the type and if the value is allowed. The 
            statusOn is set to True automatically.
               This method is here for backwards compatibility. 
        """
#Joerg S/Martin W advice
        self.StoredValue=n_value

    def get_Value(self):
        """ Gets the value of the job property.

            This is the recommended way top retrieve the actual
           value of the job property. For boolean properties it
           gives the and(statusOn,StoredValue). For the rest of the
           properties it gives the StoredValue if the statusOn==True
           otherwise it gives None. 
        """
        obj_p=object.__getattribute__(self, 'StoredValue')
        if isinstance(obj_p, types.BooleanType):
            return  obj_p  & self.statusOn 
        else: 
            if self.statusOn: 
                return obj_p 
            else: 
                self._log.warning('You are attempting to access the job'+ 
                'property %s which has not been set', self._context_name) 
                return None   

    def set_Value_and_Lock(self,n_value):
        """ Sets the value of the JobProperty and lock it in one command . 
        """
        self.set_Value(n_value)
        self.lock()

    def isDefault (self):
        """check whether a flag has been modified or if it is still containing
        its default value.
        """
        val = self.__getattribute__('StoredValue')
        dft = self.__class__.StoredValue
        return val==dft


        
    def help(self):
        """ Prints the documentation available for the JobProperty. 
            together with the actual value, values allowed and types
            allowed.   
        """
        self._log.info('### Help for the class %s ###',
                        self.__class__)
        self._log.info(self.__doc__)
    
        print '##  job property:',self._context_name, self.statusOn, self.StoredValue
        print '## allowed Values:',self.allowedValues
        print '## default Value:', self.__class__.StoredValue         
        print '## allowed Types :',self.allowedTypes 
        self._log.info('### End of the help for the class %s ###',
                        self.__class__)

    def toBePrinted (self):
        """ compute a string which is : empty if flag is set to default value
                                        (<DefaultValue> if flag not set to default value or flas is off  
        """
        if self.statusOn: 
            derived = self.get_Value() # can be overwritten
        else:
            derived = None
        stored = self.__getattribute__('StoredValue')
        default = self.__class__.StoredValue
        if derived==stored:
            tbp = '%s' % pprint.pformat(stored)
        else:
            tbp= '%s (Stored: %s)' %(pprint.pformat(derived),pprint.pformat(stored))
        if stored!=default:
            tbp= '%s (Default: %s)' %(tbp,pprint.pformat(default))

        if not self.statusOn:
            tbp = 'Off : %s ' % tbp

        return tbp


    def print_JobProperty(self,mode='minimal'):
        """ Prints the information of the JobProperty  

            The available options are: 'minimal','full','tree','tree&value','tree&valuenondefault'
        """
        indent=''
        obj_p=object.__getattribute__(self, 'StoredValue')
        if self.statusOn: 
            obj_ps=obj_p 
        else: 
            obj_ps=None   
        for i in range(self._context_name.count('.')-2):
            indent+='-'
        if self.is_locked():
            indent+='x'
        else:
            indent+='-'
            
        if mode=='minimal':  
                self._log.info(" %s-> %s = %s ",indent,
                           self._context_name,pprint.pformat(obj_ps) )
            
        elif(mode=='full'):
            if len(self.allowedTypes)>0:
                 at=self.allowedTypes
            else:
                 at='- - -'
            if len(self.allowedValues)>0:
                 av=pprint.pformat(self.allowedValues)
            else:
                 av='- - -' 
            self._log.info("%s-> %s = %s\n %40s %s\n %40s \
                            %s\n %40s %s\n %40s %s\n %40s %s\n %40s %s",
                           indent,
                           self._context_name,
                           self.__getattribute__('StoredValue'),
                           'allowedTypes :',at,
                           'allowedValues :',av,
                           'default value :', self.__class__.StoredValue,
                           'statusOn :',self.statusOn,
                           'locked :',self.is_locked(),
                           'StoredValue :',pprint.pformat(obj_p))
        elif(mode=='tree'):
            print '     |'+indent+' '+self.__name__
        elif(mode.startswith('tree&value')):    
            if mode=='tree&value':
                printit=True
            elif mode=='tree&valuenondefault':    
                if self.isDefault():
                    printit=False
                else:
                    printit=True
            else:
                raise RuntimeError("This is a non valid print mode %s " % (mode,))
            if printit:        
                print '     |'+indent+' '+self.__name__+" = "+\
                    self.toBePrinted()
#                   fnToBePrinted(self)

        elif(mode=='print_v'):
            return '     |'+indent+' '+self.__name__+" = "+\
                   self.toBePrinted()
        # fnToBePrinted(self)

        else:
            raise ValueError('Unknow mode, possible modes are: '
                             'minimal, full, tree, tree&value ')
                                
##  

class JobPropertyContainer (object):
    """ Container for the JobProperties. 

         By definition it will contain a minimal set of flags,
       but this set can be increased during the job configuration
       using the "import_JobProperties" or "add_JobProperty" 
       methods. In this way it can be adapted to the diferent 
       job needs.
         A JobPropertyContainer can contain other JobProperty 
       containers, that can be added using the "add_Container" 
       method.
         There is a top global job properties container instance 
       which is called "jobproperties" that has to be used as starting
       point. 
       
    """
    _log=Logging.logging.getLogger('JobPropertyContainer::')
    _nInstancesContextList=list()
    _locked=False 

    def __init__(self,context=''):
        """ Each JobPropertyContainer has only one possible instance
            in a given context.
        """
        if context=='': 
            context_name=self.__class__.__name__
        else: 
            context_name=context+'.'+self.__class__.__name__ 
        if not (context_name in self.__class__._nInstancesContextList):
            self.__class__._nInstancesContextList.append(context_name)
            self.__name__=self.__class__.__name__
            self._context_name=context_name
        else:
            self._log.error('There is already an instance of %s at %s',
                            self.__class__.__name__,  context_name)
            raise RuntimeError('JobProperties:JobPropertyContainer: __init__()')

    def __setattr__(self, name, n_value):
        # Protection for the already defined JobProperties classes 
        # and inmediate asignation of a new StoredValue
        if not(name=='__name__' or name=='_context_name' or name=='_locked'):
          if (issubclass(n_value.__class__,JobProperty) or
              issubclass(n_value.__class__,JobPropertyContainer)): 
              pass 
          elif(self.__dict__.__contains__(name)): 
              pass 
          else:
              errString="JobPropertyContainer:: %s does not have property %s" % (self._context_name, name)
              try:
                  from difflib import get_close_matches
                  closestMatch=get_close_matches(name,self.__dict__.keys(),1)
                  if len(closestMatch)>0:
                      errString+=". Did you mean \'%s\'?" %  closestMatch[0] 
              except Exception:
                  pass #No execption from here
                  
              raise AttributeError(errString)
        try: 
            protected=hasattr(self.__dict__[name],'_context_name') 
        except Exception:
            protected=False 
        if not protected:
            self.__dict__[name] = n_value
        else:
            property_obj=self.__dict__.get(name)
            property_obj.StoredValue=n_value
            property_obj.statusOn=True



    def __str__(self): 
        if self._context_name.count('.')==0:
            # the top container case 
            print_view='    [-]'+self.__name__+'\n'+'     | '+'\n' 
            for k in sorted(self.__dict__.keys()):
                m=self.__dict__.get(k)
                if hasattr(m,'print_JobProperty'):
                    m.print_JobProperty('print_v')
                    print_view+=str(m)+'\n'
                elif hasattr(m,'print_JobProperties'):
                    indent='-'
                    for i in range(m._context_name.count('.')-1):
                        indent+='-'
                    print_view+='     /'+indent+'> ## '+m.__name__+' ## '+'\n' 
                    print_view+=m.print_JobProperties('print_v')
            return print_view         
        else: 
            return self.print_JobProperties('print_v') 

    def is_locked(self):
        return self._locked
    
    def help(self):
        """ Prints the documentation generated with the JobProperty 
            container.  
        """
        self._log.info('### Help for the class %s ###',
                        self.__class__)
        self._log.info(self.__doc__)
        self._log.info('### End of the help for the class %s ###',
                        self.__class__)

    def import_JobProperties(self,module_name):
        """ Import modules with JobProperties specific to a given job 
           configuration.  
           
           The new specific set of JobProperties added must have a name 
           without '_' and subclass the base class "JobProperty".
        """
        try:
            if(module_name.count('.')==0):
                module=__import__(module_name,globals(),locals())
            elif(module_name.count('.')>0):
                module=__import__(module_name,globals(),locals(),\
                              ['JobProperties'])
        except ImportError:
            self._log.error(" import_JobProperties: No module named %s",
                            module_name)
            return None
        mo=vars(module)
        p = re.compile('_') # mgallas  --> look into this and relax it 
        for i in mo.keys():
            if(not(p.match(i)) and 
               (i !='JobPropertyContainer' and i!='JobProperty' and 
                i !='jobproperties')):
                 mok=mo.get(i)
                 setattr(self,mok.__name__,mok(self._context_name))

    def import_Flags(self,module_name):
        """ OBSOLET: Use import_JobProperties

            Import modules with JobProperties specific to a given job 
           configuration.  
           
           The new specific set of JobProperties added must have a name 
           without '_' and subclass the base class "JobProperty". 

           IMPORTANT: "import_Flags" method is calling the new method 
                      "import_JobProperties". For some time import_Flags
                       it will work but it will be removed soon
        """
        self._log.warning('OBSOLETE method: use import_JobProperties') 
        self.import_JobProperties(module_name)

    def add_Container(self,new_container):
        """ Adds a container of JobProperties of the type 
           JobPropertyContainer to the existing container. 
        """
        if issubclass(new_container,JobPropertyContainer):
            issubclass(new_container,JobPropertyContainer) 
            if not(new_container.__name__ in self.__dict__.keys()):
                setattr(self,new_container.__name__,
                        new_container(self._context_name))
            else:
               self._log.warning('The container %s is already in %s',
                                  new_container.__name__,self.__name__)
        else:
           self._log.error('You are not adding a JobPropertyContainer ')

    def add_JobProperty(self, new_flag):
        """ Add one JobProperty to the present container.  
        
           The new JobProperty added must have a name 
           without '_' and subclass the base class "JobProperty".
        """
        if issubclass(new_flag,JobProperty):
            if not(new_flag.__name__ in self.__dict__.keys()):
                setattr(self,new_flag.__name__,new_flag(self._context_name))
            else:
                self._log.warning('The flag %s is already in %s',
                                   new_flag.__name__, self.__name__) 
        else:
           self._log.error('You are not adding a JobProperty ')

    def del_JobProperty(self, del_flag):
        """ Deletes one JobProperty from the present container.
        """
        if (issubclass(del_flag,JobProperty) and   
            hasattr(self,del_flag.__name__)):
            flag_obj=getattr(self,del_flag.__name__)
            delattr(self,del_flag.__name__)
            del_flag._nInstancesContextDict.pop(flag_obj._context_name)
            self._log.debug('The JobProperty %s has been deleted',
                               del_flag.__name__)  
        else:
            self._log.info('The JobProperty %s is not defined here', 
                               del_flag.__name__)

    def del_Container(self, del_container):
        """ Deletes one sub-container of the present container.  
        """
        if (issubclass(del_container,JobPropertyContainer) and  
            hasattr(self,del_container.__name__)):
            flagcontainer_obj=getattr(self,del_container.__name__) 
            delattr(self,del_container.__name__)
            del_container._nInstancesContextList.remove(flagcontainer_obj._context_name)
            self._log.warning('The JobProperty Container %s has been deleted',
                               del_container.__name__)  
        else:
            self._log.info('The JobProperty Container %s is not defined here', 
                               del_container.__name__)
    def lock_JobProperties(self):
        """ Locks the Values of the JobProperties
        """
        for j in self.__dict__.keys():
            j_obj=self.__dict__.get(j)
            if hasattr(j_obj,'lock_JobProperties'):
                j_obj.lock_JobProperties()
                j_obj._locked=True
            elif hasattr(j_obj,'_locked'):
                j_obj._locked=True
        self._log.info('The JobProperty Container %s is locked',
                          self.__name__)     

    def unlock_JobProperties(self):
        """ Unlocks the Values of the JobProperties
        """
        for j in self.__dict__.keys():
            j_obj=self.__dict__.get(j)
            if hasattr(j_obj,'lock_JobProperties'):
                j_obj.unlock_JobProperties()
                j_obj._locked=False
            elif hasattr(j_obj,'_locked'):
                j_obj._locked=False            
        self._log.warning('The JobProperty Container %s is being unlocked ',
                          self.__name__)

    def print_JobProperties(self,mode='minimal'):
        """Prints all the JobProperties in the container. 

            It will print also the JobProperties within the sub-containers
           present in the container. 

            The available options are: 'minimal','full','tree','tree&value','tree&valuenondefault'
        """
        print_view=''
        if self._context_name.count('.')==0:
            additionalinfo=''
            if (mode=='tree&valuenondefault'):
                additionalinfo="(Only non default values)"
            if mode != "minimal":
                additionalinfo+="(X indicates locked properties)"
                
            self._log.info("### Printing the job properties container %s %s ###",
                           self.__name__ ,additionalinfo)

            if(mode=='tree' or mode.startswith('tree&value')):
                print '    [-]'+self.__name__
                print '     | '
            elif(mode=='print_v'):
                print_view+='    [-]'+self.__name__+'\n'+'     | '+'\n' 
            else:
                self._log.info('    [-]'+self.__name__)
       
        for k in sorted(self.__dict__.keys()):
            m=self.__dict__.get(k)
            if hasattr(m,'print_JobProperty'):
                m.print_JobProperty(mode)
                if mode=='print_v': 
                   print_view+=str(m)+'\n'
            elif hasattr(m,'print_JobProperties'):
                indent='-'
                for i in range(m._context_name.count('.')-1):
                    indent+='-'
                if(mode=='tree' or mode.startswith('tree&value')):    
                    print '     /'+indent+'> ## '+m.__name__+' ## '
                elif(mode=='print_v'): 
                    print_view+='     /'+indent+'> ## '+m.__name__+' ## '+'\n' 
                else:
                    self._log.info('     /'+indent+'> ## '+m.__name__+' ## ')

                if(mode=='print_v'):
                    print_view+=m.print_JobProperties(mode)
                else:
                    m.print_JobProperties(mode)
                
        if mode=='print_v':
           return print_view         
        if self._context_name.count('.')==0:
            self._log.info("### Ends the job properties container %s ###",
                           self.__name__ )

    def set_JobProperties(self,data):
        """ It can be used to set several job properties in different
            JobContainers in one step.


            The expected data must be a dict as for example:

             data={'JobProperties.SimFlags':{'init_Level':2,'SeedsG4':.98},
                   'JobProperties.Digitization':{'writeMuonDigit':False}
                  }
                  
        """
        tp=type(data)
        if tp.__name__=='dict':
            list_context=JobProperty._nInstancesContextDict.keys()
            for i in data.keys():
                for j in data[i].keys():
                    if list_context.count(i+'.'+j)==1:
                        jp=JobProperty._nInstancesContextDict[i+'.'+j]
                        jp.set_Value(data[i][j])
                        self._log.info("The JobProperty %s has been set to %s",
                                      i+'.'+j,data[i][j])
                    else:
                        self._log.warning("The JobProperty %s does not exist",
                                          i+'.'+j)
        else:
            raise ValueError('The received data is has not the expected'
                             'type/format') 

    def __getattribute__(self,name):
        try:
            return object.__getattribute__(self, name)   
        except AttributeError:
            
            errString="JobPropertyContainer:: %s does not have property %s" % (object.__getattribute__(self,'_context_name'), name)
            allattrs=object.__getattribute__(self,'__dict__').keys()
            from difflib import get_close_matches
            closestMatch=get_close_matches(name,allattrs,1)
            if len(closestMatch)>0:
                errString+=". Did you mean \'%s\'?" %  closestMatch[0]
            raise AttributeError(errString)
        pass
    

#=======================================================================
# data 
#=======================================================================
class JobProperties(JobPropertyContainer):
    """ The global job properties container. 
         
       - This will be the unique container with 'global' context 
        and the rest of the containers or general job properties
        will be added to this container.   

       - The job properties/containers added to this container do not 
        need to be instances. The instance is created and associated to 
        context at the time the job properties/container is added. 
        User needs only to define the job properties/container he/she 
        want to add.    
    """
jobproperties=JobProperties()

#=======================================================================
# functions  
#=======================================================================
def save_jobproperties(file_name):
    """  Saves the "jobproperties" container of JobProperties.
    """
    try:
        if not (file_name.__contains__(os.sep)) :
            file_name=os.getcwd()+os.sep+file_name
        f=open(file_name,'w') 
#       data=[JobPropertyContainer,JobProperty,jobproperties]
#       pickle.dump(data,f)
        pickle.dump(jobproperties,f)
        f.close()
    except IOError:
        print 'It is not possible to save the jobproperties '
def load_jobproperties(file_name):
    """ Loads the "jobproperties" container of JobProperties. 
    """
    try:
        if not (file_name.__contains__(os.sep)) :
            file_name=os.getcwd()+os.sep+file_name
        f=open(file_name,'r')
        return pickle.load(f)
    except IOError:
        print 'It is not possible to load the jobproperties '

