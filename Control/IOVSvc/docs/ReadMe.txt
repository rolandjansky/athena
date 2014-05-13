
For full docs, see:   http://annwm.lbl.gov/Atlas/IOVSvc

What it does:
=============

This is very similar to the previous version, but it makes no
use of a base class for registered objects, instead using 
function bindings.

Registers an object, with service.
Object is registered with a string key which identifies object
in database.
Uses boost::function<void> to hold binding callback function and
object, as bound with boos::bind( *fnc(), *obj ). 

Both DataHandles and functions can be registered with the service.
Functions must be virtual, and have a specific signature (see
IOVSvc/IOVSvcDefs.h).

At start of each event, registered objects are scanned to see if
are valid/invalid, according to a time stamp (event number). If
they are invalid, callback function is activated, and object is
updated from dB (remains to be done).


Classes:
========
IOVTime: holds a timestamp. Currently a combination of run and event
         number

IOVRange: a start and stop IOVTime

IOVEntry: associates a callback function with a key and a IOVRange

CallBackID: used to identify a callback function

IOVSvc: the service


How it does it:
===============

entries are stored in 

  - a map<void*, pair< string, boost::function<void>* > > m_obs:
    keeps track of registered objects, and mapping to function and
    key. Used to make sure object isn't registered twice.

  - a map<boost::function<void>*, string> m_observers: which associates the
    object with a db key.

  - a map<boost::function<void>*, string> m_names: which holds the
    type names of the objects that are registered.

  - a set<IOVEntry*> m_entries: which holds all registered entries

  - a multiset<IOVEntry*,IOVEntryStartCritereon> m_startSet: which
    holds list of valid entries, ordered by decreasing start time.

  - a multiset<IOVEntry*,IOVEntryStopCritereon> m_stopSet: which
    holds list of valid entries, ordered by increasing stop time.


At the start of each event, iterate through m_startSet. If entry is
invalid, add to set of functions to callback, and remove from
m_startSet. As soon as find first valid entry, can stop iteration. Do
the same for the m_stopSet.

Iterate through set of callback functions, and call each one.

Iterate through set of callback functions, and update validity range
and data content of each one from dB (remains to be done).

