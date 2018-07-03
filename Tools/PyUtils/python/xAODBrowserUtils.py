# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import cPickle as pickle
from xAODBrowserElement import Element

def load_object(path):
    f = open(path, 'rb')
    data = pickle.load(f)
    f.close()
    return data

def load_initial_data(path):
    structures = load_object(path)
    if not type(structures) is list:
        raise TypeError('Structures must be list of Element')
    for el in structures:
        if not isinstance(el, Element):
            raise TypeError('All items in list must be of type Element')
    return structures

def save_configuration(path, structures):
    if not type(structures) is list:
        raise TypeError('Structures must be list of Element')
    for el in structures:
        if not isinstance(el, Element):
            raise TypeError('All items in list must be of type Element')
    save_object(path, structures)

def save_object(path, data):
    output = open(path, 'wb')
    pickle.dump(data, output)
    output.close()

def generate_data_dict(names_arrays):
    data_dict = {}
    for array in names_arrays:
        element = data_dict
        for name in array:
            if name.strip():
                if name not in element:
                    element[name] = {}
                element = element[name]
    return data_dict
