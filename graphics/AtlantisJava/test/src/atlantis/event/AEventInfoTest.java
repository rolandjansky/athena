package atlantis.event;

import static org.junit.Assert.*;

import org.junit.Test;

public class AEventInfoTest {
	private static final long eventNumber=12345;
	private static final long runNumber=123;

	@Test
	public void testAEventInfoCreation() {
		AEventInfo e = new AEventInfo(eventNumber,runNumber,"TIME","SOURCE","LUMIBLOCK","EVENTPROPERTIES");
		assertAEventInfo(e,eventNumber,runNumber,"TIME","SOURCE","LUMIBLOCK","EVENTPROPERTIES");
	}

	/**
	 * Create AEventInfo object with null or other special values for parameters.
	 */
	@Test
	public void testAEventInfoCreationChecks() {
		AEventInfo e1 = new AEventInfo(eventNumber,runNumber,null,null,null,null);
		assertAEventInfo(e1,eventNumber,runNumber,"n/a","n/a","default","default");
		AEventInfo e2 = new AEventInfo(eventNumber,runNumber,null,null,"blah-1","blah-1");
		assertAEventInfo(e2,eventNumber,runNumber,"n/a","n/a","default","default");
	}

	@Test
	public void testAEventCopyConstructor() {
		AEventInfo e0 = new AEventInfo(eventNumber,runNumber,"TIME","SOURCE","LUMIBLOCK","EVENTPROPERTIES");
		AEventInfo e = new AEventInfo(e0);
		assertAEventInfo(e,eventNumber,runNumber,"TIME","SOURCE","LUMIBLOCK","EVENTPROPERTIES");
	}

	@Test
	public void testHashCode() {
		// Check different run/event numbers give different hash codes
		AEventInfo e1 = new AEventInfo(eventNumber,runNumber,"TIME","SOURCE","LUMIBLOCK","EVENTPROPERTIES");
		AEventInfo e2 = new AEventInfo(eventNumber+1,runNumber,"TIME","SOURCE","LUMIBLOCK","EVENTPROPERTIES");
		AEventInfo e3 = new AEventInfo(eventNumber,runNumber+1,"TIME","SOURCE","LUMIBLOCK","EVENTPROPERTIES");
		int h1 = e1.hashCode();
		int h2 = e2.hashCode();
		int h3 = e3.hashCode();
		assertTrue("hash codes should be different for e1,e2",h2!=h1);
		assertTrue("hash codes should be different for e1,e3",h1!=h3);
		assertTrue("hash codes should be different for e2,e3",h2!=h3);
	}

	@Test
	public void testEqualsAEventInfo() {
		AEventInfo e1a = new AEventInfo(eventNumber,runNumber,"TIME","SOURCE","LUMIBLOCK","EVENTPROPERTIES");
		AEventInfo e1b = new AEventInfo(eventNumber,runNumber,"TIME","SOURCE","LUMIBLOCK","EVENTPROPERTIES");
		AEventInfo e2 = new AEventInfo(eventNumber+1,runNumber,"TIME","SOURCE","LUMIBLOCK","EVENTPROPERTIES");
		assertTrue("equals should return true for e1a,e1b",e1a.equals(e1b));
		assertFalse("equals should return false for e1a,e2",e1a.equals(e2));
	}

	/** Check values in AEventInfo object */
	private void assertAEventInfo(AEventInfo eventInfo, long event, long run, String time, String source, String lumiBlock, String eventProperties) {
		assertEquals("event number not set correctly",event,eventInfo.getEventNumber());
		assertEquals("run number not set correctly",run,eventInfo.getRunNumber());
		assertEquals("time not set correctly",time,eventInfo.getDateTime());
		assertEquals("source not set correctly",source,eventInfo.getSourceName());
		assertEquals("source not set correctly",lumiBlock,eventInfo.getLumiBlock());
		assertEquals("event properties not set correctly",eventProperties,eventInfo.getEventProperties());
	}

}
