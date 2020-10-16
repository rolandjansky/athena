/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "EventContainers/IdentifiableContainerMT.h"
#include <vector>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <thread>
#include <atomic>
#include <chrono>
#include <random>

namespace IDC_TEST
{
class MyCollection;
}

namespace IDC_TEST
{
constexpr int nthreads=10;
constexpr int ndigits = 100;
constexpr int Ncontainers = 5000;
constexpr int Nevents = 50;

std::mutex abortedlock;


class MyID
{
public:
    MyID(IdentifierHash id=0): m_id(id) {}
    IdentifierHash id()const {
        return m_id;
    }
private:
    IdentifierHash m_id;
};


class MyDigit
{

public:
    MyDigit(int d) :m_digit(d) {
        s_total++;
    }
    int val() const {
        return m_digit ;
    }
    ~MyDigit() {
        s_total--;
    }
    static std::atomic<int> s_total;
private:
    int m_digit;
};
std::atomic<int> MyDigit::s_total;
static const CLID CLID_MYCOLLECTION=10000;

class MyCollection
{
public:

    typedef MyDigit DIGIT;

    typedef std::vector<DIGIT*>::const_iterator const_iterator;

    MyCollection( ) :m_id(0) {
        return;
    }
    MyCollection(const MyID& id ) {
        m_id=id;
        return;
    }
    ~MyCollection()         {
        std::vector<DIGIT*>::const_iterator it = m_vector.begin();
        std::vector<DIGIT*>::const_iterator it_end = m_vector.end();
        for(; it!=it_end; ++it)
            delete (*it);
        return ;
    }

    unsigned int   identifyHash() const  {
        return m_id.id();
    }

    const_iterator begin() const {
        return m_vector.begin();
    }
    const_iterator end()  const {
        return m_vector.end()  ;
    }

    void add (DIGIT* d) {
        m_vector.push_back(d);
    }
    DIGIT* at(size_t i) {
        return m_vector.at(i);
    }
    static const CLID& classID() {
        return CLID_MYCOLLECTION;
    }

private:
    std::vector<DIGIT*> m_vector;
    MyID m_id;

};

class MyCollectionContainer
    :public   IdentifiableContainerMT<MyCollection>
{
public:
    typedef IdentifiableContainerMT<MyCollection>  MyType;

    // constructor
    MyCollectionContainer( int m ) :
        IdentifiableContainerMT<MyCollection>(m)   {

    }

    MyCollectionContainer( EventContainers::IdentifiableCache<MyCollection> *m ) :
        IdentifiableContainerMT<MyCollection>(m)   {

    }

};

struct counters {
    int deletedcount=0;
    int cachehit=0;
    int fills =0;
    int reads =0;
    int itreads =0;
    int aborted =0;
    void Add(const counters &rh) {
        deletedcount+= rh.deletedcount;
        cachehit+= rh.cachehit;
        fills+= rh.fills;
        reads+= rh.reads;
        itreads+= rh.itreads;
        aborted += rh.aborted;
    }
    void Print() const {
        std::cout << "deleted " << deletedcount << '\n';
        std::cout << "countHit " << cachehit << '\n';
        std::cout << "fills " << fills << '\n';
        std::cout << "reads " << reads << '\n';
        std::cout << "itreads " << itreads << '\n';
        std::cout << "aborted " << aborted << '\n';
    }
    counters() {
        deletedcount=0;
        cachehit=0;
        fills =0;
        reads =0;
        itreads=0;
        aborted=0;
    }
};

class PseudoView {
public:
    EventContainers::IdentifiableCache<MyCollection> *IDC;
    size_t RoIStart;
    size_t RoIEnd;
    int threads;
    counters c;
    const std::set<size_t>& m_abortedhashes;
    const std::vector<IDC_TEST::MyCollection*>& m_initialdata;

    void Check(MyCollectionContainer &container) {
        //Collections filled
        c.aborted=0;

        for(auto x : m_abortedhashes) {
            c.aborted+= x >=RoIStart && x<RoIEnd;
        }

        int wrong = 0;
        auto hashes = container.GetAllCurrentHashes();
        auto countall = container.numberOfCollections();
        if(hashes.size() != countall){
            std::cout << "Counts don't match" << std::endl;
            std::abort();
        }
        if(hashes.size()!=(size_t) 1000-c.aborted) {
            std::cout << "Error container is " << hashes.size() << " not " << 1000-c.aborted << std::endl;
            std::abort();
        }
        //Check random access method
        for(const auto& x : hashes) {
            auto p = container.indexFindPtr(x);
            int j =0;
            for(auto q : *p) {
                if(q->val() != (m_initialdata[x.value()]->at(j++))->val()) wrong++;
            }
            if(j!=ndigits) {
                std::cout << "n digits wrong"<<std::endl;
                std::abort();
            }
            c.reads++;
        }
        //check iterator method
        int orig=0;
        for(auto p : container){
            int j =0;
            for(auto q : *p) {
                if(q->val() != (m_initialdata[hashes[orig]]->at(j++))->val()) wrong++;
            }
            if(j!=ndigits) {
                std::cout << "n digits wrong"<<std::endl;
                std::abort();
            }
            c.itreads++;
            orig++;
        }
        int orig2=0;
        const auto& directaccess = container.GetAllHashPtrPair();
        for(const auto &[hashId, ptr] : directaccess){
            int j =0;
            if(hashes[orig2] != hashId){
                std::cout << "directaccess broke " << std::endl;
                std::abort();
            }
            for(auto q : *ptr) {
                if(q->val() != (m_initialdata[hashes[orig2]]->at(j++))->val()) {
                    std::cout << "directaccess broke " << std::endl;
                    std::abort();
                }
            }
            orig2++;
        }
        if(wrong > 0) {
            std::cout << "Thread " << threads << " found wrong data " << wrong << std::endl;
            std::abort();
        }

    }

    virtual void ExecuteFill(MyCollectionContainer &container) = 0;
    void Execute() {
        MyCollectionContainer container(IDC);
        ExecuteFill(container);
        Check(container);
    }
    PseudoView(int s, int r, EventContainers::IdentifiableCache<MyCollection>* inIDC, int i,
               const std::set<size_t>& abortedhashes,
               const std::vector<IDC_TEST::MyCollection*>& initialdata) : IDC(inIDC), RoIStart(s), RoIEnd(r),
        threads(i), c(),
        m_abortedhashes (abortedhashes),
        m_initialdata (initialdata) {}

    virtual ~PseudoView() = default;
};

class PseudoViewNoLock : public PseudoView {
public:

    PseudoViewNoLock(int s, int r, EventContainers::IdentifiableCache<MyCollection>* inIDC, int i,
                     const std::set<size_t>& abortedhashes,
                     const std::vector<IDC_TEST::MyCollection*>& initialdata)
      : PseudoView(s, r, inIDC, i, abortedhashes, initialdata ) { }

    virtual void ExecuteFill(MyCollectionContainer &container) override {

        for(size_t i =RoIStart ; i < RoIEnd; i++) {
            c.fills++;
            bool cacheh = container.tryAddFromCache(i);
            if(cacheh) {
                c.cachehit++;
                continue;
            }
            std::unique_ptr<MyCollection> dcoll = std::make_unique<MyCollection>( MyID(i) );
            std::mt19937 gen(i); //Standard mersenne_twister_engine seeded with rd()
            std::uniform_int_distribution<> dis(-9999999, 9999999);
            for(int j=0; j<ndigits; j++) {
                dcoll->add(new MyDigit(dis(gen)));
            }
            if(m_abortedhashes.count(i)) { //testing aborting collections
                continue;
            }
//       std::this_thread::sleep_for(0.005s);
            bool deleted = false;
            StatusCode x = container.addOrDelete(std::move(dcoll), IdentifierHash(i), deleted);
            if(x.isFailure()) {
                std::cout << "failure in addOrDelete" << std::endl;
                std::abort();
            }
            if(deleted) ++c.deletedcount;
        }

    }
};

class PseudoViewLock : public PseudoView {
public:

    PseudoViewLock(int s, int r, EventContainers::IdentifiableCache<MyCollection>* inIDC, int i,
                   const std::set<size_t>& abortedhashes,
                   const std::vector<IDC_TEST::MyCollection*>& initialdata)
      : PseudoView(s, r, inIDC, i, abortedhashes, initialdata ) { }

    virtual void ExecuteFill(MyCollectionContainer &container) override {

        for(size_t i =RoIStart ; i < RoIEnd; i++) {
            c.fills++;
            MyCollectionContainer::IDC_WriteHandle lock = container.getWriteHandle(i);
            if(lock.alreadyPresent()) {
                c.cachehit++;
                continue;
            }
            std::unique_ptr<MyCollection> dcoll = std::make_unique<MyCollection>( MyID(i) );
            std::mt19937 gen(i); //Standard mersenne_twister_engine seeded with rd()
            std::uniform_int_distribution<> dis(-9999999, 9999999);
            for(int j=0; j<ndigits; j++) {
                dcoll->add(new MyDigit(dis(gen)));
            }
            if(m_abortedhashes.count(i)) { //testing aborting collections
               continue;
            }
            bool deleted = false;
            StatusCode x = lock.addOrDelete(std::move(dcoll));
            if(x.isFailure()) {
                std::cout << "failure in addOrDelete" << std::endl;
                std::abort();
            }
            if(deleted) ++c.deletedcount;

        }

    }
};


template <typename T>
class PseudoEvent {
public:
    std::vector< T > m_views;
    EventContainers::IdentifiableCache<MyCollection> *IDCache;

    void Execute(const std::set<size_t>& abortedhashes,
                 const std::vector<IDC_TEST::MyCollection*>& initialdata) {
        int x = 0;
        IDCache = new EventContainers::IdentifiableCache<MyCollection>(IdentifierHash(Ncontainers), nullptr);
        for(int i=0; i<nthreads; i++) {
           m_views.emplace_back(x, x+1000,IDCache, i, abortedhashes, initialdata);
            x+=50;
        }
        std::vector<std::thread> threads;
        threads.reserve(nthreads);
        for(int i=0; i<nthreads; i++) threads.emplace_back(&PseudoView::Execute, &m_views[i]);
        for(int i=0; i<nthreads; i++) threads[i].join();

        delete IDCache;
        IDCache =nullptr;
    }

    counters getcounters() const {
        if(m_views.empty()) {
            std::cout << "warning views are empty\n";
        }
        counters x;
        for(auto &i : m_views) x.Add(i.c);
        return x;
    }

};



}

int main() {
    using namespace IDC_TEST;
    int x =0;
    int highestvalue =0;
    for(int i=0; i<nthreads; i++) {
        std::cout << "range " << x << " to " << x + 1000 << std::endl;
        x+=50;
        highestvalue = std::max(highestvalue, x+1000);
    }

    std::mt19937 genabort(0);
    std::uniform_int_distribution<> abort(0, highestvalue);
    std::set<size_t> abortedhashes;
    for(int i =0; i<20; i++) {
       abortedhashes.insert(abort(genabort));
    }

    std::vector<IDC_TEST::MyCollection*> initialdata;

    MyDigit::s_total=0;
    for(int i =0; i<Ncontainers; i++) {
        std::mt19937 gen(i); //Standard mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution<> dis(-9999999, 9999999);

        MyCollection* dcoll = new MyCollection(MyID(i));
        for(int j=0; j<ndigits; j++) {
            dcoll->add(new MyDigit(dis(gen)));
        }
        initialdata.push_back(dcoll);
    }

    auto start1 = std::chrono::steady_clock::now();
    {

        counters c;
        for(int i =0; i<Nevents; i++) {
            if(i%10==0) std::cout << i << "/" << Nevents << std::endl;
            PseudoEvent<PseudoViewNoLock> event;
            event.Execute(abortedhashes, initialdata);
            c.Add(event.getcounters());
        }
        std::cout << "NoLock\n";
        c.Print();
        if(c.fills!=c.reads+c.aborted || c.fills!=c.itreads+c.aborted ) {
            std::cout << "Fills do not equal reads " << std::endl;
            std::abort();
        }
    }
    auto end1 = std::chrono::steady_clock::now();
    {
        counters c;

        for(int i =0; i<Nevents; i++) {
            if(i%10==0) std::cout << i << "/" << Nevents << std::endl;
            PseudoEvent<PseudoViewLock> event;
            event.Execute(abortedhashes, initialdata);
            c.Add(event.getcounters());
        }
        std::cout << "Lock\n";
        c.Print();
        if(c.fills!=c.reads+c.aborted || c.fills!=c.itreads+c.aborted ) {
            std::cout << "Fills do not equal reads " << std::endl;
            std::abort();
        }
    }
    auto end2 = std::chrono::steady_clock::now();
    for(auto &x : initialdata) delete x;
    initialdata.clear();
    std::cout << "Total elements " << Ncontainers * ndigits << '\n';
    std::cout << "MyDigits left undeleted " << MyDigit::s_total << std::endl;
    std::chrono::duration<double> elapsed_nolock = end1-start1;
    std::chrono::duration<double> elapsed_lock = end2-end1;
    std::cout << "no lock time " << elapsed_nolock.count() << std::endl;
    std::cout << "lock time " << elapsed_lock.count() << std::endl;
    if(MyDigit::s_total >0) {
        std::cout << "not all objects were cleaned up " << std::endl;
        std::abort();
    }
    return 0;
}


