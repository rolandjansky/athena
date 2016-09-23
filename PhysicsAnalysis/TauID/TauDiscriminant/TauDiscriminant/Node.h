/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//! This is a node class.
/**
 * Author: Noel Dawe (Noel%dot%Dawe%at%cern%dot%ch)
 */

#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <algorithm>
 //using namespace std;

//!< Abstract base class
class Node
{
    public:

        virtual ~Node() {}
        virtual bool isLeaf(void) const =0;
        virtual bool isComplete(void) const =0;
        virtual Node* clone(void) const =0;
        virtual Node* getLeftChild(void) const =0;
        virtual Node* getRightChild(void) const =0;
};

class DecisionNode: public Node
{
    public:

        /**
         * @brief Set left child of this node.
         * @param child a @c Node pointer to the new left child.
         */
        void setLeftChild(Node* child)
        {
            if (m_leftChild != 0) delete m_leftChild;
            m_leftChild = child;
        }

        /**
         * @brief Set right child of this node.
         * @param child a @c Node pointer to the new right child.
         */
        void setRightChild(Node* child)
        {
            if (m_rightChild != 0) delete m_rightChild;
            m_rightChild = child;
        }

        /**
         * @brief Returns a pointer to the left child node.
         * @return a @c Node pointer to the left child node.
         */
        Node* getLeftChild() const { return this->m_leftChild; }

        /**
         * @brief Returns a pointer to the right child node.
         * @return a @c Node pointer to the right child node.
         */
        Node* getRightChild() const { return this->m_rightChild; }

        /**
         * @brief Returns true if node has no children and false otherwise.
         * @return true if node has no children and false otherwise.
         */
        bool isLeaf() const { return !this->m_rightChild && !this->m_leftChild; }

        /**
         * @brief Returns true if both children exist.
         * @return true if both children exist.
         */
        bool isComplete() const { return this->m_rightChild && this->m_leftChild; }

        //virtual void setLeftChild(Node* node) =0;
        //virtual void setRightChild(Node* node) =0;
        virtual bool goRight(void) const =0;

    protected:

        Node* m_leftChild;
        Node* m_rightChild;
};

template <class T>
class UnivariateCut: public DecisionNode
{
    public:

        //!< Constructor
        UnivariateCut(const T* feature, T cut):
            m_feature(feature),
            m_cut(cut)
        {
            this->m_leftChild = 0;
            this->m_rightChild = 0;
        }

        //!< Copy Constructor
        UnivariateCut(const UnivariateCut<T>& other)
        {
            this->m_feature = other.m_feature;
            this->m_cut = other.m_cut;
            this->m_leftChild = other.m_leftChild ? other.m_leftChild->clone() : 0;
            this->m_rightChild = other.m_rightChild ? other.m_rightChild->clone() : 0;
        }

	UnivariateCut<T>& operator=(const UnivariateCut<T>& other)
	{
	    UnivariateCut<T> temp(other);
	    swap(*this, temp);
	    return *this;
	}

        //!< Destructor
        ~UnivariateCut()
        {
            delete this->m_rightChild;
            delete this->m_leftChild;
        }

        Node* clone() const
        {
            UnivariateCut<T>* node = new UnivariateCut<T>(this->m_feature,this->m_cut);
            if (this->m_leftChild) node->setLeftChild(this->m_leftChild->clone());
            if (this->m_rightChild) node->setRightChild(this->m_rightChild->clone());
            return node;
        }

        bool goRight() const { return this->m_feature ? *this->m_feature > this->m_cut : false; }

        const T* getFeature() const { return this->m_feature; }

        T getValue() const { return this->m_feature ? *this->m_feature : -9999.; }

        T getCut() const { return this->m_cut; }

    private:

        const T* m_feature;   //!< Pointer to the variable which is cut on at this node.
        T m_cut;              //!< The cut.
};

template <class T, class U, class V>
class UnivariateSlidingCut1D: public DecisionNode
{
    public:

        //!< Constructor
        UnivariateSlidingCut1D(const T* feature, U* function, const V* variable):
            m_feature(feature),
            m_function(function),
            m_variable(variable)
        {
            this->m_leftChild = 0;
            this->m_rightChild = 0;
        }

        //!< Copy Constructor
        UnivariateSlidingCut1D(const UnivariateSlidingCut1D<T,U,V>& other)
        {
            this->m_feature = other.m_feature;
            this->m_function = other.m_function->Clone();
            this->m_variable = other.m_variable;
            this->m_leftChild = other.m_leftChild ? other.m_leftChild->clone() : 0;
            this->m_rightChild = other.m_rightChild ? other.m_rightChild->clone() : 0;
        }

	UnivariateSlidingCut1D<T,U,V>& operator=(const UnivariateSlidingCut1D<T,U,V>& other)
	{
	    UnivariateSlidingCut1D<T,U,V> temp(other);
	    swap(*this, temp);
	    return *this;
	}

        //!< Destructor
        ~UnivariateSlidingCut1D()
        {
            delete this->m_function;
            delete this->m_rightChild;
            delete this->m_leftChild;
        }

        Node* clone() const
        {
            UnivariateSlidingCut1D<T,U,V>* node = new UnivariateSlidingCut1D<T,U,V>(this->m_feature,(U*)this->m_function->Clone(),this->m_variable);
            if (this->m_leftChild) node->setLeftChild(this->m_leftChild->clone());
            if (this->m_rightChild) node->setRightChild(this->m_rightChild->clone());
            return node;
        }

        bool goRight() const
        {
            return this->m_feature && this->m_variable ? *this->m_feature > this->m_function->Eval(float(*this->m_variable)) : false;
        }

    private:

        const T* m_feature;
        U* m_function;
        const V* m_variable;
};

template <class T, class U, class V, class W>
class MultivariateCut2D: public DecisionNode
{
    public:

        //!< Constructor
        MultivariateCut2D(T* function, const U* x, const V* y, W cut):
            m_function(function),
            m_x(x),
            m_y(y),
            m_cut(cut)
        {
            this->m_leftChild = 0;
            this->m_rightChild = 0;
        }

        //!< Copy Constructor
        MultivariateCut2D(const MultivariateCut2D<T,U,V,W>& other)
        {
            this->m_function = other.m_function->Clone();
            this->m_x = other.m_x;
            this->m_y = other.m_y;
            this->m_cut = other.m_cut;
            this->m_leftChild = other.m_leftChild ? other.m_leftChild->clone() : 0;
            this->m_rightChild = other.m_rightChild ? other.m_rightChild->clone() : 0;
        }

	MultivariateCut2D<T,U,V,W>& operator=(MultivariateCut2D<T,U,V,W> other)
	{
	    swap(*this, other);
	    return *this;
	}

        //!< Destructor
        ~MultivariateCut2D()
        {
            delete this->m_function;
            delete this->m_rightChild;
            delete this->m_leftChild;
        }

        Node* clone() const
        {
            MultivariateCut2D<T,U,V,W>* node = new MultivariateCut2D<T,U,V,W>((T*)this->m_function->Clone(),this->m_x,this->m_y,this->m_cut);
            if (this->m_leftChild) node->setLeftChild(this->m_leftChild->clone());
            if (this->m_rightChild) node->setRightChild(this->m_rightChild->clone());
            return node;
        }

        bool goRight() const
        {
            return this->m_function && this->m_x && this->m_y ? this->m_function->Eval(float(*this->m_x),float(*this->m_y)) > m_cut : false;
        }

    private:

        T* m_function;
        const U* m_x;
        const V* m_y;
        W m_cut;
};


template <class T>
class LeafNode: public Node
{
    public:

        LeafNode(T value = 0):m_value(value){}

        LeafNode(const LeafNode<T>& other) { this->m_value = other.m_value; }

	LeafNode<T>& operator=(const LeafNode<T>& other)
	{
	    LeafNode<T> temp(other);
	    swap(*this, temp);
	    return *this;
	}
	
        ~LeafNode(){}

        Node* clone() const { return new LeafNode<T>(this->m_value); }

        bool isLeaf() const { return true; }

        Node* getLeftChild() const { return 0; }

        Node* getRightChild() const { return 0; }

        bool isComplete() const { return true; }

        virtual T getValue() const { return this->m_value; }

        void setValue(T value) { this->m_value = value; }

    private:

        T m_value;
};

template <class X, class Y, class G>
class TransformationNode: public LeafNode<float>
{
    public:

        TransformationNode(const X* x, float xlow, float xhigh, const Y* y, float ylow, float yhigh, G* transform):
        m_x(x),
        m_xlow(xlow),
        m_xhigh(xhigh),
        m_y(y),
        m_ylow(ylow),
        m_yhigh(yhigh),
        m_transform(transform)
        {}

        TransformationNode(const TransformationNode<X,Y,G>& other)
        {
            this->m_x = other.m_x;
            this->m_xlow = other.m_xlow;
            this->m_xhigh = other.m_xhigh;
            this->m_y = other.m_y;
            this->m_ylow = other.m_ylow;
            this->m_yhigh = other.m_yhigh;
            this->m_transform = other.m_transform->Clone();
        }

	TransformationNode<X,Y,G>& operator=(const TransformationNode<X,Y,G>& other)
	{
	    TransformationNode<X,Y,G> temp(other);
	    swap(*this, temp);
	    return *this;
	}

	
        ~TransformationNode()
        {
            //delete m_transform;
        }

        Node* clone() const
        {
            return new TransformationNode<X,Y,G>(m_x,m_xlow,m_xhigh,m_y,m_ylow,m_yhigh,(G*)m_transform->Clone());
        }

        float getValue() const
        {
            float x = float(*m_x);
            float y = float(*m_y);
            if (x < m_xlow)
                x = m_xlow;
            else if (x > m_xhigh)
                x = m_xhigh;
            if (y < m_ylow)
                y = m_ylow;
            else if (y > m_yhigh)
                y = m_yhigh;
            return this->m_transform->Interpolate(x, y);
        }

    private:

        const X* m_x;
        float m_xlow;
        float m_xhigh;
        const Y* m_y;
        float m_ylow;
        float m_yhigh;
        G* m_transform;
};

template <class T>
class PointerLeafNode: public Node
{
    public:

        PointerLeafNode(T* value = 0):m_value(value){}

        PointerLeafNode(const PointerLeafNode<T>& other) { this->m_value = other.m_value; }

	PointerLeafNode<T>& operator=(const PointerLeafNode<T>& other)
	{
	    PointerLeafNode<T> temp(other);
	    swap(*this, temp);
	    return *this;
	}
	
        ~PointerLeafNode() { delete this->m_value; }

        Node* clone() const { return new PointerLeafNode<T>(this->m_value); }

        bool isLeaf() const { return true; }

        Node* getLeftChild() const { return 0; }

        Node* getRightChild() const { return 0; }

        bool isComplete() const { return true; }

        T* getValue() const { return this->m_value; }

        void setValue(T* value) { this->m_value = value; }

    private:

        T* m_value;
};

template <class T, class U, class V, class W>
  class MultivariateSlidingCut2D: public DecisionNode
{
 public:

  //!< Constructor
 MultivariateSlidingCut2D(const T* feature, U* graph2d, const V* x, const W* y):
  m_feature(feature),
    m_graph2d(graph2d),
    m_x(x),
    m_y(y)
    {
      this->m_leftChild = 0;
      this->m_rightChild = 0;
    }

  //!< Copy Constructor
  MultivariateSlidingCut2D(const MultivariateSlidingCut2D<T,U,V,W>& other)
    {
      this->m_feature = other.m_feature;
      this->m_graph2d = other.m_graph2d->Clone();
      this->m_x = other.m_x;
      this->m_y = other.m_y;
      this->leftChild = other.leftChild ? other.leftChild->clone() : 0;
      this->rightChild = other.rightChild ? other.rightChild->clone() : 0;
    }

  MultivariateSlidingCut2D<T,U,V,W>& operator=(const MultivariateSlidingCut2D<T,U,V,W>& other)
    {
      MultivariateSlidingCut2D<T,U,V,W> temp(other);
      swap(*this, temp);
      return *this;
    }

  //!< Destructor
  ~MultivariateSlidingCut2D()
    {
      delete this->m_graph2d;
      delete this->m_rightChild;
      delete this->m_leftChild;
    }

  Node* clone() const
  {
    MultivariateSlidingCut2D<T,U,V,W>* node = new MultivariateSlidingCut2D<T,U,V,W>(this->m_feature, (U*)this->m_graph2d->Clone(), this->m_x, this->m_y);
    if (this->m_leftChild) node->setLeftChild(this->m_leftChild->clone());
    if (this->m_rightChild) node->setRightChild(this->m_rightChild->clone());
    return node;
  }
  
  bool goRight() const
  {
    if(! this->m_feature || ! this->m_x || ! this->m_y || ! this->m_graph2d) return false;
    double x = std::min(std::max(static_cast<double>(*this->m_x), this->m_graph2d->GetXmin()), this->m_graph2d->GetXmax());
    double y = std::min(std::max(static_cast<double>(*this->m_y), this->m_graph2d->GetYmin()), this->m_graph2d->GetYmax());
    return *this->m_feature > this->m_graph2d->Interpolate(x, y);
  }

 private:

  const T* m_feature;
  U* m_graph2d;
  const V* m_x;
  const W* m_y;
};



#endif
