/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//! This is a node class.
/**
 * Author: Noel Dawe (Noel%dot%Dawe%at%cern%dot%ch)
 */

#include <iostream>
using namespace std;

#ifndef NODE_H
#define NODE_H

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
            if (leftChild != 0) delete leftChild;
            leftChild = child;
        }

        /**
         * @brief Set right child of this node.
         * @param child a @c Node pointer to the new right child.
         */
        void setRightChild(Node* child)
        {
            if (rightChild != 0) delete rightChild;
            rightChild = child;
        }

        /**
         * @brief Returns a pointer to the left child node.
         * @return a @c Node pointer to the left child node.
         */
        Node* getLeftChild() const { return this->leftChild; }

        /**
         * @brief Returns a pointer to the right child node.
         * @return a @c Node pointer to the right child node.
         */
        Node* getRightChild() const { return this->rightChild; }

        /**
         * @brief Returns true if node has no children and false otherwise.
         * @return true if node has no children and false otherwise.
         */
        bool isLeaf() const { return !this->rightChild && !this->leftChild; }

        /**
         * @brief Returns true if both children exist.
         * @return true if both children exist.
         */
        bool isComplete() const { return this->rightChild && this->leftChild; }

        //virtual void setLeftChild(Node* node) =0;
        //virtual void setRightChild(Node* node) =0;
        virtual bool goRight(void) const =0;

    protected:

        Node* leftChild;
        Node* rightChild;
};

template <class T>
class UnivariateCut: public DecisionNode
{
    public:

        //!< Constructor
        UnivariateCut(const T* _feature, T _cut):
            feature(_feature),
            cut(_cut)
        {
            this->leftChild = 0;
            this->rightChild = 0;
        }

        //!< Copy Constructor
        UnivariateCut(const UnivariateCut<T>& other)
        {
            this->feature = other.feature;
            this->cut = other.cut;
            this->leftChild = other.leftChild ? other.leftChild->clone() : 0;
            this->rightChild = other.rightChild ? other.rightChild->clone() : 0;
        }

        //!< Destructor
        ~UnivariateCut()
        {
            delete this->rightChild;
            delete this->leftChild;
        }

        Node* clone() const
        {
            UnivariateCut<T>* node = new UnivariateCut<T>(this->feature,this->cut);
            if (this->leftChild) node->setLeftChild(this->leftChild->clone());
            if (this->rightChild) node->setRightChild(this->rightChild->clone());
            return node;
        }

        bool goRight() const { return this->feature ? *this->feature > this->cut : false; }

        const T* getFeature() const { return this->feature; }

        T getValue() const { return this->feature ? *this->feature : -9999.; }

        T getCut() const { return this->cut; }

    private:

        const T* feature;   //!< Pointer to the variable which is cut on at this node.
        T cut;              //!< The cut.
};

template <class T, class U, class V>
class UnivariateSlidingCut1D: public DecisionNode
{
    public:

        //!< Constructor
        UnivariateSlidingCut1D(const T* _feature, U* _function, const V* _variable):
            feature(_feature),
            function(_function),
            variable(_variable)
        {
            this->leftChild = 0;
            this->rightChild = 0;
        }

        //!< Copy Constructor
        UnivariateSlidingCut1D(const UnivariateSlidingCut1D<T,U,V>& other)
        {
            this->feature = other.feature;
            this->function = other.function->Clone();
            this->variable = other.variable;
            this->leftChild = other.leftChild ? other.leftChild->clone() : 0;
            this->rightChild = other.rightChild ? other.rightChild->clone() : 0;
        }

        //!< Destructor
        ~UnivariateSlidingCut1D()
        {
            delete this->function;
            delete this->rightChild;
            delete this->leftChild;
        }

        Node* clone() const
        {
            UnivariateSlidingCut1D<T,U,V>* node = new UnivariateSlidingCut1D<T,U,V>(this->feature,(U*)this->function->Clone(),this->variable);
            if (this->leftChild) node->setLeftChild(this->leftChild->clone());
            if (this->rightChild) node->setRightChild(this->rightChild->clone());
            return node;
        }

        bool goRight() const
        {
            return this->feature && this->variable ? *this->feature > this->function->Eval(float(*this->variable)) : false;
        }

    private:

        const T* feature;
        U* function;
        const V* variable;
};

template <class T, class U, class V, class W>
class MultivariateCut2D: public DecisionNode
{
    public:

        //!< Constructor
        MultivariateCut2D(T* _function, const U* _x, const V* _y, W _cut):
            function(_function),
            x(_x),
            y(_y),
            cut(_cut)
        {
            this->leftChild = 0;
            this->rightChild = 0;
        }

        //!< Copy Constructor
        MultivariateCut2D(const MultivariateCut2D<T,U,V,W>& other)
        {
            this->function = other.function->Clone();
            this->x = other.x;
            this->y = other.y;
            this->cut = other.cut;
            this->leftChild = other.leftChild ? other.leftChild->clone() : 0;
            this->rightChild = other.rightChild ? other.rightChild->clone() : 0;
        }

        //!< Destructor
        ~MultivariateCut2D()
        {
            delete this->function;
            delete this->rightChild;
            delete this->leftChild;
        }

        Node* clone() const
        {
            MultivariateCut2D<T,U,V,W>* node = new MultivariateCut2D<T,U,V,W>((T*)this->function->Clone(),this->x,this->y,this->cut);
            if (this->leftChild) node->setLeftChild(this->leftChild->clone());
            if (this->rightChild) node->setRightChild(this->rightChild->clone());
            return node;
        }

        bool goRight() const
        {
            return this->function && this->x && this->y ? this->function->Eval(float(*this->x),float(*this->y)) > cut : false;
        }

    private:

        T* function;
        const U* x;
        const V* y;
        W cut;
};


template <class T>
class LeafNode: public Node
{
    public:

        LeafNode(T _value = 0):value(_value){}

        LeafNode(const LeafNode<T>& other) { this->value = other.value; }

        ~LeafNode(){}

        Node* clone() const { return new LeafNode<T>(this->value); }

        bool isLeaf() const { return true; }

        Node* getLeftChild() const { return 0; }

        Node* getRightChild() const { return 0; }

        bool isComplete() const { return true; }

        virtual T getValue() const { return this->value; }

        void setValue(T _value) { this->value = _value; }

    private:

        T value;
};

template <class X, class Y, class G>
class TransformationNode: public LeafNode<float>
{
    public:

        TransformationNode(const X* _x, float _xlow, float _xhigh, const Y* _y, float _ylow, float _yhigh, G* _transform):
        x(_x),
        xlow(_xlow),
        xhigh(_xhigh),
        y(_y),
        ylow(_ylow),
        yhigh(_yhigh),
        transform(_transform)
        {}

        TransformationNode(const TransformationNode<X,Y,G>& other)
        {
            this->x = other.x;
            this->xlow = other.xlow;
            this->xhigh = other.xhigh;
            this->y = other.y;
            this->ylow = other.ylow;
            this->yhigh = other.yhigh;
            this->transform = other.transform->Clone();
        }

        ~TransformationNode()
        {
            //delete transform;
        }

        Node* clone() const
        {
            return new TransformationNode<X,Y,G>(x,xlow,xhigh,y,ylow,yhigh,(G*)transform->Clone());
        }

        float getValue() const
        {
            float _x = float(*x);
            float _y = float(*y);
            if (_x < xlow)
                _x = xlow;
            else if (_x > xhigh)
                _x = xhigh;
            if (_y < ylow)
                _y = ylow;
            else if (_y > yhigh)
                _y = yhigh;
            return this->transform->Interpolate(_x, _y);
        }

    private:

        const X* x;
        float xlow;
        float xhigh;
        const Y* y;
        float ylow;
        float yhigh;
        G* transform;
};

template <class T>
class PointerLeafNode: public Node
{
    public:

        PointerLeafNode(T* _value = 0):value(_value){}

        PointerLeafNode(const PointerLeafNode<T>& other) { this->value = other.value; }

        ~PointerLeafNode() { delete this->value; }

        Node* clone() const { return new PointerLeafNode<T>(this->value); }

        bool isLeaf() const { return true; }

        Node* getLeftChild() const { return 0; }

        Node* getRightChild() const { return 0; }

        bool isComplete() const { return true; }

        T* getValue() const { return this->value; }

        void setValue(T* _value) { this->value = _value; }

    private:

        T* value;
};

#endif
