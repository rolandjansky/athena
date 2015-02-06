/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * @file: src/ptree_wrap.h
 * @author: Ricardo Abreu
 *
 * @brief Python bindings for boost::property_tree::ptree
 */

#ifndef PTREE_WRAP_H_
#define PTREE_WRAP_H_

namespace HLTTestApps
{
  /**
   *  Wrap the boost ptree (string values and paths) type. When this is called,
   *  a wrapped IInfoRegister is declared for Python usage, with the following
   *  public members:
   *    - a ctor with no parameters.
   *    - a ctor receiving a string.
   *    - readxml, receives a string specifying an xml file name and fills in
   *      the ptree from the contents of this file (replacing previous ptree
   *      contents).
   *    - \__str__, returns a string representation of the ptree (following the
   *      xml format). Usage examples: print my_ptree, str(my_ptree).
   *    - \__len__, returns the size of the ptree (ptree::size). Usage example:
   *      len(my_ptree).
   *    - set_data, sets the data of the root ptree node.
   *    - keys, returns a list with the keys of the first children in this
   *      ptree.
   *    - values, returns a list with the first children in this ptree
   *      (themselves ptrees).
   *    - items, returns a list of key-value pairs. The keys and values are the
   *      same as those returned in the corresponding methods.
   *    - \__iter__, returns an iterator to this ptree's keys. Usage example:
   *      for x in my_ptree: print x.
   *    - \__contains__, receives a string and returns whether or not it.
   *      corresponds to an existing path in this ptree. Usage example:
   *      x in my_ptree.
   *    - data, returns a copy of the data contained in the root node of this
   *      ptree.
   *    - \__getitem__, receives a string path and returns a copy of the data
   *      contained in the corresponding node. Usage example: my_ptree['a.b.c'].
   *    - \__setitem__, receives a string path and a string value. Sets the data
   *      of node at the specified path to the specified value. Returns a
   *      reference to the ptree at the specified path. Usage examples:
   *      my_ptree['a.b.c] = 'abc'. Notice the returned reference allows
   *      modifying the ptree - for instance, after the following Python code,
   *      my_ptree['x.y.z'] will contain the data '_'
   *      pt.__setitem__('x.y.z', 'xyz').set_data('_').
   *    - add, receives a string path and a string value and adds a ptree node
   *      with the value data at the specified path. Returns a reference to the
   *      new ptree node (which can be modified through it). Notice the path can
   *      be repeated. For instance, after doing my_pree.add('a','a1') and
   *      my_ptree.add('a','a2'), my_ptree will have two new children, both at
   *      key 'a' but with different values.
   *    - add_child, receives a string path and a ptree and adds the ptree at
   *      the specified path. Returns a reference to the newly inserted ptree
   *      which can be modified through it. Notice the path can be repeated,
   *      just like with add.
   *    - put_child, receives a string path and a ptree and puts the ptree at
   *      the specified path. If there was already one ptree at this path, it is
   *      replaced. If there were more than one ptree with this path, one of it
   *      is replaced (which one is unspecified).
   *    - get_child, receives a string path and returns an internal reference
   *      to the ptree at the specified path. If no ptree exists at that path,
   *      a KeyError is raised.
   *
   *  For more information refer to the boost::ptree directly.
   *
   */
  void wrap_ptree();
}


#endif /* PTREE_WRAP_H_ */
