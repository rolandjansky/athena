# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
# Compatibility layer allowing to convert between new (as of 2020) Gaudi Configurable2
# and old Configurable classes

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ConfigurationError
from AthenaConfiguration.Deduplication import DeduplicationFailed

import GaudiConfig2
import collections.abc


def __indent( indent = ""):
    return indent + "  "


def __setProperties( destConfigurableInstance, sourceConf2Instance, indent="" ):
    _log = logging.getLogger( "__setProperties" )
    for pname, pvalue in sourceConf2Instance._properties.items():
        if destConfigurableInstance.__class__.__name__ == 'AlgSequence' and pname == 'Members':
            continue
        propType = sourceConf2Instance._descriptors[pname].cpp_type
        if "PrivateToolHandleArray" in propType:
            setattr( destConfigurableInstance, pname, [conf2toConfigurable( tool, indent=__indent( indent ), parent = sourceConf2Instance.getName() ) for tool in pvalue] )
            _log.debug( "%sSetting private tool array property %s of %s", indent, pname, destConfigurableInstance.name() )
        elif ("PrivateToolHandle" in propType or
              "GaudiConfig2.Configurables" in propType or
              "ServiceHandle" in propType):

            _log.debug( "%sSetting property %s of %s", indent, pname, destConfigurableInstance.name() )
            try: #sometimes it is not printable
                _log.debug("%sComponent: %s", indent, pvalue)
            except Exception:
                pass
            if pvalue is not None:
                setattr( destConfigurableInstance, pname,
                         conf2toConfigurable( pvalue, indent=__indent( indent ),
                                              parent = sourceConf2Instance.getName() ) )
            else:
                setattr( destConfigurableInstance, pname, pvalue )

        else: # plain data
            if isinstance(pvalue,(GaudiConfig2.semantics._ListHelper,GaudiConfig2.semantics._DictHelper)):
                pvalue=pvalue.data
            try: #sometimes values are not printable
                _log.debug( "%sSetting property %s to value %s", indent, pname, pvalue )
            except Exception:
                pass
            setattr( destConfigurableInstance, pname, pvalue )


def conf2toConfigurable( comp, indent="", parent="", suppressDupes=False ):
    """
    Method converts from Conf2 ( comp argument ) to old Configurable
    If the Configurable of the same name exists, the properties merging process is invoked
    """
    _log = logging.getLogger( "conf2toConfigurable" )
    from AthenaCommon.CFElements import compName
    def __isOldConfigurable(c):
        from AthenaCommon.Configurable import Configurable
        return True if isinstance(c, Configurable) else False


    if __isOldConfigurable( comp ):
        _log.debug( "%sComponent is already OLD Configurable object %s, no conversion", indent, compName(comp) )
        return comp

    if isinstance( comp, str ):
        if comp:  # warning for non-empty string
            _log.warning( "%sComponent: \"%s\" is of type string, no conversion, some properties possibly not set?", indent, comp )
        return comp

    _log.debug( "%sConverting from GaudiConfig2 object %s type %s, parent %s", indent, compName(comp), comp.__class__.__name__ , parent)

    def __alreadyConfigured( comp, parent ):
        from AthenaCommon.Configurable import Configurable
        instanceName = comp.getName()
        for conf in Configurable.allConfigurables.values():
            conf_name = ''
            try:
                conf_name=conf.name()
            except TypeError:
                # Is a string?
                conf_name==conf

            if (conf_name==instanceName):
                if conf.getParent() == parent:
                    _log.debug( "%s Matched component: \"%s\" with parent %s with same from allConfigurables match.", indent, instanceName, parent if parent else "[not set]" )
                    return conf
                else:
                    _log.debug( "%sComponent: \"%s\" had parent %s whilst this allConfigurables match had parent %s.", indent, instanceName, parent if parent else "[not set]", conf.getParent() )
        return None

    def __createConf2Object( name ):
        typename,instanceName =   name.split( "/" ) if "/" in name else (name,name)
        return CompFactory.getComp( typename.replace( "__", "::" ) )( instanceName )

    def __configurableToConf2( comp, indent="" ):
        _log.debug( "%sConverting Conf2 to Configurable class %s, type %s", indent, comp.getFullName(), type(comp) )
        conf2Object = __createConf2Object( comp.getFullName() )
        __getProperties( comp, conf2Object, __indent( indent ) )
        return conf2Object

    def __getProperties( sourceConfigurableInstance, destConf2Instance, indent="" ):
        for prop, value in sourceConfigurableInstance.getProperties().items():
            _log.debug( "%sDealing with class %s property %s value type %s",
                        indent, sourceConfigurableInstance.getFullJobOptName(), prop, type(value) )
            if "ServiceHandle" in str( type( value ) ):
                instance = __alreadyConfigured(value, sourceConfigurableInstance.getName())
                if instance:
                    setattr( destConf2Instance, prop, __configurableToConf2(instance, __indent(indent)) )
            else:
                if isinstance(value,(GaudiConfig2.semantics._ListHelper,GaudiConfig2.semantics._DictHelper)):
                    value=value.data
                setattr( destConf2Instance, prop, value )

    def __findConfigurableClass( name ):
        if "::" in name: # cure namespaces
            name = name.replace( "::", "__" )

        if "<" in name:
            name=name.replace("<","_")
            name=name.replace(">","_")
            name=name.replace(", ","_")

        from AthenaCommon import CfgMgr
        classObj = getattr( CfgMgr, name )

        if not classObj:
            raise ConfigurationError( "CAtoGlobalWrapper could not find the component of type {} giving up ...".format(name) )

        return classObj


    def __listHelperToList(listOrDictHelper):
        if isinstance(listOrDictHelper,GaudiConfig2.semantics._ListHelper):
            return [ __listHelperToList(l) for l in listOrDictHelper.data]
        elif isinstance(listOrDictHelper,GaudiConfig2.semantics._DictHelper):
            return listOrDictHelper.data
        else:
            return listOrDictHelper

    def __areSettingsSame( existingConfigurableInstance, newConf2Instance, indent="" ):
        _log.debug( "%sChecking if settings are the same %s (%s) old(new)",
                    indent, existingConfigurableInstance.getFullName(), newConf2Instance.getFullJobOptName() )
        if (existingConfigurableInstance.getType() != newConf2Instance.__cpp_type__):
            raise ConfigurationError("Old/new ({} | {}) cpp types are not the same for ({} | {}) !".format(
                                    existingConfigurableInstance.getType(),newConf2Instance.__cpp_type__,
                                    existingConfigurableInstance.getFullName(), newConf2Instance.getFullJobOptName() ) )

        alreadySetProperties = existingConfigurableInstance.getValuedProperties().copy()
        _log.debug( "%sExisting properties: %s", indent, alreadySetProperties )
        _log.debug( "%sNew properties: %s", indent, newConf2Instance._properties )
        for pname, pvalue in newConf2Instance._properties.items():
            if __isOldConfigurable( pvalue ):
                _log.warning( "%sNew configuration object %s property %s has legacy configuration "
                              "components assigned to it %s. Skipping comparison, no guarantees "
                              "about configuration consistency.",
                              indent, compName(newConf2Instance), pname, compName(pvalue) )
                continue
            propType = newConf2Instance._descriptors[pname].cpp_type
            _log.debug( "%sComparing type: %s for: %s in: %s", indent, propType, pname, existingConfigurableInstance.getFullJobOptName() )
            if  "PrivateToolHandleArray" in  propType:
                toolDict = {_.getName(): _ for _ in alreadySetProperties[pname]}
                _log.debug('Private tool properties? %s', toolDict)
                newCdict = {_.getName() : _ for _ in pvalue}
                oldCset = set(toolDict); newCset = set(newCdict)
                _log.debug('Private tool property names? %s %s', oldCset, newCset)
                if ( not (oldCset == newCset) ):
                    _log.warning('%s PrivateToolHandleArray %s of %s does not have the same named components',indent, pname, existingConfigurableInstance.getFullJobOptName() )
                    _log.warning('%s Old (conf1) %s for %s',indent, sorted(oldCset), existingConfigurableInstance.getFullJobOptName())
                    _log.warning('%s New (conf2) %s for %s',indent, sorted(newCset), newConf2Instance.getFullJobOptName())
                    _log.warning('%s Will try to merge them, but this might go wrong!',indent)
                for oldC in oldCset & newCset:
                    __areSettingsSame( toolDict[oldC], newCdict[oldC], __indent(indent))
                # And now just the new properties in conf2 (the stuff just in conf1 is already in the objec)
                for newC in sorted(newCset-oldCset):
                    className = newCdict[newC].getFullJobOptName().split( "/" )[0]
                    _log.debug('%s %s not in oldconfig. Will try to create conf1 instance using this className: %s, and merge.',indent, newC, className)
                    configurableClass = __findConfigurableClass( className )
                    # Do not create with existing name, or it will try to get an existing public tool, if available
                    # (and public tools cannot be added to a PrivateToolHandleArray)
                    instance = configurableClass( newC + className + str(len(indent)) )

                    # Now give it the correct name
                    instance._name = newCdict[newC].name

                    __setProperties( instance, newCdict[newC], __indent( indent ) )
                    _log.debug('%s will now add %s to array.',indent, instance)
                    existingConfigurableInstance += instance # Makes a copy with a correctly set parent and name
                    alreadySetProperties[pname].append(instance)
            elif "PublicToolHandleArray" in propType:
                toolSet = {_.getName() for _ in alreadySetProperties[pname]}
                _log.debug('Public tool handle array properties? %s %s', toolSet, pvalue)
                # strings?
                for newC in pvalue:
                    if isinstance(newC, str):
                        pubtoolclass, pubtoolname = newC.split('/')
                        if pubtoolname not in toolSet:
                            klass = __findConfigurableClass( pubtoolclass )
                            instance = klass(pubtoolname)
                            from AthenaCommon.AppMgr import ToolSvc
                            ToolSvc += instance
                            alreadySetProperties[pname].append(instance)
                    else:
                        _log.warning('Not handling actual Configurable2s for public tool merging yet')
                        raise Exception()
            elif "PrivateToolHandle" in propType or "GaudiConfig2.Configurables" in propType or "ServiceHandle" in propType:
                existingVal = getattr(existingConfigurableInstance, pname)
                if isinstance( pvalue, str ):
                    _log.warning("%sThe handle %s of new-config component %s.%s is just a string %s, "
                                 "skipping deeper checks, configuration may be incorrect",
                                 indent, propType, newConf2Instance.name, pname, pvalue)
                elif pvalue is None:
                    _log.debug("%sThe property value for %s of %s is None. Skipping.", indent, pname, newConf2Instance.name )
                    continue
                elif str(existingVal) == "":
                    className = pvalue.getFullJobOptName().split( "/" )[0]
                    pvalueCompName = pvalue.getFullJobOptName().split( "/" )[1]
                    _log.debug("%sThe existing value for %s of %s is an empty handle. "
                               "Will try to create conf1 instance using this className: %s, and merge.",
                               indent, pname, newConf2Instance.name, className )
                    configurableClass = __findConfigurableClass( className )
                    # Do not create with existing name, or it will try to get an existing public tool, if available
                    # (and public tools cannot be added to a PrivateToolHandle)
                    instance = configurableClass( pvalueCompName + className + str(len(indent)) )
                    # Now give it the correct name, assign to the conf1 property, and merge
                    instance._name = pvalueCompName
                    setattr(existingConfigurableInstance, pname, instance)
                    existingVal = getattr(existingConfigurableInstance, pname)
                    __areSettingsSame( existingVal, pvalue, indent)
                else:
                    _log.debug( "%sSome kind of handle and, object type %s existing %s",
                                indent, type(pvalue), type(existingVal) )
                    __areSettingsSame( existingVal, pvalue, indent)
            else:
                if isinstance(pvalue, (GaudiConfig2.semantics._ListHelper, GaudiConfig2.semantics._DictHelper)):
                    pvalue = pvalue.data
                if isinstance(pvalue, list):
                    pvalue = [item.data
                              if isinstance(item, (GaudiConfig2.semantics._ListHelper, GaudiConfig2.semantics._DictHelper))
                              else item
                              for item in pvalue]

                if pname not in alreadySetProperties:
                    _log.debug( "%sAdding property: %s for %s", indent, pname, newConf2Instance.getName() )
                    try:
                        setattr(existingConfigurableInstance, pname, pvalue)
                    except AttributeError:
                        _log.info("%sCould not set attribute. Type of existingConfigurableInstance %s.",indent, type(existingConfigurableInstance) )
                        raise
                elif alreadySetProperties[pname] != pvalue:
                    # Old configuration writes some properties differently e.g. like ConditionStore+TileBadChannels instead of just TileBadChannels
                    # So check this isn't a false positive before continuing
                    merge = True

                    # Could be strings e.g. alreadySetProperties[pname]==RPCCablingDbTool and pvalue == RPCCablingDbTool/RPCCablingDbTool
                    if (isinstance(pvalue, str) and isinstance(alreadySetProperties[pname], str)):
                        if ('/' in pvalue \
                               and pvalue.split('/')[-1] == alreadySetProperties[pname]):
                            # Okay. so they probably are actually the same. Can't check type.
                            merge=False
                            _log.warning( "%sProperties here are strings and not exactly the same. ASSUMING they match types but we cannot check. %s for %s", indent, pname, newConf2Instance.getName() )

                    try:
                        if ('+' in alreadySetProperties[pname].toStringProperty() \
                            and alreadySetProperties[pname].toStringProperty().split('+')[-1] == pvalue):
                            # Okay. so they ARE actually the same
                            merge=False
                    except AttributeError :
                        # This is fine - it just means it's not e.g. a DataHandle and doesn't have toStringProperty()
                        pass


                    # Okay, not the same ... let's merge
                    if merge:
                        _log.debug( "%sMerging property: %s for new config: %s", indent, pname, newConf2Instance.getName() )
                        # create surrogate
                        clone = newConf2Instance.getInstance("Clone")
                        setattr(clone, pname, alreadySetProperties[pname])
                        try:
                            updatedPropValue = __listHelperToList(newConf2Instance._descriptors[pname].semantics.merge( getattr(newConf2Instance, pname), getattr(clone, pname)))
                        except (TypeError, ValueError):
                            err_message = f"Failed merging new config value ({getattr(newConf2Instance, pname)}) and old config value ({getattr(clone, pname)}) for the ({pname}) property of {existingConfigurableInstance.getFullJobOptName() } ({newConf2Instance.getFullJobOptName()}) old (new)."
                            _log.fatal( err_message )
                            raise ConfigurationError(err_message)

                        _log.debug("existingConfigurable.name: %s, pname: %s, updatedPropValue: %s", existingConfigurableInstance.name(), pname, updatedPropValue )

                        setattr(existingConfigurableInstance, pname, updatedPropValue)
                        del clone
                        _log.debug("%sInvoked GaudiConf2 semantics to merge the %s and the %s to %s "
                                "for property %s of %s",
                                indent, alreadySetProperties[pname], pvalue, pname,
                                updatedPropValue, existingConfigurable.getFullName())

    _log.debug( "%sConf2 Full name: %s ", indent, comp.getFullJobOptName() )
    existingConfigurable = __alreadyConfigured( comp, parent )

    if existingConfigurable: # if configurable exists we try to merge with it
        _log.debug( "%sPre-existing configurable %s was found, checking if has the same properties", indent, existingConfigurable.getFullJobOptName() )
        __areSettingsSame( existingConfigurable, comp, indent )
        _log.debug( "%sPre-existing configurable %s was found to have the same properties", indent, comp.name )
        instance = existingConfigurable if not suppressDupes else None
    else: # create new configurable
        _log.debug( "%sExisting Conf1 not found. Creating component configurable %s", indent, comp.getFullJobOptName() )
        configurableClass = __findConfigurableClass( comp.getFullJobOptName().split( "/" )[0] )
        instance = configurableClass( comp.name )
        __setProperties( instance, comp, __indent( indent ) )
    return instance


def CAtoGlobalWrapper(cfgFunc, flags, **kwargs):
    """
    Temporarily available method allowing to merge CA into the configurations based on Configurable classes
    """
    if not callable(cfgFunc):
        raise TypeError("CAtoGlobalWrapper must be called with a configuration-function as parameter")
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    result = cfgFunc(flags, **kwargs)
    if isinstance(result, tuple):
        ca = result[0]
    else:
        ca = result
    Configurable.configurableRun3Behavior=0

    appendCAtoAthena(ca)
    pass


def appendCAtoAthena(ca):
    _log = logging.getLogger( "conf2toConfigurable" )
    _log.debug( "Merging ComponentAccumulator into global configuration" )

    from AthenaCommon.AppMgr import ServiceMgr,ToolSvc,theApp,athCondSeq,athOutSeq,athAlgSeq,topSequence
    if len( ca.getPublicTools() ) != 0:
        for comp in ca.getPublicTools():
            instance = conf2toConfigurable( comp, indent="  ", parent="ToolSvc" )
            if instance not in ToolSvc:
                ToolSvc += instance

    if len(ca.getServices()) != 0:
        for comp in ca.getServices():
            instance = conf2toConfigurable( comp, indent="  " )
            if instance not in ServiceMgr:
                ServiceMgr += instance
        for svcName in ca._servicesToCreate:
            if svcName not in theApp.CreateSvc:
                theApp.CreateSvc += [svcName]

    if  len(ca._conditionsAlgs) != 0:
        for comp in ca._conditionsAlgs:
            instance = conf2toConfigurable( comp, indent="  " )
            if instance not in athCondSeq:
                athCondSeq += instance

    if len( ca.getAppProps() ) != 0:
        for propName, propValue in ca.getAppProps().items():
            # Same logic as in ComponentAccumulator.setAppProperty()
            if not hasattr(theApp, propName):
                setattr(theApp, propName, propValue)
            else:
                origPropValue = getattr(theApp, propName)
                if origPropValue == propValue:
                    _log.info("ApplicationMgr property '%s' already set to '%s'.", propName, propValue)
                elif isinstance(origPropValue, collections.abc.Sequence) and not isinstance(origPropValue, str):
                    propValue =  origPropValue + [el for el in propValue if el not in origPropValue]
                    _log.info("ApplicationMgr property '%s' already set to '%s'. Overwriting with %s", propName, origPropValue, propValue)
                    setattr(theApp, propName, propValue)
                else:
                    raise DeduplicationFailed("ApplicationMgr property {} set twice: {} and {}".format(propName, origPropValue, propValue))

    from AthenaCommon.CFElements import findSubSequence

    def __fetchOldSeq(name=""):
        from AthenaCommon.Configurable import Configurable
        currentBehaviour = Configurable.configurableRun3Behavior
        Configurable.configurableRun3Behavior=0
        from AthenaCommon.AlgSequence import AlgSequence
        seq =  AlgSequence(name)
        Configurable.configurableRun3Behavior=currentBehaviour
        return seq

    def __mergeSequences( currentConfigurableSeq, conf2Sequence, indent="" ):
        sequence = findSubSequence( currentConfigurableSeq, conf2Sequence.name )
        if not sequence:
            sequence = __fetchOldSeq( conf2Sequence.name )
            __setProperties( sequence, conf2Sequence, indent=__indent( indent ) )
            currentConfigurableSeq += sequence
            _log.debug( "%sCreated missing AlgSequence %s and added to %s",
                        __indent( indent ), sequence.name(), currentConfigurableSeq.name() )

        for el in conf2Sequence.Members:
            if el.__class__.__name__ == "AthSequencer":
                __mergeSequences( sequence, el, __indent( indent ) )
            elif el.getGaudiType() == "Algorithm":
                toadd = conf2toConfigurable( el, indent=__indent( indent ), suppressDupes=True)
                if toadd is not None:
                    sequence += toadd
                    _log.debug( "%sAlgorithm %s and added to the sequence %s",
                                __indent( indent ),  el.getFullJobOptName(), sequence.name() )

    preconfigured = [athCondSeq,athOutSeq,athAlgSeq,topSequence]

    for seq in ca._allSequences:
        merged = False
        for pre in preconfigured:
            if seq.getName() == pre.getName():
                _log.debug( "%sfound sequence %s to have the same name as predefined %s",
                            __indent(), seq.getName(),  pre.getName() )
                __mergeSequences( pre, seq )
                merged = True
                break
            if findSubSequence( pre, seq.name ):
                _log.debug( "%sfound sequence %s in predefined %s",
                            __indent(), seq.getName(),  pre.getName() )
                __mergeSequences( pre, seq )
                merged = True
                break

        if not merged:
            _log.debug( "%snot found sequence %s merging it to AthAlgSeq", __indent(), seq.name )
            __mergeSequences( athAlgSeq, seq )

    ca.wasMerged()
    _log.debug( "Merging of CA to global done" )
