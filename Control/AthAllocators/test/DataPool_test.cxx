/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG

#include "AthAllocators/DataPool.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "TestTools/initGaudi.h"
#include "CxxUtils/checker_macros.h"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Fluff
{
	public:
		virtual ~Fluff() { }
		Fluff() : m_int(1),
			m_string("this is the Fluff struct")
		{
		}

		Fluff(const Fluff& f) = default;
                Fluff& operator= (const Fluff&) = default;

		void setPar(int j) { m_int = j; }
		int value() const { return m_int; }

	private:

		int m_int;
		string m_string;

};


void test_slots()
{
  SG::ArenaHeader& head = *SG::ArenaHeader::defaultHeader();
  SG::Arena a1 ("1");
  SG::Arena a2 ("2");
  head.setArenaForSlot (1, &a1);
  head.setArenaForSlot (2, &a2);

  {
    DataPool<Fluff> pool (EventContext (0, 1));
    Fluff* f = pool.nextElementPtr();
    f->setPar(0);
  }

  assert (head.reportStr() == "\
=== 1 ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       1/   1023/   1024      80/  49104/  49184       1/      0/      1  SG::ArenaCachingHandle<Fluff,SG::ArenaPoolAllocator>\n\
=== 2 ===\n\
=== default ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
    1500/    548/   2048   72064/  26304/  98368       2/      0/      2  SG::ArenaCachingHandle<Fluff,SG::ArenaPoolAllocator>\n");

  {
    DataPool<Fluff> pool (EventContext (0, 2));
    Fluff* f = pool.nextElementPtr();
    f->setPar(0);
  }

  assert (head.reportStr() == "\
=== 1 ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       1/   1023/   1024      80/  49104/  49184       1/      0/      1  SG::ArenaCachingHandle<Fluff,SG::ArenaPoolAllocator>\n\
=== 2 ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       1/   1023/   1024      80/  49104/  49184       1/      0/      1  SG::ArenaCachingHandle<Fluff,SG::ArenaPoolAllocator>\n\
=== default ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
    1500/    548/   2048   72064/  26304/  98368       2/      0/      2  SG::ArenaCachingHandle<Fluff,SG::ArenaPoolAllocator>\n");
}


int main ATLAS_NOT_THREAD_SAFE ()
{

	ISvcLocator* pSvcLoc;
	if (!Athena_test::initGaudi("DataPool_test.txt", pSvcLoc))
	{
		std::cerr << " This test cannot be run without init Gaudi" << endl;
	}
	assert(pSvcLoc);

	IChronoStatSvc* p_svc;
	if ((pSvcLoc->service("ChronoStatSvc", p_svc)).isSuccess())
	{
		p_svc->chronoStart("ChronoStatSvc");
	}

	std::cout << " *** DataPool test in progress: " << std::endl;

	// ask for 10 Fluff's in memory, but default is 1024!
	DataPool<Fluff>* df = new DataPool<Fluff>(10);
	DataPool<Fluff>::const_iterator iter = df->begin();
	DataPool<Fluff>::const_iterator iend = df->end();
	assert(iter == iend);		 // because pool ain't accessed yet.
	assert (0 == df->allocated());
	//  check pool capacity: default is 1024 even though we asked for 10
	assert (1024 == df->capacity());

	// Now use the first 5 of these Fluff's
	for (int j = 10; j < 15; j++)
	{
		Fluff* f = df->nextElementPtr();
		f->setPar(j);
	}
	assert (5 == df->allocated());

	// loop again and check whether the first five are set:
	int k = 10;
	DataPool<Fluff>::const_iterator iter2 = df->begin();
	DataPool<Fluff>::const_iterator iend2 = df->end();
	for (; iter2 != iend2; ++iter2)
	{
		assert(nullptr != (*iter2));
                // Note: we iterate backwards...
		assert(24-k == (*iter2)->value());
		k++;
	}

	
	df->reset();						// reset DataPool
	DataPool<Fluff>::const_iterator iter3 = df->begin();
	DataPool<Fluff>::const_iterator iend3 = df->end();
	assert (iter3 == iend3);	 // after reset
	assert (0 == df->allocated());
	assert (1024 == df->capacity());			// should be same

	// Now use 1500 of these Fluff's.. should automatically resize
	//cout << "You should see a message on automatic increase of pool size" << endl;

	for (int j = 0; j < 1500; j++)
	{
		Fluff* f = df->nextElementPtr();
		f->setPar(j);
	}

	assert(1500 == df->allocated());			// up by 2 automatically
								 
	assert(2048 == df->capacity());

	// check that resizing to less than m_refCount doesn't work
	df->reserve(1000);
	assert(2048==df->capacity());
	assert(1500==df->allocated());

	// check that resizing to less than m_maxRefCount works
	// changes related to m_maxRefCount are not visible in capacity() or allocated().
	df->reserve(1600);			

	assert(2048==df->capacity());
	assert(1500==df->allocated());

	// this is test by cheating. We reset the data pool (objects are not deleted
	// we go to the memory location and check if all values are still ok.
	df->reset();
	for (int j = 0; j < 1500; j++)
	{
		Fluff* f = static_cast<Fluff*>(df->mem());
		assert(j == f->value());
	}

	// call the pool destructor
	delete df;

        test_slots();

	cout << " **** DataPool test successfully completed **** " << endl;
	p_svc->chronoStop("ChronoStatSvc");
	p_svc->chronoPrint("ChronoStatSvc");

	return 0;
}
