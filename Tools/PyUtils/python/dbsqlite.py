# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file PyUtils/python/dbsqlite.py
# reaped off: http://svn.python.org/view/sandbox/trunk/dbm_sqlite
"""Dbm based on sqlite -- Needed to support shelves

Issues:

    # ??? how to coordinate with whichdb
    # ??? Any difference between blobs and text
    # ??? does default encoding affect str-->bytes or PySqlite3 always use UTF-8
    # ??? what is the correct isolation mode

"""

from __future__ import print_function

__all__ = ['error', 'open']

import sqlite3
import pickle
from collections import MutableMapping
from operator import itemgetter
import shelve

error = sqlite3.DatabaseError

class SQLhash(MutableMapping):
    def __init__(self, filename=':memory:', flags='r', mode=None):
        # XXX add flag/mode handling
        #   c -- create if it doesn't exist
        #   n -- new empty
        #   w -- open existing
        #   r -- readonly
        if 'n' in flags:
            import os
            if os.path.exists(filename):
                os.remove(filename)

        MAKE_SHELF = 'CREATE TABLE IF NOT EXISTS shelf (key TEXT PRIMARY KEY NOT NULL, value BLOB)'
        self.conn = sqlite3.connect(filename)
        self.conn.text_factory = str
        if 'r' not in flags or filename==':memory:':
            self.conn.execute(MAKE_SHELF)
        self.conn.commit()

    def __len__(self):
        GET_LEN = 'SELECT COUNT(*) FROM shelf'
        return self.conn.execute(GET_LEN).fetchone()[0]

    def __bool__(self):
        # returns None if count is zero
        GET_BOOL = 'SELECT MAX(ROWID) FROM shelf'
        return self.conn.execute(GET_BOOL).fetchone()[0] is not None

    def keys(self):
        return list(self.iterkeys())

    def values(self):
        return list(self.itervalues())

    def items(self):
        return list(self.iteritems())

    def __iter__(self):
        return self.iterkeys()

    def iterkeys(self):
        GET_KEYS = 'SELECT key FROM shelf ORDER BY ROWID'
        return SQLHashKeyIterator(self.conn, GET_KEYS, (0,))

    def itervalues(self):
        GET_VALUES = 'SELECT value FROM shelf ORDER BY ROWID'
        return SQLHashValueIterator(self.conn, GET_VALUES, (0,))

    def iteritems(self):
        GET_ITEMS = 'SELECT key, value FROM shelf ORDER BY ROWID'
        return SQLHashItemIterator(self.conn, GET_ITEMS, (0, 1))

    def __contains__(self, key):
        HAS_ITEM = 'SELECT 1 FROM shelf WHERE key = ?'
        return self.conn.execute(HAS_ITEM, (key,)).fetchone() is not None

    def __getitem__(self, key):
        GET_ITEM = 'SELECT value FROM shelf WHERE key = ?'
        item = self.conn.execute(GET_ITEM, (key,)).fetchone()
        if item is None:
            raise KeyError(key)

        return pickle.loads(item[0])

    def __setitem__(self, key, value):       
        ADD_ITEM = 'REPLACE INTO shelf (key, value) VALUES (?,?)'
        value = pickle.dumps(value)
        self.conn.execute(ADD_ITEM, (key, value))
        #self.conn.commit()

    def __delitem__(self, key):
        if key not in self:
            raise KeyError(key)
        DEL_ITEM = 'DELETE FROM shelf WHERE key = ?'
        self.conn.execute(DEL_ITEM, (key,))
        #self.conn.commit()

    def update(self, items=(), **kwds):
        try:
            items = items.items()
            items = [(k,pickle.dumps(v)) for k,v in items]
        except AttributeError:
            pass

        UPDATE_ITEMS = 'REPLACE INTO shelf (key, value) VALUES (?, ?)'
        self.conn.executemany(UPDATE_ITEMS, items)
        self.conn.commit()
        if kwds:
            self.update(kwds)

    def clear(self):        
        CLEAR_ALL = 'DELETE FROM shelf; VACUUM;'
        self.conn.executescript(CLEAR_ALL)
        self.conn.commit()

    def sync(self):
        if self.conn is not None:    
            self.conn.commit()

    def close(self):
        if self.conn is not None:
            self.conn.commit()
            self.conn.close()
            self.conn = None

    def __del__(self):
        self.close()

def open(file=None, *args, **kw):
    if file is not None:
        return SQLhash(file, *args, **kw)
    return SQLhash()

def open_shelf(file=None, *args, **kw):
    _db = open(file, *args, **kw)
    return shelve.Shelf(_db)

class SQLHashKeyIterator(object):
    def __init__(self, conn, stmt, indices):
        c = conn.cursor()
        c.execute(stmt)
        
        self.iter = iter(c)
        self.getter = itemgetter(*indices)

    def __iter__(self):
        return self

    def next(self): #py2
        return self.getter(self.iter.next())
    def __next__(self): #py3
        return self.getter(self.iter.__next__())

class SQLHashValueIterator(object):
    def __init__(self, conn, stmt, indices):
        c = conn.cursor()
        c.execute(stmt)
        
        self.iter = iter(c)
        self.getter = itemgetter(*indices)

    def __iter__(self):
        return self

    def next(self): #py2
        o = self.getter(self.iter.next())
        return pickle.loads(o)
    def __next__(self): #py3
        o = self.getter(self.iter.__next__())
        return pickle.loads(o)

class SQLHashItemIterator(object):
    def __init__(self, conn, stmt, indices):
        c = conn.cursor()
        c.execute(stmt)
        
        self.iter = iter(c)
        self.getter = itemgetter(*indices)

    def __iter__(self):
        return self

    def next(self): #py2
        o = self.getter(self.iter.next())
        k = o[0]
        v = pickle.loads(o[1])
        return (k,v)
    def __next__(self): #py3
        o = self.getter(self.iter.__next__())
        k = o[0]
        v = pickle.loads(o[1])
        return (k,v)

if __name__ in '__main___':
    for d in SQLhash(flags='n'), SQLhash('example',flags='n'):
        list(d)
        print(list(d), "start")
        d['abc'] = 'lmno'
        print(d['abc'])    
        d['abc'] = 'rsvp'
        d['xyz'] = 'pdq'
        print(d.items())
        print(d.values())
        print('***', d.keys())
        print(list(d), 'list')
        d.update(p='x', q='y', r='z')
        print(d.items())
        
        del d['abc']
        try:
            print(d['abc'])
        except KeyError:
            pass
        else:
            raise Exception('oh noooo!')
        
        try:
            del d['abc']
        except KeyError:
            pass
        else:
            raise Exception('drat!')

        print(list(d))
        print(bool(d), True)        
        d.clear()
        print(bool(d), False)
        print(list(d))
        d.update(p='x', q='y', r='z')
        print(list(d))
        d['xyz'] = 'pdq'

        d['a_list'] = range(5)
        print(d['a_list'])

        d['a_dict'] = {1:'one',2:'two'}
        print(d['a_dict'])

        d['a_tuple'] = (1,2,3,4)
        print(d['a_tuple'])
        
        print()
        d.close()
