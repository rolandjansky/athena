/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3DCE7F140063.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3DCE7F140063.cm

//## begin module%3DCE7F140063.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3DCE7F140063.cp

//## Module: Path%3DCE7F140063; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/Path.h

#ifndef Path_h
#define Path_h 1

//## begin module%3DCE7F140063.additionalIncludes preserve=no
//## end module%3DCE7F140063.additionalIncludes

//## begin module%3DCE7F140063.includes preserve=yes
#include <vector>
#include <cstdlib>
//## end module%3DCE7F140063.includes

//## begin module%3DCE7F140063.additionalDeclarations preserve=yes
//## end module%3DCE7F140063.additionalDeclarations


//## begin Path%3DCE7F140063.preface preserve=yes
//## end Path%3DCE7F140063.preface

//## Class: Path%3DCE7F140063; Parameterized Class
//	Represents a path from one point in the general tree, to
//	the present point.
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



template <class T>
class Path 
{
  //## begin Path%3DCE7F140063.initialDeclarations preserve=yes
  //## end Path%3DCE7F140063.initialDeclarations

  public:
    //## Constructors (generated)
      Path();

      Path(const Path< T > &right);

    //## Destructor (generated)
      ~Path();

    //## Assignment Operation (generated)
      Path< T > & operator=(const Path< T > &right);


    //## Other Operations (specified)
      //## Operation: pop%3DCE7F9C0249
      //	Pops the last node from the stack.
      const T * pop ();

      //## Operation: push%3DCE7F9C025D
      //	Pushes another node on to the tail of the path.
      void push (const T *node);

      //## Operation: getLength%3DCE7F9C027B
      //	Returns the length of the path.
      unsigned int getLength () const;

      //## Operation: getItem%3DCE7F9C028F
      //	Returns the ith Item.
      const T * getItem (unsigned int i) const;

      //## Operation: getHead%3DCE7F9C02A3
      //	Gets the head of the path.
      const T * getHead () const;

      //## Operation: getTail%3DCE7F9C02C1
      //	Gets the tail of the path.
      const T * getTail () const;

    // Additional Public Declarations
      //## begin Path%3DCE7F140063.public preserve=yes
      //## end Path%3DCE7F140063.public

  protected:
    // Additional Protected Declarations
      //## begin Path%3DCE7F140063.protected preserve=yes
      //## end Path%3DCE7F140063.protected

  private:
    // Data Members for Class Attributes

      //## Attribute: _nodeList%3DCE7F8E01C7
      //	A list of physical volumes betwee the head of the path
      //	and the tail of the path.
      //## begin Path::_nodeList%3DCE7F8E01C7.attr preserve=no  private: std::vector<const T *> {U} 
      std::vector<const T *> _nodeList;
      //## end Path::_nodeList%3DCE7F8E01C7.attr

    // Additional Private Declarations
      //## begin Path%3DCE7F140063.private preserve=yes
      //## end Path%3DCE7F140063.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin Path%3DCE7F140063.implementation preserve=yes
      //## end Path%3DCE7F140063.implementation

};

//## begin Path%3DCE7F140063.postscript preserve=yes
//## end Path%3DCE7F140063.postscript

// Parameterized Class Path 

template <class T>
inline Path<T>::Path()
  //## begin Path::Path%3DCE7F140063_const.hasinit preserve=no
  //## end Path::Path%3DCE7F140063_const.hasinit
  //## begin Path::Path%3DCE7F140063_const.initialization preserve=yes
  //## end Path::Path%3DCE7F140063_const.initialization
{
  //## begin Path::Path%3DCE7F140063_const.body preserve=yes
  _nodeList.reserve(2);
  //## end Path::Path%3DCE7F140063_const.body
}

template <class T>
inline Path<T>::Path(const Path<T> &right)
  //## begin Path::Path%3DCE7F140063_copy.hasinit preserve=no
  //## end Path::Path%3DCE7F140063_copy.hasinit
  //## begin Path::Path%3DCE7F140063_copy.initialization preserve=yes
  :_nodeList(right._nodeList)
  //## end Path::Path%3DCE7F140063_copy.initialization
{
  //## begin Path::Path%3DCE7F140063_copy.body preserve=yes
  //## end Path::Path%3DCE7F140063_copy.body
}


template <class T>
inline Path<T>::~Path()
{
  //## begin Path::~Path%3DCE7F140063_dest.body preserve=yes
  //## end Path::~Path%3DCE7F140063_dest.body
}


template <class T>
inline Path<T> & Path<T>::operator=(const Path<T> &right)
{
  //## begin Path::operator=%3DCE7F140063_assign.body preserve=yes
  if (this != &right)
    {
      _nodeList = right._nodeList;
    }
  return *this;
  //## end Path::operator=%3DCE7F140063_assign.body
}



//## Other Operations (inline)
template <class T>
inline const T * Path<T>::pop ()
{
  //## begin Path::pop%3DCE7F9C0249.body preserve=yes
  if (_nodeList.size () == 0)
    {
      return NULL;
    }
  else
    {
      const T *gvp = _nodeList.back ();
      _nodeList.pop_back ();
      return gvp;
    }
  //## end Path::pop%3DCE7F9C0249.body
}

template <class T>
inline void Path<T>::push (const T *node)
{
  //## begin Path::push%3DCE7F9C025D.body preserve=yes
  _nodeList.push_back (node);
  //## end Path::push%3DCE7F9C025D.body
}

template <class T>
inline unsigned int Path<T>::getLength () const
{
  //## begin Path::getLength%3DCE7F9C027B.body preserve=yes
  return _nodeList.size ();
  //## end Path::getLength%3DCE7F9C027B.body
}

template <class T>
inline const T * Path<T>::getItem (unsigned int i) const
{
  //## begin Path::getItem%3DCE7F9C028F.body preserve=yes
  if (_nodeList.size () == 0)
	{
      return NULL;
    }
  else
    {
      return _nodeList[i];
    }
  //## end Path::getItem%3DCE7F9C028F.body
}

template <class T>
inline const T * Path<T>::getHead () const
{
  //## begin Path::getHead%3DCE7F9C02A3.body preserve=yes
  if (_nodeList.size () == 0)
    {
      return NULL;
    }
  else
    {
      return _nodeList.front ();
    }
  //## end Path::getHead%3DCE7F9C02A3.body
}

template <class T>
inline const T * Path<T>::getTail () const
{
  //## begin Path::getTail%3DCE7F9C02C1.body preserve=yes
  if (_nodeList.size () == 0)
    {
      return NULL;
    }
  else
    {
      return _nodeList.back ();
    }
  //## end Path::getTail%3DCE7F9C02C1.body
}

//## begin module%3DCE7F140063.epilog preserve=yes
//## end module%3DCE7F140063.epilog


#endif
