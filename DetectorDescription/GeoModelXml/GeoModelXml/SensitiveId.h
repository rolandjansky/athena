/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//    Class for generating unique id's for sensitive volumes (unique across 
//    all sensitive detectors in the gmx file, so not unique across ATLAS!)
//    Used in LogvolProcessor for GeoModel identifiers.
//    Conceptually, the class has a stock of identifiers, and pop() returns the next one, like a stack.
//    In fact, the identifiers are the sequence 0, 1, ...; suitable for use as an array index.
//
class SensitiveId {
public:
    SensitiveId() {m_id = 0;}
    int pop() {return m_id++;}
private:
    int m_id;
};
