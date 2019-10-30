# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
import pickle


def save_object(file_path, obj):
    f = open(file_path, 'wb')
    pickle.dump(obj, f)
    f.close()


def load_strings_set(file_path):
    f = open(file_path, 'rb')
    s = pickle.load(f)
    f.close()
    if type(s) != set:
        raise TypeError('Saved object is not a set')
    for element in s:
        if type(element) != str:
            raise TypeError('Set element is not a string')
    return s
