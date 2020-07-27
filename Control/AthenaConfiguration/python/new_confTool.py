#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
import json
import pickle
import sys
import six

from GaudiConfig2 import Configurable


def main(args):
    if args.printComps:
        for fileName in args.file:
            conf = _loadSingleFile(fileName)
            conf.printConfig()

    if args.printConf:
        for fileName in args.file:
            conf = _loadSingleFile(fileName)
            conf.printConfig(withDetails=True)

    if args.toJSON:
        if len(args.file) != 1:
            sys.exit("ERROR, can convert single file at a time, got: %s" % args.file)
        conf = _loadSingleFile(args.file[0])
        with open(args.toJSON, "w") as oFile:
            props = conf.gatherProps()
            jos_props = props[2] # to make json compatible with old configuration
            to_json = {}
            for comp, name, value in jos_props: 
                to_json.setdefault(comp, {})[name] = value
                to_json[comp][name] = value
            json.dump([[], to_json], oFile)

    if args.toPickle:
        if len(args.file) != 1:
            sys.exit("ERROR, can convert single file at a time, got: %s" % args.file)
        conf = _loadSingleFile(args.file[0])
        with open(args.toPickle, "wb") as oFile:
            conf.store(oFile)

    if args.diff:
        if len(args.file) != 2:
            sys.exit("ERROR, can diff exactly two files at a time, got: %s" % args.file)
        configRef = _loadSingleFile(args.file[0])
        configChk = _loadSingleFile(args.file[1])
        _compareConfig(configRef, configChk, args)


def _loadSingleFile(fname):
    conf = None
    if fname.endswith(".pkl"):
        with open(fname, "rb") as input_file:
            conf = pickle.load(input_file)
        print("Read pickle file: {}".format(fname)) 
    else:
        sys.exit("File format not supported.")

    if conf is None:
        sys.exit("Unable to load %s file" % fname)

    return conf


def _compareConfig(configRef, configChk, args):
    print("\nallSequences:")
    for ref_sequence, chk_sequence in zip(
        configRef._allSequences, configChk._allSequences
    ):
        _compareCollection(ref_sequence, chk_sequence, args)

    print("\nallconditionsAlgs:")
    _compareCollection(
        sorted(configRef._conditionsAlgs, key=lambda x: x.name),
        sorted(configChk._conditionsAlgs, key=lambda x: x.name),
        args,
    )

    print("\nservices:")
    _compareCollection(
        sorted(configRef._services, key=lambda x: x.name),
        sorted(configChk._services, key=lambda x: x.name),
        args,
    )

    print("\npublicTools:")
    _compareCollection(
        sorted(configRef._publicTools, key=lambda x: x.name),
        sorted(configChk._publicTools, key=lambda x: x.name),
        args,
    )

    print("\ntheAppProps:")
    _compareCollection(configRef._theAppProps, configChk._theAppProps, args)


def _compareCollection(ref_collection, chk_collection, args):
    if isinstance(ref_collection, list):
        all_names = list(set([obj.name for obj in ref_collection + chk_collection]))
        # print(all_names)
        for name in all_names:
            ref = _findByName(name, ref_collection)
            chk = _findByName(name, chk_collection)
            if not ref:
                print(
                    "\n\033[91m Component %s \033[94m exists only in Chk \033[0m \033[0m \n" % name
                )
            elif not chk:
                print(
                    "\n\033[91m Component %s \033[92m exists only in Ref \033[0m \033[0m \n" % name
                )
            elif not _equalComponent(ref, chk):
                _compareComponent(ref, chk, args)

    elif isinstance(ref_collection, dict):
        all_keys = set(list(ref_collection.keys()) + list(chk_collection.keys()))
        for key in all_keys:
            if key not in ref_collection:
                print(
                    "%s= %s: \033[94m exists only in Chk \033[0m \033[91m<< !!!\033[0m" % (key, chk_collection[key])
                )
            elif key not in chk_collection:
                print(
                    "%s= %s: \033[92m exists only in Ref \033[0m \033[91m<< !!!\033[0m" % (key, ref_collection[key])
                )
            elif ref_collection[key] != chk_collection[key]:
                print(
                    "%s : \033[92m %s \033[0m vs \033[94m %s \033[0m \033[91m<< !!!\033[0m" % (key, str(ref_collection[key]), str(chk_collection[key]))
                )


def _compareComponent(compRef, compChk, args, level=0):

    ref_keys = compRef._descriptors.keys()
    chk_keys = compChk._descriptors.keys()

    all_descriptors = set(list(ref_keys) + list(chk_keys))

    for prop in all_descriptors:
        if prop not in ref_keys:
            print(
                "%s = %s: \033[94m exists only in Chk \033[0m \033[91m<< !!!\033[0m" % (prop, compChk[prop])
            )
            continue

        if prop not in chk_keys:
            print(
                "%s = %s: \033[92m exists only in Ref \033[0m \033[91m<< !!!\033[0m" % (prop, compRef[prop])
            )
            continue

        ref_val = compRef._descriptors[prop].__get__(compRef, "")
        chk_val = compChk._descriptors[prop].__get__(compChk, "")

        if _getStrDescriptor(ref_val) == _getStrDescriptor(chk_val):
            if not args.printIdenticalPerParameter:
                continue
            diffmarker = ""
        else:
            diffmarker = " \033[91m<< !!!\033[0m"

        print(
            "%s%s : \033[92m %s \033[0m vs \033[94m %s \033[0m %s" % ('>> '*level, prop, str(ref_val), str(chk_val), diffmarker)
        )

        if isinstance(ref_val, Configurable):
            print("vvv")
            _compareComponent(ref_val, chk_val, args, level + 1)


def _equalComponent(self, other):

    if type(self) is not type(other):
        return False
    if hasattr(self, "name") != hasattr(other, "name"):
        return False
    if hasattr(self, "name") and (self.name != other.name):
        return False
    all_descriptors = set(
        list(self._descriptors.keys()) + list(other._descriptors.keys())
    )
    for name in all_descriptors:
        if name not in self._descriptors or name not in other._descriptors:
            return False
        self_prop = self._descriptors[name].__get__(self, "")
        other_prop = other._descriptors[name].__get__(other, "")
        if _getStrDescriptor(self_prop) != _getStrDescriptor(other_prop):
            return False
    return True


def _getStrDescriptor(obj):
    descr = ""
    if hasattr(obj, "_name"):
        propstr = _getFlattenedProperties(obj)
        descr = (obj.name, propstr)
    elif isinstance(obj, list) and len(obj) > 0 and isinstance(obj[0], Configurable):
        for el in obj:
            descr += str(_getStrDescriptor(el))
    else:
        descr = str(obj)
    return descr


def _getFlattenedProperties(obj):
    properties = _getPropertyValues(obj)
    propstr = ""
    for key, val in sorted(six.iteritems(properties)):
        if isinstance(val, Configurable):
            propstr += "({0}:{1})".format(key, _getFlattenedProperties(val))
        elif isinstance(val, str):
            propstr += "(%s)" % val
        elif hasattr(val, "__iter__"):
            for th in val:
                if isinstance(th, Configurable):
                    propstr += "({0}:{1})".format(th.name, _getFlattenedProperties(th))
                elif isinstance(th, str) and "{" in th:  # parse dict saved as a string
                    propstr += "(%s)" % str(sorted(json.loads(th).items()))
                else:
                    propstr += "(%s)" % th
        else:
            propstr += "({0}:{1})".format(key, str(val))
    return propstr


def _getPropertyValues(obj):
    props = {}
    for name, proxy in sorted(six.iteritems(obj._descriptors)):
        value = proxy.__get__(obj, "")
        props[name] = value
    return props


def _findByName(name, collection):
    match = list(filter(lambda x: x.name == name, collection))
    return match[0] if match else None
